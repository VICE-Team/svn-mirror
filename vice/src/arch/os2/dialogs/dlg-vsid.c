/*
 * dlg-vsid.c - The vsid-dialog.
 *
 * Written by
 *  Thomas Bretz <tbretz@gsi.de>
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

#define INCL_WINSYS       // CV_*
#define INCL_WINMENUS     // WinLoadMenu
#define INCL_WINSTDSPIN   // WinSetSpinVal
#define INCL_WINDIALOGS   // WinSendDlgItemMsg
#define INCL_WINSTDDRAG   // PDRAGINFO in dragndrop.h
#define INCL_WINPOINTERS  // WinLoadPointer
#define INCL_WINFRAMEMGR  // WM_SETICON
#include "vice.h"

#include <os2.h>
#include <stdlib.h>

#include "dialogs.h"
#include "dlg-vsid.h"
#include "dragndrop.h"
#include "menubar.h"

#include "lib.h"
#include "psid.h"
#include "resources.h"

#include "snippets\pmwin2.h"

static MRESULT EXPENTRY pm_vsid_dragndrop(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    switch (msg) {
        case DM_DRAGOVER:
        case DM_DROP:
            return DragDrop(hwnd, msg, mp1);
    }
    return WinDefDlgProc (hwnd, msg, mp1, mp2);
}

static MRESULT EXPENTRY pm_vsid(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    extern int trigger_shutdown;

    switch (msg) {
        case WM_INITDLG:
            {
                HWND hmenu;
                HPOINTER hicon = WinLoadPointer(HWND_DESKTOP, NULLHANDLE, DLG_VSID);

                if (hicon) {
                    WinSendMsg(hwnd, WM_SETICON, MPFROMLONG(hicon), MPVOID);
                }

                //
                // Try to attach the menu and make it visible
                //
                hmenu = WinLoadMenu(hwnd, NULLHANDLE, IDM_VICE2);
                if (hmenu) {
                    SWP swp;
                    WinQueryWindowPos(hwnd, &swp);

                    WinDelMenuItem(hmenu, IDM_FILE);
                    WinDelMenuItem(hmenu, IDM_VIEW);
                    WinDelMenuItem(hmenu, IDM_SETUP);
                    WinDelMenuItem(hmenu, IDM_MONITOR);

                    swp.cy += WinQuerySysValue(HWND_DESKTOP, SV_CYMENU) + 1;
                    WinSetWindowPos(hwnd, 0, 0, 0, swp.cx, swp.cy, SWP_SIZE);

                    WinSendMsg(hwnd, WM_UPDATEFRAME, MPFROMLONG(FID_MENU), MPVOID);
                }

                WinSubclassDlg(hwnd, ID_TBOX, pm_vsid_dragndrop);
                WinSubclassDlg(hwnd, ID_TNAME, pm_vsid_dragndrop);
                WinSubclassDlg(hwnd, ID_TAUTHOR, pm_vsid_dragndrop);
                WinSubclassDlg(hwnd, ID_TCOPYRIGHT, pm_vsid_dragndrop);
                WinSubclassDlg(hwnd, ID_TSYNC, pm_vsid_dragndrop);
                WinSubclassDlg(hwnd, ID_TIRQ, pm_vsid_dragndrop);
                WinSubclassDlg(hwnd, ID_TSID, pm_vsid_dragndrop);
            }
            return FALSE;
        case WM_MENUSELECT:
            menu_select(HWNDFROMMP(mp2), SHORT1FROMMP(mp1));
            break;
        case WM_DESTROY:
            trigger_shutdown = 1;
            break;
        case WM_COMMAND:
            switch (LONGFROMMP(mp1)) {
                case DID_CLOSE:
                    trigger_shutdown = 1;
                    break;
                default:
                    menu_action(hwnd, SHORT1FROMMP(mp1));
                    return FALSE;
            }
            break;
        case WM_CONTROL:
            if (mp1 == MPFROM2SHORT(SPB_SETTUNE, SPBN_ENDSPIN)) {
                const ULONG val = WinGetSpinVal((HWND)mp2);

                resources_set_int("PSIDTune", (int)val);
            }
            break;
        case WM_DISPLAY:
            {
                char *txt = lib_msprintf("Vice/2 SID Player - %d%%", mp1);

                WinSetDlgItemText(hwnd, FID_TITLEBAR, txt);
                lib_free(txt);
            }
            return FALSE;
    }
    return WinDefDlgProc(hwnd, msg, mp1, mp2);
}

HWND vsid_dialog()
{
    return WinLoadStdDlg(HWND_DESKTOP, pm_vsid, DLG_VSID, NULL);
}
