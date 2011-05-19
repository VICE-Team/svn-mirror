/*
 * digimax.c - Digimax DAC cartridge emulation.
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

#include "c64export.h"
#include "c64io.h"
#include "cartridge.h"
#include "cmdline.h"
#include "digimax.h"
#include "lib.h"
#include "maincpu.h"
#include "resources.h"
#include "sid.h"
#include "snapshot.h"
#include "sound.h"
#include "uiapi.h"
#include "translate.h"

/*
    Digimax Cartridge

    This cartridge is a 8bit 4-channel digital sound output
    interface.

    When inserted into the cart port the cart uses 4 registers,
    one for each channel. The base address can be relocated
    through the entire I/O-1 and I/O-2 range in 0x20 increments.

    TODO: Userport pin description.
*/

/* Flag: Do we enable the external DIGIMAX cartridge?  */
static int digimax_enabled = 0;

/* DIGIMAX address */
int digimax_address;

/* DIGIMAX userport address latch */
static BYTE digimax_userport_address;

/* DIGIMAX userport direction latches */
static BYTE digimax_userport_direction_A;
static BYTE digimax_userport_direction_B;

/* ---------------------------------------------------------------------*/

static void digimax_sound_store(WORD addr, BYTE value);
static BYTE digimax_sound_read(WORD addr);

static io_source_t digimax_device = {
    CARTRIDGE_NAME_DIGIMAX,
    IO_DETACH_RESOURCE,
    "DIGIMAX",
    0xde00, 0xde03, 0x03,
    1, /* read is always valid */
    digimax_sound_store,
    digimax_sound_read,
    digimax_sound_read,
    NULL,
    CARTRIDGE_DIGIMAX,
    0,
    0
};

static io_source_list_t * digimax_list_item = NULL;

static c64export_resource_t export_res = {
    CARTRIDGE_NAME_DIGIMAX, 0, 0, &digimax_device, NULL, CARTRIDGE_DIGIMAX
};

/* ---------------------------------------------------------------------*/

int digimax_cart_enabled(void)
{
    return digimax_enabled;
}

static BYTE digimax_sound_data[4];

static int digimax_is_userport(void)
{
    return (digimax_address == 0xdd00);
}

static void digimax_sound_store(WORD addr, BYTE value)
{
    digimax_sound_data[addr] = value;
    sound_store((WORD)(addr + 0x20), value, 0);
}

static BYTE digimax_sound_read(WORD addr)
{
    BYTE value;

    value = sound_read((WORD)(addr + 0x20), 0);

    return value;
}

/*
    PA2  low, /PA3  low: DAC #0 (left)
    PA2 high, /PA3  low: DAC #1 (right)
    PA2  low, /PA3 high: DAC #2 (left)
    PA2 high, /PA3 high: DAC #3 (right).
*/

static void digimax_userport_sound_store(BYTE value)
{
    WORD addr = 0;

    switch ((digimax_userport_address & digimax_userport_direction_A) & 0xc) {
        case 0x0:
            addr = 2;
            break;
        case 0x4:
            addr = 3;
            break;
        case 0x8:
            addr = 0;
            break;
        case 0xc:
            addr = 1;
            break;
    }

    digimax_sound_store(addr,(BYTE)(value & digimax_userport_direction_B));
}

void digimax_userport_store(WORD addr, BYTE value)
{
    switch (addr & 0x1f) {
        case 0:
            digimax_userport_address = value;
            break;
        case 1:
            if (digimax_enabled && digimax_is_userport()) {
                digimax_userport_sound_store(value);
            }
            break;
        case 2:
            digimax_userport_direction_A = value;
            break;
        case 3:
            digimax_userport_direction_B = value;
            break;
    }
}

/* ---------------------------------------------------------------------*/

