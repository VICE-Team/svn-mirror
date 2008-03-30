/*
 * ted-sound.c
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
 *  Tibor Biczo <crown @ axelero . hu>
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

#include "lib.h"
#include "maincpu.h"
#include "sound.h"
#include "ted-sound.h"


static BYTE siddata[5];

/* Needed data for one voice.  */
typedef struct voice_s
{
    /* Collect number of cycles elapsed  */
    DWORD accu;
    /* Toggle sign and reload accu if accu reached 0 */
    DWORD reload;
    /* Sign of the square wave  */
    SWORD sign;
    BYTE output_enabled;
} voice_t;

struct sound_s
{
    /* Number of voices  */
    voice_t voice[2];
    /* Volume multiplier  */
    SWORD volume;
    /* 8 cycles units per sample  */
    DWORD speed;
    DWORD sample_position_integer;
    DWORD sample_position_remainder;
    DWORD sample_length_integer;
    DWORD sample_length_remainder;
    /* Digital output?  */
    BYTE digital;
    /* Noise generator active?  */
    BYTE noise;
    BYTE noise_shift_register;
};

/* FIXME: Find proper volume multiplier.  */
static const SWORD volume_tab[16] = {
    0x0000, 0x0800, 0x1000, 0x1800, 0x2000, 0x2800, 0x3000, 0x3800,
    0x3fff, 0x3fff, 0x3fff, 0x3fff, 0x3fff, 0x3fff, 0x3fff, 0x3fff };

int sound_machine_calculate_samples(sound_t *psid, SWORD *pbuf, int nr,
                                    int interleave, int *delta_t)
{
    int i;
    int j;
    SWORD volume;

    if (psid->digital) {
        for (i = 0; i < nr; i++) {
            pbuf[i * interleave] = (SDWORD)(psid->volume *
                                   (psid->voice[0].output_enabled
                                   + psid->voice[1].output_enabled));
        }
    } else {
        for (i = 0; i < nr; i++) {
            psid->sample_position_remainder += psid->sample_length_remainder;
            if (psid->sample_position_remainder >= psid->speed) {
                psid->sample_position_remainder -= psid->speed;
                psid->sample_position_integer++;
            }
            psid->sample_position_integer += psid->sample_length_integer;
            if (psid->sample_position_integer >= 8) {
                /* Advance state engine */
                DWORD ticks = psid->sample_position_integer >> 3;
                if (psid->voice[0].accu <= ticks) {
                    DWORD delay = ticks - psid->voice[0].accu;
                    psid->voice[0].sign ^= 1;
                    psid->voice[0].accu = 1023 - psid->voice[0].reload;
                    if (psid->voice[0].accu == 0) psid->voice[0].accu = 1024;
                    if (delay >= psid->voice[0].accu) {
                        psid->voice[0].sign = ((delay / psid->voice[0].accu)
                                              & 1) ? psid->voice[0].sign ^ 1
                                              : psid->voice[0].sign;
                        psid->voice[0].accu = psid->voice[0].accu
                                              - (delay % psid->voice[0].accu);
                    } else {
                        psid->voice[0].accu -= delay; 
                    }
                } else {
                    psid->voice[0].accu -= ticks;
                }

                if (psid->voice[1].accu <= ticks) {
                    DWORD delay = ticks - psid->voice[1].accu;
                    psid->voice[1].sign ^= 1;
                    psid->noise_shift_register
                        = (psid->noise_shift_register << 1) +
                        ( 1 ^ ((psid->noise_shift_register >> 7) & 1) ^
                        ((psid->noise_shift_register >> 5) & 1) ^
                        ((psid->noise_shift_register >> 4) & 1) ^
                        ((psid->noise_shift_register >> 1) & 1));
                    psid->voice[1].accu = 1023 - psid->voice[1].reload;
                    if (psid->voice[1].accu == 0) psid->voice[1].accu = 1024;
                    if (delay >= psid->voice[1].accu) {
                        psid->voice[1].sign = ((delay / psid->voice[1].accu)
                                              & 1) ? psid->voice[1].sign ^ 1
                                              : psid->voice[1].sign;
                        for (j = 0; j < (int)(delay / psid->voice[1].accu);
                            j++) {
                            psid->noise_shift_register
                                = (psid->noise_shift_register << 1) +
                                ( 1 ^ ((psid->noise_shift_register >> 7) & 1) ^
                                ((psid->noise_shift_register >> 5) & 1) ^
                                ((psid->noise_shift_register >> 4) & 1) ^
                                ((psid->noise_shift_register >> 1) & 1));
                        }
                        psid->voice[1].accu = psid->voice[1].accu
                                              - (delay % psid->voice[1].accu);
                    } else {
                        psid->voice[1].accu -= delay; 
                    }
                } else {
                    psid->voice[1].accu -= ticks;
                }

            }
            psid->sample_position_integer = psid->sample_position_integer & 7;

            volume = 0;

            if (psid->voice[0].output_enabled && psid->voice[0].sign)
                volume += psid->volume;
            if (psid->voice[1].output_enabled
                && !psid->noise && psid->voice[1].sign)
                volume += psid->volume;
            if (psid->voice[1].output_enabled && psid->noise
                && (!(psid->noise_shift_register & 1)))
                volume += psid->volume;

            pbuf[i * interleave] = volume;
        }
    }
    return 0;
}

