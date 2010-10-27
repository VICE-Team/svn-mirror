/*
 * supergames.c - Cartridge handling, Super Games cart.
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
#include "c64mem.h"
#include "cartridge.h"
#include "supergames.h"
#include "types.h"
#include "util.h"

/*
    "Super Games"

    This cart uses 4 16Kb banks mapped in at $8000-$BFFF.

    The control registers is at $DF00, and has the following meaning:

    bit   meaning
    ---   -------
     0    bank bit 0
     1    bank bit 1
     2    inverted GAME line
     3    inverted EXROM line
    4-7   unused
*/

static int currbank = 0;

static void REGPARM2 supergames_io2_store(WORD addr, BYTE value)
{
    cartridge_romhbank_set(value & 3);
    cartridge_romlbank_set(value & 3);
    currbank = value & 3;

    if (value & 0x4) {
        export.game = 0;
        export.exrom = 1;
    } else {
        export.game = export.exrom = 1;
    }
    if (value == 0xc) {
        export.game = export.exrom = 0;
    }
    mem_pla_config_changed();
}

static BYTE REGPARM1 supergames_io2_peek(WORD addr)
{
    return currbank;
}

/* ---------------------------------------------------------------------*/

static io_source_t supergames_device = {
    "SUPERGAMES",
    IO_DETACH_CART,
    NULL,
    0xdf00, 0xdfff, 0xff,
    0,
    supergames_io2_store,
    NULL,
    supergames_io2_peek,
    NULL, /* TODO: dump */
    CARTRIDGE_SUPER_GAMES
};

static io_source_list_t *supergames_list_item = NULL;

static const c64export_resource_t export_res = {
    "Super Games", 1, 1, NULL, &supergames_device, CARTRIDGE_SUPER_GAMES
};

/* ---------------------------------------------------------------------*/

void supergames_config_init(void)
{
    cartridge_config_changed(0, 0, CMODE_READ);
}

void supergames_config_setup(BYTE *rawcart)
{
    memcpy(&roml_banks[0x0000], &rawcart[0x0000], 0x2000);
    memcpy(&romh_banks[0x0000], &rawcart[0x2000], 0x2000);
    memcpy(&roml_banks[0x2000], &rawcart[0x4000], 0x2000);
    memcpy(&romh_banks[0x2000], &rawcart[0x6000], 0x2000);
    memcpy(&roml_banks[0x4000], &rawcart[0x8000], 0x2000);
    memcpy(&romh_banks[0x4000], &rawcart[0xa000], 0x2000);
    memcpy(&roml_banks[0x6000], &rawcart[0xc000], 0x2000);
    memcpy(&romh_banks[0x6000], &rawcart[0xe000], 0x2000);
    cartridge_config_changed(0, 0, CMODE_READ);
}

/* ---------------------------------------------------------------------*/
static int supergames_common_attach(void)
{
    if (c64export_add(&export_res) < 0) {
        return -1;
    }
    supergames_list_item = c64io_register(&supergames_device);
    return 0;
}

int supergames_bin_attach(const char *filename, BYTE *rawcart)
{
    if (util_file_load(filename, rawcart, 0x10000, UTIL_FILE_LOAD_SKIP_ADDRESS) < 0) {
        return -1;
    }
    return supergames_common_attach();
}

int supergames_crt_attach(FILE *fd, BYTE *rawcart)
{
    BYTE chipheader[0x10];

    while (1) {
        if (fread(chipheader, 0x10, 1, fd) < 1) {
            break;
        }

        if (chipheader[0xc] != 0x80 && chipheader[0xe] != 0x40 && chipheader[0xb] > 3) {
            return -1;
        }

        if (fread(&rawcart[chipheader[0xb] << 14], 0x4000, 1, fd) < 1) {
            return -1;
        }
    }
    return supergames_common_attach();
}

void supergames_detach(void)
{
    c64export_remove(&export_res);
    c64io_unregister(supergames_list_item);
    supergames_list_item = NULL;
}
