/*
 * prophet64.c - Cartridge handling, Prophet 64 cart.
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
#include "prophet64.h"
#include "types.h"
#include "util.h"

/*
    32 banks, 8k each == 256kb

    banks are always mapped to $8000

    controlregister is $df00

    lower 5 bits ($00..$1f) selects bank
    bit 6 ($2x) disables cartridge
*/

#define PROPHET64_CART_SIZE (256*0x400)

/* ---------------------------------------------------------------------*/

static void REGPARM2 p64_io2_store(WORD addr, BYTE value)
{
    /* confirmation needed: register mirrored in entire io2 ? */
    if ((value >> 5) & 1) {
        /* cartridge off */
        cartridge_config_changed(2, 2, CMODE_READ);
    } else {
        /* cartridge on */
        cartridge_config_changed(0, 0, CMODE_READ);
    }
    cartridge_romlbank_set(value & 0x1f);
}

/* ---------------------------------------------------------------------*/

static io_source_t p64_device = {
    "Prophet 64",
    IO_DETACH_CART,
    NULL,
    0xdf00, 0xdfff, 0xff,
    0, /* read is never valid */
    p64_io2_store,
    NULL
};

static io_source_list_t *p64_list_item = NULL;

/* ---------------------------------------------------------------------*/

void p64_config_init(void)
{
    cartridge_config_changed(0, 0, CMODE_READ);
    cartridge_romlbank_set(0);
}

void p64_config_setup(BYTE *rawcart)
{
    memcpy(roml_banks, rawcart, PROPHET64_CART_SIZE);
    cartridge_config_changed(0, 0, CMODE_READ);
    cartridge_romlbank_set(0);
}

/* ---------------------------------------------------------------------*/

static const c64export_resource_t export_res = {
    "Prophet 64", 1, 0
};

static int p64_common_attach(void)
{
    if (c64export_add(&export_res) < 0) {
        return -1;
    }

    p64_list_item = c64io_register(&p64_device);

    return 0;
}

int p64_bin_attach(const char *filename, BYTE *rawcart)
{
    if (util_file_load(filename, rawcart, PROPHET64_CART_SIZE, UTIL_FILE_LOAD_SKIP_ADDRESS) < 0) {
        return -1;
    }

    return p64_common_attach();
}

int p64_crt_attach(FILE *fd, BYTE *rawcart)
{
    BYTE chipheader[0x10];
    int i, cnt = 0;

    for (i = 0; i <= 0x1f; i++) {

        if (fread(chipheader, 0x10, 1, fd) < 1) {
            break;
        }

        if (chipheader[0xb] > 0x1f) {
            return -1;
        }

        if (fread(&rawcart[chipheader[0xb] << 13], 0x2000, 1, fd) < 1) {
            return -1;
        }
        cnt++;
    }

    return p64_common_attach();
}

void p64_detach(void)
{
    c64export_remove(&export_res);
    c64io_unregister(p64_list_item);
    p64_list_item = NULL;
}