sound_t *sound_machine_open(int chipno)
{
    return (sound_t *)lib_calloc(1, sizeof(sound_t));
}

int sound_machine_init(sound_t *psid, int speed, int cycles_per_sec)
{
BYTE val;

    psid->speed = speed;
    psid->sample_length_integer = cycles_per_sec / speed;
    psid->sample_length_remainder = cycles_per_sec % speed;
    psid->sample_position_integer = 0;
    psid->sample_position_remainder = 0;

    psid->voice[0].reload = (siddata[0] | (siddata[4] << 8));
    psid->voice[1].reload = (siddata[1] | (siddata[2] << 8));
    val = siddata[3];
    psid->volume = volume_tab[val & 0x0f];
    psid->voice[0].output_enabled = (val & 0x10) ? 1 : 0;
    psid->voice[1].output_enabled = (val & 0x60) ? 1 : 0;
    psid->noise = ((val & 0x60) == 0x40) ? 1 : 0;
    psid->digital = val & 0x80;
    if (psid->digital) {
        psid->voice[0].sign = 1;
        psid->voice[0].accu = 0;
        psid->voice[1].sign = 1;
        psid->voice[1].accu = 0;
        psid->noise_shift_register = 0;
    }

    return 1;
}

void sound_machine_close(sound_t *psid)
{
    lib_free(psid);
}

void sound_machine_store(sound_t *psid, WORD addr, BYTE val)
{
    switch(addr) {
      case 0x0e:
        siddata[0] = val;
        psid->voice[0].reload = (siddata[0] | (siddata[4] << 8));
        break;
      case 0x0f:
        siddata[1] = val;
        psid->voice[1].reload = (siddata[1] | (siddata[2] << 8));
        break;
      case 0x10:
        siddata[2] = val & 3;
        psid->voice[1].reload = (siddata[1] | (siddata[2] << 8));
        break;
      case 0x11:
        psid->volume = volume_tab[val & 0x0f];
        psid->voice[0].output_enabled = (val & 0x10) ? 1 : 0;
        psid->voice[1].output_enabled = (val & 0x60) ? 1 : 0;
        psid->noise = ((val & 0x60) == 0x40) ? 1 : 0;
        psid->digital = val & 0x80;
        if (psid->digital) {
            psid->voice[0].sign = 1;
            psid->voice[0].accu = 0;
            psid->voice[1].sign = 1;
            psid->voice[1].accu = 0;
            psid->noise_shift_register = 0;
        }
        siddata[3] = val;
        break;
      case 0x12:
        siddata[4] = val & 3;
        psid->voice[0].reload = (siddata[0] | (siddata[4] << 8));
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

    psid->noise_shift_register = 0;
    for (i = 0x0e; i <= 0x12; i++)
        sound_machine_store(psid, i, 0);
}

char *sound_machine_dump_state(sound_t *psid)
{
    return lib_msprintf("#SID: clk=%d v=%d\n", maincpu_clk, psid->volume);
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

