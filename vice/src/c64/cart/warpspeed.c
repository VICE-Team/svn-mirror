/*
 * warpspeed.c - Cartridge handling, Warpspeed cart.
 *
 * Written by
 *  groepaz <groepaz@gmx.net>
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
#include "warpspeed.h"
#include "types.h"
#include "util.h"

/*
    Warpspeed

    - 16k ROM
    - uses full io1/io2

    io1
    - read: ROM (offset $1e00)
    - write: enable rom at 8000

    io2
    - read: ROM (offset $1f00)
    - write: disable rom at 8000
*/

/* some prototypes are needed */
static BYTE REGPARM1 warpspeed_io1_read(WORD addr);
static void REGPARM2 warpspeed_io1_store(WORD addr, BYTE value);
static BYTE REGPARM1 warpspeed_io2_read(WORD addr);
static void REGPARM2 warpspeed_io2_store(WORD addr, BYTE value);

static io_source_t warpspeed_io1_device = {
    "Warpspeed",
    IO_DETACH_CART,
    NULL,
    0xde00, 0xdeff, 0xff,
    1, /* read is always valid */
    warpspeed_io1_store,
    warpspeed_io1_read,
    NULL, /* no side effects when reading */
    NULL, /* FIXME: dump */
    CARTRIDGE_WARPSPEED
};

static io_source_t warpspeed_io2_device = {
    "Warpspeed",
    IO_DETACH_CART,
    NULL,
    0xdf00, 0xdfff, 0xff,
    1, /* read is always valid */
    warpspeed_io2_store,
    warpspeed_io2_read,
    NULL, /* no side effects when reading */
    NULL, /* FIXME: dump */
    CARTRIDGE_WARPSPEED
};

static io_source_list_t *warpspeed_io1_list_item = NULL;
static io_source_list_t *warpspeed_io2_list_item = NULL;

/* ---------------------------------------------------------------------*/

BYTE REGPARM1 warpspeed_io1_read(WORD addr)
{
    return roml_banks[0x1e00 + (addr & 0xff)];
}

void REGPARM2 warpspeed_io1_store(WORD addr, BYTE value)
{
    cartridge_config_changed(1, 1, CMODE_WRITE);
}

BYTE REGPARM1 warpspeed_io2_read(WORD addr)
{
    return roml_banks[0x1f00 + (addr & 0xff)];
}

void REGPARM2 warpspeed_io2_store(WORD addr, BYTE value)
{
    cartridge_config_changed(2, 2, CMODE_WRITE);
}

/* ---------------------------------------------------------------------*/

static const c64export_resource_t export_res_warpspeed = {
    "Warpspeed", 1, 0, &warpspeed_io1_device, &warpspeed_io2_device, CARTRIDGE_WARPSPEED
};

/* ---------------------------------------------------------------------*/

void warpspeed_config_setup(BYTE *rawcart)
{
    memcpy(roml_banks, rawcart, 0x2000);
    memcpy(romh_banks, &rawcart[0x2000], 0x2000);
    cartridge_config_changed(1, 1, CMODE_READ);
}

static int warpspeed_common_attach(void)
{
    if (c64export_add(&export_res_warpspeed) < 0) {
        return -1;
    }

    warpspeed_io1_list_item = c64io_register(&warpspeed_io1_device);
    warpspeed_io2_list_item = c64io_register(&warpspeed_io2_device);

    return 0;
}

int warpspeed_bin_attach(const char *filename, BYTE *rawcart)
{
    if (util_file_load(filename, rawcart, 0x4000, UTIL_FILE_LOAD_SKIP_ADDRESS) < 0) {
        return -1;
    }
    return warpspeed_common_attach();
}

int warpspeed_crt_attach(FILE *fd, BYTE *rawcart)
{
    BYTE chipheader[0x10];

    if (fread(chipheader, 0x10, 1, fd) < 1) {
        return -1;
    }

    if (chipheader[0xc] != 0x80 || chipheader[0xe] != 0x40) {
        return -1;
    }

    if (fread(rawcart, chipheader[0xe] << 8, 1, fd) < 1) {
        return -1;
    }

    return warpspeed_common_attach();
}

void warpspeed_detach(void)
{
    c64export_remove(&export_res_warpspeed);
    c64io_unregister(warpspeed_io1_list_item);
    c64io_unregister(warpspeed_io2_list_item);
    warpspeed_io1_list_item = NULL;
    warpspeed_io2_list_item = NULL;
}
