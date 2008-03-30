/*
 * uiattach.c - Implementation of the device manager dialog box.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Tibor Biczo <crown@mail.matav.hu>
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

#ifdef HAVE_SHLOBJ_H
#include <shlobj.h>
#endif

#ifndef DUMMYUNIONNAME
#define DUMMYUNIONNAME  u1
#endif

#include "attach.h"
#include "autostart.h"
#include "imagecontents.h"
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
    EnableWindow(GetDlgItem(hwnd, IDC_TOGGLE_ATTACH_READONLY),
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

static void init_dialog(HWND hwnd, unsigned int num)
{
    const char *disk_image, *dir;
    int n;

    int drive_true_emulation;

    resources_get_value("DriveTrueEmulation",
                        (resource_value_t *)&drive_true_emulation);

    if (num >= 8 && num <= 11) {
        disk_image = file_system_get_disk_name(num);
        SetDlgItemText(hwnd, IDC_DISKIMAGE,
                       disk_image != NULL ? disk_image : "");

        resources_get_sprintf("FSDevice%dDir",
                              (resource_value_t *)&dir, num);
        SetDlgItemText(hwnd, IDC_DIR, dir != NULL ? dir : "");

        resources_get_sprintf("FSDevice%dConvertP00",
                              (resource_value_t *)&n, num);
        CheckDlgButton(hwnd, IDC_TOGGLE_READP00,
                       n ? BST_CHECKED : BST_UNCHECKED);

        resources_get_sprintf("FSDevice%dSaveP00",
                              (resource_value_t *)&n, num);
        CheckDlgButton(hwnd, IDC_TOGGLE_WRITEP00,
                       n ? BST_CHECKED : BST_UNCHECKED);

        resources_get_sprintf("FSDevice%dHideCBMFiles",
                              (resource_value_t *)&n, num);
        CheckDlgButton(hwnd, IDC_TOGGLE_HIDENONP00,
                       n ? BST_CHECKED : BST_UNCHECKED);

        resources_get_sprintf("AttachDevice%dReadonly",
                              (resource_value_t *)&n, num);
        CheckDlgButton(hwnd, IDC_TOGGLE_ATTACH_READONLY,
                       n ? BST_CHECKED : BST_UNCHECKED);

        n = IDC_SELECTNONE;
        if (dir != NULL)
            if (*dir != 0)
                n = IDC_SELECTDIR;
        if (disk_image != NULL)
            if (*disk_image != 0)
                n = IDC_SELECTDISK;

        if (drive_true_emulation)
        {
            n = IDC_SELECTDISK;

            /* don't allow to check DIR or NONE */
            EnableWindow(GetDlgItem(hwnd, IDC_SELECTDIR),  FALSE );
            EnableWindow(GetDlgItem(hwnd, IDC_SELECTNONE), FALSE );
        }

        CheckRadioButton(hwnd, IDC_SELECTDISK, IDC_SELECTDIR, n);
        enable_controls_for_disk_device_type(hwnd, n);
    }
    if (num == 4) {
        resources_get_value("Printer4", (resource_value_t *)&n);
        CheckDlgButton(hwnd, IDC_TOGGLE_PRINTER,
                       n ? BST_CHECKED : BST_UNCHECKED);
    }
}

