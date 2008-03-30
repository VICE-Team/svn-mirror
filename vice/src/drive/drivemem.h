/*
 * drivemem.h - Drive memory handling.
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
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

#ifndef _DRIVEMEM_H
#define _DRIVEMEM_H

#include "types.h"

struct drive_context_s;

extern void drive_mem_init(struct drive_context_s *drv, unsigned int type);

struct mem_ioreg_list_s;

extern struct mem_ioreg_list_s *drive0_ioreg_list_get(void);
extern struct mem_ioreg_list_s *drive1_ioreg_list_get(void);

extern BYTE REGPARM2 drive_read_rom(struct drive_context_s *drv,
                                    WORD address);

#endif

