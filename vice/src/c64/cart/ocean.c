/*
 * ocean.c - Cartridge handling, Ocean cart.
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
#include <string.h>

#include "c64cart.h"
#include "c64cartmem.h"
#include "c64export.h"
#include "c64io.h"
#include "c64mem.h"
#include "cartridge.h"
#include "ocean.h"
#include "snapshot.h"
#include "types.h"
#include "util.h"

/*
    "Ocean" Game Cartridge

    This type of cart comes in 4 sizes: 32Kb, 128Kb, 256Kb, 512Kb.

    The 32Kb type of cart has 4 banks, banked in at $8000-$9FFF.

    The 128Kb type of cart has 16 banks, banked in at $8000-$9FFF.

    The 256Kb type of cart has 32 banks, 16 banked in at $8000-$9FFF,
    and 16 banked in at $A000-$BFFF.

    The 512Kb type of cart has 64 banks, banked in at $8000-$9FFF.

    Bank switching is done by writing to $DE00. The lower six bits give the
    bank number (ranging from 0-63). Bit 8 in this selection word is always
    set.
*/

static int currbank = 0;

static void REGPARM2 ocean_io1_store(WORD addr, BYTE value)
{
    /* FIXME */
    cartridge_romhbank_set(value & 0x3f);
    cartridge_romlbank_set(value & 0x3f);
    export.game = export.exrom = 1;
    mem_pla_config_changed();
    export.ultimax_phi1 = 0;
    export.ultimax_phi2 = 0;
    currbank = value & 0x3f;
}

static BYTE REGPARM1 ocean_io1_peek(WORD addr)
{
    return currbank;
}
/* ---------------------------------------------------------------------*/

static io_source_t ocean_device = {
    "Ocean",
    IO_DETACH_CART,
    NULL,
    0xde00, 0xdeff, 0xff,
    0,
    ocean_io1_store,
    NULL,
    ocean_io1_peek,
    NULL, /* dump */
    CARTRIDGE_OCEAN
};

static io_source_list_t *ocean_list_item = NULL;

static const c64export_resource_t export_res = {
    "Ocean", 1, 1, &ocean_device, NULL, CARTRIDGE_OCEAN
};

/* ---------------------------------------------------------------------*/

BYTE REGPARM1 ocean_romh_read(WORD addr)
{
    /* 256 kB OCEAN carts may access memory either at $8000 or $a000 */
    return roml_banks[(addr & 0x1fff) + (romh_bank << 13)];
}

void ocean_config_init(void)
{
    cartridge_config_changed(1, 1, CMODE_READ);
    ocean_io1_store((WORD)0xde00, 0);
}

void ocean_config_setup(BYTE *rawcart)
{
    memcpy(roml_banks, rawcart, 0x2000 * 64);
    memcpy(romh_banks, &rawcart[0x2000 * 16], 0x2000 * 16);

    /* Hack: using 16kB configuration, but some carts are 8kB only */
    cartridge_config_changed(1, 1, CMODE_READ);
}

/* ---------------------------------------------------------------------*/

static int ocean_common_attach(void)
{
    if (c64export_add(&export_res) < 0) {
        return -1;
    }
    ocean_list_item = c64io_register(&ocean_device);
    return 0;
}

int ocean_bin_attach(const char *filename, BYTE *rawcart)
{
    if (util_file_load(filename, rawcart, 0x80000, UTIL_FILE_LOAD_SKIP_ADDRESS) < 0) {
        if (util_file_load(filename, rawcart, 0x40000, UTIL_FILE_LOAD_SKIP_ADDRESS) < 0) {
            if (util_file_load(filename, rawcart, 0x20000, UTIL_FILE_LOAD_SKIP_ADDRESS) < 0) {
                if (util_file_load(filename, rawcart, 0x8000, UTIL_FILE_LOAD_SKIP_ADDRESS) < 0) {
                    return -1;
                }
            }
        }
    }
    return ocean_common_attach();
}

int ocean_crt_attach(FILE *fd, BYTE *rawcart)
{
    BYTE chipheader[0x10];

    while (1) {
        if (fread(chipheader, 0x10, 1, fd) < 1) {
            break;
        }
        if (chipheader[0xb] >= 64 || (chipheader[0xc] != 0x80 && chipheader[0xc] != 0xa0)) {
            return -1;
        }
        if (fread(&rawcart[chipheader[0xb] << 13], 0x2000, 1, fd) < 1) {
            return -1;
        }
    }
    return ocean_common_attach();
}

void ocean_detach(void)
{
    c64export_remove(&export_res);
    c64io_unregister(ocean_list_item);
    ocean_list_item = NULL;
}

/* ---------------------------------------------------------------------*/

#define CART_DUMP_VER_MAJOR   0
#define CART_DUMP_VER_MINOR   0
#define SNAP_MODULE_NAME  "CARTOCEAN"

int ocean_snapshot_write_module(snapshot_t *s)
{
    snapshot_module_t *m;

    m = snapshot_module_create(s, SNAP_MODULE_NAME,
                          CART_DUMP_VER_MAJOR, CART_DUMP_VER_MINOR);
    if (m == NULL) {
        return -1;
    }

    if (0
        || (SMW_B(m, (BYTE)currbank) < 0)
        || (SMW_BA(m, roml_banks, 0x2000 * 64) < 0)
        || (SMW_BA(m, romh_banks, 0x2000 * 16) < 0)) {
        snapshot_module_close(m);
        return -1;
    }

    snapshot_module_close(m);
    return 0;
}

int ocean_snapshot_read_module(snapshot_t *s)
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
        || (SMR_B_INT(m, &currbank) < 0)
        || (SMR_BA(m, roml_banks, 0x2000 * 64) < 0)
        || (SMR_BA(m, romh_banks, 0x2000 * 16) < 0)) {
        snapshot_module_close(m);
        return -1;
    }

    snapshot_module_close(m);

    return ocean_common_attach();
}
