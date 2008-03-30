/*
 * petsound.c - implementation of PET sound code
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

#include "vice.h"

#include <math.h>

#include "sound.h"
#include "maincpu.h"
#include "utils.h"

struct sound_s
{
    int				 on;
    CLOCK			 t;
    BYTE			 sample;

    double			 b;
    double			 bs;

    int				 speed;
    int				 cycles_per_sec;
};

static warn_t *pwarn;
static sound_t *psid;

/* XXX: this is not correct */
static WORD pet_makesample(double s, double e, BYTE sample)
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

int sound_machine_calculate_samples(sound_t *psid, SWORD *pbuf, int nr)
{
    int				 i;
    WORD			 v = 0;
    char			*t = "                ";
    warn(pwarn, 4,
	 "Sound support for PET is at _very_ experimental stage.\n"
	 "%sIf you think this doesn't sound right, please wait\n"
	 "%sfor the next snapshot or help me get this right.\n"
	 "%s                          //tvr", t, t, t);
    for (i = 0; i < nr; i++)
    {
	if (psid->on)
	    v = pet_makesample(psid->b, psid->b + psid->bs, psid->sample);
	pbuf[i] = v;
	psid->b += psid->bs;
	while (psid->b >= 8.0)
	    psid->b -= 8.0;
    }
    return 0;
}

void sound_machine_init(void)
{
    pwarn = warn_init("SOUNDPET", 128);
}

sound_t *sound_machine_open(int speed, int cycles_per_sec)
{
    psid = xmalloc(sizeof(*psid));
    if (!psid)
	return NULL;
    psid->speed = speed;
    psid->cycles_per_sec = cycles_per_sec;
    if (!psid->t)
	psid->t = 32;
    psid->b = 0.0;
    psid->bs = (double)psid->cycles_per_sec/(psid->t*psid->speed);
    return psid;
}

void sound_machine_close(sound_t *psid)
{
    free(psid);
    psid = NULL;
}

void store_petsnd_onoff(int value)
{
    int				i;
    i = sound_run_sound();
    psid->on = value;
}

void store_petsnd_rate(CLOCK t)
{
    int				i;
    i = sound_run_sound();
    psid->t = t;
    psid->bs = (double)psid->cycles_per_sec/(psid->t*psid->speed);
}

void store_petsnd_sample(BYTE sample)
{
    int				i;
    i = sound_run_sound();
    psid->sample = sample;
    while (psid->b >= 1.0)
	psid->b -= 1.0;
}

void petsnd_reset(void)
{
    sound_run_sound();
    store_petsnd_onoff(0);
    sound_prevent_clk_overflow(clk);
}
