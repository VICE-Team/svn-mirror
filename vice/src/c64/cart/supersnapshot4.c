/*
 * supersnapshot4.c - Cartridge handling, Super Snapshot cart.
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
 *  Nathan Huizinga <nathan.huizinga@chess.nl>
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
#include <string.h>

#include "c64cart.h"
#include "c64cartmem.h"
#include "c64export.h"
#include "c64io.h"
#include "cartridge.h"
#include "snapshot.h"
#include "supersnapshot4.h"
#include "types.h"
#include "util.h"

/*
    Super Snapshot v4

    - 32K ROM,4*8K Banks

    io1: (read/write)
        cart ram

    io2 (read)
     df01 - ram config
          - cart rom
    io2 (write)
     df00 -
     df01 -
*/

/* Super Snapshot configuration flags.  */
static BYTE ramconfig = 0xff, romconfig = 9;

/* ---------------------------------------------------------------------*/

/* some prototypes are needed */
static BYTE REGPARM1 supersnapshot_v4_io1_read(WORD addr);
static void REGPARM2 supersnapshot_v4_io1_store(WORD addr, BYTE value);
static BYTE REGPARM1 supersnapshot_v4_io2_read(WORD addr);
static void REGPARM2 supersnapshot_v4_io2_store(WORD addr, BYTE value);

static io_source_t ss4_io1_device = {
    "Super Snapshot 4",
    IO_DETACH_CART,
    NULL,
    0xde00, 0xdeff, 0xff,
    1, /* read is always valid */
    supersnapshot_v4_io1_store,
    supersnapshot_v4_io1_read,
    NULL,
    NULL,
    CARTRIDGE_SUPER_SNAPSHOT
};

static io_source_t ss4_io2_device = {
    "Super Snapshot 4",
    IO_DETACH_CART,
    NULL,
    0xdf00, 0xdfff, 0xff,
    0,
    supersnapshot_v4_io2_store,
    supersnapshot_v4_io2_read,
    NULL,
    NULL,
    CARTRIDGE_SUPER_SNAPSHOT
};

static io_source_list_t *ss4_io1_list_item = NULL;
static io_source_list_t *ss4_io2_list_item = NULL;


static const c64export_resource_t export_res_v4 = {
    "Super Snapshot V4", 1, 1, &ss4_io1_device, &ss4_io2_device, CARTRIDGE_SUPER_SNAPSHOT
};

/* ---------------------------------------------------------------------*/

BYTE REGPARM1 supersnapshot_v4_io1_read(WORD addr)
{
    return export_ram0[0x1e00 + (addr & 0xff)];
}

void REGPARM2 supersnapshot_v4_io1_store(WORD addr, BYTE value)
{
    export_ram0[0x1e00 + (addr & 0xff)] = value;
}

BYTE REGPARM1 supersnapshot_v4_io2_read(WORD addr)
{
    ss4_io2_device.io_source_valid = 1;

    if ((addr & 0xff) == 1) {
        return ramconfig;
    }

    addr |= 0xdf00;

    switch (roml_bank) {
        case 0:
            return roml_banks[addr & 0x1fff];
        case 1:
            return roml_banks[(addr & 0x1fff) + 0x2000];
        case 2:
            return roml_banks[(addr & 0x1fff) + 0x4000];
        case 3:
            return roml_banks[(addr & 0x1fff) + 0x6000];
    }
    ss4_io2_device.io_source_valid = 0;
    return 0;
}

/* FIXME: this one is odd, it probably doesnt quite do what really happens */
void REGPARM2 supersnapshot_v4_io2_store(WORD addr, BYTE value)
{
    if ((addr & 0xff) == 0) {
        int mode = CMODE_WRITE;

        romconfig = (BYTE)((value == 2) ? 1 : (1 | (1 << CMODE_BANK_SHIFT)));
        mode = mode | ((ramconfig == 0) ? CMODE_EXPORT_RAM : 0);
        if ((value & 0x7f) == 0) {
            romconfig = 3;
            mode |= CMODE_EXPORT_RAM;
        }
        if ((value & 0x7f) == 1 || (value & 0x7f) == 3) {
            romconfig = 0;
        }
        if ((value & 0x7f) == 6) {
            romconfig = 1 | (1 << CMODE_BANK_SHIFT);
            mode |= CMODE_RELEASE_FREEZE;
        }
        if ((value & 0x7f) == 9) {
            romconfig = 2; /* exrom */
            mode |= CMODE_PHI2_RAM;
        }
        cartridge_config_changed((BYTE)(romconfig & 3), romconfig, mode);
    }
    if ((addr & 0xff) == 1) {
        int mode = CMODE_WRITE;
        if (((ramconfig - 1) & 0xff) == value) {
            ramconfig = value;
            romconfig |= 3; /* game,exrom */
            mode |= CMODE_EXPORT_RAM;
        }
        if (((ramconfig + 1) & 0xff) == value) {
            ramconfig = value;
            romconfig &= ~(1 << 1); /* exrom */
            mode &= ~(CMODE_EXPORT_RAM);
        }
        cartridge_config_changed((BYTE)(romconfig & 3), romconfig, mode);
    }
}

