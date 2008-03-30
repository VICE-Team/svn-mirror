/*
 * cbm2ui.c - Implementation of the CBM-II-specific part of the UI.
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
#include "cbm2ui.h"
#include "kbd.h"
#include "resources.h"
#include "uisharedef.h"
#include "utils.h"

#include "cbm2/cbm2mem.h"




static unsigned char CBM2norm[KEYMAP_ENTRIES] = {
  0x84, 0x85, 0xff, 0x84,	/* 0 */
  0x85, 0xff, 0x83, 0x41,	/* 4 */
  0xff, 0xff, 0xff, 0xff,	/* 8 */
  0xff, 0xff, 0xff, 0xff,	/* 12 */
  0x92, 0xb1, 0xc1, 0xd1,	/* 16 */
  0xb0, 0xf1, 0xe0, 0x12,	/* 20 */
  0xd2, 0x31, 0x63, 0x42,	/* 24 */
  0xff, 0xff, 0x10, 0xff,	/* 28 */
  0xff, 0xa2, 0xb2, 0xc3,	/* 32 */
  0xe1, 0xf2, 0x01, 0x11,	/* 36 */
  0x12, 0x20, 0x52, 0x62,	/* 40 */
  0x70, 0x25, 0xff, 0x33,	/* 44 */
  0x91, 0xa1, 0xb3, 0xc2,	/* 48 */
  0xd2, 0xe2, 0x02, 0x13,	/* 52 */
  0xa1, 0x30, 0x73, 0x72,	/* 56 */
  0x74, 0x33, 0x40, 0xff,	/* 60 */
  0xff, 0x93, 0xa4, 0xb4,	/* 64 */
  0xd3, 0xe3, 0xf3, 0xa1,	/* 68 */
  0x04, 0x24, 0x71, 0xff,	/* 72 */
  0x55, 0xff, 0xff, 0x15,	/* 76 */
  0xff, 0xa3, 0xa5, 0xc4,	/* 80 */
  0xd4, 0xd5, 0x03, 0x04,	/* 84 */
  0xf1, 0x33, 0x55, 0x61,	/* 88 */
  0xff, 0x21, 0xff, 0xff,	/* 92 */
  0x82, 0x94, 0xe5, 0xb5,	/* 96 */
  0xc5, 0xe4, 0xf4, 0xf5,	/* 100 */
  0x05, 0xff, 0x45, 0x44,	/* 104 */
  0x64, 0xff, 0xff, 0xff,	/* 108 */
  0x70, 0x80, 0x90, 0xa0,	/* 112 */
  0xc0, 0xd0, 0xf0, 0x00,	/* 116 */
  0x22, 0x32, 0x43, 0x53,	/* 120 */
  0x54, 0xff, 0xff, 0xff
};

static unsigned char CBM2shifted[KEYMAP_ENTRIES];
static unsigned char CBM2norm_sflag[KEYMAP_ENTRIES/8];
static unsigned char CBM2shift_sflag[KEYMAP_ENTRIES/8];

static const char IBarIconName[] = "!vicecbm2";
static const char CBM2keyfile[] = "Vice:CBM-II.ROdflt/vkm";

static kbd_keymap_t CBM2keys = {
  CBM2keyfile,
  CBM2norm,
  CBM2shifted,
  CBM2norm_sflag,
  CBM2shift_sflag
};

static const conf_iconid_t conf_grey_xcbm2[] = {
  ICON_LIST_CART64
  ICON_LIST_VIC
  ICON_LIST_SYSTEM
  ICON_LIST_PET
  ICON_LIST_DEVRSUSR
  {0xff, 0xff}
};


static const char Rsrc_VICIICache[] = "VICIIVideoCache";
static const char Rsrc_CrtcCache[] = "CrtcVideoCache";

static struct MenuVideoCache {
  RO_MenuHead head;
  RO_MenuItem item[2];
} MenuVideoCache = {
  MENU_HEADER("\\MenVCaT", 200),
  {
    MENU_ITEM("\\MenVCaVIC2"),
    MENU_ITEM_LAST("\\MenVCaCrt")
  }
};

