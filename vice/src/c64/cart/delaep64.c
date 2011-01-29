/*
 * delaep64.c - Cartridge handling, Dela EP64 cart.
 *
 * Written by
 *  Michael Klein <michael.klein@puffin.lb.shuttle.de>
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

#define CARTRIDGE_INCLUDE_SLOTMAIN_API
#include "c64cartsystem.h"
#undef CARTRIDGE_INCLUDE_SLOTMAIN_API
#include "c64export.h"
#include "c64io.h"
#include "cartridge.h"
#include "delaep64.h"
#include "snapshot.h"
#include "types.h"
#include "util.h"
#include "vicii-phi1.h"

/*
 * for cart schematics, see http://a98.shuttle.de/~michael/dela-ep64/
 */


/*
    This is an eprom cartridge. It has 1 2764 (8Kb) which  holds  the  base
    eprom with the base menu, and 2 27256 eproms of  which  8Kb  parts  are
    banked into the $8000-9FFF area.

    The bank selecting is done by writing to $DE00. The following bits  are
    used for bank decoding in $DE00:

    bit   meaning
    ---   -------
     0    eprom selection bit 2
     1    eprom selection bit 3
     2    unused
     3    unused
     4    eprom selection bit 0
     5    eprom selection bit 1
     6    unused
     7    EXROM control (1=EXROM off, 0=EXROM on


    The following eprom select values result in the following bank selection:

    value   bank
    -----   ----
      0      0 (base)
      1      0 (base)
      2      0 (base)
      3      0 (base)
      4      1
      5      2
      6      3
      7      4
      8      5
      9      6
     10      7
     11      8
     12      0 (base)
     13      0 (base)
     14      0 (base)
     15      0 (base)
*/

/* ---------------------------------------------------------------------*/

static int currbank = 0;

static void delaep64_io1(BYTE value, unsigned int mode)
{
    BYTE bank, config;

    /* D7 -> EXROM */
    config = (value & 0x80) ? 2 : 0;
    cart_config_changed_slotmain(config, config, mode);

    /*
     * bank 0: 2764 (left socket)
     * bank 4-7: 1st 27256 (middle socket)
     * bank 7-11: 2nd 27256 (right socket)
     */
    bank = ((value & 0x30) >> 4) + ((value & 0x03) << 2);
    if (bank < 4 || bank > 11) {
        bank = 0;
    } else {
        bank = bank - 3;  /* turning the banks into 0-8 */
    }
    cart_romlbank_set_slotmain(bank);
    currbank = bank;
}

static BYTE delaep64_io1_read(WORD addr)
{
    BYTE value = vicii_read_phi1();

    delaep64_io1(value, CMODE_READ);

    return 0;
}

static BYTE delaep64_io1_peek(WORD addr)
{
    return currbank;
}

void delaep64_io1_store(WORD addr, BYTE value)
{
    delaep64_io1(value, CMODE_WRITE);
}

/* ---------------------------------------------------------------------*/

static io_source_t delaep64_device = {
    CARTRIDGE_NAME_DELA_EP64,
    IO_DETACH_CART,
    NULL,
    0xde00, 0xdeff, 0xff,
    0, /* read is never valid */
    delaep64_io1_store,
    delaep64_io1_read,
    delaep64_io1_peek,
    NULL, /* TODO: dump */
    CARTRIDGE_DELA_EP64
};

static io_source_list_t *delaep64_list_item = NULL;

static const c64export_resource_t export_res = {
    CARTRIDGE_NAME_DELA_EP64, 1, 0, &delaep64_device, NULL, CARTRIDGE_DELA_EP64
};

/* ---------------------------------------------------------------------*/

void delaep64_config_init(void)
{
    delaep64_io1(0, CMODE_READ);
}

/* FIXME: should copy rawcart to roml_banks ! */
void delaep64_config_setup(BYTE *rawcart)
{
    delaep64_io1(0, CMODE_READ);
}

