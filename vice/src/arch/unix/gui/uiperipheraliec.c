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
#include "uidrive.h"
#include "uimenu.h"
#include "uiperipheral.h"
#include "vsync.h"

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
UI_MENU_DEFINE_TOGGLE(Drive8RTCSave)
UI_MENU_DEFINE_TOGGLE(Drive9RTCSave)
UI_MENU_DEFINE_TOGGLE(Drive10RTCSave)
UI_MENU_DEFINE_TOGGLE(Drive11RTCSave)

static ui_menu_entry_t fsdevice_drive8_submenu[] = {
    { N_("Enable IEC device"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_IECDevice8, NULL, NULL },
    { N_("Read only access"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_AttachDevice8Readonly, NULL, NULL },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("File system directory"), UI_MENU_TYPE_DOTS, (ui_callback_t)uiperipheral_set_fsdevice_directory,
      (ui_callback_data_t)8, NULL },
    { N_("Convert P00 file names"), UI_MENU_TYPE_TICK, (ui_callback_t)toggle_FSDevice8ConvertP00,
      NULL, NULL },
    { N_("Create P00 files on save"), UI_MENU_TYPE_TICK, (ui_callback_t)toggle_FSDevice8SaveP00,
      NULL, NULL },
    { N_("Hide non-P00 files"), UI_MENU_TYPE_TICK, (ui_callback_t)toggle_FSDevice8HideCBMFiles,
      NULL, NULL },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Drive RPM"), UI_MENU_TYPE_DOTS,
      (ui_callback_t)drive_rpm_settings, (ui_callback_data_t)8, NULL },
    { N_("Drive Wobble"), UI_MENU_TYPE_DOTS,
      (ui_callback_t)drive_wobble_settings, (ui_callback_data_t)8, NULL },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Enable RTC saving"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_Drive8RTCSave, NULL, NULL },
    { NULL }
};

static ui_menu_entry_t fsdevice_drive9_submenu[] = {
    { N_("Enable IEC device"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_IECDevice9, NULL, NULL },
    { N_("Read only access"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_AttachDevice9Readonly, NULL, NULL },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("File system directory"), UI_MENU_TYPE_DOTS, (ui_callback_t)uiperipheral_set_fsdevice_directory,
      (ui_callback_data_t)9, NULL },
    { N_("Convert P00 file names"), UI_MENU_TYPE_TICK, (ui_callback_t)toggle_FSDevice9ConvertP00,
      NULL, NULL },
    { N_("Create P00 files on save"), UI_MENU_TYPE_TICK, (ui_callback_t)toggle_FSDevice9SaveP00,
      NULL, NULL },
    { N_("Hide non-P00 files"), UI_MENU_TYPE_TICK, (ui_callback_t)toggle_FSDevice9HideCBMFiles,
      NULL, NULL },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Drive RPM"), UI_MENU_TYPE_DOTS,
      (ui_callback_t)drive_rpm_settings, (ui_callback_data_t)9, NULL },
    { N_("Drive Wobble"), UI_MENU_TYPE_DOTS,
      (ui_callback_t)drive_wobble_settings, (ui_callback_data_t)9, NULL },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Enable RTC saving"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_Drive9RTCSave, NULL, NULL },
    { NULL }
};