static struct MenuDisplayVideoCache {
  disp_desc_t dd;
  const char *values[2];
} MenuDisplayVideoCache = {
  {NULL, {CONF_WIN_VIDEO, 0},
    (RO_MenuHead*)&MenuVideoCache, 2, DISP_DESC_BITFIELD, 0},
  {Rsrc_VICIICache, Rsrc_CrtcCache}
};



/* CBM2 keyboard names */
static char CBM2keyBusinessName[] = "Business";
static char CBM2keyGraphicName[] = "Graphic";
static char *CBM2ModelName = NULL;

static void cbm2ui_grey_out_machine_icons(void)
{
  ui_set_icons_grey(NULL, conf_grey_xcbm2, 0);
}

static void cbm2ui_bind_video_cache_menu(void)
{
  ConfigMenus[CONF_MENU_VIDCACHE].menu = (RO_MenuHead*)&MenuVideoCache;
  ConfigMenus[CONF_MENU_VIDCACHE].desc = (disp_desc_t*)&MenuDisplayVideoCache;
}

static const char *cbm2ui_get_machine_ibar_icon(void)
{
  return IBarIconName;
}

static const char *cbm2_get_keyboard_name(void)
{
  resource_value_t val;
  int idx;

  if (resources_get_value("KeymapIndex", (void *)&val) != 0) val = 0;
  idx = (int)val;
  if ((idx & 2) == 0) return CBM2keyBusinessName;
  return CBM2keyGraphicName;
}

static int set_cbm2_model_by_name(const char *name, resource_value_t val)
{
  util_string_set(&CBM2ModelName, (char*)val);
  return cbm2_set_model(CBM2ModelName, NULL);
}

static int cbm2ui_setup_config_window(int wnum)
{
  if (wnum == CONF_WIN_CBM2)
  {
    wimp_window_write_icon_text(ConfWindows[CONF_WIN_CBM2], Icon_ConfCBM_CBM2Kbd, cbm2_get_keyboard_name());
    return 0;
  }
  return -1;
}

static int cbm2ui_menu_select_config(int *block, int wnum)
{
  if (wnum == CONF_MENU_C2MODEL)
  {
    ui_set_menu_display_core(ConfigMenus[CONF_MENU_C2MODEL].desc, set_cbm2_model_by_name, block[0]);
    ui_setup_menu_display(ConfigMenus[CONF_MENU_C2MEM].desc);
    ui_setup_menu_display(ConfigMenus[CONF_MENU_C2RAM].desc);
    ui_setup_menu_display(ConfigMenus[CONF_MENU_C2LINE].desc);
    wimp_window_write_icon_text(ConfWindows[CONF_WIN_CBM2], Icon_ConfCBM_CBM2Kbd, cbm2_get_keyboard_name());
    ui_update_rom_names();
    return 0;
  }
  return -1;
}

static void cbm2ui_init_callbacks(void)
{
  ViceMachineCallbacks.setup_config_window = cbm2ui_setup_config_window;
  ViceMachineCallbacks.menu_select_config_main = cbm2ui_menu_select_config;
}

int cbm2ui_init(void)
{
  wimp_msg_desc *msg;

  WimpTaskName = "Vice CBM-II";
  cbm2ui_init_callbacks();
  cbm2ui_bind_video_cache_menu();
  msg = ui_emulator_init_prologue(cbm2ui_get_machine_ibar_icon());
  if (msg != NULL)
  {
    util_string_set(&CBM2ModelName, "610");
    ui_load_template("CBM2Config", ConfWindows + CONF_WIN_CBM2, msg);
    ui_emulator_init_epilogue(msg);
    cbm2ui_grey_out_machine_icons();
    return 0;
  }
  return -1;
}

void cbm2ui_shutdown(void)
{
}

int cbm2_kbd_init(void)
{
  kbd_default_keymap(&CBM2keys);
  kbd_init_keymap(2);
  kbd_add_keymap(&CBM2keys, 0); kbd_add_keymap(&CBM2keys, 1);
  kbd_load_keymap(NULL, 0);
  return kbd_init();
}
