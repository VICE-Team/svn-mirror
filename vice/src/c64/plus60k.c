/*
 * plus60k.c - PLUS60K EXPANSION HACK emulation.
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

/* Introduction :
 *
 * +60K is a kind of memory expansion for C64 proposed in late '90s by Polish
 * sceners, for sceners. Basically the whole idea was to add another bank of
 * memory and provide a shared area to exchange data between the banks.
 *
 *
 * Hardware :
 *
 * +60K circuit is somewhat complicated because quite a few new ICs have to mounted
 * inside a C64 but it is not very hard to build. I will not get into details and
 * schematics because it was described quite well in disk magazines.
 *
 *
 * Software :
 *
 * - VIC address space is divided into 4 parts: $d000-$d0ff, $d100-$d1ff, $d200-$d2ff
 *   and $d300-$d3ff
 * - only $d000-$d0ff is still visible in I/O space as VIC
 * - $d100-$d1ff returns $ff on read
 * - $d200-$d3ff is unconnected and returns random values
 * - register latch for +60K is active in all $d100-$d1ff space, but programs should
 *   use $d100 only
 * - only data bit 7 is connected to the latch, but programs should use 0 as bits 0-6
 * - VIC fetches data only from bank 0 RAM (onboard)

 * +60K is controlled by a write-only register at $d100. There are only two possible
 * values that can be written there:

 * value     | $1000-$ffff RAM area
 * ---------------------------------------------
 * %0xxxxxxx | comes from onboard RAM (bank 0)
 * %1xxxxxxx | comes from additional RAM (bank 1)

 * x - reserved bit, it seems that all existing +60K-enabled programs use 0 here

 * RAM/ROM/IO is still controlled as usual by $0001. The only thing that changes is
 * where $1000-$ffff RAM comes from. The $0000-$0fff is the shared space and always
 * comes from onboard RAM.
 * It is important to say that VIC cannot see additional RAM. It still fetches data
 * from onboard RAM thus it is possible to keep gfx data in bank 0 and code with
 * sound data in bank 1.
 * The $d100 control register returns $ff on read. Although such usage is forbidden
 * I've seen at least one example of switching to bank 0 by "INC $d100" instruction
 * so it is emulated too.
 *
 */


#include "vice.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "c64_256k.h"
#include "c64cart.h"
#include "c64export.h"
#include "c64mem.h"
#include "cartridge.h"
#include "cmdline.h"
#include "lib.h"
#include "log.h"
#include "machine.h"
#include "mem.h"
#include "resources.h"
#include "plus256k.h"
#include "plus60k.h"
#include "snapshot.h"
#ifdef HAS_TRANSLATION
#include "translate.h"
#endif
#include "types.h"
#include "uiapi.h"
#include "util.h"
#include "vicii-mem.h"

/* PLUS60K registers */
static BYTE plus60k_reg=0;

static log_t plus60k_log = LOG_ERR;

static int plus60k_activate(void);
static int plus60k_deactivate(void);

int plus60k_enabled = 0;

unsigned int plus60k_base = 0xd100;

/* Filename of the +60K image.  */
static char *plus60k_filename = NULL;

static BYTE *plus60k_ram;

static int set_plus60k_enabled(int val, void *param)
{
    if (val == plus60k_enabled)
        return 0;

    if (!val) {
        if (plus60k_deactivate() < 0)
            return -1;

        machine_trigger_reset(MACHINE_RESET_MODE_HARD);
        plus60k_enabled = 0;
        return 0;
    } else {
        if (c64_256k_enabled || plus256k_enabled) {
#ifdef HAS_TRANSLATION
            ui_error(translate_text(IDGS_RESOURCE_S_BLOCKED_BY_S),"CPU-LINES", (c64_256k_enabled) ? "256K" : "PLUS256K");
#else
            ui_error(_("Resource %s blocked by %s."),"CPU-LINES", (c64_256k_enabled) ? "256K" : "PLUS256K");
#endif
            return -1;
        } else {
            if (plus60k_activate() < 0) {
                return -1;
            }
        }
        plus60k_enabled = 1;
        machine_trigger_reset(MACHINE_RESET_MODE_HARD);
        return 0;
    }
}

static int set_plus60k_filename(const char *name, void *param)
{
    if (plus60k_filename != NULL && name != NULL
        && strcmp(name, plus60k_filename) == 0)
        return 0;

    if (plus60k_enabled) {
        plus60k_deactivate();
        util_string_set(&plus60k_filename, name);
        plus60k_activate();
    } else {
        util_string_set(&plus60k_filename, name);
    }

    return 0;
}

