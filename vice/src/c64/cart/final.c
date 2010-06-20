/*
 * final.c - Cartridge handling, Final cart.
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
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
#include <stdlib.h>
#include <string.h>

#include "c64cart.h"
#include "c64cartmem.h"
#include "c64export.h"
#include "c64io.h"
#include "c64mem.h"
#include "cartridge.h"
#include "final.h"
#include "types.h"
#include "util.h"

/*
    The Final Cartridge 1+2

   - 8k ROM

   - any access to IO1 turns cartridge ROM off
   - any access to IO2 turns cartridge ROM on

   - cart ROM mirror is visible in io1/io2
*/

/* some prototypes are needed */
static BYTE REGPARM1 final_v1_io1_read(WORD addr);
static void REGPARM2 final_v1_io1_store(WORD addr, BYTE value);
static BYTE REGPARM1 final_v1_io2_read(WORD addr);
static void REGPARM2 final_v1_io2_store(WORD addr, BYTE value);

static io_source_t final1_io1_device = {
    "Final I",
    IO_DETACH_CART,
    NULL,
    0xde00, 0xdeff, 0xff,
    1, /* read is always valid */
    final_v1_io1_store,
    final_v1_io1_read,
    NULL, /* TODO: peek */
    NULL, /* TODO: dump */
    CARTRIDGE_FINAL_I
};

static io_source_t final1_io2_device = {
    "Final I",
    IO_DETACH_CART,
    NULL,
    0xdf00, 0xdfff, 0xff,
    1, /* read is always valid */
    final_v1_io2_store,
    final_v1_io2_read,
    NULL, /* TODO: peek */
    NULL, /* TODO: dump */
    CARTRIDGE_FINAL_I
};

static io_source_list_t *final1_io1_list_item = NULL;
static io_source_list_t *final1_io2_list_item = NULL;

static const c64export_resource_t export_res_v1 = {
    "Final V1", 1, 0, &final1_io1_device, &final1_io2_device, CARTRIDGE_FINAL_I
};

/* ---------------------------------------------------------------------*/

BYTE REGPARM1 final_v1_io1_read(WORD addr)
{
    cartridge_config_changed(2, 2, CMODE_READ | CMODE_RELEASE_FREEZE);
    return roml_banks[0x1e00 + (addr & 0xff)];
}

void REGPARM2 final_v1_io1_store(WORD addr, BYTE value)
{
    cartridge_config_changed(2, 2, CMODE_WRITE | CMODE_RELEASE_FREEZE);
}

BYTE REGPARM1 final_v1_io2_read(WORD addr)
{
    cartridge_config_changed(1, 1, CMODE_READ);
    return roml_banks[0x1f00 + (addr & 0xff)];
}

void REGPARM2 final_v1_io2_store(WORD addr, BYTE value)
{
    cartridge_config_changed(1, 1, CMODE_WRITE);
}

/* ---------------------------------------------------------------------*/

BYTE REGPARM1 final_v1_roml_read(WORD addr)
{
    return roml_banks[(addr & 0x1fff)];
}

/* ---------------------------------------------------------------------*/

void final_v1_freeze(void)
{
    cartridge_config_changed(3, 3, CMODE_READ);
}

void final_v1_config_init(void)
{
    cartridge_config_changed(1, 1, CMODE_READ);
}

void final_v1_config_setup(BYTE *rawcart)
{
    memcpy(roml_banks, rawcart, 0x2000);
    memcpy(romh_banks, &rawcart[0x2000], 0x2000);
    cartridge_config_changed(1, 1, CMODE_READ);
}

/* ---------------------------------------------------------------------*/

static int final_v1_common_attach(void)
{
    if (c64export_add(&export_res_v1) < 0) {
        return -1;
    }

    final1_io1_list_item = c64io_register(&final1_io1_device);
    final1_io2_list_item = c64io_register(&final1_io2_device);

    return 0;
}

int final_v1_bin_attach(const char *filename, BYTE *rawcart)
{
    if (util_file_load(filename, rawcart, 0x4000, UTIL_FILE_LOAD_SKIP_ADDRESS) < 0) {
        return -1;
    }

    return final_v1_common_attach();
}

int final_v1_crt_attach(FILE *fd, BYTE *rawcart)
{
    BYTE chipheader[0x10];

    if (fread(chipheader, 0x10, 1, fd) < 1) {
        return -1;
    }

    if (chipheader[0xc] != 0x80 || chipheader[0xe] != 0x40) {
        return -1;
    }

    if (fread(rawcart, chipheader[0xe] << 8, 1, fd) < 1) {
        return -1;
    }

    return final_v1_common_attach();
}

void final_v1_detach(void)
{
    c64export_remove(&export_res_v1);
    c64io_unregister(final1_io1_list_item);
    c64io_unregister(final1_io2_list_item);
    final1_io1_list_item = NULL;
    final1_io2_list_item = NULL;
}
