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

int cartridge_init_resources(void);

extern int cartridge_attach_image(int type, char *filename);
extern void cartridge_detach_image(void);

/* Known cartridge types.  */

#define CARTRIDGE_NONE		-65535
#define CARTRIDGE_GENERIC_8KB	-2
#define CARTRIDGE_GENERIC_16KB	-1
#define CARTRIDGE_CRT		0
#define CARTRIDGE_ACTION_REPLAY	1

