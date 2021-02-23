
/*
 * multicart.h - plus4 multi cart handling
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
    "Plus4 multi cart"

    - 32/64 banks mapped to c1lo and c1hi
    - bank register at $FDA0
 */

#define DEBUG_MULTICART

#include "vice.h"

#include <stdio.h>
#include <string.h>

#include "archdep.h"
#include "cartridge.h"
#include "cartio.h"
#include "plus4cart.h"
#include "plus4mem.h"
#include "util.h"

#include "multicart.h"

#ifdef DEBUG_MULTICART
#define DBG(x)  printf x
#else
#define DBG(x)
#endif

static int bankreg = 0;
static int multicart_filesize = 0;
static int multicart_filetype = 0;

static unsigned char multicartromlo[0x200000]; /* FIXME: dynamically allocate this */
static unsigned char multicartromhi[0x200000]; /* FIXME: dynamically allocate this */

/* a prototype is needed */
static void multicart_store(uint16_t addr, uint8_t value);

/* This is not a real cartridge, it is only used for debugging purposes */
static io_source_t multicart_device = {
    CARTRIDGE_PLUS4_NAME_MULTI, /* name of the device */
    IO_DETACH_CART,           /* use cartridge ID to detach the device when involved in a read-collision */
    IO_DETACH_NO_RESOURCE,    /* does not use a resource for detach */
    0xfda0, 0xfda0, 0xff,     /* range for the device, reg:$fda0 */
    0,                        /* read is never valid, device is write only */
    multicart_store,          /* store function */
    NULL,                     /* NO poke function */
    NULL,                     /* NO read function */
    NULL,                     /* NO peek function */
    NULL,                     /* nothing to dump */
    CARTRIDGE_PLUS4_MULTI,    /* cartridge ID */
    IO_PRIO_NORMAL,           /* normal priority, device read needs to be checked for collisions */
    0                         /* insertion order, gets filled in by the registration function */
};

static io_source_list_t *multicart_list_item = NULL;

static void multicart_store(uint16_t addr, uint8_t value)
{
    bankreg = value;
    DBG(("multicart_store %04x %02x\n", addr, value));
}

uint8_t multicart_c1lo_read(uint16_t addr)
{
    unsigned int offset = ((addr & 0x3fff) + (bankreg * 0x4000)) & ((multicart_filesize - 1) >> 1);
    /* DBG(("multicart_c1lo_read %06x bank: %d\n", offset, bankreg)); */
    return multicartromlo[offset];
}

uint8_t multicart_c1hi_read(uint16_t addr)
{
    unsigned int offset = ((addr & 0x3fff) + (bankreg * 0x4000)) & ((multicart_filesize - 1) >> 1);
    /* DBG(("multicart_c1hi_read %06x bank: %d\n", offset, bankreg)); */
    return multicartromhi[offset];
}

void multicart_reset(void)
{
    DBG(("multicart_reset\n"));
    bankreg = 0;
}

void multicart_config_setup(uint8_t *rawcart)
{
    unsigned int half = multicart_filesize >> 1;
    DBG(("multicart_config_setup\n"));
    memcpy(multicartromlo, rawcart, half);
    memcpy(multicartromhi, rawcart + half, half);
}

static int multicart_common_attach(void)
{
    DBG(("multicart_common_attach\n"));
    multicart_list_item = io_source_register(&multicart_device);

    return 0;
}

int multicart_bin_attach(const char *filename, uint8_t *rawcart)
{
    FILE *fd;
    unsigned int len;

    multicart_filetype = 0;
    multicart_filesize = 0;

    DBG(("multicart_bin_attach '%s'\n", filename));

    fd = fopen(filename, MODE_READ);
    if (fd == NULL) {
        return -1;
    }
    len = (unsigned int)util_file_length(fd);
    fclose(fd);

    DBG(("multicart_bin_attach len: %04x\n", len));

    memset(rawcart, 0xff, 0x400000); /* FIXME */

    /* we accept 2MiB/4MiB images */
    switch (len) {
        case 0x200000:
            if (util_file_load(filename, rawcart, 0x200000, UTIL_FILE_LOAD_SKIP_ADDRESS) < 0) {
                return -1;
            }
            break;
        case 0x400000:
            if (util_file_load(filename, rawcart, 0x400000, UTIL_FILE_LOAD_SKIP_ADDRESS) < 0) {
                return -1;
            }
            break;
        default:
            return -1;
    }

    multicart_filesize = len;
    multicart_filetype = CARTRIDGE_FILETYPE_BIN;
    return multicart_common_attach();
}

void multicart_detach(void)
{
    DBG(("multicart_detach\n"));
    io_source_unregister(multicart_list_item);
}
