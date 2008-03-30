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

// Monitor Dialog
#define DLG_MONITOR       0x10b0

#define LB_MONOUT         0x1
#define EF_MONIN          0x2

#define DID_STEPOVER      0x10
#define DID_STEPINTO      0x11
#define DID_MONRETURN     0x12
#define DID_MONEXIT       0x13
#define DID_MONREC        0x14
#define DID_MONPLAY       0x15
#define DID_MONSTOP       0x16

#define IDM_SIDEFX        0x17

#define IDM_REGISTER      0x18
#define IDM_REGCPU        0x19
#define IDM_REGDRV8       0x1a
#define IDM_REGDRV9       0x1b

#define IDM_DISASSABMBLE  0x1c
#define IDM_DISCPU        0x1d
#define IDM_DISDRV8       0x1e
#define IDM_DISDRV9       0x1f

#define IDM_CPUTYPE       0x20
#define IDM_CPU6502       0x21
#define IDM_CPUZ80        0x22

// dialogs.h: #define WM_INSERT      WM_USER+0x1
#define WM_INPUT       WM_USER+0x2
#define WM_PROMPT      WM_USER+0x3
#define WM_UPDATE      WM_USER+0x4
#define WM_CONSOLE     WM_USER+0x5

#define kOPEN  ((MPARAM)TRUE)
#define kCLOSE ((MPARAM)FALSE)

// Monitor Registers dialog
#define DLG_MONREG     0x10b1
#define ID_REGN        0x1
#define ID_REGV        0x2
#define ID_REGM        0x3
#define ID_REGB        0x4
#define ID_REGD        0x5
#define ID_REGI        0x6
#define ID_REGZ        0x7
#define ID_REGC        0x8

#define ID_REGA        0x9
#define ID_REGAC       0xa
#define ID_REGX        0xb
#define ID_REGY        0xc
#define ID_REGSP       0xd
#define ID_REG01       0xe

// Monitor Disassembly
#define DLG_MONDIS     0x10b2
#define LB_MONDIS      0x1

//
// ---------------- dialog definitions ------------------
//

extern HWND hwndMonitor;

extern int console_init(void);

