/*
 * c64io.h -- C64 io handling.
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

#ifndef VICE_C64IO_H
#define VICE_C64IO_H
 
#include "types.h"

#define IO_SOURCE_NONE                0
#define IO_SOURCE_ACIA                1
#define IO_SOURCE_C64_256K            2
#define IO_SOURCE_GEORAM              3
#define IO_SOURCE_RAMCART             4
#define IO_SOURCE_REU                 5
#define IO_SOURCE_TFE_RR_NET          6
#define IO_SOURCE_STEREO_SID          7
#define IO_SOURCE_ACTION_REPLAY       8
#define IO_SOURCE_ATOMIC_POWER        9
#define IO_SOURCE_EPYX_FASTLOAD      10
#define IO_SOURCE_FINAL1             11
#define IO_SOURCE_FINAL3             12
#define IO_SOURCE_IDE64              13
#define IO_SOURCE_IEEE488            14
#define IO_SOURCE_KCS                15
#define IO_SOURCE_MAGIC_FORMEL       16
#define IO_SOURCE_RR                 17
#define IO_SOURCE_SS4                18
#define IO_SOURCE_SS5                19
#define IO_SOURCE_WARPSPEED          20
#define IO_SOURCE_EMUID              21
#define IO_SOURCE_MIKRO_ASSEMBLER    22
#define IO_SOURCE_MMC64              23
#define IO_SOURCE_DIGIMAX            24
#define IO_SOURCE_ACTION_REPLAY4     25
#define IO_SOURCE_STARDOS            26
#define IO_SOURCE_MIDI               27
#define IO_SOURCE_ISEPIC             28

#define IO_DETACH_CART       0
#define IO_DETACH_RESOURCE   1

extern BYTE REGPARM1 c64io1_read(WORD addr);
extern void REGPARM2 c64io1_store(WORD addr, BYTE value);
extern BYTE REGPARM1 c64io2_read(WORD addr);
extern void REGPARM2 c64io2_store(WORD addr, BYTE value);

struct mem_ioreg_list_s;
extern void c64io_ioreg_add_list(struct mem_ioreg_list_s **mem_ioreg_list);

extern int io_source;

#endif
