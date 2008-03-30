/*
 * uivicii.c
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
#ifdef AMIGA_M68K
#define _INLINE_MUIMASTER_H
#endif
#include "mui.h"

#include "uivicii.h"

static const char *ui_vicii_spr_spr_collisions[] = {
  "disabled",
  "enabled",
  NULL
};

static const int ui_vicii_spr_spr_collisions_values[] = {
  0,
  1,
  -1
};

static const char *ui_vicii_spr_bg_collisions[] = {
  "disabled",
  "enabled",
  NULL
};

static const int ui_vicii_spr_bg_collisions_values[] = {
  0,
  1,
  -1
};

static const char *ui_vicii_new_luminances[] = {
  "disabled",
  "enabled",
  NULL
};

static const int ui_vicii_new_luminances_values[] = {
  0,
  1,
  -1
};

static ui_to_from_t ui_to_from[] = {
  { NULL, MUI_TYPE_CYCLE, "VICIICheckSsColl", ui_vicii_spr_spr_collisions, ui_vicii_spr_spr_collisions_values },
  { NULL, MUI_TYPE_CYCLE, "VICIICheckSbColl", ui_vicii_spr_bg_collisions, ui_vicii_spr_bg_collisions_values },
  { NULL, MUI_TYPE_CYCLE, "VICIINewLuminances", ui_vicii_new_luminances, ui_vicii_new_luminances_values },
  UI_END /* mandatory */
};

static APTR build_gui(void)
{
  return GroupObject,
    CYCLE(ui_to_from[0].object, "Sprite Sprite Collisions", ui_vicii_spr_spr_collisions)
    CYCLE(ui_to_from[1].object, "Sprite Background Collisions", ui_vicii_spr_bg_collisions)
    CYCLE(ui_to_from[2].object, "New Luminances", ui_vicii_new_luminances)
  End;
}

void ui_vicii_settings_dialog(void)
{
  mui_show_dialog(build_gui(), "VICII Settings", ui_to_from);
}
