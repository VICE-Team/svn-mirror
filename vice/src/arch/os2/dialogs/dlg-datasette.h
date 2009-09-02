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

// Datasette Dialog
#define PB_STOP      0x10  /* DATASETTE_CONTROL_STOP           0 */
#define PB_START     0x11  /* DATASETTE_CONTROL_START          1 */
#define PB_FORWARD   0x12  /* DATASETTE_CONTROL_FORWARD        2 */
#define PB_REWIND    0x13  /* DATASETTE_CONTROL_REWIND         3 */
#define PB_RECORD    0x14  /* DATASETTE_CONTROL_RECORD         4 */
#define PB_RESET     0x15  /* DATASETTE_CONTROL_RESET          5 */
#define PB_RESETCNT  0x16  /* DATASETTE_CONTROL_RESET_COUNTER  6 */

#define SPB_COUNT    0x17
#define SS_SPIN      0x18
#define CB_RESETWCPU 0x19
#define SPB_DELAY    0x1a
#define SPB_GAP      0x1b
#define PB_TATTACH   0x1c
#define PB_TDETACH   0x1d


#define WM_COUNTER   WM_USER+0x1
#define WM_TAPESTAT  WM_USER+0x2
#define WM_SPINNING  WM_USER+0x3

extern HWND hwndDatasette;

extern void datasette_dialog(HWND hwnd);
