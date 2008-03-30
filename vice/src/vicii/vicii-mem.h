/*
 * vicii-mem.h - Memory interface for the MOS6569 (VIC-II) emulation.
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
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

#ifndef _VIC_II_MEM_H
#define _VIC_II_MEM_H

/* FIXME: Names should be changed.  */
void REGPARM2 store_vic (ADDRESS addr, BYTE value);
BYTE REGPARM1 read_vic (ADDRESS addr);
BYTE REGPARM1 peek_vic (ADDRESS addr);
BYTE REGPARM1 read_colorram (ADDRESS addr);
void REGPARM2 store_colorram (ADDRESS addr, BYTE value);
void REGPARM2 store_vbank (ADDRESS addr, BYTE value);
void REGPARM2 store_vbank_39xx (ADDRESS addr, BYTE value);
void REGPARM2 store_vbank_3fxx (ADDRESS addr, BYTE value);

#endif
