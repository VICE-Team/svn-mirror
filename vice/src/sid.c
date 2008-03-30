/*
 * sid.c - MOS6581 (SID) emulation.
 *
 * Written by
 *  Teemu Rantanen (tvr@cs.hut.fi)
 *  Michael Schwendt (sidplay@geocities.com)
 *
 * AIX support added by
 *  Chris Sharp (sharpc@hursley.ibm.com)
 *
 * NetBSD patch by
 *  Krister Walfridsson (cato@df.lth.se)
 *
 * This file is part of VICE, the Versatile Commodore Emulator.
 * See README for copyright notice.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *  02111-1307  USA.
 *
 */

#include "vice.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <math.h>

#if defined(HAVE_SYS_IOCTL_H) && !defined(__MSDOS__)
#ifdef __hpux
#define _INCLUDE_HPUX_SOURCE
#endif
#include <sys/ioctl.h>
#endif

#ifndef HAVE_USLEEP
extern int usleep(unsigned long);
#endif

#include "vmachine.h"
#include "types.h"
#include "mem.h"
#include "ui.h"
#include "interrupt.h"
#include "warn.h"
#include "sid.h"
#include "vsync.h"
#include "resources.h"
#include "utils.h"

#ifdef SOUND

/* needed datatypes */
typedef unsigned int u32_t;
typedef int s32_t;
typedef unsigned short u16_t;
typedef short s16_t;
typedef unsigned char u8_t;

#ifdef SID

#ifdef CBM64
/* use wavetables (sampled waveforms) */
#define WAVETABLES
#endif

/* ADSR state */
#define ATTACK 0
#define DECAY 1
#define SUSTAIN 2
#define RELEASE 3
#define IDLE 4

#ifndef WAVETABLES
/* Current waveform */
#define TESTWAVE 0
#define PULSEWAVE 1
#define SAWTOOTHWAVE 2
#define TRIANGLEWAVE 3
#define NOISEWAVE 4
#define NOWAVE 5
#define RINGWAVE 6
#define PULSETRIANGLEWAVE 7
#define PULSESAWTOOTHWAVE 8
#endif

/* noise magic */
#define NSHIFT(v, n) (((v)<<(n))|((((v)>>(23-(n)))^(v>>(18-(n))))&((1<<(n))-1)))
#define NVALUE(v) (noiseLSB[v&0xff]|noiseMID[(v>>8)&0xff]|noiseMSB[(v>>16)&0xff])
#define NSEED 0x7ffff8

#ifdef WAVETABLES

#include "wave6581.h"
#include "wave8580.h"

static u16_t wavetable00[2];
static u16_t wavetable10[4096];
static u16_t wavetable20[4096];
static u16_t wavetable30[4096];
static u16_t wavetable40[8192];
static u16_t wavetable50[8192];
static u16_t wavetable60[8192];
static u16_t wavetable70[8192];

#endif

/* Noise tables */
#define NOISETABLESIZE 256
static u8_t noiseMSB[NOISETABLESIZE];
static u8_t noiseMID[NOISETABLESIZE];
static u8_t noiseLSB[NOISETABLESIZE];

/* needed data for one voice */
typedef struct voice_s
{
    struct sound_s	*s;
    struct voice_s	*vprev;
    struct voice_s	*vnext;
    int			 nr;

    /* counter value */
    u32_t		 f;
    /* counter step / sample */
    u32_t		 fs;
#ifdef WAVETABLES
    /* do we have noise enabled? */
    BYTE		 noise;
#else
    /* waveform that we use */
    BYTE		 fm;
    /* pulse threshold compared to the 32-bit counter */
    u32_t		 pw;
#endif

    /* 31-bit adsr counter */
    u32_t		 adsr;
    /* adsr counter step / sample */
    s32_t		 adsrs;
    /* adsr sustain level compared to the 31-bit counter */
    u32_t		 adsrz;

    /* does this voice use hard sync? */
    BYTE		 sync;
    /* does this voice use filter? */
    BYTE		 filter;
    /* does this structure need updating before next sample? */
    BYTE		 update;
    /* did we do multiple gate flips after last calculated sample? */
    BYTE		 gateflip;

    /* ADSR mode */
    BYTE		 adsrm;
    /* 4-bit attack value */
    BYTE		 attack;
    /* 4-bit decay value */
    BYTE		 decay;
    /* 4-bit sustain value */
    BYTE		 sustain;
    /* 4-bit release value */
    BYTE		 release;

    /* pointer to registers of this voice */
    BYTE		*d;

    /* noise shift register. Note! rv may be 0 to 15 shifts 'behind' the
       real noise shift register value. Remaining shifts are done when
       it is referenced */
    u32_t		 rv;
#ifdef WAVETABLES
    /* pointer to wavetable data */
    u16_t		*wt;
    /* 32-bit offset to add to the counter before referencing the wavetable.
       This is used on combined waveforms, when other waveforms are combined
       with pulse */
    u32_t		 wtpf;
    /* length of wavetable (actually number of shifts needed for 32-bit
       counter) */
    u32_t		 wtl;
    /* kludge for ring modulation. Set wtr[1] = 0x7fff if ring modulation is
       used */
    u16_t		 wtr[2];
#endif

    signed char		filtIO;
    float		filtLow, filtRef;
} voice_t;

/* needed data for SID */
typedef struct sound_s
{
    /* number of voices (4 because of VIC20) */
    voice_t		 v[4];
    /* SID registers */
    BYTE		 d[64];
    /* is voice 3 enabled? */
    BYTE		 has3;
    /* 4-bit volume value */
    BYTE		 vol;
    /* pointer to the sample data that is generated when SID runs */
    s16_t		*pbuf;
    /* offset of next sample being generated */
    s32_t		 bufptr;

    /* ADSR counter step values for each adsr values */
    s32_t		 adrs[16];
    /* sustain values compared to 31-bit ADSR counter */
    u32_t		 sz[16];

    /* internal constant used for sample rate dependent calculations */
    u32_t		 speed1;

    /* warnings */
    warn_t		*pwarn;

    /* does this structure need updating before next sample? */
    BYTE		 update;
#ifdef WAVETABLES
    /* do we have a new sid or an old one? */
    BYTE		 newsid;
#endif
    /* constants needed to implement write-only register reads */
    BYTE		 laststore;
    BYTE		 laststorebit;
    CLOCK		 laststoreclk;
    /* do we want to use filters? */
    BYTE		 emulatefilter;
} sound_t;

/* XXX: check these */
/* table for internal ADSR counter step calculations */
static u16_t adrtable[16] =
{
    1, 4, 8, 12, 19, 28, 34, 40, 50, 125, 250, 400, 500, 1500, 2500, 4000
};

/* XXX: check these */
/* table for pseudo-exponential ADSR calculations */
static u32_t exptable[6] =
{
    0x30000000, 0x1c000000, 0x0e000000, 0x08000000, 0x04000000, 0x00000000
};

/* clockcycles for each dropping bit when write-only register read is done */
static u32_t sidreadclocks[9];

static float lowPassParam[0x800];
#define filterTable lowPassParam
static float bandPassParam[0x800];
static float filterResTable[16];
static float filterDy, filterResDy;
static BYTE filterType = 0;
static BYTE filterCurType = 0;
static u16_t filterValue;
static const float filterRefFreq = 44100.0;
static signed char ampMod1x8[256];

inline static void dofilter(voice_t *pVoice)
{
    float sample, sample2;
    int tmp;

    if ( pVoice->filter )
    {
	if ( filterType != 0 )
	{
	    if ( filterType == 0x20 )
	    {
		pVoice->filtLow += ( pVoice->filtRef * filterDy );
		sample = pVoice->filtIO;
		sample2 = sample - pVoice->filtLow;
		sample2 -= pVoice->filtRef * filterResDy;
		pVoice->filtRef += ( sample2 * filterDy );
		pVoice->filtIO = (signed char)(pVoice->filtRef-pVoice->filtLow/4);
	    }
	    else if ( filterType == 0x40 )
	    {
		pVoice->filtLow += ( pVoice->filtRef * filterDy * 0.1 );
		sample = pVoice->filtIO;
		sample2 = sample - pVoice->filtLow;
		sample2 -= pVoice->filtRef * filterResDy;
		pVoice->filtRef += ( sample2 * filterDy );
		sample2 = pVoice->filtRef - pVoice->filtIO/8;
		if (sample2 < -128)
		    sample2 = -128;
		if (sample2 > 127)
		    sample2 = 127;
		pVoice->filtIO = (signed char)sample2;
	    }
	    else
	    {
		pVoice->filtLow += ( pVoice->filtRef * filterDy );
		sample = pVoice->filtIO;
		sample2 = sample - pVoice->filtLow;
		tmp = (int)sample2;
		sample2 -= pVoice->filtRef * filterResDy;
		pVoice->filtRef += ( sample2 * filterDy );

		if ( filterType == 0x10 )
		    pVoice->filtIO = (signed char)pVoice->filtLow;
		else if ( filterType == 0x30 )
		    pVoice->filtIO = (signed char)pVoice->filtLow;
		else if ( filterType == 0x50 )
		    pVoice->filtIO = (signed char)(sample - (tmp >> 1));
		else if ( filterType == 0x60 )
		    pVoice->filtIO = (signed char)tmp;
		else if ( filterType == 0x70 )
		    pVoice->filtIO = (signed char)(sample - (tmp >> 1));
	    }
	}
	else /* filterType == 0x00 */
	{
	    pVoice->filtIO = 0;
	}
    }
}

/* 15-bit oscillator value */
#ifdef WAVETABLES
inline static u32_t doosc(voice_t *pv)
{
    if (pv->noise)
	return ((u32_t)NVALUE(NSHIFT(pv->rv, pv->f >> 28))) << 7;
    return pv->wt[(pv->f + pv->wtpf) >> pv->wtl] ^ pv->wtr[pv->vprev->f >> 31];
}
#else
static u32_t doosc(voice_t *pv)
{
    u32_t		f = pv->f;
    switch (pv->fm)
    {
#ifndef VIC20
    case PULSESAWTOOTHWAVE:
	if (f <= pv->pw)
	    return 0x0000;
    case SAWTOOTHWAVE:
	return f >> 17;
    case RINGWAVE:
	f ^= pv->vprev->f & 0x80000000;
    case TRIANGLEWAVE:
	if (f < 0x80000000)
	    return f >> 16;
	return 0xffff - (f >> 16);
    case PULSETRIANGLEWAVE:
	if (f <= pv->pw)
	    return 0x0000;
	if (f < 0x80000000)
	    return f >> 16;
	return 0xffff - (f >> 16);
#endif
    case NOISEWAVE:
	return ((u32_t)NVALUE(NSHIFT(pv->rv, pv->f >> 28))) << 7;
    case PULSEWAVE:
	if (f > pv->pw)
	    return 0x7fff;
    }
    return 0x0000;
}
#endif

