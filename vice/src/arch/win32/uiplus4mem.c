/*
 * uiplus4mem.c - Implementation of PLUS4 memory settings dialog box.
 *
 * Written by
 *  Tibor Biczo <crown@axelero.hu>
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

#include <windows.h>

#include "plus4memhacks.h"
#include "res.h"
#include "resources.h"
#include "system.h"
#include "translate.h"
#include "types.h"
#include "uilib.h"
#include "winmain.h"

static int orig_ramsize;
static int set_ramsize;

static uilib_localize_dialog_param plus4_dialog_trans[] = {
    { 0, IDS_PLUS4_SETTINGS_CAPTION, -1 },
    { IDC_PLUS4_VIC20_MEMORY, IDS_PLUS4_VIC20_MEMORY, 0 },
    { IDC_SELECT_PLUS4_MEM_16, IDS_SELECT_PLUS4_MEM_16, 0 },
    { IDC_SELECT_PLUS4_MEM_32, IDS_SELECT_PLUS4_MEM_32, 0 },
    { IDC_SELECT_PLUS4_MEM_64, IDS_SELECT_PLUS4_MEM_64, 0 },
    { IDC_SELECT_PLUS4_MEM_256_CSORY, IDS_SELECT_PLUS4_MEM_256_CSORY, 0 },
    { IDC_SELECT_PLUS4_MEM_256_HANNES, IDS_SELECT_PLUS4_MEM_256_HANNES, 0 },
    { IDC_SELECT_PLUS4_MEM_1024_HANNES, IDS_SELECT_PLUS4_MEM_1M_HANNES, 0 },
    { IDC_SELECT_PLUS4_MEM_4096_HANNES, IDS_SELECT_PLUS4_MEM_4M_HANNES, 0 },
    { IDOK, IDS_OK, 0 },
    { IDCANCEL, IDS_CANCEL, 0 },
    { 0, 0, 0 }
};

static uilib_dialog_group plus4_main_group[] = {
    { IDC_PLUS4_VIC20_MEMORY, 1 },
    { IDC_SELECT_PLUS4_MEM_16, 1 },
    { IDC_SELECT_PLUS4_MEM_32, 1 },
    { IDC_SELECT_PLUS4_MEM_64, 1 },
    { IDC_SELECT_PLUS4_MEM_256_CSORY, 1 },
    { IDC_SELECT_PLUS4_MEM_256_HANNES, 1 },
    { IDC_SELECT_PLUS4_MEM_1024_HANNES, 1 },
    { IDC_SELECT_PLUS4_MEM_4096_HANNES, 1 },
    { 0, 0 }
};

static int move_buttons_group[] = {
    IDOK,
    IDCANCEL,
    0
};

static void init_dialog(HWND hwnd)
{
    int n, res, hack;
    int xstart;
    int xpos;
    RECT rect;

    /* translate all dialog items */
    uilib_localize_dialog(hwnd, plus4_dialog_trans);

    /* adjust the size of the elements in the main group */
    uilib_adjust_group_width(hwnd, plus4_main_group);

    /* get the min x of the 16kb element */
    uilib_get_element_min_x(hwnd, IDC_SELECT_PLUS4_MEM_16, &xstart);

    /* get the max x of the main group */
    uilib_get_group_max_x(hwnd, plus4_main_group, &xpos);

    /* resize and move the group box to the correct position */
    uilib_move_and_set_element_width(hwnd, IDC_PLUS4_VIC20_MEMORY, xstart - 10, xpos - xstart + 20);

    /* get the max x of the group box */
    uilib_get_element_max_x(hwnd, IDC_PLUS4_VIC20_MEMORY, &xpos);

    /* set the width of the dialog to 'surround' all the elements */
    GetWindowRect(hwnd, &rect);
    MoveWindow(hwnd, rect.left, rect.top, xpos + 20, rect.bottom - rect.top, TRUE);

    /* recenter the buttons in the newly resized dialog window */
    uilib_center_buttons(hwnd, move_buttons_group, 0);

    resources_get_int("RamSize", &res);
    switch (res) {
        case 16:
            n = IDC_SELECT_PLUS4_MEM_16;
            break;
        case 32:
            n = IDC_SELECT_PLUS4_MEM_32;
            break;
        case 4096:
            n = IDC_SELECT_PLUS4_MEM_4096_HANNES;
            break;
        case 1024:
            n = IDC_SELECT_PLUS4_MEM_1024_HANNES;
            break;
        case 256:
            resources_get_int("MemoryHack", &hack);
            if (hack == MEMORY_HACK_C256K) {
                n = IDC_SELECT_PLUS4_MEM_256_CSORY;
            } else {
                n = IDC_SELECT_PLUS4_MEM_256_HANNES;
            }
            break;
        case 64:
        default:
            n = IDC_SELECT_PLUS4_MEM_64;
            break;
    }
    if (res == 256 && hack != MEMORY_HACK_C256K) {
        res++;
    }
    orig_ramsize = set_ramsize = res;

    CheckRadioButton(hwnd, IDC_SELECT_PLUS4_MEM_16, IDC_SELECT_PLUS4_MEM_4096_HANNES, n);
}

static void end_dialog(void)
{
    if (orig_ramsize != set_ramsize) {
        if (set_ramsize == 257) {
            resources_set_int("MemoryHack", MEMORY_HACK_C256K);
        }
        if (set_ramsize == 256) {
            resources_set_int("MemoryHack", MEMORY_HACK_H256K);
        }
        if (set_ramsize == 1024) {
            resources_set_int("MemoryHack", MEMORY_HACK_H1024K);
        }
        if (set_ramsize == 4096) {
            resources_set_int("MemoryHack", MEMORY_HACK_H4096K);
        }
        if (set_ramsize < 256) {
            resources_set_int("RamSize", set_ramsize);
        }
    }
}

static INT_PTR CALLBACK dialog_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    int type;

    switch (msg) {
        case WM_INITDIALOG:
            system_init_dialog(hwnd);
            init_dialog(hwnd);
            return TRUE;
        case WM_COMMAND:
            type = LOWORD(wparam);
            switch (type) {
                case IDC_SELECT_PLUS4_MEM_16:
                    set_ramsize = 16;
                    break;
                case IDC_SELECT_PLUS4_MEM_32:
                    set_ramsize = 32;
                    break;
                case IDC_SELECT_PLUS4_MEM_64:
                    set_ramsize = 64;
                    break;
                case IDC_SELECT_PLUS4_MEM_256_HANNES:
                    set_ramsize = 256;
                    break;
                case IDC_SELECT_PLUS4_MEM_1024_HANNES:
                    set_ramsize = 1024;
                    break;
                case IDC_SELECT_PLUS4_MEM_4096_HANNES:
                    set_ramsize = 4096;
                    break;
                case IDC_SELECT_PLUS4_MEM_256_CSORY:
                    set_ramsize = 257;
                    break;
                case IDOK:
                    end_dialog();
                case IDCANCEL:
                    EndDialog(hwnd,0);
                    return TRUE;
            }
            break;
    }
    return FALSE;
}

void ui_plus4_memory_dialog(HWND hwnd)
{
    DialogBox(winmain_instance, MAKEINTRESOURCE(IDD_PLUS4_MEMORY_DIALOG), hwnd, dialog_proc);
}
