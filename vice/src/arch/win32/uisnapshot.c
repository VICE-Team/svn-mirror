/*
 * uisnapshot.c - Implementation of the snapshot load/save dialogs.
 *
 * Written by
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
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

#include <string.h>
#include <windows.h>

#include "drive.h"
#include "machine.h"
#include "res.h"
#include "resources.h"
#include "screenshot.h"
#include "serial.h"
#include "sound.h"
#include "ui.h"
#include "uidrive.h"
#include "uilib.h"
#include "winmain.h"
#include "utils.h"

/* Mingw & pre VC 6 headers doesn't have this definition */
#ifndef OFN_ENABLESIZING
#define OFN_ENABLESIZING    0x00800000
#endif

static int save_roms = 0;
static int save_disks = 0;
static char image[100];

static void init_dialog(HWND hwnd)
{
    CheckDlgButton(hwnd, IDC_TOGGLE_SNAPSHOT_SAVE_DISKS, save_disks
                   ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(hwnd, IDC_TOGGLE_SNAPSHOT_SAVE_ROMS, save_roms
                   ? BST_CHECKED : BST_UNCHECKED);
    image[0] = '\0';
    SetDlgItemText(hwnd, IDC_SNAPSHOT_SAVE_IMAGE, "");
}


static UINT APIENTRY hook_save_snapshot(HWND hwnd, UINT uimsg, WPARAM wparam, LPARAM lparam)
{
    switch (uimsg) {
        case WM_NOTIFY:
          save_disks = 
              IsDlgButtonChecked
              (hwnd,IDC_TOGGLE_SNAPSHOT_SAVE_DISKS)==BST_CHECKED ? 1 : 0;
          save_roms = 
              IsDlgButtonChecked
              (hwnd,IDC_TOGGLE_SNAPSHOT_SAVE_ROMS)==BST_CHECKED ? 1 : 0;
            break;
    }
    return 0;
}


char *ui_save_snapshot(const char *title, const char *filter, 
                            HWND hwnd, int dialog_template)
{
    char name[1024] = "";
    OPENFILENAME ofn;

    memset(&ofn, 0, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.hInstance = winmain_instance;
    ofn.lpstrFilter = filter;
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

    if (GetSaveFileName(&ofn)) {
        return stralloc(name);
    } else {
        return NULL;
    }
}


void ui_snapshot_save_dialog(HWND hwnd)
{
    char *s;
    s = ui_save_snapshot("Save snapshot image",
        "VICE snapshot files (*.vsf)\0*.vsf\0",hwnd,IDD_SNAPSHOT_SAVE_DIALOG);
    if (s != NULL) {
        char *sExt = ui_ensure_extension( s, ".vsf" );
        if (machine_write_snapshot(sExt, save_roms, save_disks) < 0)
            ui_error("Cannot write snapshot file.");
        free(sExt);
        free(s);
    }
}


void ui_snapshot_load_dialog(HWND hwnd)
{
    char *s;
    if ((s = ui_select_file(hwnd,"Load snapshot image",
        "VICE snapshot files (*.vsf)\0*.vsf\0"
        "All files (*.*)\0*.*\0", FILE_SELECTOR_DEFAULT_STYLE,NULL)) != NULL) {
         if (machine_read_snapshot(s) < 0)
             ui_error("Cannot read snapshot image");
         free(s);
    }
}

extern  HWND            window_handles[2];

void ui_screenshot_save_dialog(HWND hwnd)
{
    char *s;
    int i;
    
    for (i=0; i<2; i++) {
        if (hwnd == window_handles[i])
            break;
    }
    s = ui_save_snapshot("Save screenshot image",
        "Bitmap files (*.bmp)\0*.bmp\0",hwnd,0);
    if (s != NULL) {
        char *sExt = ui_ensure_extension( s, ".bmp" );
        if (screenshot_save("BMP",sExt, i) < 0)
            ui_error("Cannot write screenshot file.");
        free(sExt);
        free(s);
    }
}

void ui_soundshot_save_dialog(HWND hwnd)
{
    char *s;
    char *devicename;

    resources_get_value("SoundDeviceName",(resource_value_t *) &devicename);
    if (devicename && !strcmp(devicename,"wav")) {
        /* the recording is active; stop it by switching to the default device*/
        resources_set_value("SoundDeviceName","");
        ui_display_statustext("");
    } else {
        /* get the filename and switch to wav device */
        s = ui_save_snapshot("Save sound file",
            "Sound files (*.wav)\0*.wav\0",hwnd,0);
        if (s != NULL) {
            char *sExt = ui_ensure_extension( s, ".wav" );
            resources_set_value("SoundDeviceArg",sExt);
            resources_set_value("SoundDeviceName","wav");
            resources_set_value("Sound",(resource_value_t) 1);
            free(sExt);
            free(s);
            ui_display_statustext("Recording wav...");
        }
    }
    free(devicename);
}
