/*
 * epyxfastload.c - Cartridge handling, EPYX Fastload cart.
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
#include "epyxfastload.h"
#include "types.h"


static const c64export_resource_t export_res_epyx = {
    "Epyx Fastload", 0, 0
};

static const c64export_resource_t export_res_rex = {
    "REX", 0, 0
};


BYTE REGPARM1 epyxfastload_io2_read(WORD addr)
{
    io_source=IO_SOURCE_EPYX_FASTLOAD;

    if (addr == 0xdf18)
        cartridge_config_changed(0, 0, CMODE_READ);
    if (addr == 0xdf38)
        cartridge_config_changed(2, 2, CMODE_READ);
    return roml_banks[0x1f00 + (addr & 0xff)];
}

void epyxfastload_config_init(void)
{
    cartridge_config_changed(0, 0, CMODE_READ);
}

void epyxfastload_config_setup(BYTE *rawcart)
{
    memcpy(roml_banks, rawcart, 0x2000);
    cartridge_config_changed(0, 0, CMODE_READ);
}

static int generic_epyxfastload_crt_attach(FILE *fd, BYTE *rawcart)
{
    BYTE chipheader[0x10];

    if (fread(chipheader, 0x10, 1, fd) < 1)
        return -1;

    if (fread(rawcart, 0x2000, 1, fd) < 1)
        return -1;

    return 0;
}

int epyxfastload_crt_attach(FILE *fd, BYTE *rawcart)
{
    if (generic_epyxfastload_crt_attach(fd,rawcart) < 0)
        return -1;

    if (c64export_add(&export_res_epyx) < 0)
        return -1;

    return 0;
}

int rex_crt_attach(FILE *fd, BYTE *rawcart)
{
    if (generic_epyxfastload_crt_attach(fd,rawcart) < 0)
        return -1;

    if (c64export_add(&export_res_rex) < 0)
        return -1;

    return 0;
}

void epyxfastload_detach(void)
{
    c64export_remove(&export_res_epyx);
}

void rex_detach(void)
{
    c64export_remove(&export_res_rex);
}
