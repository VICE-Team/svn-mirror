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
#define DLG_VSID       0x10f0
#define ID_TUNENO      0x10f1
#define ID_TUNES       0x10f2
#define SPB_SETTUNE    0x10f3
#define PB_DEFTUNE     0x10f4
#define ID_TIME        0x10f5
#define ID_TNAME       0x10f6
#define ID_TAUTHOR     0x10f7
#define ID_TCOPYRIGHT  0x10f8
#define ID_TSYNC       0x10f9
#define ID_OPTIONS     0x10fa

extern HWND hwndVsid;

extern HWND vsid_dialog(void);

