/*
 * uiperipheraliec.c
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
#include <stdlib.h>
#include <string.h>

#include "attach.h"
#include "lib.h"
#include "resources.h"
#include "uimenu.h"
#include "uiprinteriec.h"
#include "uiprinteriecplus4.h"
#include "uiperipheral.h"
#include "vsync.h"

UI_MENU_DEFINE_TOGGLE(VirtualDevices)
UI_MENU_DEFINE_TOGGLE(FSDevice8ConvertP00)
UI_MENU_DEFINE_TOGGLE(FSDevice9ConvertP00)
UI_MENU_DEFINE_TOGGLE(FSDevice10ConvertP00)
UI_MENU_DEFINE_TOGGLE(FSDevice11ConvertP00)
UI_MENU_DEFINE_TOGGLE(FSDevice8SaveP00)
UI_MENU_DEFINE_TOGGLE(FSDevice9SaveP00)
UI_MENU_DEFINE_TOGGLE(FSDevice10SaveP00)
UI_MENU_DEFINE_TOGGLE(FSDevice11SaveP00)
UI_MENU_DEFINE_TOGGLE(FSDevice8HideCBMFiles)
UI_MENU_DEFINE_TOGGLE(FSDevice9HideCBMFiles)
UI_MENU_DEFINE_TOGGLE(FSDevice10HideCBMFiles)
UI_MENU_DEFINE_TOGGLE(FSDevice11HideCBMFiles)
UI_MENU_DEFINE_TOGGLE(AttachDevice8Readonly)
UI_MENU_DEFINE_TOGGLE(AttachDevice9Readonly)
UI_MENU_DEFINE_TOGGLE(AttachDevice10Readonly)
UI_MENU_DEFINE_TOGGLE(AttachDevice11Readonly)
UI_MENU_DEFINE_TOGGLE(IECDevice8)
UI_MENU_DEFINE_TOGGLE(IECDevice9)
UI_MENU_DEFINE_TOGGLE(IECDevice10)
UI_MENU_DEFINE_TOGGLE(IECDevice11)

static ui_menu_entry_t fsdevice_drive8_submenu[] = {
    { N_("Device type"), NULL, NULL, uiperipheral_set_device8_type_submenu },
    { "--" },
    { N_("*Enable IEC device"),
      (ui_callback_t)toggle_IECDevice8, NULL, NULL },
    { N_("*Read only access"),
      (ui_callback_t)toggle_AttachDevice8Readonly, NULL, NULL },
    { "--" },
    { N_("File system directory..."), (ui_callback_t)uiperipheral_set_fsdevice_directory,
      (ui_callback_data_t)8, NULL },
    { N_("*Convert P00 file names"), (ui_callback_t)toggle_FSDevice8ConvertP00,
      NULL, NULL },
    { N_("*Create P00 files on save"), (ui_callback_t)toggle_FSDevice8SaveP00,
      NULL, NULL },
    { N_("*Hide raw CBM files"), (ui_callback_t)toggle_FSDevice8HideCBMFiles,
      NULL, NULL },
    { NULL }
};

static ui_menu_entry_t fsdevice_drive9_submenu[] = {
    { N_("Device type"), NULL, NULL, uiperipheral_set_device9_type_submenu },
    { "--" },
    { N_("*Enable IEC device"),
      (ui_callback_t)toggle_IECDevice9, NULL, NULL },
    { N_("*Read only access"),
      (ui_callback_t)toggle_AttachDevice9Readonly, NULL, NULL },
    { "--" },
    { N_("File system directory..."), (ui_callback_t)uiperipheral_set_fsdevice_directory,
      (ui_callback_data_t)9, NULL },
    { N_("*Convert P00 file names"), (ui_callback_t)toggle_FSDevice9ConvertP00,
      NULL, NULL },
    { N_("*Create P00 files on save"), (ui_callback_t)toggle_FSDevice9SaveP00,
      NULL, NULL },
    { N_("*Hide raw CBM files"), (ui_callback_t)toggle_FSDevice9HideCBMFiles,
      NULL, NULL },
    { NULL }
};

static ui_menu_entry_t fsdevice_drive10_submenu[] = {
    { N_("Device type"), NULL, NULL, uiperipheral_set_device10_type_submenu },
    { "--" },
    { N_("*Enable IEC device"),
      (ui_callback_t)toggle_IECDevice10, NULL, NULL },
    { N_("*Read only access"),
      (ui_callback_t)toggle_AttachDevice10Readonly, NULL, NULL },
    { "--" },
    { N_("File system directory..."),
      (ui_callback_t)uiperipheral_set_fsdevice_directory, (ui_callback_data_t)10, NULL },
    { N_("*Convert P00 file names"),
      (ui_callback_t)toggle_FSDevice10ConvertP00, NULL, NULL },
    { N_("*Create P00 files on save"),
      (ui_callback_t)toggle_FSDevice10SaveP00, NULL, NULL },
    { N_("*Hide raw CBM files"),
      (ui_callback_t)toggle_FSDevice10HideCBMFiles, NULL, NULL },
    { NULL }
};

static ui_menu_entry_t fsdevice_drive11_submenu[] = {
    { N_("Device type"), NULL, NULL, uiperipheral_set_device11_type_submenu },
    { "--" },
    { N_("*Enable IEC device"),
      (ui_callback_t)toggle_IECDevice11, NULL, NULL },
    { N_("*Read only access"),
      (ui_callback_t)toggle_AttachDevice11Readonly, NULL, NULL },
    { "--" },
    { N_("File system directory..."),
      (ui_callback_t)uiperipheral_set_fsdevice_directory, (ui_callback_data_t)11, NULL },
    { N_("*Convert P00 file names"),
      (ui_callback_t)toggle_FSDevice11ConvertP00, NULL, NULL },
    { N_("*Create P00 files on save"),
      (ui_callback_t)toggle_FSDevice11SaveP00, NULL, NULL },
    { N_("*Hide raw CBM files"),
      (ui_callback_t)toggle_FSDevice11HideCBMFiles, NULL, NULL },
    { NULL }
};

ui_menu_entry_t peripheraliec_settings_submenu[] = {
    { N_("Device #8"), NULL, NULL, fsdevice_drive8_submenu },
    { N_("Device #9"), NULL, NULL, fsdevice_drive9_submenu },
    { N_("Device #10"), NULL, NULL, fsdevice_drive10_submenu },
    { N_("Device #11"), NULL, NULL, fsdevice_drive11_submenu },
    { "--" },
    { N_("Printer settings"), NULL, NULL, printeriec_settings_menu },
    { "--" },
    { N_("*Enable Virtual Devices"), (ui_callback_t)toggle_VirtualDevices,
      NULL, NULL },
    { NULL }
};

ui_menu_entry_t peripheraliec_plus4_settings_submenu[] = {
    { N_("Device #8"), NULL, NULL, fsdevice_drive8_submenu },
    { N_("Device #9"), NULL, NULL, fsdevice_drive9_submenu },
    { N_("Device #10"), NULL, NULL, fsdevice_drive10_submenu },
    { N_("Device #11"), NULL, NULL, fsdevice_drive11_submenu },
    { "--" },
    { N_("Printer settings"), NULL, NULL, printeriec_plus4_settings_menu },
    { "--" },
    { N_("*Enable Virtual Devices"), (ui_callback_t)toggle_VirtualDevices,
      NULL, NULL },
    { NULL }
};

ui_menu_entry_t ui_peripheraliec_settings_menu[] = {
    { N_("Peripheral settings"),
      NULL, NULL, peripheraliec_settings_submenu },
    { NULL }
};

ui_menu_entry_t ui_peripheraliec_plus4_settings_menu[] = {
    { N_("Peripheral settings"),
      NULL, NULL, peripheraliec_plus4_settings_submenu },
    { NULL }
};
