/*
 * ui.h - A user interface for OS/2.
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

#ifndef _UI_H
#define _UI_H

#include "uiapi.h"

#include "types.h"
#include "cmdline.h"

// ------------------------- OS/2 only -------------------------
extern void WinOkDlg       (HWND hwnd, char *title, char *msg);
extern void WinError       (HWND hwnd, const char *format,...);
extern void ui_OK_dialog   (char *title, char *msg);
extern int  ui_yesno_dialog(HWND hwnd, char *title, char *msg);

#endif
