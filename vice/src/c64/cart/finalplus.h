/*
 * finalplus.h - Cartridge handling, Final cart.
 *
 * Written by
 *  groepaz <groepaz@gmx.net>
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

#ifndef VICE_FINALPLUS_H
#define VICE_FINALPLUS_H

#include <stdio.h>

#include "types.h"

extern void final_plus_freeze(void);
extern void final_plus_config_init(void);
extern void final_plus_config_setup(BYTE *rawcart);
extern int final_plus_bin_attach(const char *filename, BYTE *rawcart);
extern int final_plus_crt_attach(FILE *fd, BYTE *rawcart);
extern void final_plus_detach(void);
extern BYTE REGPARM1 final_plus_roml_read(WORD addr);
extern BYTE REGPARM1 final_plus_romh_read(WORD addr);
extern void REGPARM2 final_plus_romh_store(WORD addr, BYTE value);
extern BYTE REGPARM1 final_plus_1000_7fff_read(WORD addr);
extern void REGPARM2 final_plus_1000_7fff_store(WORD addr, BYTE value);
extern BYTE REGPARM1 final_plus_a000_bfff_read(WORD addr);
extern void REGPARM2 final_plus_a000_bfff_store(WORD addr, BYTE value);
extern BYTE REGPARM1 final_plus_c000_cfff_read(WORD addr);
extern void REGPARM2 final_plus_c000_cfff_store(WORD addr, BYTE value);
extern BYTE REGPARM1 final_plus_d000_dfff_read(WORD addr);
extern void REGPARM2 final_plus_d000_dfff_store(WORD addr, BYTE value);

#endif
