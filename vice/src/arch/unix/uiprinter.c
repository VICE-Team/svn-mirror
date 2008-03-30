/*
 * uiprinter.c
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
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
#include <string.h>

#include "lib.h"
#include "printer.h"
#include "resources.h"
#include "uilib.h"
#include "uimenu.h"
#include "util.h"
#include "vsync.h"


UI_MENU_DEFINE_STRING_RADIO(Printer4Driver)

ui_menu_entry_t uiprinter_pr4_driver_submenu[] = {
    { "*ASCII", (ui_callback_t)radio_Printer4Driver,
      (ui_callback_data_t)"ascii", NULL },
    { "*MPS803", (ui_callback_t)radio_Printer4Driver,
      (ui_callback_data_t)"mps803", NULL },
    { "*NL10", (ui_callback_t)radio_Printer4Driver,
      (ui_callback_data_t)"nl10", NULL },
    { NULL }
};

UI_MENU_DEFINE_STRING_RADIO(Printer5Driver)

ui_menu_entry_t uiprinter_pr5_driver_submenu[] = {
    { "*ASCII", (ui_callback_t)radio_Printer5Driver,
      (ui_callback_data_t)"ascii", NULL },
    { "*MPS803", (ui_callback_t)radio_Printer5Driver,
      (ui_callback_data_t)"mps803", NULL },
    { "*NL10", (ui_callback_t)radio_Printer5Driver,
      (ui_callback_data_t)"nl10", NULL },
    { NULL }
};

UI_MENU_DEFINE_STRING_RADIO(PrinterUserportDriver)

ui_menu_entry_t uiprinter_pruser_driver_submenu[] = {
    { "*ASCII", (ui_callback_t)radio_PrinterUserportDriver,
      (ui_callback_data_t)"ascii", NULL },
    { "*MPS803", (ui_callback_t)radio_PrinterUserportDriver,
      (ui_callback_data_t)"mps803", NULL },
    { "*NL10", (ui_callback_t)radio_PrinterUserportDriver,
      (ui_callback_data_t)"nl10", NULL },
    { NULL }
};

UI_MENU_DEFINE_RADIO(PrinterUserportTextDevice)

ui_menu_entry_t uiprinter_pruser_device_submenu[] = {
    { N_("*Printer 1 (file dump)"),
      (ui_callback_t)radio_PrinterUserportTextDevice,
      (ui_callback_data_t)0, NULL },
    { N_("*Printer 2 (exec)"),
      (ui_callback_t)radio_PrinterUserportTextDevice,
      (ui_callback_data_t)1, NULL },
    { N_("*Printer 3 (exec)"),
      (ui_callback_t)radio_PrinterUserportTextDevice,
      (ui_callback_data_t)2, NULL },
    { NULL }
};

UI_MENU_DEFINE_RADIO(Printer4TextDevice)

ui_menu_entry_t uiprinter_pr4_device_submenu[] = {
    { N_("*Printer 1 (file dump)"),
      (ui_callback_t)radio_Printer4TextDevice, (ui_callback_data_t)0, NULL },
    { N_("*Printer 2 (exec)"),
      (ui_callback_t)radio_Printer4TextDevice, (ui_callback_data_t)1, NULL },
    { N_("*Printer 3 (exec)"),
      (ui_callback_t)radio_Printer4TextDevice, (ui_callback_data_t)2, NULL },
    { NULL }
};

UI_MENU_DEFINE_RADIO(Printer5TextDevice)

ui_menu_entry_t uiprinter_pr5_device_submenu[] = {
    { N_("*Printer 1 (file dump)"),
      (ui_callback_t)radio_Printer5TextDevice, (ui_callback_data_t)0, NULL },
    { N_("*Printer 2 (exec)"),
      (ui_callback_t)radio_Printer5TextDevice, (ui_callback_data_t)1, NULL },
    { N_("*Printer 3 (exec)"),
      (ui_callback_t)radio_Printer5TextDevice, (ui_callback_data_t)2, NULL },
    { NULL }
};

#if 0
/* The file selector cannot select a non-existing file -> does not work */
UI_CALLBACK(set_printer_dump_file)
{
    char *resource = (char *)UI_MENU_CB_PARAM;
    char *filename;
    ui_button_t button;
    static char *last_dir;

    vsync_suspend_speed_eval();

    filename = ui_select_file(_("Select printer dump file"),
                              NULL, False, last_dir, NULL, &button, False);
    switch (button) {
      case UI_BUTTON_OK:
        resources_set_value(resource, (resource_value_t)filename);
        if (last_dir)
            lib_free(last_dir);
        util_fname_split(filename, &last_dir, NULL);
        break;
      default:
        /* Do nothing special.  */
        break;
    }
    if (filename != NULL)
        lib_free(filename);
}
#endif

UI_CALLBACK(uiprinter_set_printer_exec_file)
{
    uilib_select_string((char *)UI_MENU_CB_PARAM,
                        _("Command to execute for printing (preceed with '|')"),
                        _("Command:"));
}

/* ------------------------------------------------------------------------- */

UI_MENU_DEFINE_RADIO(Printer4)
UI_MENU_DEFINE_RADIO(Printer5)

ui_menu_entry_t uiprinter_set_printer4_type_submenu[] = {
    { N_("*None"), (ui_callback_t)radio_Printer4,
      (ui_callback_data_t)PRINTER_DEVICE_NONE, NULL },
    { N_("*File system access"), (ui_callback_t)radio_Printer4,
      (ui_callback_data_t)PRINTER_DEVICE_FS, NULL },
#ifdef HAVE_OPENCBM
    { N_("*Real device access"), (ui_callback_t)radio_Printer4,
      (ui_callback_data_t)PRINTER_DEVICE_REAL, NULL },
#endif
    { NULL }
};

ui_menu_entry_t uiprinter_set_printer5_type_submenu[] = {
    { N_("*None"), (ui_callback_t)radio_Printer5,
      (ui_callback_data_t)PRINTER_DEVICE_NONE, NULL },
    { N_("*File system access"), (ui_callback_t)radio_Printer5,
      (ui_callback_data_t)PRINTER_DEVICE_FS, NULL },
#ifdef HAVE_OPENCBM
    { N_("*Real device access"), (ui_callback_t)radio_Printer5,
      (ui_callback_data_t)PRINTER_DEVICE_REAL, NULL },
#endif
    { NULL }
};

/* ------------------------------------------------------------------------- */

UI_CALLBACK(uiprinter_formfeed)
{
    printer_formfeed((unsigned int)UI_MENU_CB_PARAM);
}

