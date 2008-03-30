/*
 * uirs232user.c
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

#include "uirs232user.h"

static const char *ui_rs232user_enable[] = {
  "Disabled",
  "Enabled",
  NULL
};

static const int ui_rs232user_enable_values[] = {
  0,
  1,
  -1
};

static const char *ui_rs232user_device[] = {
  "RS232 Device 1",
  "RS232 Device 2",
  "RS232 Device 3",
  "RS232 Device 4",
  NULL
};

static const int ui_rs232user_device_values[] = {
  0,
  1,
  2,
  3,
  -1
};

static const char *ui_rs232user_baud[] = {
  "300",
  "600",
  "1200",
  "2400",
  "4800",
  "9600",
  NULL
};

static const int ui_rs232user_baud_values[] = {
  300,
  600,
  1200,
  2400,
  4800,
  9600,
  -1
};

static ui_to_from_t ui_to_from[] = {
  { NULL, MUI_TYPE_CYCLE, "RsUserEnable", ui_rs232user_enable, ui_rs232user_enable_values },
  { NULL, MUI_TYPE_CYCLE, "RsUserDev", ui_rs232user_device, ui_rs232user_device_values },
  { NULL, MUI_TYPE_CYCLE, "RsUserBaud", ui_rs232user_baud, ui_rs232user_baud_values },
  UI_END /* mandatory */
};

static APTR build_gui(void)
{
  return GroupObject,
    CYCLE(ui_to_from[0].object, "Userport RS232", ui_rs232user_enable)
    CYCLE(ui_to_from[1].object, "Userport Device", ui_rs232user_device)
    CYCLE(ui_to_from[2].object, "Userport Baud-rate", ui_rs232user_baud)
  End;
}

void ui_rs232user_settings_dialog(void)
{
  mui_show_dialog(build_gui(), "RS232 Userport Settings", ui_to_from);
}
