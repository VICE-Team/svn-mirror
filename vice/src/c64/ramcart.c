/*
 * ramcart.c - RAMCART emulation.
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

#include "c64_256k.h"
#include "c64cart.h"
#include "c64export.h"
#include "c64io.h"
#include "c64mem.h"
#include "cartridge.h"
#include "cmdline.h"
#include "lib.h"
#include "log.h"
#include "machine.h"
#include "mem.h"
#include "plus256k.h"
#include "plus60k.h"
#include "resources.h"
#include "ramcart.h"
#include "snapshot.h"
#ifdef HAS_TRANSLATION
#include "translate.h"
#endif
#include "types.h"
#include "util.h"


static const c64export_resource_t export_res = {
    "RAMCART", 1, 0
};

/* RAMCART registers */
static BYTE ramcart[2];

/* RAMCART image.  */
static BYTE *ramcart_ram = NULL;
static unsigned int old_ramcart_ram_size = 0;

static log_t ramcart_log = LOG_ERR;

static int ramcart_activate(void);
static int ramcart_deactivate(void);

/* ------------------------------------------------------------------------- */

/* Flag: Do we enable the external RAMCART?  */
int ramcart_enabled;

/* Flag: Is the RAMCART readonly ?  */
int ramcart_readonly=0;

/* Size of the RAMCART.  */
static DWORD ramcart_size = 0;

/* Size of the RAMCART in KB.  */
static DWORD ramcart_size_kb = 0;

/* Filename of the RAMCART image.  */
static char *ramcart_filename = NULL;

void ramcart_init_config(void)
{
  if (ramcart_enabled)
  {
    export.exrom=1;
    mem_pla_config_changed();
  }
}

static int set_ramcart_enabled(int val, void *param)
{
    if (!val) {
        if (ramcart_enabled) {
            if (ramcart_deactivate() < 0) {
                return -1;
            }
        }
        c64export_remove(&export_res);
        ramcart_enabled = 0;
        export.exrom=0;
        mem_pla_config_changed();
        return 0;
    } else { 
        if (c64export_query(&export_res) >= 0) {
            if (!ramcart_enabled) {
                if (ramcart_activate() < 0) {
                    return -1;
                }
            }

            if (c64export_add(&export_res) < 0)
                return -1;

            ramcart_enabled = 1;
            export.exrom=1;
            mem_pla_config_changed();
            return 0;
        } else {
            return -1;
        }
    }
}

static int set_ramcart_readonly(int val, void *param)
{
  ramcart_readonly = val;
  return 0;
}

static int set_ramcart_size(int val, void *param)
{
    if ((DWORD)val == ramcart_size_kb)
        return 0;

    switch ((DWORD)val) {
      case 64:
      case 128:
        break;
      default:
        log_message(ramcart_log, "Unknown RAMCART size %ld.", (long)val);
        return -1;
    }

    if (ramcart_enabled) {
        ramcart_deactivate();
        ramcart_size_kb = (DWORD)val;
        ramcart_size = ramcart_size_kb << 10;
        ramcart_activate();
    } else {
        ramcart_size_kb = (DWORD)val;
        ramcart_size = ramcart_size_kb << 10;
    }

    return 0;
}

static int set_ramcart_filename(const char *name, void *param)
{
    if (ramcart_filename != NULL && name != NULL
        && strcmp(name, ramcart_filename) == 0)
        return 0;

    if (ramcart_enabled) {
        ramcart_deactivate();
        util_string_set(&ramcart_filename, name);
        ramcart_activate();
    } else {
        util_string_set(&ramcart_filename, name);
    }

    return 0;
}

static const resource_string_t resources_string[] = {
    { "RAMCARTfilename", "", RES_EVENT_NO, NULL,
      &ramcart_filename, set_ramcart_filename, NULL },
    { NULL }
};

static const resource_int_t resources_int[] = {
    { "RAMCART", 0, RES_EVENT_STRICT, (resource_value_t)0,
      &ramcart_enabled, set_ramcart_enabled, NULL },
    { "RAMCART_RO", 0, RES_EVENT_NO, NULL,
      &ramcart_readonly, set_ramcart_readonly, NULL },
    { "RAMCARTsize", 128, RES_EVENT_NO, NULL,
      (int *)&ramcart_size_kb, set_ramcart_size, NULL },
    { NULL }
};

int ramcart_resources_init(void)
{
    if (resources_register_string(resources_string) < 0)
        return -1;

    return resources_register_int(resources_int);
}

void ramcart_resources_shutdown(void)
{
    lib_free(ramcart_filename);
}

/* ------------------------------------------------------------------------- */

