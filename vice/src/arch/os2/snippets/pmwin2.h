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

extern void    WinLboxInsertMultitems(HWND hwnd, PSZ *txt, ULONG cnt);
extern void    WinDlgLboxInsertMultitems(HWND hwnd, ULONG id, PSZ *txt, ULONG cnt);
extern ULONG   WinQueryDlgLboxSelectedItem(HWND hwnd, ULONG id);
extern BOOL    WinQueryFont(HWND hwnd, const char *font, int size);
extern BOOL    WinQueryDlgFont(HWND hwnd, ULONG id, const char *font, int size);
extern BOOL    WinSetFont(HWND hwnd, const char *font);
extern BOOL    WinSetDlgFont(HWND hwnd, ULONG id, const char *fnt);
extern BOOL    WinQueryDlgPos(HWND hwnd, ULONG id, SWP *swp);
extern BOOL    WinSetDlgFocus(HWND hwnd, ULONG id);
extern BOOL    WinShowDlg(HWND hwnd, ULONG id, BOOL bool);
extern void    WinEmptyLbox(HWND hwnd);
extern void    WinEmptyDlgLbox(HWND hwnd, ULONG id);
extern MRESULT WinDelMenuItem(HWND hwnd, USHORT id);
extern void    WinSetSpinVal(HWND hwnd, ULONG val);
extern void    WinSetDlgSpinVal(HWND hwnd, ULONG id, ULONG val);
extern ULONG   WinGetSpinVal(HWND hwnd);
extern ULONG   WinGetDlgSpinVal(HWND hwnd, ULONG id);

