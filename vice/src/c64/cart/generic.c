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

#include <stdio.h>

#include "archdep.h"
#include "generic.h"
#include "types.h"
#include "utils.h"

int generic_8kb_attach(const char *filename, BYTE *rawcart)
{
    FILE *fd;

    fd = fopen(filename, MODE_READ);

    if (!fd)
        return -1;

    if (util_file_length(fd) == 0x2002)
        fread(rawcart, 2, 1, fd);

    if (fread(rawcart, 0x2000, 1, fd) < 1) {
        fclose(fd);
        return -1;
    }

    fclose(fd);
    return 0;
}

int generic_16kb_attach(const char *filename, BYTE *rawcart)
{
    FILE *fd;

    fd = fopen(filename, MODE_READ);

    if (!fd)
        return -1;

    if (util_file_length(fd) == 0x4002)
        fread(rawcart, 2, 1, fd);

    if (fread(rawcart, 0x4000, 1, fd) < 1) {
        fclose(fd);
        return -1;
    }

    fclose(fd);
    return 0;
}

