/*
 * uiplus60k.c
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

#include "uiplus60k.h"

static const char *ui_plus60k_on_off[] = {
  "off",
  "on",
  NULL
};

static const int ui_plus60k_on_off_values[] = {
  0,
  1,
  -1
};

static const char *ui_plus60k_base[] = {
  "$D040",
  "$D100",
  NULL
};

static const int ui_plus60k_base_values[] = {
  0xd040,
  0xd100,
  -1
};

static ui_to_from_t ui_to_from[] = {
  { NULL, MUI_TYPE_CYCLE, "PLUS60K", ui_plus60k_on_off, ui_plus60k_on_off_values },
  { NULL, MUI_TYPE_CYCLE, "PLUS60Kbase", ui_plus60k_base, ui_plus60k_base_values },
  UI_END /* mandatory */
};

static APTR build_gui(void)
{
  return GroupObject,
    CYCLE(ui_to_from[0].object, "PLUS60K Enabled", ui_plus60k_on_off)
    CYCLE(ui_to_from[1].object, "PLUS60K Base", ui_plus60k_base)
  End;
}

void ui_plus60k_settings_dialog(void)
{
  mui_show_dialog(build_gui(), "PLUS60K Settings", ui_to_from);
}