/* change ADSR state and all related variables */
static void set_adsr(voice_t *pv, BYTE fm)
{
    int				i;
    switch (fm)
    {
    case ATTACK:
	pv->adsrs = pv->s->adrs[pv->attack];
	pv->adsrz = 0;
	break;
    case DECAY:
        /* XXX: fix this */
	if (pv->adsr <= pv->s->sz[pv->sustain])
	{
	    set_adsr(pv, SUSTAIN);
	    return;
	}
	for (i = 0; pv->adsr < exptable[i]; i++);
	pv->adsrs = -pv->s->adrs[pv->decay] >> i;
	pv->adsrz = pv->s->sz[pv->sustain];
	if (exptable[i] > pv->adsrz)
	    pv->adsrz = exptable[i];
	break;
    case SUSTAIN:
	if (pv->adsr > pv->s->sz[pv->sustain])
	{
	    set_adsr(pv, DECAY);
	    return;
	}
	pv->adsrs = 0;
	pv->adsrz = 0;
	break;
    case RELEASE:
	if (!pv->adsr)
	{
	    set_adsr(pv, IDLE);
	    return;
	}
	for (i = 0; pv->adsr < exptable[i]; i++);
	pv->adsrs = -pv->s->adrs[pv->release] >> i;
	pv->adsrz = exptable[i];
	break;
    case IDLE:
	pv->adsrs = 0;
	pv->adsrz = 0;
	break;
    }
    pv->adsrm = fm;
}

/* ADSR counter triggered state change */
static void trigger_adsr(voice_t *pv)
{
    switch (pv->adsrm)
    {
    case ATTACK:
	pv->adsr = 0x7fffffff;
	set_adsr(pv, DECAY);
	break;
    case DECAY:
    case RELEASE:
	if (pv->adsr >= 0x80000000)
	    pv->adsr = 0;
	set_adsr(pv, pv->adsrm);
	break;
    }
}

#ifdef DEBUG
static void print_voice(FILE *fd, voice_t *pv)
{
    char *m = "ADSRI";
#ifdef WAVETABLES
    char *w = "0123456789abcdef";
#else
    char *w = "TPSTN-R5";
#endif
    fprintf(fd, "#SID: V%d: e=%5.1f%%(%c) w=%6.1fHz(%c) f=%5.1f%% p=%5.1f%%\n",
	    pv->nr,
	    (double)pv->adsr*100.0 / (((u32_t)1 << 31) - 1), m[pv->adsrm],
	    (double)pv->fs / (pv->s->speed1*16),
#ifdef WAVETABLES
	    w[pv->d[4]>>4],
#else
	    w[pv->fm],
#endif
	    (double)pv->f*100.0 / ((u32_t)-1),
#ifdef WAVETABLES
	    (double)(pv->d[2] + (pv->d[3]&0x0f)*0x100)/40.95
#else
	    (double)pv->pw*100.0 / ((u32_t)-1)
#endif
	);
}

static void print_sid(FILE *fd, sound_t *psid)
{
    int			i;
    fprintf(fd, "#SID: clk=%d v=%d s3=%d\n", clk, psid->vol, psid->has3);
    for (i = 0; i < 3; i++)
	print_voice(fd, &psid->v[i]);
}
#endif

/* update SID structure */
inline static void setup_sid(sound_t *psid)
{
    if (!psid->update)
	return;
    psid->vol = psid->d[0x18] & 0x0f;
    psid->has3 = psid->d[0x18] & 0x80 ? 0 : 1;
    if (psid->emulatefilter)
    {
	psid->v[0].filter = psid->d[0x17] & 0x01 ? 1 : 0;
	psid->v[1].filter = psid->d[0x17] & 0x02 ? 1 : 0;
	psid->v[2].filter = psid->d[0x17] & 0x04 ? 1 : 0;
	filterType = psid->d[0x18]&0x70;
	if (filterType != filterCurType)
	{
	    filterCurType = filterType;
	    psid->v[0].filtLow = 0;
	    psid->v[0].filtRef = 0;
	    psid->v[1].filtLow = 0;
	    psid->v[1].filtRef = 0;
	    psid->v[2].filtLow = 0;
	    psid->v[2].filtRef = 0;
	}
	filterValue = 0x7ff&((psid->d[0x15]&7)|((u16_t)psid->d[0x16])<<3);
	if (filterType == 0x20)
	    filterDy = bandPassParam[filterValue];
	else
	    filterDy = lowPassParam[filterValue];
	filterResDy = filterResTable[psid->d[0x17]>>4]-filterDy;
	if (filterResDy < 1.0)
	    filterResDy = 1.0;
	if (psid->d[0x17] & 0x07)
	    warn(psid->pwarn, 0, "program uses filters");
    }
    else
    {
	psid->v[0].filter = 0;
	psid->v[1].filter = 0;
	psid->v[2].filter = 0;
	if (psid->d[0x17] & 0x07)
	    warn(psid->pwarn, 0, "filters are disabled");
    }
    psid->update = 0;
}

/* update voice structure */
inline static void setup_voice(voice_t *pv)
{
    if (!pv->update)
	return;
    pv->attack = pv->d[5] / 0x10;
    pv->decay = pv->d[5] & 0x0f;
    pv->sustain = pv->d[6] / 0x10;
    pv->release = pv->d[6] & 0x0f;
#ifndef WAVETABLES
    pv->pw = (pv->d[2] + (pv->d[3]&0x0f)*0x100) * 0x100100;
#endif
    pv->sync = pv->d[4] & 0x02 ? 1 : 0;
    if (pv->sync)
	warn(pv->s->pwarn, 1, "program uses hard sync");
    pv->fs = pv->s->speed1 * (pv->d[0] + pv->d[1]*0x100);
#ifdef WAVETABLES
    if (pv->d[4] & 0x08)
    {
	pv->f = pv->fs = 0;
	pv->rv = NSEED;
    }
    pv->noise = 0;
    pv->wtl = 20;
    pv->wtpf = 0;
    pv->wtr[1] = 0;
    switch ((pv->d[4] & 0xf0) >> 4)
    {
    case 0:
	pv->wt = wavetable00;
	pv->wtl = 31;
	break;
    case 1:
	pv->wt = wavetable10;
	if (pv->d[4] & 0x04)
	    pv->wtr[1] = 0x7fff;
	break;
    case 2:
	pv->wt = wavetable20;
	break;
    case 3:
	pv->wt = wavetable30;
	if (pv->d[4] & 0x04)
	    pv->wtr[1] = 0x7fff;
	warn(pv->s->pwarn, 3, "program combines waveforms");
	break;
    case 4:
	pv->wt = &wavetable40[4096 - (pv->d[2] + (pv->d[3]&0x0f)*0x100)];
	break;
    case 5:
	warn(pv->s->pwarn, 9, "program combines pulse and triangle waveforms");
	pv->wt = &wavetable50[pv->wtpf = 4096 - (pv->d[2] + (pv->d[3]&0x0f)*0x100)];
	pv->wtpf <<= 20;
	if (pv->d[4] & 0x04)
	    pv->wtr[1] = 0x7fff;
	break;
    case 6:
	warn(pv->s->pwarn, 10,
	     "program combines pulse and sawtooth waveforms");
	pv->wt = &wavetable60[pv->wtpf = 4096 - (pv->d[2] + (pv->d[3]&0x0f)*0x100)];
	pv->wtpf <<= 20;
	break;
    case 7:
	pv->wt = &wavetable70[pv->wtpf = 4096 - (pv->d[2] + (pv->d[3]&0x0f)*0x100)];
	pv->wtpf <<= 20;
	if (pv->d[4] & 0x04 && pv->s->newsid)
	    pv->wtr[1] = 0x7fff;
	warn(pv->s->pwarn, 3, "program combines waveforms");
	break;
    case 8:
	pv->noise = 1;
	pv->wt = NULL;
	pv->wtl = 0;
	break;
    default:
	/* XXX: noise locking correct? */
	pv->rv = 0;
	pv->wt = wavetable00;
	pv->wtl = 31;
    }
    if (pv->wtr[1])
	warn(pv->s->pwarn, 2, "program uses ring modulation");
#else
    if (pv->d[4] & 0x08)
    {
	pv->fm = TESTWAVE;
	pv->f = pv->fs = 0;
	pv->rv = NSEED;
    }
    else switch ((pv->d[4] & 0xf0) >> 4)
    {
    case 4:
	pv->fm = PULSEWAVE;
	break;
    case 2:
	pv->fm = SAWTOOTHWAVE;
	break;
    case 1:
	if (pv->d[4] & 0x04)
	{
	    pv->fm = RINGWAVE;
	    warn(pv->s->pwarn, 2, "program uses ring modulation");
	}
	else
	    pv->fm = TRIANGLEWAVE;
	break;
    case 8:
	pv->fm = NOISEWAVE;
	break;
    case 0:
	pv->fm = NOWAVE;
	break;
    case 5:
	pv->fm = PULSETRIANGLEWAVE;
	warn(pv->s->pwarn, 9, "program combines pulse and triangle waveforms");
	break;
    case 6:
	pv->fm = PULSESAWTOOTHWAVE;
	warn(pv->s->pwarn, 10,
	     "program combines pulse and sawtooth waveforms");
	break;
    default:
	pv->fm = NOWAVE;
	warn(pv->s->pwarn, 3, "program combines waveforms");
    }
#endif
    switch (pv->adsrm)
    {
    case ATTACK:
    case DECAY:
    case SUSTAIN:
	if (pv->d[4] & 0x01)
	    set_adsr(pv, pv->gateflip ? ATTACK : pv->adsrm);
	else
	    set_adsr(pv, RELEASE);
	break;
    case RELEASE:
    case IDLE:
	if (pv->d[4] & 0x01)
	    set_adsr(pv, ATTACK);
	else
	    set_adsr(pv, pv->adsrm);
	break;
    }
    pv->update = 0;
    pv->gateflip = 0;
}

