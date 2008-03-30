/*
 * uiramcart.c
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

#include "uiramcart.h"

static const char *ui_ramcart_on_off[] = {
  "off",
  "on",
  NULL
};

static const int ui_ramcart_on_off_values[] = {
  0,
  1,
  -1
};

static const char *ui_ramcart_read_only[] = {
  "read/write",
  "read-only",
  NULL
};

static const int ui_ramcart_read_only_values[] = {
  0,
  1,
  -1
};

static const char *ui_ramcart_size[] = {
  "64K",
  "128K",
  NULL
};

static const int ui_ramcart_size_values[] = {
  64,
  128,
  -1
};

static ui_to_from_t ui_to_from[] = {
  { NULL, MUI_TYPE_CYCLE, "RAMCART", ui_ramcart_on_off, ui_ramcart_on_off_values },
  { NULL, MUI_TYPE_CYCLE, "RAMCART_RO", ui_ramcart_read_only, ui_ramcart_read_only_values },
  { NULL, MUI_TYPE_CYCLE, "RAMCARTsize", ui_ramcart_size, ui_ramcart_size_values },
  UI_END /* mandatory */
};

static APTR build_gui(void)
{
  return GroupObject,
    CYCLE(ui_to_from[0].object, "RAMCART Enabled", ui_ramcart_on_off)
    CYCLE(ui_to_from[1].object, "RAMCART Read/Write", ui_ramcart_read_only)
    CYCLE(ui_to_from[2].object, "RAMCART Size", ui_ramcart_size)
  End;
}

void ui_ramcart_settings_dialog(void)
{
  mui_show_dialog(build_gui(), "RAMCART Settings", ui_to_from);
}