static int set_digimax_enabled(int val, void *param)
{
    if (!digimax_enabled && val) {
        if (!digimax_is_userport()) {
            if (c64export_add(&export_res) < 0) {
                return -1;
            }
            digimax_list_item = io_source_register(&digimax_device);
        }
        digimax_enabled = 1;
    } else if (digimax_enabled && !val) {
        if (digimax_list_item != NULL) {
            c64export_remove(&export_res);
            io_source_unregister(digimax_list_item);
            digimax_list_item = NULL;
        }
        digimax_enabled = 0;
    }
    return 0;
}

static int set_digimax_base(int val, void *param)
{
    int old = digimax_enabled;

    if (val == digimax_address) {
        return 0;
    }

    set_digimax_enabled(0, NULL);

    switch (val) {
        case 0xdd00:   /* special case, userport interface */
            break;
        case 0xde00:
        case 0xde20:
        case 0xde40:
        case 0xde60:
        case 0xde80:
        case 0xdea0:
        case 0xdec0:
        case 0xdee0:
            digimax_device.start_address = (WORD)val;
            digimax_device.end_address = (WORD)(val + 3);
            export_res.io1 = &digimax_device;
            export_res.io2 = NULL;
            break;
        case 0xdf00:
        case 0xdf20:
        case 0xdf40:
        case 0xdf60:
        case 0xdf80:
        case 0xdfa0:
        case 0xdfc0:
        case 0xdfe0:
            digimax_device.start_address = (WORD)val;
            digimax_device.end_address = (WORD)(val + 3);
            export_res.io1 = NULL;
            export_res.io2 = &digimax_device;
            break;
        default:
            return -1;
    }

    digimax_address = val;
    set_digimax_enabled(old, NULL);
    return 0;
}

void digimax_reset(void)
{
}

int digimax_enable(void)
{
    return resources_set_int("DIGIMAX", 1);
}
void digimax_detach(void)
{
    resources_set_int("DIGIMAX", 0);
}

/* ---------------------------------------------------------------------*/

static const resource_int_t resources_int[] = {
  { "DIGIMAX", 0, RES_EVENT_STRICT, (resource_value_t)0,
    &digimax_enabled, set_digimax_enabled, NULL },
  { "DIGIMAXbase", 0xde00, RES_EVENT_NO, NULL,
    &digimax_address, set_digimax_base, NULL },
  { NULL }
};

int digimax_resources_init(void)
{
    return resources_register_int(resources_int);
}
void digimax_resources_shutdown(void)
{
}
/* ---------------------------------------------------------------------*/

static const cmdline_option_t cmdline_options[] =
{
    { "-digimax", SET_RESOURCE, 0,
      NULL, NULL, "DIGIMAX", (resource_value_t)1,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_ENABLE_DIGIMAX,
      NULL, NULL },
    { "+digimax", SET_RESOURCE, 0,
      NULL, NULL, "DIGIMAX", (resource_value_t)0,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_DISABLE_DIGIMAX,
      NULL, NULL },
    { "-digimaxbase", SET_RESOURCE, 1,
      NULL, NULL, "DIGIMAXbase", NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_BASE_ADDRESS, IDCLS_DIGIMAX_BASE,
      NULL, NULL },
    { NULL }
};

int digimax_cmdline_options_init(void)
{
    return cmdline_register_options(cmdline_options);
}

/* ---------------------------------------------------------------------*/

struct digimax_sound_s {
    BYTE voice0;
    BYTE voice1;
    BYTE voice2;
    BYTE voice3;
};

static struct digimax_sound_s snd;

int digimax_sound_machine_calculate_samples(sound_t *psid, SWORD *pbuf, int nr, int interleave, int *delta_t)
{
    int i;

    /* FIXME: this should use bandlimited step synthesis. Sadly, VICE does not
     * have an easy-to-use infrastructure for blep generation. We should write
     * this code. */
    if (digimax_enabled) {
        for (i = 0; i < nr; i++) {
            pbuf[i * interleave] = sound_audio_mix(pbuf[i * interleave],((int)snd.voice0) << 6);
            pbuf[i * interleave] = sound_audio_mix(pbuf[i * interleave],((int)snd.voice1) << 6);
            pbuf[i * interleave] = sound_audio_mix(pbuf[i * interleave],((int)snd.voice2) << 6);
            pbuf[i * interleave] = sound_audio_mix(pbuf[i * interleave],((int)snd.voice3) << 6);
        }
    }
    return 0;
}

