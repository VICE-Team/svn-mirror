/*
 * petsound.c - implementation of PET sound code
 *
 * Written by
 *  Teemu Rantanen <tvr@cs.hut.fi>
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
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
#include <string.h>
#include <math.h>

#include "lib.h"
#include "machine.h"
#include "petsound.h"
#include "sid.h"
#include "sidcart.h"
#include "sid-resources.h"
#include "sound.h"
#include "types.h"

/* ------------------------------------------------------------------------- */

/* Some prototypes are needed */
static int pet_sound_machine_init(sound_t *psid, int speed, int cycles_per_sec);
static int pet_sound_machine_calculate_samples(sound_t **psid, int16_t *pbuf, int nr, int sound_output_channels, int sound_chip_channels, CLOCK *delta_t);
static void pet_sound_reset(sound_t *psid, CLOCK cpu_clk);

static int pet_sound_machine_cycle_based(void)
{
    return 0;
}

static int pet_sound_machine_channels(void)
{
    return 1;
}

/* PET userport sound device */
static sound_chip_t pet_sound_chip = {
    NULL,                                /* NO sound chip open function */
    pet_sound_machine_init,              /* sound chip init function */
    NULL,                                /* NO sound chip close function */
    pet_sound_machine_calculate_samples, /* sound chip calculate samples function */
    NULL,                                /* NO sound chip store function */
    NULL,                                /* NO sound chip read function */
    pet_sound_reset,                     /* sound chip reset function */
    pet_sound_machine_cycle_based,       /* sound chip 'is_cycle_based()' function, chip is NOT cycle based */
    pet_sound_machine_channels,          /* sound chip 'get_amount_of_channels()' function, sound chip has 1 channel */
    1                                    /* sound chip enabled flag, chip is always enabled */
};

static uint16_t pet_sound_chip_offset = 0;

void pet_sound_chip_init(void)
{
    pet_sound_chip_offset = sound_chip_register(&pet_sound_chip);
}

/* ------------------------------------------------------------------------- */

/* dummy function for now */
int machine_sid2_check_range(unsigned int sid_adr)
{
    return 0;
}

/* dummy function for now */
int machine_sid3_check_range(unsigned int sid_adr)
{
    return 0;
}

/* dummy function for now */
int machine_sid4_check_range(unsigned int sid_adr)
{
    return 0;
}

/* dummy function for now */
int machine_sid5_check_range(unsigned int sid_adr)
{
    return 0;
}

/* dummy function for now */
int machine_sid6_check_range(unsigned int sid_adr)
{
    return 0;
}

/* dummy function for now */
int machine_sid7_check_range(unsigned int sid_adr)
{
    return 0;
}

/* dummy function for now */
int machine_sid8_check_range(unsigned int sid_adr)
{
    return 0;
}

void machine_sid2_enable(int val)
{
}

struct pet_sound_s {
    int on;             /* are we even making sound? */
    CLOCK t;            /* clocks between shifts: (VIA timer T2 + 2) * 2 */
    uint8_t waveform;   /* value from the VIA Shift Register; msb first */

    double b;           /* SR bit# being output [0,8> (counting from left) */
    double bs;          /* SR bits per output sample */

    int speed;          /* sample rate * 100 / speed_percent */
    int cycles_per_sec;

    int manual;         /* 1 if CB2 set to manual control "high", 0 otherwise */
};

static struct pet_sound_s snd;

/* XXX: this used to be not correct; is a lot better now */
/*
 * This function averages several output bits from the Shift Register
 * into an output sample.
 * Because the shifting speed and the sample speed don't need to line up
 * nicely, this can involve fractional bit times at the start and the end.
 *
 * s: starting bit in the shift register (remember: counting from left)
 * e: ending bit in the SR.
 */
