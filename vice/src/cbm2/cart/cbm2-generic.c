/*
 * cbm2-generic.c -- CBM2 generic cartridge handling.
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

/* #define DEBUGGENERIC */

#include "vice.h"

#include <string.h>

#include "archdep.h"
#include "cartridge.h"
#include "cartio.h"
#include "cbm2cart.h"
#include "cbm2mem.h"
#include "cbm2rom.h"
#include "cmdline.h"
#include "crt.h"
#include "lib.h"
#include "log.h"
#include "machine.h"
#include "resources.h"
#include "snapshot.h"
#include "util.h"

#include "cbm2-generic.h"

#ifdef DEBUGGENERIC
#define DBG(x)  log_debug x
#else
#define DBG(x)
#endif

static int generic_type = 0;
static int generic_filetype = 0;

/* Name of the external cartridge ROMs.  */
static char *cart_1_name = NULL;
static char *cart_2_name = NULL;
static char *cart_4_name = NULL;
static char *cart_6_name = NULL;

/*
    unlike in the other emulators, there are no separate arrays for the cartridge
    images, we'll have to use the mem_rom array with offsets instead:

*/

#define MEMPTR_CART1    (mem_rom + 0x1000)  /* 4k */
#define MEMPTR_CART2    (mem_rom + 0x2000)  /* 8k */
#define MEMPTR_CART4    (mem_rom + 0x4000)  /* 8k */
#define MEMPTR_CART6    (mem_rom + 0x6000)  /* 8k */

#define CBM2_C1_ROM_SIZE    0x1000
#define CBM2_C2_ROM_SIZE    0x2000
#define CBM2_C4_ROM_SIZE    0x2000
#define CBM2_C6_ROM_SIZE    0x2000

/* FIXME: this function must check the actual generic type and then
          update C1/C2/C4/C6 accordingly */
void generic_config_setup(uint8_t *rawcart)
{
    DBG(("generic_config_setup"));
    if ((generic_type & CARTRIDGE_CBM2_GENERIC_TYPE_MASK) & CARTRIDGE_CBM2_GENERIC_C1) {
        memcpy(MEMPTR_CART1, rawcart + 0x1000, CBM2_C1_ROM_SIZE);
        DBG(("generic_config_setup c1"));
    }
    if ((generic_type & CARTRIDGE_CBM2_GENERIC_TYPE_MASK) & CARTRIDGE_CBM2_GENERIC_C2) {
        memcpy(MEMPTR_CART2, rawcart + 0x2000, CBM2_C2_ROM_SIZE);
        DBG(("generic_config_setup c2"));
    }
    if ((generic_type & CARTRIDGE_CBM2_GENERIC_TYPE_MASK) & CARTRIDGE_CBM2_GENERIC_C4) {
        memcpy(MEMPTR_CART4, rawcart + 0x4000, CBM2_C4_ROM_SIZE);
        DBG(("generic_config_setup c4"));
    }
    if ((generic_type & CARTRIDGE_CBM2_GENERIC_TYPE_MASK) & CARTRIDGE_CBM2_GENERIC_C6) {
        memcpy(MEMPTR_CART6, rawcart + 0x6000, CBM2_C6_ROM_SIZE);
        DBG(("generic_config_setup c6"));
    }
}

/* FIXME: alloc ROMs here */
static int generic_common_attach(void)
{
    DBG(("generic_common_attach (type :%04x)", (unsigned)generic_type));

    return generic_type;
}

/* since we also need to handle adding to existing carts, copy the old
   content to the new rawcart first */
static void prepare_rawcart(uint8_t *rawcart)
{
    DBG(("prepare_rawcart"));
    memcpy(rawcart + 0x1000, MEMPTR_CART1, CBM2_C1_ROM_SIZE);
    memcpy(rawcart + 0x2000, MEMPTR_CART2, CBM2_C2_ROM_SIZE);
    memcpy(rawcart + 0x4000, MEMPTR_CART4, CBM2_C4_ROM_SIZE);
    memcpy(rawcart + 0x6000, MEMPTR_CART6, CBM2_C6_ROM_SIZE);
}

