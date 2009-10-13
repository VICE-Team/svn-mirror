/*
 * uipetset.c - Implementation of PET settings dialog box.
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

#include <windows.h>
#include <prsht.h>

#ifndef DUMMYUNIONNAME
#define DUMMYUNIONNAME  u1
#endif

#include "intl.h"
#include "pets.h"
#include "res.h"
#include "resources.h"
#include "system.h"
#include "translate.h"
#include "uilib.h"
#include "uipetset.h"
#include "winmain.h"

#ifdef _WIN64
#define _ANONYMOUS_UNION
#endif

static uilib_localize_dialog_param model_dialog_trans[] = {
    {IDC_PET_MACHINE_DEFAULTS, IDS_PET_MACHINE_DEFAULTS, 0},
    {IDC_PET_MEMORY, IDS_PET_MEMORY, 0},
    {0, 0, 0}
};

static uilib_localize_dialog_param parent_dialog_trans[] = {
    {IDOK, IDS_OK, 0},
    {IDCANCEL, IDS_CANCEL, 0},
    {0, 0, 0}
};

static uilib_dialog_group model_main_group[] = {
    {IDC_PET_MACHINE_DEFAULTS, 1},
    {IDC_PET_MEMORY, 1},
    {0, 0}
};

static uilib_dialog_group model_left_group[] = {
    {IDC_PET_MACHINE_DEFAULTS, 0},
    {IDC_SELECT_PET_2001_8N, 0},
    {IDC_SELECT_PET_3008, 0},
    {IDC_SELECT_PET_3032, 0},
    {IDC_SELECT_PET_3032B, 0},
    {IDC_SELECT_PET_4016, 0},
    {IDC_SELECT_PET_4032, 0},
    {IDC_SELECT_PET_4032B, 0},
    {IDC_SELECT_PET_8032, 0},
    {IDC_SELECT_PET_8096, 0},
    {IDC_SELECT_PET_8296, 0},
    {IDC_SELECT_PET_SUPER, 0},
    {0, 0}
};

static uilib_dialog_group model_left_move_group[] = {
    {IDC_SELECT_PET_2001_8N, 0},
    {IDC_SELECT_PET_3008, 0},
    {IDC_SELECT_PET_3032, 0},
    {IDC_SELECT_PET_3032B, 0},
    {IDC_SELECT_PET_4016, 0},
    {IDC_SELECT_PET_4032, 0},
    {IDC_SELECT_PET_4032B, 0},
    {IDC_SELECT_PET_8032, 0},
    {IDC_SELECT_PET_8096, 0},
    {IDC_SELECT_PET_8296, 0},
    {IDC_SELECT_PET_SUPER, 0},
    {0, 0}
};

static uilib_dialog_group model_right_group[] = {
    {IDC_PET_MEMORY, 0},
    {IDC_SELECT_PET_MEM4K, 0},
    {IDC_SELECT_PET_MEM8K, 0},
    {IDC_SELECT_PET_MEM16K, 0},
    {IDC_SELECT_PET_MEM32K, 0},
    {IDC_SELECT_PET_MEM96K, 0},
    {IDC_SELECT_PET_MEM128K, 0},
    {0, 0}
};

static uilib_dialog_group model_right_move_group[] = {
    {IDC_SELECT_PET_MEM4K, 0},
    {IDC_SELECT_PET_MEM8K, 0},
    {IDC_SELECT_PET_MEM16K, 0},
    {IDC_SELECT_PET_MEM32K, 0},
    {IDC_SELECT_PET_MEM96K, 0},
    {IDC_SELECT_PET_MEM128K, 0},
    {0, 0}
};

static void init_model_dialog(HWND hwnd)
{
    int n, res;
    HWND parent_hwnd;
    int xpos;
    int xstart;

    parent_hwnd = GetParent(hwnd);

    /* translate all dialog items */
    uilib_localize_dialog(hwnd, model_dialog_trans);

    /* translate all dialog items of the parent */
    uilib_localize_dialog(parent_hwnd, parent_dialog_trans);

    /* adjust the size of the elements in the main group */
    uilib_adjust_group_width(hwnd, model_main_group);

    /* get the min x of the left move group */
    uilib_get_group_min_x(hwnd, model_left_move_group, &xstart);

    /* get the max x of the left group */
    uilib_get_group_max_x(hwnd, model_left_group, &xpos);

    /* move and resize the left group element */
    uilib_move_and_set_element_width(hwnd, IDC_PET_MACHINE_DEFAULTS, xstart - 10, xpos - xstart + 20);

    /* move the right group element */
    uilib_move_element(hwnd, IDC_PET_MEMORY, xpos + 20);

    /* move the right move group to the correct position */
    uilib_move_group(hwnd, model_right_move_group, xpos + 30);

    xstart = xpos + 30;

    /* get the max x of the right group */
    uilib_get_group_max_x(hwnd, model_right_group, &xpos);

    /* move and resize the right group element */
    uilib_move_and_set_element_width(hwnd, IDC_PET_MEMORY, xstart - 10, xpos - xstart + 20);

    resources_get_int("RamSize", &res);
    switch (res) {
      case 4:
        n = IDC_SELECT_PET_MEM4K;
        break;
      case 8:
        n = IDC_SELECT_PET_MEM8K;
        break;
      case 16:
        n = IDC_SELECT_PET_MEM16K;
        break;
      case 32:
        n = IDC_SELECT_PET_MEM32K;
        break;
      case 96:
        n = IDC_SELECT_PET_MEM96K;
        break;
      case 128:
        n = IDC_SELECT_PET_MEM128K;
        break;
    }
    CheckRadioButton(hwnd, IDC_SELECT_PET_MEM4K, IDC_SELECT_PET_MEM128K, n);
}

