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

#include "vice.h"
#include "dialogs.h"

#include "resources.h"

int toggle(char *resource_name)
{
    int val;
    if (resources_get_value(resource_name, (resource_value_t *) &val) <0)
        return -1;
    resources_set_value(resource_name, (resource_value_t)   !val);
    return !val;
}

static int dlg_open = FALSE;

int dlgOpen(int dlg)
{
    return dlg_open & dlg;
}

void setDlgOpen(int dlg)
{
    dlg_open |= dlg;
}

void delDlgOpen(int dlg)
{
    dlg_open &= ~dlg;
}

/* call to open dialog                                              */
/*----------------------------------------------------------------- */

void about_dialog(HWND hwnd)
{
    WinLoadDlg(HWND_DESKTOP, hwnd, WinDefDlgProc, NULLHANDLE,
               DLG_ABOUT, NULL);
}

