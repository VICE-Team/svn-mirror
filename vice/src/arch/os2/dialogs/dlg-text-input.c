/*
 * dlg-text-input.c - Generic text input dialog.
 *
 * Written by
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
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

#define INCL_WINBUTTONS
#define INCL_WINSTDSPIN   // SPBN_*
#define INCL_WINDIALOGS

#include <os2.h>

#include "vice.h"

#include "lib.h"
#include "ui.h"
#include "ui_status.h"

#include "dialogs.h"

#include "resources.h"         // resources_*
#include "snippets\pmwin2.h"   // WinShowDlg

#define TEXT_LENGTH 100

static char text_buf[TEXT_LENGTH + 1];

static char *input_label;

static MRESULT EXPENTRY pm_text_input(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    switch (msg) {
        case WM_INITDLG:
            {
                WinSetDlgItemText(hwnd, IDC_TEXT_INPUT_LABEL, input_label);
                WinSendDlgItemMsg(hwnd, IDC_TEXT_INPUT, EM_SETTEXTLIMIT, MPFROMSHORT(TEXT_LENGTH), 0L);
                WinSetDlgItemText(hwnd, IDC_TEXT_INPUT, text_buf);
                WinSendDlgItemMsg(hwnd, IDC_TEXT_INPUT, EM_SETSEL, MPFROM2SHORT(0, strlen(text_buf)), 0L);
            }
            break;
        case WM_COMMAND:
            switch(LONGFROMMP(mp1)) {
                case DID_CLOSE:
                    WinQueryDlgItemText(hwnd, IDC_TEXT_INPUT, sizeof (text_buf), text_buf);
                    return TRUE;
            }
            break;
    }
    return WinDefDlgProc(hwnd, msg, mp1, mp2);
}

HWND hwndTIP = NULLHANDLE;

/* call to open dialog                                              */
/*----------------------------------------------------------------- */

char *text_input_dialog(HWND hwnd, char *title, char *text)
{
    if (WinIsWindowVisible(hwndTIP)) {
        return NULL;
    }

    input_label = title;
    sprintf(text_buf, "%s", text);

    hwndTIP = WinLoadStdDlg(hwnd, pm_text_input, DLG_TEXT_INPUT, NULL);

    return text_buf;
}