static void init_io_dialog(HWND hwnd)
{
    int n, res;

    resources_get_int("Crtc", &n);
    CheckDlgButton(hwnd, IDC_TOGGLE_PET_CRTC,
                   n ? BST_CHECKED : BST_UNCHECKED);

    resources_get_int("VideoSize", &res);
    switch (res) {
      case 0:
        n = IDC_SELECT_PET_VIDEO_AUTO;
        break;
      case 40:
        n = IDC_SELECT_PET_VIDEO_40;
        break;
      case 80:
        n = IDC_SELECT_PET_VIDEO_80;
        break;
    }
    CheckRadioButton(hwnd, IDC_SELECT_PET_VIDEO_AUTO, IDC_SELECT_PET_VIDEO_80,
                     n);

    resources_get_int("IOSize", &res);
    switch (res) {
      case 0x100:
        n = IDC_SELECT_PET_IO256;
        break;
      case 0x800:
        n = IDC_SELECT_PET_IO2K;
        break;
    }
    CheckRadioButton(hwnd, IDC_SELECT_PET_IO2K, IDC_SELECT_PET_IO256, n);

    resources_get_int("KeymapIndex", &res);
    switch (res) {
      case 2:
        n = IDC_SELECT_PET_KEYB_GRAPHICS;
        break;
      case 0:
        n = IDC_SELECT_PET_KEYB_BUSINESS;
        break;
    }
    CheckRadioButton(hwnd, IDC_SELECT_PET_KEYB_GRAPHICS,
                     IDC_SELECT_PET_KEYB_BUSINESS, n);
}

static void init_superpet_io_dialog(HWND hwnd)
{
    int n, res;

    resources_get_int("SuperPET", &n);
    CheckDlgButton(hwnd, IDC_TOGGLE_PET_SUPER_IO_ENABLE,
                   n ? BST_CHECKED : BST_UNCHECKED);
}

static void init_pet8296_dialog(HWND hwnd)
{
    int n, res;

    resources_get_int("Ram9", &n);
    CheckDlgButton(hwnd, IDC_TOGGLE_PET_8296_RAM9,
                   n ? BST_CHECKED : BST_UNCHECKED);
    resources_get_int("RamA", &n);
    CheckDlgButton(hwnd, IDC_TOGGLE_PET_8296_RAMA,
                   n ? BST_CHECKED : BST_UNCHECKED);

}

