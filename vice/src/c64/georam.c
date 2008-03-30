/*
 * georam.c - GEORAM emulation.
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

#include "c64cart.h"
#include "c64export.h"
#include "cartridge.h"
#include "cmdline.h"
#include "interrupt.h"
#include "lib.h"
#include "log.h"
#include "machine.h"
#include "maincpu.h"
#include "mem.h"
#include "resources.h"
#include "georam.h"
#include "snapshot.h"
#include "types.h"
#include "util.h"


/*
 Offsets of the different GEORAM registers
*/
#define GEORAM_REG_PAGE_LOW      0xfe
#define GEORAM_REG_PAGE_HIGH     0xff


static const c64export_resource_t export_res = {
    "GEORAM", 1, 1, 0, 0
};

/* GEORAM registers */
static BYTE georam[2];

/* GEORAM image.  */
static BYTE *georam_ram = NULL;
static unsigned int old_georam_ram_size = 0;

static log_t georam_log = LOG_ERR;

static int georam_activate(void);
static int georam_deactivate(void);

/* ------------------------------------------------------------------------- */

/* Flag: Do we enable the external GEORAM?  */
int georam_enabled;

/* Size of the GEORAM.  */
static DWORD georam_size = 0;

/* Size of the GEORAM in KB.  */
static DWORD georam_size_kb = 0;

/* Filename of the GEORAM image.  */
static char *georam_filename = NULL;


static int set_georam_enabled(resource_value_t v, void *param)
{
    if (!(int)v) {
        if (georam_enabled) {
            if (georam_deactivate() < 0) {
                return -1;
            }
        }
        c64export_remove(&export_res);
        georam_enabled = 0;
        return 0;
    } else { 
        if (c64export_query(&export_res) >= 0) {
            if (!georam_enabled) {
                if (georam_activate() < 0) {
                    return -1;
                }
            }

            if (c64export_add(&export_res) < 0)
                return -1;

            georam_enabled = 1;
            return 0;
        } else {
            return -1;
        }
    }
}

static int set_georam_size(resource_value_t v, void *param)
{
    if ((DWORD)v == georam_size_kb)
        return 0;

    switch ((DWORD)v) {
      case 64:
      case 128:
      case 256:
      case 512:
      case 1024:
      case 2048:
      case 4096:
        break;
      default:
        log_message(georam_log, "Unknown GEORAM size %ld.", (long)v);
        return -1;
    }

    if (georam_enabled) {
        georam_deactivate();
        georam_size_kb = (DWORD)v;
        georam_size = georam_size_kb << 10;
        georam_activate();
    } else {
        georam_size_kb = (DWORD)v;
        georam_size = georam_size_kb << 10;
    }

    return 0;
}

static int set_georam_filename(resource_value_t v, void *param)
{
    const char *name = (const char *)v;

    if (georam_filename != NULL && name != NULL
        && strcmp(name, georam_filename) == 0)
        return 0;

    if (georam_enabled) {
        georam_deactivate();
        util_string_set(&georam_filename, name);
        georam_activate();
    } else {
        util_string_set(&georam_filename, name);
    }

    return 0;
}

static const resource_t resources[] = {
    { "GEORAM", RES_INTEGER, (resource_value_t)0,
      (void *)&georam_enabled, set_georam_enabled, NULL },
    { "GEORAMsize", RES_INTEGER, (resource_value_t)512,
      (void *)&georam_size_kb, set_georam_size, NULL },
    { "GEORAMfilename", RES_STRING, (resource_value_t)"",
      (void *)&georam_filename, set_georam_filename, NULL },
    { NULL }
};

int georam_resources_init(void)
{
    return resources_register(resources);
}

void georam_resources_shutdown(void)
{
    lib_free(georam_filename);
}

/* ------------------------------------------------------------------------- */

static const cmdline_option_t cmdline_options[] =
{
    { "-georam", SET_RESOURCE, 0, NULL, NULL, "GEORAM", (resource_value_t)1,
      NULL, "Enable the GEORAM expansion unit" },
    { "+georam", SET_RESOURCE, 0, NULL, NULL, "GEORAM", (resource_value_t)0,
      NULL, "Disable the GEORAM expansion unit" },
    { "-georamimage", SET_RESOURCE, 1, NULL, NULL, "GEORAMfilename", NULL,
      "<name>", "Specify name of GEORAM image" },
    { "-georamsize", SET_RESOURCE, 1, NULL, NULL, "GEORAMsize", NULL,
      "<size in KB>", "Size of the GEORAM expansion unit" },
    { NULL }
};

int georam_cmdline_options_init(void)
{
    return cmdline_register_options(cmdline_options);
}

/* ------------------------------------------------------------------------- */

void georam_init(void)
{
    georam_log = log_open("GEORAM");
}