/* calculate number of samples */
#ifdef VIC20
inline static void update_sid(sound_t *psid, int nr)
{
    int			i;
    u32_t		o0, o1, o2, o3;
    for (i = 0; i < nr; i++)
    {
	/* addfptrs */
	psid->v[0].f += psid->v[0].fs;
	psid->v[1].f += psid->v[1].fs;
	psid->v[2].f += psid->v[2].fs;
	psid->v[3].f += psid->v[3].fs;
	/* noise */
	if (psid->v[3].f < psid->v[3].fs)
	    psid->v[3].rv = NSHIFT(psid->v[3].rv, 16);
	/* do adsr */
	if ((psid->v[0].adsr += psid->v[0].adsrs) + 0x80000000 <
	    psid->v[0].adsrz + 0x80000000)
	    trigger_adsr(&psid->v[0]);
	if ((psid->v[1].adsr += psid->v[1].adsrs) + 0x80000000 <
	    psid->v[1].adsrz + 0x80000000)
	    trigger_adsr(&psid->v[1]);
	if ((psid->v[2].adsr += psid->v[2].adsrs) + 0x80000000 <
	    psid->v[2].adsrz + 0x80000000)
	    trigger_adsr(&psid->v[2]);
	if ((psid->v[3].adsr += psid->v[3].adsrs) + 0x80000000 <
	    psid->v[3].adsrz + 0x80000000)
	    trigger_adsr(&psid->v[3]);
	/* no sample? */
	if (!psid->pbuf)
	    continue;
	/* volume check */
	if (!psid->vol)
	{
	    psid->pbuf[psid->bufptr++] = 0;
	    continue;
	}
	/* oscillators */
	o0 = psid->v[0].adsr >> 16;
	o1 = psid->v[1].adsr >> 16;
	o2 = psid->v[2].adsr >> 16;
	o3 = psid->v[3].adsr >> 16;
	if (o0)
	    o0 *= doosc(&psid->v[0]);
	if (o1)
	    o1 *= doosc(&psid->v[1]);
	if (o2)
	    o2 *= doosc(&psid->v[2]);
	if (o3)
	    o3 *= doosc(&psid->v[3]);
	/* sample */
	psid->pbuf[psid->bufptr++] = ((s32_t)((o0+o1+o2+o3)>>20)-0x800)*psid->vol;
    }
}
#else
inline static void update_sid(sound_t *psid, int nr)
{
    register u32_t		o0, o1, o2;
    register int		dosync1, dosync2;
    while (nr-- > 0)
    {
	/* addfptrs, noise & hard sync test */
	dosync1 = 0;
	if ((psid->v[0].f += psid->v[0].fs) < psid->v[0].fs)
	{
	    psid->v[0].rv = NSHIFT(psid->v[0].rv, 16);
	    if (psid->v[1].sync)
		dosync1 = 1;
	}
	dosync2 = 0;
	if ((psid->v[1].f += psid->v[1].fs) < psid->v[1].fs)
	{
	    psid->v[1].rv = NSHIFT(psid->v[1].rv, 16);
	    if (psid->v[2].sync)
		dosync2 = 1;
	}
	if ((psid->v[2].f += psid->v[2].fs) < psid->v[2].fs)
	{
	    psid->v[2].rv = NSHIFT(psid->v[2].rv, 16);
	    if (psid->v[0].sync)
	    {
		/* hard sync */
		psid->v[0].rv = NSHIFT(psid->v[0].rv, psid->v[0].f >> 28);
		psid->v[0].f = 0;
	    }
	}
	/* hard sync */
	if (dosync2)
	{
	    psid->v[2].rv = NSHIFT(psid->v[2].rv, psid->v[2].f >> 28);
	    psid->v[2].f = 0;
	}
	if (dosync1)
	{
	    psid->v[1].rv = NSHIFT(psid->v[1].rv, psid->v[1].f >> 28);
	    psid->v[1].f = 0;
	}
	/* do adsr */
	if ((psid->v[0].adsr += psid->v[0].adsrs) + 0x80000000 <
	    psid->v[0].adsrz + 0x80000000)
	    trigger_adsr(&psid->v[0]);
	if ((psid->v[1].adsr += psid->v[1].adsrs) + 0x80000000 <
	    psid->v[1].adsrz + 0x80000000)
	    trigger_adsr(&psid->v[1]);
	if ((psid->v[2].adsr += psid->v[2].adsrs) + 0x80000000 <
	    psid->v[2].adsrz + 0x80000000)
	    trigger_adsr(&psid->v[2]);
	/* no sample? */
	if (!psid->pbuf)
	    continue;
	/* oscillators */
	o0 = psid->v[0].adsr >> 16;
	o1 = psid->v[1].adsr >> 16;
	o2 = psid->v[2].adsr >> 16;
	if (o0)
	    o0 *= doosc(&psid->v[0]);
	if (o1)
	    o1 *= doosc(&psid->v[1]);
	if (psid->has3 && o2)
	    o2 *= doosc(&psid->v[2]);
	else
	    o2 = 0;
	/* sample */
	if (psid->emulatefilter)
	{
	    psid->v[0].filtIO = ampMod1x8[(o0>>22)];
	    dofilter(&psid->v[0]);
	    o0 = ((u32_t)(psid->v[0].filtIO)+0x80)<<(7+15);
	    psid->v[1].filtIO = ampMod1x8[(o1>>22)];
	    dofilter(&psid->v[1]);
	    o1 = ((u32_t)(psid->v[1].filtIO)+0x80)<<(7+15);
	    psid->v[2].filtIO = ampMod1x8[(o2>>22)];
	    dofilter(&psid->v[2]);
	    o2 = ((u32_t)(psid->v[2].filtIO)+0x80)<<(7+15);
	}
	psid->pbuf[psid->bufptr++] = ((s32_t)((o0+o1+o2)>>20)-0x600)*psid->vol;
    }
}
#endif

static void init_filter(sound_t *psid, int freq)
{
    u16_t uk;
    float rk;
    long int si;

    float yMax = 1.0;
    float yMin = 0.01;
    float resDyMax = 1.0;
    float resDyMin = 2.0;
    float resDy = resDyMin;

    float yAdd, yTmp;

    float filterFs = 400.0;
    float filterFm = 60.0;
    float filterFt = 0.05;

    float filterAmpl = 1.0;

    filterValue = 0;
    filterType = 0;
    filterCurType = 0;
    filterDy = 0;
    filterResDy = 0;

    for ( uk = 0, rk = 0; rk < 0x800; rk++, uk++ )
    {
	lowPassParam[uk] = (((exp(rk/2048*log(filterFs))/filterFm)+filterFt)
			    *filterRefFreq) / freq;
	if ( lowPassParam[uk] < yMin )
	    lowPassParam[uk] = yMin;
	if ( lowPassParam[uk] > yMax )
	    lowPassParam[uk] = yMax;
    }

	yMax = 0.22;
	yMin = 0.002;
	yAdd = (yMax-yMin)/2048.0;
	yTmp = yMin;
	for ( uk = 0, rk = 0; rk < 0x800; rk++, uk++ )
	{
	    bandPassParam[uk] = (yTmp*filterRefFreq) / freq;
	    yTmp += yAdd;
	}

    for ( uk = 0; uk < 16; uk++ )
    {
	filterResTable[uk] = resDy;
	resDy -= (( resDyMin - resDyMax ) / 15 );
    }
    filterResTable[0] = resDyMin;
    filterResTable[15] = resDyMax;

    /* XXX: if psid->emulatefilter = 0, ampMod1x8 is never referenced */
    if (psid->emulatefilter)
	filterAmpl = 0.7;
    else
	filterAmpl = 1.0;
    for ( uk = 0, si = 0; si < 256; si++, uk++ )
	ampMod1x8[uk] = (signed char)((si-0x80)*filterAmpl);
}

/* SID initialization routine */
static void init_sid(sound_t *psid, s16_t *pbuf, int speed)
{
    u32_t		 i, nr;
    char		*name;

    psid->speed1 = (CYCLES_PER_SEC << 8) / speed;
    for (i = 0; i < 16; i++)
    {
	psid->adrs[i] = 500*8*psid->speed1/adrtable[i];
	psid->sz[i] = 0x8888888*i;
    }
    psid->pbuf = pbuf;
    psid->bufptr = 0;
#ifdef VIC20
    nr = 4;
    name = "SOUND";
#else
    nr = 3;
    name = "SID";
#endif
    if (!psid->pwarn)
	psid->pwarn = warn_init(name, 32);
    else
	warn_reset(psid->pwarn);
    psid->update = 1;
    psid->emulatefilter = app_resources.sidFilters;
    setup_sid(psid);
    init_filter(psid, speed);
    for (i = 0; i < nr; i++)
    {
#ifndef VIC20
	psid->v[i].vprev = &psid->v[(i+2)%3];
	psid->v[i].vnext = &psid->v[(i+1)%3];
#endif
	psid->v[i].nr = i;
	psid->v[i].d = psid->d + i*7;
	psid->v[i].s = psid;
	psid->v[i].rv = NSEED;
	psid->v[i].filtLow = 0;
	psid->v[i].filtRef = 0;
	psid->v[i].filtIO = 0;
	psid->v[i].update = 1;
	setup_voice(&psid->v[i]);
    }
#ifdef VIC20
    psid->v[3].d = psid->d + 57;
#endif
#ifdef WAVETABLES
    psid->newsid = app_resources.sidModel;
    for (i = 0; i < 4096; i++)
    {
	wavetable10[i] = i < 2048 ? i << 4 : 0xffff - (i << 4);
	wavetable20[i] = i << 3;
	wavetable30[i] = waveform30_8580[i] << 7;
	wavetable40[i + 4096] = 0x7fff;
	if (psid->newsid)
	{
	    wavetable50[i + 4096] = waveform50_8580[i] << 7;
	    wavetable60[i + 4096] = waveform60_8580[i] << 7;
	    wavetable70[i + 4096] = waveform70_8580[i] << 7;
	}
	else
	{
	    wavetable50[i + 4096] = waveform50_6581[i >> 3] << 7;
	    wavetable60[i + 4096] = 0;
	    wavetable70[i + 4096] = 0;
	}
    }
#endif
    for (i = 0; i < NOISETABLESIZE; i++)
    {
	noiseLSB[i] = (((i>>(7-2))&0x04)|((i>>(4-1))&0x02)|((i>>(2-0))&0x01));
	noiseMID[i] = (((i>>(13-8-4))&0x10)|((i<<(3-(11-8)))&0x08));
	noiseMSB[i] = (((i<<(7-(22-16)))&0x80)|((i<<(6-(20-16)))&0x40)
 		       |((i<<(5-(16-16)))&0x20));
    }
    for (i = 0; i < 9; i++)
	sidreadclocks[i] = 13;
}

#else /* !SID */

/* PET -code */

typedef struct sound_s
{
    warn_t			*pwarn;
    s16_t			*pbuf;
    s32_t			 bufptr;

    int				 on;
    CLOCK			 t;
    BYTE			 sample;

    double			 b;
    double			 bs;

    int				 speed;
} sound_t;

/* XXX: this can be greatly optimized */
static s16_t pet_makesample(double s, double e, BYTE sample)
{
    double				v;
    int					sc, ec, sf, ef, i, nr;
    sc = ceil(s);
    sf = floor(s);
    ec = ceil(e);
    ef = floor(e);
    nr = 0;
    for (i = sc; i < ef; i++)
	if (sample & (1 << (i%8)))
	    nr++;
    v = nr;
    if (sample & (1 << (sf % 8)))
	v += sc - s;
    if (sample & (1 << (ef % 8)))
	v += e - ef;
    return v * 4095.0 / (e-s);
}

