/*
 * plus256k.c - +256K EXPANSION emulation.
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
#include "c64cia.h"
#include "c64export.h"
#include "c64mem.h"
#include "cartridge.h"
#include "cmdline.h"
#include "interrupt.h"
#include "lib.h"
#include "log.h"
#include "machine.h"
#include "maincpu.h"
#include "mem.h"
#include "plus256k.h"
#include "plus60k.h"
#include "resources.h"
#include "reu.h"
#include "snapshot.h"
#ifdef HAS_TRANSLATION
#include "translate.h"
#endif
#include "types.h"
#include "uiapi.h"
#include "util.h"
#include "vicii.h"

/* PLUS256K registers */
static BYTE plus256k_reg=0;

static log_t plus256k_log = LOG_ERR;

static int plus256k_activate(void);
static int plus256k_deactivate(void);

int plus256k_enabled=0;

static int plus256k_video_bank=0;
static int plus256k_low_bank=0;
static int plus256k_high_bank=0;
static int plus256k_protected=0;

/* Filename of the +256K image.  */
static char *plus256k_filename = NULL;

BYTE *plus256k_ram=NULL;

static int set_plus256k_enabled(resource_value_t v, void *param)
{
  if ((int)v == plus256k_enabled)
      return 0;

  if (!(int)v)
  {
    if (plus256k_deactivate() < 0)
    {
      return -1;
    }
    machine_trigger_reset(MACHINE_RESET_MODE_HARD);
    plus256k_enabled = 0;
    return 0;
  }
  else
  { 
    if (plus60k_enabled || c64_256k_enabled)
    {
#ifdef HAS_TRANSLATION
      ui_error(translate_text(IDGS_RESOURCE_S_BLOCKED_BY_S),"CPU-LINES", (plus60k_enabled) ? "PLUS60K" : "256K");
#else
      ui_error(_("Resource %s blocked by %s."),"CPU-LINES", (plus60k_enabled) ? "PLUS60K" : "256K");
#endif
      return -1;
    }
    else
    {
      if (plus256k_activate() < 0)
      {
        return -1;
      }
    }
    machine_trigger_reset(MACHINE_RESET_MODE_HARD);
    plus256k_enabled = 1;
    return 0;
  }
}

static int set_plus256k_filename(resource_value_t v, void *param)
{
  const char *name = (const char *)v;

  if (plus256k_filename != NULL && name != NULL && strcmp(name, plus256k_filename) == 0)
    return 0;

  if (plus256k_enabled)
  {
    plus256k_deactivate();
    util_string_set(&plus256k_filename, name);
    plus256k_activate();
  }
  else
  {
    util_string_set(&plus256k_filename, name);
  }
  return 0;
}

static const resource_t resources[] = {
    { "PLUS256K", RES_INTEGER, (resource_value_t)0,
      RES_EVENT_SAME, NULL,
      (void *)&plus256k_enabled, set_plus256k_enabled, NULL },
    { "PLUS256Kfilename", RES_STRING, (resource_value_t)"",
      RES_EVENT_NO, NULL,
      (void *)&plus256k_filename, set_plus256k_filename, NULL },
    { NULL }
};

int plus256k_resources_init(void)
{
  return resources_register(resources);
}

void plus256k_resources_shutdown(void)
{
  lib_free(plus256k_filename);
}

/* ------------------------------------------------------------------------- */

#ifdef HAS_TRANSLATION
static const cmdline_option_t cmdline_options[] =
{
    { "-plus256k", SET_RESOURCE, 0, NULL, NULL, "PLUS256K", (resource_value_t)1,
      0, IDCLS_ENABLE_PLUS256K_EXPANSION },
    { "+plus256k", SET_RESOURCE, 0, NULL, NULL, "PLUS256K", (resource_value_t)0,
      0, IDCLS_DISABLE_PLUS256K_EXPANSION },
    { "-plus256kimage", SET_RESOURCE, 1, NULL, NULL, "PLUS256Kfilename", NULL,
      IDCLS_P_NAME, IDCLS_SPECIFY_PLUS256K_NAME },
    { NULL }
};
#else
static const cmdline_option_t cmdline_options[] =
{
    { "-plus256k", SET_RESOURCE, 0, NULL, NULL, "PLUS256K", (resource_value_t)1,
      NULL, N_("Enable the PLUS256K RAM expansion") },
    { "+plus256k", SET_RESOURCE, 0, NULL, NULL, "PLUS256K", (resource_value_t)0,
      NULL, N_("Disable the PLUS256K RAM expansion") },
    { "-plus256kimage", SET_RESOURCE, 1, NULL, NULL, "PLUS256Kfilename", NULL,
      N_("<name>"), N_("Specify name of PLUS256K image") },
    { NULL }
};
#endif

