/*
 * funplay.c - Cartridge handling, Fun Play cart.
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
#include "types.h"
#include "util.h"


/*
    the funplay/powerplay carts have 16 banks of ROM at $8000-9FFF,
    $DE00 is used for bank switching according to the following scheme:

    $DE00 bit 76543210
              xx210xx3

    leading to the following values:

    $00 -> Bank 0
    $08 -> Bank 1
    $10 -> Bank 2
    $18 -> Bank 3
    $20 -> Bank 4
    $28 -> Bank 5
    $30 -> Bank 6
    $38 -> Bank 7
    $01 -> Bank 8
    $09 -> Bank 9
    $11 -> Bank 10
    $19 -> Bank 11
    $21 -> Bank 12
    $29 -> Bank 13
    $31 -> Bank 14
    $39 -> Bank 15
 */

static void REGPARM2 funplay_io1_store(WORD addr, BYTE value)
{
    /* FIXME */
    cartridge_romhbank_set(((value >> 2) | (value & 1)) & 15);
    cartridge_romlbank_set(((value >> 2) | (value & 1)) & 15);
    export.game = export.exrom = 1;
    mem_pla_config_changed();
    export.ultimax_phi1 = 0;
    export.ultimax_phi2 = 0;
}

/* ---------------------------------------------------------------------*/

static io_source_t funplay_device = {
    "Fun Play",
    IO_DETACH_CART,
    NULL,
    0xde00, 0xdeff, 0xff,
    0,
    funplay_io1_store,
    NULL,
    NULL,
    NULL,
    CARTRIDGE_FUNPLAY
};

static io_source_list_t *funplay_list_item = NULL;

static const c64export_resource_t export_res = {
    "Fun Play", 1, 1, &funplay_device, NULL, CARTRIDGE_FUNPLAY
};

/* ---------------------------------------------------------------------*/

void funplay_config_init(void)
{
    cartridge_config_changed(1, 1, CMODE_READ);
    funplay_io1_store((WORD)0xde00, 0);
}

void funplay_config_setup(BYTE *rawcart)
{
    memcpy(roml_banks, rawcart, 0x2000 * 64);
    memcpy(romh_banks, &rawcart[0x2000 * 16], 0x2000 * 16);

    /* Hack: using 16kB configuration, but some carts are 8kB only */
    cartridge_config_changed(1, 1, CMODE_READ);
}

/* ---------------------------------------------------------------------*/

int funplay_crt_attach(FILE *fd, BYTE *rawcart)
{
    BYTE chipheader[0x10];

    while (1) {
        if (fread(chipheader, 0x10, 1, fd) < 1) {
            break;
        }
        if (chipheader[0xc] != 0x80 && chipheader[0xc] != 0xa0) {
            return -1;
        }
        if (fread(&rawcart[(((chipheader[0xb] >> 2) | (chipheader[0xb] & 1)) & 15) << 13], 0x2000, 1, fd) < 1) {
            return -1;
        }
    }

    if (c64export_add(&export_res) < 0) {
        return -1;
    }

    funplay_list_item = c64io_register(&funplay_device);

    return 0;
}

void funplay_detach(void)
{
    c64io_unregister(funplay_list_item);
    funplay_list_item = NULL;
    c64export_remove(&export_res);
}
