/*
 * petui.c - Implementation of the PET-specific part of the UI.
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

#include <string.h>


#include "ui.h"
#include "petui.h"
#include "resources.h"
#include "kbd.h"
#include "uisharedef.h"




const char *WimpTaskName = "Vice PET";


/* Modified PET keymap */
static unsigned char PETnormBusiness[KEYMAP_ENTRIES] = {
  0x60, 0x20, 0xff, 0x60,	/* 0 */
  0x20, 0xff, 0x66, 0x20,	/* 4 */
  0xff, 0xff, 0xff, 0xff,	/* 8 */
  0xff, 0xff, 0xff, 0xff,	/* 12 */
  0x50, 0x91, 0x11, 0x01,	/* 16 */
  0xff, 0x02, 0xff, 0x03,	/* 20 */
  0x92, 0x05, 0x37, 0x14,	/* 24 */
  0xff, 0xff, 0xff, 0xff,	/* 28 */
  0xff, 0x41, 0x51, 0x52,	/* 32 */
  0x12, 0x45, 0x93, 0x13,	/* 36 */
  0x03, 0x54, 0x04, 0x17,	/* 40 */
  0x94, 0x90, 0xff, 0x47,	/* 44 */
  0x10, 0x00, 0x31, 0x42,	/* 48 */
  0x92, 0x53, 0x55, 0x46,	/* 52 */
  0x36, 0x54, 0x26, 0x03,	/* 56 */
  0x34, 0x47, 0x84, 0xff,	/* 60 */
  0xff, 0x30, 0x81, 0x22,	/* 64 */
  0x43, 0x33, 0x25, 0x00,	/* 68 */
  0x26, 0x34, 0xff, 0xff,	/* 72 */
  0x64, 0xff, 0xff, 0x95,	/* 76 */
  0xff, 0x21, 0x61, 0x32,	/* 80 */
  0x23, 0x72, 0x35, 0x26,	/* 84 */
  0x95, 0x47, 0xff, 0x95,	/* 88 */
  0xff, 0x03, 0xff, 0xff,	/* 92 */
  0x40, 0x70, 0x82, 0x71,	/* 96 */
  0x62, 0x83, 0x73, 0x64,	/* 100 */
  0x86, 0xff, 0x74, 0x87,	/* 104 */
  0x67, 0xff, 0xff, 0xff,	/* 108 */
  0x94, 0xff, 0xff, 0xff,	/* 112 */
  0xff, 0xff, 0xff, 0xff,	/* 116 */
  0x15, 0x05, 0x57, 0x27,	/* 120 */
  0x77, 0xff, 0xff, 0xff	/* 124 */
};

static unsigned char PETshiftBusiness[KEYMAP_ENTRIES];
static unsigned char PETnormB_sflags[KEYMAP_ENTRIES/8];
static unsigned char PETshiftB_sflags[KEYMAP_ENTRIES/8];
static const char IBarIconName[] = "!vicepet";
static const char PETnormfile[] = "Vice:PET.RObusi/vkm";

static keymap_t PETkeysBusiness = {
  PETnormfile,
  PETnormBusiness,
  PETshiftBusiness,
  PETnormB_sflags,
  PETshiftB_sflags
};


