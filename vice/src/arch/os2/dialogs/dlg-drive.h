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

// Drive Dialog
#define RB_DRIVE8      0x1020
#define RB_DRIVE9      0x1021
#define RB_DRIVE10     0x1022
#define RB_DRIVE11     0x1023
#define CB_TRUEDRIVE   0x1024

#define SPB_TRACK8     0x1027
#define SPB_TRACK9     0x1028
#define SS_HALFTRACK8  0x1029
#define SS_HALFTRACK9  0x102a
#define SS_LED8        0x102b
#define SS_LED9        0x102c

#define RB_NONE        0x1030
#define RB_SKIP        0x1031
#define RB_TRAP        0x1032

#define CBS_IMAGE      0x1033
#define CBS_PATH       0x1034
#define CBS_TYPE       0x1035
#define CB_PARALLEL    0x1036

#define CB_ALLOWACCESS 0x1037
#define CB_CONVERTP00  0x1038
#define CB_SAVEP00     0x1039
#define CB_HIDENONP00  0x103a
#define PB_CREATE      0x103b
#define PB_ATTACH      0x103c
#define PB_DETACH      0x103d
#define CB_READONLY    0x103e

#define RB_NEVER       0x1040
#define RB_ASK         0x1041
#define RB_ALWAYS      0x1042

#define PB_FLIP        0x1045
#define PB_FLIPADD     0x1046
#define PB_FLIPREMOVE  0x1047

#define CB_MEM2000     0x1048
#define CB_MEM4000     0x1049
#define CB_MEM6000     0x104a
#define CB_MEM8000     0x104b
#define CB_MEMA000     0x104c

#define WM_SWITCH      WM_USER + 0x1
#define WM_SWITCHTT    WM_USER + 0x2
#define WM_DRIVEIMAGE  WM_USER + 0x3
#define WM_TRACK       WM_USER + 0x4
#define WM_DRIVELEDS   WM_USER + 0x5
#define WM_DRIVESTATE  WM_USER + 0x6

extern HWND hwndDrive;

extern void drive_dialog(HWND hwnd);
