/*
 * uic64_256k.c
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

#include "uic64_256k.h"
#include "intl.h"
#include "translate.h"


static int ui_c64_256k_enable_translate[] = {
  IDMS_DISABLED,
  IDS_ENABLED,
  0
};

static char *ui_c64_256k_enable[countof(ui_c64_256k_enable_translate)];


static const int ui_c64_256k_enable_values[] = {
  0,
  1,
  -1
};

static char *ui_c64_256k_base[] = {
  "$DE00",
  "$DE80",
  "$DF00",
  "$DF80",
  NULL
};

static const int ui_c64_256k_base_values[] = {
  0xde00,
  0xde80,
  0xdf00,
  0xdf80,
  -1
};

static ui_to_from_t ui_to_from[] = {
  { NULL, MUI_TYPE_CYCLE, "C64_256K", ui_c64_256k_enable, ui_c64_256k_enable_values },
  { NULL, MUI_TYPE_CYCLE, "C64_256Kbase", ui_c64_256k_base, ui_c64_256k_base_values },
  UI_END /* mandatory */
};

static APTR build_gui(void)
{
  return GroupObject,
    CYCLE(ui_to_from[0].object, translate_text(IDS_256K_ENABLED), ui_c64_256k_enable)
    CYCLE(ui_to_from[1].object, translate_text(IDS_256K_BASE), ui_c64_256k_base)
  End;
}

void ui_c64_256k_settings_dialog(void)
{
  intl_convert_mui_table(ui_c64_256k_enable_translate, ui_c64_256k_enable);
  mui_show_dialog(build_gui(), translate_text(IDS_256K_SETTINGS), ui_to_from);
}