void georam_reset(void)
{
    georam[0]=0;
    georam[1]=0;
}

static int georam_activate(void)
{
    if (!georam_size)
        return 0;

    georam_ram = (BYTE *)lib_realloc((void *)georam_ram, (size_t)georam_size);

    /* Clear newly allocated RAM.  */
    if (georam_size > old_georam_ram_size)
        memset(georam_ram, 0, (size_t)(georam_size - old_georam_ram_size));

    old_georam_ram_size = georam_size;

    log_message(georam_log, "%dKB unit installed.", (int)(georam_size >> 10));

    if (!util_check_null_string(georam_filename)) {
        if (util_file_load(georam_filename, georam_ram, (size_t)georam_size,
                           UTIL_FILE_LOAD_RAW) < 0) {
            log_message(georam_log,
                        "Reading GEORAM image %s failed.", georam_filename);
            if (util_file_save(georam_filename, georam_ram, georam_size) < 0) {
                log_message(georam_log,
                            "Creating GEORAM image %s failed.", georam_filename);
                return -1;
            }
            log_message(georam_log, "Creating GEORAM image %s.", georam_filename);
            return 0;
        }
        log_message(georam_log, "Reading GEORAM image %s.", georam_filename);
    }

    georam_reset();
    return 0;
}

static int georam_deactivate(void)
{
    if (georam_ram == NULL)
        return 0;

    if (!util_check_null_string(georam_filename)) {
        if (util_file_save(georam_filename, georam_ram, georam_size) < 0) {
            log_message(georam_log,
                        "Writing GEORAM image %s failed.", georam_filename);
            return -1;
        }
        log_message(georam_log, "Writing GEORAM image %s.", georam_filename);
    }

    lib_free(georam_ram);
    georam_ram = NULL;
    old_georam_ram_size = 0;

    return 0;
}

void georam_shutdown(void)
{
    georam_deactivate();
}

/* ------------------------------------------------------------------------- */

BYTE REGPARM1 georam_reg_read(WORD addr)
{
    BYTE retval;

    retval=georam[addr&1];

    return retval;
}

BYTE REGPARM1 georam_window_read(WORD addr)
{
    BYTE retval;

    retval=georam_ram[(georam[1]*16384)+(georam[0]*256)+addr];

    return retval;
}


void REGPARM2 georam_reg_store(WORD addr, BYTE byte)
{
    if ((addr&1)==1)
    {
      while (byte>((georam_size_kb/16)-1))
      {
        byte=byte-(georam_size_kb/16);
      }
      georam[1]=byte;
    }
    if ((addr&1)==0)
    {
      while (byte>63)
      {
        byte=byte-64;
      }
      georam[0]=byte;
    }
}

void REGPARM2 georam_window_store(WORD addr, BYTE byte)
{
  georam_ram[(georam[1]*16384)+(georam[0]*256)+addr]=byte;
}

/* ------------------------------------------------------------------------- */

static char snap_module_name[] = "GEORAM";
#define SNAP_MAJOR 0
#define SNAP_MINOR 0

int georam_write_snapshot_module(snapshot_t *s)
{
    snapshot_module_t *m;

    m = snapshot_module_create(s, snap_module_name, SNAP_MAJOR, SNAP_MINOR);
    if (m == NULL)
        return -1;

    if (SMW_DW(m, (georam_size >> 10)) < 0
        || SMW_BA(m, georam, sizeof(georam)) < 0
        || SMW_BA(m, georam_ram, georam_size) < 0) {
        snapshot_module_close(m);
        return -1;
    }

    snapshot_module_close(m);
    return 0;
}

int georam_read_snapshot_module(snapshot_t *s)
{
    BYTE major_version, minor_version;
    snapshot_module_t *m;
    DWORD size;

    m = snapshot_module_open(s, snap_module_name,
                             &major_version, &minor_version);
    if (m == NULL)
        return -1;

    if (major_version != SNAP_MAJOR) {
        log_error(georam_log, "Major version %d not valid; should be %d.",
                major_version, SNAP_MAJOR);
        goto fail;
    }

    /* Read RAM size.  */
    if (SMR_DW(m, &size) < 0)
        goto fail;

    if (size > 4096) {
        log_error(georam_log, "Size %ld in snapshot not supported.", (long)size);
        goto fail;
    }

    set_georam_size((resource_value_t)size, NULL);

    if (!georam_enabled)
        set_georam_enabled((resource_value_t)1, NULL);

    if (SMR_BA(m, georam, sizeof(georam)) < 0 || SMR_BA(m, georam_ram, georam_size) < 0)
        goto fail;

    snapshot_module_close(m);
    return 0;

fail:
    snapshot_module_close(m);
    return -1;
}