static int set_plus60k_base(int val, void *param)
{
    if ((DWORD)val == plus60k_base)
        return 0;

    switch ((DWORD)val) {
      case 0xd040:
      case 0xd100:
        break;
      default:
        log_message(plus60k_log, "Unknown PLUS60K base address $%lX.",
                    (unsigned long)val);
        return -1;
    }

    if (plus60k_enabled) {
        plus60k_deactivate();
        plus60k_base = (DWORD)val;
        plus60k_activate();
    } else {
        plus60k_base = (DWORD)val;
    }

    return 0;
}

static const resource_string_t resources_string[] = {
    { "PLUS60Kfilename", "", RES_EVENT_NO, NULL,
      &plus60k_filename, set_plus60k_filename, NULL },
    { NULL }
};

static const resource_int_t resources_int[] = {
    { "PLUS60K", 0, RES_EVENT_STRICT, (resource_value_t)0,
      &plus60k_enabled, set_plus60k_enabled, NULL },
    { "PLUS60Kbase", 0xd100, RES_EVENT_NO, NULL,
      (int *)&plus60k_base, set_plus60k_base, NULL },
    { NULL }
};

int plus60k_resources_init(void)
{
    if (resources_register_string(resources_string) < 0)
        return -1;

    return resources_register_int(resources_int);
}

void plus60k_resources_shutdown(void)
{
    lib_free(plus60k_filename);
}

/* ------------------------------------------------------------------------- */

#ifdef HAS_TRANSLATION
static const cmdline_option_t cmdline_options[] =
{
    { "-plus60k", SET_RESOURCE, 0, NULL, NULL, "PLUS60K", (resource_value_t)1,
      0, IDCLS_ENABLE_PLUS60K_EXPANSION },
    { "+plus60k", SET_RESOURCE, 0, NULL, NULL, "PLUS60K", (resource_value_t)0,
      0, IDCLS_DISABLE_PLUS60K_EXPANSION },
    { "-plus60kimage", SET_RESOURCE, 1, NULL, NULL, "PLUS60Kfilename", NULL,
      IDCLS_P_NAME, IDCLS_SPECIFY_PLUS60K_NAME },
    { "-plus60kbase", SET_RESOURCE, 1, NULL, NULL, "PLUS60Kbase", NULL,
      IDCLS_P_BASE_ADDRESS, IDCLS_PLUS60K_BASE },
    { NULL }
};
#else
static const cmdline_option_t cmdline_options[] =
{
    { "-plus60k", SET_RESOURCE, 0, NULL, NULL, "PLUS60K", (resource_value_t)1,
      NULL, N_("Enable the PLUS60K RAM expansion") },
    { "+plus60k", SET_RESOURCE, 0, NULL, NULL, "PLUS60K", (resource_value_t)0,
      NULL, N_("Disable the PLUS60K RAM expansion") },
    { "-plus60kimage", SET_RESOURCE, 1, NULL, NULL, "PLUS60Kfilename", NULL,
      N_("<name>"), N_("Specify name of PLUS60K image") },
    { "-plus60kbase", SET_RESOURCE, 1, NULL, NULL, "PLUS60Kbase", NULL,
      N_("<base address>"), N_("Base address of the PLUS60K expansion") },
    { NULL }
};
#endif

int plus60k_cmdline_options_init(void)
{
    return cmdline_register_options(cmdline_options);
}

/* ------------------------------------------------------------------------- */

void plus60k_init(void)
{
    plus60k_log = log_open("PLUS60");
}

void plus60k_reset(void)
{
  plus60k_reg=0;
}

static int plus60k_activate(void)
{
    plus60k_ram = (BYTE *)lib_realloc((void *)plus60k_ram, (size_t)0xf000);

    log_message(plus60k_log, "PLUS60K expansion installed.");

    if (!util_check_null_string(plus60k_filename)) {
        if (util_file_load(plus60k_filename, plus60k_ram, (size_t)0xf000,
                           UTIL_FILE_LOAD_RAW) < 0) {
            log_message(plus60k_log,
                        "Reading PLUS60K image %s failed.", plus60k_filename);
            if (util_file_save(plus60k_filename, plus60k_ram, 0xf000) < 0) {
                log_message(plus60k_log,
                            "Creating PLUS60K image %s failed.", plus60k_filename);
                return -1;
            }
            log_message(plus60k_log, "Creating PLUS60K image %s.", plus60k_filename);
            return 0;
        }
        log_message(plus60k_log, "Reading PLUS60K image %s.", plus60k_filename);
    }

    plus60k_reset();
    return 0;
}

