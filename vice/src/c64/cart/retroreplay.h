/*
 * retroreplay.h - Cartridge handling, Retro Replay cart.
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

#ifndef _RETROREPLAY_H
#define _RETROREPLAY_H

#include <stdio.h>

#include "types.h"

extern BYTE REGPARM1 retroreplay_io1_read(ADDRESS addr);
extern void REGPARM2 retroreplay_io1_store(ADDRESS addr, BYTE value);
extern BYTE REGPARM1 retroreplay_io2_read(ADDRESS addr);
extern void REGPARM2 retroreplay_io2_store(ADDRESS addr, BYTE value);

extern BYTE REGPARM1 retroreplay_roml_read(ADDRESS addr);
extern void REGPARM2 retroreplay_roml_store(ADDRESS addr, BYTE value);

extern void retroreplay_freeze(void);
extern int retroreplay_freeze_allowed(void);

extern void retroreplay_config_init(void);
extern void retroreplay_config_setup(BYTE *rawcart);
extern int retroreplay_bin_attach(const char *filename, BYTE *rawcart);

#endif

