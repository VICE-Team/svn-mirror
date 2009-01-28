/*
 * rexep256.h - Cartridge handling, REX EP256 cart.
 *
 * Written by
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
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

#ifndef VICE_REXEP256_H
#define VICE_REXEP256_H

#include <stdio.h>

#include "types.h"

extern BYTE REGPARM1 rexep256_io2_read(WORD addr);
extern void REGPARM2 rexep256_io2_store(WORD addr, BYTE value);

extern void rexep256_config_init(void);
extern void rexep256_config_setup(BYTE *rawcart);
extern int rexep256_crt_attach(FILE *fd, BYTE *rawcart);
extern void rexep256_detach(void);

#endif
