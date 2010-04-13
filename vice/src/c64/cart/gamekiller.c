/*
 * gamekiller.c - Cartridge handling, Game Killer cart.
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
#include "gamekiller.h"
#include "types.h"
#include "util.h"

/* #define GKDEBUG */

#ifdef GKDEBUG
#define DBG(x) printf x
#else
#define DBG(x)
#endif

/*
    ROBTEK "Game Killer"

    - 1 8k ROM
    - when cartridge is active, ultimax is enabled when addr>=e000, so the
      rom is visible at e000, below is normal c64 ram
    - the code writes 0 to both de00 and df00 to disable the cartridge. we
      assume the cart uses the full io1 and io2 range
    - when the freezer button is pressed the cartridge will be enabled and
      an NMI will be triggered
*/

#define GAME_KILLER_CART_SIZE (8*0x400)

/* ---------------------------------------------------------------------*/

static int cartridge_disable_flag;

static void REGPARM2 gamekiller_io1_store(WORD addr, BYTE value)
{
    DBG(("io1 %04x %02x\n",addr,value));
    cartridge_disable_flag++;
    if (cartridge_disable_flag > 1) {
        cartridge_config_changed(2, 2, CMODE_READ);
        DBG(("Game Killer disabled\n"));
    }
}

static void REGPARM2 gamekiller_io2_store(WORD addr, BYTE value)
{
    DBG(("io2 %04x %02x\n",addr,value));
    cartridge_disable_flag++;
    if (cartridge_disable_flag > 1) {
        cartridge_config_changed(2, 2, CMODE_READ);
        DBG(("Game Killer disabled\n"));
    }
}

BYTE REGPARM1 gamekiller_1000_7fff_read(WORD addr)
{
    return mem_read_without_ultimax(addr);
}

void REGPARM2 gamekiller_1000_7fff_store(WORD addr, BYTE value)
{
    mem_store_without_ultimax(addr, value);
}

BYTE REGPARM1 gamekiller_roml_read(WORD addr)
{
    return mem_read_without_ultimax(addr);
}

void REGPARM2 gamekiller_roml_store(WORD addr, BYTE value)
{
    mem_store_without_ultimax(addr, value);
}

BYTE REGPARM1 gamekiller_a000_bfff_read(WORD addr)
{
    return mem_read_without_ultimax(addr);
}

void REGPARM2 gamekiller_a000_bfff_store(WORD addr, BYTE value)
{
    mem_store_without_ultimax(addr, value);
}

BYTE REGPARM1 gamekiller_c000_cfff_read(WORD addr)
{
    return mem_read_without_ultimax(addr);
}

void REGPARM2 gamekiller_c000_cfff_store(WORD addr, BYTE value)
{
    mem_store_without_ultimax(addr, value);
}
/* ---------------------------------------------------------------------*/

static io_source_t gamekiller_io1_device = {
    "Game Killer",
    IO_DETACH_CART,
    NULL,
    0xde00, 0xdeff, 0xff,
    0, /* read is never valid */
    gamekiller_io1_store,
    NULL
};
static io_source_t gamekiller_io2_device = {
    "Game Killer",
    IO_DETACH_CART,
    NULL,
    0xdf00, 0xdfff, 0xff,
    0, /* read is never valid */
    gamekiller_io2_store,
    NULL
};

static io_source_list_t *gamekiller_io1_list_item = NULL;
static io_source_list_t *gamekiller_io2_list_item = NULL;

/* ---------------------------------------------------------------------*/

void gamekiller_freeze(void)
{
    DBG(("Game Killer freeze\n"));
    cartridge_config_changed(3, 3 | 0x40, CMODE_READ);
    cartridge_disable_flag = 0;
}


void gamekiller_config_init(void)
{
    cartridge_config_changed(3, 3, CMODE_READ);
    cartridge_disable_flag = 0;
}

void gamekiller_config_setup(BYTE *rawcart)
{
    memcpy(romh_banks, rawcart, GAME_KILLER_CART_SIZE);
    cartridge_config_changed(3, 3, CMODE_READ);
    cartridge_disable_flag = 0;
}

/* ---------------------------------------------------------------------*/

static const c64export_resource_t export_res = {
    "Game Killer", 1, 1
};

static int gamekiller_common_attach(void)
{
    if (c64export_add(&export_res) < 0) {
        return -1;
    }

    gamekiller_io1_list_item = c64io_register(&gamekiller_io1_device);
    gamekiller_io2_list_item = c64io_register(&gamekiller_io2_device);

    return 0;
}

int gamekiller_bin_attach(const char *filename, BYTE *rawcart)
{
    if (util_file_load(filename, rawcart, GAME_KILLER_CART_SIZE, UTIL_FILE_LOAD_SKIP_ADDRESS) < 0) {
        return -1;
    }

    return gamekiller_common_attach();
}

int gamekiller_crt_attach(FILE *fd, BYTE *rawcart)
{
    BYTE chipheader[0x10];

    if (fread(chipheader, 0x10, 1, fd) < 1) {
        return -1;
    }

    if (chipheader[0xb] > 0) {
        return -1;
    }

    if (fread(rawcart, GAME_KILLER_CART_SIZE, 1, fd) < 1) {
        return -1;
    }

    return gamekiller_common_attach();
}

void gamekiller_detach(void)
{
    c64export_remove(&export_res);
    c64io_unregister(gamekiller_io1_list_item);
    c64io_unregister(gamekiller_io2_list_item);
    gamekiller_io1_list_item = NULL;
    gamekiller_io2_list_item = NULL;
}