#ifdef HAS_TRANSLATION
static const cmdline_option_t cmdline_options[] =
{
    { "-ramcart", SET_RESOURCE, 0, NULL, NULL, "RAMCART", (resource_value_t)1,
      0, IDCLS_ENABLE_RAMCART },
    { "+ramcart", SET_RESOURCE, 0, NULL, NULL, "RAMCART", (resource_value_t)0,
      0, IDCLS_DISABLE_RAMCART },
    { "-ramcartimage", SET_RESOURCE, 1, NULL, NULL, "RAMCARTfilename", NULL,
      IDCLS_P_NAME, IDCLS_SPECIFY_RAMCART_NAME },
    { "-ramcartsize", SET_RESOURCE, 1, NULL, NULL, "RAMCARTsize", NULL,
      IDCLS_P_SIZE_IN_KB, IDCLS_RAMCART_SIZE },
    { NULL }
};
#else
static const cmdline_option_t cmdline_options[] =
{
    { "-ramcart", SET_RESOURCE, 0, NULL, NULL, "RAMCART", (resource_value_t)1,
      NULL, N_("Enable the RAMCART expansion") },
    { "+ramcart", SET_RESOURCE, 0, NULL, NULL, "RAMCART", (resource_value_t)0,
      NULL, N_("Disable the RAMCART expansion") },
    { "-ramcartimage", SET_RESOURCE, 1, NULL, NULL, "RAMCARTfilename", NULL,
      N_("<name>"), N_("Specify name of RAMCART image") },
    { "-ramcartsize", SET_RESOURCE, 1, NULL, NULL, "RAMCARTsize", NULL,
      N_("<size in KB>"), N_("Size of the RAMCART expansion") },
    { NULL }
};
#endif

int ramcart_cmdline_options_init(void)
{
    return cmdline_register_options(cmdline_options);
}

/* ------------------------------------------------------------------------- */

void ramcart_init(void)
{
    ramcart_log = log_open("RAMCART");
}

void ramcart_reset(void)
{
  ramcart[0]=0;
  ramcart[1]=0;
}

static int ramcart_activate(void)
{
    if (!ramcart_size)
        return 0;

    ramcart_ram = (BYTE *)lib_realloc((void *)ramcart_ram, (size_t)ramcart_size);

    /* Clear newly allocated RAM.  */
    if (ramcart_size > old_ramcart_ram_size)
        memset(ramcart_ram, 0, (size_t)(ramcart_size - old_ramcart_ram_size));

    old_ramcart_ram_size = ramcart_size;

    log_message(ramcart_log, "%dKB unit installed.", (int)(ramcart_size >> 10));

    if (!util_check_null_string(ramcart_filename)) {
        if (util_file_load(ramcart_filename, ramcart_ram, (size_t)ramcart_size,
                           UTIL_FILE_LOAD_RAW) < 0) {
            log_message(ramcart_log,
                        "Reading RAMCART image %s failed.", ramcart_filename);
            if (util_file_save(ramcart_filename, ramcart_ram, ramcart_size) < 0) {
                log_message(ramcart_log,
                            "Creating RAMCART image %s failed.", ramcart_filename);
                return -1;
            }
            log_message(ramcart_log, "Creating RAMCART image %s.", ramcart_filename);
            return 0;
        }
        log_message(ramcart_log, "Reading RAMCART image %s.", ramcart_filename);
    }

    ramcart_reset();
    return 0;
}

static int ramcart_deactivate(void)
{
    if (ramcart_ram == NULL)
        return 0;

    if (!util_check_null_string(ramcart_filename)) {
        if (util_file_save(ramcart_filename, ramcart_ram, ramcart_size) < 0) {
            log_message(ramcart_log,
                        "Writing RAMCART image %s failed.", ramcart_filename);
            return -1;
        }
        log_message(ramcart_log, "Writing RAMCART image %s.", ramcart_filename);
    }

    lib_free(ramcart_ram);
    ramcart_ram = NULL;
    old_ramcart_ram_size = 0;

    return 0;
}

void ramcart_shutdown(void)
{
    ramcart_deactivate();
}

/* ------------------------------------------------------------------------- */

BYTE REGPARM1 ramcart_reg_read(WORD addr)
{
    BYTE retval;

    io_source=IO_SOURCE_RAMCART;
    retval=ramcart[addr];

    return retval;
}

void REGPARM2 ramcart_reg_store(WORD addr, BYTE byte)
{
  if (addr==1 && ramcart_size_kb==128)
    ramcart[1]=byte;
  if (addr==0)
    ramcart[0]=byte;
}

BYTE REGPARM1 ramcart_roml_read(WORD addr)
{
    if (ramcart_readonly==1 && ramcart_size_kb==128 && addr>=0x8000 && addr<=0x80ff)
      return ramcart_ram[((ramcart[1]&1)*65536)+(ramcart[0]*256)+(addr&0xff)];
    if (plus60k_enabled)
      return plus60k_ram_read(addr);
    if (plus256k_enabled)
      return plus256k_ram_high_read(addr);
    if (c64_256k_enabled)
      return c64_256k_ram_segment2_read(addr);
    return mem_ram[addr];
}

void REGPARM2 ramcart_roml_store(WORD addr, BYTE byte)
{
  if (plus60k_enabled)
  {
    plus60k_ram_store(addr, byte);
    return;
  }
  if (plus256k_enabled)
  {
    plus256k_ram_high_store(addr, byte);
    return;
  }
  if (c64_256k_enabled)
  {
    c64_256k_ram_segment2_store(addr, byte);
    return;
  }
  mem_ram[addr]=byte;
}

BYTE REGPARM1 ramcart_window_read(WORD addr)
{
    BYTE retval;

    io_source=IO_SOURCE_RAMCART;
    retval=ramcart_ram[((ramcart[1]&1)*65536)+(ramcart[0]*256)+(addr&0xff)];

    return retval;
}

void REGPARM2 ramcart_window_store(WORD addr, BYTE byte)
{
  ramcart_ram[((ramcart[1]&1)*65536)+(ramcart[0]*256)+(addr&0xff)]=byte;
}

