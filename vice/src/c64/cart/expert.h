/*
 * expert.h - Cartridge handling, Expert cart.
 *
 * Written by
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
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

#ifndef _EXPERT_H
#define _EXPERT_H

#include <stdio.h>

#include "types.h"

extern BYTE REGPARM1 expert_io1_read(ADDRESS addr);
extern void REGPARM2 expert_io1_store(ADDRESS addr, BYTE value);
extern BYTE REGPARM1 expert_io2_read(ADDRESS addr);
extern void REGPARM2 expert_io2_store(ADDRESS addr, BYTE value);

extern BYTE REGPARM1 expert_roml_read(ADDRESS addr);
extern void REGPARM2 expert_roml_store(ADDRESS addr, BYTE value);
extern BYTE REGPARM1 expert_romh_read(ADDRESS addr);
extern void REGPARM1 expert_decode_address(ADDRESS addr);

extern void expert_ack_nmi_reset(void);
extern void expert_freeze(void);

extern void expert_config_init(void);
extern void expert_config_setup(BYTE *rawcart);
extern int expert_bin_attach(const char *filename, BYTE *rawcart);
extern int expert_crt_attach(FILE *fd, BYTE *rawcart);

#endif

