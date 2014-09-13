/*
 * uiprinteriecplus4.c
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

#include <stdio.h>

#include "printer.h"
#include "uimenu.h"
#include "uiprinter.h"

UI_MENU_DEFINE_TOGGLE(IECDevice4)
UI_MENU_DEFINE_TOGGLE(IECDevice5)
UI_MENU_DEFINE_TOGGLE(IECDevice6)

#ifdef HAVE_OPENCBM
UI_MENU_DEFINE_TOGGLE(IECDevice7)
UI_MENU_DEFINE_TOGGLE(Printer7)
#endif

ui_menu_entry_t printeriec_plus4_settings_menu[] = {
    { N_("Printer #4 emulation"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, uiprinter_set_printer4_type_submenu },
    { N_("Printer #4 enable IEC device"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_IECDevice4, NULL, NULL },
    { N_("Printer #4 driver"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, uiprinter_pr4_driver_submenu },
    { N_("Printer #4 output"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, uiprinter_pr4_output_submenu },
    { N_("Printer #4 text output device"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, uiprinter_pr4_device_submenu },
    { N_("Printer #4 formfeed"), UI_MENU_TYPE_NORMAL,
      (ui_callback_t)uiprinter_formfeed, (ui_callback_data_t)0, NULL, 
      KEYSYM_4, UI_HOTMOD_META },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Printer #5 emulation"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, uiprinter_set_printer5_type_submenu },
    { N_("Printer #5 enable IEC device"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_IECDevice5, NULL, NULL },
    { N_("Printer #5 driver"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, uiprinter_pr5_driver_submenu },
    { N_("Printer #5 output"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, uiprinter_pr5_output_submenu },
    { N_("Printer #5 text output device"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, uiprinter_pr5_device_submenu },
    { N_("Printer #5 formfeed"), UI_MENU_TYPE_NORMAL,
      (ui_callback_t)uiprinter_formfeed, (ui_callback_data_t)1, NULL,
      KEYSYM_5, UI_HOTMOD_META },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Printer #6 emulation"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, uiprinter_set_printer6_type_submenu },
    { N_("Printer #6 enable IEC device"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_IECDevice6, NULL, NULL },
    { N_("Printer #6 driver"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, uiprinter_pr6_driver_submenu },
    { N_("Printer #6 output"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, uiprinter_pr6_output_submenu },
    { N_("Printer #6 text output device"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, uiprinter_pr6_device_submenu },
    { N_("Printer #6 formfeed"), UI_MENU_TYPE_NORMAL,
      (ui_callback_t)uiprinter_formfeed, (ui_callback_data_t)2, NULL,
      KEYSYM_6, UI_HOTMOD_META },
#ifdef HAVE_OPENCBM
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Device #7 Real device access"), UI_MENU_TYPE_TICK, (ui_callback_t)toggle_Printer7,
      (ui_callback_data_t)PRINTER_DEVICE_REAL, NULL },
    { N_("Device #7 enable IEC device"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_IECDevice7, NULL, NULL },
#endif
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Printer text device 1"), UI_MENU_TYPE_DOTS, (ui_callback_t)uiprinter_set_printer_exec_file,
      (ui_callback_data_t)"PrinterTextDevice1", NULL },
    { N_("Printer text device 2"), UI_MENU_TYPE_DOTS, (ui_callback_t)uiprinter_set_printer_exec_file,
      (ui_callback_data_t)"PrinterTextDevice2", NULL },
    { N_("Printer text device 3"), UI_MENU_TYPE_DOTS, (ui_callback_t)uiprinter_set_printer_exec_file,
      (ui_callback_data_t)"PrinterTextDevice3", NULL },
    { NULL }
};
