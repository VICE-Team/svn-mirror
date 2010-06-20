/*
 * exos.c - Cartridge handling, Exos cart.
 *
 * Written by
 *  groepaz <groepaz@gmx.net>
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

#include "exos.h"
#include "c64cart.h"
#include "c64cartmem.h"
#include "c64export.h"
#include "c64io.h"
#include "c64mem.h"
#include "c64memrom.h"
#include "c64rom.h"
#include "cartridge.h"
#include "machine.h"
#include "resources.h"
#include "types.h"
#include "util.h"

/*
    Exos v3
    - this refers to a family of cartridge boards produced by REX Datentechnik.
      generally it can be used with any kernal replacement

    8K ROM
    - ROM is mapped to $e000 using ultimax mode, but only when hirom is selected
      (the cartridge uses a clip to the inside of the computer for this)
*/

static const c64export_resource_t export_res = {
    "Exos", 1, 1, NULL, NULL, CARTRIDGE_EXOS
};

/* ---------------------------------------------------------------------*/

BYTE REGPARM1 exos_romh_read(WORD addr)
{
    return romh_banks[(addr & 0x1fff)];
}

void exos_config_init(void)
{
    cartridge_config_changed(2, 3, CMODE_READ);
}

/* ---------------------------------------------------------------------*/

void exos_config_setup(BYTE *rawcart)
{
    memcpy(romh_banks, &rawcart[0], 0x2000);
    cartridge_config_changed(2, 3, CMODE_READ);
}

/* ---------------------------------------------------------------------*/

static int exos_common_attach(void)
{
    if (c64export_add(&export_res) < 0) {
        return -1;
    }
    return 0;
}

int exos_bin_attach(const char *filename, BYTE *rawcart)
{
    if (util_file_load(filename, rawcart, 0x2000, UTIL_FILE_LOAD_SKIP_ADDRESS) < 0) {
        return -1;
    }

    return exos_common_attach();
}

int exos_crt_attach(FILE *fd, BYTE *rawcart)
{
    BYTE chipheader[0x10];

    if (fread(chipheader, 0x10, 1, fd) < 1) {
        return -1;
    }

    if (fread(&rawcart[0x0000], 0x2000, 1, fd) < 1) {
        return -1;
    }

    return exos_common_attach();
}

void exos_detach(void)
{
    c64export_remove(&export_res);
}
