/*
 * atomicpower.c - Cartridge handling, Atomic Power cart.
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

#include "atomicpower.h"
#include "c64cart.h"
#include "c64cartmem.h"
#include "types.h"
#include "utils.h"


/* Atomic Power RAM hack.  */
static int export_ram_at_a000 = 0;

BYTE REGPARM1 atomicpower_io1_read(ADDRESS addr)
{
    return rand();
}

void REGPARM2 atomicpower_io1_store(ADDRESS addr, BYTE value)
{
    if (value == 0x22) {
        value = 0x03;
        export_ram_at_a000 = 1;
    } else {
        export_ram_at_a000 = 0;
    }
    cartridge_config_changed(value);
}

BYTE REGPARM1 atomicpower_io2_read(ADDRESS addr)
{
    if (export_ram)
        return export_ram0[0x1f00 + (addr & 0xff)];

    switch (roml_bank) {
      case 0:
        return roml_banks[addr & 0x1fff];
      case 1:
        return roml_banks[(addr & 0x1fff) + 0x2000];
      case 2:
        return roml_banks[(addr & 0x1fff) + 0x4000];
      case 3:
        return roml_banks[(addr & 0x1fff) + 0x6000];
    }
    return 0;
}

void REGPARM2 atomicpower_io2_store(ADDRESS addr, BYTE value)
{
    if (export_ram)
        export_ram0[0x1f00 + (addr & 0xff)] = value;
}

BYTE REGPARM1 atomicpower_roml_read(ADDRESS addr)
{
    if (export_ram)
        return export_ram0[addr & 0x1fff];

    return roml_banks[(addr & 0x1fff) + (roml_bank << 13)];
}

void REGPARM2 atomicpower_roml_store(ADDRESS addr, BYTE value)
{
    if (export_ram)
        export_ram0[addr & 0x1fff] = value;
}

BYTE REGPARM1 atomicpower_romh_read(ADDRESS addr)
{
    if (export_ram_at_a000)
        return export_ram0[addr & 0x1fff];

    return romh_banks[(addr & 0x1fff) + (romh_bank << 13)];
}

BYTE REGPARM1 atomicpower_a000_bfff_read(ADDRESS addr)
{
    if (export_ram_at_a000)
        return export_ram0[addr & 0x1fff];
    return 0x55;
}

void REGPARM2 atomicpower_a000_bfff_store(ADDRESS addr, BYTE value)
{
    if (export_ram_at_a000)
        export_ram0[addr & 0x1fff] = value;
    return;
}

void atomicpower_freeze(void)
{
    cartridge_config_changed(35);
}

void atomicpower_config_init(void)
{
    export_ram_at_a000 = 0;
    cartridge_config_changed(0);
}

void atomicpower_config_setup(BYTE *rawcart)
{
    memcpy(roml_banks, rawcart, 0x8000);
    memcpy(romh_banks, rawcart, 0x8000);
    cartridge_config_changed(0);
}

int atomicpower_bin_attach(const char *filename, BYTE *rawcart)
{
    if (util_file_load(filename, rawcart, 0x8000,
        UTIL_FILE_LOAD_SKIP_ADDRESS) < 0)
        return -1;

    return 0;
}

int atomicpower_crt_attach(FILE *fd, BYTE *rawcart)
{
    BYTE chipheader[0x10];
    int i;

    for (i = 0; i <= 3; i++) {
        if (fread(chipheader, 0x10, 1, fd) < 1)
            return -1;

        if (chipheader[0xb] > 3)
            return -1;

        if (fread(&rawcart[chipheader[0xb] << 13], 0x2000, 1, fd) < 1)
            return -1;
    }

    return 0;
}

