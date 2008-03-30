/*
 * uidatasette.c
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

#include "uidatasette.h"

static const char *ui_datasette_reset_with_cpu[] = {
  "disabled",
  "enabled",
  NULL
};

static const int ui_datasette_reset_with_cpu_values[] = {
  0,
  1,
  -1
};

static const char *ui_datasette_additional_delay[] = {
  "0 cyles",
  "1 cycle",
  "2 cycles",
  "3 cycles",
  "4 cycles",
  "5 cycles",
  "6 cycles",
  "7 cycles",
  NULL
};

static const int ui_datasette_additional_delay_values[] = {
  0,
  1,
  2,
  3,
  4,
  5,
  6,
  7,
  -1
};

static const char *ui_datasette_delay_at_zero[] = {
  "1000 cycles",
  "2000 cycles",
  "5000 cycles",
  "10000 cycles",
  "20000 cycles",
  "50000 cycles",
  "100000 cycles",
  NULL
};

static const int ui_datasette_delay_at_zero_values[] = {
  1000,
  2000,
  5000,
  10000,
  20000,
  50000,
  100000,
  -1
};

static ui_to_from_t ui_to_from[] = {
  { NULL, MUI_TYPE_CYCLE, "DatasetteResetWithCPU", ui_datasette_reset_with_cpu, ui_datasette_reset_with_cpu_values },
  { NULL, MUI_TYPE_CYCLE, "DatasetteSpeedTuning", ui_datasette_additional_delay, ui_datasette_additional_delay_values },
  { NULL, MUI_TYPE_CYCLE, "DatasetteZeroGapDelay", ui_datasette_delay_at_zero, ui_datasette_delay_at_zero_values },
  UI_END /* mandatory */
};

static APTR build_gui(void)
{
  return GroupObject,
    CYCLE(ui_to_from[0].object, "Reset Datasette with CPU", ui_datasette_reset_with_cpu)
    CYCLE(ui_to_from[1].object, "Additional Delay", ui_datasette_additional_delay)
    CYCLE(ui_to_from[2].object, "Delay at Zero Values", ui_datasette_delay_at_zero)
  End;
}

void ui_datasette_settings_dialog(void)
{
  mui_show_dialog(build_gui(), "DATASETTE Settings", ui_to_from);
}
