/*
 * uirom.h - Implementation of the ROM settings dialog box.
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

#include <string.h>
#include <windows.h>

#include "lib.h"
#include "res.h"
#include "resources.h"
#include "ui.h"
#include "uilib.h"
#include "uirom.h"
#include "winmain.h"


const uirom_settings_t *settings;


static void init_rom_dialog(HWND hwnd)
{
    unsigned int n = 0;

    while (settings[n].realname != NULL) {
        const char *filename;

        resources_get_value(settings[n].resname, (void *)&filename);
        SetDlgItemText(hwnd, settings[n].idc_filename,
                       filename != NULL ? filename : "");
        n++;
    }
}

static void set_resources(HWND hwnd)
{
    unsigned int n = 0;

    while (settings[n].realname != NULL) {
        char filename[MAX_PATH];

        GetDlgItemText(hwnd, settings[n].idc_filename, filename, MAX_PATH);
        resources_set_value(settings[n].resname, (resource_value_t)filename);
        n++;
    }
}

static void browse_command(HWND hwnd, unsigned int command)
{
    unsigned int n = 0;

    while (settings[n].realname != NULL) {
        if ((unsigned int)command == settings[n].idc_browse) {
            char *filename, *realname;

            realname = lib_msprintf("Load %s ROM image", settings[n].realname);
            filename = ui_select_file(hwnd, realname, UI_LIB_FILTER_ALL,
                                      FILE_SELECTOR_DEFAULT_STYLE, NULL);
            lib_free(realname);

            if (filename == NULL)
                return;

            SetDlgItemText(hwnd, settings[n].idc_filename, filename);
            lib_free(filename);
        }
        n++;
    }
}

static BOOL CALLBACK dialog_proc(HWND hwnd, UINT msg, WPARAM wparam,
                                 LPARAM lparam)
{
    int command;

    switch (msg) {
      case WM_COMMAND:
        command = LOWORD(wparam);

        browse_command(hwnd, command);

        switch (command) {
          case IDOK:
            set_resources(hwnd);
          case IDCANCEL:
            EndDialog(hwnd, 0);
            return TRUE;
        }
        return FALSE;
      case WM_CLOSE:
        EndDialog(hwnd, 0);
        return TRUE;
      case WM_INITDIALOG:
        init_rom_dialog(hwnd);
        return TRUE;
    }
    return FALSE;
}

void uirom_settings_dialog(HWND hwnd, unsigned int idd_dialog,
                           const uirom_settings_t *uirom_settings)
{
    settings = uirom_settings;

    DialogBox(winmain_instance, (LPCTSTR)idd_dialog, hwnd, dialog_proc);
}

