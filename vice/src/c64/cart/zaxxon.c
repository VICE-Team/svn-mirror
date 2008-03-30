/*
 * zaxxon.c - Cartridge handling, Zaxxon cart.
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
 *  Markus Brenner <markus@brenner.de>
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
#include "types.h"
#include "zaxxon.h"


static const c64export_resource_t export_res = {
    "Zaxxon", 1, 1
};


BYTE REGPARM1 zaxxon_roml_read(WORD addr)
{
    cartridge_romhbank_set((addr & 0x1000) ? 1 : 0);
    return roml_banks[(addr & 0x1fff) + (roml_bank << 13)];
}

void zaxxon_config_init(void)
{
    cartridge_config_changed(1, 1, CMODE_READ);
}

void zaxxon_config_setup(BYTE *rawcart)
{
    memcpy(roml_banks, rawcart, 0x2000);
    memcpy(romh_banks, &rawcart[0x2000], 0x4000);
    cartridge_config_changed(1, 1, CMODE_READ);
}

int zaxxon_crt_attach(FILE *fd, BYTE *rawcart)
{
    BYTE chipheader[0x10];
    int i;

    /* first CHIP header holds $8000-$a000 data */
    if (fread(chipheader, 0x10, 1, fd) < 1)
        return -1;

    if (chipheader[0xc] != 0x80
        || (chipheader[0xe] != 0x10 && chipheader[0xe] != 0x20)
        || fread(rawcart, chipheader[0xe] << 8, 1, fd) < 1)
        return -1;

    /* 4kB ROM is mirrored to $9000 */
    if (chipheader[0xe] == 0x10)
        memcpy(&rawcart[0x1000], &rawcart[0x0000], 0x1000);

    /* second/third CHIP headers hold $a000-$c000 banked data */
    for (i = 0; i <= 1; i++) {
        if (fread(chipheader, 0x10, 1, fd) < 1)
            return -1;

        if (chipheader[0xc] != 0xa0 || chipheader[0xe] != 0x20
            || fread(&rawcart[0x2000+(chipheader[0xb] << 13)],
                     0x2000, 1, fd) < 1)
            return -1;
    }

    if (c64export_add(&export_res) < 0)
        return -1;

    return 0;
}

void zaxxon_detach(void)
{
    c64export_remove(&export_res);
}

