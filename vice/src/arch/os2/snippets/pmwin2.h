/*
 * pmwin2.h - The pmwin snippets.
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

extern void WinLboxEmpty(HWND hwnd);
extern MRESULT WinLboxSettop(HWND hwnd);
extern MRESULT WinLboxSettopIdx(HWND hwnd, UINT idx);
extern UINT WinLboxQueryTopIdx(HWND hwnd);
extern BOOL WinLboxSelectItem(HWND hwnd, UINT idx);
extern LONG WinLboxInsertItem(HWND hwnd, const char *psz);
extern void WinLboxInsertMultitems(HWND hwnd, PSZ *txt, ULONG cnt);
extern LONG WinLboxItemHandle(HWND hwnd, UINT idx);
extern void WinLboxSetItemHandle(HWND hwnd, UINT idx, LONG hdl);

extern void WinDlgLboxEmpty(HWND hwnd, ULONG id);
extern MRESULT WinDlgLboxSettop(HWND hwnd, USHORT id);
extern MRESULT WinDlgLboxSettopIdx(HWND hwnd, USHORT id, UINT idx);
extern UINT WinDlgLboxQueryTopIdx(HWND hwnd, USHORT id);
extern LONG WinDlgLboxQueryCount(HWND hwnd, USHORT id);
extern BOOL WinDlgLboxSelectItem(HWND hwnd, USHORT id, UINT idx);
extern LONG WinDlgLboxInsertItem(HWND hwnd, USHORT id, const char *psz);
extern void WinDlgLboxInsertMultitems(HWND hwnd, ULONG id, PSZ *txt, ULONG cnt);
extern ULONG WinDlgLboxSelectedItem(HWND hwnd, ULONG id);
extern LONG WinDlgLboxDeleteItem(HWND hwnd, USHORT id, UINT idx);
extern LONG WinDlgLboxItemHandle(HWND hwnd, USHORT id, UINT idx);
extern void WinDlgLboxSetItemHandle(HWND hwnd, USHORT id, UINT idx, LONG hdl);

extern BOOL WinQueryFont(HWND hwnd, const char *font, int size);
extern BOOL WinQueryDlgFont(HWND hwnd, ULONG id, const char *font, int size);
extern BOOL WinSetFont(HWND hwnd, const char *font);
extern BOOL WinSetDlgFont(HWND hwnd, ULONG id, const char *fnt);
extern BOOL WinQueryDlgPos(HWND hwnd, ULONG id, SWP *swp);
extern BOOL WinSetDlgFocus(HWND hwnd, ULONG id);
extern BOOL WinShowDlg(HWND hwnd, ULONG id, BOOL bool);
extern BOOL WinDelMenuItem(HWND hwnd, USHORT id);
extern BOOL WinActivateWindow(HWND hwnd, int state);
extern BOOL WinActivateDlg(HWND hwnd, USHORT id, int state);

extern void WinSetSpinVal(HWND hwnd, ULONG val);
extern void WinSetDlgSpinVal(HWND hwnd, ULONG id, ULONG val);
extern ULONG WinGetSpinVal(HWND hwnd);
extern ULONG WinGetDlgSpinVal(HWND hwnd, ULONG id);

extern BOOL WinIsDlgShowing(HWND hwnd, ULONG id);

extern HWND WinLoadStdDlg(HWND hwnd, PFNWP pfnwp, USHORT id, VOID *param);

extern PFNWP WinSubclassDlg(HWND hwnd, ULONG id, PFNWP pNew);
