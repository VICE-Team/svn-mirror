/*
 * uiattach.c - Implementation of the device manager dialog box.
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
 *  Tibor Biczo (crown@mail.matav.hu)
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

#ifdef _MSC_VER
#include <shlobj.h>
#endif

#include "attach.h"
#include "autostart.h"
#include "res.h"
#include "resources.h"
#include "serial.h"
#include "ui.h"
#include "uiattach.h"
#include "uilib.h"
#include "winmain.h"

static void enable_controls_for_disk_device_type(HWND hwnd, int type)
{
    EnableWindow(GetDlgItem(hwnd, IDC_DISKIMAGE),
                 type == IDC_SELECTDISK);
    EnableWindow(GetDlgItem(hwnd, IDC_BROWSEDISK),
                 type == IDC_SELECTDISK);
    EnableWindow(GetDlgItem(hwnd, IDC_AUTOSTART),
                 type == IDC_SELECTDISK);
    EnableWindow(GetDlgItem(hwnd, IDC_DIR),
                 type == IDC_SELECTDIR);
    EnableWindow(GetDlgItem(hwnd, IDC_BROWSEDIR),
                 type == IDC_SELECTDIR);
    EnableWindow(GetDlgItem(hwnd, IDC_TOGGLE_READP00),
                 type == IDC_SELECTDIR);
    EnableWindow(GetDlgItem(hwnd, IDC_TOGGLE_WRITEP00),
                 type == IDC_SELECTDIR);
    EnableWindow(GetDlgItem(hwnd, IDC_TOGGLE_HIDENONP00),
                 type == IDC_SELECTDIR);
}

static void init_dialog(HWND hwnd, int num)
{
    const char *disk_image, *dir;
    int n;
    char tmp[256];

    disk_image = serial_get_file_name(num);
    SetDlgItemText(hwnd, IDC_DISKIMAGE, disk_image != NULL ? disk_image : "");

    sprintf(tmp, "FSDevice%dDir", num);
    resources_get_value(tmp, (resource_value_t *) &dir);
    SetDlgItemText(hwnd, IDC_DIR, dir != NULL ? dir : "");

    sprintf(tmp, "FSDevice%dConvertP00", num);
    resources_get_value(tmp, (resource_value_t *) &n);
    CheckDlgButton(hwnd, IDC_TOGGLE_READP00, n ? BST_CHECKED : BST_UNCHECKED);

    sprintf(tmp, "FSDevice%dSaveP00", num);
    resources_get_value(tmp, (resource_value_t *) &n);
    CheckDlgButton(hwnd, IDC_TOGGLE_WRITEP00, n ? BST_CHECKED : BST_UNCHECKED);

    sprintf(tmp, "FSDevice%dHideCBMFiles", num);
    resources_get_value(tmp, (resource_value_t *) &n);
    CheckDlgButton(hwnd, IDC_TOGGLE_HIDENONP00, n ? BST_CHECKED : BST_UNCHECKED);

    if (disk_image != NULL) {
        n = IDC_SELECTDISK;
    } else if (dir != NULL) {
        n = IDC_SELECTDIR;
    } else {
        n = IDC_SELECTNONE;
    }
    CheckRadioButton(hwnd, IDC_SELECTDISK, IDC_SELECTDIR, n);
    enable_controls_for_disk_device_type(hwnd, n);
}

static BOOL CALLBACK dialog_proc(int num, HWND hwnd, UINT msg,
                                 WPARAM wparam, LPARAM lparam)
{
    switch (msg) {
      case WM_INITDIALOG:
        init_dialog(hwnd, num);
        return TRUE;
      case WM_COMMAND:
          switch (LOWORD(wparam)) {
            case IDC_SELECTDIR:
            case IDC_SELECTDISK:
            case IDC_SELECTNONE:
              enable_controls_for_disk_device_type(hwnd,
                                                   LOWORD(wparam));
              break;
            case IDC_BROWSEDISK:
              {
                  char *s;
                  if ((s = ui_select_file("Attach disk image",
                      "Disk image files (*.d64;*.d71;*.d81;*.g64;*.g41;*.x64)\0*.d64;*.d71;*.d81;*.g64;*.g41;*.x64\0"
                      "All files (*.*)\0*.*\0", hwnd)) != NULL) {
                      SetDlgItemText(hwnd, IDC_DISKIMAGE, s);
                      if (file_system_attach_disk(num, s) < 0)
                          ui_error("Cannot attach specified file");
                      free(s);
                  }
              }
              break;
            case IDC_AUTOSTART:
              {
                  char *s;
                  if ((s = ui_select_file("Autostart disk image",
                      "Disk image files (*.d64;*.d71;*.d81;*.g64;*.g41;*.x64)\0*.d64;*.d71;*.d81;*.g64;*.g41;*.x64\0"
                      "All files (*.*)\0*.*\0", hwnd)) != NULL) {
                      SetDlgItemText(hwnd, IDC_DISKIMAGE, s);
                      if (autostart_autodetect(s, "*") < 0)
                          ui_error("Cannot autostart specified file.");
                      free(s);
                  }
              }
              break;
            case IDC_BROWSEDIR:
              {
                  BROWSEINFO    bi;
                  char          s[MAX_PATH];
                  LPITEMIDLIST  idlist;

                  bi.hwndOwner=hwnd;
                  bi.pidlRoot=NULL;
                  bi.pszDisplayName=s;
                  bi.lpszTitle="Select file system directory";
                  bi.ulFlags=0;
                  bi.lpfn=NULL;
                  bi.lParam=0;
                  bi.iImage=0;
                  if ((idlist=SHBrowseForFolder(&bi))!=NULL) {
                      char tmp[256];
                      SHGetPathFromIDList(idlist,s);
                      LocalFree(idlist);
                      SetDlgItemText(hwnd, IDC_DIR, s);
                      sprintf(tmp, "FSDevice%dDir", num);
                      resources_set_value(tmp, (resource_value_t) s);
                  }
              }
              break;
            case IDC_TOGGLE_READP00:
              {
                  int n;
                  char tmp[256];
                  sprintf(tmp, "FSDevice%dConvertP00", num);
                  resources_get_value(tmp, (resource_value_t *) &n);
                  resources_set_value(tmp, (resource_value_t) !n);
                  break;
              }
            case IDC_TOGGLE_WRITEP00:
              {
                  int n;
                  char tmp[256];
                  sprintf(tmp, "FSDevice%dSaveP00", num);
                  resources_get_value(tmp, (resource_value_t *) &n);
                  resources_set_value(tmp, (resource_value_t) !n);
                  break;
              }
            case IDC_TOGGLE_HIDENONP00:
              {
                  int n;
                  char tmp[256];
                  sprintf(tmp, "FSDevice%dHideCBMFiles", num);
                  resources_get_value(tmp, (resource_value_t *) &n);
                  resources_set_value(tmp, (resource_value_t) !n);
                  break;
              }
          }
          return TRUE;
    }
    return FALSE;
}

#define _CALLBACK(num)                                            \
static BOOL CALLBACK callback_##num(HWND dialog, UINT msg,        \
                                    WPARAM wparam, LPARAM lparam) \
{                                                                 \
    return dialog_proc(num, dialog, msg, wparam, lparam);         \
}

_CALLBACK(8)
_CALLBACK(9)
_CALLBACK(10)
_CALLBACK(11)

void ui_attach_dialog(HWND hwnd)
{
    PROPSHEETPAGE psp[4];
    PROPSHEETHEADER psh;
    int i;

    for (i = 0; i < 4; i++) {
        psp[i].dwSize = sizeof(PROPSHEETPAGE);
        psp[i].dwFlags = PSP_USETITLE /*| PSP_HASHELP*/ ;
        psp[i].hInstance = winmain_instance;
