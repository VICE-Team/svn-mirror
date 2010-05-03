/*
 * simonsbasic.c - Cartridge handling, Simons Basic cart.
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
#include "simonsbasic.h"
#include "types.h"

static BYTE REGPARM1 simon_io1_read(WORD addr)
{
    cartridge_config_changed(0, 0, CMODE_READ);
    return 0;
}

static void REGPARM2 simon_io1_store(WORD addr, BYTE value)
{
    cartridge_config_changed(1, 1, CMODE_WRITE);
}

/* ---------------------------------------------------------------------*/

static io_source_t simon_device = {
    "Simon's Basic",
    IO_DETACH_CART,
    NULL,
    0xde00, 0xdeff, 0xff,
    0, /* read is never valid */
    simon_io1_store,
    simon_io1_read
};

static io_source_list_t *simon_list_item = NULL;

/* ---------------------------------------------------------------------*/

static const c64export_resource_t export_res_simon = {
    "Simon's Basic", 1, 1
};

static int generic_sb_crt_attach(FILE *fd, BYTE *rawcart)
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

    return 0;
}

int simon_crt_attach(FILE *fd, BYTE *rawcart)
{
    if (generic_sb_crt_attach(fd,rawcart) < 0) {
        return -1;
    }

    if (c64export_add(&export_res_simon) < 0) {
        return -1;
    }

    simon_list_item = c64io_register(&simon_device);

    return 0;
}

void simon_detach(void)
{
    c64export_remove(&export_res_simon);
    c64io_unregister(simon_list_item);
    simon_list_item = NULL;
}