static ui_menu_entry_t fsdevice_drive10_submenu[] = {
    { N_("Enable IEC device"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_IECDevice10, NULL, NULL },
    { N_("Read only access"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_AttachDevice10Readonly, NULL, NULL },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("File system directory"), UI_MENU_TYPE_DOTS,
      (ui_callback_t)uiperipheral_set_fsdevice_directory, (ui_callback_data_t)10, NULL },
    { N_("Convert P00 file names"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_FSDevice10ConvertP00, NULL, NULL },
    { N_("Create P00 files on save"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_FSDevice10SaveP00, NULL, NULL },
    { N_("Hide non-P00 files"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_FSDevice10HideCBMFiles, NULL, NULL },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Drive RPM"), UI_MENU_TYPE_DOTS,
      (ui_callback_t)drive_rpm_settings, (ui_callback_data_t)10, NULL },
    { N_("Drive Wobble"), UI_MENU_TYPE_DOTS,
      (ui_callback_t)drive_wobble_settings, (ui_callback_data_t)10, NULL },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Enable RTC saving"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_Drive10RTCSave, NULL, NULL },
    { NULL }
};

static ui_menu_entry_t fsdevice_drive11_submenu[] = {
    { N_("Enable IEC device"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_IECDevice11, NULL, NULL },
    { N_("Read only access"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_AttachDevice11Readonly, NULL, NULL },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("File system directory"), UI_MENU_TYPE_DOTS,
      (ui_callback_t)uiperipheral_set_fsdevice_directory, (ui_callback_data_t)11, NULL },
    { N_("Convert P00 file names"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_FSDevice11ConvertP00, NULL, NULL },
    { N_("Create P00 files on save"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_FSDevice11SaveP00, NULL, NULL },
    { N_("Hide non-P00 files"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_FSDevice11HideCBMFiles, NULL, NULL },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Drive RPM"), UI_MENU_TYPE_DOTS,
      (ui_callback_t)drive_rpm_settings, (ui_callback_data_t)11, NULL },
    { N_("Drive Wobble"), UI_MENU_TYPE_DOTS,
      (ui_callback_t)drive_wobble_settings, (ui_callback_data_t)11, NULL },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Enable RTC saving"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_Drive11RTCSave, NULL, NULL },
    { NULL }
};

ui_menu_entry_t peripheraliec_settings_drive8_submenu[] = {
    { N_("Drive #8 device type"), UI_MENU_TYPE_NORMAL, NULL, NULL, uiperipheral_set_device8_type_submenu },
    { N_("Drive #8 options"), UI_MENU_TYPE_NORMAL, NULL, NULL, fsdevice_drive8_submenu },
    { NULL }
};

ui_menu_entry_t peripheraliec_settings_drive9_submenu[] = {
    { N_("Drive #9 device type"), UI_MENU_TYPE_NORMAL, NULL, NULL, uiperipheral_set_device9_type_submenu },
    { N_("Drive #9 options"), UI_MENU_TYPE_NORMAL, NULL, NULL, fsdevice_drive9_submenu },
    { NULL }
};

ui_menu_entry_t peripheraliec_settings_drive10_submenu[] = {
    { N_("Drive #10 device type"), UI_MENU_TYPE_NORMAL, NULL, NULL, uiperipheral_set_device10_type_submenu },
    { N_("Drive #10 options"), UI_MENU_TYPE_NORMAL, NULL, NULL, fsdevice_drive10_submenu },
    { NULL }
};

ui_menu_entry_t peripheraliec_settings_drive11_submenu[] = {
    { N_("Drive #11 device type"), UI_MENU_TYPE_NORMAL, NULL, NULL, uiperipheral_set_device11_type_submenu },
    { N_("Drive #11 options"), UI_MENU_TYPE_NORMAL, NULL, NULL, fsdevice_drive11_submenu },
    { NULL }
};

ui_menu_entry_t peripheraliec_plus4_settings_drive8_submenu[] = {
    { N_("Drive #8 device type"), UI_MENU_TYPE_NORMAL, NULL, NULL, uiperipheral_set_device8_type_submenu },
    { N_("Drive #8 options"), UI_MENU_TYPE_NORMAL, NULL, NULL, fsdevice_drive8_submenu },
    { NULL }
};

ui_menu_entry_t peripheraliec_plus4_settings_drive9_submenu[] = {
    { N_("Drive #9 device type"), UI_MENU_TYPE_NORMAL, NULL, NULL, uiperipheral_set_device9_type_submenu },
    { N_("Drive #9 options"), UI_MENU_TYPE_NORMAL, NULL, NULL, fsdevice_drive9_submenu },
    { NULL }
};

ui_menu_entry_t peripheraliec_plus4_settings_drive10_submenu[] = {
    { N_("Drive #10 device type"), UI_MENU_TYPE_NORMAL, NULL, NULL, uiperipheral_set_device10_type_submenu },
    { N_("Drive #10 options"), UI_MENU_TYPE_NORMAL, NULL, NULL, fsdevice_drive10_submenu },
    { NULL }
};

ui_menu_entry_t peripheraliec_plus4_settings_drive11_submenu[] = {
    { N_("Drive #11 device type"), UI_MENU_TYPE_NORMAL, NULL, NULL, uiperipheral_set_device11_type_submenu },
    { N_("Drive #11 options"), UI_MENU_TYPE_NORMAL, NULL, NULL, fsdevice_drive11_submenu },
    { NULL }
};
