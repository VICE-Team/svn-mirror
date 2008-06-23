/*
 * uihelp.c - Help menu UI.
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
 *  Ettore Perazzoli <ettore@comm2000.it>
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
#include <string.h>
#include <windows.h>
#include <tchar.h>

#include "archdep.h"
#include "info.h"
#include "intl.h"
#include "lib.h"
#include "res.h"
#include "system.h"
#include "translate.h"
#include "uihelp.h"
#include "uilib.h"
#include "util.h"
#include "version.h"
#include "winmain.h"


int CALLBACK about_dialog_proc(HWND dialog, UINT msg, UINT wparam, LONG lparam)
{
    char *version;
    TCHAR *st_version;

    switch (msg) {
      case WM_INITDIALOG:
#ifdef UNSTABLE
        version = lib_msprintf(translate_text(IDS_VERSION_S_UNSTABLE), VERSION);
#else /* #ifdef UNSTABLE */
        version = lib_msprintf(translate_text(IDS_VERSION_S), VERSION);
#endif /* #ifdef UNSTABLE */
        st_version = system_mbstowcs_alloc(version);
        SetDlgItemText(dialog, IDC_ABOUT_VERSION, st_version);
        system_mbstowcs_free(st_version);
        lib_free(version);
        return TRUE;
      case WM_CLOSE:
        EndDialog(dialog,0);
        return TRUE;
      case WM_COMMAND:
        if ((wparam == IDOK) || (wparam == IDCANCEL)) {
            EndDialog(dialog, 0);
            return TRUE;
        }
        break;
    }
    return FALSE;
}

void uihelp_dialog(HWND hwnd, WPARAM wparam)
{
    char *fname;
    char *dname;

    switch (wparam) {
      case IDM_ABOUT:
        DialogBox(winmain_instance, MAKEINTRESOURCE(IDD_ABOUT), hwnd,
                  (DLGPROC)about_dialog_proc);
        break;
      case IDM_HELP:
        fname = util_concat(archdep_boot_path(), "\\DOC\\vice_toc.html", NULL);
        dname = util_concat(archdep_boot_path(), "\\DOC", NULL);
        ShellExecute(hwnd, "open", fname, NULL, dname, 0);
        lib_free(fname);
        lib_free(dname);
        break;
      case IDM_CONTRIBUTORS:
        ui_show_text(hwnd, translate_text(IDS_VICE_CONTRIBUTORS), translate_text(IDS_WHO_MADE_WHAT),
                     info_contrib_text);
        break;
      case IDM_LICENSE:
        ui_show_text(hwnd, translate_text(IDS_LICENSE),
                     "VICE license (GNU General Public License)",
                     info_license_text);
        break;
      case IDM_WARRANTY:
        ui_show_text(hwnd, translate_text(IDS_NO_WARRANTY),
                     translate_text(IDS_VICE_WITHOUT_WARRANTY),
                     info_warranty_text);
        break;
      case IDM_CMDLINE:
        uilib_show_options(hwnd);
        break;
    }
}

