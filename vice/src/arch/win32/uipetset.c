/*
 * uipetset.c - Implementation of PET settings dialog box.
 *
 * Written by
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
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

#include "pets.h"
#include "res.h"
#include "resources.h"
#include "ui.h"
#include "uilib.h"
#include "uipetset.h"
#include "utils.h"
#include "winmain.h"

static void enable_controls_for_pet_settings(HWND hwnd, int type)
{
}

static void init_dialog(HWND hwnd)
{
    int n, res;

    resources_get_value("Crtc", (resource_value_t *) &n);
    CheckDlgButton(hwnd, IDC_TOGGLE_PET_CRTC, 
                   n ? BST_CHECKED : BST_UNCHECKED);
    resources_get_value("SuperPET", (resource_value_t *) &n);
    CheckDlgButton(hwnd, IDC_TOGGLE_PET_SUPER_IO_ENABLE,
                   n ? BST_CHECKED : BST_UNCHECKED);
    resources_get_value("Ram9", (resource_value_t *) &n);
    CheckDlgButton(hwnd, IDC_TOGGLE_PET_8296_RAM9,
                   n ? BST_CHECKED : BST_UNCHECKED);
    resources_get_value("RamA", (resource_value_t *) &n);
    CheckDlgButton(hwnd, IDC_TOGGLE_PET_8296_RAMA,
                   n ? BST_CHECKED : BST_UNCHECKED);

    resources_get_value("RamSize", (resource_value_t *) &res);
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

    resources_get_value("VideoSize", (resource_value_t *) &res);
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

    resources_get_value("IOSize", (resource_value_t *) &res);
    switch (res) {
      case 0x100:
        n = IDC_SELECT_PET_IO256;
        break;
      case 0x800:
        n = IDC_SELECT_PET_IO2K;
        break;
    }
    CheckRadioButton(hwnd, IDC_SELECT_PET_IO2K, IDC_SELECT_PET_IO256, n);

    resources_get_value("KeymapIndex", (resource_value_t *) &res);
    switch (res) {
      case 1:
        n = IDC_SELECT_PET_KEYB_GRAPHICS;
        break;
      case 0:
        n = IDC_SELECT_PET_KEYB_BUSINESS;
        break;
    }
    CheckRadioButton(hwnd, IDC_SELECT_PET_KEYB_GRAPHICS,
                     IDC_SELECT_PET_KEYB_BUSINESS, n);
}

static BOOL CALLBACK dialog_proc(HWND hwnd, UINT msg,
                                 WPARAM wparam, LPARAM lparam)
{
    int type;

    switch (msg) {
      case WM_INITDIALOG:
        init_dialog(hwnd);
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
              resources_set_value("RamSize", (resource_value_t) 4);
              break;
            case IDC_SELECT_PET_MEM8K:
              resources_set_value("RamSize", (resource_value_t) 8);
              break;
            case IDC_SELECT_PET_MEM16K:
              resources_set_value("RamSize", (resource_value_t) 16);
              break;
            case IDC_SELECT_PET_MEM32K:
              resources_set_value("RamSize", (resource_value_t) 32);
              break;
            case IDC_SELECT_PET_MEM96K:
              resources_set_value("RamSize", (resource_value_t) 96);
              break;
            case IDC_SELECT_PET_MEM128K:
              resources_set_value("RamSize", (resource_value_t) 128);
              break;
            case IDC_SELECT_PET_VIDEO_AUTO:
              resources_set_value("VideoSize", (resource_value_t) 0);
              break;
            case IDC_SELECT_PET_VIDEO_40:
              resources_set_value("VideoSize", (resource_value_t) 40);
              break;
            case IDC_SELECT_PET_VIDEO_80:
              resources_set_value("VideoSize", (resource_value_t) 80);
              break;
            case IDC_SELECT_PET_IO256:
              resources_set_value("IOSize", (resource_value_t) 0x100);
              break;
            case IDC_SELECT_PET_IO2K:
              resources_set_value("IOSize", (resource_value_t) 0x800);
              break;
            case IDC_SELECT_PET_KEYB_GRAPHICS:
              resources_set_value("KeymapIndex", (resource_value_t) 1);
              break;
            case IDC_SELECT_PET_KEYB_BUSINESS:
              resources_set_value("KeymapIndex", (resource_value_t) 0);
              break;
            case IDC_TOGGLE_PET_CRTC:
              {
                  int n;
                  resources_get_value("Crtc", (resource_value_t *) &n);
                  resources_set_value("Crtc", (resource_value_t) !n);
                  break;
              }
            case IDC_TOGGLE_PET_SUPER_IO_ENABLE:
              {
                  int n;
                  resources_get_value("SuperPET", (resource_value_t *) &n);
                  resources_set_value("SuperPET", (resource_value_t) !n);
                  break;
              }
            case IDC_TOGGLE_PET_8296_RAM9:
              {
                  int n;
                  resources_get_value("Ram9", (resource_value_t *) &n);
                  resources_set_value("Ram9", (resource_value_t) !n);
                  break;
              }
            case IDC_TOGGLE_PET_8296_RAMA:
              {
                  int n;
                  resources_get_value("RamA", (resource_value_t *) &n);
                  resources_set_value("RamA", (resource_value_t) !n);
                  break;
              }
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
        psp[i].u2.pszIcon = NULL;
        psp[i].lParam = 0;
        psp[i].pfnCallback = NULL;
    }

    psp[0].pfnDlgProc = dialog_proc;
    psp[0].pszTitle = "Model";
    psp[1].pfnDlgProc = dialog_proc;
    psp[1].pszTitle = "Input/Output";
    psp[2].pfnDlgProc = dialog_proc;
    psp[2].pszTitle = "Super PET";
    psp[3].pfnDlgProc = dialog_proc;
    psp[3].pszTitle = "8296 PET";

    psp[0].u1.pszTemplate = MAKEINTRESOURCE(IDD_PET_SETTINGS_MODEL_DIALOG);
    psp[1].u1.pszTemplate = MAKEINTRESOURCE(IDD_PET_SETTINGS_IO_DIALOG);
    psp[2].u1.pszTemplate = MAKEINTRESOURCE(IDD_PET_SETTINGS_SUPER_DIALOG);
    psp[3].u1.pszTemplate = MAKEINTRESOURCE(IDD_PET_SETTINGS_8296_DIALOG);

    psh.dwSize = sizeof(PROPSHEETHEADER);
    psh.dwFlags = PSH_PROPSHEETPAGE | PSH_NOAPPLYNOW;
    psh.hwndParent = hwnd;
    psh.hInstance = winmain_instance;
    psh.u1.pszIcon = NULL;
    psh.pszCaption = "PET settings";
    psh.nPages = 4;
    psh.u2.nStartPage = 0;
    psh.u3.ppsp = psp;
    psh.pfnCallback = NULL;

    PropertySheet(&psh);
}

