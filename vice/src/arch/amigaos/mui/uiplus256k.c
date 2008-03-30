/*
 * uiplus256k.c
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

#include "uiplus256k.h"
#include "intl.h"
#include "translate.h"

static int ui_plus256k_enable_translate[] = {
  IDS_DISABLED,
  IDS_ENABLED,
  0
};

static char *ui_plus256k_enable[countof(ui_plus256k_enable_translate)];

static const int ui_plus256k_enable_values[] = {
  0,
  1,
  -1
};

static ui_to_from_t ui_to_from[] = {
  { NULL, MUI_TYPE_CYCLE, "PLUS256K", ui_plus256k_enable, ui_plus256k_enable_values },
  UI_END /* mandatory */
};

static APTR build_gui(void)
{
  return GroupObject,
    CYCLE(ui_to_from[0].object, "PLUS256K", ui_plus256k_enable)
  End;
}

void ui_plus256k_settings_dialog(void)
{
  intl_convert_mui_table(ui_plus256k_enable_translate, ui_plus256k_enable);
  mui_show_dialog(build_gui(), translate_text(IDS_PLUS256K_SETTINGS), ui_to_from);
}