static int plus60k_deactivate(void)
{
    if (!util_check_null_string(plus60k_filename)) {
        if (util_file_save(plus60k_filename, plus60k_ram, 0xf000) < 0) {
            log_message(plus60k_log,
                        "Writing PLUS60K image %s failed.", plus60k_filename);
            return -1;
        }
        log_message(plus60k_log, "Writing PLUS60K image %s.", plus60k_filename);
    }
    lib_free(plus60k_ram);
    plus60k_ram = NULL;
    return 0;
}

void plus60k_shutdown(void)
{
    if (plus60k_enabled)
      plus60k_deactivate();
}

/* ------------------------------------------------------------------------- */

static void REGPARM2 plus60k_memory_store(WORD addr, BYTE value)
{
    plus60k_ram[addr-0x1000]=value;
}

static void REGPARM2 vicii_mem_vbank_store_wrapper(WORD addr, BYTE value)
{
    vicii_mem_vbank_store(addr,value);
}

static void REGPARM2 vicii_mem_vbank_39xx_store_wrapper(WORD addr, BYTE value)
{
    vicii_mem_vbank_39xx_store(addr,value);
}

static void REGPARM2 vicii_mem_vbank_3fxx_store_wrapper(WORD addr, BYTE value)
{
    vicii_mem_vbank_3fxx_store(addr,value);
}

static void REGPARM2 ram_hi_store_wrapper(WORD addr, BYTE value)
{
    ram_hi_store(addr,value);
}

static store_func_ptr_t plus60k_mem_write_tab[] =
{ vicii_mem_vbank_store_wrapper, plus60k_memory_store,
  vicii_mem_vbank_39xx_store_wrapper, plus60k_memory_store,
  vicii_mem_vbank_3fxx_store_wrapper, plus60k_memory_store,
  ram_hi_store_wrapper, plus60k_memory_store };

void REGPARM2 plus60k_vicii_mem_vbank_store(WORD addr, BYTE value)
{
  plus60k_mem_write_tab[plus60k_reg](addr, value);
}

void REGPARM2 plus60k_vicii_mem_vbank_39xx_store(WORD addr, BYTE value)
{
  plus60k_mem_write_tab[plus60k_reg+2](addr, value);
}

void REGPARM2 plus60k_vicii_mem_vbank_3fxx_store(WORD addr, BYTE value)
{
  plus60k_mem_write_tab[plus60k_reg+4](addr, value);
}

void REGPARM2 plus60k_ram_hi_store(WORD addr, BYTE value)
{
  plus60k_mem_write_tab[plus60k_reg+6](addr, value);
}

static BYTE REGPARM1 vicii_read_wrapper(WORD addr)
{
  return vicii_read(addr);
}

static void REGPARM2 vicii_store_wrapper(WORD addr, BYTE value)
{
  vicii_store(addr, value);
}

static read_func_ptr_t plus60k_partial_vicii_read_tab[] =
{ vicii_read_wrapper, plus60k_vicii_read,
  plus60k_vicii_read0, plus60k_vicii_read0 };

static store_func_ptr_t plus60k_partial_vicii_write_tab[] =
{ vicii_store_wrapper, plus60k_vicii_store,
  plus60k_vicii_store0, plus60k_vicii_store0 };

BYTE REGPARM1 plus60k_vicii_read_old(WORD addr)
{
  return plus60k_partial_vicii_read_tab[(addr&0x3f)>>6](addr);
}

void REGPARM2 plus60k_vicii_store_old(WORD addr, BYTE value)
{
  plus60k_partial_vicii_write_tab[(addr&0x3f)>>6](addr, value);
}

BYTE REGPARM1 plus60k_vicii_read(WORD addr)
{
  return 0xff;
}

BYTE REGPARM1 plus60k_vicii_read0(WORD addr)
{
  return addr>>8;
}

void REGPARM2 plus60k_vicii_store(WORD addr, BYTE value)
{
  plus60k_reg=(value&0x80)>>7;
}

void REGPARM2 plus60k_vicii_store0(WORD addr, BYTE value)
{
}

BYTE REGPARM1 plus60k_ram_read(WORD addr)
{
  if (plus60k_enabled && addr>=0x1000 && plus60k_reg==1)
    return plus60k_ram[addr-0x1000];
  else
    return mem_ram[addr];
}

void REGPARM2 plus60k_ram_store(WORD addr, BYTE value)
{
  if (plus60k_enabled && addr>=0x1000 && plus60k_reg==1)
    plus60k_ram[addr-0x1000] = value;
  else
    mem_ram[addr] = value;
}
