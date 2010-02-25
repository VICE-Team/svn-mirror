/*
 * rexutility.c - Cartridge handling, REX Utility cart.
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
#include "rexutility.h"
#include "types.h"

static BYTE REGPARM1 rex_io2_read(WORD addr)
{
    if ((addr & 0xff) < 0xc0) {
        /* disable cartridge rom */
        cartridge_config_changed(2, 2, CMODE_READ);
    } else {
        /* enable cartridge rom */
        cartridge_config_changed(0, 0, CMODE_READ);
    }
    return 0;
}

/* ---------------------------------------------------------------------*/

static io_source_t rex_device = {
    "REX UTIL CART",
    IO_DETACH_CART,
    NULL,
    0xdf00, 0xdfff, 0xff,
    0, /* read is never valid */
    NULL,
    rex_io2_read
};

static io_source_list_t *rex_list_item = NULL;

/* ---------------------------------------------------------------------*/

static const c64export_resource_t export_res_rex = {
    "REX", 0, 0
};

int rex_crt_attach(FILE *fd, BYTE *rawcart)
{
    BYTE chipheader[0x10];

    if (fread(chipheader, 0x10, 1, fd) < 1) {
        return -1;
    }

    if (fread(rawcart, 0x2000, 1, fd) < 1) {
        return -1;
    }

    if (c64export_add(&export_res_rex) < 0) {
        return -1;
    }

    rex_list_item = c64io_register(&rex_device);

    return 0;
}

void rex_detach(void)
{
    c64export_remove(&export_res_rex);
    c64io_unregister(rex_list_item);
    rex_list_item = NULL;
}
