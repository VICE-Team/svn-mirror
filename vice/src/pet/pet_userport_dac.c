/*
 * pet_userport_dac.c - PET userport DAC emulation.
 *
 * Written by
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

#include "cmdline.h"
#include "lib.h"
#include "maincpu.h"
#include "resources.h"
#include "pet_userport_dac.h"
#include "sound.h"
#include "uiapi.h"
#include "translate.h"

/* ------------------------------------------------------------------------- */

static int pet_userport_dac_sound_machine_cycle_based(void)
{
    return 0;
}

static int pet_userport_dac_sound_machine_channels(void)
{
    return 1;
}

static sound_chip_t pet_userport_dac_sound_chip = {
    NULL, /* no open */
    pet_userport_dac_sound_machine_init,
    NULL, /* no close */
    pet_userport_dac_sound_machine_calculate_samples,
    pet_userport_dac_sound_machine_store,
    pet_userport_dac_sound_machine_read,
    pet_userport_dac_sound_reset,
    NULL, /* no enable */
    pet_userport_dac_sound_machine_cycle_based,
    pet_userport_dac_sound_machine_channels,
    0x40, /* offset to be filled in by register routine */
    0 /* chip enabled */
};

static sound_chip_list_t *pet_userport_dac_sound_chip_item = NULL;

void pet_userport_dac_sound_chip_init(void)
{
    pet_userport_dac_sound_chip_item = sound_chip_register(&pet_userport_dac_sound_chip);
}

/* ------------------------------------------------------------------------- */

static int set_pet_userport_dac_enabled(int val, void *param)
{
    pet_userport_dac_sound_chip.chip_enabled = val;
    return 0;
}

static const resource_int_t resources_int[] = {
    { "PETUserportDAC", 0, RES_EVENT_STRICT, (resource_value_t)0,
      &pet_userport_dac_sound_chip.chip_enabled, set_pet_userport_dac_enabled, NULL },
    { NULL }
};

int pet_userport_dac_resources_init(void)
{
    return resources_register_int(resources_int);
}

static const cmdline_option_t cmdline_options[] =
{
    { "-userportdac", SET_RESOURCE, 0,
      NULL, NULL, "PETUserportDAC", (resource_value_t)1,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_ENABLE_USERPORT_DAC,
      NULL, NULL },
    { "+userportdac", SET_RESOURCE, 0,
      NULL, NULL, "PETUserportDAC", (resource_value_t)0,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_DISABLE_USERPORT_DAC,
      NULL, NULL },
    { NULL }
};

int pet_userport_dac_cmdline_options_init(void)
{
    return cmdline_register_options(cmdline_options);
}

/* ---------------------------------------------------------------------*/

static BYTE pet_userport_dac_sound_data;

void pet_userport_dac_store(BYTE value)
{
    if (pet_userport_dac_sound_chip.chip_enabled) {
        pet_userport_dac_sound_data = value;
        sound_store(pet_userport_dac_sound_chip.offset, value, 0);
    }
}

struct pet_userport_dac_sound_s
{
    BYTE voice0;
};

static struct pet_userport_dac_sound_s snd;

int pet_userport_dac_sound_machine_calculate_samples(sound_t *psid, SWORD *pbuf, int nr, int interleave, int *delta_t)
{
    int i;

    if (pet_userport_dac_sound_chip.chip_enabled) {
        for (i = 0; i < nr; i++) {
            pbuf[i * interleave] = sound_audio_mix(pbuf[i * interleave], snd.voice0 << 8);
        }
    }
    return 0;
}

int pet_userport_dac_sound_machine_init(sound_t *psid, int speed, int cycles_per_sec)
{
    snd.voice0 = 0;

    return 1;
}

void pet_userport_dac_sound_machine_store(sound_t *psid, WORD addr, BYTE val)
{
    snd.voice0 = val;
}

BYTE pet_userport_dac_sound_machine_read(sound_t *psid, WORD addr)
{
    return pet_userport_dac_sound_data;
}

void pet_userport_dac_sound_reset(sound_t *psid, CLOCK cpu_clk)
{
    snd.voice0 = 0;
    pet_userport_dac_sound_data = 0;
}