static void init_sid(sound_t *psid, s16_t *pbuf, int speed)
{
    if (!psid->pwarn)
	psid->pwarn = warn_init("SOUND", 32);
    else
	warn_reset(psid->pwarn);
    psid->pbuf = pbuf;
    psid->bufptr = 0;
    psid->speed = speed;
    if (!psid->t)
	psid->t = 32;
    psid->b = 0.0;
    psid->bs = (double)CYCLES_PER_SEC/(psid->t*psid->speed);
}

inline static void update_sid(sound_t *psid, int nr)
{
    int				 i;
    u16_t			 v = 0;
    char			*t = "                ";
    if (!psid->pbuf)
	return;
    warn(psid->pwarn, 4,
	 "Sound support for PET is at _very_ experimental stage.\n"
	 "%sIf you think this doesn't sound right, please wait\n"
	 "%sfor the next snapshot or help me get this right.\n"
	 "%s                          //tvr", t, t, t);
    for (i = 0; i < nr; i++)
    {
	if (psid->on)
	    v = pet_makesample(psid->b, psid->b + psid->bs, psid->sample);
	psid->pbuf[psid->bufptr++] = v;
	psid->b += psid->bs;
	while (psid->b >= 8.0)
	    psid->b -= 8.0;
    }
}

#endif /* !SID */


/*
 * devices
 */
typedef struct
{
    /* name of the device or NULL */
    char			 *name;
    /* init -routine to be called at device initialization. Should use
       suggested values if possible or return new values if they cannot be
       used */
    int				(*init)(sound_t *s, char *device, int *speed,
					int *fragsize, int *fragnr,
					double bufsize);
    /* send number of bytes to the soundcard. it is assumed to block if kernel
       buffer is full */
    int				(*write)(sound_t *s, s16_t *pbuf, int nr);
    /* dump-routine to be called for every write to SID */
    int				(*dump)(ADDRESS addr, BYTE byte, CLOCK clks);
    /* flush-routine to be called every frame */
    int				(*flush)(sound_t *s);
    /* return number of samples unplayed in the kernel buffer at the moment */
    int				(*bufferstatus)(sound_t *s, int first);
    /* close and cleanup device */
    void			(*close)(void);
    /* suspend device */
    int				(*suspend)(sound_t *s);
    /* resume device */
    int				(*resume)(sound_t *s);
} sid_device_t;

#define FRAGS_PER_SECOND ((int)RFSH_PER_SEC)

/*
 * fs-device
 */
static FILE *fs_fd = NULL;

static int fs_init(sound_t *s, char *param, int *speed,
		   int *fragsize, int *fragnr, double bufsize)
{
    if (!param)
	param = "vicesnd.raw";
    fs_fd = fopen(param, "w");
    if (!fs_fd)
	return 1;
    return 0;
}

static int fs_write(sound_t *s, s16_t *pbuf, int nr)
{
    int			i;
    i = fwrite(pbuf, sizeof(s16_t), nr, fs_fd);
    if (i != nr)
	return 1;
    return 0;
}

static void fs_close(void)
{
    fclose(fs_fd);
    fs_fd = NULL;
}

static sid_device_t fs_device =
{
    "fs",
    fs_init,
    fs_write,
    NULL,
    NULL,
    NULL,
    fs_close,
    NULL,
    NULL
};


/*
 * dummy device to get all the benefits of running sid
 */
static sid_device_t dummy_device =
{
    "dummy",
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};

/*
 * Another dummy device to measure speed (this calculates samples)
 */
static int speed_write(sound_t *s, s16_t *pbuf, int nr)
{
    return 0;
}

static sid_device_t speed_device =
{
    "speed",
    NULL,
    speed_write,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};


/*
 * dump device to dump all writes to file for further examination
 */
static FILE *dump_fd = NULL;

static int dump_init(sound_t *s, char *param, int *speed,
		     int *fragsize, int *fragnr, double bufsize)
{
    if (!param)
	param = "vicesnd.sid";
    dump_fd = fopen(param, "w");
    if (!dump_fd)
	return 1;
    return 0;
}

static int dump_dump(ADDRESS addr, BYTE byte, CLOCK clks)
{
    int				i;
    i = fprintf(dump_fd, "%d %d %d\n", (int)clks, addr, byte);
    if (i < 0)
	return 1;
    return 0;
}

static int dump_flush(sound_t *s)
{
    int				i;
#if defined(DEBUG) && defined(CBM64)
    print_sid(dump_fd, s);
#endif
    i = fflush(dump_fd);
    return i;
}

static void dump_close(void)
{
    fclose(dump_fd);
    dump_fd = NULL;
}

static sid_device_t dump_device =
{
    "dump",
    dump_init,
    NULL,
    dump_dump,
    dump_flush,
    NULL,
    dump_close,
    NULL,
    NULL
};


/*
 * timer device to emulate fragmented blocking device behaviour
 */

#ifdef TESTDEVICE

static long test_time_zero = 0;
static long test_time_fragment = 0;
static int test_time_written = 0;
static int test_time_fragsize = 0;
static int test_time_nrfrags = 0;

static long test_now(void)
{
    struct timeval tp;
    gettimeofday(&tp, NULL);
    return tp.tv_sec*1000000 + tp.tv_usec;
}

static int test_bufferstatus(sound_t *s, int first);


static int test_init(sound_t *s, char *param, int *speed,
		     int *fragsize, int *fragnr, double bufsize)
{
    test_time_zero = test_now();
    test_time_fragment = 1000000.0 / ((double)*speed / *fragsize);
    test_time_written = 0;
    test_time_fragsize = *fragsize;
    test_time_nrfrags = *fragnr;
    return 0;
}

static int test_write(sound_t *s, s16_t *pbuf, int nr)
{
    (void)test_bufferstatus(s, 0);
    test_time_written += nr / test_time_fragsize;
    while (test_bufferstatus(s, 0) > test_time_nrfrags*test_time_fragsize)
	usleep(1000000 / (4 * (int)RFSH_PER_SEC));
    return 0;
}

static int test_bufferstatus(sound_t *s, int first)
{
    int			ret;
    long		now = test_now();
    ret = test_time_written - (now - test_time_zero) / test_time_fragment;
    if (ret < 0)
    {
	warn(s->pwarn, -1, "virtual soundbuffer empty");
	test_time_zero = now;
	test_time_written = 0;
	return 0;
    }
    return ret*test_time_fragsize;
}

static sid_device_t test_device =
{
    "test",
    test_init,
    test_write,
    NULL,
    NULL,
    test_bufferstatus,
    NULL,
    NULL,
    NULL
};
#else /* TESTDEVICE */
static sid_device_t test_device;
#endif

/*
 * linux/freebsd -device
 */
#if defined(HAVE_LINUX_SOUNDCARD_H) || defined(HAVE_MACHINE_SOUNDCARD_H)
#if defined(HAVE_LINUX_SOUNDCARD_H)
#include <linux/soundcard.h>
#endif
#if defined(HAVE_MACHINE_SOUNDCARD_H)
#include <machine/soundcard.h>
#endif

static int uss_fd = -1;
static int uss_8bit = 0;
static int uss_bufsize = 0;
static int uss_fragsize = 0;

static int uss_bufferstatus(sound_t *s, int first);

static int uss_init(sound_t *s, char *param, int *speed,
		    int *fragsize, int *fragnr, double bufsize)
{
    int			 st, tmp, orig;
    if (!param)
	param = "/dev/dsp";
    uss_fd = open(param, O_WRONLY, 0777);
    if (uss_fd < 0)
    {
	warn(s->pwarn, -1, "cannot open '%s' for writing", param);
	return 1;
    }
    /* samplesize 16 bits */
#ifdef WORDS_BIGENDIAN
    orig = tmp = AFMT_S16_BE;
#else
    orig = tmp = AFMT_S16_LE;
#endif
    st = ioctl(uss_fd, SNDCTL_DSP_SETFMT, &tmp);
    if (st < 0 || orig != tmp || getenv("USS8BIT"))
    {
	/* samplesize 8 bits */
	orig = tmp = AFMT_U8;
	st = ioctl(uss_fd, SNDCTL_DSP_SETFMT, &tmp);
	if (st < 0 || orig != tmp)
	{
	    warn(s->pwarn, -1, "SNDCTL_DSP_SETFMT failed");
	    goto fail;
	}
	warn(s->pwarn, -1, "playing 8bit sample");
	uss_8bit = 1;
    }
    /* no stereo */
    tmp = 0;
    st = ioctl(uss_fd, SNDCTL_DSP_STEREO, &tmp);
    if (st < 0 || tmp != 0)
    {
	warn(s->pwarn, -1, "SNDCTL_DSP_STEREO failed");
	goto fail;
    }
    /* speed */
    tmp = *speed;
    st = ioctl(uss_fd, SNDCTL_DSP_SPEED, &tmp);
    if (st < 0 || tmp <= 0)
    {
	warn(s->pwarn, -1, "SNDCTL_DSP_SPEED failed");
	goto fail;
    }
    *speed = tmp;
    /* fragments */
    for (tmp = 1; 1 << tmp < *fragsize; tmp++);
    orig = tmp = tmp + (*fragnr << 16) + !uss_8bit;
    st = ioctl(uss_fd, SNDCTL_DSP_SETFRAGMENT, &tmp);
    if (st < 0 || (tmp^orig)&0xffff)
    {
	warn(s->pwarn, -1, "SNDCTL_DSP_SETFRAGMENT failed");
	goto fail;
    }
    if (tmp != orig)
    {
	if (tmp >> 16 > *fragnr)
	{
	    warn(s->pwarn, -1, "SNDCTL_DSP_SETFRAGMENT: too many fragments");
	    goto fail;
	}
	*fragnr = tmp >> 16;
	if (*fragnr < 3)
	{
	    warn(s->pwarn, -1, "SNDCTL_DSP_SETFRAGMENT: too few fragments");
	    goto fail;
	}
    }
    uss_bufsize = (*fragsize)*(*fragnr);
    uss_fragsize = *fragsize;
    return 0;
fail:
    close(uss_fd);
    uss_fd = -1;
    uss_8bit = 0;
    uss_bufsize = 0;
    uss_fragsize = 0;
    return 1;
}

static int uss_write(sound_t *s, s16_t *pbuf, int nr)
{
    int			total, i, now;
    if (uss_8bit)
    {
	/* XXX: ugly to change contents of the buffer */
	for (i = 0; i < nr; i++)
	    ((char *)pbuf)[i] = pbuf[i]/256 + 128;
	total = nr;
    }
    else
	total = nr*sizeof(s16_t);
    for (i = 0; i < total; i += now)
    {
	now = write(uss_fd, (char *)pbuf + i, total - i);
	if (now <= 0)
	{
	    if (now < 0)
		perror("uss_write");
	    return 1;
	}
    }
    return 0;
}

