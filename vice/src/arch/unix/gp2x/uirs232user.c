/*
 * uirs232user.c
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
 *  André Fachat <fachat@physik.tu-chemnitz.de>
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

#include <stdio.h>

#include "uimenu.h"
#include "uirs232user.h"


UI_MENU_DEFINE_RADIO(RsUserDev)

ui_menu_entry_t rs232user_device_submenu[] = {
    { N_("*Serial 1"),
      (ui_callback_t)radio_RsUserDev, (ui_callback_data_t)0, NULL },
    { N_("*Serial 2"),
      (ui_callback_t)radio_RsUserDev, (ui_callback_data_t)1, NULL },
    { N_("*Dump to file"),
      (ui_callback_t)radio_RsUserDev, (ui_callback_data_t)2, NULL },
    { N_("*Exec process"),
      (ui_callback_t)radio_RsUserDev, (ui_callback_data_t)3, NULL },
    { NULL }
};

UI_MENU_DEFINE_RADIO(RsUserBaud)

ui_menu_entry_t rs232user_baudrate_submenu[] = {
    { N_("*300 baud"),
      (ui_callback_t)radio_RsUserBaud, (ui_callback_data_t)300, NULL },
    { N_("*600 baud"),
      (ui_callback_t)radio_RsUserBaud, (ui_callback_data_t)600, NULL },
    { N_("*1200 baud"),
      (ui_callback_t)radio_RsUserBaud, (ui_callback_data_t)1200, NULL },
    { N_("*2400 baud"),
      (ui_callback_t)radio_RsUserBaud, (ui_callback_data_t)2400, NULL },
    { N_("*4800 baud"),
      (ui_callback_t)radio_RsUserBaud, (ui_callback_data_t)4800, NULL },
    { N_("*9600 baud"),
      (ui_callback_t)radio_RsUserBaud, (ui_callback_data_t)9600, NULL },
    { NULL }
};

