/*
 * snapshot64.h - Cartridge handling, Snapshot64 cart.
 *
 * Written by
 *  Groepaz <groepaz@gmx.net>
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

#ifndef VICE_SNAPSHOT64_H
#define VICE_SNAPSHOT64_H

#include "types.h"

extern BYTE REGPARM1 snapshot64_roml_read(WORD addr);
extern void REGPARM2 snapshot64_roml_store(WORD addr, BYTE value);

extern void snapshot64_freeze(void);

extern void snapshot64_config_init(void);
extern void snapshot64_config_setup(BYTE *rawcart);
extern int snapshot64_bin_attach(const char *filename, BYTE *rawcart);

extern void snapshot64_detach(void);

#endif
