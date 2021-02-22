
/*
 * plus4cart.c -- Plus4 generic cartridge handling.
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

#define DEBUGGENERIC

#include "vice.h"

#include <string.h>

#include "archdep.h"
#include "cartridge.h"
#include "cartio.h"
#include "cmdline.h"
#include "lib.h"
#include "log.h"
#include "machine.h"
#include "plus4cart.h"
#include "plus4mem.h"
#include "resources.h"
#include "sysfile.h"
#include "util.h"

#include "plus4-generic.h"

#ifdef DEBUGGENERIC
#define DBG(x)  log_debug x
#else
#define DBG(x)
#endif

/* FIXME: get rid of this ugly hack */
extern int plus4_rom_loaded;

/* Name of the external cartridge ROMs.  */
static char *c1lo_rom_name = NULL;
static char *c1hi_rom_name = NULL;

/* FIXME: allocate dynamically */
uint8_t extromlo2[PLUS4_BASIC_ROM_SIZE];
uint8_t extromhi2[PLUS4_KERNAL_ROM_SIZE];

uint8_t generic_c1lo_read(uint16_t addr)
{
    return extromlo2[addr & 0x3fff];
}

uint8_t generic_c1hi_read(uint16_t addr)
{
    return extromhi2[addr & 0x3fff];
}

/* FIXME: this is totally wrong, we should only use c1lo/c1hi for generic */
/* FIXME: also use rawcart */
int generic_bin_attach(int type, const char *filename)
{
    FILE *fd;
    unsigned char *blocks[6] = {
        extromlo1, extromhi1, extromlo2, extromhi2, extromlo3, extromhi3
    };
    int i;

    fd = fopen(filename, "rb");
    if (fd == NULL) {
        return -1;
    }
    for (i = 0; i < 6; i++) {
        if (type & 1) {
            memset(blocks[i], 0, PLUS4_CART16K_SIZE);
            log_debug("loading block %d", i);
            if (fread(blocks[i], 1, PLUS4_CART16K_SIZE, fd) < PLUS4_CART16K_SIZE) {
                break;
            }
        }
        type >>= 1;
    }
    fclose (fd);

    return 0;
}

/* c1lo is always external cartridge */
int plus4cart_load_c1lo(const char *rom_name)
{
    unsigned char *rawcartdata; /* FIXME: should happen in cartridge_attach_image */
    unsigned int len;
    FILE *fd;

    if (!plus4_rom_loaded) {
        return 0;
    }
    DBG(("plus4cart_load_c1lo '%s'\n", rom_name));

    DBG(("clearing c1lo"));
    memset(extromlo2, 0xff, PLUS4_CART16K_SIZE);

    if ((rom_name == NULL) || (*rom_name == 0)) {
        return 0;
    }

    fd = fopen(rom_name, MODE_READ);
    if (fd == NULL) {
        return -1;
    }
    len = util_file_length(fd);
    fclose(fd);

    /* allocate temporary array */
    /* FIXME: should happen in cartridge_attach_image */
    rawcartdata = lib_malloc(PLUS4CART_ROM_LIMIT);

    DBG(("plus4cart_load_c1lo len: %02x\n", len));

    /* Load c1 low ROM.  */
    switch (len) {
        case 0x2000: /* 8K */
            if (util_file_load(rom_name, rawcartdata, PLUS4_CART8K_SIZE, UTIL_FILE_LOAD_SKIP_ADDRESS) < 0) {
                return -1;
            }
            /* create a mirror for 8k ROM */
            memcpy(extromlo2, rawcartdata, PLUS4_CART8K_SIZE);
            memcpy(&extromlo2[PLUS4_CART8K_SIZE], rawcartdata, PLUS4_CART8K_SIZE);
            break;
        case 0x4000: /* 16K */
            if (util_file_load(rom_name, rawcartdata, PLUS4_CART16K_SIZE, UTIL_FILE_LOAD_SKIP_ADDRESS) < 0) {
                return -1;
            }
            memcpy(extromlo2, rawcartdata, PLUS4_CART16K_SIZE);
            break;
        case 0x8000: /* 32K */ /* FIXME: this doesnt seem to work */
            if (util_file_load(rom_name, rawcartdata, PLUS4_CART16K_SIZE * 2, UTIL_FILE_LOAD_SKIP_ADDRESS) < 0) {
                return -1;
            }
            memcpy(extromlo2, rawcartdata, PLUS4_CART16K_SIZE);
            memcpy(extromhi2, &rawcartdata[PLUS4_CART16K_SIZE], PLUS4_CART16K_SIZE);
            break;
        default:
            return -1;
    }

    lib_free(rawcartdata); /* FIXME: should happen in cartridge_attach_image */

    return 0;
}

/* c1hi is always external cartridge */
int plus4cart_load_c1hi(const char *rom_name)
{
    if (!plus4_rom_loaded) {
        return 0;
    }

    /* Load c1 high ROM.  */
    if (*rom_name != 0) {
        if (sysfile_load(rom_name, machine_name, extromhi2, PLUS4_CART16K_SIZE, PLUS4_CART16K_SIZE) < 0) {
            log_error(LOG_ERR,
                      "Couldn't load cartridge 1 high ROM `%s'.",
                      rom_name);
            return -1;
        }
    } else {
        DBG(("clearing c1hi\n"));
        memset(extromhi2, 0, PLUS4_CART16K_SIZE);
    }
    return 0;
}

void generic_detach(void)
{
    resources_set_string("c1loName", "");
    resources_set_string("c1hiName", "");
    memset(extromlo2, 0, PLUS4_CART16K_SIZE);
    memset(extromhi2, 0, PLUS4_CART16K_SIZE);
}

static int set_c1lo_rom_name(const char *val, void *param)
{
    if (util_string_set(&c1lo_rom_name, val)) {
        return 0;
    }

    return plus4cart_load_c1lo(c1lo_rom_name);
}

static int set_c1hi_rom_name(const char *val, void *param)
{
    if (util_string_set(&c1hi_rom_name, val)) {
        return 0;
    }

    return plus4cart_load_c1hi(c1hi_rom_name);
}

static const resource_string_t resources_string[] = {
    { "c1loName", "", RES_EVENT_NO, NULL,
      &c1lo_rom_name, set_c1lo_rom_name, NULL },
    { "c1hiName", "", RES_EVENT_NO, NULL,
      &c1hi_rom_name, set_c1hi_rom_name, NULL },
    RESOURCE_STRING_LIST_END
};

int generic_resources_init(void)
{
    if (resources_register_string(resources_string) < 0) {
        return -1;
    }

    /* return resources_register_int(resources_int); */
    return 0;
}

void generic_resources_shutdown(void)
{
    lib_free(c1lo_rom_name);
    lib_free(c1hi_rom_name);
}

static const cmdline_option_t cmdline_options[] =
{
    { "-c1lo", SET_RESOURCE, CMDLINE_ATTRIB_NEED_ARGS,
      NULL, NULL, "c1loName", NULL,
      "<Name>", "Specify name of Cartridge 1 low ROM image" },
    { "-c1hi", SET_RESOURCE, CMDLINE_ATTRIB_NEED_ARGS,
      NULL, NULL, "c1hiName", NULL,
      "<Name>", "Specify name of Cartridge 1 high ROM image" },
    CMDLINE_LIST_END
};

int generic_cmdline_options_init(void)
{
    return cmdline_register_options(cmdline_options);
}

