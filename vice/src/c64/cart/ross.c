/*
 * ross.c - Cartridge handling, Ross cart.
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
#include <stdlib.h>
#include <string.h>

#include "c64cart.h"
#include "c64cartmem.h"
#include "c64export.h"
#include "c64io.h"
#include "c64mem.h"
#include "cartridge.h"
#include "ross.h"
#include "types.h"

/*
    This cart has 16Kb mapped in at $8000-$BFFF.

    Any read access to $DE00 will switch in bank 1 (if cart is 32Kb).

    Any read access to $DF00 will switch off EXROM and GAME.

 */

static BYTE REGPARM1 ross_io1_read(WORD addr)
{
    cartridge_romhbank_set(1);
    cartridge_romlbank_set(1);

    return 0;
}

static BYTE REGPARM1 ross_io2_read(WORD addr)
{
    export.game = export.exrom = 0;
    mem_pla_config_changed();

    return 0;
}

/* ---------------------------------------------------------------------*/

static io_source_t ross_io1_device = {
    "ROSS",
    IO_DETACH_CART,
    NULL,
    0xde00, 0xdeff, 0xff,
    0, /* read is never valid */
    NULL,
    ross_io1_read,
    NULL, /* TODO: peek */
    NULL, /* TODO: dump */
    CARTRIDGE_ROSS
};

static io_source_t ross_io2_device = {
    "ROSS",
    IO_DETACH_CART,
    NULL,
    0xdf00, 0xdfff, 0xff,
    0, /* read is never valid */
    NULL,
    ross_io2_read,
    NULL, /* TODO: peek */
    NULL, /* TODO: dump */
    CARTRIDGE_ROSS
};

static io_source_list_t *ross_io1_list_item = NULL;
static io_source_list_t *ross_io2_list_item = NULL;

static const c64export_resource_t export_res = {
    "Ross", 1, 1, &ross_io1_device, &ross_io2_device, CARTRIDGE_ROSS
};

/* ---------------------------------------------------------------------*/

void ross_config_init(void)
{
    cartridge_config_changed(1, 1, CMODE_READ);
}

void ross_config_setup(BYTE *rawcart)
{
    memcpy(&roml_banks[0x0000], &rawcart[0x0000], 0x2000);
    memcpy(&romh_banks[0x0000], &rawcart[0x2000], 0x2000);
    memcpy(&roml_banks[0x2000], &rawcart[0x4000], 0x2000);
    memcpy(&romh_banks[0x2000], &rawcart[0x6000], 0x2000);
    cartridge_config_changed(0, 0, CMODE_READ);
}

/* ---------------------------------------------------------------------*/

int ross_crt_attach(FILE *fd, BYTE *rawcart)
{
    BYTE chipheader[0x10];
    int amount=0;

    while (1) {
        if (fread(chipheader, 0x10, 1, fd) < 1) {
            break;
        }

        amount++;

        if (chipheader[0xc] != 0x80 && chipheader[0xe] != 0x40 && chipheader[0xb] > 1) {
            return -1;
        }

        if (fread(&rawcart[chipheader[0xb] << 14], 0x4000, 1, fd) < 1) {
            return -1;
        }
    }

    if (amount == 1) {
        memcpy(&rawcart[0x4000], &rawcart[0x0000], 0x4000);
    }

    if (c64export_add(&export_res) < 0) {
        return -1;
    }

    ross_io1_list_item = c64io_register(&ross_io1_device);
    ross_io2_list_item = c64io_register(&ross_io2_device);

    return 0;
}

void ross_detach(void)
{
    c64export_remove(&export_res);
    c64io_unregister(ross_io1_list_item);
    c64io_unregister(ross_io2_list_item);
    ross_io1_list_item = NULL;
    ross_io2_list_item = NULL;
}
