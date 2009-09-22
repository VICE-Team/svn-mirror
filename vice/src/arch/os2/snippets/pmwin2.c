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

#include "vice.h"

#define INCL_WINSYS       // PP_*
#define INCL_WINMENUS     // MM_*
#define INCL_WINSTDSPIN   // SPBM_*
#define INCL_WINLISTBOXES // LM_*
#define INCL_WINWINDOWMGR // WinSubclassWindow

#include <os2.h>
#include <string.h>

void WinLboxInsertMultitems(HWND hwnd, PSZ *txt, ULONG cnt)
{
#ifdef WATCOM_COMPILE
    struct _LBOXINFO info;

    info.lItemIndex = LIT_END;
    info.ulItemCount = cnt;
    info.reserved = 0;
    info.reserved2 = 0;
#else
    LBOXINFO info = { LIT_END, cnt, 0, 0 };
#endif

    WinSendMsg(hwnd, LM_INSERTMULTITEMS, &info, txt);
}

void WinDlgLboxInsertMultitems(HWND hwnd, ULONG id, PSZ *txt, ULONG cnt)
{
    WinLboxInsertMultitems(WinWindowFromID(hwnd, id), txt, cnt);
}

ULONG WinDlgLboxSelectedItem(HWND hwnd, ULONG id)
{
    return WinQueryLboxSelectedItem(WinWindowFromID(hwnd, id));
}

BOOL WinQueryFont(HWND hwnd, const char *font, int size)
{
    return WinQueryPresParam(hwnd, PP_FONTNAMESIZE, NULLHANDLE, NULL, size, (MPARAM)font, 0);
}

BOOL WinQueryDlgFont(HWND hwnd, ULONG id, const char *font, int size)
{
    return WinQueryFont(WinWindowFromID(hwnd, id), font, size);
}

BOOL WinSetFont(HWND hwnd, const char *font)
{
    return WinSetPresParam(hwnd, PP_FONTNAMESIZE, strlen(font) + 1, (void*)font);
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

void WinLboxEmpty(HWND hwnd)
{
    WinSendMsg(hwnd, LM_DELETEALL, 0, 0);
}

void WinDlgLboxEmpty(HWND hwnd, ULONG id)
{
    WinLboxEmpty(WinWindowFromID(hwnd, id));
}

BOOL WinDelMenuItem(HWND hwnd, USHORT id)
{
    return (BOOL)WinSendMsg(hwnd, MM_DELETEITEM, MPFROM2SHORT(id, TRUE), 0);
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

BOOL WinIsDlgShowing(HWND hwnd, ULONG id)
{
    return WinIsWindowShowing(WinWindowFromID(hwnd, id));
}

HWND WinLoadStdDlg(HWND hwnd, PFNWP pfnwp, USHORT id, VOID *param)
{
    return WinLoadDlg(HWND_DESKTOP, hwnd, pfnwp, NULLHANDLE, id, param);
}

LONG WinLboxInsertItem(HWND hwnd, const char *psz)
{
    return (LONG)WinSendMsg(hwnd, LM_INSERTITEM, (void*)LIT_END, (void*)psz);
}

LONG WinDlgLboxInsertItem(HWND hwnd, USHORT id, const char *psz)
{
    return WinLboxInsertItem(WinWindowFromID(hwnd, id), psz);
}

MRESULT WinLboxSettop(HWND hwnd)
{
    return WinSendMsg(hwnd, LM_SETTOPINDEX, (void*)WinQueryLboxCount(hwnd), 0);
}

MRESULT WinDlgLboxSettop(HWND hwnd, USHORT id)
{
    return WinLboxSettop(WinWindowFromID(hwnd, id));
}

MRESULT WinLboxSettopIdx(HWND hwnd, UINT idx)
{
    return WinSendMsg(hwnd, LM_SETTOPINDEX, (void*)idx, 0);
}

MRESULT WinDlgLboxSettopIdx(HWND hwnd, USHORT id, UINT idx)
{
    return WinLboxSettopIdx(WinWindowFromID(hwnd, id), idx);
}

LONG WinDlgLboxQueryCount(HWND hwnd, USHORT id)
{
    return WinQueryLboxCount(WinWindowFromID(hwnd, id));
}

BOOL WinLboxSelectItem(HWND hwnd, UINT idx)
{
    return (BOOL)WinSendMsg(hwnd, LM_SELECTITEM, (void*)(idx), (void*)TRUE);
}

BOOL WinDlgLboxSelectItem(HWND hwnd, USHORT id, UINT idx)
{
    return WinLboxSelectItem(WinWindowFromID(hwnd, id), idx);
}

LONG WinDlgLboxDeleteItem(HWND hwnd, USHORT id, UINT idx)
{
    return WinDeleteLboxItem(WinWindowFromID(hwnd, id), idx);
}

LONG WinLboxItemHandle(HWND hwnd, UINT idx)
{
    return (HWND)WinSendMsg(hwnd, LM_QUERYITEMHANDLE, (void*)idx, 0);
}

LONG WinDlgLboxItemHandle(HWND hwnd, USHORT id, UINT idx)
{
    return WinLboxItemHandle(WinWindowFromID(hwnd, id), idx);
}

void WinLboxSetItemHandle(HWND hwnd, UINT idx, LONG hdl)
{
    WinSendMsg(hwnd, LM_SETITEMHANDLE, (void*)idx, (void*)hdl);
}

void WinDlgLboxSetItemHandle(HWND hwnd, USHORT id, UINT idx, LONG hdl)
{
    WinLboxSetItemHandle(WinWindowFromID(hwnd, id), idx, hdl);
}

BOOL WinActivateWindow(HWND hwnd, int state)
{
    return WinSetWindowPos(hwnd, 0, 0, 0, 0, 0, state ? SWP_SHOW | SWP_RESTORE | SWP_FOCUSACTIVATE : SWP_DEACTIVATE | SWP_HIDE | SWP_MINIMIZE | SWP_FOCUSDEACTIVATE);
}

BOOL WinActivateDlg(HWND hwnd, USHORT id, int state)
{
    return WinActivateWindow(WinWindowFromID(hwnd, id), state);
}

UINT WinLboxQueryTopIdx(HWND hwnd)
{
    return (UINT)WinSendMsg(hwnd, LM_QUERYTOPINDEX, 0, 0);
}

UINT WinDlgLboxQueryTopidx(HWND hwnd, USHORT id)
{
    return WinLboxQueryTopIdx(WinWindowFromID(hwnd, id));
}

PFNWP WinSubclassDlg(HWND hwnd, ULONG id, PFNWP pNew)
{
    return WinSubclassWindow(WinWindowFromID(hwnd, id), pNew);
}
