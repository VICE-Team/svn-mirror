
/*
 * magiccart.h - c264 magic cart handling
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

/*
    "c264 magic cart"

    - 32/64/128 banks mapped to c1lo
    - bank register at $FDFE
 */

#define DEBUG_MAGICCART

#include "vice.h"

#include <stdio.h>
#include <string.h>

#include "archdep.h"
#include "cartridge.h"
#include "cartio.h"
#include "lib.h"
#include "monitor.h"
#include "plus4cart.h"
#include "plus4mem.h"
#include "snapshot.h"
#include "util.h"

#include "magiccart.h"

#ifdef DEBUG_MAGICCART
#define DBG(x)  printf x
#else
#define DBG(x)
#endif

static int bankreg = 0;
static int magiccart_filesize = 0;
static int magiccart_filetype = 0;

static unsigned char *magiccartrom = NULL;

/* a prototype is needed */
static void magiccart_store(uint16_t addr, uint8_t value);
static int magiccart_dump(void);

/* This is not a real cartridge, it is only used for debugging purposes */
static io_source_t magiccart_device = {
    CARTRIDGE_PLUS4_NAME_MAGIC, /* name of the device */
    IO_DETACH_CART,             /* use cartridge ID to detach the device when involved in a read-collision */
    IO_DETACH_NO_RESOURCE,      /* does not use a resource for detach */
    0xfdfe, 0xfdfe, 0xff,       /* range for the device, reg:$fdfe */
    0,                          /* read is never valid, device is write only */
    magiccart_store,            /* store function */
    NULL,                       /* NO poke function */
    NULL,                       /* NO read function */
    NULL,                       /* NO peek function */
    magiccart_dump,             /* dump function for the monitor */
    CARTRIDGE_PLUS4_MAGIC,      /* cartridge ID */
    IO_PRIO_NORMAL,             /* normal priority, device read needs to be checked for collisions */
    0                           /* insertion order, gets filled in by the registration function */
};

static io_source_list_t *magiccart_list_item = NULL;

static int magiccart_dump(void)
{
    mon_out("ROM bank: %d\n", bankreg);
    return 0;
}

static void magiccart_store(uint16_t addr, uint8_t value)
{
    bankreg = value;
    DBG(("magiccart_store %04x %02x\n", addr, value));
}

uint8_t magiccart_c1lo_read(uint16_t addr)
{
    unsigned int offset = ((addr & 0x3fff) + (bankreg * 0x4000)) & (magiccart_filesize - 1);
    /* DBG(("magiccart_c1lo_read %06x bank: %d\n", offset, bankreg)); */
    return magiccartrom[offset];
}

void magiccart_reset(void)
{
    DBG(("magiccart_reset\n"));
    bankreg = 0;
}

void magiccart_config_setup(uint8_t *rawcart)
{
    DBG(("magiccart_config_setup\n"));
    memcpy(magiccartrom, rawcart, 0x200000);
}

static int magiccart_common_attach(void)
{
    DBG(("magiccart_common_attach\n"));

    if(!(magiccartrom = lib_malloc(magiccart_filesize))) {
        return -1;
    }

    magiccart_list_item = io_source_register(&magiccart_device);

    return 0;
}

int magiccart_bin_attach(const char *filename, uint8_t *rawcart)
{
    FILE *fd;
    unsigned int len;

    magiccart_filetype = 0;
    magiccart_filesize = 0;

    DBG(("magiccart_bin_attach '%s'\n", filename));

    fd = fopen(filename, MODE_READ);
    if (fd == NULL) {
        return -1;
    }
    len = (unsigned int)util_file_length(fd);
    fclose(fd);

    DBG(("magiccart_bin_attach len: %04x\n", len));

    memset(rawcart, 0xff, 0x200000); /* FIXME */

    /* we accept 512KiB/1MiB/2MiB images */
    switch (len) {
        case 0x80000:
            if (util_file_load(filename, rawcart, 0x80000, UTIL_FILE_LOAD_SKIP_ADDRESS) < 0) {
                return -1;
            }
            break;
        case 0x100000:
            if (util_file_load(filename, rawcart, 0x100000, UTIL_FILE_LOAD_SKIP_ADDRESS) < 0) {
                return -1;
            }
            break;
        case 0x200000:
            if (util_file_load(filename, rawcart, 0x200000, UTIL_FILE_LOAD_SKIP_ADDRESS) < 0) {
                return -1;
            }
            break;
        default:
            return -1;
    }

    magiccart_filesize = len;
    magiccart_filetype = CARTRIDGE_FILETYPE_BIN;
    return magiccart_common_attach();
}

void magiccart_detach(void)
{
    DBG(("magiccart_detach\n"));
    io_source_unregister(magiccart_list_item);
    lib_free(magiccartrom);
    magiccartrom = NULL;
}

/* ---------------------------------------------------------------------*/

/* CARTMAGIC snapshot module format:

   type  | name              | version | description
   -------------------------------------------------
   BYTE  | bankreg           |   0.1+  | state of banking register
   DWORD | filesize          |   0.1+  | size of the ROM
   ARRAY | ROM               |   0.1+  | 512KiB/1MiB/2MiB of ROM data
 */

/* FIXME: since we cant actually make snapshots due to TED bugs, the following
          is completely untested */

static const char snap_module_name[] = "CARTMAGIC";
#define SNAP_MAJOR   0
#define SNAP_MINOR   1

int magiccart_snapshot_write_module(snapshot_t *s)
{
    snapshot_module_t *m;

    DBG(("magiccart_snapshot_write_module\n"));

    m = snapshot_module_create(s, snap_module_name, SNAP_MAJOR, SNAP_MINOR);

    if (m == NULL) {
        return -1;
    }

    if (0
        || SMW_B(m, (uint8_t)bankreg) < 0
        || SMW_DW(m, (uint32_t)magiccart_filesize) < 0
        || SMW_BA(m, magiccartrom, magiccart_filesize) < 0) {
        snapshot_module_close(m);
        return -1;
    }

    snapshot_module_close(m);

    return 0;
}

int magiccart_snapshot_read_module(snapshot_t *s)
{
    uint8_t vmajor, vminor;
    snapshot_module_t *m;
    uint32_t temp_filesize;

    DBG(("magiccart_snapshot_read_module\n"));

    m = snapshot_module_open(s, snap_module_name, &vmajor, &vminor);

    if (m == NULL) {
        return -1;
    }

    /* Do not accept versions higher than current */
    if (snapshot_version_is_bigger(vmajor, vminor, SNAP_MAJOR, SNAP_MINOR)) {
        snapshot_set_error(SNAPSHOT_MODULE_HIGHER_VERSION);
        goto fail;
    }

    if (0
        || SMR_B_INT(m, &bankreg) < 0
        || SMR_DW(m, &temp_filesize) < 0
        ) {
        goto fail;
    }

    magiccart_filesize = temp_filesize;

    if (0
        || SMR_BA(m, magiccartrom, magiccart_filesize) < 0) {
        goto fail;
    }

    snapshot_module_close(m);

    magiccart_common_attach();

    /* set filetype to none */
    magiccart_filetype = 0;

    return 0;

fail:
    snapshot_module_close(m);
    return -1;
}


