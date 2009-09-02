/*
 * dlg-fileio.h - The new file dialog.
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

#ifndef FDS_OPEN_DIALOG
#define FDS_OPEN_DIALOG   0x100
#endif

#ifndef FDS_SAVEAS_DIALOG
#define FDS_SAVEAS_DIALOG 0x200
#endif

#ifndef FDS_ENABLEFILELB
#define FDS_ENABLEFILELB  0x800
#endif

#define DID_CONTENTS_LB   0x2001
#define DID_CONTENTS_CB   0x2003
#define DID_AUTOSTART_PB  0x2005
#define DID_DIRUP         0x2002
#define DID_DIR_SELECTED  0x2006
#define DID_FFILTER_CB    0x2007
#define DID_ACTION_CB     0x2018
#define DID_SUBACTION_CB  0x2019
#define DID_FONTNAME_LB   0x2020
#define NA                0xFFFF // (unsigned short)-1

extern void ViceFileDialog(HWND hwnd, ULONG action, ULONG fl);
