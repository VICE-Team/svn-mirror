/*
 * dlg-reset.c - The reset-dialogs.
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

#define INCL_WINBUTTONS    // HPOINTER
#define INCL_WINPOINTERS   // BS_DEFAULT

#include <os2.h>
#include <string.h>
#include <stdlib.h>

#include "lib.h"
#include "vsync.h"         // vsync_suspend_speed_eval
#include "machine.h"

ULONG ResetDialog(HWND hwnd, char *text)
{
    ULONG rc;
    const int sz = sizeof(MB2INFO) + sizeof(MB2D);
    HPOINTER hpt = WinLoadPointer(HWND_DESKTOP, NULLHANDLE, 0x103); //PTR_NOTE);
    MB2INFO *mb = malloc(sz);
    mb->cb = sz;
    mb->hIcon = hpt;
    mb->cButtons = 2;
    mb->flStyle = MB_CUSTOMICON | WS_VISIBLE;
    mb->hwndNotify = NULLHANDLE;
    strcpy(mb->mb2d[0].achText, "      ~Yes      ");
    strcpy(mb->mb2d[1].achText, "      ~No      ");
    mb->mb2d[0].idButton = 0;
    mb->mb2d[1].idButton = MBID_ERROR; // 0xffff
    mb->mb2d[0].flStyle = 0;
    mb->mb2d[1].flStyle = BS_DEFAULT;

    rc = WinMessageBox2(HWND_DESKTOP, hwnd, text, "VICE/2 Reset", 0, mb);
    lib_free(mb);

    return rc;
}

void hardreset_dialog(HWND hwnd)
{
    if (ResetDialog(hwnd, " Hard Reset:\n Do you really want to reset the emulated machine?")) {
        return;
    }

    vsync_suspend_speed_eval();
    machine_trigger_reset(MACHINE_RESET_MODE_HARD);
}

void softreset_dialog(HWND hwnd)
{
    if (ResetDialog(hwnd, " Soft Reset:\n Do you really want to reset the emulated machine?")) {
        return;
    }

    vsync_suspend_speed_eval();
    machine_trigger_reset(MACHINE_RESET_MODE_SOFT);
}