static INT_PTR CALLBACK model_dialog_proc(HWND hwnd, UINT msg,
                                    WPARAM wparam, LPARAM lparam)
{
    int type;

    switch (msg) {
      case WM_INITDIALOG:
        init_model_dialog(hwnd);
        return TRUE;
      case WM_COMMAND:
          type = LOWORD(wparam);
          switch (type) {
            case IDC_SELECT_PET_2001_8N:
              pet_set_model("2001", NULL);
              break;
            case IDC_SELECT_PET_3008:
              pet_set_model("3008", NULL);
              break;
            case IDC_SELECT_PET_3016:
              pet_set_model("3016", NULL);
              break;
            case IDC_SELECT_PET_3032:
              pet_set_model("3032", NULL);
              break;
            case IDC_SELECT_PET_3032B:
              pet_set_model("3032B", NULL);
              break;
            case IDC_SELECT_PET_4016:
              pet_set_model("4016", NULL);
              break;
            case IDC_SELECT_PET_4032:
              pet_set_model("4032", NULL);
              break;
            case IDC_SELECT_PET_4032B:
              pet_set_model("4032B", NULL);
              break;
            case IDC_SELECT_PET_8032:
              pet_set_model("8032", NULL);
              break;
            case IDC_SELECT_PET_8096:
              pet_set_model("8096", NULL);
              break;
            case IDC_SELECT_PET_8296:
              pet_set_model("8296", NULL);
              break;
            case IDC_SELECT_PET_SUPER:
              pet_set_model("SuperPET", NULL);
              break;
            case IDC_SELECT_PET_MEM4K:
              resources_set_int("RamSize", 4);
              break;
            case IDC_SELECT_PET_MEM8K:
              resources_set_int("RamSize", 8);
              break;
            case IDC_SELECT_PET_MEM16K:
              resources_set_int("RamSize", 16);
              break;
            case IDC_SELECT_PET_MEM32K:
              resources_set_int("RamSize", 32);
              break;
            case IDC_SELECT_PET_MEM96K:
              resources_set_int("RamSize", 96);
              break;
            case IDC_SELECT_PET_MEM128K:
              resources_set_int("RamSize", 128);
              break;
          }
          return TRUE;
    }
    return FALSE;
}

static INT_PTR CALLBACK io_dialog_proc(HWND hwnd, UINT msg,
                                    WPARAM wparam, LPARAM lparam)
{
    int type;

    switch (msg) {
      case WM_INITDIALOG:
        init_io_dialog(hwnd);
        return TRUE;
      case WM_COMMAND:
          type = LOWORD(wparam);
          switch (type) {
            case IDC_SELECT_PET_VIDEO_AUTO:
              resources_set_int("VideoSize", 0);
              break;
            case IDC_SELECT_PET_VIDEO_40:
              resources_set_int("VideoSize", 40);
              break;
            case IDC_SELECT_PET_VIDEO_80:
              resources_set_int("VideoSize", 80);
              break;
            case IDC_SELECT_PET_IO256:
              resources_set_int("IOSize", 0x100);
              break;
            case IDC_SELECT_PET_IO2K:
              resources_set_int("IOSize", 0x800);
              break;
            case IDC_SELECT_PET_KEYB_GRAPHICS:
              resources_set_int("KeymapIndex", 2);
              break;
            case IDC_SELECT_PET_KEYB_BUSINESS:
              resources_set_int("KeymapIndex", 0);
              break;
            case IDC_TOGGLE_PET_CRTC:
              resources_toggle("Crtc", NULL);
              break;
          }
          return TRUE;
    }
    return FALSE;
}

static INT_PTR CALLBACK superpet_io_dialog_proc(HWND hwnd, UINT msg,
                                    WPARAM wparam, LPARAM lparam)
{
    int type;

    switch (msg) {
      case WM_INITDIALOG:
        init_superpet_io_dialog(hwnd);
        return TRUE;
      case WM_COMMAND:
          type = LOWORD(wparam);
          switch (type) {
            case IDC_TOGGLE_PET_SUPER_IO_ENABLE:
              resources_toggle("SuperPET", NULL);
              break;
          }
          return TRUE;
    }
    return FALSE;
}

