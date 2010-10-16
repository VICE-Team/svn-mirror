/*
 * freezemachine.c - Cartridge handling, Freeze Machine cart.
 *
 * Written by
 *  Groepaz <groepaz@gmx.net>
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

#include "c64cart.h"
#include "c64cartmem.h"
#include "c64export.h"
#include "c64io.h"
#include "c64mem.h"
#include "cartridge.h"
#include "freezemachine.h"
#include "types.h"
#include "util.h"

/* #define FMDEBUG */

#ifdef FMDEBUG
#define DBG(x) printf x
#else
#define DBG(x)
#endif

/*
    FIXME: this implementation is based on vague guesses on how the hardware
           really works. remaining problems are:
           - fastloader fails

    Evesham Micros "Freeze Frame MK4"

    - 2 Buttons (Freeze, Reset)
    - 16k ROM

    Evesham Micros "Freeze Machine"

    - 2 Buttons (Freeze, Reset)
    - 7474, 74163, 2 * 7400, 7402
    - 32k ROM, splitted into 2 16K parts of which 2 8k banks are used at once

    - reading io1 (the software uses de00 only it seems)
      - switches to 16k game mode
      - ROM bank 0 (2) is mapped to 8000
      - ROM bank 1 (3) is mapped to A000

    - reading io2 (the software uses df00 only it seems)
      - disables cartridge ROM

    - reset
      - enables 8K game mode
      - toggles a flipflop which selects wether the upper or lower part of the
        32k ROM is selected. (Freeze Machine only)
      - ROM bank 0 (2) is mapped to 8000

    - freeze
      - enables ultimax mode
      - ROM bank 0 (2) is mapped to 8000
      - ROM bank 0 (2) is mapped to E000
*/

#define FREEZE_MACHINE_CART_SIZE (32*0x400)
#define FREEZE_FRAME_MK4_CART_SIZE (16*0x400)

static int rom_A14;     /* when set, bank 2 and 3 are active */
static int roml_toggle; /* when set, bank 1 or 3 will be used for roml */

/* ---------------------------------------------------------------------*/

static BYTE REGPARM1 freezemachine_io1_read(WORD addr)
{
    DBG(("io1 r %04x\n", addr));
    if (addr == 0) {
        roml_toggle = 1;
        cartridge_config_changed(2, (BYTE)(1 | (rom_A14 << CMODE_BANK_SHIFT)), CMODE_READ);
        DBG(("Freeze Machine: switching to 16k game mapping\n"));
    }
    return 0; /* invalid */
}

static void REGPARM2 freezemachine_io1_store(WORD addr, BYTE value)
{
    DBG(("io1 %04x %02x\n", addr, value));
}

static BYTE REGPARM1 freezemachine_io2_read(WORD addr)
{
    DBG(("io2 r %04x\n", addr));
    if (addr == 0) {
        cartridge_config_changed(2, 2, CMODE_READ);
        DBG(("Freeze Machine: disabled\n"));
    }

    return 0; /* invalid */
}

static void REGPARM2 freezemachine_io2_store(WORD addr, BYTE value)
{
    DBG(("io2 %04x %02x\n", addr, value));
}

static io_source_t freezemachine_io1_device = {
    "Freeze Machine",
    IO_DETACH_CART,
    NULL,
    0xde00, 0xdeff, 0xff,
    0, /* read is never valid */
    freezemachine_io1_store,
    freezemachine_io1_read,
    NULL,
    NULL,
    CARTRIDGE_FREEZE_MACHINE
};
static io_source_t freezemachine_io2_device = {
    "Freeze Machine",
    IO_DETACH_CART,
    NULL,
    0xdf00, 0xdfff, 0xff,
    0, /* read is never valid */
    freezemachine_io2_store,
    freezemachine_io2_read,
    NULL,
    NULL,
    CARTRIDGE_FREEZE_MACHINE
};

static io_source_list_t *freezemachine_io1_list_item = NULL;
static io_source_list_t *freezemachine_io2_list_item = NULL;

