/*
 * pmwin2.c - The pm win snippets
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

#define INCL_WINSYS       // PP_*
#define INCL_WINMENUS     // MM_*
#define INCL_WINSTDSPIN   // SPBM_*
#define INCL_WINLISTBOXES // LM_*
#include <os2.h>
#include <string.h>

void WinLboxInsertMultitems(HWND hwnd, PSZ *txt, ULONG cnt)
{
    LBOXINFO info = { LIT_END, cnt, 0, 0 };
    WinSendMsg(hwnd, LM_INSERTMULTITEMS, &info, txt);
}

void WinDlgLboxInsertMultitems(HWND hwnd, ULONG id, PSZ *txt, ULONG cnt)
{
    WinLboxInsertMultitems(WinWindowFromID(hwnd, id), txt, cnt);
}

ULONG WinQueryDlgLboxSelectedItem(HWND hwnd, ULONG id)
{
    return WinQueryLboxSelectedItem(WinWindowFromID(hwnd, id));
}

BOOL WinQueryFont(HWND hwnd, const char *font, int size)
{
    return WinQueryPresParam(hwnd, PP_FONTNAMESIZE, NULLHANDLE,
                             NULL, size, (MPARAM)font, 0);
}

BOOL WinQueryDlgFont(HWND hwnd, ULONG id, const char *font, int size)
{
    return WinQueryFont(WinWindowFromID(hwnd, id), font, size);
}

BOOL WinSetFont(HWND hwnd, const char *font)
{
    return WinSetPresParam(hwnd, PP_FONTNAMESIZE,
                           strlen(font)+1, (void*)font);
}

BOOL WinSetDlgFont(HWND hwnd, ULONG id, const char *font)
{
    return WinSetFont(WinWindowFromID(hwnd, id), font);
}

BOOL WinQueryDlgPos(HWND hwnd, ULONG id, SWP *swp)
{
    return WinQueryWindowPos(WinWindowFromID(hwnd, id), swp);
}

BOOL WinSetDlgFocus(HWND hwnd, ULONG id)
{
    return WinSetFocus(HWND_DESKTOP, WinWindowFromID(hwnd, id));
}

BOOL WinShowDlg(HWND hwnd, ULONG id, BOOL bool)
{
    return WinShowWindow(WinWindowFromID(hwnd, id), bool);
}

void WinEmptyLbox(HWND hwnd)
{
    WinSendMsg(hwnd, LM_DELETEALL, 0, 0);
/*    int nr = (int)WinSendMsg(hwnd, LM_QUERYITEMCOUNT, 0, 0);

    while (nr--)
    WinSendMsg(hwnd, LM_DELETEITEM, (MPARAM)nr, 0);
    */
}

void WinEmptyDlgLbox(HWND hwnd, ULONG id)
{
    WinEmptyLbox(WinWindowFromID(hwnd, id));
}

MRESULT WinDelMenuItem(HWND hwnd, USHORT id)
{
    return WinSendMsg(hwnd, MM_DELETEITEM, MPFROM2SHORT(id, TRUE), 0);
}

void WinSetSpinVal(HWND hwnd, LONG val)
{
    WinSendMsg(hwnd, SPBM_SETCURRENTVALUE, (MPARAM)val, 0);
}

void WinSetDlgSpinVal(HWND hwnd, ULONG id, LONG val)
{
    WinSetSpinVal(WinWindowFromID(hwnd, id), val);
}

ULONG WinGetSpinVal(HWND hwnd)
{
    ULONG val;
    WinSendMsg(hwnd, SPBM_QUERYVALUE, (MPARAM)&val, 0);
    return val;
}

ULONG WinGetDlgSpinVal(HWND hwnd, ULONG id)
{
    return WinGetSpinVal(WinWindowFromID(hwnd, id));
}
