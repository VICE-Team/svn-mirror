/*
 * dqbb.c - Double Quick Brown Box emulation.
 *
 * Written by
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
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

#define CARTRIDGE_INCLUDE_SLOT1_API
#include "c64cartsystem.h"
#undef CARTRIDGE_INCLUDE_SLOT1_API
#include "c64export.h"
#include "c64io.h"
#include "c64mem.h"
#include "cartridge.h"
#include "cmdline.h"
#include "lib.h"
#include "mem.h"
#include "resources.h"
#include "translate.h"
#include "types.h"
#include "util.h"

#define CARTRIDGE_INCLUDE_PRIVATE_API
#include "dqbb.h"
#undef CARTRIDGE_INCLUDE_PRIVATE_API

/*
    "Double Quick Brown box"

    - 16k RAM

    The Double Quick Brown box is a banked memory system.
    It uses a register at $de00 to control the areas used, the
    read/write / read-only state and on/off of the cart.

    This is done as follows:

    bit 2:   1 = $A000-$BFFF mapped in, 0 = $A000-$BFFF not mapped in.
    bit 4:   1 = read/write, 0 = read-only.
    bit 7:   1 = cart off, 0 = cart on.

    The register is write-only. Attempting to read it will
    only return random values.

    The current emulation has the two registers mirrorred through the
    range of $de00-$deff
*/

/* #define DBGDQBB */

#ifdef DBGDQBB
#define DBG(x) printf x
#else
#define DBG(x)
#endif

/* DQBB register bits */
static int dqbb_a000_mapped;
static int dqbb_readwrite;
static int dqbb_off;

/* DQBB image.  */
static BYTE *dqbb_ram = NULL;

static int dqbb_activate(void);
static int dqbb_deactivate(void);
static void dqbb_change_config(void);

/* Flag: Do we enable the DQBB?  */
static int dqbb_enabled = 0;

/* Filename of the DQBB image.  */
static char *dqbb_filename = NULL;

#define DQBB_RAM_SIZE   0x4000

static int reg_value = 0;

static int dqbb_write_image = 0;

/* ------------------------------------------------------------------------- */
static BYTE REGPARM1 dqbb_io1_peek(WORD addr);
static void REGPARM2 dqbb_io1_store(WORD addr, BYTE byte);

static io_source_t dqbb_io1_device = {
    CARTRIDGE_NAME_DQBB,
    IO_DETACH_RESOURCE,
    "DQBB",
    0xde00, 0xdeff, 0x01,
    0,
    dqbb_io1_store,
    NULL,
    dqbb_io1_peek,
    NULL, /* dump */
    CARTRIDGE_DQBB
};

static io_source_list_t *dqbb_list_item = NULL;

static const c64export_resource_t export_res = {
    CARTRIDGE_NAME_DQBB, 1, 1, &dqbb_io1_device, NULL, CARTRIDGE_DQBB
};

/* ------------------------------------------------------------------------- */

int dqbb_cart_enabled(void)
{
    return dqbb_enabled;
}

static void dqbb_change_config(void)
{
    if (dqbb_enabled) {
        if (dqbb_off) {
            cart_config_changed_slot1(2, 2, CMODE_READ);
        } else {
            if (dqbb_a000_mapped) {
                cart_config_changed_slot1(1, 1, CMODE_READ);
            } else {
                cart_config_changed_slot1(0, 0, CMODE_READ);
            }
        }
    } else {
        cart_config_changed_slot1(2, 2, CMODE_READ);
    }
}

static void REGPARM2 dqbb_io1_store(WORD addr, BYTE byte)
{
    dqbb_a000_mapped = (byte & 4) >> 2;
    dqbb_readwrite = (byte & 0x10) >> 4;
    dqbb_off = (byte & 0x80) >> 7;
    dqbb_change_config();
    reg_value = byte;
}

static BYTE REGPARM1 dqbb_io1_peek(WORD addr)
{
    return reg_value;
}

/* ------------------------------------------------------------------------- */

static int dqbb_activate(void)
{
    lib_free(dqbb_ram);
    dqbb_ram = lib_malloc(DQBB_RAM_SIZE);

    if (!util_check_null_string(dqbb_filename)) {
        if (util_file_load(dqbb_filename, dqbb_ram, DQBB_RAM_SIZE, UTIL_FILE_LOAD_RAW) < 0) {
            if (util_file_save(dqbb_filename, dqbb_ram, DQBB_RAM_SIZE) < 0) {
                return -1;
            }
            return 0;
        }
    }
    return 0;
}

static int dqbb_deactivate(void)
{
    if (dqbb_ram == NULL) {
        return 0;
    }

    if (!util_check_null_string(dqbb_filename)) {
        if (util_file_save(dqbb_filename, dqbb_ram, DQBB_RAM_SIZE) < 0) {
            return -1;
        }
    }

    lib_free(dqbb_ram);
    dqbb_ram = NULL;

    c64export_remove(&export_res);

    return 0;
}

static int set_dqbb_enabled(int val, void *param)
{
    if ((!val) && (dqbb_enabled)) {
        cart_power_off();
        if (dqbb_deactivate() < 0) {
            return -1;
        }
        c64io_unregister(dqbb_list_item);
        dqbb_list_item = NULL;
        dqbb_enabled = 0;
        dqbb_reset();
        dqbb_change_config();
    } else if ((val) && (!dqbb_enabled)) {
        cart_power_off();
        if (c64export_add(&export_res) < 0) {
            return -1;
        }
        if (dqbb_activate() < 0) {
            return -1;
        }
        dqbb_list_item = c64io_register(&dqbb_io1_device);
        dqbb_enabled = 1;
        dqbb_reset();
        dqbb_change_config();
    }
    return 0;
}

