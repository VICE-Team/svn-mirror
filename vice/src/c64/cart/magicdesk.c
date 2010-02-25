/*
 * magicdesk.c - Cartridge handling, Magic Desk cart.
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
#include "types.h"
#include "util.h"
#include "vicii-phi1.h"

static void REGPARM2 magicdesk_io1_store(WORD addr, BYTE value)
{
    cartridge_romlbank_set(value & 0x3f);
    export.game = 0;
    if (value & 0x80) {
        export.exrom = 0;
    } else {
        export.exrom = 1;  /* turn off cart ROM */
    }
    mem_pla_config_changed();
}

/* ---------------------------------------------------------------------*/

static io_source_t magicdesk_device = {
    "MAGIC DESK",
    IO_DETACH_CART,
    NULL,
    0xde00, 0xdeff, 0xff,
    0,
    magicdesk_io1_store,
    NULL
};

static io_source_list_t *magicdesk_list_item = NULL;

/* ---------------------------------------------------------------------*/

void magicdesk_config_init(void)
{
    cartridge_config_changed(0, 0, CMODE_READ);
    magicdesk_io1_store((WORD)0xde00, 0);
}

void magicdesk_config_setup(BYTE *rawcart)
{
    memcpy(roml_banks, rawcart, 0x2000 * 64);
    cartridge_config_changed(0, 0, CMODE_READ);
}

/* ---------------------------------------------------------------------*/

static const c64export_resource_t export_res = {
    "Magic Desk", 1, 1
};

int magicdesk_crt_attach(FILE *fd, BYTE *rawcart)
{
    BYTE chipheader[0x10];

    while (1) {
        if (fread(chipheader, 0x10, 1, fd) < 1) {
            fclose(fd);
            break;
        }
        if (chipheader[0xb] >= 64 || (chipheader[0xc] != 0x80 && chipheader[0xc] != 0xa0)) {
            fclose(fd);
            return -1;
        }
        if (fread(&rawcart[chipheader[0xb] << 13], 0x2000, 1, fd) < 1) {
            fclose(fd);
            return -1;
        }
    }

    if (c64export_add(&export_res) < 0) {
        return -1;
    }

    magicdesk_list_item = c64io_register(&magicdesk_device);

    return 0;
}

void magicdesk_detach(void)
{
    c64export_remove(&export_res);
    c64io_unregister(magicdesk_list_item);
    magicdesk_list_item = NULL;
}