static const c64export_resource_t export_res = {
    "Freeze Machine", 1, 1, &freezemachine_io1_device, &freezemachine_io2_device, CARTRIDGE_FREEZE_MACHINE
};

/* ---------------------------------------------------------------------*/

BYTE REGPARM1 freezemachine_roml_read(WORD addr)
{
    if (roml_toggle) {
        return romh_banks[(addr & 0x1fff) | (rom_A14 << 13)];
    } else {
        return roml_banks[(addr & 0x1fff) | (rom_A14 << 13)];
    }
}

/* ---------------------------------------------------------------------*/

void freezemachine_reset(void)
{
    rom_A14 ^= 1; /* select other 16k ROM bank on every other reset */
    roml_toggle = 0;
    cartridge_config_changed(2, (BYTE)(0 | (rom_A14 << CMODE_BANK_SHIFT)), CMODE_READ);
    DBG(("Freeze Machine: reset (%d)\n", rom_A14));
}

void freezemachine_freeze(void)
{
    DBG(("Freeze Machine: freeze\n"));
    roml_toggle = 1;
    cartridge_config_changed(2, (BYTE)(3 | (rom_A14 << CMODE_BANK_SHIFT)), CMODE_READ | CMODE_RELEASE_FREEZE);
}

void freezemachine_config_init(void)
{
    cartridge_config_changed(2, (BYTE)(0 | (rom_A14 << CMODE_BANK_SHIFT)), CMODE_READ);
}

void freezemachine_config_setup(BYTE *rawcart)
{
    rom_A14 = 1; /* the following first reset will turn it to 0 again */
    roml_toggle = 0;
    memcpy(roml_banks, rawcart, FREEZE_MACHINE_CART_SIZE);
    memcpy(romh_banks, &rawcart[0x2000], FREEZE_MACHINE_CART_SIZE);
    memcpy(&roml_banks[0x2000], &rawcart[0x4000], FREEZE_MACHINE_CART_SIZE);
    memcpy(&romh_banks[0x2000], &rawcart[0x6000], FREEZE_MACHINE_CART_SIZE);
    cartridge_config_changed(2, 0 | (0 << CMODE_BANK_SHIFT), CMODE_READ);
}

/* ---------------------------------------------------------------------*/

static int freezemachine_common_attach(void)
{
    if (c64export_add(&export_res) < 0) {
        return -1;
    }

    freezemachine_io1_list_item = c64io_register(&freezemachine_io1_device);
    freezemachine_io2_list_item = c64io_register(&freezemachine_io2_device);

    return 0;
}

int freezemachine_bin_attach(const char *filename, BYTE *rawcart)
{
    DBG(("Freeze Machine: bin attach '%s'\n", filename));
    if (util_file_load(filename, rawcart, FREEZE_MACHINE_CART_SIZE, UTIL_FILE_LOAD_SKIP_ADDRESS) < 0) {
        if (util_file_load(filename, rawcart, FREEZE_FRAME_MK4_CART_SIZE, UTIL_FILE_LOAD_SKIP_ADDRESS) < 0) {
            return -1;
        }
        memcpy(&rawcart[FREEZE_FRAME_MK4_CART_SIZE], rawcart, FREEZE_FRAME_MK4_CART_SIZE);
    }

    return freezemachine_common_attach();
}

int freezemachine_crt_attach(FILE *fd, BYTE *rawcart)
{
    int i;
    BYTE chipheader[0x10];

    for (i = 0; i < 4; i++) {

        if (fread(chipheader, 0x10, 1, fd) < 1) {
            break;
        }
/*
        if (chipheader[0xb] > 0) {
            return -1;
        }
*/
        if (fread(&rawcart[0x2000 * i], 0x2000, 1, fd) < 1) {
            break;
        }
    }

    if (!((i == 2) || (i == 4))) {
        return -1;
    }

    return freezemachine_common_attach();
}

void freezemachine_detach(void)
{
    c64export_remove(&export_res);
    c64io_unregister(freezemachine_io1_list_item);
    c64io_unregister(freezemachine_io2_list_item);
    freezemachine_io1_list_item = NULL;
    freezemachine_io2_list_item = NULL;
}