/* FIXME: handle mirroring of 4k/8k ROMs */
int generic_bin_attach(int type, const char *filename, uint8_t *rawcart)
{
    FILE *fd;
    int i;
    int offset = 0;
    int length = 0;

    DBG(("generic_bin_attach type: %04x", (unsigned)type));

    fd = fopen(filename, "rb");
    if (fd == NULL) {
        return -1;
    }

    prepare_rawcart(rawcart);

    /* get offset of first block */
    switch (type & CARTRIDGE_CBM2_GENERIC_TYPE_MASK) {
        case CARTRIDGE_CBM2_GENERIC_C1 & CARTRIDGE_CBM2_GENERIC_TYPE_MASK:
            offset = 0x1000;
            length = 0x1000;
            break;
        case CARTRIDGE_CBM2_GENERIC_C2 & CARTRIDGE_CBM2_GENERIC_TYPE_MASK:
            offset = 0x2000;
            length = 0x2000;
            break;
        case CARTRIDGE_CBM2_GENERIC_C4 & CARTRIDGE_CBM2_GENERIC_TYPE_MASK:
            offset = 0x4000;
            length = 0x2000;
            break;
        case CARTRIDGE_CBM2_GENERIC_C6 & CARTRIDGE_CBM2_GENERIC_TYPE_MASK:
            offset = 0x6000;
            length = 0x2000;
            break;
    }
    for (i = 0; i < 4; i++) {
        memset(&rawcart[offset], 0xff, length);
        if (fread(&rawcart[offset], 1, length, fd) < length) {
            break;
        }
        DBG(("loaded block %d offset %04x", i, (unsigned)offset));
        offset += length;
        length = 0x2000; /* only first block may have different length */
    }
    fclose (fd);
    /*return type;*/
    generic_type |= type;

    DBG(("generic_bin_attach generic_type: %04x", (unsigned)generic_type));

    return generic_common_attach();
}

/* FIXME: handle mirroring of 4k/8k ROMs */
int generic_crt_attach(FILE *fd, uint8_t *rawcart)
{
    crt_chip_header_t chip;
    int i;
    int offset = 0, newtype = 0;
    int length;

    DBG(("generic_crt_attach"));

    prepare_rawcart(rawcart);

    for (i = 0; i < 4; i++) {
        if (crt_read_chip_header(&chip, fd)) {
            break;
        }

        DBG(("bank: %d start: %04x size: %04x", chip.bank, chip.start, chip.size));
        switch (chip.start) {
            case 0x1000:
                newtype = CARTRIDGE_CBM2_GENERIC_C1;
                offset = 0x1000;
                length = CBM2_C1_ROM_SIZE;
                break;
            case 0x2000:
                newtype = CARTRIDGE_CBM2_GENERIC_C2;
                offset = 0x2000;
                length = CBM2_C2_ROM_SIZE;
                break;
            case 0x4000:
                newtype = CARTRIDGE_CBM2_GENERIC_C4;
                offset = 0x4000;
                length = CBM2_C4_ROM_SIZE;
                break;
            case 0x6000:
                newtype = CARTRIDGE_CBM2_GENERIC_C6;
                offset = 0x6000;
                length = CBM2_C6_ROM_SIZE;
                break;
            default:
                return -1;
        }

        /* accept 4k,8k banks */
        if (!((chip.size == 0x1000) || (chip.size == 0x2000))) {
            return -1;
        }

        generic_type &= ~newtype;
        DBG(("offset: %04x size: %04x", (unsigned)offset, chip.size));
        memset(&rawcart[offset], 0xff, length);
        if (crt_read_chip(rawcart, offset, &chip, fd)) {
            return -1;
        }
        generic_type |= newtype;
        DBG(("generic_type: %04x", (unsigned)generic_type));
        DBG(("%02x %02x %02x %02x", rawcart[offset+0], rawcart[offset+1], rawcart[offset+2], rawcart[offset+3]));
    }

    return generic_common_attach();
}

void generic_detach(int type)
{
    DBG(("generic_detach type: '%04x'", (unsigned)type));
    if (type & CARTRIDGE_CBM2_GENERIC_TYPE_MASK & CARTRIDGE_CBM2_GENERIC_C1) {
        resources_set_string("Cart1Name", "");
        memset(MEMPTR_CART1, 0xff, CBM2_C1_ROM_SIZE);
    }
    if (type & CARTRIDGE_CBM2_GENERIC_TYPE_MASK & CARTRIDGE_CBM2_GENERIC_C2) {
        resources_set_string("Cart2Name", "");
        memset(MEMPTR_CART2, 0xff, CBM2_C2_ROM_SIZE);
    }
    if (type & CARTRIDGE_CBM2_GENERIC_TYPE_MASK & CARTRIDGE_CBM2_GENERIC_C4) {
        resources_set_string("Cart4Name", "");
        memset(MEMPTR_CART4, 0xff, CBM2_C4_ROM_SIZE);
    }
    if (type & CARTRIDGE_CBM2_GENERIC_TYPE_MASK & CARTRIDGE_CBM2_GENERIC_C6) {
        resources_set_string("Cart6Name", "");
        memset(MEMPTR_CART6, 0xff, CBM2_C6_ROM_SIZE);
    }

    generic_type &= ~(type & CARTRIDGE_CBM2_GENERIC_TYPE_MASK);
}

