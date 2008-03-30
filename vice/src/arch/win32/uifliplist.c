/*
 * uifliplist.c - Implementation of the fliplist dialogs.
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
#include <windows.h>
#include <tchar.h>

#include "fliplist.h"
#include "lib.h"
#include "system.h"
#include "ui.h"
#include "uilib.h"
#include "util.h"


void uifliplist_load_dialog(HWND hwnd)
{
    TCHAR *st_name;

    if ((st_name = uilib_select_file(hwnd, TEXT("Load flip list file"),
        UILIB_FILTER_FLIPLIST, UILIB_SELECTOR_TYPE_FILE_LOAD,
        UILIB_SELECTOR_STYLE_DEFAULT)) != NULL) {
        char *name;

        name = system_wcstombs_alloc(st_name);
        if (flip_load_list((unsigned int)-1, name, 0) != 0)
            ui_error("Cannot read flip list file");
        system_wcstombs_free(name);
        lib_free(st_name);
    }
}

void uifliplist_save_dialog(HWND hwnd)
{
    TCHAR *st_name;

    if ((st_name = uilib_select_file(hwnd, TEXT("Save flip list file"),
        UILIB_FILTER_FLIPLIST, UILIB_SELECTOR_TYPE_FILE_SAVE,
        UILIB_SELECTOR_STYLE_DEFAULT)) != NULL) {
        char *name;

        util_add_extension(&st_name, "vfl");

        name = system_wcstombs_alloc(st_name);
        if (flip_save_list((unsigned int)-1, name) != 0)
            ui_error("Cannot write flip list file");
        system_wcstombs_free(name);
        lib_free(st_name);
    }
}

