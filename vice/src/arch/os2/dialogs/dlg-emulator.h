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

// Emulator Dialog
#define SPB_SPEED      0x1091
#define PB_SPEED100    0x1092
#define CBS_REFRATE    0x1093
#define ID_SPEEDDISP   0x1099
#define ID_REFRATEDISP 0x109a
#define CBS_SSNAME     0x109d
#define PB_SSCHANGE    0x109e
#define CBS_SPSNAME    0x109f
#define PB_SPSCHANGE   0x10a0
#define RB_BMP         0x10a1
#define RB_PNG         0x10a2
#define WM_DISPLAY     WM_USER + 1

extern HWND hwndEmulator;

extern void emulator_dialog(HWND hwnd);
