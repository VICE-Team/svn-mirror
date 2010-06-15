/*
 * freezeframe.c - Cartridge handling, Freeze Frame cart.
 *
 * Written by
 *  Groepaz <groepaz@gmx.net>
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
#include "freezeframe.h"
#include "types.h"
#include "util.h"

/* #define FFDEBUG */

#ifdef FFDEBUG
#define DBG(x) printf x
#else
#define DBG(x)
#endif

/*
    Evesham Micros "Freeze Frame"

    - 8k ROM

    - reading io1 (the software uses de00 only it seems)
      - switches to 8k game mode
      - ROM is mapped to 8000

    - reading io2 (the software uses df00 only it seems)
      - disables cartridge ROM

    - reset
      - enables 8K game mode
      - ROM bank is mapped to 8000

    - freeze
      - enables ultimax mode
      - ROM is mapped to 8000
      - ROM is mapped to E000
*/

#define FREEZE_FRAME_CART_SIZE (8*0x400)

/* ---------------------------------------------------------------------*/

static BYTE REGPARM1 freezeframe_io1_read(WORD addr)
{
    DBG(("io1 r %04x\n", addr));
    if (addr == 0) {
        cartridge_config_changed(2, 1, CMODE_READ);
        DBG(("Freeze Frame: switching to 8k game mode\n"));
    }
    return 0; /* invalid */
}

static void REGPARM2 freezeframe_io1_store(WORD addr, BYTE value)
{
    DBG(("io1 %04x %02x\n", addr, value));
}

static BYTE REGPARM1 freezeframe_io2_read(WORD addr)
{
    DBG(("io2 r %04x\n", addr));
    if (addr == 0) {
        cartridge_config_changed(2, 2, CMODE_READ);
        DBG(("Freeze Frame disabled\n"));
    }
    return 0; /* invalid */
}

static void REGPARM2 freezeframe_io2_store(WORD addr, BYTE value)
{
    DBG(("io2 %04x %02x\n", addr, value));
}

static io_source_t freezeframe_io1_device = {
    "Freeze Frame",
    IO_DETACH_CART,
    NULL,
    0xde00, 0xdeff, 0xff,
    0, /* read is never valid */
    freezeframe_io1_store,
    freezeframe_io1_read,
    NULL,
    NULL,
    CARTRIDGE_FREEZE_FRAME
};
static io_source_t freezeframe_io2_device = {
    "Freeze Frame",
    IO_DETACH_CART,
    NULL,
    0xdf00, 0xdfff, 0xff,
    0, /* read is never valid */
    freezeframe_io2_store,
    freezeframe_io2_read,
    NULL,
    NULL,
    CARTRIDGE_FREEZE_FRAME
};

static io_source_list_t *freezeframe_io1_list_item = NULL;
static io_source_list_t *freezeframe_io2_list_item = NULL;

static const c64export_resource_t export_res = {
    "Freeze Frame", 1, 1, &freezeframe_io1_device, &freezeframe_io2_device, CARTRIDGE_FREEZE_FRAME
};

/* ---------------------------------------------------------------------*/

void freezeframe_freeze(void)
{
    DBG(("Freeze Frame: freeze\n"));
    cartridge_config_changed(2, 3, CMODE_READ | CMODE_RELEASE_FREEZE);
}


void freezeframe_config_init(void)
{
    cartridge_config_changed(2, 0, CMODE_READ);
}

void freezeframe_config_setup(BYTE *rawcart)
{
    memcpy(roml_banks, rawcart, FREEZE_FRAME_CART_SIZE);
    memcpy(romh_banks, rawcart, FREEZE_FRAME_CART_SIZE);
    cartridge_config_changed(2, 0, CMODE_READ);
}

/* ---------------------------------------------------------------------*/

static int freezeframe_common_attach(void)
{
    if (c64export_add(&export_res) < 0) {
        return -1;
    }

    freezeframe_io1_list_item = c64io_register(&freezeframe_io1_device);
    freezeframe_io2_list_item = c64io_register(&freezeframe_io2_device);

    return 0;
}

int freezeframe_bin_attach(const char *filename, BYTE *rawcart)
{
    DBG(("Freeze Frame: bin attach '%s'\n", filename));
    if (util_file_load(filename, rawcart, FREEZE_FRAME_CART_SIZE, UTIL_FILE_LOAD_SKIP_ADDRESS) < 0) {
        return -1;
    }

    return freezeframe_common_attach();
}

int freezeframe_crt_attach(FILE *fd, BYTE *rawcart)
{
    BYTE chipheader[0x10];

    if (fread(chipheader, 0x10, 1, fd) < 1) {
        return -1;
    }

    if (chipheader[0xb] > 0) {
        return -1;
    }

    if (fread(rawcart, FREEZE_FRAME_CART_SIZE, 1, fd) < 1) {
        return -1;
    }

    return freezeframe_common_attach();
}

void freezeframe_detach(void)
{
    c64export_remove(&export_res);
    c64io_unregister(freezeframe_io1_list_item);
    c64io_unregister(freezeframe_io2_list_item);
    freezeframe_io1_list_item = NULL;
    freezeframe_io2_list_item = NULL;
}

