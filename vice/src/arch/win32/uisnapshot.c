/*
 * uisnapshot.c - Implementation of the snapshot load/save dialogs.
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
 *  Ettore Perazzoli <ettore@comm2000.it>
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
#include <windows.h>
#include <commdlg.h>
#include <tchar.h>

#include "drive.h"
#include "lib.h"
#include "machine.h"
#include "res.h"
#include "resources.h"
#include "system.h"
#include "ui.h"
#include "uilib.h"
#include "winmain.h"
#include "util.h"
#include "videoarch.h"
#include "vsync.h"


/* Mingw & pre VC 6 headers doesn't have this definition */
#ifndef OFN_ENABLESIZING
#define OFN_ENABLESIZING    0x00800000
#endif

static int save_roms = 0;
static int save_disks = 0;

static void init_snapshot_dialog(HWND hwnd)
{
    CheckDlgButton(hwnd, IDC_TOGGLE_SNAPSHOT_SAVE_DISKS, save_disks
                   ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(hwnd, IDC_TOGGLE_SNAPSHOT_SAVE_ROMS, save_roms
                   ? BST_CHECKED : BST_UNCHECKED);
    SetDlgItemText(hwnd, IDC_SNAPSHOT_SAVE_IMAGE, TEXT(""));
}


static UINT APIENTRY hook_save_snapshot(HWND hwnd, UINT uimsg, WPARAM wparam,
                                        LPARAM lparam)
{
    switch (uimsg) {
      case WM_INITDIALOG:
        init_snapshot_dialog(hwnd);
        break;
      case WM_NOTIFY:
        save_disks = IsDlgButtonChecked(hwnd,
                         IDC_TOGGLE_SNAPSHOT_SAVE_DISKS) == BST_CHECKED ? 1 : 0;
        save_roms = IsDlgButtonChecked(hwnd,
                        IDC_TOGGLE_SNAPSHOT_SAVE_ROMS) == BST_CHECKED ? 1 : 0;
        break;
    }
    return 0;
}


char *ui_save_snapshot(const char *title, const char *filter, 
                       HWND hwnd, int dialog_template)
{
    TCHAR name[1024] = "";
    OPENFILENAME ofn;
    char *ret = NULL;
    TCHAR *st_filter;

    st_filter = system_mbstowcs_alloc(filter);

    memset(&ofn, 0, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.hInstance = winmain_instance;
    ofn.lpstrFilter = st_filter;
    ofn.lpstrCustomFilter = NULL;
    ofn.nMaxCustFilter = 0;
    ofn.nFilterIndex = 1;
    ofn.lpstrFile = name;
    ofn.nMaxFile = sizeof(name);
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.lpstrTitle = title;
    ofn.Flags = (OFN_EXPLORER
                 | OFN_HIDEREADONLY
                 | OFN_NOTESTFILECREATE
                 | OFN_FILEMUSTEXIST
                 | OFN_SHAREAWARE
                 | OFN_ENABLESIZING);
    if (dialog_template) {
        ofn.lpfnHook = hook_save_snapshot;
        ofn.lpTemplateName = MAKEINTRESOURCE(dialog_template);
        ofn.Flags = (ofn.Flags
                 | OFN_ENABLEHOOK
                 | OFN_ENABLETEMPLATE);
    }
    ofn.nFileOffset = 0;
    ofn.nFileExtension = 0;
    ofn.lpstrDefExt = NULL;
    vsync_suspend_speed_eval();

    if (GetSaveFileName(&ofn))
        ret = system_wcstombs_alloc(name);

    return ret;
}


void ui_snapshot_save_dialog(HWND hwnd)
{
    char *s;
    s = ui_save_snapshot("Save snapshot image",
                         "VICE snapshot files (*.vsf)\0*.vsf\0",
                         hwnd, IDD_SNAPSHOT_SAVE_DIALOG);
    if (s != NULL) {
        util_add_extension(&s, "vsf");

        if (machine_write_snapshot(s, save_roms, save_disks, 0) < 0)
            ui_error("Cannot write snapshot file `%s'.", s);
        lib_free(s);
    }
}


void ui_snapshot_load_dialog(HWND hwnd)
{
    char *s;
    if ((s = ui_select_file(hwnd, "Load snapshot image",
            UI_LIB_FILTER_ALL | UI_LIB_FILTER_SNAPSHOT,
            FILE_SELECTOR_SNAPSHOT_STYLE,NULL)) != NULL) {
         if (machine_read_snapshot(s, 0) < 0)
             ui_error("Cannot read snapshot image");
         lib_free(s);
    }
}