static int uss_bufferstatus(sound_t *s, int first)
{
    audio_buf_info		info;
    int				st, ret;

    st = ioctl(uss_fd, SNDCTL_DSP_GETOSPACE, &info);
    if (st < 0)
    {
	warn(s->pwarn, -1, "SNDCTL_DSP_GETOSPACE failed");
	return -1;
    }
    ret = info.fragments*info.fragsize;
    if (ret != info.bytes)
    {
	warn(s->pwarn, 11, "GETOSPACE: ret(%d)!=bytes(%d)", ret, info.bytes);
	ret = info.bytes;
    }
    if (ret < 0)
    {
        warn(s->pwarn, 12, "GETOSPACE: bytes < 0");
	ret = 0;
    }
    if (!uss_8bit)
	ret /= sizeof(s16_t);
    if (ret > uss_bufsize)
    {
	warn(s->pwarn, 13, "GETOSPACE: bytes > bufsize");
	ret = uss_bufsize;
    }
#if defined(linux)
    /*
     * GETOSPACE before first write returns random value (or actually the
     * value on which the device was when it was closed last time). I hope
     * this has been fixed after 'Sound Driver:3.5-beta2-960210'
     */
    if (first && !ret)
    {
	ret = 1;
	warn(s->pwarn, -1, "SNDCTL_DSP_GETOSPACE not reliable after open()");
    }
#endif
    return ret;
}

static void uss_close(void)
{
    close(uss_fd);
    uss_fd = -1;
    uss_8bit = 0;
    uss_bufsize = 0;
    uss_fragsize = 0;
}

static int uss_suspend(sound_t *s)
{
    int			 st;
    st = ioctl(uss_fd, SNDCTL_DSP_POST, NULL);
    if (st < 0)
    {
	warn(s->pwarn, -1, "SNDCTL_DSP_POST failed");
	return 1;
    }
    return 0;
}

static sid_device_t uss_device =
{
    "uss",
    uss_init,
    uss_write,
    NULL,
    NULL,
    uss_bufferstatus,
    uss_close,
    uss_suspend,
    NULL
};

#else
static sid_device_t uss_device;
#endif


/*
 * sgi sound device
 */
#if defined(sgi) && defined(HAVE_DMEDIA_AUDIO_H)
#include <dmedia/audio.h>
#if defined(HAVE_BSTRING_H)
#include <bstring.h>
#endif

static ALconfig    sgi_audioconfig = NULL;
static ALport      sgi_audioport = NULL;

static void sgi_errorhandler(long err, const char *msg, ...)
{
    printf("sgierrorhandler: %d, %s\n", (int)err, msg);
}

static int sgi_init(sound_t *s, char *param, int *speed,
		    int *fragsize, int *fragnr, double bufsize)
{
    long	chpars[] = {AL_OUTPUT_RATE, 0};
    int		st;

    ALseterrorhandler(sgi_errorhandler);
    chpars[1] = *speed;
    st = ALsetparams(AL_DEFAULT_DEVICE, chpars, 2);
    if (st < 0)
	return 1;
    st = ALgetparams(AL_DEFAULT_DEVICE, chpars, 2);
    if (st < 0)
	return 1;
    *speed = chpars[1];

    sgi_audioconfig = ALnewconfig();
    if (!sgi_audioconfig)
	return 1;
    st = ALsetchannels(sgi_audioconfig, AL_MONO);
    if (st < 0)
	goto fail;
    st = ALsetwidth(sgi_audioconfig, AL_SAMPLE_16);
    if (st < 0)
	goto fail;
    st = ALsetqueuesize(sgi_audioconfig, *fragsize * *fragnr);
    if (st < 0)
        goto fail;
    sgi_audioport = ALopenport("outport", "w", sgi_audioconfig);
    if (!sgi_audioport)
	goto fail;
    return 0;
fail:
    ALfreeconfig(sgi_audioconfig);
    sgi_audioconfig = NULL;
    return 1;
}

static int sgi_write(sound_t *s, s16_t *pbuf, int nr)
{
    int				i;
    i = ALwritesamps(sgi_audioport, pbuf, nr);
    if (i < 0)
	return 1;
    return 0;
}

static int sgi_bufferstatus(sound_t *s, int first)
{
    int				i;
    i = ALgetfilled(sgi_audioport);
    return i;
}

static void sgi_close(void)
{
    /* XXX: code missing */
    ALfreeconfig(sgi_audioconfig);
    sgi_audioconfig = NULL;
}

static sid_device_t sgi_device =
{
    "sgi",
    sgi_init,
    sgi_write,
    NULL,
    NULL,
    sgi_bufferstatus,
    sgi_close,
    NULL,
    NULL
};

#else
static sid_device_t sgi_device;
#endif


/*
 * NetBSD and Solaris
 */
#if defined(HAVE_SYS_AUDIOIO_H)
#include <sys/audioio.h>

static int sun_bufferstatus(sound_t *s, int first);

static int sun_fd = -1;
static int sun_8bit = 0;
static int sun_bufsize = 0;
static int sun_written = 0;

static int toulaw8(s16_t data)
{
    int			v, s, a;

    a = data / 8;

    v = (a < 0 ? -a : a);
    s = (a < 0 ? 0 : 0x80);

    if (v >= 4080)
        a = 0;
    else if (v >= 2032)
        a = 0x0f - (v - 2032) / 128;
    else if (v >= 1008)
        a = 0x1f - (v - 1008) / 64;
    else if (v >= 496)
        a = 0x2f - (v - 496) / 32;
    else if (v >= 240)
        a = 0x3f - (v - 240) / 16;
    else if (v >= 112)
        a = 0x4f - (v - 112) / 8;
    else if (v >= 48)
        a = 0x5f - (v - 48) / 4;
    else if (v >= 16)
        a = 0x6f - (v - 16) / 2;
    else
        a = 0x7f - v;

    a |= s;

    return a;
}


static int sun_init(sound_t *s, char *param, int *speed,
		    int *fragsize, int *fragnr, double bufsize)
{
    int			st;
    struct audio_info	info;

    if (!param)
	param = "/dev/audio";
    sun_fd = open(param, O_WRONLY, 0777);
    if (sun_fd < 0)
	return 1;
    AUDIO_INITINFO(&info);
    info.play.sample_rate = *speed;
    info.play.channels = 1;
    info.play.precision = 16;
    info.play.encoding = AUDIO_ENCODING_LINEAR;
    st = ioctl(sun_fd, AUDIO_SETINFO, &info);
    if (st < 0)
    {
	AUDIO_INITINFO(&info);
	info.play.sample_rate = 8000;
	info.play.channels = 1;
	info.play.precision = 8;
	info.play.encoding = AUDIO_ENCODING_ULAW;
	st = ioctl(sun_fd, AUDIO_SETINFO, &info);
	if (st < 0)
	    goto fail;
	sun_8bit = 1;
	*speed = 8000;
	warn(s->pwarn, -1, "playing 8 bit ulaw at 8000Hz");
    }
    sun_bufsize = (*fragsize)*(*fragnr);
    sun_written = 0;
    return 0;
fail:
    close(sun_fd);
    sun_fd = -1;
    return 1;
}

static int sun_write(sound_t *s, s16_t *pbuf, int nr)
{
    int			total, i, now;
    if (sun_8bit)
    {
	/* XXX: ugly to change contents of the buffer */
	for (i = 0; i < nr; i++)
	    ((char *)pbuf)[i] = toulaw8(pbuf[i]);
	total = nr;
    }
    else
	total = nr*sizeof(s16_t);
    for (i = 0; i < total; i += now)
    {
	now = write(sun_fd, (char *)pbuf + i, total - i);
	if (now <= 0)
	    return 1;
    }
    sun_written += nr;

    while (sun_bufferstatus(s, 0) > sun_bufsize)
	usleep(1000000 / (4 * (int)RFSH_PER_SEC));
    return 0;
}

static int sun_bufferstatus(sound_t *s, int first)
{
    int			st;
    struct audio_info	info;
    st = ioctl(sun_fd, AUDIO_GETINFO, &info);
    if (st < 0)
	return -1;
#ifdef NetBSD
    if (!sun_8bit)
	return sun_written - info.play.samples / sizeof(s16_t);
#endif
    return sun_written - info.play.samples;
}

static void sun_close(void)
{
    close(sun_fd);
    sun_fd = -1;
    sun_8bit = 0;
    sun_bufsize = 0;
    sun_written = 0;
}


static sid_device_t sun_device =
{
#ifdef NetBSD
    "netbsd",
#else
    "sun",
#endif
    sun_init,
    sun_write,
    NULL,
    NULL,
    sun_bufferstatus,
    sun_close,
    NULL,
    NULL
};

#else
static sid_device_t sun_device;
#endif


#if defined(HAVE_LIBUMSOBJ) && defined(HAVE_UMS_UMSAUDIODEVICE_H) && defined(HAVE_UMS_UMSBAUDDEVICE_H)

/* AIX -support by Chris Sharp (sharpc@hursley.ibm.com) */

#include <UMS/UMSAudioDevice.h>
#include <UMS/UMSBAUDDevice.h>

/* XXX: AIX: can these be made static and use aix_ -prefix on these? */
UMSAudioDeviceMClass audio_device_class;
UMSAudioDevice_ReturnCode rc;
UMSBAUDDevice audio_device;
Environment *ev;
UMSAudioTypes_Buffer buffer;
UMSAudioDeviceMClass_ErrorCode audio_device_class_error;
char* error_string;
char* audio_formats_alias;
char* audio_inputs_alias;
char* audio_outputs_alias;
char* obyte_order;
long out_rate;
long left_gain, right_gain;


