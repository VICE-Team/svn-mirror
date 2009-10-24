/*
 * sfx_soundexpander.c - SFX soundexpnader cartridge emulation.
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

#include "c64io.h"
#include "cmdline.h"
#include "fmopl.h"
#include "lib.h"
#include "maincpu.h"
#include "resources.h"
#include "sfx_soundexpander.h"
#include "sid.h"
#include "sound.h"
#include "uiapi.h"
#include "translate.h"

/* Flag: Do we enable the SFX soundexpander cartridge?  */
int sfx_soundexpander_enabled;

/* Flag: What type of ym chip is used?  */
int sfx_soundexpander_chip = 3526;

static FM_OPL *YM3526_chip = NULL;
static FM_OPL *YM3812_chip = NULL;

static int set_sfx_soundexpander_enabled(int val, void *param)
{
    if (sid_sound_machine_cycle_based() == 1 && val) {
        ui_error(translate_text(IDGS_SFX_SE_NOT_WITH_RESID));
        return -1;
    }
    sfx_soundexpander_enabled=val;

    return 0;
}

static int set_sfx_soundexpander_chip(int val, void *param)
{
    int newval;

    if (val != 3526 && val != 3812) {
        newval = 3526;
    } else {
        newval = val;
    }

    if (newval != sfx_soundexpander_chip) {
        sid_state_changed = 1;
        sfx_soundexpander_chip = newval;
    }
    return 0;
}

static const resource_int_t resources_int[] = {
    { "SFXSoundExpander", 0, RES_EVENT_STRICT, (resource_value_t)0,
      &sfx_soundexpander_enabled, set_sfx_soundexpander_enabled, NULL },
    { "SFXSoundExpanderChip", 0, RES_EVENT_STRICT, (resource_value_t)3526,
      &sfx_soundexpander_chip, set_sfx_soundexpander_chip, NULL },
    { NULL }
};

int sfx_soundexpander_resources_init(void)
{
    return resources_register_int(resources_int);
}

static const cmdline_option_t cmdline_options[] =
{
    { "-sfxse", SET_RESOURCE, 0,
      NULL, NULL, "SFXSoundExpander", (resource_value_t)1,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_ENABLE_SFX_SE,
      NULL, NULL },
    { "+sfxse", SET_RESOURCE, 0,
      NULL, NULL, "SFXSoundExpander", (resource_value_t)0,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_DISABLE_SFX_SE,
      NULL, NULL },
    { "-sfxsetype", SET_RESOURCE, 1,
      NULL, NULL, "SFXSoundExpanderChip", NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_TYPE, IDCLS_SET_YM_CHIP_TYPE,
      NULL, NULL },
    { NULL }
};

int sfx_soundexpander_cmdline_options_init(void)
{
    return cmdline_register_options(cmdline_options);
}

/* ---------------------------------------------------------------------*/

struct sfx_soundexpander_sound_s
{
    BYTE command;
};

static struct sfx_soundexpander_sound_s snd;

int sfx_soundexpander_sound_machine_calculate_samples(sound_t *psid, SWORD *pbuf, int nr, int interleave, int *delta_t)
{
    int i;
    SWORD *buffer;

    if (sid_sound_machine_cycle_based() == 0 && sfx_soundexpander_enabled) {
        buffer = lib_malloc(nr * 2);
        if (sfx_soundexpander_chip == 3812) {
            ym3812_update_one(YM3812_chip, buffer, nr);
        } else {
            ym3526_update_one(YM3526_chip, buffer, nr);
        }

        for (i = 0; i < nr; i++) {
            pbuf[i * interleave] = sound_audio_mix(pbuf[i * interleave], buffer[i]);
        }
        lib_free(buffer);
    }
    return 0;
}

int sfx_soundexpander_sound_machine_init(sound_t *psid, int speed, int cycles_per_sec)
{
    if (sfx_soundexpander_chip == 3812) {
        if (YM3812_chip != NULL) {
            ym3812_shutdown(YM3812_chip);
        }
        YM3812_chip = ym3812_init((UINT32)3579545, (UINT32)speed);
    } else {
        if (YM3526_chip != NULL) {
            ym3526_shutdown(YM3526_chip);
        }
        YM3526_chip = ym3526_init((UINT32)3579545, (UINT32)speed);
    }
    snd.command = 0;

    return 1;
}

void sfx_soundexpander_sound_machine_close(sound_t *psid)
{
    if (YM3526_chip != NULL) {
        ym3526_shutdown(YM3526_chip);
        YM3526_chip = NULL;
    }
    if (YM3812_chip != NULL) {
        ym3812_shutdown(YM3812_chip);
        YM3526_chip = NULL;
    }
}

void sfx_soundexpander_sound_machine_store(sound_t *psid, WORD addr, BYTE val)
{
    snd.command = val;

    if (sfx_soundexpander_chip == 3812) {
        ym3812_write(YM3812_chip, 1, val);
    } else {
        ym3526_write(YM3526_chip, 1, val);
    }
}

BYTE sfx_soundexpander_sound_machine_read(sound_t *psid, WORD addr)
{
    if (sfx_soundexpander_chip == 3812) {
        return ym3812_read(YM3812_chip, 1);
    }
    return ym3526_read(YM3526_chip, 1);
}

void sfx_soundexpander_sound_reset(void)
{
    if (sfx_soundexpander_chip == 3812) {
        ym3812_reset_chip(YM3812_chip);
    } else {
        ym3526_reset_chip(YM3526_chip);
    }
}

/* ---------------------------------------------------------------------*/

void REGPARM2 sfx_soundexpander_sound_address_store(WORD addr, BYTE value)
{
    if (sfx_soundexpander_chip == 3812) {
        ym3812_write(YM3812_chip, 0, value);
    } else {
        ym3526_write(YM3526_chip, 0, value);
    }
}

void REGPARM2 sfx_soundexpander_sound_register_store(WORD addr, BYTE value)
{
    sound_store((WORD)0x60, value, 0);
}

BYTE REGPARM1 sfx_soundexpander_sound_read(WORD addr)
{
  BYTE value;

  io_source = IO_SOURCE_SFX_SE;
  value=sound_read((WORD)0x60, 0);

  return value;
}

/* No piano keyboard is emulated currently, so we return 0xff */
BYTE REGPARM1 sfx_soundexpander_piano_read(WORD addr)
{
  io_source = IO_SOURCE_SFX_SE;

  return (BYTE)0xff;
}