/* ---------------------------------------------------------------------*/

BYTE REGPARM1 supersnapshot_v4_roml_read(WORD addr)
{
    if (export_ram) {
        return export_ram0[addr & 0x1fff];
    }

    return roml_banks[(addr & 0x1fff) + (roml_bank << 13)];
}

void REGPARM2 supersnapshot_v4_roml_store(WORD addr, BYTE value)
{
    if (export_ram) {
        export_ram0[addr & 0x1fff] = value;
    }
}

/* ---------------------------------------------------------------------*/

void supersnapshot_v4_freeze(void)
{
    cartridge_config_changed(3, 3, CMODE_READ | CMODE_EXPORT_RAM);
}

void supersnapshot_v4_config_init(void)
{
    cartridge_config_changed(1 | (1 << CMODE_BANK_SHIFT), 1 | (1 << CMODE_BANK_SHIFT), CMODE_READ);
}

void supersnapshot_v4_config_setup(BYTE *rawcart)
{
    memcpy(&roml_banks[0x0000], &rawcart[0x0000], 0x2000);
    memcpy(&romh_banks[0x0000], &rawcart[0x2000], 0x2000);
    memcpy(&roml_banks[0x2000], &rawcart[0x4000], 0x2000);
    memcpy(&romh_banks[0x2000], &rawcart[0x6000], 0x2000);
    cartridge_config_changed(1 | (1 << CMODE_BANK_SHIFT), 1 | (1 << CMODE_BANK_SHIFT), CMODE_READ);
}

/* ---------------------------------------------------------------------*/

static int supersnapshot_v4_common_attach(void)
{
    if (c64export_add(&export_res_v4) < 0) {
        return -1;
    }
    ss4_io1_list_item = c64io_register(&ss4_io1_device);
    ss4_io2_list_item = c64io_register(&ss4_io2_device);
    return 0;
}

int supersnapshot_v4_bin_attach(const char *filename, BYTE *rawcart)
{
    if (util_file_load(filename, rawcart, 0x8000, UTIL_FILE_LOAD_SKIP_ADDRESS) < 0) {
        return -1;
    }
    return supersnapshot_v4_common_attach();
}

int supersnapshot_v4_crt_attach(FILE *fd, BYTE *rawcart)
{
    int i = 4;
    BYTE chipheader[0x10];

    while (i--) {
        if (fread(chipheader, 0x10, 1, fd) < 1) {
            return -1;
        }

        if (chipheader[0xb] > 3) {
            return -1;
        }

        if (fread(&rawcart[chipheader[0xb] << 13], 0x2000, 1, fd) < 1) {
            return -1;
        }
    }

    return supersnapshot_v4_common_attach();
}

void supersnapshot_v4_detach(void)
{
    c64export_remove(&export_res_v4);
    c64io_unregister(ss4_io1_list_item);
    c64io_unregister(ss4_io2_list_item);
    ss4_io1_list_item = NULL;
    ss4_io2_list_item = NULL;
}

/* ---------------------------------------------------------------------*/

#define CART_DUMP_VER_MAJOR   0
#define CART_DUMP_VER_MINOR   0
#define SNAP_MODULE_NAME  "CARTSS4"

int supersnapshot_v4_snapshot_write_module(snapshot_t *s)
{
    snapshot_module_t *m;

    m = snapshot_module_create(s, SNAP_MODULE_NAME,
                          CART_DUMP_VER_MAJOR, CART_DUMP_VER_MINOR);
    if (m == NULL) {
        return -1;
    }

    if (0
        || (SMW_B(m, ramconfig) < 0)
        || (SMW_B(m, romconfig) < 0)
        || (SMW_BA(m, roml_banks, 0x4000) < 0)
        || (SMW_BA(m, romh_banks, 0x4000) < 0)
        || (SMW_BA(m, export_ram0, 0x2000) < 0)) {
        snapshot_module_close(m);
        return -1;
    }

    snapshot_module_close(m);
    return 0;
}

int supersnapshot_v4_snapshot_read_module(snapshot_t *s)
{
    BYTE vmajor, vminor;
    snapshot_module_t *m;

    m = snapshot_module_open(s, SNAP_MODULE_NAME, &vmajor, &vminor);
    if (m == NULL) {
        return -1;
    }

    if ((vmajor != CART_DUMP_VER_MAJOR) || (vminor != CART_DUMP_VER_MINOR)) {
        snapshot_module_close(m);
        return -1;
    }

    if (0
        || (SMR_B(m, &ramconfig) < 0)
        || (SMR_B(m, &romconfig) < 0)
        || (SMR_BA(m, roml_banks, 0x4000) < 0)
        || (SMR_BA(m, romh_banks, 0x4000) < 0)
        || (SMR_BA(m, export_ram0, 0x2000) < 0)) {
        snapshot_module_close(m);
        return -1;
    }

    snapshot_module_close(m);

    return supersnapshot_v4_common_attach();
}
