/*
 * uicbm2set.c - Implementation of CBM2 settings dialog box.
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

#include "c610mem.h"
#include "res.h"
#include "resources.h"
#include "ui.h"
#include "uicbm2set.h"
#include "uilib.h"
#include "utils.h"
#include "winmain.h"

/*
static void enable_controls_for_cbm2_settings(HWND hwnd, int type)
{
}
*/

static void init_dialog(HWND hwnd)
{
    int n, res;

    resources_get_value("Ram08", (resource_value_t *) &n);
    CheckDlgButton(hwnd, IDC_TOGGLE_CBMII_RAM08, 
                   n ? BST_CHECKED : BST_UNCHECKED);
    resources_get_value("Ram1", (resource_value_t *) &n);
    CheckDlgButton(hwnd, IDC_TOGGLE_CBMII_RAM1,
                   n ? BST_CHECKED : BST_UNCHECKED);
    resources_get_value("Ram2", (resource_value_t *) &n);
    CheckDlgButton(hwnd, IDC_TOGGLE_CBMII_RAM2,
                   n ? BST_CHECKED : BST_UNCHECKED);
    resources_get_value("Ram4", (resource_value_t *) &n);
    CheckDlgButton(hwnd, IDC_TOGGLE_CBMII_RAM4,
                   n ? BST_CHECKED : BST_UNCHECKED);
    resources_get_value("Ram6", (resource_value_t *) &n);
    CheckDlgButton(hwnd, IDC_TOGGLE_CBMII_RAM6,
                   n ? BST_CHECKED : BST_UNCHECKED);
    resources_get_value("RamC", (resource_value_t *) &n);
    CheckDlgButton(hwnd, IDC_TOGGLE_CBMII_RAMC,
                   n ? BST_CHECKED : BST_UNCHECKED);

    resources_get_value("RamSize", (resource_value_t *) &res);
    switch (res) {
      case 128:
        n = IDC_SELECT_CBMII_MEM_128;
        break;
      case 256:
        n = IDC_SELECT_CBMII_MEM_256;
        break;
      case 512:
        n = IDC_SELECT_CBMII_MEM_512;
        break;
      case 1024:
        n = IDC_SELECT_CBMII_MEM_1024;
        break;
    }
    CheckRadioButton(hwnd, IDC_SELECT_CBMII_MEM_128, IDC_SELECT_CBMII_MEM_1024,
                     n);

    resources_get_value("ModelLine", (resource_value_t *) &res);
    switch (res) {
      case 0:
        n = IDC_SELECT_CBMII_HW0;
        break;
      case 1:
        n = IDC_SELECT_CBMII_HW1;
        break;
      case 2:
        n = IDC_SELECT_CBMII_HW2;
        break;
    }
    CheckRadioButton(hwnd, IDC_SELECT_CBMII_HW0, IDC_SELECT_CBMII_HW2, n);
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
            case IDC_SELECT_CBMII_610:
              cbm2_set_model("610", NULL);
              break;
            case IDC_SELECT_CBMII_620:
              cbm2_set_model("620", NULL);
              break;
            case IDC_SELECT_CBMII_620P:
              cbm2_set_model("620+", NULL);
              break;
            case IDC_SELECT_CBMII_710:
              cbm2_set_model("710", NULL);
              break;
            case IDC_SELECT_CBMII_720:
              cbm2_set_model("720", NULL);
              break;
            case IDC_SELECT_CBMII_720P:
              cbm2_set_model("720+", NULL);
              break;

            case IDC_SELECT_CBMII_MEM_128:
              resources_set_value("RamSize", (resource_value_t) 128);
              break;
            case IDC_SELECT_CBMII_MEM_256:
              resources_set_value("RamSize", (resource_value_t) 256);
              break;
            case IDC_SELECT_CBMII_MEM_512:
              resources_set_value("RamSize", (resource_value_t) 512);
              break;
            case IDC_SELECT_CBMII_MEM_1024:
              resources_set_value("RamSize", (resource_value_t) 1024);
              break;
            case IDC_SELECT_CBMII_HW0:
              resources_set_value("ModelLine", (resource_value_t) 0);
              break;
            case IDC_SELECT_CBMII_HW1:
              resources_set_value("ModelLine", (resource_value_t) 1);
              break;
            case IDC_SELECT_CBMII_HW2:
              resources_set_value("ModelLine", (resource_value_t) 2);
              break;
            case IDC_TOGGLE_CBMII_RAM08:
              {
                  int n;
                  resources_get_value("Ram08", (resource_value_t *) &n);
                  resources_set_value("Ram08", (resource_value_t) !n);
                  break;
              }
            case IDC_TOGGLE_CBMII_RAM1:
              {
                  int n;
                  resources_get_value("Ram1", (resource_value_t *) &n);
                  resources_set_value("Ram1", (resource_value_t) !n);
                  break;
              }
            case IDC_TOGGLE_CBMII_RAM2:
              {
                  int n;
                  resources_get_value("Ram2", (resource_value_t *) &n);
                  resources_set_value("Ram2", (resource_value_t) !n);
                  break;
              }
            case IDC_TOGGLE_CBMII_RAM4:
              {
                  int n;
                  resources_get_value("Ram4", (resource_value_t *) &n);
                  resources_set_value("Ram4", (resource_value_t) !n);
                  break;
              }
            case IDC_TOGGLE_CBMII_RAM6:
              {
                  int n;
                  resources_get_value("Ram6", (resource_value_t *) &n);
                  resources_set_value("Ram6", (resource_value_t) !n);
                  break;
              }
            case IDC_TOGGLE_CBMII_RAMC:
              {
                  int n;
                  resources_get_value("RamC", (resource_value_t *) &n);
                  resources_set_value("RamC", (resource_value_t) !n);
                  break;
              }
          }
          return TRUE;
    }
    return FALSE;
}

void ui_cbm2_settings_dialog(HWND hwnd)
{
    PROPSHEETPAGE psp[4];
    PROPSHEETHEADER psh;
    int i;

    for (i = 0; i < 2; i++) {
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
    psp[1].pszTitle = "Memory";

    psp[0].u1.pszTemplate = MAKEINTRESOURCE(IDD_CBMII_SETTINGS_MODEL_DIALOG);
    psp[1].u1.pszTemplate = MAKEINTRESOURCE(IDD_CBMII_SETTINGS_IO_DIALOG);

    psh.dwSize = sizeof(PROPSHEETHEADER);
    psh.dwFlags = PSH_PROPSHEETPAGE | PSH_NOAPPLYNOW;
    psh.hwndParent = hwnd;
    psh.hInstance = winmain_instance;
    psh.u1.pszIcon = NULL;
    psh.pszCaption = "CBM2 settings";
    psh.nPages = 2;
    psh.u2.nStartPage = 0;
    psh.u3.ppsp = psp;
    psh.pfnCallback = NULL;

    PropertySheet(&psh);
}