int plus256k_cmdline_options_init(void)
{
  return cmdline_register_options(cmdline_options);
}

/* ------------------------------------------------------------------------- */

void plus256k_init(void)
{
  plus256k_log = log_open("PLUS256K");
}

void plus256k_reset(void)
{
  plus256k_reg=0;
  plus256k_video_bank=0;
  plus256k_low_bank=0;
  plus256k_high_bank=0;
  plus256k_protected=0;
  if (plus256k_enabled)
  {
    vicii_set_ram_base(plus256k_ram);
  }
}

static int plus256k_activate(void)
{
  plus256k_ram = (BYTE *)lib_realloc((void *)plus256k_ram, (size_t)0x40000);

  log_message(plus256k_log, "PLUS256K hack installed.");

  if (!util_check_null_string(plus256k_filename))
  {
    if (util_file_load(plus256k_filename, plus256k_ram, (size_t)0x40000, UTIL_FILE_LOAD_RAW) < 0)
    {
      log_message(plus256k_log, "Reading PLUS256K image %s failed.", plus256k_filename);
      if (util_file_save(plus256k_filename, plus256k_ram, 0x40000) < 0)
      {
        log_message(plus256k_log, "Creating PLUS256K image %s failed.", plus256k_filename);
        return -1;
      }
      log_message(plus256k_log, "Creating PLUS256K image %s.", plus256k_filename);
      return 0;
    }
    log_message(plus256k_log, "Reading PLUS256K image %s.", plus256k_filename);
  }
  plus256k_reset();
  return 0;
}

static int plus256k_deactivate(void)
{
  if (!util_check_null_string(plus256k_filename))
  {
    if (util_file_save(plus256k_filename, plus256k_ram, 0x40000) < 0)
    {
      log_message(plus256k_log, "Writing PLUS256K image %s failed.", plus256k_filename);
            return -1;
    }
    log_message(plus256k_log, "Writing PLUS256K image %s.", plus256k_filename);
  }
  vicii_set_ram_base(mem_ram);
  lib_free(plus256k_ram);
  plus256k_ram = NULL;
  return 0;
}

void plus256k_shutdown(void)
{
  if (plus256k_enabled)
    plus256k_deactivate();
}

/* ------------------------------------------------------------------------- */

BYTE REGPARM1 plus256k_vicii_read(WORD addr)
{
  return 0xff;
}

BYTE REGPARM1 plus256k_vicii_read0(WORD addr)
{
  return addr>>8;
}

void REGPARM2 plus256k_vicii_store(WORD addr, BYTE value)
{
  int new_bank;

  if (plus256k_protected==0)
  {
    plus256k_reg=value;
    plus256k_high_bank=(value&0xc0)>>6;
    plus256k_low_bank=value&3;
    plus256k_protected=(value&0x10)>>4;
    new_bank=(value&0xc)>>2;
    if (new_bank!=plus256k_video_bank)
    {
      vicii_set_ram_base(plus256k_ram+(new_bank*0x10000));
      plus256k_video_bank=new_bank;
    }
  }
}

void REGPARM2 plus256k_vicii_store0(WORD addr, BYTE value)
{
}

void REGPARM2 plus256k_ram_low_store(WORD addr, BYTE value)
{
  plus256k_ram[(plus256k_low_bank*0x10000)+addr]=value;
}

void REGPARM2 plus256k_ram_high_store(WORD addr, BYTE value)
{
  plus256k_ram[(plus256k_high_bank*0x10000)+addr]=value;
  if (addr == 0xff00)
    reu_dma(-1);
}

BYTE REGPARM1 plus256k_ram_low_read(WORD addr)
{
  return plus256k_ram[(plus256k_low_bank*0x10000)+addr];
}

BYTE REGPARM1 plus256k_ram_high_read(WORD addr)
{
  return plus256k_ram[(plus256k_high_bank*0x10000)+addr];
}
