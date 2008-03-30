/*
 * plus4ui.c - Implementation of the Plus4-specific part of the UI.
 *
 * Written by
 *  Andreas Dehmel <dehmel@forwiss.tu-muenchen.de>
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

#include "ui.h"
#include "plus4ui.h"
#include "kbd.h"
#include "resources.h"
#include "uisharedef.h"



const char *WimpTaskName = "Vice Plus4";


static unsigned char Plus4norm[KEYMAP_ENTRIES] = {
  0x17, 0x72, 0x75, 0x17,	/* 0 */
  0x72, 0x75, 0x64, 0x72,	/* 4 */
  0x75, 0xff, 0xff, 0xff,	/* 8 */
  0xff, 0xff, 0xff, 0xff,	/* 12 */
  0x76, 0x10, 0x13, 0x20,	/* 16 */
  0x03, 0x33, 0xff, 0x53,	/* 20 */
  0x23, 0x02, 0x23, 0x30,	/* 24 */
  0xff, 0xff, 0xff, 0xff,	/* 28 */
  0xff, 0x11, 0x16, 0x26,	/* 32 */
  0x30, 0x41, 0x40, 0x43,	/* 36 */
  0x53, 0x07, 0x33, 0x40,	/* 40 */
  0x77, 0x71, 0x60, 0x00,	/* 44 */
  0x70, 0x73, 0x22, 0x21,	/* 48 */
  0x23, 0x36, 0x46, 0x51,	/* 52 */
  0x55, 0x53, 0x50, 0x53,	/* 56 */
  0x01, 0x00, 0x71, 0xff,	/* 60 */
  0x17, 0x12, 0x27, 0x25,	/* 64 */
  0x31, 0x42, 0x45, 0x73,	/* 68 */
  0x55, 0x01, 0xff, 0xff,	/* 72 */
  0x54, 0xff, 0xff, 0x62,	/* 76 */
  0xff, 0x15, 0x24, 0x32,	/* 80 */
  0x35, 0x47, 0x52, 0x55,	/* 84 */
  0x61, 0x00, 0xff, 0x33,	/* 88 */
  0xff, 0x65, 0xff, 0xff,	/* 92 */
  0x72, 0x14, 0x74, 0x37,	/* 96 */
  0x34, 0x44, 0x57, 0x54,	/* 100 */
  0x67, 0xff, 0x43, 0x70,	/* 104 */
  0x10, 0xff, 0xff, 0xff,	/* 108 */
  0x77, 0x04, 0x05, 0x06,	/* 112 */
  0xff, 0xff, 0xff, 0xff,	/* 116 */
  0x60, 0x02, 0x13, 0x20,	/* 120 */
  0x73, 0xff, 0xff, 0xff
};

static unsigned char Plus4shifted[KEYMAP_ENTRIES];
static unsigned char Plus4norm_sflag[KEYMAP_ENTRIES/8];
static unsigned char Plus4shift_sflag[KEYMAP_ENTRIES/8];

static const char IBarIconName[] = "!viceplus4";
static const char Plus4keyfile[] = "Vice:PLUS4.ROdflt/vkm";

static keymap_t Plus4keys = {
  Plus4keyfile,
  Plus4norm,
  Plus4shifted,
  Plus4norm_sflag,
  Plus4shift_sflag
};

static const conf_iconid_t conf_grey_xplus4[] = {
  ICON_LIST_CART64
  ICON_LIST_VIC
  ICON_LIST_SYSTEM
  ICON_LIST_SID
  ICON_LIST_PET
  {0xff, 0xff}
};


static const char Rsrc_TEDCache[] = "TEDVideoCache";

static struct MenuVideoCache {
  RO_MenuHead head;
  RO_MenuItem item[1];
} MenuVideoCache = {
  MENU_HEADER("\\MenVCaT", 200),
  {
    MENU_ITEM_LAST("\\MenVCaTED")
  }
};

static struct MenuDisplayVideoCache {
  disp_desc_t dd;
  const char *values[1];
} MenuDisplayVideoCache = {
  {NULL, {CONF_WIN_VIDEO, 0},
    (RO_MenuHead*)&MenuVideoCache, 1, DISP_DESC_BITFIELD, 0},
  {Rsrc_TEDCache}
};


int plus4_ui_init(void)
{
  return ui_init_named_app("VicePlus4", IBarIconName);
}

int plus4_kbd_init(void)
{
  kbd_default_keymap(&Plus4keys);
  kbd_init_keymap(2);
  kbd_add_keymap(&Plus4keys, 0); kbd_add_keymap(&Plus4keys, 1);
  kbd_load_keymap(NULL, 0);
  return kbd_init();
}

void ui_grey_out_machine_icons(void)
{
  ui_set_icons_grey(NULL, conf_grey_xplus4, 0);
}

void ui_bind_video_cache_menu(void)
{
  ConfigMenus[CONF_MENU_VIDCACHE].menu = (RO_MenuHead*)&MenuVideoCache;
  ConfigMenus[CONF_MENU_VIDCACHE].desc = (disp_desc_t*)&MenuDisplayVideoCache;
}

const char *ui_get_machine_ibar_icon(void)
{
  return IBarIconName;
}



/* dummy stubs for linker */
char *pet_get_keyboard_name(void)
{
  return NULL;
}

int pet_set_model(const char *name, void *extra)
{
  return 0;
}

char *cbm2_get_keyboard_name(void)
{
  return NULL;
}

int cbm2_set_model(const char *name, void *extra)
{
  return 0;
}
