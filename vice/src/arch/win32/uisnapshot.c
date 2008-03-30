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
#include "res.h"
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
          case IDOK:
            if (image[0] != '\0') {
                if (machine_write_snapshot(image, save_roms, save_disks) < 0) {
                    ui_error("Cannot write snapshot file.");
                    break;
                }
                EndDialog(hwnd, IDOK);
                return TRUE;
            }
            ui_error("No file name specified.");
            break;
          case IDC_CANCEL:
            EndDialog(hwnd, IDC_CANCEL);
            return TRUE;
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
    }
    return FALSE;
}

void ui_snapshot_save_dialog(HWND hwnd)
{
    DialogBox(winmain_instance, (LPCTSTR)IDD_SNAPSHOT_SAVE_DIALOG, hwnd,
              ui_snapshot_save_dialog_proc);
}

void ui_snapshot_load_dialog(HWND hwnd)
{
    char *s;
    if ((s = ui_select_file("Load snapshot image",
        "VICE snapshot files (*.vsf)\0*.vsf\0"
        "All files (*.*)\0*.*\0", hwnd)) != NULL) {
         if (machine_read_snapshot(s) < 0)
             ui_error("Cannot read snapshot image");
         free(s);
    }
}

