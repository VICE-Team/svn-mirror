/*
 * final.h - Cartridge handling, Final cart.
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

#ifndef _FINAL_H
#define _FINAL_H

#include <stdio.h>

#include "types.h"

extern BYTE REGPARM1 final_v1_io1_read(ADDRESS addr);
extern void REGPARM2 final_v1_io1_store(ADDRESS addr, BYTE value);
extern BYTE REGPARM1 final_v1_io2_read(ADDRESS addr);
extern void REGPARM2 final_v1_io2_store(ADDRESS addr, BYTE value);
extern BYTE REGPARM1 final_v3_io1_read(ADDRESS addr);
extern void REGPARM2 final_v3_io1_store(ADDRESS addr, BYTE value);
extern BYTE REGPARM1 final_v3_io2_read(ADDRESS addr);
extern void REGPARM2 final_v3_io2_store(ADDRESS addr, BYTE value);

extern BYTE REGPARM1 final_v1_roml_read(ADDRESS addr);
extern void REGPARM2 final_v1_roml_store(ADDRESS addr, BYTE value);
extern BYTE REGPARM1 final_v3_roml_read(ADDRESS addr);
extern void REGPARM2 final_v3_roml_store(ADDRESS addr, BYTE value);

extern void final_v1_freeze(void);
extern void final_v3_freeze(void);

extern void final_v1_config_init(void);
extern void final_v3_config_init(void);
extern void final_v1_config_setup(BYTE *rawcart);
extern void final_v3_config_setup(BYTE *rawcart);
extern int final_v1_crt_attach(FILE *fd, BYTE *rawcart);
extern int final_v3_crt_attach(FILE *fd, BYTE *rawcart);

#endif

