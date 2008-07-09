/*
 * stardos.h - Cartridge handling, StarDOS cart.
 *
 * (w)2008 Groepaz/Hitmen
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

#ifndef _STARDOS_H
#define _STARDOS_H

#include <stdio.h>

#include "types.h"

extern int stardos_kernal_enabled;

extern BYTE REGPARM1 stardos_io1_read(WORD addr);
extern BYTE REGPARM1 stardos_io2_read(WORD addr);
extern BYTE REGPARM1 stardos_kernal_read(WORD addr);
extern BYTE REGPARM1 stardos_roml_read(WORD addr);

extern void stardos_config_init(void);
extern void stardos_reset(void);
extern void stardos_config_setup(BYTE *rawcart);
extern int stardos_bin_attach(const char *filename, BYTE *rawcart);
extern int stardos_crt_attach(FILE *fd, BYTE *rawcart);
extern void stardos_detach(void);

#endif