static int aix_init(sound_t *s, char *param, int *speed,
		     int *fragsize, int *fragnr, double bufsize)
{
    int	st, tmp, i;
    /* open device */
    ev = somGetGlobalEnvironment();
    audio_device = UMSBAUDDeviceNew();
    rc = UMSAudioDevice_open(audio_device, ev, "/dev/paud0", "PLAY",
			     UMSAudioDevice_BlockingIO);
    if (audio_device == NULL)
    {
    	fprintf(stderr,"can't create audio device object\nError: %s\n",
		error_string);
	return 1;
    }

    rc = UMSAudioDevice_set_volume(audio_device, ev, 100);
    rc = UMSAudioDevice_set_balance(audio_device, ev, 0);

    rc = UMSAudioDevice_set_time_format(audio_device, ev, UMSAudioTypes_Msecs);

    if (obyte_order)
        free(obyte_order);
    rc = UMSAudioDevice_set_byte_order(audio_device, ev, "LSB");

    /* set 16bit */
    rc = UMSAudioDevice_set_bits_per_sample(audio_device, ev, 16);
    rc = UMSAudioDevice_set_audio_format_type(audio_device, ev, "PCM");
    rc = UMSAudioDevice_set_number_format(audio_device, ev, "TWOS_COMPLEMENT");

    /* set speed */
    rc = UMSAudioDevice_set_sample_rate(audio_device, ev, *speed, &out_rate);

    /* channels */
    rc = UMSAudioDevice_set_number_of_channels(audio_device, ev, 1);

    /* should we use the default? */
    left_gain = right_gain = 100;
    rc = UMSAudioDevice_enable_output(audio_device, ev, "LINE_OUT",
				      &left_gain, &right_gain);

    /* set buffer size */
    tmp = (*fragsize)*(*fragnr)*sizeof(s16_t);
    buffer._maximum = tmp;
    buffer._buffer  = (char *) xmalloc(tmp);
    buffer._length = 0;


    rc = UMSAudioDevice_initialize(audio_device, ev);
    rc = UMSAudioDevice_start(audio_device, ev);

    return 0;
#if 0
    /* XXX: AIX: everything should check rc, this isn't used now */
fail:
    UMSAudioDevice_stop(audio_device, ev);
    UMSAudioDevice_close(audio_device, ev);
    _somFree(audio_device);
    free(buffer._buffer);
    audio_device = NULL;

    return 1;
#endif
}

static int aix_write(sound_t *s, s16_t *pbuf, int nr)
{
    int	total, i, now;
    long samples_written;

    total = nr*sizeof(s16_t);
    buffer._length = total;
    memcpy(buffer._buffer,pbuf,total);
    rc = UMSAudioDevice_write(audio_device, ev, &buffer, total,
			      &samples_written);
    return 0;
}

static int aix_bufferstatus(sound_t *s, int first)
{
    int i = -1;
    rc = UMSAudioDevice_write_buff_remain(audio_device, ev, &i);
    if (i < 0)
      return -1;
    /* fprintf(stderr,"Audio Buffer remains: %d\n blocks",i); */
    return i/sizeof(s16_t);
}

static void aix_close(void)
{
    rc = UMSAudioDevice_play_remaining_data(audio_device, ev, TRUE);
    UMSAudioDevice_stop(audio_device, ev);
    UMSAudioDevice_close(audio_device, ev);
    _somFree(audio_device);
    free(buffer._buffer);
    audio_device = NULL;
}


static sid_device_t aix_device =
{
    "aix",
    aix_init,
    aix_write,
    NULL,
    NULL,
    aix_bufferstatus,
    aix_close,
    NULL,
    NULL
};

#else
static sid_device_t aix_device;
#endif

#if defined(__hpux) && defined(HAVE_SYS_AUDIO_H)
#include <sys/audio.h>

static int hpux_fd = -1;

static int hpux_init(sound_t *s, char *param, int *speed,
		     int *fragsize, int *fragnr, double bufsize)
{
    int				st, tmp, i;
    if (!param)
	param = "/dev/audio";
    /* open device */
    hpux_fd = open(param, O_WRONLY, 0777);
    if (hpux_fd < 0)
	return 1;
    /* set 16bit */
    st = ioctl(hpux_fd, AUDIO_SET_DATA_FORMAT, AUDIO_FORMAT_LINEAR16BIT);
    if (st < 0)
	goto fail;
    /* set speed */
    st = ioctl(hpux_fd, AUDIO_SET_SAMPLE_RATE, *speed);
    if (st < 0)
	goto fail;
    /* channels */
    st = ioctl(hpux_fd, AUDIO_SET_CHANNELS, 1);
    if (st < 0)
	goto fail;
    /* should we use the default? */
    st = ioctl(hpux_fd, AUDIO_SET_OUTPUT, AUDIO_OUT_SPEAKER);
    if (st < 0)
	goto fail;
    /* set buffer size */
    tmp = (*fragsize)*(*fragnr)*sizeof(s16_t);
    st = ioctl(hpux_fd, AUDIO_SET_TXBUFSIZE, tmp);
    if (st < 0)
    {
	/* XXX: what are valid buffersizes? */
	for (i = 1; i < tmp; i *= 2);
	tmp = i;
	st = ioctl(hpux_fd, AUDIO_SET_TXBUFSIZE, tmp);
	if (st < 0)
	    goto fail;
	*fragnr = tmp / ((*fragsize)*sizeof(s16_t));
    }
    return 0;
fail:
    close(hpux_fd);
    hpux_fd = -1;
    return 1;
}

static int hpux_write(sound_t *s, s16_t *pbuf, int nr)
{
    int			total, i, now;
    total = nr*sizeof(s16_t);
    for (i = 0; i < total; i += now)
    {
	now = write(hpux_fd, (char *)pbuf + i, total - i);
	if (now <= 0)
	    return 1;
    }
    return 0;
}

static int hpux_bufferstatus(sound_t *s, int first)
{
    int				st;
    struct audio_status		ast;
    st = ioctl(hpux_fd, AUDIO_GET_STATUS, &ast);
    if (st < 0)
	return -1;
    return ast.transmit_buffer_count / sizeof(s16_t);
}

static void hpux_close(void)
{
    close(hpux_fd);
    hpux_fd = -1;
}


static sid_device_t hpux_device =
{
    "hpux",
    hpux_init,
    hpux_write,
    NULL,
    NULL,
    hpux_bufferstatus,
    hpux_close,
    NULL,
    NULL
};

#else
static sid_device_t hpux_device;
#endif

#ifdef __MSDOS__

/*
 * MIDAS
 */

#include "vmidas.h"

static int midas_bufferstatus(sound_t *s, int first);

static MIDASstreamHandle midas_stream = NULL;
static int midas_bufsize = -1;
static int midas_maxsize = -1;

static int midas_init(sound_t *s, char *param, int *speed,
		      int *fragsize, int *fragnr, double bufsize)
{
    BOOL		st;

    st = vmidas_startup();
    if (st != TRUE)
	return 1;
    st = MIDASsetOption(MIDAS_OPTION_MIXRATE, *speed);
    if (st != TRUE)
	return 1;
    st = MIDASsetOption(MIDAS_OPTION_MIXING_MODE, MIDAS_MIX_NORMAL_QUALITY);
    if (st != TRUE)
	return 1;
    st = MIDASsetOption(MIDAS_OPTION_OUTPUTMODE, MIDAS_MODE_16BIT_MONO);
    if (st != TRUE)
	return 1;
    st = MIDASsetOption(MIDAS_OPTION_MIXBUFLEN,
			(*fragsize)*(*fragnr)*sizeof(s16_t));
    if (st != TRUE)
	return 1;
    st = MIDASsetOption(MIDAS_OPTION_MIXBUFBLOCKS, *fragnr);
    if (st != TRUE)
	return 1;
#ifdef __MSDOS__
#if 0
    st = MIDASconfig();
    if (st != TRUE)
	return 1;
#endif
#endif
    st = vmidas_init();
    if (st != TRUE)
	return 1;
    st = MIDASopenChannels(1);
    if (st != TRUE)
    {
	/* st = MIDASclose(); */
	return 1;
    }
    midas_stream = MIDASplayStreamPolling(MIDAS_SAMPLE_16BIT_MONO, *speed,
					  (int)(bufsize*1000));
    if (!midas_stream)
    {
	st = MIDAScloseChannels();
	/* st = MIDASclose(); */
	return 1;
    }
    midas_bufsize = (*fragsize)*(*fragnr);
    midas_maxsize = midas_bufsize / 2;
    return 0;
}

static int midas_write(sound_t *s, s16_t *pbuf, int nr)
{
    BOOL		st = 1;
    unsigned int	ist;

    ist = MIDASfeedStreamData(midas_stream, (unsigned char *)pbuf,
			      nr*sizeof(s16_t), TRUE);
    if (ist != nr*sizeof(s16_t))
	return 1;
#ifndef __MSDOS__
    st = MIDASpoll();
#endif
    return !st;
}

static int midas_bufferstatus(sound_t *s, int first)
{
    int			nr;
    if (first)
	return 0;
    nr = MIDASgetStreamBytesBuffered(midas_stream);
    if (nr < 0)
	nr = 0;
    nr /= sizeof(s16_t);
    if (nr > midas_maxsize)
	midas_maxsize = nr;
    return (int)((double)nr/midas_maxsize*midas_bufsize);
}

static void midas_close(void)
{
    BOOL		st;

    /* XXX: we might come here from `atexit', so MIDAS might have been shut
       down already.  This is a dirty kludge, we should find a cleaner way to
       do it. */
    if (vmidas_available())
    {
	st = MIDASstopStream(midas_stream);
	st = MIDAScloseChannels();
	/* st = MIDASclose(); */
    }
    midas_stream = NULL;
    midas_bufsize = -1;
    midas_maxsize = -1;
}

static sid_device_t midas_device =
{
    "midas",
    midas_init,
    midas_write,
    NULL,
    NULL,
    midas_bufferstatus,
    midas_close,
    NULL,
    NULL
};
#else
static sid_device_t midas_device;
#endif


#if defined(HAVE_SDL_AUDIO_H) && defined(HAVE_SDL_SLEEP_H)
#include "SDL_audio.h"
#include "SDL_sleep.h"

static s16_t *sdl_buf = NULL;
static const SDL_AudioSpec *sdl_spec = NULL;
static volatile int sdl_inptr = 0;
static volatile int sdl_outptr = 0;
static volatile int sdl_len = 0;

static void sdl_callback(void *userdata, Uint8 *stream, Uint16 len,
			 Uint8 *lookahead)
{
    int			amount, total;
    total = 0;
    while (total < len/sizeof(s16_t))
    {
	amount = sdl_inptr - sdl_outptr;
	if (amount < 0)
	    amount = sdl_len - sdl_outptr;
	if (amount + total > len/sizeof(s16_t))
	    amount = len/sizeof(s16_t) - total;
	if (!amount)
	{
	    if (!sdl_spec)
	    {
		memset(stream + total*sizeof(s16_t), 0,
		       len - total*sizeof(s16_t));
		return;
	    }
	    Sleep(5);
	    continue;
	}
	memcpy(stream + total*sizeof(s16_t), sdl_buf + sdl_outptr,
	       amount*sizeof(s16_t));
	total += amount;
	sdl_outptr += amount;
	if (sdl_outptr == sdl_len)
	    sdl_outptr = 0;
    }
}

