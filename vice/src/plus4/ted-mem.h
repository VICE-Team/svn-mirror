/*
 * vicii-mem.h - Memory interface for the MOS6569 (VIC-II) emulation.
 *
 * Written by
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
 *  Ettore Perazzoli <ettore@comm2000.it>
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

#ifndef _TED_MEM_H
#define _TED_MEM_H

#include "types.h"

extern void REGPARM2 ted_store(ADDRESS addr, BYTE value);
extern BYTE REGPARM1 ted_read(ADDRESS addr);
extern BYTE REGPARM1 ted_peek(ADDRESS addr);
extern BYTE REGPARM1 colorram_read(ADDRESS addr);
extern void REGPARM2 colorram_store(ADDRESS addr, BYTE value);
extern void REGPARM2 vicii_mem_vbank_store(ADDRESS addr, BYTE value);
extern void REGPARM2 vicii_mem_vbank_39xx_store(ADDRESS addr, BYTE value);
extern void REGPARM2 vicii_mem_vbank_3fxx_store(ADDRESS addr, BYTE value);

#endif

