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
#include "c64io.h"
#include "cartridge.h"
#include "cmdline.h"
#include "lib.h"
#include "log.h"
#include "machine.h"
#include "mem.h"
#include "resources.h"
#include "georam.h"
#include "snapshot.h"
#ifdef HAS_TRANSLATION
#include "translate.h"
#endif
#include "types.h"
#include "util.h"


/*
 Offsets of the different GEORAM registers
*/
#define GEORAM_REG_PAGE_LOW      0xfe
#define GEORAM_REG_PAGE_HIGH     0xff

/* GEORAM registers */
static BYTE georam[2];

/* GEORAM image.  */
static BYTE *georam_ram = NULL;
static int old_georam_ram_size = 0;

static log_t georam_log = LOG_ERR;

static int georam_activate(void);
static int georam_deactivate(void);

/* ------------------------------------------------------------------------- */

/* Flag: Do we enable the external GEORAM?  */
int georam_enabled;

/* Size of the GEORAM.  */
static int georam_size = 0;

/* Size of the GEORAM in KB.  */
static int georam_size_kb = 0;

/* Filename of the GEORAM image.  */
static char *georam_filename = NULL;

static int set_georam_enabled(int val, void *param)
{
    if (!val) {
        if (georam_enabled) {
            if (georam_deactivate() < 0) {
                return -1;
            }
        }
        georam_enabled = 0;
        return 0;
    } else { 
        if (!georam_enabled) {
            if (georam_activate() < 0) {
                return -1;
            }
        }

        georam_enabled = 1;
        return 0;
    }
}

static int set_georam_size(int val, void *param)
{
    if (val == georam_size_kb)
        return 0;

    switch (val) {
      case 64:
      case 128:
      case 256:
      case 512:
      case 1024:
      case 2048:
      case 4096:
        break;
      default:
        log_message(georam_log, "Unknown GEORAM size %d.", val);
        return -1;
    }

    if (georam_enabled) {
        georam_deactivate();
        georam_size_kb = val;
        georam_size = georam_size_kb << 10;
        georam_activate();
    } else {
        georam_size_kb = val;
        georam_size = georam_size_kb << 10;
    }

    return 0;
}

static int set_georam_filename(const char *name, void *param)
{
    if (georam_filename != NULL && name != NULL
        && strcmp(name, georam_filename) == 0)
        return 0;

    if (name != NULL && *name != '\0') {
        if (util_check_filename_access(name) < 0)
            return -1;
    }

    if (georam_enabled) {
        georam_deactivate();
        util_string_set(&georam_filename, name);
        georam_activate();
    } else {
        util_string_set(&georam_filename, name);
    }

    return 0;
}

static const resource_string_t resources_string[] = {
    { "GEORAMfilename", "", RES_EVENT_NO, NULL,
      &georam_filename, set_georam_filename, NULL },
    { NULL }
};

static const resource_int_t resources_int[] = {
    { "GEORAM", 0, RES_EVENT_STRICT, (resource_value_t)0,
      &georam_enabled, set_georam_enabled, NULL },
    { "GEORAMsize", 512, RES_EVENT_NO, NULL,
      &georam_size_kb, set_georam_size, NULL },
    { NULL }
};

int georam_resources_init(void)
{
    if (resources_register_string(resources_string) < 0)
        return -1;

    return resources_register_int(resources_int);
}

void georam_resources_shutdown(void)
{
    lib_free(georam_filename);
}

/* ------------------------------------------------------------------------- */

#ifdef HAS_TRANSLATION
static const cmdline_option_t cmdline_options[] =
{
    { "-georam", SET_RESOURCE, 0, NULL, NULL, "GEORAM", (resource_value_t)1,
      0, IDCLS_ENABLE_GEORAM },
    { "+georam", SET_RESOURCE, 0, NULL, NULL, "GEORAM", (resource_value_t)0,
      0, IDCLS_DISABLE_GEORAM },
    { "-georamimage", SET_RESOURCE, 1, NULL, NULL, "GEORAMfilename", NULL,
      IDCLS_P_NAME, IDCLS_SPECIFY_GEORAM_NAME },
    { "-georamsize", SET_RESOURCE, 1, NULL, NULL, "GEORAMsize", NULL,
      IDCLS_P_SIZE_IN_KB, IDCLS_GEORAM_SIZE },
    { NULL }
};
#else
static const cmdline_option_t cmdline_options[] =
{
    { "-georam", SET_RESOURCE, 0, NULL, NULL, "GEORAM", (resource_value_t)1,
      NULL, N_("Enable the GEORAM expansion unit") },
    { "+georam", SET_RESOURCE, 0, NULL, NULL, "GEORAM", (resource_value_t)0,
      NULL, N_("Disable the GEORAM expansion unit") },
    { "-georamimage", SET_RESOURCE, 1, NULL, NULL, "GEORAMfilename", NULL,
      N_("<name>"), N_("Specify name of GEORAM image") },
    { "-georamsize", SET_RESOURCE, 1, NULL, NULL, "GEORAMsize", NULL,
      N_("<size in KB>"), N_("Size of the GEORAM expansion unit") },
    { NULL }
};
#endif

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

    log_message(georam_log, "%dKB unit installed.", georam_size >> 10);

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

    io_source=IO_SOURCE_GEORAM;
    retval=georam[addr&1];

    return retval;
}

BYTE REGPARM1 georam_window_read(WORD addr)
{
    BYTE retval;

    io_source=IO_SOURCE_GEORAM;
    retval=georam_ram[(georam[1]*16384)+(georam[0]*256)+addr];

    return retval;
}


void REGPARM2 georam_reg_store(WORD addr, BYTE byte)
{
    if ((addr & 1) == 1) {
        while (byte > ((georam_size_kb / 16) - 1)) {
            byte = byte - (unsigned char)(georam_size_kb / 16);
        }
        georam[1] = byte;
    }
    if ((addr & 1) == 0) {
        while (byte > 63) {
            byte = byte - 64;
        }
        georam[0] = byte;
    }
}

void REGPARM2 georam_window_store(WORD addr, BYTE byte)
{
    georam_ram[(georam[1] * 16384) + (georam[0] * 256) + addr] = byte;
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
        log_error(georam_log, "Size %d in snapshot not supported.", (int)size);
        goto fail;
    }

    set_georam_size((int)size, NULL);

    if (!georam_enabled)
        set_georam_enabled(1, NULL);

    if (SMR_BA(m, georam, sizeof(georam)) < 0 || SMR_BA(m, georam_ram, georam_size) < 0)
        goto fail;

    snapshot_module_close(m);
    return 0;

fail:
    snapshot_module_close(m);
    return -1;
}
