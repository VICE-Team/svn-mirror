/*
 * mousedrv.h - Mouse handling for OS/2
 *
 * Written by
 *  Thomas Bretz <tbretz@uni-sw.gwdg.de>
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
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#ifndef VICE_MOUSEDRV_H
#define VICE_MOUSEDRV_H

#include <os2.h>

#include "types.h"

extern int mousedrv_resources_init(void);
extern int mousedrv_cmdline_options_init(void);
extern void mousedrv_init(void);

extern void mousedrv_mouse_changed(void);

extern int mousedrv_get_x(void);
extern int mousedrv_get_y(void);
extern unsigned long mousedrv_get_timestamp(void);

/* ------------------ OS/2 specific ------------------- */

void mouse_button(HWND hwnd, ULONG msg, MPARAM mp1);

#endif
