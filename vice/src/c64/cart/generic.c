/*
 * generic.c - Cartridge handling, generic carts.
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

#include <string.h>

#include "c64cart.h"
#include "c64cartmem.h"
#include "cartridge.h"
#include "crt.h"
#include "generic.h"
#include "types.h"
#include "utils.h"

void generic_8kb_config_init(void)
{
    cartridge_config_changed(0);
}

void generic_16kb_config_init(void)
{
    cartridge_config_changed(1);
}

void generic_ultimax_config_init(void)
{
    cartridge_config_changed(3);
}

void generic_8kb_config_setup(BYTE *rawcart)
{
    memcpy(roml_banks, rawcart, 0x2000);
    cartridge_config_changed(0);
}

void generic_16kb_config_setup(BYTE *rawcart)
{
    memcpy(roml_banks, rawcart, 0x2000);
    memcpy(romh_banks, &rawcart[0x2000], 0x2000);
    cartridge_config_changed(1);
}

void generic_ultimax_config_setup(BYTE *rawcart)
{
    memcpy(&roml_banks[0x0000], &rawcart[0x0000], 0x2000);
    memcpy(&romh_banks[0x0000], &rawcart[0x2000], 0x2000);
    cartridge_config_changed(3);
}

int generic_8kb_bin_attach(const char *filename, BYTE *rawcart)
{
    if (util_file_load(filename, rawcart, 0x2000,
        UTIL_FILE_LOAD_SKIP_ADDRESS) < 0)
        return -1;

    return 0;
}

int generic_16kb_bin_attach(const char *filename, BYTE *rawcart)
{
    if (util_file_load(filename, rawcart, 0x4000,
        UTIL_FILE_LOAD_SKIP_ADDRESS) < 0)
        return -1;

    return 0;
}

int generic_crt_attach(FILE *fd, BYTE *rawcart)
{
    BYTE chipheader[0x10];

    if (fread(chipheader, 0x10, 1, fd) < 1)
        return -1;

    if (chipheader[0xc] == 0x80 && chipheader[0xe] != 0
        && chipheader[0xe] <= 0x40) {
        if (fread(rawcart, chipheader[0xe] << 8, 1, fd) < 1)
            return -1;

        crttype = (chipheader[0xe] <= 0x20) ? CARTRIDGE_GENERIC_8KB
                  : CARTRIDGE_GENERIC_16KB;
        /* try to read next CHIP header in case of 16k Ultimax cart */
        if (fread(chipheader, 0x10, 1, fd) < 1)
            return 0;
    }

    if (chipheader[0xc] >= 0xe0 && chipheader[0xe] != 0
        && (chipheader[0xe] + chipheader[0xc]) == 0x100) {
        if (fread(rawcart + ((chipheader[0xc] << 8) & 0x3fff),
            chipheader[0xe] << 8, 1, fd) < 1)
            return -1;

        crttype = CARTRIDGE_ULTIMAX;
        return 0;
    }

    return -1;
}