/* ---------------------------------------------------------------------*/
static int delaep64_common_attach(void)
{
    if (c64export_add(&export_res) < 0) {
        return -1;
    }
    delaep64_list_item = c64io_register(&delaep64_device);
    return 0;
}

/* FIXME: this function should setup rawcart instead of copying to roml_banks ! */
/* FIXME: handle the various combinations / possible file lengths */
int delaep64_bin_attach(const char *filename, BYTE *rawcart)
{
    if (util_file_load(filename, roml_banks, 0x2000, UTIL_FILE_LOAD_SKIP_ADDRESS) < 0) {
        return -1;
    }
    return delaep64_common_attach();
}

/* FIXME: this function should setup rawcart instead of copying to roml_banks ! */
int delaep64_crt_attach(FILE *fd, BYTE *rawcart)
{
    WORD chip;
    WORD size;
    WORD rom_size = 0;
    BYTE chipheader[0x10];

    /*
     * 0x00000-0x01fff: 2764
     * 0x02000-0x09fff: 1st 27256
     * 0x0a000-0x11fff: 2nd 27256
     */
    memset(roml_banks, 0xff, 0x12000);

    if (fread(chipheader, 0x10, 1, fd) < 1) {
        return -1;
    }

    chip = (chipheader[0x0a] << 8) + chipheader[0x0b];
    size = (chipheader[0x0e] << 8) + chipheader[0x0f];

    /* First handle the base image */
    if (size != 0x2000) {
        return -1;
    }

    if (fread(roml_banks, 0x2000, 1, fd) < 1) {
        return -1;
    }

    while (1) {
        if (fread(chipheader, 0x10, 1, fd) < 1) {
            break;
        }

        chip = (chipheader[0x0a] << 8) + chipheader[0x0b];
        size = (chipheader[0x0e] << 8) + chipheader[0x0f];

        /* check for the size of the following rom images,
           they can be of either 0x2000 or 0x8000 */
        if (size != 0x2000 && size != 0x8000) {
            return -1;
        }

        /* make sure all rom images are of the same size */
        if (rom_size == 0) {
            rom_size = size;
        } else {
            if (size != rom_size) {
                return -1;
            }
        }

        /* maximum of 2 32kb or 8 8kb images allowed */
        if ((rom_size == 0x8000 && chip > 2) || (rom_size == 0x2000 && chip > 8)) {
            return -1;
        }

        /* put the images in the right place */
        if (fread(roml_banks + 0x2000 + ((chip - 1) * rom_size), size , 1, fd) < 1) {
            return -1;
        }
    }
    return delaep64_common_attach();
}

void delaep64_detach(void)
{
    c64export_remove(&export_res);
    c64io_unregister(delaep64_list_item);
    delaep64_list_item = NULL;
}

/* ---------------------------------------------------------------------*/

#define CART_DUMP_VER_MAJOR   0
#define CART_DUMP_VER_MINOR   0
#define SNAP_MODULE_NAME  "CARTDELAEP64"

int delaep64_snapshot_write_module(snapshot_t *s)
{
    snapshot_module_t *m;

    m = snapshot_module_create(s, SNAP_MODULE_NAME,
                          CART_DUMP_VER_MAJOR, CART_DUMP_VER_MINOR);
    if (m == NULL) {
        return -1;
    }

    if (0
        || (SMW_B(m, (BYTE)currbank) < 0)
        || (SMW_BA(m, roml_banks, 0x2000 * 9) < 0)) {
        snapshot_module_close(m);
        return -1;
    }

    snapshot_module_close(m);
    return 0;
}

int delaep64_snapshot_read_module(snapshot_t *s)
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
        || (SMR_B_INT(m, &currbank) < 0)
        || (SMR_BA(m, roml_banks, 0x2000 * 9) < 0)) {
        snapshot_module_close(m);
        return -1;
    }

    snapshot_module_close(m);

    return delaep64_common_attach();
}
