
/*
 * attach.h - file system attach management
 *
 * Written by
 *  Andreas Boose (boose@unixserv.rz.fh-hannover.de)
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

#ifndef _ATTACH_H
#define _ATTACH_H

#include "types.h"
#include "vdrive.h"

int file_system_init_resources(void);
void file_system_init(void);
int file_system_set_hooks(int unit,
                          drive_attach_func_t attach_func,
                          drive_detach_func_t detach_func);
int file_system_attach_disk(int unit, const char *filename);
void file_system_detach_disk(int unit);

#endif
