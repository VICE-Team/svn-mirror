/*
 * cartridge.h - Cartridge emulation.
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

/* FIXME: This should be split between `vic20cartridge.h' and
   `c64cartridge.h' IMO -- EP */
#ifndef _CARTRIDGE_H
#define _CARTRIDGE_H

#include "types.h"

extern int cartridge_init_resources(void);
extern int cartridge_init_cmdline_options(void);

extern int cartridge_attach_image(int type, const char *filename);
extern void cartridge_detach_image(void);
extern void cartridge_set_default(void);
extern void cartridge_trigger_freeze(void);
extern void cartridge_release_freeze(void);
extern const char *cartridge_get_file_name(ADDRESS addr);

/* Known cartridge types.  */

#define CARTRIDGE_ULTIMAX           -6
#define CARTRIDGE_SUPER_SNAPSHOT    -4
#define CARTRIDGE_GENERIC_8KB       -3
#define CARTRIDGE_GENERIC_16KB      -2
#define CARTRIDGE_NONE              -1
#define CARTRIDGE_CRT                0
#define CARTRIDGE_ACTION_REPLAY      1
#define CARTRIDGE_KCS_POWER          2
#define CARTRIDGE_FINAL_III          3
#define CARTRIDGE_SIMONS_BASIC       4
#define CARTRIDGE_OCEAN              5

/* VIC20: &1 -> 0=4k, 1=8k */
#define	CARTRIDGE_VIC20_DETECT		0
#define	CARTRIDGE_VIC20_4KB_2000	2
#define	CARTRIDGE_VIC20_8KB_2000	3
#define	CARTRIDGE_VIC20_4KB_6000	4
#define	CARTRIDGE_VIC20_8KB_6000	5
#define	CARTRIDGE_VIC20_4KB_A000	6
#define	CARTRIDGE_VIC20_8KB_A000	7
#define	CARTRIDGE_VIC20_4KB_B000	8

#endif
