/*
 * c64_256k.c - 256K EXPANSION emulation.
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
#include "reu.h"
#include "snapshot.h"
#ifdef HAS_TRANSLATION
#include "translate.h"
#endif
#include "types.h"
#include "uiapi.h"
#include "util.h"
#include "vicii.h"

/* 256K registers */
BYTE c64_256k_DDA;
BYTE c64_256k_PRA;
BYTE c64_256k_CRA;
BYTE c64_256k_DDB;
BYTE c64_256k_PRB;
BYTE c64_256k_CRB;

int c64_256k_start;

static log_t c64_256k_log = LOG_ERR;

static int c64_256k_activate(void);
static int c64_256k_deactivate(void);

int c64_256k_enabled = 0;

int cia_vbank;
int video_bank_segment;

int c64_256k_segment0;
int c64_256k_segment1;
int c64_256k_segment2;
int c64_256k_segment3;

/* Filename of the 256K image.  */
static char *c64_256k_filename = NULL;

BYTE *c64_256k_ram = NULL;

static int set_c64_256k_enabled(int val, void *param)
{
    if (val == c64_256k_enabled)
        return 0;

    if (!val) {
        if (c64_256k_deactivate() < 0) {
            return -1;
        }
        machine_trigger_reset(MACHINE_RESET_MODE_HARD);
        c64_256k_enabled = 0;
        return 0;
    } else {
        if (plus60k_enabled || plus256k_enabled) {
#ifdef HAS_TRANSLATION
            ui_error(translate_text(IDGS_RESOURCE_S_BLOCKED_BY_S),"CPU-LINES", (plus60k_enabled) ? "PLUS60K" : "PLUS256K");
#else
            ui_error(_("Resource %s blocked by %s."),"CPU-LINES", (plus60k_enabled) ? "PLUS60K" : "PLUS256K");
#endif
            return -1;
        } else {
            if (c64_256k_activate() < 0) {
                return -1;
            }
        }
        machine_trigger_reset(MACHINE_RESET_MODE_HARD);
        c64_256k_enabled = 1;
        return 0;
    }
}

static int set_c64_256k_filename(const char *name, void *param)
{
    if (c64_256k_filename != NULL && name != NULL
        && strcmp(name, c64_256k_filename) == 0)
        return 0;

    if (name != NULL && *name != '\0') {
        if (util_check_filename_access(name) < 0)
            return -1;
    }

    if (c64_256k_enabled) {
        c64_256k_deactivate();
        util_string_set(&c64_256k_filename, name);
        c64_256k_activate();
    } else {
        util_string_set(&c64_256k_filename, name);
    }

    return 0;
}

static int set_c64_256k_base(int val, void *param)
{
    if (val == c64_256k_start)
        return 0;

    switch (val) {
      case 0xde00:
      case 0xde80:
      case 0xdf00:
      case 0xdf80:
        break;
      default:
        log_message(c64_256k_log, "Unknown 256K base %X.", val);
        return -1;
    }

    c64_256k_start = val;

    return 0;
}

static const resource_string_t resources_string[] = {
    { "C64_256Kfilename", "", RES_EVENT_NO, NULL,
      &c64_256k_filename, set_c64_256k_filename, NULL },
    { NULL }
};

static const resource_int_t resources_int[] = {
    { "C64_256K", 0, RES_EVENT_STRICT, (resource_value_t)0,
      &c64_256k_enabled, set_c64_256k_enabled, NULL },
    { "C64_256Kbase", 0xdf80, RES_EVENT_NO, NULL,
      &c64_256k_start, set_c64_256k_base, NULL },
    { NULL }
};

int c64_256k_resources_init(void)
{
    if (resources_register_string(resources_string) < 0)
        return -1;

    return resources_register_int(resources_int);
}

void c64_256k_resources_shutdown(void)
{
  lib_free(c64_256k_filename);
}

/* ------------------------------------------------------------------------- */

