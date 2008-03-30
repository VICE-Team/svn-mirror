/*
 * uisnapshot.c - Implementation of the snapshot load/save dialogs.
 *
 * Written by
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
 *  Ettore Perazzoli (ettore@comm2000.it)
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

#include "drive.h"
#include "machine.h"
#include "resc64.h"
#include "resources.h"
#include "serial.h"
#include "ui.h"
#include "uidrive.h"
#include "uilib.h"
#include "winmain.h"

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

static BOOL CALLBACK ui_snapshot_save_dialog_proc(HWND hwnd, 
                                 UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch (msg) {
      case WM_INITDIALOG:
        init_dialog(hwnd);
        return TRUE;
      case WM_COMMAND:
        switch (LOWORD(wparam)) {
          case IDC_SNAPSHOT_SAVE_IMAGE:
            GetDlgItemText(hwnd, IDC_SNAPSHOT_SAVE_IMAGE, (LPSTR)image, 100);
            break;
          case IDC_TOGGLE_SNAPSHOT_SAVE_DISKS:
            save_disks = !save_disks; 
            break;
          case IDC_TOGGLE_SNAPSHOT_SAVE_ROMS:
            save_roms = !save_roms;
            break;
          default:
            return FALSE;
        }
        return TRUE;
      case WM_CLOSE:
        printf("Hurz!\n");
    }
    return FALSE;
}

void ui_snapshot_save_dialog(HWND hwnd)
{
    /* FIXME: Of course this is not the right window.  */
    PROPSHEETPAGE psp[1];
    PROPSHEETHEADER psh;
    int i;

    for (i = 0; i < 1; i++) {
        psp[i].dwSize = sizeof(PROPSHEETPAGE);
        psp[i].dwFlags = PSP_USETITLE /*| PSP_HASHELP*/ ;
        psp[i].hInstance = winmain_instance;
        psp[i].u1.pszTemplate = MAKEINTRESOURCE(IDD_SNAPSHOT_SAVE_DIALOG);
        psp[i].u2.pszIcon = NULL;
        psp[i].lParam = 0;
        psp[i].pfnCallback = NULL;
    }

    psp[0].pfnDlgProc = ui_snapshot_save_dialog_proc;
    psp[0].pszTitle = ":-)";

    psh.dwSize = sizeof(PROPSHEETHEADER);
    psh.dwFlags = PSH_PROPSHEETPAGE | PSH_NOAPPLYNOW;
    psh.hwndParent = hwnd;
    psh.hInstance = winmain_instance;
    psh.u1.pszIcon = NULL;
    psh.pszCaption = "Save snapshot image";
    psh.nPages = 1;
    psh.u2.nStartPage = 0;
    psh.u3.ppsp = psp;
    psh.pfnCallback = NULL;

    PropertySheet(&psh);
}

void ui_snapshot_load_dialog(HWND hwnd)
{
    char *s;
    if ((s = ui_select_file("Load snapshot image",
        "VICE snapshot files (*.vsf)\0*.vsf)\0"
        "All files (*.*)\0*.*\0", hwnd)) != NULL) {
         if (machine_read_snapshot(s) < 0)
             ui_error("Cannot read snapshot image");
         free(s);
    }
}

