/*
 * westermann.c - Cartridge handling, Westermann cart.
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
#include "westermann.h"
#include "types.h"

/* some prototypes are needed */
static BYTE REGPARM1 westermann_io2_read(WORD addr);

static io_source_t westermann_device = {
    "Westermann",
    IO_DETACH_CART,
    NULL,
    0xdf00, 0xdfff, 0xff,
    0, /* read is never valid */
    NULL,
    westermann_io2_read
};

static io_source_list_t *westermann_list_item = NULL;

/* ---------------------------------------------------------------------*/

BYTE REGPARM1 westermann_io2_read(WORD addr)
{
    cartridge_config_changed(0, 0, CMODE_READ);
    return 0;
}

/* ---------------------------------------------------------------------*/

static const c64export_resource_t export_res_westermann = {
    "Westermann", 1, 0
};

/* ---------------------------------------------------------------------*/

int westermann_crt_attach(FILE *fd, BYTE *rawcart)
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

    if (c64export_add(&export_res_westermann) < 0) {
        return -1;
    }

    westermann_list_item = c64io_register(&westermann_device);

    return 0;
}

void westermann_detach(void)
{
    c64export_remove(&export_res_westermann);
    c64io_unregister(westermann_list_item);
    westermann_list_item = NULL;
}

