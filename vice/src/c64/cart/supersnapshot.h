/*
 * supersnapshot.h - Cartridge handling, Super Snapshot cart.
 *
 * Written by
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
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

#ifndef _SUPERSNAPSHOT_H
#define _SUPERSNAPSHOT_H

#include "types.h"

extern BYTE REGPARM1 supersnapshot_v4_io1_read(ADDRESS addr);
extern void REGPARM2 supersnapshot_v4_io1_store(ADDRESS addr, BYTE value);
extern BYTE REGPARM1 supersnapshot_v4_io2_read(ADDRESS addr);
extern void REGPARM2 supersnapshot_v4_io2_store(ADDRESS addr, BYTE value);

extern BYTE REGPARM1 supersnapshot_v5_io1_read(ADDRESS addr);
extern void REGPARM2 supersnapshot_v5_io1_store(ADDRESS addr, BYTE value);
extern BYTE REGPARM1 supersnapshot_v5_io2_read(ADDRESS addr);
extern void REGPARM2 supersnapshot_v5_io2_store(ADDRESS addr, BYTE value);

extern BYTE REGPARM1 supersnapshot_v4_roml_read(ADDRESS addr);
extern void REGPARM2 supersnapshot_v4_roml_store(ADDRESS addr, BYTE value);
extern BYTE REGPARM1 supersnapshot_v5_roml_read(ADDRESS addr);
extern void REGPARM2 supersnapshot_v5_roml_store(ADDRESS addr, BYTE value);

extern void supersnapshot_v4_freeze(void);
extern void supersnapshot_v5_freeze(void);

extern void supersnapshot_v4_config_init(void);
extern void supersnapshot_v4_config_setup(BYTE *rawcart);
extern int supersnapshot_v4_bin_attach(const char *filename, BYTE *rawcart);

extern void supersnapshot_v5_config_init(void);
extern void supersnapshot_v5_config_setup(BYTE *rawcart);
extern int supersnapshot_v5_bin_attach(const char *filename, BYTE *rawcart);

#endif