static BOOL CALLBACK dialog_proc(unsigned int num, HWND hwnd, UINT msg,
                                 WPARAM wparam, LPARAM lparam)
{
    int n;
    char tmp[256];

    switch (msg) {
      case WM_INITDIALOG:
        init_dialog(hwnd, num);
        return TRUE;
      case WM_DESTROY:
          /* check if DISKIMAGE is selected? */
          if (IsDlgButtonChecked( hwnd, IDC_SELECTDISK)!=BST_CHECKED)
          {
              /* no, detach the diskimage so a directory is possible as drive */
              file_system_detach_disk(num);

              /* check if DIR is selected? */
              if (IsDlgButtonChecked( hwnd, IDC_SELECTDIR)!=BST_CHECKED)
              {
                  /* no, detach the virtual disk */
                  resources_set_sprintf("FSDevice%dDir",
                    (resource_value_t)"", num);
              }
          }
          break;
      case WM_COMMAND:
        switch (LOWORD(wparam)) {
          case IDC_SELECTDIR:
          case IDC_SELECTDISK:
          case IDC_SELECTNONE:
            enable_controls_for_disk_device_type(hwnd, LOWORD(wparam));
            break;
          case IDC_DISKIMAGE:
            {
                char s[MAX_PATH];

                switch (HIWORD(wparam))
                {
                case CBN_KILLFOCUS:
                    GetDlgItemText(hwnd, IDC_DISKIMAGE, s, MAX_PATH);
                    if (s)
                        if (*s)
                            if (file_system_attach_disk(num, s) < 0)
                                ui_error("Cannot attach specified file");
                    break;

                default:
                    break;
                }
            }
            break;
          case IDC_BROWSEDISK:
            {
                char *s;

                s = ui_select_file(hwnd,"Attach disk image",
                    UI_LIB_FILTER_ALL | UI_LIB_FILTER_DISK | UI_LIB_FILTER_ZIP,
                    FILE_SELECTOR_DISK_STYLE,NULL);

                if (s != NULL) {
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

                s = ui_select_file(hwnd,"Autostart disk image",
                    UI_LIB_FILTER_ALL | UI_LIB_FILTER_DISK | UI_LIB_FILTER_ZIP,
                    FILE_SELECTOR_DISK_STYLE,NULL);

                if (s != NULL) {
                    SetDlgItemText(hwnd, IDC_DISKIMAGE, s);
                    if (autostart_autodetect(s, "*", 0, AUTOSTART_MODE_RUN) < 0)
                        ui_error("Cannot autostart specified file.");
                    free(s);
                }
            }
            break;
          case IDC_DIR:
            {
                char s[MAX_PATH];
                GetDlgItemText(hwnd, IDC_DIR, s, MAX_PATH);
                resources_set_sprintf("FSDevice%dDir",
                                      (resource_value_t)s, num);
            }
            break;
          case IDC_BROWSEDIR:
            {
                BROWSEINFO bi;
                char s[MAX_PATH];
                LPITEMIDLIST idlist;

                bi.hwndOwner = hwnd;
                bi.pidlRoot = NULL;
                bi.pszDisplayName = s;
                bi.lpszTitle = "Select file system directory";
                bi.ulFlags = 0;
                bi.lpfn = NULL;
                bi.lParam = 0;
                bi.iImage = 0;
                if ((idlist = SHBrowseForFolder(&bi)) != NULL) {
                    SHGetPathFromIDList(idlist, s);
                    LocalFree(idlist);
                    /*
                    If a root directory is selected, \ is appended
                    and has to be deleted.
                    */
                    if (s[strlen(s) - 1] == '\\')
                        s[strlen(s) - 1] = '\0';
                    SetDlgItemText(hwnd, IDC_DIR, s);
                    resources_set_sprintf("FSDevice%dDir",
                                          (resource_value_t)s, num);
                }
            }
            break;
          case IDC_TOGGLE_READP00:
            sprintf(tmp, "FSDevice%dConvertP00", num);
            resources_get_value(tmp, (resource_value_t *)&n);
            resources_set_value(tmp, (resource_value_t)!n);
            break;
          case IDC_TOGGLE_WRITEP00:
            sprintf(tmp, "FSDevice%dSaveP00", num);
            resources_get_value(tmp, (resource_value_t *)&n);
            resources_set_value(tmp, (resource_value_t)!n);
            break;
          case IDC_TOGGLE_HIDENONP00:
            sprintf(tmp, "FSDevice%dHideCBMFiles", num);
            resources_get_value(tmp, (resource_value_t *)&n);
            resources_set_value(tmp, (resource_value_t)!n);
            break;
          case IDC_TOGGLE_ATTACH_READONLY:
            sprintf(tmp, "AttachDevice%dReadonly", num);
            resources_get_value(tmp, (resource_value_t *)&n);
            resources_set_value(tmp, (resource_value_t)!n);
            break;
          case IDC_TOGGLE_PRINTER:
            resources_get_value("Printer4", (resource_value_t *)&n);
            resources_set_value("Printer4", (resource_value_t)!n);
            break;
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

_CALLBACK(4)
_CALLBACK(8)
_CALLBACK(9)
_CALLBACK(10)
_CALLBACK(11)

void ui_attach_dialog(HWND hwnd)
{
    PROPSHEETPAGE psp[5];
    PROPSHEETHEADER psh;
    int i;

    int drive_true_emulation;
    int no_of_drives;

    resources_get_value("DriveTrueEmulation",
                        (resource_value_t *)&drive_true_emulation);

    no_of_drives = drive_true_emulation ? 3 : 5;

    psp[0].dwSize = sizeof(PROPSHEETPAGE);
    psp[0].dwFlags = PSP_USETITLE /*| PSP_HASHELP*/ ;
    psp[0].hInstance = winmain_instance;
#ifdef HAVE_UNNAMED_UNIONS
    psp[0].pszTemplate = MAKEINTRESOURCE(IDD_PRINTDEVICE_DIALOG);
    psp[0].pszIcon = NULL;
#else
    psp[0].DUMMYUNIONNAME.pszTemplate = MAKEINTRESOURCE(IDD_PRINTDEVICE_DIALOG);
    psp[0].u2.pszIcon = NULL;
#endif
    psp[0].lParam = 0;
    psp[0].pfnCallback = NULL;


    for (i = 1; i < no_of_drives; i++) {
        psp[i].dwSize = sizeof(PROPSHEETPAGE);
        psp[i].dwFlags = PSP_USETITLE /*| PSP_HASHELP*/ ;
        psp[i].hInstance = winmain_instance;
#ifdef HAVE_UNNAMED_UNIONS
        psp[i].pszTemplate = MAKEINTRESOURCE(IDD_DISKDEVICE_DIALOG);
        psp[i].pszIcon = NULL;
#else
        psp[i].DUMMYUNIONNAME.pszTemplate = MAKEINTRESOURCE(IDD_DISKDEVICE_DIALOG);
        psp[i].u2.pszIcon = NULL;
#endif
        psp[i].lParam = 0;
        psp[i].pfnCallback = NULL;
    }

    psp[0].pfnDlgProc = callback_4;
    psp[0].pszTitle = "Printer 4";
    psp[1].pfnDlgProc = callback_8;
    psp[1].pszTitle = "Drive 8";
    psp[2].pfnDlgProc = callback_9;
    psp[2].pszTitle = "Drive 9";
    psp[3].pfnDlgProc = callback_10;
    psp[3].pszTitle = "Drive 10";
    psp[4].pfnDlgProc = callback_11;
    psp[4].pszTitle = "Drive 11";

    psh.dwSize = sizeof(PROPSHEETHEADER);
    psh.dwFlags = PSH_PROPSHEETPAGE | PSH_NOAPPLYNOW;
    psh.hwndParent = hwnd;
    psh.hInstance = winmain_instance;
    psh.pszCaption = "Device manager";
    psh.nPages = no_of_drives;
#ifdef HAVE_UNNAMED_UNIONS
    psh.pszIcon = NULL;
    psh.nStartPage = 0;
    psh.ppsp = psp;
#else
    psh.DUMMYUNIONNAME.pszIcon = NULL;
    psh.u2.nStartPage = 0;
    psh.u3.ppsp = psp;
#endif
    psh.pfnCallback = NULL;

    PropertySheet(&psh);
}