#ifdef HAVE_UNNAMED_UNIONS
        psp[i].pszTemplate = MAKEINTRESOURCE(IDD_DISKDEVICE_DIALOG);
        psp[i].pszIcon = NULL;
#else
        psp[i].u1.pszTemplate = MAKEINTRESOURCE(IDD_DISKDEVICE_DIALOG);
        psp[i].u2.pszIcon = NULL;
#endif
        psp[i].lParam = 0;
        psp[i].pfnCallback = NULL;
    }

    psp[0].pfnDlgProc = callback_8;
    psp[0].pszTitle = "Drive 8";
    psp[1].pfnDlgProc = callback_9;
    psp[1].pszTitle = "Drive 9";
    psp[2].pfnDlgProc = callback_10;
    psp[2].pszTitle = "Drive 10";
    psp[3].pfnDlgProc = callback_11;
    psp[3].pszTitle = "Drive 11";

    psh.dwSize = sizeof(PROPSHEETHEADER);
    psh.dwFlags = PSH_PROPSHEETPAGE | PSH_NOAPPLYNOW;
    psh.hwndParent = hwnd;
    psh.hInstance = winmain_instance;
    psh.pszCaption = "Device manager";
    psh.nPages = 4;
#ifdef HAVE_UNNAMED_UNIONS
    psh.pszIcon = NULL;
    psh.nStartPage = 0;
    psh.ppsp = psp;
#else
    psh.u1.pszIcon = NULL;
    psh.u2.nStartPage = 0;
    psh.u3.ppsp = psp;
#endif
    psh.pfnCallback = NULL;

    PropertySheet(&psh);
}

