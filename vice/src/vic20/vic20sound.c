/*
 * vic20sound.c - implementation of VIC20 sound code
 *
 * Written by
 *  Teemu Rantanen (tvr@cs.hut.fi)
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

#include "vice.h"
#include "sound.h"
#include "maincpu.h"
#include "utils.h"

#include "vic20sound.h"

/* warnings */
static warn_t *pwarn;

/* argh */
static BYTE siddata[16];

/* ADSR state */
#define ATTACK 0
#define DECAY 1
#define SUSTAIN 2
#define RELEASE 3
#define IDLE 4

/* Current waveform */
#define PULSEWAVE 1
#define NOISEWAVE 4
#define NOWAVE 5

/* noise magic */
#define NSHIFT(v, n) (((v)<<(n))|((((v)>>(23-(n)))^(v>>(18-(n))))&((1<<(n))-1)))
#define NVALUE(v) (noiseLSB[v&0xff]|noiseMID[(v>>8)&0xff]|noiseMSB[(v>>16)&0xff])
#define NSEED 0x7ffff8

/* Noise tables */
#define NOISETABLESIZE 256
static BYTE noiseMSB[NOISETABLESIZE];
static BYTE noiseMID[NOISETABLESIZE];
static BYTE noiseLSB[NOISETABLESIZE];

/* needed data for one voice */
typedef struct voice_s
{
    struct sound_s	*s;
    int			 nr;

    /* counter value */
    DWORD		 f;
    /* counter step / sample */
    DWORD		 fs;
    /* waveform that we use */
    BYTE		 fm;
    /* pulse threshold compared to the 32-bit counter */
    DWORD		 pw;

    /* 31-bit adsr counter */
    DWORD		 adsr;
    /* adsr counter step / sample */
    SDWORD		 adsrs;
    /* adsr sustain level compared to the 31-bit counter */
    DWORD		 adsrz;

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
} voice_t;

/* needed data for SID */
struct sound_s
{
    /* number of voices */
    voice_t		 v[4];
    /* SID registers */
    BYTE		 d[64];
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

/* 15-bit oscillator value */
static DWORD doosc(voice_t *pv)
{
    DWORD		f = pv->f;
    switch (pv->fm)
    {
    case NOISEWAVE:
	return ((DWORD)NVALUE(NSHIFT(pv->rv, pv->f >> 28))) << 7;
    case PULSEWAVE:
	if (f > pv->pw)
	    return 0x7fff;
    }
    return 0x0000;
}

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

/* update SID structure */
inline static void setup_sid(sound_t *psid)
{
    if (!psid->update)
	return;
    psid->vol = psid->d[0x18] & 0x0f;
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
    pv->pw = (pv->d[2] + (pv->d[3]&0x0f)*0x100) * 0x100100;
    pv->fs = pv->s->speed1 * (pv->d[0] + pv->d[1]*0x100);
    switch ((pv->d[4] & 0xf0) >> 4)
    {
    case 4:
	pv->fm = PULSEWAVE;
	break;
    case 8:
	pv->fm = NOISEWAVE;
	break;
    case 0:
	pv->fm = NOWAVE;
	break;
    }
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
    int			i;
    DWORD		o0, o1, o2, o3;

    setup_sid(psid);
    setup_voice(&psid->v[0]);
    setup_voice(&psid->v[1]);
    setup_voice(&psid->v[2]);
    setup_voice(&psid->v[3]);

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
	pbuf[i] = ((SDWORD)((o0+o1+o2+o3)>>20)-0x800)*psid->vol;
    }
    return 0;
}


/* SID initialization routine */
sound_t *sound_machine_open(int speed, int cycles_per_sec)
{
    DWORD		 i;
    sound_t		*psid;

    psid = xmalloc(sizeof(*psid));
    memset(psid, 0, sizeof(psid));
    psid->speed1 = (cycles_per_sec << 8) / speed;
    for (i = 0; i < 16; i++)
    {
	psid->adrs[i] = 500*8*psid->speed1/adrtable[i];
	psid->sz[i] = 0x8888888*i;
    }
    psid->update = 1;
    setup_sid(psid);
    for (i = 0; i < 4; i++)
    {
	psid->v[i].nr = i;
	psid->v[i].d = psid->d + i*7;
	psid->v[i].s = psid;
	psid->v[i].rv = NSEED;
	psid->v[i].update = 1;
	setup_voice(&psid->v[i]);
    }
    psid->v[3].d = psid->d + 57;
    for (i = 0; i < NOISETABLESIZE; i++)
    {
	noiseLSB[i] = (((i>>(7-2))&0x04)|((i>>(4-1))&0x02)|((i>>(2-0))&0x01));
	noiseMID[i] = (((i>>(13-8-4))&0x10)|((i<<(3-(11-8)))&0x08));
	noiseMSB[i] = (((i<<(7-(22-16)))&0x80)|((i<<(6-(20-16)))&0x40)
 		       |((i<<(5-(16-16)))&0x20));
    }
    for (i = 0; i < 16; i++)
	sound_machine_store(psid, i, siddata[i]);
    return psid;
}

void sound_machine_close(sound_t *psid)
{
    free(psid);
}

/* write register value to sid */
static void store_sid(sound_t *psid, ADDRESS addr, BYTE byte)
{
    int				i;
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
    case 57: case 58: case 59: case 60: case 61: case 62: case 63:
	psid->v[3].update = 1;
	break;
    default:
	psid->update = 1;
    }
    psid->d[addr] = byte;
}

void sound_reset(void)
{
    int				i;
    for (i = 0; i < 16; i++)
	store_sound(i, 0);
    warn_reset(pwarn);
    sound_prevent_clk_overflow(clk);
}

void sound_machine_init(void)
{
    pwarn = warn_init("SOUNDVIC20", 128);
}


/*
 * XXX: This is _really_ experimental
 */
#define VIC20FREQBASE    65535

void store_sound(ADDRESS addr, BYTE value)
{
    addr &= 0x0f;
    siddata[addr] = value;
    sound_store(addr, value);
}

void sound_machine_store(sound_t *psid, ADDRESS addr, BYTE value)
{
    DWORD			freq;
    int				sbase, wval, shift, div;

#if 0
    {
        char			*t = "                ";
        warn(pwarn, 8,
             "Sound support for VIC20 is at _very_ experimental stage.\n"
             "%sIf you think this doesn't sound right, please wait\n"
             "%sfor the next snapshot or help me get this right.\n"
             "%s                          // tvr", t, t, t);
    }
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
	store_sid(psid, sbase + 2, 0x00);
	store_sid(psid, sbase + 3, 0x08);
	store_sid(psid, sbase + 5, 0x00);
	store_sid(psid, sbase + 6, 0xf0);
	store_sid(psid, sbase + 4, wval+(value>>7));
	div = 255 - value;
	/* XXX: ? */
	if (!div)
	    div = 128;
	freq = VIC20FREQBASE*(1 << shift)/div;
	store_sid(psid, sbase + 0, freq & 0xff);
	store_sid(psid, sbase + 1, (freq / 256) & 0xff);
	break;
    case 14:
	/* volume */
	store_sid(psid, 0x18, value & 0x0f);
	break;
    }
}

void sound_machine_prevent_clk_overflow(sound_t *psid, CLOCK sub)
{
}

BYTE sound_machine_read(sound_t *psid, ADDRESS addr)
{
    abort();
    return 0;
}

char *sound_machine_dump_state(sound_t *psid)
{
    return stralloc("");
}
