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
#include "machine.h"
#include "platform_discovery.h"
#include "res.h"
#include "system.h"
#include "translate.h"
#include "uihelp.h"
#include "uilib.h"
#include "util.h"
#include "version.h"
#include "vicefeatures.h"
#include "winmain.h"

#define ID_TEXT 200
#define ID_MEMBERS_TEXT 300

#ifdef USE_SVN_REVISION
#include "svnversion.h"
#endif

LPWORD lpdwAlign(LPWORD lpIn)
{
    ULONG ul;
    ul = (ULONG)lpIn;
    ul += 3;
    ul >>= 2;
    ul <<= 2;
    return (LPWORD)ul;
}

static char *get_compiletime_features(void)
{
    feature_list_t *list;
    char *str, *lstr;
    unsigned int len = 0;

    list = vice_get_feature_list();
    while (list->symbol) {
        len += strlen(list->descr) + strlen(list->symbol) + (15);
        ++list;
    }
    str = lib_malloc(len);
    lstr = str;
    list = vice_get_feature_list();
    while (list->symbol) {
        sprintf(lstr, "%4s\t%s (%s)\n", list->isdefined ? "yes " : "no  ", list->descr, list->symbol);
        lstr += strlen(lstr);
        ++list;
    }
    return str;
}

static BOOL CALLBACK AboutDialogProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
    HWND element;
    char *version;
    char *tmp;
    TCHAR *st_version;
    int i;

    switch (iMsg) {
        case WM_INITDIALOG:
            SetWindowText(hDlg, "About VICE");
            element = GetDlgItem(hDlg, IDOK);
            SetWindowText(element, "OK");
            element = GetDlgItem(hDlg, ID_TEXT);
            SetWindowText(element, "VICE");
            element = GetDlgItem(hDlg, ID_TEXT + 1);
            SetWindowText(element, "Versatile Commodore Emulator");
            element = GetDlgItem(hDlg, ID_TEXT + 2);
            SetWindowText(element, "Version 0.0");
#ifdef UNSTABLE
#  ifdef USE_SVN_REVISION
            version = lib_msprintf(translate_text(IDS_VERSION_S_REV_S_UNSTABLE), VERSION, VICE_SVN_REV_STRING, PLATFORM);
#  else
            version = lib_msprintf(translate_text(IDS_VERSION_S_UNSTABLE), VERSION, PLATFORM);
#  endif
#else /* #ifdef UNSTABLE */
#  ifdef USE_SVN_REVISION
            version = lib_msprintf(translate_text(IDS_VERSION_S_REV_S), VERSION, VICE_SVN_REV_STRING, PLATFORM);
#  else
            version = lib_msprintf(translate_text(IDS_VERSION_S), VERSION, PLATFORM);
#  endif
#endif /* #ifdef UNSTABLE */
            st_version = system_mbstowcs_alloc(version);
            SetDlgItemText(hDlg, ID_TEXT + 2, st_version);
            system_mbstowcs_free(st_version);
            lib_free(version);
            for (i = 0; core_team[i].name; i++) {
                    tmp = util_concat("Copyright (c) ", core_team[i].years, " ", core_team[i].name, NULL);
                    element = GetDlgItem(hDlg, ID_MEMBERS_TEXT + i);
                    SetWindowText(element, tmp);
                    lib_free(tmp);
            }
            return TRUE ;
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case IDOK:
                    EndDialog(hDlg, 0) ;
                    return TRUE ;
            }
            break ;
    }
    return FALSE ;
}