#ifdef HAS_TRANSLATION
static const cmdline_option_t cmdline_options[] =
{
    { "-256k", SET_RESOURCE, 0, NULL, NULL, "C64_256K", (resource_value_t)1,
      0, IDCLS_ENABLE_C64_256K_EXPANSION },
    { "+256k", SET_RESOURCE, 0, NULL, NULL, "C64_256K", (resource_value_t)0,
      0, IDCLS_DISABLE_C64_256K_EXPANSION },
    { "-256kimage", SET_RESOURCE, 1, NULL, NULL, "C64_256Kfilename", NULL,
      IDCLS_P_NAME, IDCLS_SPECIFY_C64_256K_NAME },
    { "-256kbase", SET_RESOURCE, 1, NULL, NULL, "C64_256Kbase", NULL,
      IDCLS_P_BASE_ADDRESS, IDCLS_C64_256K_BASE },
    { NULL }
};
#else
static const cmdline_option_t cmdline_options[] =
{
    { "-256k", SET_RESOURCE, 0, NULL, NULL, "C64_256K", (resource_value_t)1,
      NULL, N_("Enable the 256K RAM expansion") },
    { "+256k", SET_RESOURCE, 0, NULL, NULL, "C64_256K", (resource_value_t)0,
      NULL, N_("Disable the 256K RAM expansion") },
    { "-256kimage", SET_RESOURCE, 1, NULL, NULL, "C64_256Kfilename", NULL,
      N_("<name>"), N_("Specify name of 256K image") },
    { "-256kbase", SET_RESOURCE, 1, NULL, NULL, "C64_256Kbase", NULL,
      N_("<base address>"), N_("Base address of the 256K expansion") },
    { NULL }
};
#endif

int c64_256k_cmdline_options_init(void)
{
  return cmdline_register_options(cmdline_options);
}

/* ------------------------------------------------------------------------- */

void c64_256k_init(void)
{
  c64_256k_log = log_open("C64_256K");
}

void c64_256k_reset(void)
{
  c64_256k_DDA=0;
  c64_256k_DDB=0;
  c64_256k_PRA=0xdc;
  c64_256k_PRB=0xfe;
  c64_256k_CRA=4;
  c64_256k_CRB=4;
  cia_vbank=0;
  video_bank_segment=0xc;
  c64_256k_segment0=0xc;
  c64_256k_segment1=0xd;
  c64_256k_segment2=0xe;
  c64_256k_segment3=0xf;
  if (c64_256k_enabled)
  {
    vicii_set_ram_base(c64_256k_ram+0x30000);
    mem_set_vbank(0);
  }
}

void c64_256k_cia_set_vbank(int ciabank)
{
  cia_vbank=ciabank;
  video_bank_segment=((c64_256k_PRB&0xc0)>>4)+cia_vbank;
  vicii_set_ram_base(c64_256k_ram+(video_bank_segment*0x4000));
  mem_set_vbank(0);
}

void pia_set_vbank(void)
{
  video_bank_segment=((c64_256k_PRB&0xc0)>>4)+cia_vbank;
  vicii_set_ram_base(c64_256k_ram+(video_bank_segment*0x4000));
  mem_set_vbank(0);
}

static int c64_256k_activate(void)
{
  c64_256k_ram = (BYTE *)lib_realloc((void *)c64_256k_ram, (size_t)0x40000);

  log_message(c64_256k_log, "256K hack installed.");

  if (!util_check_null_string(c64_256k_filename))
  {
    if (util_file_load(c64_256k_filename, c64_256k_ram, (size_t)0x40000, UTIL_FILE_LOAD_RAW) < 0)
    {
      log_message(c64_256k_log, "Reading 256K image %s failed.", c64_256k_filename);
      if (util_file_save(c64_256k_filename, c64_256k_ram, 0x40000) < 0)
      {
        log_message(c64_256k_log, "Creating 256K image %s failed.", c64_256k_filename);
        return -1;
      }
      log_message(c64_256k_log, "Creating 256K image %s.", c64_256k_filename);
      return 0;
    }
    log_message(c64_256k_log, "Reading 256K image %s.", c64_256k_filename);
  }
  c64_256k_reset();
  return 0;
}

static int c64_256k_deactivate(void)
{
  if (!util_check_null_string(c64_256k_filename))
  {
    if (util_file_save(c64_256k_filename, c64_256k_ram, 0x40000) < 0)
    {
      log_message(c64_256k_log, "Writing 256K image %s failed.", c64_256k_filename);
            return -1;
    }
    log_message(c64_256k_log, "Writing 256K image %s.", c64_256k_filename);
  }
  vicii_set_ram_base(mem_ram);
  lib_free(c64_256k_ram);
  c64_256k_ram = NULL;
  return 0;
}

