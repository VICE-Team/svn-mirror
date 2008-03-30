/*
 * sid.c - MOS6581 (SID) emulation.
 *
 * Written by
 *  Teemu Rantanen (tvr@cs.hut.fi)
 *  Michael Schwendt (sidplay@geocities.com)
 *
 * Resource and cmdline code by
 *  Ettore Perazzoli (ettore@comm2000.it)
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

#include <stdio.h>
#include <math.h>

#include "vice.h"

#include "sid.h"

#include "cmdline.h"
#include "machine.h"
#include "maincpu.h"
#include "resources.h"
#include "snapshot.h"
#include "utils.h"

#ifdef HAVE_RESID
#include "resid.h"
static int useresid;
#endif

#ifdef HAVE_MOUSE
#include "mouse.h"
#endif

/* ------------------------------------------------------------------------- */

/* Resource handling -- Added by Ettore 98-04-26.  */

/* FIXME: We need sanity checks!  And do we really need all of these
   `close_sound()' calls?  */

static int sid_filters_enabled;       /* app_resources.sidFilters */
static int sid_model;                 /* app_resources.sidModel */
static int sid_useresid;

static int set_sid_filters_enabled(resource_value_t v)
{
    sid_filters_enabled = (int)v;
    sound_close();
    return 0;
}

static int set_sid_model(resource_value_t v)
{
    sid_model = (int)v;
    sound_close();
    return 0;
}

static int set_sid_useresid(resource_value_t v)
{
    sid_useresid = (int)v;
    sound_close();
    return 0;
}

static resource_t resources[] = {
    { "SidFilters", RES_INTEGER, (resource_value_t) 1,
      (resource_value_t *) &sid_filters_enabled, set_sid_filters_enabled },
    { "SidModel", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &sid_model, set_sid_model },
    { "SidUseResid", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &sid_useresid, set_sid_useresid },
    { NULL }
};

int sid_init_resources(void)
{
    return resources_register(resources);
}

/* ------------------------------------------------------------------------- */

/* Command-line options -- Added by Ettore 98-05-09.  */
static cmdline_option_t cmdline_options[] = {
    { "-sidmodel", SET_RESOURCE, 1, NULL, NULL, "SidModel", NULL,
      "<model>", "Specify SID model (1: 8580, 0: 6581)" },
    { "-sidfilters", SET_RESOURCE, 0, NULL, NULL, "SidFilters", (resource_value_t) 1,
      NULL, "Emulate SID filters" },
    { "+sidfilters", SET_RESOURCE, 0, NULL, NULL, "SidFilters", (resource_value_t) 0,
      NULL, "Do not emulate SID filters" },
#ifdef HAVE_RESID
    { "-resid", SET_RESOURCE, 0, NULL, NULL, "SidUseResid", (resource_value_t) 1,
      NULL, "Use reSID emulation" },
    { "+resid", SET_RESOURCE, 0, NULL, NULL, "SidUseResid", (resource_value_t) 0,
      NULL, "Use fast SID emulation" },
#endif
    { NULL }
};

int sid_init_cmdline_options(void)
{
    return cmdline_register_options(cmdline_options);
}

/* ------------------------------------------------------------------------- */

/* warnings */
static warn_t *pwarn;

/* argh */
static BYTE siddata[32];

/* use wavetables (sampled waveforms) */
#define WAVETABLES

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

static WORD wavetable00[2];
static WORD wavetable10[4096];
static WORD wavetable20[4096];
static WORD wavetable30[4096];
static WORD wavetable40[8192];
static WORD wavetable50[8192];
static WORD wavetable60[8192];
static WORD wavetable70[8192];

#endif

/* Noise tables */
#define NOISETABLESIZE 256
static BYTE noiseMSB[NOISETABLESIZE];
static BYTE noiseMID[NOISETABLESIZE];
static BYTE noiseLSB[NOISETABLESIZE];

