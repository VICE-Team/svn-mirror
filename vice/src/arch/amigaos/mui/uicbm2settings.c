/*
 * uicbm2settings.c
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

#include "uicbm2settings.h"

static const char *ui_cbm2_model[] = {
  "7x0 (50 Hz)",
  "6x0 (60 Hz)",
  "6x0 (50 Hz)",
  NULL
};

static const int ui_cbm2_model_values[] = {
  0,
  1,
  2,
  -1
};

static const char *ui_cbm2_memsize[] = {
  "128K",
  "256K",
  "512K",
  "1024K",
  NULL
};

static const int ui_cbm2_memsize_values[] = {
  128,
  256,
  512,
  1024,
  -1
};

static const char *ui_cbm2mem_block08_on_off[] = {
  "disabled",
  "enabled",
  NULL
};

static const int ui_cbm2mem_block08_on_off_values[] = {
  0,
  1,
  -1
};

static const char *ui_cbm2mem_block1_on_off[] = {
  "disabled",
  "enabled",
  NULL
};

static const int ui_cbm2mem_block1_on_off_values[] = {
  0,
  1,
  -1
};
static const char *ui_cbm2mem_block2_on_off[] = {
  "disabled",
  "enabled",
  NULL
};

static const int ui_cbm2mem_block2_on_off_values[] = {
  0,
  1,
  -1
};

static const char *ui_cbm2mem_block4_on_off[] = {
  "disabled",
  "enabled",
  NULL
};

static const int ui_cbm2mem_block4_on_off_values[] = {
  0,
  1,
  -1
};

static const char *ui_cbm2mem_block6_on_off[] = {
  "disabled",
  "enabled",
  NULL
};

static const int ui_cbm2mem_block6_on_off_values[] = {
  0,
  1,
  -1
};

static const char *ui_cbm2mem_blockC_on_off[] = {
  "disabled",
  "enabled",
  NULL
};

static const int ui_cbm2mem_blockC_on_off_values[] = {
  0,
  1,
  -1
};

static ui_to_from_t ui_to_from[] = {
  { NULL, MUI_TYPE_CYCLE, "ModelLine", ui_cbm2_model, ui_cbm2_model_values },
  { NULL, MUI_TYPE_CYCLE, "RamSize", ui_cbm2_memsize, ui_cbm2_memsize_values },
  { NULL, MUI_TYPE_CYCLE, "Ram08", ui_cbm2mem_block08_on_off, ui_cbm2mem_block08_on_off_values },
  { NULL, MUI_TYPE_CYCLE, "Ram1", ui_cbm2mem_block1_on_off, ui_cbm2mem_block1_on_off_values },
  { NULL, MUI_TYPE_CYCLE, "Ram2", ui_cbm2mem_block2_on_off, ui_cbm2mem_block2_on_off_values },
  { NULL, MUI_TYPE_CYCLE, "Ram4", ui_cbm2mem_block4_on_off, ui_cbm2mem_block4_on_off_values },
  { NULL, MUI_TYPE_CYCLE, "Ram6", ui_cbm2mem_block6_on_off, ui_cbm2mem_block6_on_off_values },
  { NULL, MUI_TYPE_CYCLE, "RamC", ui_cbm2mem_blockC_on_off, ui_cbm2mem_blockC_on_off_values },
  UI_END /* mandatory */
};

static APTR build_gui(void)
{
  return GroupObject,
    CYCLE(ui_to_from[0].object, "model line", ui_cbm2_model)
    CYCLE(ui_to_from[1].object, "RAM Size", ui_cbm2_memsize)
    CYCLE(ui_to_from[2].object, "RAM Block $0800-$0FFF", ui_cbm2mem_block08_on_off)
    CYCLE(ui_to_from[3].object, "RAM Block $1000-$1FFF", ui_cbm2mem_block1_on_off)
    CYCLE(ui_to_from[4].object, "RAM Block $2000-$3FFF", ui_cbm2mem_block2_on_off)
    CYCLE(ui_to_from[5].object, "RAM Block $4000-$5FFF", ui_cbm2mem_block4_on_off)
    CYCLE(ui_to_from[6].object, "RAM Block $6000-$7FFF", ui_cbm2mem_block6_on_off)
    CYCLE(ui_to_from[7].object, "RAM Block $C000-$CFFF", ui_cbm2mem_blockC_on_off)
  End;
}

void ui_cbm2_settings_dialog(void)
{
  mui_show_dialog(build_gui(), "CBM2 Settings", ui_to_from);
}
