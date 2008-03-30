/*
 * c64cart.h -- C64 cartridge memory interface.
 *
 * Written by
 *  Andreas Boose (boose@unixserv.rz.fh-hannover.de)
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

#ifndef _C64CART_H
#define _C64CART_H

#include "types.h"

extern BYTE REGPARM1 cartridge_read_io1(ADDRESS addr);
extern BYTE REGPARM1 cartridge_read_io2(ADDRESS addr);
extern void REGPARM2 cartridge_store_io1(ADDRESS addr, BYTE value);
extern void REGPARM2 cartridge_store_io2(ADDRESS addr, BYTE value);
extern void cartridge_init_config(void);

/* Flag: Ultimax (VIC-10) memory configuration enabled.  */
extern int ultimax;

/* Exansion port ROML/ROMH images.  */
extern BYTE roml_banks[], romh_banks[];

/* Expansion port ROML/ROMH/RAM banking.  */
extern int roml_bank, romh_bank, export_ram;

/* Expansion port signals.  */
typedef struct {
    BYTE exrom;
    BYTE game;
} export_t;

extern export_t export;
extern int mem_cartridge_type;

#endif 

