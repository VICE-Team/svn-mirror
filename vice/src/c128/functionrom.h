/*
 * functionrom.c
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

#ifndef _FUNCTIONROM_H
#define _FUNCTIONROM_H

#include "types.h"

extern BYTE int_function_rom[];
extern BYTE ext_function_rom[];

extern BYTE REGPARM1 internal_function_rom_read(WORD addr);
extern void REGPARM2 internal_function_rom_store(WORD addr, BYTE value);
extern BYTE REGPARM1 external_function_rom_read(WORD addr);
extern void REGPARM2 external_function_rom_store(WORD addr, BYTE value);

extern int functionrom_resources_init(void);
extern int functionrom_cmdline_options_init(void);

#endif

