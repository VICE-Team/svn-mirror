/*
 * sid.h - MOS6581 (SID) emulation.
 *
 * Written by
 *  Teemu Rantanen (tvr@cs.hut.fi)
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

#ifndef _SID_H
#define _SID_H

#include "sound.h"

extern int sid_init_resources(void);
extern int sid_init_cmdline_options(void);

extern void sid_prevent_clk_overflow(CLOCK sub);

extern void REGPARM2 store_sid(ADDRESS address, BYTE byte);
extern BYTE REGPARM1 read_sid(ADDRESS address);

extern void sid_reset(void);

#endif
