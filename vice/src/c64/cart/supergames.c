/*
 * supergames.c - Cartridge handling, Super Games cart.
 *
 * Written by
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
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
#include "c64mem.h"
#include "types.h"
#include "utils.h"


void REGPARM2 supergames_io2_store(ADDRESS addr, BYTE value)
{
    romh_bank = roml_bank = value & 3;
    if (value & 0x4) {
        export.game = 0;
        export.exrom = 1;
    } else {
        export.game = export.exrom = 1;
    }
    if (value == 0xc)
        export.game = export.exrom = 0;
    pla_config_changed();
}

void supergames_config_init(void)
{
    cartridge_config_changed(0);
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
    cartridge_config_changed(0);
}

int supergames_crt_attach(FILE *fd, BYTE *rawcart)
{
    BYTE chipheader[0x10];

    while (1) {
        if (fread(chipheader, 0x10, 1, fd) < 1)
            break;

        if (chipheader[0xc] != 0x80 && chipheader[0xe] != 0x40
            && chipheader[0xb] > 3)
            return -1;

        if (fread(&rawcart[chipheader[0xb] << 14], 0x4000, 1, fd) < 1)
            return -1;
    }
    return 0;
}

