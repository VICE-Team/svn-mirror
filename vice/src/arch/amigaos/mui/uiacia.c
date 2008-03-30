/*
 * uiacia.c
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

#include "uiacia.h"

static const char *ui_acia_enable[] = {
  "Disabled",
  "Enabled",
  NULL
};

static const int ui_acia_enable_values[] = {
  0,
  1,
  -1
};

static const char *ui_acia_device[] = {
  "RS232 Device 1",
  "RS232 Device 2",
  "RS232 Device 3",
  "RS232 Device 4",
  NULL
};

static const int ui_acia_device_values[] = {
  0,
  1,
  2,
  3,
  -1
};

static const char *ui_acia_interrupt[] = {
  "None",
  "IRQ",
  "NMI",
  NULL
};

static const int ui_acia_interrupt_values[] = {
  0,
  1,
  2,
  -1
};

static const char *ui_acia_mode[] = {
  "Normal",
  "Swiftlink",
  "Turbo232",
  NULL
};

static const int ui_acia_mode_values[] = {
  0,
  1,
  2,
  -1
};

static ui_to_from_t ui_to_from64128[] = {
  { NULL, MUI_TYPE_CYCLE, "Acia1Enable", ui_acia_enable, ui_acia_enable_values },
  { NULL, MUI_TYPE_CYCLE, "Acia1Dev", ui_acia_device, ui_acia_device_values },
  { NULL, MUI_TYPE_CYCLE, "Acia1Irq", ui_acia_interrupt, ui_acia_interrupt_values },
  { NULL, MUI_TYPE_CYCLE, "Acia1Mode", ui_acia_mode, ui_acia_mode_values },
  UI_END /* mandatory */
};

static ui_to_from_t ui_to_from[] = {
  { NULL, MUI_TYPE_CYCLE, "Acia1Dev", ui_acia_device, ui_acia_device_values },
  UI_END /* mandatory */
};

static APTR build_gui64128(void)
{
  return GroupObject,
    CYCLE(ui_to_from64128[0].object, "ACIA", ui_acia_enable)
    CYCLE(ui_to_from64128[1].object, "ACIA Device", ui_acia_device)
    CYCLE(ui_to_from64128[2].object, "ACIA Interrupt", ui_acia_interrupt)
    CYCLE(ui_to_from64128[3].object, "ACIA Mode", ui_acia_mode)
  End;
}

static APTR build_gui(void)
{
  return GroupObject,
    CYCLE(ui_to_from[1].object, "ACIA Device", ui_acia_device)
  End;
}

void ui_acia_settings_dialog(void)
{
  mui_show_dialog(build_gui(), "ACIA Settings", ui_to_from);
}

void ui_acia64128_settings_dialog(void)
{
  mui_show_dialog(build_gui64128(), "ACIA Settings", ui_to_from64128);
}