static int sdl_init(sound_t *s, char *param, int *speed,
		    int *fragsize, int *fragnr, double bufsize)
{
    SDL_AudioSpec		spec;
    memset(&spec, 0, sizeof(spec));
    spec.freq = *speed;
    spec.format = AUDIO_S16 | AUDIO_MONO;
    spec.samples = *fragsize;
    spec.callback = sdl_callback;
    sdl_spec = SDL_OpenAudio(&spec);
    if (!sdl_spec)
	return 1;
    if (sdl_spec->format != (AUDIO_S16 | AUDIO_MONO))
    {
	sdl_spec = NULL;
	SDL_CloseAudio();
	return 1;
    }
    sdl_len = (*fragsize)*(*fragnr) + 1;
    sdl_inptr = sdl_outptr = 0;
    sdl_buf = xmalloc(sizeof(s16_t)*sdl_len);
    if (!sdl_buf)
    {
	SDL_CloseAudio();
	return 1;
    }
    *speed = sdl_spec->freq;
    SDL_PauseAudio(0);
    return 0;
}

static int sdl_write(sound_t *s, s16_t *pbuf, int nr)
{
    int			total, amount;
    total = 0;
    while (total < nr)
    {
	amount = sdl_outptr - sdl_inptr;
	if (amount <= 0)
	    amount = sdl_len - sdl_inptr;
	if ((sdl_inptr + amount)%sdl_len == sdl_outptr)
	    amount--;
	if (amount <= 0)
	{
	    Sleep(5);
	    continue;
	}
	if (total + amount > nr)
	    amount = nr - total;
	memcpy(sdl_buf + sdl_inptr, pbuf + total, amount*sizeof(s16_t));
	sdl_inptr += amount;
	total += amount;
	if (sdl_inptr == sdl_len)
	    sdl_inptr = 0;
    }
    return 0;
}

static int sdl_bufferstatus(sound_t *s, int first)
{
    int		amount;
    amount = sdl_inptr - sdl_outptr;
    if (amount < 0)
	amount += sdl_len;
    return amount;
}

static void sdl_close(void)
{
    sdl_spec = NULL;
    SDL_CloseAudio();
    free(sdl_buf);
    sdl_buf = NULL;
    sdl_inptr = sdl_outptr = sdl_len = 0;
}


static sid_device_t sdl_device =
{
    "sdl",
    sdl_init,
    sdl_write,
    NULL,
    NULL,
    sdl_bufferstatus,
    sdl_close,
    NULL,
    NULL
};

#else
static sid_device_t sdl_device;
#endif


static sid_device_t *sid_devices[13] =
{
    &uss_device,
    &sgi_device,
    &sun_device,
    &hpux_device,
    &aix_device,
    &midas_device,
    &sdl_device,
    &dummy_device,
    &fs_device,
    &speed_device,
    &dump_device,
    &test_device,
    NULL
};

/*
 * and the code itself
 */

#define BUFSIZE 32768
typedef struct
{
    /* sid itself */
    sound_t		 sid;
    /* number of clocks between each sample. used value */
    double		 clkstep;
    /* number of clocks between each sample. original value */
    double		 origclkstep;
    /* factor between those two clksteps */
    double		 clkfactor;
    /* time of last sample generated */
    double		 fclk;
    /* time of last write to sid. used for pdev->dump() */
    CLOCK		 wclk;
    /* sample buffer */
    s16_t		 buffer[BUFSIZE];
    /* pointer to device structure in use */
    sid_device_t	*pdev;
    /* number of samples in a fragment */
    int			 fragsize;
    /* number of fragments in kernel buffer */
    int			 fragnr;
    /* number of samples in kernel buffer */
    int			 bufsize;
    /* return value of first pdev->bufferstatus() call to device */
    int			 firststatus;
    /* constants related to adjusting sound */
    int			 prevused;
    int			 prevfill;
    /* is the device suspended? */
    int			 issuspended;
    s16_t		 lastsample;
} siddata_t;

static siddata_t siddata;

/* close sid device and show error dialog if needed */
static int closesid(char *msg)
{
    if (siddata.pdev)
    {
	warn(siddata.sid.pwarn, -1, "closing device '%s'", siddata.pdev->name);
	if (siddata.pdev->close)
	    siddata.pdev->close();
	siddata.pdev = NULL;
    }
    if (msg)
    {
        suspend_speed_eval();
	if (strcmp(msg, ""))
	{
	    UiError(msg);
	    app_resources.sound = 0;
	    UiUpdateMenus();
	}
    }
    siddata.prevused = siddata.prevfill = 0;
    return 1;
}

/* code to disable sid for a given number of seconds if needed */
static int disabletime;

static void suspendsid(char *reason)
{
    disabletime = time(0);
    warn(siddata.sid.pwarn, -1, "SUSPEND: disabling sid for %d secs (%s)",
	 app_resources.soundSuspendTime, reason);
    closesid("");
}

static void enablesid(void)
{
    int		now, diff;
    if (!disabletime)
        return;
    now = time(0);
    diff = now - disabletime;
    if (diff < 0 || diff >= app_resources.soundSuspendTime)
    {
        warn(siddata.sid.pwarn, -1, "ENABLE");
        disabletime = 0;
    }
}

/* open sound device */
static int initsid(void)
{
    int					 i, tmp;
    sid_device_t			*pdev;
    char				*name;
    char				*param;
    int					 speed;
    int					 fragsize;
    int					 fragnr;
    double				 bufsize;
    char				 err[1024];

    if (app_resources.soundSuspendTime > 0 && disabletime)
        return 1;

    name = app_resources.soundDeviceName;
    param = app_resources.soundDeviceArg;
    tmp = app_resources.soundBufferSize;
    if (tmp < 100 || tmp > 1000)
	tmp = SOUND_SAMPLE_BUFFER_SIZE;
    bufsize = tmp / 1000.0;

    speed = app_resources.soundSampleRate;
    if (speed < 8000 || speed > 50000)
	speed = SOUND_SAMPLE_RATE;
    /* calculate optimal fragments */
    fragsize = speed / FRAGS_PER_SECOND;
    for (i = 1; 1 << i < fragsize; i++);
    fragsize = 1 << i;
    fragnr = (speed*bufsize + fragsize - 1) / fragsize;
    if (fragnr < 3)
        fragnr = 3;

    for (i = 0; (pdev = sid_devices[i]); i++)
    {
	if ((name && pdev->name && !strcmp(pdev->name, name)) ||
	    (!name && pdev->name))
	{
	    if (pdev->init)
	    {
		tmp = pdev->init(&siddata.sid, param, &speed,
				 &fragsize, &fragnr, bufsize);
		if (tmp)
		{
		    sprintf(err, "Audio: initialization failed for device `%s'.",
			    pdev->name);
		    return closesid(err);
		}
	    }
	    siddata.issuspended = -1;
	    siddata.lastsample = 0;
	    siddata.pdev = pdev;
	    siddata.fragsize = fragsize;
	    siddata.fragnr = fragnr;
	    siddata.bufsize = fragsize*fragnr;
	    warn(siddata.sid.pwarn, -1,
		 "opened device '%s' speed %dHz fragsize %.3fs bufsize %.3fs",
		 pdev->name, speed, (double)fragsize / speed,
		 (double)siddata.bufsize / speed);
	    app_resources.soundSampleRate = speed;
	    if (pdev->write)
		init_sid(&siddata.sid, siddata.buffer, speed);
	    else
		init_sid(&siddata.sid, NULL, speed);
	    if (pdev->bufferstatus)
		siddata.firststatus = pdev->bufferstatus(&siddata.sid, 1);
	    siddata.clkstep = (double)CYCLES_PER_SEC / speed;
	    siddata.origclkstep = siddata.clkstep;
	    siddata.clkfactor = 1.0;
	    siddata.fclk = clk;
	    siddata.wclk = clk;
	    return 0;
	}
    }
    sprintf(err, "Audio: device `%s' not found or not supported.", name);
    return closesid(err);
}

/* run sid */
static int run_sid(void)
{
    int				nr, i;
    if (!app_resources.sound)
	return 1;
    if (app_resources.soundSuspendTime > 0 && disabletime)
        return 1;
    if (!siddata.pdev)
    {
	i = initsid();
	if (i)
	    return i;
    }
    nr = (clk - siddata.fclk) / siddata.clkstep;
    if (!nr)
	return 0;
    if (siddata.sid.bufptr + nr > BUFSIZE)
	return closesid("Audio: sound buffer overflow.");
#ifdef SID
    setup_sid(&siddata.sid);
    setup_voice(&siddata.sid.v[0]);
    setup_voice(&siddata.sid.v[1]);
    setup_voice(&siddata.sid.v[2]);
#ifdef VIC20
    setup_voice(&siddata.sid.v[3]);
#endif
#endif
    update_sid(&siddata.sid, nr);
    siddata.fclk += nr*siddata.clkstep;
    return 0;
}

/* flush all generated samples from buffer to sounddevice. adjust sid runspeed
   to match real running speed of program */
int flush_sound(void)
{
    int			i, nr, space, used, fill = 0;

    if (app_resources.soundSuspendTime > 0)
        enablesid();
    i = run_sid();
    if (i)
	return 0;
    resume_sound();
    if (siddata.pdev->flush)
    {
	i = siddata.pdev->flush(&siddata.sid);
	if (i)
	{
	    closesid("Audio: cannot flush.");
	    return 0;
	}
    }
    if (siddata.sid.bufptr < siddata.fragsize)
	return 0;
    nr = siddata.sid.bufptr - siddata.sid.bufptr % siddata.fragsize;
    /* adjust speed */
    if (siddata.pdev->bufferstatus)
    {
	space = siddata.pdev->bufferstatus(&siddata.sid, 0);
	if (!siddata.firststatus)
	    space = siddata.bufsize - space;
	used = siddata.bufsize - space;
	if (space < 0 || used < 0)
	{
	    warn(siddata.sid.pwarn, -1, "fragment problems %d %d %d",
		 space, used, siddata.firststatus);
	    closesid("Audio: fragment problems.");
	    return 0;
	}
	/* buffer empty */
	if (used <= siddata.fragsize)
	{
	    s16_t		*p, v;
	    int			 j;
	    static int		 prev;
	    int			 now;
	    if (app_resources.soundSuspendTime > 0)
	    {
	        now = time(0);
		if (now == prev)
		{
		    suspendsid("buffer overruns");
		    return 0;
		}
		prev = now;
	    }
	    j = siddata.fragsize*siddata.fragnr - nr;
	    if (j > siddata.bufsize / 2 &&
		!app_resources.soundSpeedAdjustment &&
		app_resources.speed)
	    {
		j = siddata.fragsize*(siddata.fragnr/2);
	    }
	    j *= sizeof(*p);
	    if (j > 0)
	    {
	        p = xmalloc(j);
		v = siddata.sid.bufptr > 0 ? siddata.buffer[0] : 0;
		for (i = 0; i < j / sizeof(*p); i++)
		    p[i] = (float)v*i/(j / sizeof(*p));
		i = siddata.pdev->write(&siddata.sid, p,
					j / sizeof(*p));
		free(p);
		if (i)
		{
		    closesid("Audio: write to sound device failed.");
		    return 0;
		}
		siddata.lastsample = v;
	    }
	    fill = j;
	}
	if (!app_resources.soundSpeedAdjustment &&
	    app_resources.speed > 0)
	    siddata.clkfactor = app_resources.speed / 100.0;
	else
	{
	    if (siddata.prevfill)
		siddata.prevused = used;
	    siddata.clkfactor *= 1.0 + 0.9*(used - siddata.prevused)/
		siddata.bufsize;
	}
	siddata.prevused = used;
	siddata.prevfill = fill;
	siddata.clkfactor *= 0.9 + (used+nr)*0.12/siddata.bufsize;
	siddata.clkstep = siddata.origclkstep * siddata.clkfactor;
	if (CYCLES_PER_RFSH / siddata.clkstep >= siddata.bufsize)
	{
	    if (app_resources.soundSuspendTime > 0)
	        suspendsid("running too slow");
	    else
	        closesid("Audio: running too slow.");
	    return 0;
	}
	if (nr > space && nr < used)
	    nr = space;
    }
    i = siddata.pdev->write(&siddata.sid, siddata.buffer, nr);
    if (i)
    {
	closesid("Audio: write to sounddevice failed.");
	return 0;
    }
    siddata.lastsample = siddata.buffer[nr-1];
    siddata.sid.bufptr -= nr;
    if (siddata.sid.bufptr > 0)
    {
	for (i = 0; i < siddata.sid.bufptr; i++)
	    siddata.buffer[i] = siddata.buffer[i + nr];
    }
    return 0;
}

