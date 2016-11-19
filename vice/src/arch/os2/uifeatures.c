/*
 * uifeatures.c - VICE features output for Vice/2
 *
 * Written by
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
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

#define INCL_DOSPROCESS
#define INCL_WINDIALOGS // WinProcessDlg

#include "vice.h"

#include <os2.h>
#include <string.h>
#include <stdlib.h>

#include "dialogs.h"
#include "lib.h"
#include "vicefeatures.h"

void ui_show_features(void *arg)
{
    int chars;   // maximum area could be shown
    HWND hwnd;
    feature_list_t *list = vice_get_feature_list();

    //
    // open dialog
    //
    hwnd = cmdopt_dialog((HWND)arg);

    if (!hwnd) {
        return;
    }

    while (list->symbol) {
        WinSendMsg(hwnd, WM_INSERT, list->isdefined ? "yes " : "no  ", (void*)TRUE);
        WinSendMsg(hwnd, WM_INSERT, list->descr, (void*)TRUE);
        WinSendMsg(hwnd, WM_INSERT, list->symbol, (void*)TRUE);
        WinSendMsg(hwnd, WM_INSERT, "", (void*)TRUE);
        ++list;
    }

    //
    // MAINLOOP
    //
    WinProcessDlg(hwnd);

    //
    // WinProcessDlg() does NOT destroy the window on return! Do it here,
    // otherwise the window procedure won't ever get a WM_DESTROY,
    // which we may want :-)
    //
    WinDestroyWindow(hwnd);
}