static const cmdline_option_t cmdline_options[] =
{
    { "-cart1", SET_RESOURCE, CMDLINE_ATTRIB_NEED_ARGS,
      NULL, NULL, "Cart1Name", NULL,
      "<Name>", "Specify name of cartridge ROM image for $1000" },
    { "-cart2", SET_RESOURCE, CMDLINE_ATTRIB_NEED_ARGS,
      NULL, NULL, "Cart2Name", NULL,
      "<Name>", "Specify name of cartridge ROM image for $2000-$3fff" },
    { "-cart4", SET_RESOURCE, CMDLINE_ATTRIB_NEED_ARGS,
      NULL, NULL, "Cart4Name", NULL,
      "<Name>", "Specify name of cartridge ROM image for $4000-$5fff" },
    { "-cart6", SET_RESOURCE, CMDLINE_ATTRIB_NEED_ARGS,
      NULL, NULL, "Cart6Name", NULL,
      "<Name>", "Specify name of cartridge ROM image for $6000-$7fff" },
    CMDLINE_LIST_END
};

int generic_cmdline_options_init(void)
{
    return cmdline_register_options(cmdline_options);
}


static int set_cart1_rom_name(const char *val, void *param)
{
    if (util_string_set(&cart_1_name, val)) {
        return 0;
    }

    return cbm2rom_load_cart_1(cart_1_name);
}

static int set_cart2_rom_name(const char *val, void *param)
{
    if (util_string_set(&cart_2_name, val)) {
        return 0;
    }

    return cbm2rom_load_cart_2(cart_2_name);
}

static int set_cart4_rom_name(const char *val, void *param)
{
    if (util_string_set(&cart_4_name, val)) {
        return 0;
    }

    return cbm2rom_load_cart_4(cart_4_name);
}

static int set_cart6_rom_name(const char *val, void *param)
{
    if (util_string_set(&cart_6_name, val)) {
        return 0;
    }

    return cbm2rom_load_cart_6(cart_6_name);
    /* only does something after mem_load() */
}

static const resource_string_t resources_string[] = {
    { "Cart1Name", "", RES_EVENT_NO, NULL,
      &cart_1_name, set_cart1_rom_name, NULL },
    { "Cart2Name", "", RES_EVENT_NO, NULL,
      &cart_2_name, set_cart2_rom_name, NULL },
    { "Cart4Name", "", RES_EVENT_NO, NULL,
      &cart_4_name, set_cart4_rom_name, NULL },
    { "Cart6Name", "", RES_EVENT_NO, NULL,
      &cart_6_name, set_cart6_rom_name, NULL },
    RESOURCE_STRING_LIST_END
};

int generic_resources_init(void)
{
    return resources_register_string(resources_string);
}

void generic_resources_shutdown(void)
{
    if (cart_1_name != NULL) {
        lib_free(cart_1_name);
    }
    if (cart_2_name != NULL) {
        lib_free(cart_2_name);
    }
    if (cart_4_name != NULL) {
        lib_free(cart_4_name);
    }
    if (cart_6_name != NULL) {
        lib_free(cart_6_name);
    }
}

/* ---------------------------------------------------------------------*/

/* CARTGENERIC snapshot module format:

   type  | name              | version | description
   -------------------------------------------------
   ARRAY | ROM C1            |   0.1+  | 4kiB of ROM data
   ARRAY | ROM C2            |   0.1+  | 8kiB of ROM data
   ARRAY | ROM C4            |   0.1+  | 8kiB of ROM data
   ARRAY | ROM C6            |   0.1+  | 8kiB of ROM data
 */

/* FIXME: the following is completely untested */

static const char snap_module_name[] = "CARTGENERIC";
#define SNAP_MAJOR   0
#define SNAP_MINOR   1

int generic_snapshot_write_module(snapshot_t *s)
{
    snapshot_module_t *m;

    DBG(("generic_snapshot_write_module"));

    m = snapshot_module_create(s, snap_module_name, SNAP_MAJOR, SNAP_MINOR);

    if (m == NULL) {
        return -1;
    }

    if (0
        || SMW_BA(m, MEMPTR_CART1, CBM2_C1_ROM_SIZE)
        || SMW_BA(m, MEMPTR_CART2, CBM2_C2_ROM_SIZE)
        || SMW_BA(m, MEMPTR_CART4, CBM2_C4_ROM_SIZE)
        || SMW_BA(m, MEMPTR_CART6, CBM2_C6_ROM_SIZE) < 0) {
        snapshot_module_close(m);
        return -1;
    }

    snapshot_module_close(m);

    return 0;
}

int generic_snapshot_read_module(snapshot_t *s)
{
    uint8_t vmajor, vminor;
    snapshot_module_t *m;

    DBG(("generic_snapshot_read_module"));

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
        || SMR_BA(m, MEMPTR_CART1, CBM2_C1_ROM_SIZE)
        || SMR_BA(m, MEMPTR_CART2, CBM2_C2_ROM_SIZE)
        || SMR_BA(m, MEMPTR_CART4, CBM2_C4_ROM_SIZE)
        || SMR_BA(m, MEMPTR_CART6, CBM2_C6_ROM_SIZE) < 0) {
        goto fail;
    }

    snapshot_module_close(m);

    /* generic_common_attach(); */

    /* set filetype to none */
    generic_filetype = 0;

    return 0;

fail:
    snapshot_module_close(m);
    return -1;
}
