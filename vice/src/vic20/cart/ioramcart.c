/*
 * ioramcart.c - VIC20 RAM in I/O space emulation.
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

#include "cartio.h"
#include "cartridge.h"
#include "cmdline.h"
#include "ioramcart.h"
#include "resources.h"
#include "translate.h"
#include "types.h"

static BYTE ram_io2[0x400];
static BYTE ram_io3[0x400];

static int ram_io2_enabled = 0;
static int ram_io3_enabled = 0;

/* ---------------------------------------------------------------------*/

static BYTE ram_io2_read(WORD addr)
{
    return ram_io2[addr & 0x3ff];
}

static BYTE ram_io3_read(WORD addr)
{
    return ram_io3[addr & 0x3ff];
}

static void ram_io2_store(WORD addr, BYTE val)
{
    ram_io2[addr & 0x3ff] = val;
}

static void ram_io3_store(WORD addr, BYTE val)
{
    ram_io3[addr & 0x3ff] = val;
}

/* ---------------------------------------------------------------------*/

static io_source_t ram_io2_device = {
    "I/O-2 RAM",
    IO_DETACH_RESOURCE,
    "IO2RAM",
    0x9800, 0x9bff, 0x3ff,
    1, /* read is always valid */
    ram_io2_store,
    ram_io2_read,
    ram_io2_read,
    NULL, /* TODO: dump */
    CARTRIDGE_VIC20_IO2_RAM,
    0,
    0
};

static io_source_t ram_io3_device = {
    "I/O-3 RAM",
    IO_DETACH_RESOURCE,
    "IO3RAM",
    0x9c00, 0x9fff, 0x3ff,
    1, /* read is always valid */
    ram_io3_store,
    ram_io3_read,
    ram_io3_read,
    NULL, /* TODO: dump */
    CARTRIDGE_VIC20_IO3_RAM,
    0,
    0
};

static io_source_list_t *ram_io2_list_item = NULL;
static io_source_list_t *ram_io3_list_item = NULL;

/* ---------------------------------------------------------------------*/

static int set_ram_io2_enabled(int value, void *param)
{
    int val = value ? 1 : 0;

    if (!ram_io2_enabled && val) {
        ram_io2_list_item = io_source_register(&ram_io2_device);
    } else if (ram_io2_enabled && !val) {
        io_source_unregister(ram_io2_list_item);
        ram_io2_list_item = NULL;
    }
    ram_io2_enabled = val;
    return 0;
}

static int set_ram_io3_enabled(int value, void *param)
{
    int val = value ? 1 : 0;

    if (!ram_io3_enabled && val) {
        ram_io3_list_item = io_source_register(&ram_io3_device);
    } else if (ram_io3_enabled && !val) {
        io_source_unregister(ram_io3_list_item);
        ram_io3_list_item = NULL;
    }
    ram_io3_enabled = val;
    return 0;
}

static const resource_int_t resources_int[] = {
    { "IO2RAM", 0, RES_EVENT_STRICT, (resource_value_t)0,
      &ram_io2_enabled, set_ram_io2_enabled, NULL },
    { "IO3RAM", 0, RES_EVENT_STRICT, (resource_value_t)0,
      &ram_io3_enabled, set_ram_io3_enabled, NULL },
    { NULL }
};

int ioramcart_resources_init(void)
{
    return resources_register_int(resources_int);
}

static const cmdline_option_t cmdline_options[] =
{
    { "-io2ram", SET_RESOURCE, 0,
      NULL, NULL, "IO2RAM", (resource_value_t)1,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_ENABLE_IO2_RAM,
      NULL, NULL },
    { "+io2ram", SET_RESOURCE, 0,
      NULL, NULL, "IO2RAM", (resource_value_t)0,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_DISABLE_IO2_RAM,
      NULL, NULL },
    { "-io3ram", SET_RESOURCE, 0,
      NULL, NULL, "IO3RAM", (resource_value_t)1,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_ENABLE_IO3_RAM,
      NULL, NULL },
    { "+io3ram", SET_RESOURCE, 0,
      NULL, NULL, "IO3RAM", (resource_value_t)0,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_DISABLE_IO3_RAM,
      NULL, NULL },
    { NULL }
};

int ioramcart_cmdline_options_init(void)
{
    return cmdline_register_options(cmdline_options);
}
