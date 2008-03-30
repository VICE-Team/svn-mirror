/*
 * magicformel.h - Cartridge handling, Magic Formel cart.
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

#ifndef _MAGICFORMEL_H
#define _MAGICFORMEL_H

#include <stdio.h>

#include "types.h"

extern BYTE REGPARM1 magicformel_io1_read(WORD addr);
extern void REGPARM2 magicformel_io1_store(WORD addr, BYTE value);
extern BYTE REGPARM1 magicformel_io2_read(WORD addr);
extern void REGPARM2 magicformel_io2_store(WORD addr, BYTE value);

extern BYTE REGPARM1 magicformel_roml_read(WORD addr);
extern void REGPARM2 magicformel_roml_store(WORD addr, BYTE value);
extern void REGPARM2 magicformel_romh_store(WORD addr, BYTE value);
extern BYTE REGPARM1 magicformel_1000_7fff_read(WORD addr);
extern void REGPARM2 magicformel_1000_7fff_store(WORD addr, BYTE value);
extern BYTE REGPARM1 magicformel_a000_bfff_read(WORD addr);
extern void REGPARM2 magicformel_a000_bfff_store(WORD addr, BYTE value);
extern BYTE REGPARM1 magicformel_c000_cfff_read(WORD addr);
extern void REGPARM2 magicformel_c000_cfff_store(WORD addr, BYTE value);
extern BYTE REGPARM1 magicformel_d000_dfff_read(WORD addr);
extern void REGPARM2 magicformel_d000_dfff_store(WORD addr, BYTE value);

extern void magicformel_freeze(void);

extern void magicformel_config_init(void);
extern void magicformel_reset(void);
extern void magicformel_config_setup(BYTE *rawcart);
extern int magicformel_crt_attach(FILE *fd, BYTE *rawcart);
extern void magicformel_detach(void);

#endif

