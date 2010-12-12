/*
 * kcs.c - Cartridge handling, KCS cart.
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

#include "c64cart.h"
#include "c64cartmem.h"
#include "c64export.h"
#include "c64io.h"
#include "cartridge.h"
#include "kcs.h"
#include "snapshot.h"
#include "types.h"
#include "util.h"

/*
    KCS Power Cartridge

    - 16kb ROM, 128 bytes RAM

    io1:
    - the second last page of the first 8k bank is visible
    - when reading, bit 1 of the address selects mapping mode:
      0 : 8k game
      1 : cartridge disabled

    - when writing, 16k game mode is selected

    io2:
    - cartridge RAM
    - when reading, if bit 7 of the address is set freeze mode
      is released and ultimax mapping selected

    - when writing, 16k game mode is selected (?)
    - writes go to cartridge RAM 
*/

static BYTE REGPARM1 kcs_io1_read(WORD addr)
{
    BYTE config;

    /* A1 switches off roml/romh banks */
    config = (addr & 2) ? 2 : 0;

    cartridge_config_changed(config, config, CMODE_READ);
    return roml_banks[0x1e00 + (addr & 0xff)];
}

static BYTE REGPARM1 kcs_io1_peek(WORD addr)
{
    return roml_banks[0x1e00 + (addr & 0xff)];
}

static void REGPARM2 kcs_io1_store(WORD addr, BYTE value)
{
    cartridge_config_changed(1, 1, CMODE_WRITE);
}

static BYTE REGPARM1 kcs_io2_read(WORD addr)
{
    if (addr & 0x80) {
        cartridge_config_changed(3, 3, CMODE_READ | CMODE_RELEASE_FREEZE);
    }
    return export_ram0[0x1f00 + (addr & 0x7f)];
}

static BYTE REGPARM1 kcs_io2_peek(WORD addr)
{
    return export_ram0[0x1f00 + (addr & 0x7f)];
}

static void REGPARM2 kcs_io2_store(WORD addr, BYTE value)
{
    if (!export.ultimax_phi2) { /* FIXME */
        cartridge_config_changed(1, 1, CMODE_WRITE);
    }
    export_ram0[0x1f00 + (addr & 0x7f)] = value;
}

/* ---------------------------------------------------------------------*/

static io_source_t kcs_io1_device = {
    "KCS POWER",
    IO_DETACH_CART,
    NULL,
    0xde00, 0xdeff, 0xff,
    1, /* read is always valid */
    kcs_io1_store,
    kcs_io1_read,
    kcs_io1_peek,
    NULL,
    CARTRIDGE_KCS_POWER
};

static io_source_t kcs_io2_device = {
    "KCS POWER",
    IO_DETACH_CART,
    NULL,
    0xdf00, 0xdfff, 0xff,
    1, /* read is always valid */
    kcs_io2_store,
    kcs_io2_read,
    kcs_io2_peek,
    NULL,
    CARTRIDGE_KCS_POWER
};

static io_source_list_t *kcs_io1_list_item = NULL;
static io_source_list_t *kcs_io2_list_item = NULL;

static const c64export_resource_t export_res_kcs = {
    "KCS Power", 1, 1, &kcs_io1_device, &kcs_io2_device, CARTRIDGE_KCS_POWER
};

/* ---------------------------------------------------------------------*/

void kcs_freeze(void)
{
    cartridge_config_changed(3, 3, CMODE_READ);
}

void kcs_config_init(void)
{
    cartridge_config_changed(0, 0, CMODE_READ);
}

void kcs_config_setup(BYTE *rawcart)
{
    memcpy(roml_banks, rawcart, 0x2000);
    memcpy(romh_banks, &rawcart[0x2000], 0x2000);
    cartridge_config_changed(0, 0, CMODE_READ);
}

/* ---------------------------------------------------------------------*/

static int kcs_common_attach(void)
{
    if (c64export_add(&export_res_kcs) < 0) {
        return -1;
    }

    kcs_io1_list_item = c64io_register(&kcs_io1_device);
    kcs_io2_list_item = c64io_register(&kcs_io2_device);
    return 0;
}

int kcs_bin_attach(const char *filename, BYTE *rawcart)
{
    if (util_file_load(filename, rawcart, 0x4000, UTIL_FILE_LOAD_SKIP_ADDRESS) < 0) {
        return -1;
    }
    return kcs_common_attach();
}

int kcs_crt_attach(FILE *fd, BYTE *rawcart)
{
    BYTE chipheader[0x10];
    int i;

    for (i = 0; i <= 1; i++) {
        if (fread(chipheader, 0x10, 1, fd) < 1) {
            return -1;
        }

        if (chipheader[0xc] != 0x80 && chipheader[0xc] != 0xa0) {
            return -1;
        }

        if (fread(&rawcart[(chipheader[0xc] << 8) - 0x8000], 0x2000, 1, fd) < 1) {
            return -1;
        }
    }

    return kcs_common_attach();
}

void kcs_detach(void)
{
    c64export_remove(&export_res_kcs);
    c64io_unregister(kcs_io1_list_item);
    c64io_unregister(kcs_io2_list_item);
    kcs_io1_list_item = NULL;
    kcs_io2_list_item = NULL;
}

/* ---------------------------------------------------------------------*/

#define CART_DUMP_VER_MAJOR   0
#define CART_DUMP_VER_MINOR   0
#define SNAP_MODULE_NAME  "CARTKCS"

int kcs_snapshot_write_module(snapshot_t *s)
{
    snapshot_module_t *m;

    m = snapshot_module_create(s, SNAP_MODULE_NAME,
                          CART_DUMP_VER_MAJOR, CART_DUMP_VER_MINOR);
    if (m == NULL) {
        return -1;
    }

    if (0
        || (SMW_BA(m, roml_banks, 0x2000) < 0)
        || (SMW_BA(m, romh_banks, 0x2000) < 0)
        || (SMW_BA(m, export_ram0, 0x2000) < 0)) {
        snapshot_module_close(m);
        return -1;
    }

    snapshot_module_close(m);
    return 0;
}

int kcs_snapshot_read_module(snapshot_t *s)
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
        || (SMR_BA(m, roml_banks, 0x2000) < 0)
        || (SMR_BA(m, romh_banks, 0x2000) < 0)
        || (SMR_BA(m, export_ram0, 0x2000) < 0)) {
        snapshot_module_close(m);
        return -1;
    }

    snapshot_module_close(m);

    return kcs_common_attach();
}
