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
#include "uimenu.h"
#include "util.h"
#include "vsync.h"


UI_MENU_DEFINE_STRING_RADIO(Printer4Driver)

static ui_menu_entry_t pr4_driver_submenu[] = {
    { "*ASCII", (ui_callback_t)radio_Printer4Driver,
      (ui_callback_data_t)"ascii", NULL },
    { "*MPS803", (ui_callback_t)radio_Printer4Driver,
      (ui_callback_data_t)"mps803", NULL },
    { "*NL10", (ui_callback_t)radio_Printer4Driver,
      (ui_callback_data_t)"nl10", NULL },
    { NULL }
};

UI_MENU_DEFINE_STRING_RADIO(Printer5Driver)

static ui_menu_entry_t pr5_driver_submenu[] = {
    { "*ASCII", (ui_callback_t)radio_Printer5Driver,
      (ui_callback_data_t)"ascii", NULL },
    { "*MPS803", (ui_callback_t)radio_Printer5Driver,
      (ui_callback_data_t)"mps803", NULL },
    { "*NL10", (ui_callback_t)radio_Printer5Driver,
      (ui_callback_data_t)"nl10", NULL },
    { NULL }
};

UI_MENU_DEFINE_STRING_RADIO(PrinterUserportDriver)

static ui_menu_entry_t pruser_driver_submenu[] = {
    { "*ASCII", (ui_callback_t)radio_PrinterUserportDriver,
      (ui_callback_data_t)"ascii", NULL },
    { "*MPS803", (ui_callback_t)radio_PrinterUserportDriver,
      (ui_callback_data_t)"mps803", NULL },
    { "*NL10", (ui_callback_t)radio_PrinterUserportDriver,
      (ui_callback_data_t)"nl10", NULL },
    { NULL }
};

UI_MENU_DEFINE_RADIO(PrinterUserportTextDevice)

static ui_menu_entry_t pruser_device_submenu[] = {
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

static ui_menu_entry_t pr4_device_submenu[] = {
    { N_("*Printer 1 (file dump)"),
      (ui_callback_t)radio_Printer4TextDevice, (ui_callback_data_t)0, NULL },
    { N_("*Printer 2 (exec)"),
      (ui_callback_t)radio_Printer4TextDevice, (ui_callback_data_t)1, NULL },
    { N_("*Printer 3 (exec)"),
      (ui_callback_t)radio_Printer4TextDevice, (ui_callback_data_t)2, NULL },
    { NULL }
};

UI_MENU_DEFINE_RADIO(Printer5TextDevice)

static ui_menu_entry_t pr5_device_submenu[] = {
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
static UI_CALLBACK(set_printer_dump_file)
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

static UI_CALLBACK(set_printer_exec_file)
{
    char *resname = (char *)UI_MENU_CB_PARAM;
    char *title;
    char *value;
    char *new_value;
    int len;
    ui_button_t button;

    vsync_suspend_speed_eval();
    title = lib_stralloc(_("Command to execute for printing (preceed with '|')"));

    resources_get_value(resname, (void *)&value);
    len = strlen(value) * 2;
    if (len < 255)
        len = 255;

    new_value = lib_malloc(len + 1);
    strcpy(new_value, value);

    button = ui_input_string(title, _("Command:"), new_value, len);
    lib_free(title);

    if (button == UI_BUTTON_OK)
        resources_set_value(resname, (resource_value_t)new_value);

    lib_free(new_value);
}

/* ------------------------------------------------------------------------- */

UI_MENU_DEFINE_RADIO(Printer4)
UI_MENU_DEFINE_RADIO(Printer5)

static ui_menu_entry_t set_printer4_type_submenu[] = {
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

static ui_menu_entry_t set_printer5_type_submenu[] = {
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

UI_MENU_DEFINE_TOGGLE(PrinterUserport)
UI_MENU_DEFINE_TOGGLE(IECDevice4)
UI_MENU_DEFINE_TOGGLE(IECDevice5)

static UI_CALLBACK(flush_printer)
{
    printer_interface_serial_close((unsigned int)UI_MENU_CB_PARAM);
}

ui_menu_entry_t printer_settings_menu[] = {
    { N_("*IEC printer #4 emulation"),
      NULL, NULL, set_printer4_type_submenu },
    { N_("*IEC printer #4 enable IEC device"),
      (ui_callback_t)toggle_IECDevice4, NULL, NULL },
    { N_("IEC printer #4 driver"),
      NULL, NULL, pr4_driver_submenu },
    { N_("IEC printer #4 text output device"),
      NULL, NULL, pr4_device_submenu },
    { N_("Flush IEC printer #4 device"),
      (ui_callback_t)flush_printer, (ui_callback_data_t)4, NULL },
    { "--" },
    { N_("*IEC printer #5 emulation"),
      NULL, NULL, set_printer5_type_submenu },
    { N_("*IEC printer #5 enable IEC device"),
      (ui_callback_t)toggle_IECDevice5, NULL, NULL },
    { N_("IEC printer #5 driver"),
      NULL, NULL, pr5_driver_submenu },
    { N_("IEC printer #5 text output device"),
      NULL, NULL, pr5_device_submenu },
    { N_("Flush IEC printer #5 device"),
      (ui_callback_t)flush_printer, (ui_callback_data_t)5, NULL },
    { "--" },
    { N_("*Userport printer emulation"),
      (ui_callback_t)toggle_PrinterUserport, NULL, NULL },
    { N_("Userport printer driver"),
      NULL, NULL, pruser_driver_submenu },
    { N_("Userport printer text output device"),
      NULL, NULL, pruser_device_submenu },
    { "--" },
    { N_("Printer text device 1..."), (ui_callback_t)set_printer_exec_file,
      (ui_callback_data_t)"PrinterTextDevice1", NULL },
    { N_("Printer text device 2..."), (ui_callback_t)set_printer_exec_file,
      (ui_callback_data_t)"PrinterTextDevice2", NULL },
    { N_("Printer text device 3..."), (ui_callback_t)set_printer_exec_file,
      (ui_callback_data_t)"PrinterTextDevice3", NULL },
    { NULL }
};