static LRESULT DisplayAboutBox(HINSTANCE hinst, HWND hwndOwner)
{
    HGLOBAL hgbl;
    LPDLGTEMPLATE lpdt;
    LPDLGITEMTEMPLATE lpdit;
    LPWORD lpw;
    LPWSTR lpwsz;
    LRESULT ret;
    int nchar;
    int i;
    int amount = 0;

    hgbl = GlobalAlloc(GMEM_ZEROINIT, 32768);

    if (!hgbl) {
       return -1;
    }

    for (i = 0; core_team[i].name; i++) {
        amount++;
    }

    lpdt = (LPDLGTEMPLATE)GlobalLock(hgbl);

    /* Dialog Box */
    lpdt->style = WS_POPUP | WS_BORDER | WS_SYSMENU | DS_MODALFRAME | WS_CAPTION | DS_SETFONT;
    lpdt->cdit = 4 + amount;
    lpdt->x = 0;
    lpdt->y = 0;
    lpdt->cx = 225;
    lpdt->cy = 50 + ((amount + 1) * 9);

    lpw = (LPWORD)(lpdt + 1);
    *lpw++ = 0;
    *lpw++ = 0;
    *lpw++ = 0;
    *lpw++ = 8;

    lpwsz = (LPWSTR)lpw;
    nchar = 1 + MultiByteToWideChar(CP_ACP, 0, "MS Sans Serif", -1, lpwsz, 50);
    lpw += nchar;

    /* OK Button */
    lpw = lpdwAlign(lpw);

    lpdit = (LPDLGITEMTEMPLATE)lpw;
    lpdit->x = 132;
    lpdit->y = 14;
    lpdit->cx = 50;
    lpdit->cy = 14;
    lpdit->id = IDOK;
    lpdit->style = WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON;

    lpw = (LPWORD)(lpdit + 1);
    *lpw++ = 0xFFFF;
    *lpw++ = 0x0080;
    *lpw++ = 0;
    *lpw++ = 0;

    /* "VICE" */
    lpw = lpdwAlign(lpw);
    lpdit = (LPDLGITEMTEMPLATE)lpw;
    lpdit->x = 7;
    lpdit->y = 5;
    lpdit->cx = 118;
    lpdit->cy = 8;
    lpdit->id = ID_TEXT;
    lpdit->style = WS_CHILD | WS_VISIBLE | SS_CENTER;

    lpw = (LPWORD)(lpdit + 1);
    *lpw++ = 0xFFFF;
    *lpw++ = 0x0082;
    *lpw++ = 0;
    *lpw++ = 0;

    /* "Versatile Commodore Emulator" */
    lpw = lpdwAlign(lpw);
    lpdit = (LPDLGITEMTEMPLATE)lpw;
    lpdit->x = 7;
    lpdit->y = 18;
    lpdit->cx = 118;
    lpdit->cy = 8;
    lpdit->id = ID_TEXT + 1;
    lpdit->style = WS_CHILD | WS_VISIBLE | SS_CENTER;

    lpw = (LPWORD)(lpdit + 1);
    *lpw++ = 0xFFFF;
    *lpw++ = 0x0082;
    *lpw++ = 0;
    *lpw++ = 0;

    /* "Version" */
    lpw = lpdwAlign(lpw);
    lpdit = (LPDLGITEMTEMPLATE)lpw;
    lpdit->x = 7;
    lpdit->y = 31;
    lpdit->cx = 180;
    lpdit->cy = 8;
    lpdit->id = ID_TEXT + 2;
    lpdit->style = WS_CHILD | WS_VISIBLE | SS_CENTER;

    lpw = (LPWORD)(lpdit + 1);
    *lpw++ = 0xFFFF;
    *lpw++ = 0x0082;
    *lpw++ = 0;
    *lpw++ = 0;

    for (i = 0; core_team[i].name; i++) {
        lpw = lpdwAlign(lpw);
        lpdit = (LPDLGITEMTEMPLATE)lpw;
        lpdit->x = 5;
        lpdit->y = 50 + (i * 9);
        lpdit->cx = 180;
        lpdit->cy = 8;
        lpdit->id = ID_MEMBERS_TEXT + i;
        lpdit->style = WS_CHILD | WS_VISIBLE | SS_CENTER;
        lpw = (LPWORD)(lpdit + 1);
        *lpw++ = 0xFFFF;
        *lpw++ = 0x0082;
        *lpw++ = 0;
        *lpw++ = 0;
    }

    GlobalUnlock(hgbl);

    ret = DialogBoxIndirect(hinst, (LPDLGTEMPLATE)hgbl, hwndOwner, (DLGPROC)AboutDialogProc);

    GlobalFree(hgbl);

    return ret;
}

void uihelp_dialog(HWND hwnd, WPARAM wparam)
{
    char *fname;
    char *dname;
    char *features = NULL;

    STARTUPINFOW si;
    PROCESS_INFORMATION pi;
    memset(&si, 0, sizeof(si));
    memset(&pi, 0, sizeof(pi));
    si.cb = sizeof(si);

    switch (wparam) {
        case IDM_ABOUT:
            DisplayAboutBox(winmain_instance, hwnd);
            break;
        case IDM_HELP:
            fname = util_concat(archdep_boot_path(), "\\DOC\\vice.chm", NULL);
            dname = util_concat(archdep_boot_path(), "\\DOC", NULL);
            ShellExecute(NULL, "open", fname, NULL, dname, SW_SHOWNORMAL);
            lib_free(fname);
            lib_free(dname);
            break;
        case IDM_FEATURES:
            features = get_compiletime_features();
            ui_show_text(hwnd, translate_text(IDS_VICE_FEATURES), translate_text(IDS_WHAT_FEATURES_ARE_AVAILABLE), features);
            lib_free(features);
            break;
        case IDM_CONTRIBUTORS:
            ui_show_text(hwnd, translate_text(IDS_VICE_CONTRIBUTORS), translate_text(IDS_WHO_MADE_WHAT), info_contrib_text);
            break;
        case IDM_LICENSE:
            ui_show_text(hwnd, translate_text(IDS_LICENSE), "VICE license (GNU General Public License)", info_license_text);
            break;
        case IDM_WARRANTY:
            ui_show_text(hwnd, translate_text(IDS_NO_WARRANTY), translate_text(IDS_VICE_WITHOUT_WARRANTY), info_warranty_text);
            break;
        case IDM_CMDLINE:
            uilib_show_options(hwnd);
            break;
    }
}