static INT_PTR CALLBACK pet8296_dialog_proc(HWND hwnd, UINT msg,
                                    WPARAM wparam, LPARAM lparam)
{
    int type;

    switch (msg) {
      case WM_INITDIALOG:
        init_pet8296_dialog(hwnd);
        return TRUE;
      case WM_COMMAND:
          type = LOWORD(wparam);
          switch (type) {
            case IDC_TOGGLE_PET_8296_RAM9:
              resources_toggle("Ram9", NULL);
              break;
            case IDC_TOGGLE_PET_8296_RAMA:
              resources_toggle("RamA", NULL);
              break;
          }
          return TRUE;
    }
    return FALSE;
}

void ui_pet_settings_dialog(HWND hwnd)
{
    PROPSHEETPAGE psp[4];
    PROPSHEETHEADER psh;
    int i;

    for (i = 0; i < 4; i++) {
        psp[i].dwSize = sizeof(PROPSHEETPAGE);
        psp[i].dwFlags = PSP_USETITLE /*| PSP_HASHELP*/ ;
        psp[i].hInstance = winmain_instance;
#ifdef _ANONYMOUS_UNION
        psp[i].pszIcon = NULL;
#else
        psp[i].u2.pszIcon = NULL;
#endif
        psp[i].lParam = 0;
        psp[i].pfnCallback = NULL;
    }

    psp[0].pfnDlgProc = model_dialog_proc;
    psp[0].pszTitle = translate_text(IDS_MODEL);
    psp[1].pfnDlgProc = io_dialog_proc;
    psp[1].pszTitle = translate_text(IDS_INPUT_OUTPUT);
    psp[2].pfnDlgProc = superpet_io_dialog_proc;
    psp[2].pszTitle = TEXT("Super PET");
    psp[3].pfnDlgProc = pet8296_dialog_proc;
    psp[3].pszTitle = TEXT("8296 PET");

#ifdef _ANONYMOUS_UNION
    psp[0].pszTemplate = MAKEINTRESOURCE(IDD_PET_SETTINGS_MODEL_DIALOG);
    psp[1].pszTemplate = MAKEINTRESOURCE(translate_res(IDD_PET_SETTINGS_IO_DIALOG));
    psp[2].pszTemplate = MAKEINTRESOURCE(translate_res(IDD_PET_SETTINGS_SUPER_DIALOG));
    psp[3].pszTemplate = MAKEINTRESOURCE(translate_res(IDD_PET_SETTINGS_8296_DIALOG));
#else
    psp[0].DUMMYUNIONNAME.pszTemplate
        = MAKEINTRESOURCE(IDD_PET_SETTINGS_MODEL_DIALOG);
    psp[1].DUMMYUNIONNAME.pszTemplate
        = MAKEINTRESOURCE(translate_res(IDD_PET_SETTINGS_IO_DIALOG));
    psp[2].DUMMYUNIONNAME.pszTemplate
        = MAKEINTRESOURCE(translate_res(IDD_PET_SETTINGS_SUPER_DIALOG));
    psp[3].DUMMYUNIONNAME.pszTemplate
        = MAKEINTRESOURCE(translate_res(IDD_PET_SETTINGS_8296_DIALOG));
#endif

    psh.dwSize = sizeof(PROPSHEETHEADER);
    psh.dwFlags = PSH_PROPSHEETPAGE | PSH_NOAPPLYNOW;
    psh.hwndParent = hwnd;
    psh.hInstance = winmain_instance;
    psh.pszCaption = translate_text(IDS_PET_SETTINGS);
    psh.nPages = 4;
#ifdef _ANONYMOUS_UNION
    psh.pszIcon = NULL;
    psh.nStartPage = 0;
    psh.ppsp = psp;
#else
    psh.DUMMYUNIONNAME.pszIcon = NULL;
    psh.u2.nStartPage = 0;
    psh.u3.ppsp = psp;
#endif
    psh.pfnCallback = NULL;

    system_psh_settings(&psh);
    PropertySheet(&psh);
}

