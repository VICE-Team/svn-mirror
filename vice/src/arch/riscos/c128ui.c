/*
 * c128ui.c - Implementation of the C128-specific part of the UI.
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
#include "c128ui.h"
#include "c64ui.h"
#include "c64c128ui.h"
#include "kbd.h"
#include "uisharedef.h"
#include "uiconfig.h"




static const char Rsrc_VDCpalette[] = "VDC_PaletteFile";
static const char Rsrc_Z80Bios[] = "Z80BiosName";

static const char IBarIconName[] = "!vice128";
static const char C128keyfile[] = "Vice:C128.ROdflt/vkm";

static const conf_iconid_t conf_grey_x128[] = {
  ICON_LIST_PET
  ICON_LIST_VIC
  {0xff, 0xff}
};


static const char Rsrc_VICIICache[] = "VICIIVideoCache";
static const char Rsrc_VDCCache[] = "VDCVideoCache";

static struct MenuVideoCache {
  RO_MenuHead head;
  RO_MenuItem item[2];
} MenuVideoCache = {
  MENU_HEADER("\\MenVCaT", 200),
  {
    MENU_ITEM("\\MenVCaVIC2"),
    MENU_ITEM_LAST("\\MenVCaVDC")
  }
};

static struct MenuDisplayVideoCache {
  disp_desc_t dd;
  const char *values[2];
} MenuDisplayVideoCache = {
  {NULL, {CONF_WIN_VIDEO, 0},
    (RO_MenuHead*)&MenuVideoCache, 2, DISP_DESC_BITFIELD, 0},
  {Rsrc_VICIICache, Rsrc_VDCCache}
};


static void c128ui_grey_out_machine_icons(void)
{
  ui_set_icons_grey(NULL, conf_grey_x128, 0);
}

static void c128ui_bind_video_cache_menu(void)
{
  c64c128_ui_cartridge_callbacks();
  ConfigMenus[CONF_MENU_VIDCACHE].menu = (RO_MenuHead*)&MenuVideoCache;
  ConfigMenus[CONF_MENU_VIDCACHE].desc = (disp_desc_t*)&MenuDisplayVideoCache;
}

static const char *c128ui_get_machine_ibar_icon(void)
{
  return IBarIconName;
}

static int c128ui_key_pressed_config(int *block, int wnum, const char *data)
{
  if (wnum == CONF_WIN_C128)
  {
    if (block[KeyPB_Icon] == Icon_Conf128_C128Palette)
    {
      resources_set_string(Rsrc_VDCpalette, data);
      return 0;
    }
  }
  return -1;
}

static int c128ui_usr_msg_data_load(int *block)
{
  if (block[5] == ConfWindows[CONF_WIN_C128]->Handle)
  {
    if ((block[6] == Icon_Conf128_C128Palette) || (block[6] == Icon_Conf128_C128z80bios))
    {
      const char *filename, *rsrc;

      if (block[6] == Icon_Conf128_C128Palette)
        rsrc = Rsrc_VDCpalette;
      else
        rsrc = Rsrc_Z80Bios;

      filename = ui_check_for_syspath(((const char*)block)+44);
      if (resources_set_string(rsrc, filename) == 0)
      {
        wimp_window_write_icon_text(ConfWindows[CONF_WIN_C128], block[6], filename);
      }
      return 0;
    }
  }
  return -1;
}


static void c128ui_init_callbacks(void)
{
  c64c128_ui_cartridge_callbacks();
  ViceMachineCallbacks.key_pressed_config = c128ui_key_pressed_config;
  ViceMachineCallbacks.usr_msg_data_load = c128ui_usr_msg_data_load;
}

int c128ui_init(void)
{
  wimp_msg_desc *msg;

  WimpTaskName = "Vice C128";
  c128ui_init_callbacks();
  c128ui_bind_video_cache_menu();
  msg = ui_emulator_init_prologue(c128ui_get_machine_ibar_icon());
  if (msg != NULL)
  {
    ui_load_template("C128Config", ConfWindows + CONF_WIN_C128, msg);
    ui_emulator_init_epilogue(msg);
    c128ui_grey_out_machine_icons();
    return 0;
  }
  return -1;
}

void c128ui_shutdown(void)
{
}

int c128_kbd_init(void)
{
  c64c128_ui_init_keyboard(C128keyfile);
  kbd_load_keymap(NULL, 0);
  return kbd_init();
}
