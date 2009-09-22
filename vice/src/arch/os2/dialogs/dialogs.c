/*
 * dialogs.c - The dialogs.
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

#define INCL_WINDIALOGS
#define INCL_WINBUTTONS   // BS_DEFAULT
#define INCL_WINPOINTERS  // WinLoadPointer
#include "vice.h"

#include <os2.h>

#include "dialogs.h"
#include "resources.h"

#include "snippets\\pmwin2.h"

int toggle(const char *resource_name)
{
    int val;

    if (resources_toggle(resource_name, &val) < 0) {
        return -1;
    }

    return val;
}

void ViceErrorDlg(HWND hwnd, int id, char *text)
{
#ifdef WATCOM_COMPILE
    struct _MB2INFO mb;
    struct _MB2D mbtemp;

    mb.cb = sizeof(struct _MB2INFO);
    mb.hIcon = WinLoadPointer(HWND_DESKTOP, NULLHANDLE, id);
    mb.cButtons = 1;
    mb.flStyle = MB_CUSTOMICON|WS_VISIBLE;
    mb.hwndNotify = NULLHANDLE;
    sprintf(mbtemp.achText,"      OK      ");
    mbtemp.idButton = 0;
    mbtemp.flStyle = BS_DEFAULT;
    mb.mb2d[0]=mbtemp;
#else
    MB2INFO mb = { sizeof(MB2INFO), WinLoadPointer(HWND_DESKTOP, NULLHANDLE, id),
                  1, MB_CUSTOMICON|WS_VISIBLE, NULLHANDLE, "      OK      ",
                  0, BS_DEFAULT };
#endif
    WinMessageBox2(HWND_DESKTOP, hwnd, text, "VICE/2 Error", 0, &mb);
}

/* call to open dialog                                              */
/*----------------------------------------------------------------- */
void about_dialog(HWND hwnd)
{
    static HWND hwnd2 = NULLHANDLE;

    if (WinIsWindowVisible(hwnd2)) {
        return;
    }

    hwnd2 = WinLoadStdDlg(hwnd, WinDefDlgProc, DLG_ABOUT, NULL);
}