static uint16_t pet_makesample(double s, double e, uint8_t waveform)
{
    double v;
    int sc, sf, ef, i, nr;

    /* Determine whole-bit boundaries */
    sf = (int)floor(s);         /* start floor ("rounded down") */
    sc = sf + 1;                /* start ceiling ("rounded up"); floor+1 */
    ef = (int)floor(e);         /* end floor */
    nr = 0;

    /*
     * Count the value of the signal over whole bit-periods falling in
     * the interval.
     *
     * For example, a time line; | is when the next bit is shifted out:
     *
     *      <-----------snd.bs------------->
     *      s                               e
     * |----------|----------|----------|----------|....more...bits...
     * sf         sc                    ef
     *
     *            [-------------------->
     *            i          i
     */
    for (i = sc; i < ef; i++) {
        if (waveform & (0x80 >> (i % 8))) {
            nr++;
        }
    }

    v = nr;

    if (sf == ef) {
        /*
         * The method of counting fractional bits above doesn't work if s and e
         * fall within the same bit time:
         * - sc-s is more than the time period we're processing
         * - e-ef is also more than the time period we're processing
         *
         *      <-----------snd.bs------------->
         *      s                               e
         * |-------------------------------------------|....more...bits...
         * sf                                          sc
         * ef
         */
        if (waveform & (0x80 >> (sf % 8))) {
            v += e - s;
        }
    } else {
        /*
         * Now add the signal during fractional bit times.
         * The bit at the start is (if set) sc - s wide.
         */
        if (waveform & (0x80 >> (sf % 8))) {
            v += sc - s;
        }
        /* And similar at the end. */
        if (waveform & (0x80 >> (ef % 8))) {
            v += e - ef;
        }
    }

    /*
     * Average over the whole period (e - s) and scale
     * to a range of 0 ... 4095.
     */
    return (uint16_t)(v * 4095.0 / (e - s));
}

static int pet_sound_machine_calculate_samples(sound_t **psid, int16_t *pbuf, int nr, int soc, int scc, CLOCK *delta_t)
{
    int i;
    uint16_t v = 0;

    for (i = 0; i < nr; i++) {
        if (snd.on) {
            v = pet_makesample(snd.b, snd.b + snd.bs, snd.waveform);
        } else if (snd.manual) {
            v = 4095;
        } else {
            v = 0;
        }

        pbuf[i * soc] = sound_audio_mix(pbuf[i * soc], (int16_t)v);
        if (soc > 1) {
            pbuf[(i * soc) + 1] = sound_audio_mix(pbuf[(i * soc) + 1], (int16_t)v);
        }

        snd.b += snd.bs;
        while (snd.b >= 8.0) {
            snd.b -= 8.0;
        }
    }
    return nr;
}

void petsound_store_onoff(int value)
{
    /* Run sound on previous values */
    sound_store(pet_sound_chip_offset, 0, 0);

    snd.on = value;
}

void petsound_store_rate(CLOCK t)
{
    /* Run sound on previous values */
    sound_store(pet_sound_chip_offset, 0, 0);

    snd.t = (uint16_t)t;
    snd.bs = (double)snd.cycles_per_sec / (snd.t * snd.speed);
}

void petsound_store_waveform(uint8_t waveform)
{
    /* Run sound on previous values */
    sound_store(pet_sound_chip_offset, 0, 0);

    snd.waveform = waveform;
    while (snd.b >= 1.0) {
        snd.b -= 1.0;
    }
}

/* For manual control of CB2 sound using $E84C */
void petsound_store_manual(int value)
{
    /* Run sound on previous values */
    sound_store(pet_sound_chip_offset, 0, 0);

    snd.manual = value;
}

static int pet_sound_machine_init(sound_t *psid, int speed, int cycles_per_sec)
{
    snd.speed = speed;
    snd.cycles_per_sec = cycles_per_sec;
    snd.b = 0.0;
    petsound_store_rate(32);

    return 1;
}

static void pet_sound_reset(sound_t *psid, CLOCK cpu_clk)
{
    petsound_store_onoff(0);
}

void petsound_reset(sound_t *psid, CLOCK cpu_clk)
{
    sound_reset();
}

char *sound_machine_dump_state(sound_t *psid)
{
    return sid_sound_machine_dump_state(psid);
}

void sound_machine_enable(int enable)
{
    sid_sound_machine_enable(enable);
}