static unsigned char PETnormGraphic[KEYMAP_ENTRIES] = {
  0x80, 0x94, 0xff, 0x80,	/* 0 */
  0x94, 0xff, 0x85, 0x94,	/* 4 */
  0xff, 0xff, 0xff, 0xff,	/* 8 */
  0xff, 0xff, 0xff, 0xff,	/* 12 */
  0x20, 0x67, 0x46, 0x56,	/* 16 */
  0xff, 0x36, 0xff, 0x87,	/* 20 */
  0x47, 0x07, 0x47, 0x26,	/* 24 */
  0xff, 0xff, 0xff, 0xff,	/* 28 */
  0xff, 0x30, 0x21, 0x22,	/* 32 */
  0x26, 0x33, 0x27, 0x86,	/* 36 */
  0x87, 0x16, 0x36, 0x27,	/* 40 */
  0x94, 0x81, 0x13, 0x17,	/* 44 */
  0x66, 0x76, 0x41, 0x31,	/* 48 */
  0x47, 0x23, 0x24, 0x34,	/* 52 */
  0x04, 0x16, 0x77, 0x87,	/* 56 */
  0x65, 0x17, 0x06, 0xff,	/* 60 */
  0xff, 0x40, 0x70, 0x51,	/* 64 */
  0x32, 0x43, 0x53, 0x76,	/* 68 */
  0x54, 0x65, 0xff, 0xff,	/* 72 */
  0xff, 0xff, 0xff, 0x64,	/* 76 */
  0xff, 0x50, 0x61, 0x42,	/* 80 */
  0x52, 0x72, 0x44, 0x54,	/* 84 */
  0x14, 0x17, 0xff, 0xff,	/* 88 */
  0xff, 0x97, 0xff, 0xff,	/* 92 */
  0x90, 0x60, 0x92, 0x71,	/* 96 */
  0x62, 0x63, 0x73, 0x96,	/* 100 */
  0x37, 0xff, 0xff, 0xff,	/* 104 */
  0xff, 0xff, 0xff, 0xff,	/* 108 */
  0x94, 0xff, 0xff, 0xff,	/* 112 */
  0xff, 0xff, 0xff, 0xff,	/* 116 */
  0x13, 0x07, 0xff, 0xff,	/* 120 */
  0xff, 0xff, 0xff, 0xff	/* 124 */
};

static unsigned char PETshiftGraphic[KEYMAP_ENTRIES];
static unsigned char PETnormG_sflags[KEYMAP_ENTRIES/8];
static unsigned char PETshiftG_sflags[KEYMAP_ENTRIES/8];
static const char PETgrphfile[] = "Vice:PET.ROgrph/vkm";

static keymap_t PETkeysGraphic = {
  PETgrphfile,
  PETnormGraphic,
  PETshiftGraphic,
  PETnormG_sflags,
  PETshiftG_sflags
};

static const conf_iconid_t conf_grey_xpet[] = {
  ICON_LIST_CART64
  ICON_LIST_VIC
  ICON_LIST_SYS64
  ICON_LIST_SYSTEM
  ICON_LIST_DEVICES
  ICON_LIST_SID
  ICON_LIST_TRUE
  {0xff, 0xff}
};


static const char Rsrc_CrtcCache[] = "CrtcVideoCache";

static struct MenuVideoCache {
  RO_MenuHead head;
  RO_MenuItem item[1];
} MenuVideoCache = {
  MENU_HEADER("\\MenVCaT", 200),
  {
    MENU_ITEM_LAST("\\MenVCaCrt")
  }
};

static struct MenuDisplayVideoCache {
  disp_desc_t dd;
  const char *values[1];
} MenuDisplayVideoCache = {
  {NULL, {CONF_WIN_VIDEO, 0},
    (RO_MenuHead*)&MenuVideoCache, 1, DISP_DESC_BITFIELD, 0},
  {Rsrc_CrtcCache}
};


/* PET keyboard names */
static char PETkeyBusinessName[] = "Business";
static char PETkeyGraphicName[] = "Graphic";

char *pet_get_keyboard_name(void)
{
  resource_value_t val;
  int idx;

  if (resources_get_value("KeymapIndex", (void *)&val) != 0) val = 0;
  idx = (int)val;
  if ((idx & 2) == 0) return PETkeyBusinessName;
  return PETkeyGraphicName;
}

int petui_init(void)
{
  PetModelName = "8032";

  return ui_init_named_app("VicePET", IBarIconName);
}

void petui_shutdown(void)
{
}

int pet_kbd_init(void)
{
  kbd_default_keymap(&PETkeysBusiness);
  kbd_default_keymap(&PETkeysGraphic);
  kbd_init_keymap(4);
  kbd_add_keymap(&PETkeysBusiness, 0); kbd_add_keymap(&PETkeysBusiness, 1);
  kbd_add_keymap(&PETkeysGraphic, 2); kbd_add_keymap(&PETkeysGraphic, 3);
  kbd_load_keymap(NULL, 0);
  kbd_load_keymap(NULL, 2);
  return kbd_init();
}

void ui_grey_out_machine_icons(void)
{
  ui_set_icons_grey(NULL, conf_grey_xpet, 0);
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



/* Dummies */
char *cbm2_get_keyboard_name(void)
{
  return NULL;
}

int cbm2_set_model(const char *name, void *extra)
{
  return 0;
}
