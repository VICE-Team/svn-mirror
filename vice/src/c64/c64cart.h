/*
 * c64cart.h -- C64 cartridge memory interface.
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

#ifndef VICE_C64CART_H
#define VICE_C64CART_H

#include "types.h"

extern BYTE REGPARM1 roml_read(WORD addr);
extern void REGPARM2 roml_store(WORD addr, BYTE value);
extern BYTE REGPARM1 romh_read(WORD addr);
extern void REGPARM2 romh_store(WORD addr, BYTE value);
extern void REGPARM2 roml_no_ultimax_store(WORD addr, BYTE value);
extern void REGPARM2 romh_no_ultimax_store(WORD addr, BYTE value);

extern BYTE REGPARM1 ultimax_1000_7fff_read(WORD addr);
extern void REGPARM2 ultimax_1000_7fff_store(WORD addr, BYTE value);
extern BYTE REGPARM1 ultimax_a000_bfff_read(WORD addr);
extern void REGPARM2 ultimax_a000_bfff_store(WORD addr, BYTE value);
extern BYTE REGPARM1 ultimax_c000_cfff_read(WORD addr);
extern void REGPARM2 ultimax_c000_cfff_store(WORD addr, BYTE value);
extern BYTE REGPARM1 ultimax_d000_dfff_read(WORD addr);
extern void REGPARM2 ultimax_d000_dfff_store(WORD addr, BYTE value);

extern BYTE REGPARM1 cartridge_read_io1(WORD addr);
extern BYTE REGPARM1 cartridge_read_io2(WORD addr);
extern void REGPARM2 cartridge_store_io1(WORD addr, BYTE value);
extern void REGPARM2 cartridge_store_io2(WORD addr, BYTE value);
extern void REGPARM1 cartridge_decode_address(WORD addr);

extern void cartridge_init_config(void);
extern void cartridge_release_freeze(void);

/* Handle nmi/reset acknowledge for cartridge emulation. (CARTRIDGE_EXPERT) */
extern void cartridge_ack_nmi_reset(void);

/* Flag: Ultimax (VIC-10) memory configuration enabled.  */
extern unsigned int cart_ultimax_phi1;
extern unsigned int cart_ultimax_phi2;

/* Exansion port ROML/ROMH images.  */
extern BYTE roml_banks[], romh_banks[];

/* Expansion port ROML/ROMH/RAM banking.  */
extern int roml_bank, romh_bank, export_ram;

/* Cartridge RAM limit = 32Kb */
#define C64CART_RAM_LIMIT               0x8000

/* Expansion port signals.  */
typedef struct {
    BYTE exrom;
    BYTE game;
} export_t;

extern export_t export;
extern int mem_cartridge_type;

extern int c64cart_type;

extern int try_cartridge_init(int c);

#endif

