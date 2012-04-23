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

// Joystick Dialog
#define CB_JOY11       0x1101
#define CB_JOY12       0x1201
#define CB_JOY21       0x1102
#define CB_JOY22       0x1202

#define CB_NUMJOY1     0x1104
#define CB_NUMJOY2     0x1204
#define CB_KS1JOY1     0x1108
#define CB_KS1JOY2     0x1208
#define CB_KS2JOY1     0x1110
#define CB_KS2JOY2     0x1210

#define ID_SWAP        0x1402
#define ID_CALIBRATE   0x1403
#define ID_KEYSET      0x1404
#define WM_SETCBS      WM_USER + 0x1
#define WM_SETDLGS     WM_USER + 0x2

// Calibrate Dialog
#define RB_JOY1        0x1406
#define RB_JOY2        0x1407
#define SPB_UP         0x1408
#define SPB_DOWN       0x1409
#define SPB_LEFT       0x140a
#define SPB_RIGHT      0x140b
#define ID_START       0x140c
#define ID_STOP        0x140d
#define ID_RESET       0x140e
#define WM_SETJOY      WM_USER + 0x3
#define WM_PROCESS     WM_USER + 0x4
#define WM_FILLSPB     WM_USER + 0x5
#define WM_ENABLECTRL  WM_USER + 0x6

// Keyset Dialog
#define RB_SET1        0x1410
#define RB_SET2        0x1411
#define SPB_N          0x1412
#define SPB_NW         0x1413
#define SPB_W          0x1414
#define SPB_SW         0x1415
#define SPB_S          0x1416
#define SPB_SE         0x1417
#define SPB_E          0x1418
#define SPB_NE         0x1419
#define SPB_FIRE       0x141a
#define WM_SETKEY      WM_USER + 0x7
#define WM_KPROCESS    WM_USER + 0x8
#define WM_KFILLSPB    WM_USER + 0x9
#define WM_KENABLECTRL WM_USER + 0xa

extern void joystick_dialog(HWND hwnd);
extern void joystick_extra_dialog(HWND hwnd);
extern void calibrate_dialog(HWND hwnd);
extern void keyset_dialog(HWND hwnd);
