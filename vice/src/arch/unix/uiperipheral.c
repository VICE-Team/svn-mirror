/*
 * uiperipheral.c
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

#include "resources.h"
#include "uimenu.h"
#include "uiprinter.h"
#include "utils.h"
#include "vsync.h"


#if HAVE_CBM4LINUX || HAVE_OPENCBM
UI_MENU_DEFINE_TOGGLE(DriveRealdrive)
#endif

UI_MENU_DEFINE_TOGGLE(VirtualDevices)
UI_MENU_DEFINE_TOGGLE(FileSystemDevice8)
UI_MENU_DEFINE_TOGGLE(FileSystemDevice9)
UI_MENU_DEFINE_TOGGLE(FileSystemDevice10)
UI_MENU_DEFINE_TOGGLE(FileSystemDevice11)
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
UI_MENU_DEFINE_TOGGLE(AttachDevice8Raw)
UI_MENU_DEFINE_TOGGLE(AttachDevice9Raw)
UI_MENU_DEFINE_TOGGLE(AttachDevice10Raw)
UI_MENU_DEFINE_TOGGLE(AttachDevice11Raw)

static UI_CALLBACK(set_fsdevice_directory)
{
    /* FIXME: We need a real directory browser here.  */
    int unit = (int)UI_MENU_CB_PARAM;
    char *title;
    char *value, *new_value;
    int len;
    ui_button_t button;

    vsync_suspend_speed_eval();

    title = xmsprintf("Attach file system directory to device #%d", unit);

    resources_get_sprintf("FSDevice%dDir", (resource_value_t *)&value, unit);
    len = strlen(value) * 2;
    if (len < 255)
        len = 255;

    new_value = xmalloc(len + 1);
    strcpy(new_value, value);

    button = ui_input_string(title, _("Path:"), new_value, len);
    free(title);

    if (button == UI_BUTTON_OK)
        resources_set_sprintf("FSDevice%dDir", (resource_value_t)new_value,
                              unit);

    free(new_value);
}


static ui_menu_entry_t fsdevice_drive8_submenu[] = {
    { N_("*File system access"), (ui_callback_t)toggle_FileSystemDevice8,
      NULL, NULL },
    { "--" },
    { N_("*Raw device access"), (ui_callback_t)toggle_AttachDevice8Raw,
      NULL, NULL },
    { "--" },
    { N_("*Read only access"),
      (ui_callback_t)toggle_AttachDevice8Readonly, NULL, NULL },
    { "--" },
    { N_("File system directory..."), (ui_callback_t)set_fsdevice_directory,
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
    { N_("*File system access"),
      (ui_callback_t)toggle_FileSystemDevice9, NULL, NULL },
    { "--" },
    { N_("*Raw device access"), (ui_callback_t)toggle_AttachDevice9Raw,
      NULL, NULL },
    { "--" },
    { N_("*Read only access"),
      (ui_callback_t)toggle_AttachDevice9Readonly, NULL, NULL },
    { "--" },
    { N_("File system directory..."), (ui_callback_t)set_fsdevice_directory,
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
    { N_("*File system access"),
      (ui_callback_t)toggle_FileSystemDevice10, NULL, NULL },
    { "--" },
    { N_("*Raw device access"), (ui_callback_t)toggle_AttachDevice10Raw,
      NULL, NULL },
    { "--" },
    { N_("*Read only access"),
      (ui_callback_t)toggle_AttachDevice10Readonly, NULL, NULL },
    { "--" },
    { N_("File system directory..."),
      (ui_callback_t)set_fsdevice_directory, (ui_callback_data_t)10, NULL },
    { N_("*Convert P00 file names"),
      (ui_callback_t)toggle_FSDevice10ConvertP00, NULL, NULL },
    { N_("*Create P00 files on save"),
      (ui_callback_t)toggle_FSDevice10SaveP00, NULL, NULL },
    { N_("*Hide raw CBM files"),
      (ui_callback_t)toggle_FSDevice10HideCBMFiles, NULL, NULL },
    { NULL }
};

static ui_menu_entry_t fsdevice_drive11_submenu[] = {
    { N_("*File system access"),
      (ui_callback_t)toggle_FileSystemDevice11, NULL, NULL },
    { "--" },
    { N_("*Raw device access"), (ui_callback_t)toggle_AttachDevice11Raw,
      NULL, NULL },
    { "--" },
    { N_("*Read only access"),
      (ui_callback_t)toggle_AttachDevice11Readonly, NULL, NULL },
    { "--" },
    { N_("File system directory..."),
      (ui_callback_t)set_fsdevice_directory, (ui_callback_data_t)11, NULL },
    { N_("*Convert P00 file names"),
      (ui_callback_t)toggle_FSDevice11ConvertP00, NULL, NULL },
    { N_("*Create P00 files on save"),
      (ui_callback_t)toggle_FSDevice11SaveP00, NULL, NULL },
    { N_("*Hide raw CBM files"),
      (ui_callback_t)toggle_FSDevice11HideCBMFiles, NULL, NULL },
    { NULL }
};

ui_menu_entry_t peripheral_settings_submenu[] = {
    { N_("Device #8"), NULL, NULL, fsdevice_drive8_submenu },
    { N_("Device #9"), NULL, NULL, fsdevice_drive9_submenu },
    { N_("Device #10"), NULL, NULL, fsdevice_drive10_submenu },
    { N_("Device #11"), NULL, NULL, fsdevice_drive11_submenu },
    { "--" },
    { N_("Printer settings"), NULL, NULL, printer_settings_menu },
    { "--" },
    { N_("*Enable Virtual Devices"), (ui_callback_t)toggle_VirtualDevices,
      NULL, NULL },
#if HAVE_CBM4LINUX || HAVE_OPENCBM
    { N_("*Enable CBM4Linux drive access"),
      (ui_callback_t)toggle_DriveRealdrive, NULL, NULL },
#endif
    { NULL }
};

ui_menu_entry_t ui_peripheral_settings_menu[] = {
    { N_("Peripheral settings"),
      NULL, NULL, peripheral_settings_submenu },
    { NULL }
};

