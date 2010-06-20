/*
 * supersnapshot.c - Cartridge handling, Super Snapshot cart.
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
 *  Nathan Huizinga <nathan.huizinga@chess.nl>
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
#include <string.h>

#include "c64cart.h"
#include "c64cartmem.h"
#include "c64export.h"
#include "c64io.h"
#include "cartridge.h"
#include "supersnapshot.h"
#include "types.h"
#include "util.h"

/*
    Super Snapshot v5

    - 32K ROM,4*8K Banks
    - 32K RAM,4*8K Banks

    io1: (read)
        cart rom

    io1 (write)

    df00/df01:

    bit 4 - rom/ram bank bit1
    bit 3 - rom enable
    bit 2 - rom/ram bank bit0 
    bit 1 - ram enable, EXROM
    bit 0 - release freeze, !GAME
*/

/* Super Snapshot configuration flags.  */
static BYTE romconfig = 9;
static int ram_bank = 0; /* Version 5 supports 4 - 8Kb RAM banks. */

/* ---------------------------------------------------------------------*/

/* some prototypes are needed */
static BYTE REGPARM1 supersnapshot_v5_io1_read(WORD addr);
static void REGPARM2 supersnapshot_v5_io1_store(WORD addr, BYTE value);

static io_source_t ss5_device = {
    "SUPER SNAPSHOT 5",
    IO_DETACH_CART,
    NULL,
    0xde00, 0xdeff, 0xff,
    0,
    supersnapshot_v5_io1_store,
    supersnapshot_v5_io1_read,
    NULL, /* TODO: peek */
    NULL, /* TODO: dump */
    CARTRIDGE_SUPER_SNAPSHOT_V5
};

static io_source_list_t *ss5_list_item = NULL;

static const c64export_resource_t export_res_v5 = {
    "Super Snapshot V5", 1, 1, &ss5_device, NULL, CARTRIDGE_SUPER_SNAPSHOT_V5
};

/* ---------------------------------------------------------------------*/

BYTE REGPARM1 supersnapshot_v5_io1_read(WORD addr)
{
    ss5_device.io_source_valid = 1;
    switch (roml_bank) {
        case 0:
            return roml_banks[0x1e00 + (addr & 0xff)];
        case 1:
            return roml_banks[0x1e00 + (addr & 0xff) + 0x2000];
        case 2:
            return roml_banks[0x1e00 + (addr & 0xff) + 0x4000];
        case 3:
            return roml_banks[0x1e00 + (addr & 0xff) + 0x6000];
    }
    ss5_device.io_source_valid = 0;
    return 0;
}

void REGPARM2 supersnapshot_v5_io1_store(WORD addr, BYTE value)
{
    if (((addr & 0xff) == 0) || ((addr & 0xff) == 1)) {
        int banknr, mode = CMODE_WRITE;

        if ((value & 1) == 1) {
            mode |= CMODE_RELEASE_FREEZE;
        }

        /* D0 = ~GAME */
        romconfig = ((value & 1) ^ 1);

        /* Calc RAM/ROM bank nr. */
        banknr = ((value >> 2) & 0x1) | ((value >> 3) & 0x2);

        /* ROM ~OE set? */
        if (((value >> 3) & 1) == 0) {
            romconfig |= (banknr << CMODE_BANK_SHIFT); /* Select ROM banknr. */
        }

        /* RAM ~OE set? */
        if (((value >> 1) & 1) == 0) {
            ram_bank = banknr;          /* Select RAM banknr. */
            mode |= CMODE_EXPORT_RAM;   /* export_ram */
            romconfig |= (1 << 1);      /* exrom */
        }
        cartridge_config_changed(1, romconfig, mode);
    }
}

/* ---------------------------------------------------------------------*/

BYTE REGPARM1 supersnapshot_v5_roml_read(WORD addr)
{
    if (export_ram) {
        return export_ram0[(addr & 0x1fff) + (ram_bank << 13)];
    }

    return roml_banks[(addr & 0x1fff) + (roml_bank << 13)];
}

void REGPARM2 supersnapshot_v5_roml_store(WORD addr, BYTE value)
{
    if (export_ram) {
        export_ram0[(addr & 0x1fff) + (ram_bank << 13)] = value;
    }
}

/* ---------------------------------------------------------------------*/

void supersnapshot_v5_freeze(void)
{
    cartridge_config_changed(3, 3, CMODE_READ | CMODE_EXPORT_RAM);
}

void supersnapshot_v5_config_init(void)
{
    supersnapshot_v5_io1_store((WORD)0xde00, 2);
}

void supersnapshot_v5_config_setup(BYTE *rawcart)
{
    memcpy(&roml_banks[0x0000], &rawcart[0x0000], 0x2000);
    memcpy(&romh_banks[0x0000], &rawcart[0x2000], 0x2000);
    memcpy(&roml_banks[0x2000], &rawcart[0x4000], 0x2000);
    memcpy(&romh_banks[0x2000], &rawcart[0x6000], 0x2000);
    memcpy(&roml_banks[0x4000], &rawcart[0x8000], 0x2000);
    memcpy(&romh_banks[0x4000], &rawcart[0xa000], 0x2000);
    memcpy(&roml_banks[0x6000], &rawcart[0xc000], 0x2000);
    memcpy(&romh_banks[0x6000], &rawcart[0xe000], 0x2000);
    supersnapshot_v5_io1_store((WORD)0xde00, 2);
}

/* ---------------------------------------------------------------------*/

static int supersnapshot_v5_common_attach(void)
{
    if (c64export_add(&export_res_v5) < 0) {
        return -1;
    }

    ss5_list_item = c64io_register(&ss5_device);

    return 0;
}

int supersnapshot_v5_bin_attach(const char *filename, BYTE *rawcart)
{
    if (util_file_load(filename, rawcart, 0x10000, UTIL_FILE_LOAD_SKIP_ADDRESS) < 0) {
        return -1;
    }

    return supersnapshot_v5_common_attach();
}

int supersnapshot_v5_crt_attach(FILE *fd, BYTE *rawcart)
{
    BYTE chipheader[0x10];
    int i;

    for (i = 0; i < 4; i++) {
        if (fread(chipheader, 0x10, 1, fd) < 1) {
            return -1;
        }

        if (chipheader[0xc] != 0x80 || chipheader[0xe] != 0x40 || chipheader[0xb] > 3 || fread(rawcart + 0x4000 * chipheader[0xb], chipheader[0xe] << 8, 1, fd) < 1) {
            return -1;
        }
    }

    return supersnapshot_v5_common_attach();
}

void supersnapshot_v5_detach(void)
{
    c64export_remove(&export_res_v5);
    c64io_unregister(ss5_list_item);
    ss5_list_item = NULL;
}
