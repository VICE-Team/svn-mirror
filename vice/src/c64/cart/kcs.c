/*
 * kcs.c - Cartridge handling, KCS cart.
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
#include "kcs.h"
#include "types.h"
#include "utils.h"


BYTE REGPARM1 kcs_io1_read(ADDRESS addr)
{
    cartridge_config_changed(0);
    return roml_banks[0x1e00 + (addr & 0xff)];
}

void REGPARM2 kcs_io1_store(ADDRESS addr, BYTE value)
{
    cartridge_config_changed(1);
}

BYTE REGPARM1 kcs_io2_read(ADDRESS addr)
{
    if (addr & 0x80)
        cartridge_config_changed(0x43);
    return export_ram0[0x1f00 + (addr & 0xff)];
}

void REGPARM2 kcs_io2_store(ADDRESS addr, BYTE value)
{
    if (!ultimax)
        cartridge_config_changed(1);
    export_ram0[0x1f00 + (addr & 0xff)] = value;
}

void kcs_freeze(void)
{
    cartridge_config_changed(3);
}

void kcs_config_init(void)
{
    cartridge_config_changed(0);
}

void kcs_config_setup(BYTE *rawcart)
{
    memcpy(roml_banks, rawcart, 0x2000);
    memcpy(romh_banks, &rawcart[0x2000], 0x2000);
    cartridge_config_changed(0);
}

int kcs_crt_attach(FILE *fd, BYTE *rawcart)
{
    BYTE chipheader[0x10];
    int i;

    for (i = 0; i <= 1; i++) {
        if (fread(chipheader, 0x10, 1, fd) < 1)
            return -1;

        if (chipheader[0xc] != 0x80 && chipheader[0xc] != 0xa0)
            return -1;
        
        if (fread(&rawcart[(chipheader[0xc] << 8) - 0x8000], 0x2000, 1, fd) < 1)
            return -1;
    }

    return 0;
}