int digimax_sound_machine_init(sound_t *psid, int speed, int cycles_per_sec)
{
    snd.voice0 = 0;
    snd.voice1 = 0;
    snd.voice2 = 0;
    snd.voice3 = 0;

    return 1;
}

void digimax_sound_machine_store(sound_t *psid, WORD addr, BYTE val)
{
    switch (addr & 3) {
        case 0:
            snd.voice0 = val;
            break;
        case 1:
            snd.voice1 = val;
            break;
        case 2:
            snd.voice2 = val;
            break;
        case 3:
            snd.voice3 = val;
            break;
    }
}

BYTE digimax_sound_machine_read(sound_t *psid, WORD addr)
{
    return digimax_sound_data[addr&3];
}

void digimax_sound_reset(void)
{
    snd.voice0 = 0;
    snd.voice1 = 0;
    snd.voice2 = 0;
    snd.voice3 = 0;
    digimax_sound_data[0] = 0;
    digimax_sound_data[1] = 0;
    digimax_sound_data[2] = 0;
    digimax_sound_data[3] = 0;
}

/* ---------------------------------------------------------------------*/

#define CART_DUMP_VER_MAJOR   0
#define CART_DUMP_VER_MINOR   0
#define SNAP_MODULE_NAME  "CARTDIGIMAX"

int digimax_snapshot_write_module(snapshot_t *s)
{
    snapshot_module_t *m;

    m = snapshot_module_create(s, SNAP_MODULE_NAME,
                          CART_DUMP_VER_MAJOR, CART_DUMP_VER_MINOR);
    if (m == NULL) {
        return -1;
    }

    if (0
        || (SMW_DW(m, (DWORD)digimax_address) < 0)
        || (SMW_B(m, digimax_userport_address) < 0)
        || (SMW_B(m, digimax_userport_direction_A) < 0)
        || (SMW_B(m, digimax_userport_direction_B) < 0)
        || (SMW_BA(m, digimax_sound_data, 4) < 0)
        || (SMW_B(m, snd.voice0) < 0)
        || (SMW_B(m, snd.voice1) < 0)
        || (SMW_B(m, snd.voice2) < 0)
        || (SMW_B(m, snd.voice3) < 0)) {
        snapshot_module_close(m);
        return -1;
    }

    snapshot_module_close(m);
    return 0;
}

int digimax_snapshot_read_module(snapshot_t *s)
{
    BYTE vmajor, vminor;
    snapshot_module_t *m;
    int temp_digimax_address;

    m = snapshot_module_open(s, SNAP_MODULE_NAME, &vmajor, &vminor);
    if (m == NULL) {
        return -1;
    }

    if ((vmajor != CART_DUMP_VER_MAJOR) || (vminor != CART_DUMP_VER_MINOR)) {
        snapshot_module_close(m);
        return -1;
    }

    if (0
        || (SMR_DW_INT(m, &temp_digimax_address) < 0)
        || (SMR_B(m, &digimax_userport_address) < 0)
        || (SMR_B(m, &digimax_userport_direction_A) < 0)
        || (SMR_B(m, &digimax_userport_direction_B) < 0)
        || (SMR_BA(m, digimax_sound_data, 4) < 0)
        || (SMR_B(m, &snd.voice0) < 0)
        || (SMR_B(m, &snd.voice1) < 0)
        || (SMR_B(m, &snd.voice2) < 0)
        || (SMR_B(m, &snd.voice3) < 0)) {
        snapshot_module_close(m);
        return -1;
    }

    snapshot_module_close(m);

    /* HACK set address to an invalid value, then use the function */
    digimax_address = -1;
    set_digimax_base(temp_digimax_address, NULL);

    return digimax_enable();
}