static int set_dqbb_filename(const char *name, void *param)
{
    if (dqbb_filename != NULL && name != NULL && strcmp(name, dqbb_filename) == 0) {
        return 0;
    }

    if (name != NULL && *name != '\0') {
        if (util_check_filename_access(name) < 0) {
            return -1;
        }
    }

    if (dqbb_enabled) {
        dqbb_deactivate();
        util_string_set(&dqbb_filename, name);
        dqbb_activate();
    } else {
        util_string_set(&dqbb_filename, name);
    }

    return 0;
}

static int set_dqbb_image_write(int val, void *param)
{
    if (dqbb_write_image && !val) {
        dqbb_write_image = 0;
    } else if (!dqbb_write_image && val) {
        dqbb_write_image = 1;
    }
    return 0;
}

/* ---------------------------------------------------------------------*/

static const resource_string_t resources_string[] = {
    { "DQBBfilename", "", RES_EVENT_NO, NULL,
      &dqbb_filename, set_dqbb_filename, NULL },
    { NULL }
};

static const resource_int_t resources_int[] = {
    { "DQBB", 0, RES_EVENT_STRICT, (resource_value_t)0,
      &dqbb_enabled, set_dqbb_enabled, NULL },
    { "DQBBImageWrite", 0, RES_EVENT_NO, NULL,
      &dqbb_write_image, set_dqbb_image_write, NULL },
    { NULL }
};

int dqbb_resources_init(void)
{
    if (resources_register_string(resources_string) < 0) {
        return -1;
    }

    return resources_register_int(resources_int);
}

void dqbb_resources_shutdown(void)
{
    lib_free(dqbb_filename);
    dqbb_filename = NULL;
}

/* ------------------------------------------------------------------------- */

static const cmdline_option_t cmdline_options[] =
{
    { "-dqbb", SET_RESOURCE, 0,
      NULL, NULL, "DQBB", (resource_value_t)1,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_ENABLE_DQBB,
      NULL, NULL },
    { "+dqbb", SET_RESOURCE, 0,
      NULL, NULL, "DQBB", (resource_value_t)0,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_DISABLE_DQBB,
      NULL, NULL },
    { "-dqbbimage", SET_RESOURCE, 1,
      NULL, NULL, "DQBBfilename", NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_SPECIFY_DQBB_NAME,
      NULL, NULL },
    { "-dqbbimagerw", SET_RESOURCE, 0,
      NULL, NULL, "DQBBImageWrite", (resource_value_t)1,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_ALLOW_WRITING_TO_DQBB_IMAGE,
      NULL, NULL },
    { "+dqbbimagerw", SET_RESOURCE, 0,
      NULL, NULL, "DQBBImageWrite", (resource_value_t)0,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_DO_NOT_WRITE_TO_DQBB_IMAGE,
      NULL, NULL },
    { NULL }
};

int dqbb_cmdline_options_init(void)
{
    return cmdline_register_options(cmdline_options);
}

/* ------------------------------------------------------------------------- */

const char *dqbb_get_file_name(void)
{
    return dqbb_filename;
}

void dqbb_reset(void)
{
    dqbb_a000_mapped = 0;
    dqbb_readwrite = 0;
    dqbb_off = 0;
    if (dqbb_enabled) {
        dqbb_change_config();
    }
}

void dqbb_init_config(void)
{
    dqbb_reset();
}

void dqbb_config_setup(BYTE *rawcart)
{
    memcpy(dqbb_ram, rawcart, DQBB_RAM_SIZE);
}

/* ------------------------------------------------------------------------- */

void dqbb_detach(void)
{
    resources_set_int("DQBB", 0);
}

int dqbb_enable(void)
{
    if (resources_set_int("DQBB", 1) < 0) {
        return -1;
    }
    return 0;
}

int dqbb_bin_attach(const char *filename, BYTE *rawcart)
{
    if (util_file_load(filename, rawcart, DQBB_RAM_SIZE, UTIL_FILE_LOAD_RAW) < 0) {
        return -1;
    }
    util_string_set(&dqbb_filename, filename);
    return dqbb_enable();
}

int dqbb_bin_save(const char *filename)
{
    if (dqbb_ram == NULL) {
        return -1;
    }

    if (filename == NULL) {
        return -1;
    }

    if (util_file_save(filename, dqbb_ram, DQBB_RAM_SIZE) < 0) {
        return -1;
    }
    return 0;
}

int dqbb_flush_image(void)
{
    return dqbb_bin_save(dqbb_filename);
}

/* ------------------------------------------------------------------------- */

BYTE REGPARM1 dqbb_roml_read(WORD addr)
{
    return dqbb_ram[addr & 0x1fff];
}

void REGPARM2 dqbb_roml_store(WORD addr, BYTE byte)
{
    if (dqbb_readwrite) {
        dqbb_ram[addr & 0x1fff] = byte;
    }
    mem_store_without_romlh(addr, byte);
}

BYTE REGPARM1 dqbb_romh_read(WORD addr)
{
    return dqbb_ram[(addr & 0x1fff) + 0x2000];
}

void REGPARM2 dqbb_romh_store(WORD addr, BYTE byte)
{
    if (dqbb_readwrite) {
        dqbb_ram[(addr & 0x1fff) + 0x2000] = byte;
    }
    mem_store_without_romlh(addr, byte);
}

int dqbb_peek_mem(WORD addr, BYTE *value)
{
    if ((addr >= 0x8000) && (addr <= 0x9fff)) {
        *value = dqbb_ram[addr & 0x1fff];
        return CART_READ_VALID;
    } else if ((addr >= 0xa000) && (addr <= 0xbfff)) {
        *value = dqbb_ram[(addr & 0x1fff) + 0x2000];
        return CART_READ_VALID;
    }
    return CART_READ_THROUGH;
}