void c64_256k_shutdown(void)
{
  if (c64_256k_enabled)
    c64_256k_deactivate();
}

/* ------------------------------------------------------------------------- */

BYTE REGPARM1 c64_256k_read(WORD addr)
{
  BYTE retval=0;

  io_source=IO_SOURCE_C64_256K;
  if (addr==1)
    retval=c64_256k_CRA;
  if (addr==3)
    retval=c64_256k_CRB;
  if (addr==0 && (c64_256k_CRA&4)==4)
    retval=c64_256k_PRA;
  if (addr==0 && (c64_256k_CRA&4)==0)
    retval=c64_256k_DDA;
  if (addr==2 && (c64_256k_CRB&4)==4)
    retval=c64_256k_PRB;
  if (addr==2 && (c64_256k_CRB&4)==0)
    retval=c64_256k_DDB;

  return retval;
}

void REGPARM2 c64_256k_store(WORD addr, BYTE byte)
{
  BYTE old_prb;

  if (addr==1)
    c64_256k_CRA=byte&0x3f;
  if (addr==3)
    c64_256k_CRB=byte&0x3f;
  if (addr==0 && (c64_256k_CRA&4)==4)
  {
    if (c64_256k_PRA!=byte)
    {
      c64_256k_PRA=byte;
      c64_256k_segment0=(c64_256k_PRA&0xf);
      c64_256k_segment1=(c64_256k_PRA&0xf0)>>4;
    }
  }
  if (addr==0 && (c64_256k_CRA&4)==0)
    c64_256k_DDA=byte;
  if (addr==2 && (c64_256k_CRB&4)==4)
  {
    if (c64_256k_PRB!=byte)
    {
      old_prb=c64_256k_PRB;
      c64_256k_PRB=byte;
      c64_256k_segment2=(c64_256k_PRB&0xf);
      c64_256k_segment3=(c64_256k_PRB&0xf0)>>4;
      if ((old_prb&0xc0)!=(byte&0xc0))
        pia_set_vbank();
    }
  }
  if (addr==2 && (c64_256k_CRB&4)==0)
    c64_256k_DDB=byte;
}

void REGPARM2 c64_256k_ram_segment0_store(WORD addr, BYTE value)
{
  c64_256k_ram[(c64_256k_segment0*0x4000)+(addr&0x3fff)]=value;
  if (addr == 0xff00)
    reu_dma(-1);
}

void REGPARM2 c64_256k_ram_segment1_store(WORD addr, BYTE value)
{
  c64_256k_ram[(c64_256k_segment1*0x4000)+(addr&0x3fff)]=value;
  if (addr == 0xff00)
    reu_dma(-1);
}

void REGPARM2 c64_256k_ram_segment2_store(WORD addr, BYTE value)
{
  c64_256k_ram[(c64_256k_segment2*0x4000)+(addr&0x3fff)]=value;
  if (addr == 0xff00)
    reu_dma(-1);
}

void REGPARM2 c64_256k_ram_segment3_store(WORD addr, BYTE value)
{
  c64_256k_ram[(c64_256k_segment3*0x4000)+(addr&0x3fff)]=value;
  if (addr == 0xff00)
    reu_dma(-1);
}

BYTE REGPARM1 c64_256k_ram_segment0_read(WORD addr)
{
  return c64_256k_ram[(c64_256k_segment0*0x4000)+(addr&0x3fff)];
}

BYTE REGPARM1 c64_256k_ram_segment1_read(WORD addr)
{
  return c64_256k_ram[(c64_256k_segment1*0x4000)+(addr&0x3fff)];
}

BYTE REGPARM1 c64_256k_ram_segment2_read(WORD addr)
{
  return c64_256k_ram[(c64_256k_segment2*0x4000)+(addr&0x3fff)];
}

BYTE REGPARM1 c64_256k_ram_segment3_read(WORD addr)
{
  return c64_256k_ram[(c64_256k_segment3*0x4000)+(addr&0x3fff)];
}
