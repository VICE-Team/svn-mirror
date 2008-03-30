/*
 * uivic20mem.c
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

#include "uivic20mem.h"

static const char *ui_vic20mem_block0_on_off[] = {
  "disabled",
  "enabled",
  NULL
};

static const int ui_vic20mem_block0_on_off_values[] = {
  0,
  1,
  -1
};

static const char *ui_vic20mem_block1_on_off[] = {
  "disabled",
  "enabled",
  NULL
};

static const int ui_vic20mem_block1_on_off_values[] = {
  0,
  1,
  -1
};

static const char *ui_vic20mem_block2_on_off[] = {
  "disabled",
  "enabled",
  NULL
};

static const int ui_vic20mem_block2_on_off_values[] = {
  0,
  1,
  -1
};

static const char *ui_vic20mem_block3_on_off[] = {
  "disabled",
  "enabled",
  NULL
};

static const int ui_vic20mem_block3_on_off_values[] = {
  0,
  1,
  -1
};

static const char *ui_vic20mem_block5_on_off[] = {
  "disabled",
  "enabled",
  NULL
};

static const int ui_vic20mem_block5_on_off_values[] = {
  0,
  1,
  -1
};

static ui_to_from_t ui_to_from[] = {
  { NULL, MUI_TYPE_CYCLE, "RAMBlock0", ui_vic20mem_block0_on_off, ui_vic20mem_block0_on_off_values },
  { NULL, MUI_TYPE_CYCLE, "RAMBlock1", ui_vic20mem_block1_on_off, ui_vic20mem_block1_on_off_values },
  { NULL, MUI_TYPE_CYCLE, "RAMBlock2", ui_vic20mem_block2_on_off, ui_vic20mem_block2_on_off_values },
  { NULL, MUI_TYPE_CYCLE, "RAMBlock3", ui_vic20mem_block3_on_off, ui_vic20mem_block3_on_off_values },
  { NULL, MUI_TYPE_CYCLE, "RAMBlock5", ui_vic20mem_block5_on_off, ui_vic20mem_block5_on_off_values },
  UI_END /* mandatory */
};

static APTR build_gui(void)
{
  return GroupObject,
    CYCLE(ui_to_from[0].object, "RAM Block $0400-$0FFF", ui_vic20mem_block0_on_off)
    CYCLE(ui_to_from[1].object, "RAM Block $2000-$3FFF", ui_vic20mem_block1_on_off)
    CYCLE(ui_to_from[2].object, "RAM Block $4000-$5FFF", ui_vic20mem_block2_on_off)
    CYCLE(ui_to_from[3].object, "RAM Block $6000-$7FFF", ui_vic20mem_block3_on_off)
    CYCLE(ui_to_from[4].object, "RAM Block $A000-$BFFF", ui_vic20mem_block5_on_off)
  End;
}

void ui_vic_settings_dialog(void)
{
  mui_show_dialog(build_gui(), "VIC Settings", ui_to_from);
}
