/*
 * isepic.h - ISEPIC cart emulation.
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

#ifndef VICE_ISEPIC_H
#define VICE_ISEPIC_H

#include "types.h"

extern int isepic_enabled;
extern int isepic_switch;

extern int isepic_resources_init(void);
extern int isepic_cmdline_options_init(void);
extern void isepic_freeze(void);

extern BYTE REGPARM1 isepic_reg_read(WORD addr);
extern void REGPARM2 isepic_reg_store(WORD addr, BYTE byte);
extern BYTE REGPARM1 isepic_romh_read(WORD addr);
extern void REGPARM2 isepic_romh_store(WORD addr, BYTE byte);
extern BYTE REGPARM1 isepic_window_read(WORD addr);
extern void REGPARM2 isepic_window_store(WORD addr, BYTE byte);
extern BYTE REGPARM1 isepic_roml_read(WORD addr);
extern void REGPARM2 isepic_roml_store(WORD addr, BYTE value);
extern BYTE REGPARM1 isepic_1000_7fff_read(WORD addr);
extern void REGPARM2 isepic_1000_7fff_store(WORD addr, BYTE value);
extern BYTE REGPARM1 isepic_a000_bfff_read(WORD addr);
extern void REGPARM2 isepic_a000_bfff_store(WORD addr, BYTE value);
extern BYTE REGPARM1 isepic_c000_cfff_read(WORD addr);
extern void REGPARM2 isepic_c000_cfff_store(WORD addr, BYTE value);

#endif
