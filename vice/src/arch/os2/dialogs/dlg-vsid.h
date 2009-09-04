/*
 * dialogs.h - The dialog windows.
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

// Vsid Dialog
#define ID_TUNENO      0x10
#define ID_TUNES       0x11
#define SPB_SETTUNE    0x12
#define PB_DEFTUNE     0x13
#define ID_TIME        0x14
#define ID_TNAME       0x15
#define ID_TAUTHOR     0x16
#define ID_TCOPYRIGHT  0x17
#define ID_TSYNC       0x18
#define ID_TIRQ        0x19
#define ID_TSID        0x20
#define ID_TBOX        0x21

#define WM_DISPLAY     WM_USER + 1

extern HWND hwndVsid;

extern HWND vsid_dialog(void);
