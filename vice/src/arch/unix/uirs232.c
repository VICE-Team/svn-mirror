/*
 * uirs232.c - Implementation of RS232 UI settings.
 *
 * Written by
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
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

#include "resources.h"
#include "uimenu.h"
#include "utils.h"
#include "vsync.h"


UI_MENU_DEFINE_RADIO(RsUserDev)

ui_menu_entry_t rsuser_device_submenu[] = {
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

UI_MENU_DEFINE_RADIO(Acia1Dev)

ui_menu_entry_t acia1_device_submenu[] = {
    { N_("*Serial 1"),
      (ui_callback_t)radio_Acia1Dev, (ui_callback_data_t)0, NULL },
    { N_("*Serial 2"),
      (ui_callback_t)radio_Acia1Dev, (ui_callback_data_t)1, NULL },
    { N_("*Dump to file"),
      (ui_callback_t)radio_Acia1Dev, (ui_callback_data_t)2, NULL },
    { N_("*Exec process"),
      (ui_callback_t)radio_Acia1Dev, (ui_callback_data_t)3, NULL },
    { NULL }
};

UI_MENU_DEFINE_RADIO(Acia1Irq)

ui_menu_entry_t acia1_irq_submenu[] = {
    { N_("*No IRQ/NMI"),
      (ui_callback_t)radio_Acia1Irq, (ui_callback_data_t)0, NULL },
    { N_("*IRQ"),
      (ui_callback_t)radio_Acia1Irq, (ui_callback_data_t)1, NULL },
    { N_("*NMI"),
      (ui_callback_t)radio_Acia1Irq, (ui_callback_data_t)2, NULL },
    { NULL }
};

UI_MENU_DEFINE_RADIO(RsDevice1Baud)

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
                              NULL, False, "/dev", "ttyS*", &button, False,
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
        free(filename);
}

UI_CALLBACK(set_rs232_exec_file)
{
    char *resname = (char *)UI_MENU_CB_PARAM;
    char *title;
    ui_button_t button;
    char *value;
    char *new_value;
    int len;

    vsync_suspend_speed_eval();
    title = stralloc(_("Command to execute for RS232 (preceed with '|')"));

    resources_get_value(resname, (resource_value_t *)&value);
    len = strlen(value) * 2;
    if (len < 255)
        len = 255;

    new_value = xmalloc(len + 1);
    strcpy(new_value, value);

    button = ui_input_string(title, _("Command:"), new_value, len);
    free(title);

    if (button == UI_BUTTON_OK)
        resources_set_value(resname, (resource_value_t)new_value);

    free(new_value);
}

UI_CALLBACK(set_rs232_dump_file)
{
    char *resname = (char *)UI_MENU_CB_PARAM;
    char *title;
    ui_button_t button;
    char *value;
    char *new_value;
    int len;

    vsync_suspend_speed_eval();
    title = stralloc(_("File to dump RS232 to"));

    resources_get_value(resname, (resource_value_t *)&value);
    len = strlen(value) * 2;
    if (len < 255)
        len = 255;

    new_value = xmalloc(len + 1);
    strcpy(new_value, value);

    button = ui_input_string(title, _("Command:"), new_value, len);
    free(title);

    if (button == UI_BUTTON_OK)
        resources_set_value(resname, (resource_value_t)new_value);

    free(new_value);
}


UI_MENU_DEFINE_TOGGLE(AciaDE)
UI_MENU_DEFINE_RADIO(RsUser)

ui_menu_entry_t rs232_submenu[] = {
    { N_("*ACIA $DExx RS232 interface emulation"),
      (ui_callback_t) toggle_AciaDE, NULL, NULL },
    { N_("ACIA $DExx device"),
      NULL, NULL, acia1_device_submenu },
    { N_("ACIA $DExx Interrupt"),
      NULL, NULL, acia1_irq_submenu },
    { "--" },
    { N_("*No Userport RS232 emulation"),
      (ui_callback_t)radio_RsUser, (ui_callback_data_t)0, NULL },
    { N_("*Userport 300 baud RS232 emulation"),
      (ui_callback_t)radio_RsUser, (ui_callback_data_t)300, NULL },
    { N_("*Userport 1200 baud RS232 emulation"),
      (ui_callback_t)radio_RsUser, (ui_callback_data_t)1200, NULL },
    { N_("*CIA 9600 baud RS232 emulation"),
      (ui_callback_t)radio_RsUser, (ui_callback_data_t)9600, NULL },
    { N_("Userport RS232 device"),
      NULL, NULL, rsuser_device_submenu },
    { "--" },
    { N_("Serial 1 device..."), (ui_callback_t)set_rs232_device_file,
      (ui_callback_data_t)"RsDevice1", NULL },
    { N_("Serial 1 baudrate"),
      NULL, NULL, ser1_baud_submenu },
    { "--" },
    { "Serial 2 device...", (ui_callback_t)set_rs232_device_file,
      (ui_callback_data_t)"RsDevice2", NULL },
    { N_("Serial 2 baudrate"),
      NULL, NULL, ser2_baud_submenu },
    { "--" },
    { N_("Dump filename..."), (ui_callback_t)set_rs232_dump_file,
      (ui_callback_data_t)"RsDevice3", NULL },
    { "--" },
    { N_("Program name to exec..."), (ui_callback_t)set_rs232_exec_file,
      (ui_callback_data_t)"RsDevice4", NULL },
    { NULL }
};

