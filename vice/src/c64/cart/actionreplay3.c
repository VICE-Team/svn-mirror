/*
 * actionreplay3.c - Cartridge handling, Action Replay III cart.
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
#include <string.h>

#include "actionreplay.h"
#include "c64cart.h"
#include "c64cartmem.h"
#include "c64export.h"
#include "c64io.h"
#include "types.h"
#include "util.h"

static unsigned int ar_active;

/* ---------------------------------------------------------------------*/

/* some prototypes are needed */
static void REGPARM2 actionreplay3_io1_store(WORD addr, BYTE value);
static BYTE REGPARM1 actionreplay3_io2_read(WORD addr);

static io_source_t actionreplay3_io1_device = {
    "Action Replay III",
    IO_DETACH_CART,
    NULL,
    0xde00, 0xdeff, 0xff,
    0,
    actionreplay3_io1_store,
    NULL
};

static io_source_t actionreplay3_io2_device = {
    "Action Replay III",
    IO_DETACH_CART,
    NULL,
    0xdf00, 0xdfff, 0xff,
    0,
    NULL,
    actionreplay3_io2_read
};

static io_source_list_t *actionreplay3_io1_list_item = NULL;
static io_source_list_t *actionreplay3_io2_list_item = NULL;

/* ---------------------------------------------------------------------*/

static void REGPARM2 actionreplay3_io1_store(WORD addr, BYTE value)
{
    int exrom, bank, conf;

    exrom = (value >> 3) & 1;
    bank = value & 1;
    conf = (bank << 3) | ((exrom ^ 1) << 1);

    if (ar_active) {
        cartridge_config_changed((BYTE)conf, (BYTE)conf, CMODE_WRITE);
        if (value & 4) {
            ar_active = 0;
        }
    }
}

static BYTE REGPARM1 actionreplay3_io2_read(WORD addr)
{
    actionreplay3_io2_device.io_source_valid = 0;

    if (!ar_active) {
        return 0;
    }

    actionreplay3_io2_device.io_source_valid = 1;

    addr |= 0xdf00;

    switch (roml_bank) {
        case 0:
           return roml_banks[addr & 0x1fff];
        case 1:
           return roml_banks[(addr & 0x1fff) + 0x2000];
    }

    actionreplay3_io2_device.io_source_valid = 0;

    return 0;
}

/* ---------------------------------------------------------------------*/

void actionreplay3_freeze(void)
{
    ar_active = 1;
    cartridge_config_changed(3, 3, CMODE_READ);
}

void actionreplay3_config_init(void)
{
    ar_active = 1;
    cartridge_config_changed(8, 8, CMODE_READ);
}

void actionreplay3_reset(void)
{
    ar_active = 1;
}

void actionreplay3_config_setup(BYTE *rawcart)
{
    memcpy(roml_banks, rawcart, 0x4000);
    memcpy(romh_banks, rawcart, 0x4000);

    cartridge_config_changed(8, 8, CMODE_READ);
}

/* ---------------------------------------------------------------------*/

static const c64export_resource_t export_res = {
    "Action Replay III", 1, 1
};

static int actionreplay3_common_attach(void)
{
    if (c64export_add(&export_res) < 0) {
        return -1;
    }

    actionreplay3_io1_list_item = c64io_register(&actionreplay3_io1_device);
    actionreplay3_io2_list_item = c64io_register(&actionreplay3_io2_device);

    return 0;
}

int actionreplay3_bin_attach(const char *filename, BYTE *rawcart)
{
    if (util_file_load(filename, rawcart, 0x4000, UTIL_FILE_LOAD_SKIP_ADDRESS) < 0) {
        return -1;
    }

    return actionreplay3_common_attach();
}

int actionreplay3_crt_attach(FILE *fd, BYTE *rawcart)
{
    BYTE chipheader[0x10];
    int i;

    for (i = 0; i <= 1; i++) {
        if (fread(chipheader, 0x10, 1, fd) < 1) {
            return -1;
        }

        if (chipheader[0xb] > 1) {
            return -1;
        }

        if (fread(&rawcart[chipheader[0xb] << 13], 0x2000, 1, fd) < 1) {
            return -1;
        }
    }

    return actionreplay3_common_attach();
}

void actionreplay3_detach(void)
{
    c64export_remove(&export_res);
    c64io_unregister(actionreplay3_io1_list_item);
    c64io_unregister(actionreplay3_io2_list_item);
    actionreplay3_io1_list_item = NULL;
    actionreplay3_io2_list_item = NULL;
}
