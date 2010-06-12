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
extern BYTE REGPARM1 ultimax_romh_read_hirom(WORD addr);
extern void REGPARM2 romh_store(WORD addr, BYTE value);
extern void REGPARM2 roml_no_ultimax_store(WORD addr, BYTE value);
extern void REGPARM2 raml_no_ultimax_store(WORD addr, BYTE value);
extern void REGPARM2 romh_no_ultimax_store(WORD addr, BYTE value);

extern BYTE REGPARM1 ultimax_1000_7fff_read(WORD addr);
extern void REGPARM2 ultimax_1000_7fff_store(WORD addr, BYTE value);
extern BYTE REGPARM1 ultimax_a000_bfff_read(WORD addr);
extern void REGPARM2 ultimax_a000_bfff_store(WORD addr, BYTE value);
extern BYTE REGPARM1 ultimax_c000_cfff_read(WORD addr);
extern void REGPARM2 ultimax_c000_cfff_store(WORD addr, BYTE value);
extern BYTE REGPARM1 ultimax_d000_dfff_read(WORD addr);
extern void REGPARM2 ultimax_d000_dfff_store(WORD addr, BYTE value);

/* FIXME: these are for the "Main Slot" */

/* Flag: Ultimax (VIC-10) memory configuration enabled.  */
extern unsigned int cart_ultimax_phi1;
extern unsigned int cart_ultimax_phi2;

/* Exansion port ROML/ROMH images.  */
extern BYTE roml_banks[], romh_banks[];
/* Expansion port ROML/ROMH/RAM banking.  */
extern int roml_bank, romh_bank, export_ram;

/* Cartridge ROM limit = 512kB (MMCReplay) */
#define C64CART_ROM_LIMIT (1024*512)
/* Cartridge RAM limit = 512kB (MMCReplay) */
#define C64CART_RAM_LIMIT (1024*512)
/* maximum size of a full "all inclusive" cartridge image */
#define C64CART_IMAGE_LIMIT (C64CART_ROM_LIMIT+C64CART_RAM_LIMIT)

/* Expansion port signals.  */

/*
!IRQ        4     Interrupt Request line to 6502 (active low)
R/W         5     Read/Write (write active low)
DOT CLOCK   6     8.18 MHz video dot clock
!I/O1       7     I/O block 1 @ $ DE00-$DEFF (active low) unbuffered I/O
!GAME       8     active low ls ttl input
!EXROM      9     active low ls ttl input
!I/O2       10    I/O block 2 @ $DF00-$DFFF (active low) buff'ed ls ttl output
!ROML       11    8K decoded RAM/ROM block @ $8000 (active low) buffered ls ttl output
BA          12    Bus available signal from the VIC-II chip unbuffered 1 Is load max.
!DMA        13    Direct memory access request line (active low input) ls ttl input
D7-D0       14-21 Data bus bit 7-0 - unbuffered, 1 ls ttl load max
!ROMH       B     8K decoded RAM/ROM block @ $E000 buffered
!RESET      C     6502 RESET pin(active low) buff'ed ttl out/unbuff'ed in
!NMI        D     6502 Non Maskable Interrupt (active low) buff'ed ttl out, unbuff'ed in
phi2        E     Phase 2 system clock
A15-A0      F-Y   Address bus bit 0-15 - unbuffered, 1 ls ttl load max
*/

typedef struct {
    BYTE exrom;
    BYTE game;
} export_t;

extern export_t export;

extern int try_cartridge_init(int c); /* FIXME: don't use, remove */

#endif
