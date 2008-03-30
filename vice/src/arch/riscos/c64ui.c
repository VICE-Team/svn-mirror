/*
 * c64ui.c - Implementation of the C64-specific part of the UI.
 *
 * Written by
 *  Andreas Dehmel <zarquon@t-online.de>
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

#include <wimp.h>

#include "ui.h"
#include "c64ui.h"
#include "kbd.h"
#include "machine.h"
#include "c64c128ui.h"
#include "uisharedef.h"
#include "uiconfig.h"
#include "vsync.h"
#include "c64/c64mem.h"




static const char IBarIconName64[] = "!vice64";
static const char C64keyfile[] = "Vice:C64.ROdflt/vkm";

static const conf_iconid_t conf_grey_x64[] = {
  ICON_LIST_PET
  ICON_LIST_VIC
  {CONF_WIN_DEVICES, Icon_ConfDev_ACIAD67},
  {0xff, 0xff}
};


static const char Rsrc_VICIICache[] = "VICIIVideoCache";

static struct MenuVideoCache {
  RO_MenuHead head;
  RO_MenuItem item[1];
} MenuVideoCache = {
  MENU_HEADER("\\MenVCaT", 200),
  {
    MENU_ITEM_LAST("\\MenVCaVIC2")
  }
};

static struct MenuDisplayVideoCache {
  disp_desc_t dd;
  const char *values[1];
} MenuDisplayVideoCache = {
  {NULL, {CONF_WIN_VIDEO, 0},
    (RO_MenuHead*)&MenuVideoCache, 1, DISP_DESC_BITFIELD, 0},
  {Rsrc_VICIICache}
};


void c64ui_grey_out_machine_icons(void)
{
  ui_set_icons_grey(NULL, conf_grey_x64, 0);
}

void c64ui_bind_video_cache_menu(void)
{
  ConfigMenus[CONF_MENU_VIDCACHE].menu = (RO_MenuHead*)&MenuVideoCache;
  ConfigMenus[CONF_MENU_VIDCACHE].desc = (disp_desc_t*)&MenuDisplayVideoCache;
}

static const char *c64ui_get_machine_ibar_icon(void)
{
  return IBarIconName64;
}


static int c64ui_load_prg_file(const char *filename)
{
  FILE *fp;

  vsync_suspend_speed_eval();

  if ((fp = fopen(filename, "rb")) != NULL)
  {
    BYTE lo, hi;
    int length;

    lo = fgetc(fp); hi = fgetc(fp); length = lo + (hi << 8);
    length += fread(mem_ram + length, 1, C64_RAM_SIZE - length, fp);
    fclose(fp);
    mem_ram[0xc3] = lo; mem_ram[0xc4] = hi;
    lo = length & 0xff; hi = (length >> 8) & 0xff;
    mem_ram[0xae] = lo; mem_ram[0x2d] = lo; mem_ram[0x2f] = lo; mem_ram[0x31] = lo; mem_ram[0x33] = lo;
    mem_ram[0xaf] = hi; mem_ram[0x2e] = hi; mem_ram[0x30] = hi; mem_ram[0x32] = hi; mem_ram[0x34] = hi;
    return 0;
  }
  return -1;
}

static void c64ui_init_callbacks(void)
{
  c64c128_ui_cartridge_callbacks();
  ViceMachineCallbacks.load_prg_file = c64ui_load_prg_file;
}


int c64ui_init(void)
{
  wimp_msg_desc *msg;

  WimpTaskName = "Vice C64";
  c64ui_init_callbacks();
  c64ui_bind_video_cache_menu();
  msg = ui_emulator_init_prologue(c64ui_get_machine_ibar_icon());
  if (msg != NULL)
  {
    ui_emulator_init_epilogue(msg);
    c64ui_grey_out_machine_icons();
    return 0;
  }
  return -1;
}

void c64ui_shutdown(void)
{
}

int c64_kbd_init(void)
{
  c64c128_ui_init_keyboard(C64keyfile);
  kbd_load_keymap(NULL, 0);
  return kbd_init();
}
