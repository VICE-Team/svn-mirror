/*
 * snapshot64.c - Cartridge handling, Super Snapshot cart.
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
#include <string.h>

#include "c64cart.h"
#include "c64cartmem.h"
#include "c64export.h"
#include "c64io.h"
#include "snapshot64.h"
#include "types.h"
#include "util.h"

/*
    Snapshot 64 (LMS Technologies)

    FIXME: this implementation is purely based on guesswork

    - 4K ROM (2732)
      - mapped to 8000 and e000 (ultimax mode) ?
    - NE555, 7406, 7474
    - one button (freeze)

    io2 - df00 (r/w)
     0 - cart off
     1 - cart enabled

    after reset NO menu is shown. when pressing freeze the screen goes blank, then

    return - starts the "code inspector"
       l - load and execute ml program
       j - jump to address
       d - dump memory (hex)
       e - return to basic
    f1 - analyze and save backup
    f3 - clear memory
    f5 - format disk
    f7 - restart

*/

/* #define SS64DEBUG */

#ifdef SS64DEBUG
#define DBG(x) printf x
#else
#define DBG(x)
#endif

static BYTE romconfig = 0;

/* ---------------------------------------------------------------------*/

/* some prototypes are needed */
static BYTE REGPARM1 snapshot64_io2_read(WORD addr);
static void REGPARM2 snapshot64_io2_store(WORD addr, BYTE value);

static io_source_t ss64_io2_device = {
    "Snapshot 64",
    IO_DETACH_CART,
    NULL,
    0xdf00, 0xdfff, 0xff,
    0,
    snapshot64_io2_store,
    snapshot64_io2_read
};

static io_source_list_t *ss64_io2_list_item = NULL;

/* ---------------------------------------------------------------------*/

BYTE REGPARM1 snapshot64_io2_read(WORD addr)
{
    DBG(("io2 rd %04x\n", addr));

    ss64_io2_device.io_source_valid = 1;

    if ((addr & 0xff) == 0) {
        return romconfig;
    }

    ss64_io2_device.io_source_valid = 0;
    return 0;

}

void REGPARM2 snapshot64_io2_store(WORD addr, BYTE value)
{
    DBG(("io2 wr %04x %02x\n", addr, value));

    romconfig = value & 1;

    if ((addr & 0xff) == 0) {
        if (romconfig == 0) {
            cartridge_config_changed(2, 2, CMODE_WRITE);
        } else {
            cartridge_config_changed(3, 3, CMODE_WRITE);
        }
    }
}

/* ---------------------------------------------------------------------*/

void snapshot64_freeze(void)
{
    cartridge_config_changed(3, 3, CMODE_READ);
}

void snapshot64_config_init(void)
{
    DBG(("snapshot64_config_init\n"));
    romconfig = 0;
    cartridge_config_changed(2, 2, CMODE_READ);
}

void snapshot64_config_setup(BYTE *rawcart)
{
    memcpy(&roml_banks[0x0000], &rawcart[0x0000], 0x1000);
    memcpy(&roml_banks[0x1000], &rawcart[0x0000], 0x1000);
    memcpy(&romh_banks[0x0000], &rawcart[0x0000], 0x1000);
    memcpy(&romh_banks[0x1000], &rawcart[0x0000], 0x1000);
    cartridge_config_changed(2, 2, CMODE_READ);
}

/* ---------------------------------------------------------------------*/

static const c64export_resource_t export_res_v4 = {
    "Snapshot 64", 1, 1
};

int snapshot64_bin_attach(const char *filename, BYTE *rawcart)
{
    if (util_file_load(filename, rawcart, 0x1000, UTIL_FILE_LOAD_SKIP_ADDRESS) < 0) {
        return -1;
    }

    if (c64export_add(&export_res_v4) < 0) {
        return -1;
    }

    ss64_io2_list_item = c64io_register(&ss64_io2_device);

    return 0;
}

void snapshot64_detach(void)
{
    c64export_remove(&export_res_v4);
    c64io_unregister(ss64_io2_list_item);
    ss64_io2_list_item = NULL;
}
