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

#include "drive.h"
#include "gfxoutput.h"
#include "lib.h"
#include "machine.h"
#include "res.h"
#include "resources.h"
#include "screenshot.h"
#include "serial.h"
#include "sound.h"
#include "ui.h"
#include "uilib.h"
#include "winmain.h"
#include "utils.h"
#include "videoarch.h"
#include "vsync.h"


/* Mingw & pre VC 6 headers doesn't have this definition */
#ifndef OFN_ENABLESIZING
#define OFN_ENABLESIZING    0x00800000
#endif

static int save_roms = 0;
static int save_disks = 0;
static char image[100];
#define MAXSCRNDRVLEN 20
static char screendrivername[MAXSCRNDRVLEN];

static void init_dialog(HWND hwnd)
{
    CheckDlgButton(hwnd, IDC_TOGGLE_SNAPSHOT_SAVE_DISKS, save_disks
                   ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(hwnd, IDC_TOGGLE_SNAPSHOT_SAVE_ROMS, save_roms
                   ? BST_CHECKED : BST_UNCHECKED);
    image[0] = '\0';
    SetDlgItemText(hwnd, IDC_SNAPSHOT_SAVE_IMAGE, "");
}


static UINT APIENTRY hook_save_snapshot(HWND hwnd, UINT uimsg, WPARAM wparam,
                                        LPARAM lparam)
{
    /* this is also the hook for screenshot dialog */
    switch (uimsg) {
      case WM_INITDIALOG:
        {
            HWND scrndrv_combo;
            int i;
            scrndrv_combo = GetDlgItem(hwnd,IDC_SCREENSHOT_DRIVER);
            if (scrndrv_combo) {
                gfxoutputdrv_t *driver = gfxoutput_drivers_iter_init();
                for (i = 0; i < gfxoutput_num_drivers(); i++) {
                    SendMessage(scrndrv_combo,CB_ADDSTRING, 0,
                                (LPARAM)driver->name);
                           	driver = gfxoutput_drivers_iter_next();
                }
                SendMessage(scrndrv_combo,CB_SETCURSEL,(WPARAM)0, 0);
            }
        }
        break;
      case WM_NOTIFY:
        GetDlgItemText(hwnd,IDC_SCREENSHOT_DRIVER,
                       screendrivername,MAXSCRNDRVLEN);
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
    vsync_suspend_speed_eval();

    if (GetSaveFileName(&ofn)) {
        return lib_stralloc(name);
    } else {
        return NULL;
    }
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

void ui_screenshot_save_dialog(HWND hwnd)
{
    char *s;

    s = ui_save_snapshot("Save screenshot image",
        "Picture files (*.bmp;*.png)\0*.bmp;*.png\0", hwnd,
        IDD_SCREENSHOT_SAVE_DIALOG);

    if (s != NULL) {
        gfxoutputdrv_t *selected_driver;
        selected_driver = gfxoutput_get_driver(screendrivername);
        if (!selected_driver) {
            ui_error("No driver selected or selected driver not supported");
            return;
        }
        util_add_extension(&s, selected_driver->default_extension);

        if (screenshot_save(selected_driver->name, s,
            video_canvas_for_hwnd(hwnd)) < 0)
            ui_error("Cannot write screenshot file `%s'.", s);
        lib_free(s);
    }
}

void ui_soundshot_save_dialog(HWND hwnd)
{
    char *s;
    char *devicename;
    static char old_device[16] = "dx";

    resources_get_value("SoundDeviceName",(resource_value_t *) &devicename);
    if (devicename && !strcmp(devicename,"wav")) {
        /* the recording is active; stop it by switching to the default device*/
        resources_set_value("SoundDeviceName", old_device);
        ui_display_statustext("");
    } else {
        /* get the filename and switch to wav device */
        if (devicename)
            strcpy(old_device, devicename);
        else
            strcpy(old_device, "");

        s = ui_save_snapshot("Save sound file",
            "Sound files (*.wav)\0*.wav\0",hwnd,0);
        if (s != NULL) {
            util_add_extension(&s, "wav");
            resources_set_value("SoundDeviceArg", s);
            resources_set_value("SoundDeviceName", "wav");
            resources_set_value("Sound", (resource_value_t)1);
            lib_free(s);
            ui_display_statustext("Recording wav...");
        }
    }
}

