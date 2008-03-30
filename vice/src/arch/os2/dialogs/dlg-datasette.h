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
#define DLG_DATASETTE  0x1060
#define PB_STOP        0x1070  /* DATASETTE_CONTROL_STOP           0 */
#define PB_START       0x1071  /* DATASETTE_CONTROL_START          1 */
#define PB_FORWARD     0x1072  /* DATASETTE_CONTROL_FORWARD        2 */
#define PB_REWIND      0x1073  /* DATASETTE_CONTROL_REWIND         3 */
#define PB_RECORD      0x1074  /* DATASETTE_CONTROL_RECORD         4 */
#define PB_RESET       0x1075  /* DATASETTE_CONTROL_RESET          5 */
#define PB_RESETCNT    0x1076  /* DATASETTE_CONTROL_RESET_COUNTER  6 */
#define SPB_COUNT      0x1077
#define SS_SPIN        0x1078
#define CB_RESETWCPU   0x1079
#define SPB_DELAY      0x107a
#define SPB_GAP        0x107b
#define PB_TATTACH     0x107c
#define PB_TDETACH     0x107d
#define WM_COUNTER     WM_USER+0x1
#define WM_TAPESTAT    WM_USER+0x2
#define WM_SPINNING    WM_USER+0x3

extern HWND hwndDatasette;

extern void datasette_dialog(HWND hwnd);
