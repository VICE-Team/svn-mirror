
/*
 * jacint1mb.h - 1MB Cartridge handling
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
    "1MB Cartridge"

    - 64 banks mapped to c1lo
    - bank register at $FE00
 */

#define DEBUG_JACINT1MB

#include "vice.h"

#include <stdio.h>
#include <string.h>

#include "archdep.h"
#include "cartridge.h"
#include "cartio.h"
#include "plus4cart.h"
#include "plus4mem.h"
#include "util.h"

#include "jacint1mb.h"

#ifdef DEBUG_JACINT1MB
#define DBG(x)  printf x
#else
#define DBG(x)
#endif

#define JACINT1MBROMSIZE    0x100000

static int bankreg = 0;
static int jacint1mb_filetype = 0;

static unsigned char jacint1mbrom[JACINT1MBROMSIZE]; /* FIXME: dynamically allocate this */

/* a prototype is needed */
static void jacint1mb_store(uint16_t addr, uint8_t value);

/* This is not a real cartridge, it is only used for debugging purposes */
static io_source_t jacint1mb_device = {
    CARTRIDGE_PLUS4_NAME_JACINT1MB, /* name of the device */
    IO_DETACH_CART,           /* use cartridge ID to detach the device when involved in a read-collision */
    IO_DETACH_NO_RESOURCE,    /* does not use a resource for detach */
    0xfe00, 0xfe00, 0xff,     /* range for the device, reg:$fe00 */
    0,                        /* read is never valid, device is write only */
    jacint1mb_store,          /* store function */
    NULL,                     /* NO poke function */
    NULL,                     /* NO read function */
    NULL,                     /* NO peek function */
    NULL,                     /* nothing to dump */
    CARTRIDGE_PLUS4_JACINT1MB,    /* cartridge ID */
    IO_PRIO_NORMAL,           /* normal priority, device read needs to be checked for collisions */
    0                         /* insertion order, gets filled in by the registration function */
};

static io_source_list_t *jacint1mb_list_item = NULL;

static void jacint1mb_store(uint16_t addr, uint8_t value)
{
    bankreg = value;
    DBG(("jacint1mb_store %04x %02x\n", addr, value));
}

uint8_t jacint1mb_c1lo_read(uint16_t addr)
{
    unsigned int offset = ((addr & 0x3fff) + (bankreg * 0x4000)) & (JACINT1MBROMSIZE - 1);
    /* DBG(("jacint1mb_c1lo_read %06x bank: %d\n", offset, bankreg)); */
    return jacint1mbrom[offset];
}

void jacint1mb_reset(void)
{
    DBG(("jacint1mb_reset\n"));
    bankreg = 0;
}

void jacint1mb_config_setup(uint8_t *rawcart)
{
    DBG(("jacint1mb_config_setup\n"));
    memcpy(jacint1mbrom, rawcart, JACINT1MBROMSIZE);
}

static int jacint1mb_common_attach(void)
{
    DBG(("jacint1mb_common_attach\n"));
    jacint1mb_list_item = io_source_register(&jacint1mb_device);

    return 0;
}

int jacint1mb_bin_attach(const char *filename, uint8_t *rawcart)
{
    jacint1mb_filetype = 0;

    DBG(("jacint1mb_bin_attach '%s'\n", filename));

    /* we accept 1MiB images */
    if (util_file_load(filename, rawcart, JACINT1MBROMSIZE, UTIL_FILE_LOAD_SKIP_ADDRESS) < 0) {
        return -1;
    }

    jacint1mb_filetype = CARTRIDGE_FILETYPE_BIN;
    return jacint1mb_common_attach();
}

void jacint1mb_detach(void)
{
    DBG(("jacint1mb_detach\n"));
    io_source_unregister(jacint1mb_list_item);
}
