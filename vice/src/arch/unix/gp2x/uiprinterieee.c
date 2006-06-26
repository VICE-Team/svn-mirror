/*
 * uiprinterieee.c
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

#include "printer.h"
#include "uimenu.h"
#include "uiprinter.h"


UI_MENU_DEFINE_TOGGLE(PrinterUserport)

ui_menu_entry_t printerieee_settings_menu[] = {
    { N_("*IEC printer #4 emulation"),
      NULL, NULL, uiprinter_set_printer4_type_submenu },
    { N_("IEC printer #4 driver"),
      NULL, NULL, uiprinter_pr4_driver_submenu },
    { N_("IEC printer #4 text output device"),
      NULL, NULL, uiprinter_pr4_device_submenu },
    { N_("IEC printer #4 formfeed"),
      (ui_callback_t)uiprinter_formfeed, (ui_callback_data_t)0, NULL,
      XK_4, UI_HOTMOD_META },
    { "--" },
    { N_("*IEC printer #5 emulation"),
      NULL, NULL, uiprinter_set_printer5_type_submenu },
    { N_("IEC printer #5 driver"),
      NULL, NULL, uiprinter_pr5_driver_submenu },
    { N_("IEC printer #5 text output device"),
      NULL, NULL, uiprinter_pr5_device_submenu },
    { N_("IEC printer #5 formfeed"),
      (ui_callback_t)uiprinter_formfeed, (ui_callback_data_t)1, NULL,
      XK_5, UI_HOTMOD_META },
    { "--" },
    { N_("*Userport printer emulation"),
      (ui_callback_t)toggle_PrinterUserport, NULL, NULL },
    { N_("Userport printer driver"),
      NULL, NULL, uiprinter_pruser_driver_submenu },
    { N_("Userport printer text output device"),
      NULL, NULL, uiprinter_pruser_device_submenu },
    { N_("Userport printer formfeed"),
      (ui_callback_t)uiprinter_formfeed, (ui_callback_data_t)2, NULL },
    { "--" },
    { N_("Printer text device 1..."), (ui_callback_t)uiprinter_set_printer_exec_file,
      (ui_callback_data_t)"PrinterTextDevice1", NULL },
    { N_("Printer text device 2..."), (ui_callback_t)uiprinter_set_printer_exec_file,
      (ui_callback_data_t)"PrinterTextDevice2", NULL },
    { N_("Printer text device 3..."), (ui_callback_t)uiprinter_set_printer_exec_file,
      (ui_callback_data_t)"PrinterTextDevice3", NULL },
    { NULL }
};