/* needed data for one voice */
typedef struct voice_s
{
    struct sound_s	*s;
    struct voice_s	*vprev;
    struct voice_s	*vnext;
    int			 nr;

    /* counter value */
    DWORD		 f;
    /* counter step / sample */
    DWORD		 fs;
#ifdef WAVETABLES
    /* do we have noise enabled? */
    BYTE		 noise;
#else
    /* waveform that we use */
    BYTE		 fm;
    /* pulse threshold compared to the 32-bit counter */
    DWORD		 pw;
#endif

    /* 31-bit adsr counter */
    DWORD		 adsr;
    /* adsr counter step / sample */
    SDWORD		 adsrs;
    /* adsr sustain level compared to the 31-bit counter */
    DWORD		 adsrz;

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
    DWORD		 rv;
#ifdef WAVETABLES
    /* pointer to wavetable data */
    WORD		*wt;
    /* 32-bit offset to add to the counter before referencing the wavetable.
       This is used on combined waveforms, when other waveforms are combined
       with pulse */
    DWORD		 wtpf;
    /* length of wavetable (actually number of shifts needed for 32-bit
       counter) */
    DWORD		 wtl;
    /* kludge for ring modulation. Set wtr[1] = 0x7fff if ring modulation is
       used */
    WORD		 wtr[2];
#endif

    signed char		filtIO;
    float		filtLow, filtRef;
} voice_t;

/* needed data for SID */
struct sound_s
{
    /* number of voices */
    voice_t		 v[3];
    /* SID registers */
    BYTE		 d[32];
    /* is voice 3 enabled? */
    BYTE		 has3;
    /* 4-bit volume value */
    BYTE		 vol;

    /* ADSR counter step values for each adsr values */
    SDWORD		 adrs[16];
    /* sustain values compared to 31-bit ADSR counter */
    DWORD		 sz[16];

    /* internal constant used for sample rate dependent calculations */
    DWORD		 speed1;

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
};

/* XXX: check these */
/* table for internal ADSR counter step calculations */
static WORD adrtable[16] =
{
    1, 4, 8, 12, 19, 28, 34, 40, 50, 125, 250, 400, 500, 1500, 2500, 4000
};

/* XXX: check these */
/* table for pseudo-exponential ADSR calculations */
static DWORD exptable[6] =
{
    0x30000000, 0x1c000000, 0x0e000000, 0x08000000, 0x04000000, 0x00000000
};

/* clockcycles for each dropping bit when write-only register read is done */
static DWORD sidreadclocks[9];

