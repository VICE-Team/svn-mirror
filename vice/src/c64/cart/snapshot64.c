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

#define CARTRIDGE_INCLUDE_SLOTMAIN_API
#include "c64cartsystem.h"
#undef CARTRIDGE_INCLUDE_SLOTMAIN_API
#include "c64export.h"
#include "c64io.h"
#include "c64mem.h"
#include "cartridge.h"
#include "snapshot.h"
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
    bit 0:  0 - cart off  1 - cart enabled

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
    CARTRIDGE_NAME_SNAPSHOT64,
    IO_DETACH_CART,
    NULL,
    0xdf00, 0xdfff, 0xff,
    0,
    snapshot64_io2_store,
    snapshot64_io2_read,
    NULL, /* reads have no side effect */
    NULL, /* TODO: dump */
    CARTRIDGE_SNAPSHOT64
};

static io_source_list_t *ss64_io2_list_item = NULL;

static const c64export_resource_t export_res = {
    CARTRIDGE_NAME_SNAPSHOT64, 1, 1, NULL, &ss64_io2_device, CARTRIDGE_SNAPSHOT64
};

/* ---------------------------------------------------------------------*/

BYTE REGPARM1 snapshot64_io2_read(WORD addr)
{
    DBG(("io2 rd %04x (%02x)\n", addr, romconfig));

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

    if ((addr & 0xff) == 0) {

        romconfig = value & 1;

        if (romconfig == 0) {
            cart_config_changed_slotmain(2, 2, CMODE_WRITE);
/*            cart_config_changed_slotmain(2, 2, CMODE_WRITE | CMODE_RELEASE_FREEZE); */
        } else {
            cart_config_changed_slotmain(3, 3, CMODE_WRITE);
/*            cart_config_changed_slotmain(3, 3, CMODE_WRITE | CMODE_RELEASE_FREEZE); */
        }
    }
}

/* ---------------------------------------------------------------------*/

BYTE REGPARM1 snapshot64_roml_read(WORD addr)
{
#if 1
    if (addr < 0x9000) {
        return roml_banks[addr & 0x0fff];
    } else {
        return mem_read_without_ultimax(addr);
    }
#else
    return roml_banks[addr & 0x0fff];
#endif
}

BYTE REGPARM1 snapshot64_romh_read(WORD addr)
{
#if 1
    if (addr >= 0xf000) {
        return roml_banks[addr & 0x0fff];
    } else {
        return mem_read_without_ultimax(addr);
    }
#else
    return roml_banks[addr & 0x0fff];
#endif
}

/* ---------------------------------------------------------------------*/

void snapshot64_freeze(void)
{
    DBG(("SNAPSHOT64: freeze\n"));
    romconfig = 1;
    cart_config_changed_slotmain(3, 3, CMODE_READ | CMODE_RELEASE_FREEZE);
/*    cart_config_changed_slotmain(3, 3, CMODE_READ); */
}

void snapshot64_config_init(void)
{
    DBG(("SNAPSHOT64: config_init\n"));
    romconfig = 0;
    cart_config_changed_slotmain(2, 2, CMODE_READ);
}

void snapshot64_config_setup(BYTE *rawcart)
{
    DBG(("SNAPSHOT64: config setup\n"));
    memcpy(&roml_banks[0x0000], &rawcart[0x0000], 0x1000);
    cart_config_changed_slotmain(2, 2, CMODE_READ);
}

/* ---------------------------------------------------------------------*/

static int snapshot64_common_attach(void)
{
    if (c64export_add(&export_res) < 0) {
        return -1;
    }

    ss64_io2_list_item = c64io_register(&ss64_io2_device);
    return 0;
}

int snapshot64_bin_attach(const char *filename, BYTE *rawcart)
{
    if (util_file_load(filename, rawcart, 0x1000, UTIL_FILE_LOAD_SKIP_ADDRESS) < 0) {
        return -1;
    }
    return snapshot64_common_attach();
}

int snapshot64_crt_attach(FILE *fd, BYTE *rawcart)
{
    BYTE chipheader[0x10];

    if (fread(chipheader, 0x10, 1, fd) < 1) {
        return -1;
    }

    if (chipheader[0xb] > 0) {
        return -1;
    }

    if (fread(rawcart, 0x1000, 1, fd) < 1) {
        return -1;
    }

    return snapshot64_common_attach();
}

void snapshot64_detach(void)
{
    c64export_remove(&export_res);
    c64io_unregister(ss64_io2_list_item);
    ss64_io2_list_item = NULL;
}

/* ---------------------------------------------------------------------*/

#define CART_DUMP_VER_MAJOR   0
#define CART_DUMP_VER_MINOR   0
#define SNAP_MODULE_NAME  "CARTSNAP64"

int snapshot64_snapshot_write_module(snapshot_t *s)
{
    snapshot_module_t *m;

    m = snapshot_module_create(s, SNAP_MODULE_NAME,
                          CART_DUMP_VER_MAJOR, CART_DUMP_VER_MINOR);
    if (m == NULL) {
        return -1;
    }

    if (0
        || (SMW_B(m, romconfig) < 0)
        || (SMW_BA(m, roml_banks, 0x1000) < 0)) {
        snapshot_module_close(m);
        return -1;
    }

    snapshot_module_close(m);
    return 0;
}

int snapshot64_snapshot_read_module(snapshot_t *s)
{
    BYTE vmajor, vminor;
    snapshot_module_t *m;

    m = snapshot_module_open(s, SNAP_MODULE_NAME, &vmajor, &vminor);
    if (m == NULL) {
        return -1;
    }

    if ((vmajor != CART_DUMP_VER_MAJOR) || (vminor != CART_DUMP_VER_MINOR)) {
        snapshot_module_close(m);
        return -1;
    }

    if (0
        || (SMR_B(m, &romconfig) < 0)
        || (SMR_BA(m, roml_banks, 0x1000) < 0)) {
        snapshot_module_close(m);
        return -1;
    }

    snapshot_module_close(m);

    return snapshot64_common_attach();
}
