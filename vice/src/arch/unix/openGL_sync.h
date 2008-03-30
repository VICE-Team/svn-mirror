/*
 * openGL_sync.h
 *
 * Written by
 * Martin Pottendorfer
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

#ifndef __openGL_sync_h__
#define __openGL_sync_h__
#include "vice.h"

struct ui_menu_entry_s;

void openGL_sync_init(void);
void openGL_sync_with_raster(void);
void openGL_sync_shutdown(void);
int openGL_available(int val);
void init_openGL(void);
void openGL_register_resources(void);

#endif /* __openGL_sync_h__ */
