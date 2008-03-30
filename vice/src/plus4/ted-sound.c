/*
 * ted-sound.c
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
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

#include "maincpu.h"
#include "sound.h"
#include "ted-sound.h"
#include "utils.h"

static BYTE siddata[5];

/* Needed data for one voice.  */
typedef struct voice_s
{
    /* Collect number of cycles elapsed  */
    DWORD accu;
    /* Toggle sign if accu has reached the compare value  */
    DWORD compare;
    /* Sign of the square wave  */
    SWORD sign;
} voice_t;

struct sound_s
{
    /* Number of voices  */
    voice_t voice[2];
    /* Volume multiplier  */
    SWORD volume;
    /* 8 cycles units per sample  */
    DWORD speed;
    /* Digital output?  */
    BYTE digital;
    /* Noise generator active?  */
    BYTE noise;
};

/* FIXME: Find proper volume multiplier.  */
static SWORD volume_tab[16] = {
    0x0000, 0x0010, 0x0020, 0x0040, 0x0080, 0x0100, 0x0200, 0x0400,
    0x0800, 0x0800, 0x0800, 0x0800, 0x0800, 0x0800, 0x0800, 0x0800 };

int sound_machine_calculate_samples(sound_t *psid, SWORD *pbuf, int nr,
                                    int interleave, int *delta_t)
{
    int i;

    if (psid->digital) {
        for (i = 0; i < nr; i++) {
            /* FIXME: Implement digis.  */
            pbuf[i * interleave] = (SDWORD)(psid->volume * 0);
        }
    } else {
        for (i = 0; i < nr; i++) {
            if (psid->voice[0].sign) {
                psid->voice[0].accu += psid->speed;
                while (psid->voice[0].accu > psid->voice[0].compare) {
                    psid->voice[0].sign *= -1;
                    psid->voice[0].accu -= psid->voice[0].compare;
                }
            }

            if (psid->voice[1].sign){
                if (psid->noise) {
                    /* FIXME: Implement noise.  */
                } else {
                    psid->voice[1].accu += psid->speed;
                    while (psid->voice[1].accu > psid->voice[1].compare) {
                        psid->voice[1].sign *= -1;
                        psid->voice[1].accu -= psid->voice[1].compare;
                    }
                }
            }
            pbuf[i * interleave] = (SDWORD)(psid->volume *
                                   (psid->voice[0].sign + psid->voice[1].sign));
        }
    }
    return 0;
}

sound_t *sound_machine_open(int chipno)
{
    return (sound_t*)xcalloc(1, sizeof(sound_t));
}

int sound_machine_init(sound_t *psid, int speed, int cycles_per_sec)
{
    psid->speed = cycles_per_sec / speed / 8;

    ted_sound_reset();

    return 1;
}

void sound_machine_close(sound_t *psid)
{
    free(psid);
}

void sound_machine_store(sound_t *psid, WORD addr, BYTE val)
{
    switch(addr) {
      case 0x0e:
        siddata[0] = val;
        psid->voice[0].compare = (siddata[0] | (siddata[4] << 8)) + 1;
        break;
      case 0x0f:
        siddata[1] = val;
        psid->voice[1].compare = (siddata[1] | (siddata[2] << 8)) + 1;
        break;
      case 0x10:
        siddata[2] = val & 3;
        psid->voice[1].compare = (siddata[1] | (siddata[2] << 8)) + 1;
        break;
      case 0x11:
        psid->volume = volume_tab[val & 0x0f];
        if ((val & 0x10) ^ (siddata[3] & 0x10)) {
            psid->voice[0].sign = (val & 0x10) ? 1 : 0;
            psid->voice[0].accu = 0;
        }
        if ((val & 0x20) ^ (siddata[3] & 0x20)) {
            psid->voice[1].sign = (val & 0x20) ? 1 : 0;
            psid->voice[1].accu = 0;
        }
        if ((val & 0x60) == 0x60)
            psid->noise = 1;
        else
            psid->noise = 0;
        psid->digital = val & 0x80;
        siddata[3] = val;
        break;
      case 0x12:
        siddata[4] = val & 3;
        psid->voice[0].compare = (siddata[0] | (siddata[4] << 8)) + 1;
        break;
    }
}

BYTE sound_machine_read(sound_t *psid, WORD addr)
{
    switch(addr) {
      case 0x0e:
        return siddata[0];
      case 0x0f:
        return siddata[1];
      case 0x10:
        return siddata[2] | 0xc0;
      case 0x11:
        return siddata[3];
      case 0x12:
        return siddata[4];
    }

    return 0;
}

void sound_machine_prevent_clk_overflow(sound_t *psid, CLOCK sub)
{
}

void sound_machine_reset(sound_t *psid, CLOCK cpu_clk)
{
    WORD i;

    for (i = 0x0e; i <= 0x12; i++)
        sound_machine_store(psid, i, 0);
}

char *sound_machine_dump_state(sound_t *psid)
{
    return xmsprintf("#SID: clk=%d v=%d\n", maincpu_clk, psid->volume);
}

int sound_machine_cycle_based(void)
{
    return 0;
}

int sound_machine_channels(void)
{
    return 1;
}

void sound_machine_enable(int enable)
{

}

/* ---------------------------------------------------------------------*/

void REGPARM2 ted_sound_store(WORD addr, BYTE value)
{
    sound_store(addr, value, 0);
}

void ted_sound_reset(void)
{
    sound_reset();
}

BYTE REGPARM1 ted_sound_read(WORD addr)
{
    BYTE value;

    value = sound_read(addr, 0);

    if (addr == 0x12)
        value &= 3;

    return value;
}

