/*
 * diashowmaker.c - Cartridge handling, Diashow Maker cart.
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
#include "diashowmaker.h"
#include "snapshot.h"
#include "types.h"
#include "util.h"

/* #define DSMDEBUG */

#ifdef DSMDEBUG
#define DBG(x) printf x
#else
#define DBG(x)
#endif

/*
    "Diashow Maker" (c) M.Grieb & B.Trenkel

    - 8k ROM

    - accessing io1 (the software uses de00 only it seems)
      - disables cartridge ROM

    - reset
      - enables 8K game mode
      - ROM bank is mapped to 8000

    - freeze
      - ROM is mapped to 8000

    freeze, then:
        return:     main menu
        space:      dos-kit/fastload
        arrow left: normal reset
        f7:         cycle video banks (?)

*/

#define DSM_CART_SIZE (8*0x400)

/* ---------------------------------------------------------------------*/

static BYTE REGPARM1 dsm_io1_read(WORD addr)
{
    DBG(("io1 r %04x\n", addr));
    if (addr == 0) {
        cartridge_config_changed(2, 2, CMODE_READ);
        DBG(("Diashow Maker disabled\n"));
    }
    return 0; /* invalid */
}

static BYTE REGPARM1 dsm_io1_peek(WORD addr)
{
    return 0;
}

static void REGPARM2 dsm_io1_store(WORD addr, BYTE value)
{
    DBG(("io1 w %04x %02x\n", addr, value));
    if (addr == 0) {
        cartridge_config_changed(2, 2, CMODE_READ);
        DBG(("Diashow Maker disabled\n"));
    }
}

static io_source_t dsm_io1_device = {
    CARTRIDGE_NAME_DIASHOW_MAKER,
    IO_DETACH_CART,
    NULL,
    0xde00, 0xdeff, 0xff,
    0, /* read is never valid */
    dsm_io1_store,
    dsm_io1_read,
    dsm_io1_peek,
    NULL,
    CARTRIDGE_DIASHOW_MAKER
};

static io_source_list_t *dsm_io1_list_item = NULL;

static const c64export_resource_t export_res = {
    CARTRIDGE_NAME_DIASHOW_MAKER, 1, 1, &dsm_io1_device, NULL, CARTRIDGE_DIASHOW_MAKER
};

/* ---------------------------------------------------------------------*/

void dsm_freeze(void)
{
    DBG(("Diashow Maker: freeze\n"));
    cartridge_config_changed(0, 0, CMODE_READ | CMODE_RELEASE_FREEZE);
}


void dsm_config_init(void)
{
    cartridge_config_changed(0, 0, CMODE_READ);
}

void dsm_config_setup(BYTE *rawcart)
{
    memcpy(roml_banks, rawcart, DSM_CART_SIZE);
    cartridge_config_changed(0, 0, CMODE_READ);
}

/* ---------------------------------------------------------------------*/

static int dsm_common_attach(void)
{
    if (c64export_add(&export_res) < 0) {
        return -1;
    }

    dsm_io1_list_item = c64io_register(&dsm_io1_device);
    return 0;
}

int dsm_bin_attach(const char *filename, BYTE *rawcart)
{
    DBG(("Diashow Maker: bin attach '%s'\n", filename));
    if (util_file_load(filename, rawcart, DSM_CART_SIZE, UTIL_FILE_LOAD_SKIP_ADDRESS) < 0) {
        return -1;
    }

    return dsm_common_attach();
}

int dsm_crt_attach(FILE *fd, BYTE *rawcart)
{
    BYTE chipheader[0x10];

    if (fread(chipheader, 0x10, 1, fd) < 1) {
        return -1;
    }

    if (chipheader[0xb] > 0) {
        return -1;
    }

    if (fread(rawcart, DSM_CART_SIZE, 1, fd) < 1) {
        return -1;
    }

    return dsm_common_attach();
}

void dsm_detach(void)
{
    c64export_remove(&export_res);
    c64io_unregister(dsm_io1_list_item);
    dsm_io1_list_item = NULL;
}

/* ---------------------------------------------------------------------*/

#define CART_DUMP_VER_MAJOR   0
#define CART_DUMP_VER_MINOR   0
#define SNAP_MODULE_NAME  "CARTDSM"

int dsm_snapshot_write_module(snapshot_t *s)
{
    snapshot_module_t *m;

    m = snapshot_module_create(s, SNAP_MODULE_NAME,
                          CART_DUMP_VER_MAJOR, CART_DUMP_VER_MINOR);
    if (m == NULL) {
        return -1;
    }

    if (0
        || (SMW_BA(m, roml_banks, DSM_CART_SIZE) < 0)) {
        snapshot_module_close(m);
        return -1;
    }

    snapshot_module_close(m);
    return 0;
}

int dsm_snapshot_read_module(snapshot_t *s)
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
        || (SMR_BA(m, roml_banks, DSM_CART_SIZE) < 0)) {
        snapshot_module_close(m);
        return -1;
    }

    snapshot_module_close(m);

    return dsm_common_attach();
}