/* close sid */
void close_sound(void)
{
    closesid(NULL);
}

/* suspend sid (eg. before pause) */
void suspend_sound(void)
{
    int				 i;
    s16_t			*p, v;
    if (siddata.pdev)
    {
	if (siddata.pdev->write && siddata.issuspended == 0)
	{
	    p = xmalloc(siddata.fragsize*sizeof(s16_t));
	    if (!p)
		return;
	    v = siddata.lastsample;
	    for (i = 0; i < siddata.fragsize; i++)
		p[i] = v - (float)v*i/siddata.fragsize;
	    free(p);
	    i = siddata.pdev->write(&siddata.sid, p, siddata.fragsize);
	    if (i)
		return;
	}
	if (siddata.pdev->suspend && siddata.issuspended == 0)
	{
	    i = siddata.pdev->suspend(&siddata.sid);
	    if (i)
		return;
	}
	siddata.issuspended = 1;
    }
}

/* resume sid */
void resume_sound(void)
{
    int				i;
    if (siddata.pdev)
    {
	if (siddata.pdev->resume && siddata.issuspended == 1)
	{
	    i = siddata.pdev->resume(&siddata.sid);
	    siddata.issuspended = i ? 1 : 0;
	}
	else
	    siddata.issuspended = 0;
    }
}

/* initialize sid at program start -time */
void initialize_sound(void)
{
    /* dummy init to get pwarn */
    init_sid(&siddata.sid, NULL, SOUND_SAMPLE_RATE);
}

#ifdef SID
/* adjust clk before overflow */
void sid_prevent_clk_overflow(void)
{
#ifdef CBM64
    siddata.sid.laststoreclk -= PREVENT_CLK_OVERFLOW_SUB;
#endif
    if (!siddata.pdev)
	return;
    siddata.wclk -= PREVENT_CLK_OVERFLOW_SUB;
    siddata.fclk -= PREVENT_CLK_OVERFLOW_SUB;
}

/* read register value from sid */
BYTE REGPARM1 read_sid(ADDRESS addr)
{
    BYTE		ret;
    u32_t		ffix;
    register u32_t	rvstore;
    register CLOCK	tmp;
    run_sid();
    addr &= 0x1f;
    /* XXX: this is not correct, but what can we do without a running sid? */
    if (!siddata.pdev && addr != 0x19 && addr != 0x1a)
    {
	warn(siddata.sid.pwarn, 5, "program reading sid-registers (no sound)");
	if (addr == 0x1b || addr == 0x1c)
	    return rand();
	/* return siddata.sid.d[addr]; */
	return 0;
    }
    switch (addr)
    {
    case 0x19:
	/* pot/x */
        ret = 0xff;
	break;
    case 0x1a:
	/* pot/y */
        ret = 0xff;
	break;
    case 0x1b:
	/* osc3 / random */
	ffix = ((clk - siddata.fclk) / siddata.clkstep)*siddata.sid.v[2].fs;
	if (
#ifdef WAVETABLES
	    siddata.sid.v[2].noise
#else
	    siddata.sid.v[2].fm == NOISEWAVE
#endif
	    )
	{
	    rvstore = siddata.sid.v[2].rv;
	    if (siddata.sid.v[2].f + ffix < siddata.sid.v[2].f)
		siddata.sid.v[2].rv = NSHIFT(siddata.sid.v[2].rv, 16);
	}
	siddata.sid.v[2].f += ffix;
	ret = doosc(&siddata.sid.v[2]) >> 7;
	siddata.sid.v[2].f -= ffix;
	if (
#ifdef WAVETABLES
	    siddata.sid.v[2].noise
#else
	    siddata.sid.v[2].fm == NOISEWAVE
#endif
	    )
	{
	    siddata.sid.v[2].rv = rvstore;
	}
	warn(siddata.sid.pwarn, 6, "program reading osc3 register");
	break;
    case 0x1c:
	ret = siddata.sid.v[2].adsr >> 23;
	warn(siddata.sid.pwarn, 7, "program reading env3 register");
	break;
    default:
	while ((tmp = siddata.sid.laststorebit) &&
	       (tmp = siddata.sid.laststoreclk + sidreadclocks[tmp]) < clk)
	{
	    siddata.sid.laststoreclk = tmp;
	    siddata.sid.laststore &= 0xfeff >> siddata.sid.laststorebit--;
	}
        ret = siddata.sid.laststore;
    }
    return ret;
}

/* write register value to sid */
void REGPARM2 store_sid(ADDRESS addr, BYTE byte)
{
    int				i;
#ifndef VIC20
    addr &= 0x1f;
#endif
    i = run_sid();
    if (!i && siddata.pdev->dump)
    {
	i = siddata.pdev->dump(addr, byte, clk - siddata.wclk);
	siddata.wclk = clk;
	if (i)
	    closesid("Audio: store to sounddevice failed.");
    }
    switch (addr)
    {
    case 4:
	if ((siddata.sid.d[addr] ^ byte) & 1)
	    siddata.sid.v[0].gateflip = 1;
    case 0: case 1: case 2: case 3: case 5: case 6:
	siddata.sid.v[0].update = 1;
	break;
    case 11:
	if ((siddata.sid.d[addr] ^ byte) & 1)
	    siddata.sid.v[1].gateflip = 1;
    case 7: case 8: case 9: case 10: case 12: case 13:
	siddata.sid.v[1].update = 1;
	break;
    case 18:
	if ((siddata.sid.d[addr] ^ byte) & 1)
	    siddata.sid.v[2].gateflip = 1;
    case 14: case 15: case 16: case 17: case 19: case 20:
	siddata.sid.v[2].update = 1;
	break;
#ifdef VIC20
    case 57: case 58: case 59: case 60: case 61: case 62: case 63:
	siddata.sid.v[3].update = 1;
	break;
#endif
    default:
	siddata.sid.update = 1;
    }
    siddata.sid.d[addr] = byte;
    siddata.sid.laststore = byte;
    siddata.sid.laststorebit = 8;
    siddata.sid.laststoreclk = clk;
}

void reset_sid(void)
{
    int				i;
    for (i = 0; i < 64; i++)
	store_sid(i, 0);
}

#ifdef VIC20
/*
 * XXX: This is _really_ experimental
 */
#define VIC20FREQBASE    65535

void store_vic20sid(ADDRESS addr, BYTE value)
{
    u32_t			freq;
    int				sbase, wval, shift, div;
    addr &= 0x0f;
    if (siddata.pdev)
    {
	char			*t = "                ";
	warn(siddata.sid.pwarn, 8,
	     "Sound support for VIC20 is at _very_ experimental stage.\n"
	     "%sIf you think this doesn't sound right, please wait\n"
	     "%sfor the next snapshot or help me get this right.\n"
	     "%s                          // tvr", t, t, t);
    }
#if 0
    warn(siddata.sid.pwarn, -1, "store_vic20sid(%d, %d)", addr, value);
#endif
    switch (addr)
    {
    case 10:
    case 11:
    case 12:
    case 13:
	sbase = (addr - 10)*7;
	wval = 0x40;
	shift = addr - 10;
	if (addr == 13)
	{
	    sbase = 57;
	    wval = 0x80;
	    shift = 0;
	}
	store_sid(sbase + 2, 0x00);
	store_sid(sbase + 3, 0x08);
	store_sid(sbase + 5, 0x00);
	store_sid(sbase + 6, 0xf0);
	store_sid(sbase + 4, wval+(value>>7));
	div = 255 - value;
	/* XXX: ? */
	if (!div)
	    div = 128;
	freq = VIC20FREQBASE*(1 << shift)/div;
	store_sid(sbase + 0, freq & 0xff);
	store_sid(sbase + 1, (freq / 256) & 0xff);
	break;
    case 14:
	/* volume */
	store_sid(0x18, value & 0x0f);
	break;
    }
}
#endif /* VIC20 */

#endif /* SID */

#ifdef PET
void store_petsnd_onoff(int value)
{
    int				i;
    /* warn(siddata.sid.pwarn, -1, "store_petsnd_onoff(%d)", value); */
    i = run_sid();
    siddata.sid.on = value;
}

void store_petsnd_rate(CLOCK t)
{
    int				i;
    /* warn(siddata.sid.pwarn, -1, "store_petsnd_rate(%d)", t); */
    i = run_sid();
    siddata.sid.t = t;
    /* siddata.sid.b = 0; */
    siddata.sid.bs = (double)CYCLES_PER_SEC/(siddata.sid.t*siddata.sid.speed);
}

void store_petsnd_sample(BYTE sample)
{
    int				i;
    /* warn(siddata.sid.pwarn, -1, "store_petsnd_sample(%d)", sample); */
    i = run_sid();
    siddata.sid.sample = sample;
    while (siddata.sid.b >= 1.0)
	siddata.sid.b -= 1.0;
}
#endif

#endif /* SOUND */

#if !defined(SOUND) && defined(SID)

static BYTE sid[0x20];

void REGPARM2 store_sid(ADDRESS address, BYTE byte)
{
    address &= 0x1f;
    sid[address] = byte;
    return;
}

BYTE REGPARM1 read_sid(ADDRESS address)
{
    address &= 0x1f;
    if (address == 0x1b)
	return rand();
    else
	return sid[address];
}

void reset_sid(void)
{
    int				i;
    for (i = 0; i < 32; i++)
	sid[i] = 0;
}

#endif /* !SOUND && SID */
