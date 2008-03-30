/*
 * dlg-contents.c - The contents-dialog.
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

#define INCL_WINSYS // font
#define INCL_WININPUT
#define INCL_WINDIALOGS
#define INCL_WINLISTBOXES

#include "vice.h"
#include "dialogs.h"

#include <stdio.h>
#include <string.h>

#include "charsets.h"
#include "autostart.h"
#include "imagecontents.h"

#include "log.h"

static MRESULT EXPENTRY pm_contents(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    static char image_name[CCHMAXPATH];
    static image_contents_t *image;
    static int first = TRUE;

    switch (msg)
    {
    case WM_INITDLG:
        setDlgOpen(DLGO_CONTENTS);
        image=image_contents_read_disk(mp2);
        strcpy(image_name, mp2);
        first = TRUE;
        break;
    case WM_COMMAND:
        switch (LONGFROMMP(mp1))
        {
        case DID_CLOSE:
            delDlgOpen(DLGO_CONTENTS);
            break;
        }
        break;
    case WM_CLOSE:
        delDlgOpen(DLGO_CONTENTS);
        break;
    case WM_PAINT:
        {
            if (first)
            {
                CHAR achFont[] = "11.System VIO";
                char text[1024];
                image_contents_file_list_t *entry=image->file_list;

                WinSetDlgFont(hwnd, LB_CONTENTS, achFont);

                sprintf(text, " 0 \"%s\" %s", p2a(image->name), p2a(image->id));
                WinLboxInsertItem(hwnd, LB_CONTENTS, text);
                while (entry)
                {
                    sprintf(text, " %-5i\"%s\"%6s",
                            entry->size, p2a(entry->name), p2a(entry->type));
                    WinLboxInsertItem(hwnd, LB_CONTENTS, text);
                    entry = entry->next;
                }
                sprintf(text, " %i blocks free.", image->blocks_free);
                WinLboxInsertItem(hwnd, LB_CONTENTS, text);

                first=FALSE;
            }
        }
        break;

    case WM_CONTROL:
        switch(SHORT1FROMMP(mp1))
        {
        case LB_CONTENTS:
            if (SHORT2FROMMP(mp1)==LN_ENTER)
            {
                int pos=WinLboxQuerySelectedItem(hwnd, LB_CONTENTS);
                image_contents_file_list_t *entry=image->file_list;

                if (!pos)
                {
                    if (autostart_autodetect(image_name, "*"))
                        ;// WinError(hwnd, "Cannot autostart specified image.");
                    else
                        WinSendMsg(hwnd, WM_CLOSE, 0, 0);
                    return FALSE;
                }

                while (--pos && entry) entry = entry->next;

                if (!entry)
                {
                    // WinError(hwnd, "Cannot autostart specified file.");
                    return FALSE;
                }
                {
                    char name[32];
                    strcpy(name, entry->name);
                    if (strlen(name))
                    {
                        char *end = name+strlen(name);
                        while (strrchr(name,' ')==(--end)) *end='\0';
                    }
                    if (autostart_autodetect(image_name, p2a(name)))
                        ;// WinError(hwnd, "Cannot autostart specified image.");
                    else
                        WinSendMsg(hwnd, WM_CLOSE, 0, 0);
                    return FALSE;
                }
            }
        }
        break;

    }
    return WinDefDlgProc (hwnd, msg, mp1, mp2);
}

/* call to open dialog                                              */
/*----------------------------------------------------------------- */

void contents_dialog(HWND hwnd, char *szFullFile)
{
    if (dlgOpen(DLGO_CONTENTS)) return;
    WinLoadDlg(HWND_DESKTOP, hwnd, pm_contents, NULLHANDLE,
               DLG_CONTENTS, (void*)szFullFile);
}

