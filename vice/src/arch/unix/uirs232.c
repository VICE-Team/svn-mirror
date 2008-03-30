/*
 * uirs232.c - Implementation of RS232 UI settings.
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
#include <stdlib.h>
#include <string.h>

#include "lib.h"
#include "resources.h"
#include "uilib.h"
#include "uimenu.h"
#include "vsync.h"


UI_MENU_DEFINE_RADIO(RsDevice1Baud)

ui_menu_entry_t ser1_c64c128_baud_submenu[] = {
  { "*300",
      (ui_callback_t)radio_RsDevice1Baud, (ui_callback_data_t)300, NULL },
  { "*1200",
      (ui_callback_t)radio_RsDevice1Baud, (ui_callback_data_t)1200, NULL },
  { "*2400",
      (ui_callback_t)radio_RsDevice1Baud, (ui_callback_data_t)2400, NULL },
  { "*9600",
      (ui_callback_t)radio_RsDevice1Baud, (ui_callback_data_t)9600, NULL },
  { "*19200",
      (ui_callback_t)radio_RsDevice1Baud, (ui_callback_data_t)19200, NULL },
  { N_("*38400 (Swiftlink/Turbo232 only)"),
      (ui_callback_t)radio_RsDevice1Baud, (ui_callback_data_t)38400, NULL },
  { N_("*57600 (Turbo232 only)"),
      (ui_callback_t)radio_RsDevice1Baud, (ui_callback_data_t)57600, NULL },
  { N_("*115200 (Turbo232 only)"),
      (ui_callback_t)radio_RsDevice1Baud, (ui_callback_data_t)115200, NULL },
  { NULL }
};

ui_menu_entry_t ser1_baud_submenu[] = {
  { "*300",
      (ui_callback_t)radio_RsDevice1Baud, (ui_callback_data_t)300, NULL },
  { "*1200",
      (ui_callback_t)radio_RsDevice1Baud, (ui_callback_data_t)1200, NULL },
  { "*2400",
      (ui_callback_t)radio_RsDevice1Baud, (ui_callback_data_t)2400, NULL },
  { "*9600",
      (ui_callback_t)radio_RsDevice1Baud, (ui_callback_data_t)9600, NULL },
  { "*19200",
      (ui_callback_t)radio_RsDevice1Baud, (ui_callback_data_t)19200, NULL },
  { NULL }
};

UI_MENU_DEFINE_RADIO(RsDevice2Baud)

ui_menu_entry_t ser2_c64c128_baud_submenu[] = {
  { "*300",
      (ui_callback_t)radio_RsDevice2Baud, (ui_callback_data_t)300, NULL },
  { "*1200",
      (ui_callback_t)radio_RsDevice2Baud, (ui_callback_data_t)1200, NULL },
  { "*2400",
      (ui_callback_t)radio_RsDevice2Baud, (ui_callback_data_t)2400, NULL },
  { "*9600",
      (ui_callback_t)radio_RsDevice2Baud, (ui_callback_data_t)9600, NULL },
  { "*19200",
      (ui_callback_t)radio_RsDevice2Baud, (ui_callback_data_t)19200, NULL },
  { N_("*38400 (Swiftlink/Turbo232 only)"),
      (ui_callback_t)radio_RsDevice2Baud, (ui_callback_data_t)38400, NULL },
  { N_("*57600 (Turbo232 only)"),
      (ui_callback_t)radio_RsDevice2Baud, (ui_callback_data_t)57600, NULL },
  { N_("*115200 (Turbo232 only)"),
      (ui_callback_t)radio_RsDevice2Baud, (ui_callback_data_t)115200, NULL },
  { NULL }
};

ui_menu_entry_t ser2_baud_submenu[] = {
  { "*300",
      (ui_callback_t)radio_RsDevice2Baud, (ui_callback_data_t)300, NULL },
  { "*1200",
      (ui_callback_t)radio_RsDevice2Baud, (ui_callback_data_t)1200, NULL },
  { "*2400",
      (ui_callback_t)radio_RsDevice2Baud, (ui_callback_data_t)2400, NULL },
  { "*9600",
      (ui_callback_t)radio_RsDevice2Baud, (ui_callback_data_t)9600, NULL },
  { "*19200",
      (ui_callback_t)radio_RsDevice2Baud, (ui_callback_data_t)19200, NULL },
  { NULL }
};

UI_CALLBACK(set_rs232_device_file)
{
    char *resource = (char *)UI_MENU_CB_PARAM;
    char *filename;
    ui_button_t button;

    vsync_suspend_speed_eval();

    filename = ui_select_file(_("Select RS232 device file"),
                              NULL, 0, False, "/dev", "ttyS*", &button, False,
                              NULL);
    switch (button) {
      case UI_BUTTON_OK:
        resources_set_value(resource, (resource_value_t)filename);
        break;
      default:
        /* Do nothing special.  */
        break;
    }
    if (filename != NULL)
        lib_free(filename);
}

UI_CALLBACK(set_rs232_exec_file)
{
    uilib_select_string((char *)UI_MENU_CB_PARAM,
                        _("Command to execute for RS232 (preceed with '|')"),
                        _("Command:"));
}

UI_CALLBACK(set_rs232_dump_file)
{
    uilib_select_string((char *)UI_MENU_CB_PARAM, _("File to dump RS232 to"),
                        _("Dump file:"));
}