static float lowPassParam[0x800];
#define filterTable lowPassParam
static float bandPassParam[0x800];
static float filterResTable[16];
static float filterDy, filterResDy;
static BYTE filterType = 0;
static BYTE filterCurType = 0;
static WORD filterValue;
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
inline static DWORD doosc(voice_t *pv)
{
    if (pv->noise)
	return ((DWORD)NVALUE(NSHIFT(pv->rv, pv->f >> 28))) << 7;
    return pv->wt[(pv->f + pv->wtpf) >> pv->wtl] ^ pv->wtr[pv->vprev->f >> 31];
}
#else
static DWORD doosc(voice_t *pv)
{
    DWORD		f = pv->f;
    switch (pv->fm)
    {
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
    case NOISEWAVE:
	return ((DWORD)NVALUE(NSHIFT(pv->rv, pv->f >> 28))) << 7;
    case PULSEWAVE:
	if (f >= pv->pw)
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

static void print_voice(char *buf, voice_t *pv)
{
    char *m = "ADSRI";
#ifdef WAVETABLES
    char *w = "0123456789abcdef";
#else
    char *w = "TPSTN-R5";
#endif
    sprintf(buf,
	    "#SID: V%d: e=%5.1f%%(%c) w=%6.1fHz(%c) f=%5.1f%% p=%5.1f%%\n",
	    pv->nr,
	    (double)pv->adsr*100.0 / (((DWORD)1 << 31) - 1), m[pv->adsrm],
	    (double)pv->fs / (pv->s->speed1*16),
#ifdef WAVETABLES
	    w[pv->d[4]>>4],
#else
	    w[pv->fm],
#endif
	    (double)pv->f*100.0 / ((DWORD)-1),
#ifdef WAVETABLES
	    (double)(pv->d[2] + (pv->d[3]&0x0f)*0x100)/40.95
#else
	    (double)pv->pw*100.0 / ((DWORD)-1)
#endif
	);
}

char *sound_machine_dump_state(sound_t *psid)
{
    int			i;
    char		buf[1024];
#ifdef HAVE_RESID
    if (useresid)
	return resid_sound_machine_dump_state(psid);
#endif
    sprintf(buf, "#SID: clk=%d v=%d s3=%d\n", clk, psid->vol, psid->has3);
    for (i = 0; i < 3; i++)
	print_voice(buf + strlen(buf), &psid->v[i]);
    return stralloc(buf);
}

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
	filterValue = 0x7ff&((psid->d[0x15]&7)|((WORD)psid->d[0x16])<<3);
	if (filterType == 0x20)
	    filterDy = bandPassParam[filterValue];
	else
	    filterDy = lowPassParam[filterValue];
	filterResDy = filterResTable[psid->d[0x17]>>4]-filterDy;
	if (filterResDy < 1.0)
	    filterResDy = 1.0;
	if (psid->d[0x17] & 0x07)
	    warn(pwarn, 0, "program uses filters");
    }
    else
    {
	psid->v[0].filter = 0;
	psid->v[1].filter = 0;
	psid->v[2].filter = 0;
	if (psid->d[0x17] & 0x07)
	    warn(pwarn, 0, "filters are disabled");
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
	warn(pwarn, 1, "program uses hard sync");
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
	warn(pwarn, 3, "program combines waveforms");
	break;
    case 4:
	if (pv->d[4] & 0x08)
	    pv->wt = &wavetable40[4096];
	else
	    pv->wt = &wavetable40[4096 - (pv->d[2] + (pv->d[3]&0x0f)*0x100)];
	break;
    case 5:
	warn(pwarn, 9, "program combines pulse and triangle waveforms");
	pv->wt = &wavetable50[pv->wtpf = 4096 - (pv->d[2] + (pv->d[3]&0x0f)*0x100)];
	pv->wtpf <<= 20;
	if (pv->d[4] & 0x04)
	    pv->wtr[1] = 0x7fff;
	break;
    case 6:
	warn(pwarn, 10, "program combines pulse and sawtooth waveforms");
	pv->wt = &wavetable60[pv->wtpf = 4096 - (pv->d[2] + (pv->d[3]&0x0f)*0x100)];
	pv->wtpf <<= 20;
	break;
    case 7:
	pv->wt = &wavetable70[pv->wtpf = 4096 - (pv->d[2] + (pv->d[3]&0x0f)*0x100)];
	pv->wtpf <<= 20;
	if (pv->d[4] & 0x04 && pv->s->newsid)
	    pv->wtr[1] = 0x7fff;
	warn(pwarn, 3, "program combines waveforms");
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
	warn(pwarn, 2, "program uses ring modulation");
#else
    if (pv->d[4] & 0x08)
    {
	pv->fm = TESTWAVE;
	pv->pw = pv->f = pv->fs = 0;
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
	    warn(pwarn, 2, "program uses ring modulation");
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
	warn(pwarn, 9, "program combines pulse and triangle waveforms");
	break;
    case 6:
	pv->fm = PULSESAWTOOTHWAVE;
	warn(pwarn, 10,
	     "program combines pulse and sawtooth waveforms");
	break;
    default:
	pv->fm = NOWAVE;
	warn(pwarn, 3, "program combines waveforms");
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

int sound_machine_calculate_samples(sound_t *psid, SWORD *pbuf, int nr)
{
    register DWORD		o0, o1, o2;
    register int		dosync1, dosync2, i;

#ifdef HAVE_RESID
    if (useresid)
	return resid_sound_machine_calculate_samples(psid, pbuf, nr);
#endif

    setup_sid(psid);
    setup_voice(&psid->v[0]);
    setup_voice(&psid->v[1]);
    setup_voice(&psid->v[2]);

    for (i = 0; i < nr; i++)
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
	    o0 = ((DWORD)(psid->v[0].filtIO)+0x80)<<(7+15);
	    psid->v[1].filtIO = ampMod1x8[(o1>>22)];
	    dofilter(&psid->v[1]);
	    o1 = ((DWORD)(psid->v[1].filtIO)+0x80)<<(7+15);
	    psid->v[2].filtIO = ampMod1x8[(o2>>22)];
	    dofilter(&psid->v[2]);
	    o2 = ((DWORD)(psid->v[2].filtIO)+0x80)<<(7+15);
	}
	pbuf[i] = ((SDWORD)((o0+o1+o2)>>20)-0x600)*psid->vol;
    }
    return 0;
}


static void init_filter(sound_t *psid, int freq)
{
    WORD uk;
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
sound_t *sound_machine_open(int speed, int cycles_per_sec)
{
    DWORD		 i;
    sound_t		*psid;

#ifdef HAVE_RESID
    useresid = sid_useresid;
    if (useresid)
	return resid_sound_machine_open(speed, cycles_per_sec,
					sid_filters_enabled, siddata,
					sid_model, clk);
#endif
    psid = xmalloc(sizeof(*psid));
    memset(psid, 0, sizeof(*psid));
    memcpy(psid->d, siddata, 32);
    psid->speed1 = (cycles_per_sec << 8) / speed;
    for (i = 0; i < 16; i++)
    {
	psid->adrs[i] = 500*8*psid->speed1/adrtable[i];
	psid->sz[i] = 0x8888888*i;
    }
    psid->update = 1;
    psid->emulatefilter = sid_filters_enabled;
    setup_sid(psid);
    init_filter(psid, speed);
    for (i = 0; i < 3; i++)
    {
	psid->v[i].vprev = &psid->v[(i+2)%3];
	psid->v[i].vnext = &psid->v[(i+1)%3];
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
#ifdef WAVETABLES
    psid->newsid = sid_model == 1;
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
    return psid;
}

void sound_machine_close(sound_t *psid)
{
#ifdef HAVE_RESID
    if (useresid)
	resid_sound_machine_close(psid);
    else
#endif
    free(psid);
}


/* read register value from sid */
static BYTE lastsidread;

BYTE REGPARM1 read_sid(ADDRESS addr)
{
    int				val;
    machine_handle_pending_alarms(0);
    addr = addr & 0x1f;
#ifdef HAVE_MOUSE
    if (addr == 0x19)
        val = mouse_get_x();
    else if (addr == 0x1a)
        val = mouse_get_y();
    else
#endif
    val = sound_read(addr);
    if (val < 0)
    {
        if (addr == 0x19 || addr == 0x1a)
	    val = 0xff;
	else
	{
	    warn(pwarn, 5, "program reading sid-registers (no sound)");
	    if (addr == 0x1b || addr == 0x1c)
		val = rand();
	    else
		val = 0;
	}
    }
    lastsidread = val;
    return val;
}

BYTE sound_machine_read(sound_t *psid, ADDRESS addr)
{
    BYTE		ret;
    WORD		ffix;
    register WORD	rvstore;
    register CLOCK	tmp;

#ifdef HAVE_RESID
    if (useresid)
	return resid_sound_machine_read(psid, addr, clk);
#endif
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
	ffix = sound_sample_position()*psid->v[2].fs;
	rvstore = psid->v[2].rv;
	if (
#ifdef WAVETABLES
	    psid->v[2].noise
#else
	    psid->v[2].fm == NOISEWAVE
#endif
	    && psid->v[2].f + ffix < psid->v[2].f)
	{
	    psid->v[2].rv = NSHIFT(psid->v[2].rv, 16);
	}
	psid->v[2].f += ffix;
	ret = doosc(&psid->v[2]) >> 7;
	psid->v[2].f -= ffix;
	psid->v[2].rv = rvstore;
	warn(pwarn, 6, "program reading osc3 register");
	break;
    case 0x1c:
	ret = psid->v[2].adsr >> 23;
	warn(pwarn, 7, "program reading env3 register");
	break;
    default:
	while ((tmp = psid->laststorebit) &&
	       (tmp = psid->laststoreclk + sidreadclocks[tmp]) < clk)
	{
	    psid->laststoreclk = tmp;
	    psid->laststore &= 0xfeff >> psid->laststorebit--;
	}
        ret = psid->laststore;
    }
    return ret;
}

/* write register value to sid */
void REGPARM2 store_sid(ADDRESS addr, BYTE byte)
{
    addr &= 0x1f;
    siddata[addr] = byte;
    machine_handle_pending_alarms(rmw_flag + 1);
    if (rmw_flag)
    {
	clk--;
	sound_store(addr, lastsidread);
	clk++;
#if 0
	/* XXX: remove me some day */
	warn(pwarn, 4, "rmw instruction");
#endif
    }
    sound_store(addr, byte);
}

void sound_machine_store(sound_t *psid, ADDRESS addr, BYTE byte)
{
#ifdef HAVE_RESID
    if (useresid)
    {
	resid_sound_machine_store(psid, addr, byte, clk);
	return;
    }
#endif
    switch (addr)
    {
    case 4:
	if ((psid->d[addr] ^ byte) & 1)
	    psid->v[0].gateflip = 1;
    case 0: case 1: case 2: case 3: case 5: case 6:
	psid->v[0].update = 1;
	break;
    case 11:
	if ((psid->d[addr] ^ byte) & 1)
	    psid->v[1].gateflip = 1;
    case 7: case 8: case 9: case 10: case 12: case 13:
	psid->v[1].update = 1;
	break;
    case 18:
	if ((psid->d[addr] ^ byte) & 1)
	    psid->v[2].gateflip = 1;
    case 14: case 15: case 16: case 17: case 19: case 20:
	psid->v[2].update = 1;
	break;
    default:
	psid->update = 1;
    }
    psid->d[addr] = byte;
    psid->laststore = byte;
    psid->laststorebit = 8;
    psid->laststoreclk = clk;
}

void sid_reset(void)
{
    int				i;
    memset(siddata, 0, 32);
    for (i = 0; i < 32; i++)
	sound_store(i, 0);
    warn_reset(pwarn);
    sound_prevent_clk_overflow(clk);
}

void sound_machine_init(void)
{
    pwarn = warn_init("SID", 128);
#ifdef HAVE_RESID
    resid_sound_machine_init();
#endif
}

void sound_machine_prevent_clk_overflow(sound_t *psid, CLOCK sub)
{
#ifdef HAVE_RESID
    if (useresid)
	resid_sound_machine_prevent_clk_overflow(psid, sub);
    else
#endif
    psid->laststoreclk -= sub;
}

/* ------------------------------------------------------------------------- */

static char snap_module_name[] = "SID";
#define SNAP_MAJOR 0
#define SNAP_MINOR 0

int sid_write_snapshot_module(snapshot_t *s)
{
    snapshot_module_t *m;

    m = snapshot_module_create(s, snap_module_name, SNAP_MAJOR, SNAP_MINOR);
    if (m == NULL)
        return -1;

    if (snapshot_module_write_byte_array(m, siddata, 32) < 0) {
        snapshot_module_close(m);
        return -1;
    }

    snapshot_module_close(m);
    return 0;
}

int sid_read_snapshot_module(snapshot_t *s)
{
    BYTE major_version, minor_version;
    snapshot_module_t *m;

    sound_close();

    m = snapshot_module_open(s, snap_module_name,
                             &major_version, &minor_version);
    if (m == NULL)
        return -1;

    if (major_version > SNAP_MAJOR || minor_version > SNAP_MINOR) {
        fprintf(stderr,
                "SID: Snapshot module version (%d.%d) newer than %d.%d.\n",
                major_version, minor_version,
                SNAP_MAJOR, SNAP_MINOR);
        return snapshot_module_close(m);
    }

    if (snapshot_module_read_byte_array(m, siddata, 32) < 0)
	return -1;

    return snapshot_module_close(m);
}
