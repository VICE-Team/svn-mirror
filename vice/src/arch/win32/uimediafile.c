/*
 * uimediafile.c - Implementation of the mediafile save dialog.
 *
 * Written by
 *  Andreas Matthies <andreas.matthies@gmx.net>
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
#include <tchar.h>
#include <commdlg.h>

#include "gfxoutput.h"
#include "lib.h"
#include "machine.h"
#include "res.h"
#include "resources.h"
#include "screenshot.h"
#include "sound.h"
#include "system.h"
#include "translate.h"
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

#define MAXSCRNDRVLEN 256
static char screendrivername[MAXSCRNDRVLEN];
static OPENFILENAME ofn;
static gfxoutputdrv_t *selected_driver;

static void init_mediafile_dialog(HWND hwnd)
{
    HWND scrndrv_combo;
    gfxoutputdrv_t *driver;
    int i;
    int last_driver = 0;
    scrndrv_combo = GetDlgItem(hwnd,IDC_SCREENSHOT_DRIVER);
    driver = gfxoutput_drivers_iter_init();
    for (i = 0; i < gfxoutput_num_drivers(); i++) {
        if (driver == selected_driver)
            last_driver = i;
        SendMessage(scrndrv_combo,CB_ADDSTRING, 0,
                    (LPARAM)driver->displayname);
        driver = gfxoutput_drivers_iter_next();
    }
    SendMessage(scrndrv_combo,CB_SETCURSEL,(WPARAM)last_driver, 0);
}


static UINT APIENTRY hook_save_mediafile(HWND hwnd, UINT uimsg, WPARAM wparam,
                                        LPARAM lparam)
{
    TCHAR st_screendrivername[MAXSCRNDRVLEN];

    switch (uimsg) {
      case WM_INITDIALOG:
        init_mediafile_dialog(hwnd);
        break;
      case WM_NOTIFY:
        GetDlgItemText(hwnd,IDC_SCREENSHOT_DRIVER,
                       st_screendrivername, MAXSCRNDRVLEN);
        system_wcstombs(screendrivername, st_screendrivername, MAXSCRNDRVLEN);
        selected_driver = gfxoutput_get_driver(screendrivername);
        if (selected_driver)
            ofn.lpstrFilter = selected_driver->default_extension;

        break;
    }
    return 0;
}


static char *ui_save_mediafile(const TCHAR *title, const TCHAR *filter, 
                       HWND hwnd, int dialog_template)
{
    TCHAR name[1024] = TEXT("");
    char *ret = NULL;

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
                 | OFN_SHAREAWARE);
    if (dialog_template) {
        ofn.lpfnHook = hook_save_mediafile;
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



void ui_mediafile_save_dialog(HWND hwnd)
{
    int filter_len;
    char *s;
    char filter[100];

    if (screenshot_is_recording()) {
        /* the recording is active; stop it  */
        screenshot_stop_recording();
        ui_display_statustext("", 0);
        return;
    }

    filter_len=strlen(translate_text(IDS_MEDIA_FILES_FILTER));
    sprintf(filter,"%s0*.bmp;*.png;*.wav;*.mp3;*.avi;*.mpg0",translate_text(IDS_MEDIA_FILES_FILTER));
    filter[filter_len]='\0';
    filter[filter_len+26]='\0';
    s = ui_save_mediafile(translate_text(IDS_SAVE_MEDIA_IMAGE),
        filter,
        hwnd,
        translate_res(IDD_MEDIAFILE_DIALOG));

    if (s != NULL) {
        selected_driver = gfxoutput_get_driver(screendrivername);
        if (!selected_driver) {
            ui_error(translate_text(IDS_NO_DRIVER_SELECT_SUPPORT));
            return;
        }

        util_add_extension(&s, selected_driver->default_extension);

        if (screenshot_save(selected_driver->name, s,
            video_canvas_for_hwnd(hwnd)) < 0)
            ui_error(translate_text(IDS_CANT_WRITE_SCREENSHOT_S), s);
        lib_free(s);
    }
}

