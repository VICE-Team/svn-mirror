/*
 * ted-sound.c
 *
 * Written by
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
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

#include "sound.h"
#include "ted-sound.h"
#include "utils.h"

struct sound_s
{
    int on;
    CLOCK t;
    BYTE sample;

    double b;
    double bs;

    int speed;
    int cycles_per_sec;
};

int sound_machine_calculate_samples(sound_t *psid, SWORD *pbuf, int nr,
                                    int interleave, int *delta_t)
{
    return 0;
}

sound_t *sound_machine_open(int chipno)
{
    sound_t *psid;

    psid = xmalloc(sizeof(*psid));
    memset(psid, 0, sizeof(psid));

    return psid;
}

int sound_machine_init(sound_t *psid, int speed, int cycles_per_sec)
{
    return 1;
}

void sound_machine_close(sound_t *psid)
{
    free(psid);
}

void store_petsnd_onoff(int value)
{
}

void store_petsnd_rate(CLOCK t)
{
}

void store_petsnd_sample(BYTE sample)
{
}

void sound_machine_store(sound_t *psid, ADDRESS addr, BYTE val)
{
}

void petsnd_reset(void)
{
}

BYTE sound_machine_read(sound_t *psid, ADDRESS addr)
{
    return 0;
}

void sound_machine_prevent_clk_overflow(sound_t *psid, CLOCK sub)
{
}

void sound_machine_reset(sound_t *psid, CLOCK cpu_clk)
{
}

char *sound_machine_dump_state(sound_t *psid)
{
    char buf[256];

    sprintf(buf, "on=%d sample=%d rate=%d\n", psid->on, psid->sample, psid->t);
    return stralloc(buf);
}

int sound_machine_cycle_based(void)
{
    return 0;
}

int sound_machine_channels(void)
{
    return 1;
}

