/*
 * uiattach.c - Implementation of the disk/tape attach dialog box.
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
 *  Tibor Biczo <crown@mail.matav.hu>
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
#include <windows.h>
#include <tchar.h>

#include "attach.h"
#include "autostart.h"
#include "fullscrn.h"
#include "lib.h"
#include "res.h"
#include "system.h"
#include "tape.h"
#include "ui.h"
#include "uiattach.h"
#include "uilib.h"


void ui_attach_disk_dialog(WPARAM wparam, HWND hwnd)
{
    TCHAR *st_name;
    int unit = 8;
    int autostart_index = -1;

    SuspendFullscreenModeKeep(hwnd);
    switch (wparam & 0xffff) {
      case IDM_ATTACH_8:
        unit = 8;
        break;
      case IDM_ATTACH_9:
        unit = 9;
        break;
      case IDM_ATTACH_10:
        unit = 10;
        break;
      case IDM_ATTACH_11:
        unit = 11;
        break;
    }
    if ((st_name = uilib_select_file_autostart(hwnd, TEXT("Attach disk image"),
        UILIB_FILTER_DISK | UILIB_FILTER_ZIP | UILIB_FILTER_ALL,
        UILIB_SELECTOR_TYPE_FILE_LOAD, UILIB_SELECTOR_STYLE_DISK,
        &autostart_index)) != NULL) {
        char *name;

        name = system_wcstombs_alloc(st_name);
        if (autostart_index >= 0) {
            if (autostart_autodetect(name, NULL, autostart_index,
                AUTOSTART_MODE_RUN) < 0)
                ui_error("Cannot autostart specified file.");
        } else {
            if (file_system_attach_disk(unit, name) < 0)
                ui_error("Cannot attach specified file");
        }
        system_wcstombs_free(name);
        lib_free(st_name);
    }
    ResumeFullscreenModeKeep(hwnd);
}

void ui_attach_tape_dialog(HWND hwnd)
{
    TCHAR *st_name;
    int autostart_index = -1;

    SuspendFullscreenModeKeep(hwnd);
    if ((st_name = uilib_select_file_autostart(hwnd, TEXT("Attach tape image"),
        UILIB_FILTER_TAPE | UILIB_FILTER_ZIP | UILIB_FILTER_ALL,
        UILIB_SELECTOR_TYPE_FILE_LOAD, UILIB_SELECTOR_STYLE_TAPE,
        &autostart_index)) != NULL) {
        char *name;

        name = system_wcstombs_alloc(st_name);
        if (autostart_index >= 0) {
            if (autostart_autodetect(name, NULL, autostart_index,
                AUTOSTART_MODE_RUN) < 0)
                ui_error("Cannot autostart specified file.");
        } else {
            if (tape_image_attach(1, name) < 0)
                ui_error("Cannot attach specified file");
        }
        system_wcstombs_free(name);
        lib_free(st_name);
    }
    ResumeFullscreenModeKeep(hwnd);
}

void ui_attach_autostart_dialog(HWND hwnd)
{
    TCHAR *st_name;
    int autostart_index = 0;

    if ((st_name = uilib_select_file_autostart(hwnd,
        TEXT("Autostart disk/tape image"),
        UILIB_FILTER_DISK | UILIB_FILTER_TAPE | UILIB_FILTER_ZIP
        | UILIB_FILTER_ALL, UILIB_SELECTOR_TYPE_FILE_LOAD,
        UILIB_SELECTOR_STYLE_DISK_AND_TAPE, &autostart_index)) != NULL) {
        char *name;

        name = system_wcstombs_alloc(st_name);
        if (autostart_autodetect(name, NULL, autostart_index,
            AUTOSTART_MODE_RUN) < 0)
            ui_error("Cannot autostart specified file.");
        system_wcstombs_free(name);
        lib_free(st_name);
    }
}

