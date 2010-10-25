/*
 * c64cartmem.h -- C64 cartridge emulation, memory handling.
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

#ifndef VICE_C64CARTMEM_H
#define VICE_C64CARTMEM_H

#include "types.h"

/* mode_phiN bit 0,1 control exrom/game */
#define CMODE_8KGAME 0
#define CMODE_16KGAME 1
#define CMODE_RAM 2
#define CMODE_ULTIMAX 3

/* mode_phiN other bits select bank */
#define CMODE_BANK_SHIFT 2
#define CMODE_BANK_MASK 0x3f                    /* 64 Banks, meaning 512K max */

/* bits for wflag */
#define CMODE_READ  0
#define CMODE_WRITE 1                           /* config changes during a write access */
#define CMODE_RELEASE_FREEZE 2                  /* cartridge releases NMI condition */
#define CMODE_PHI2_RAM 4                        /* vic always sees RAM if set */
#define CMODE_EXPORT_RAM 8                      /* RAM connected to expansion port */
#define CMODE_TRIGGER_FREEZE_NMI_ONLY 16        /* Trigger NMI after config changed */
/* shift value for the above */
#define CMODE_RW_SHIFT  0
#define CMODE_RELEASE_FREEZE_SHIFT 1
#define CMODE_PHI2_RAM_SHIFT 2
#define CMODE_EXPORT_RAM_SHIFT 3
#define CMODE_TRIGGER_FREEZE_NMI_ONLY_SHIFT 4

extern void cartridge_config_changed(BYTE mode_phi1, BYTE mode_phi2, unsigned int wflag);

/* FIXME: these are for the "Main Slot" only, move to internal header and rename to cart_... */
extern void cartridge_romhbank_set(unsigned int bank);
extern void cartridge_romlbank_set(unsigned int bank);
extern BYTE export_ram0[];
extern BYTE roml_banks[], romh_banks[]; /* "Main Slot" ROML/ROMH images.  */
extern int roml_bank, romh_bank, export_ram; /* "Main Slot" ROML/ROMH/RAM banking.  */

/*
    this interface is used by: vicii-fetch.c, vicii.c, c64meminit.c, c64mem.c, c64memsc.c
*/

/* expansion port memory read/write hooks */
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

extern BYTE *ultimax_romh_phi1_ptr(WORD addr);
extern BYTE *ultimax_romh_phi2_ptr(WORD addr);
extern BYTE REGPARM1 ultimax_romh_phi1_read(WORD addr);
extern BYTE REGPARM1 ultimax_romh_phi2_read(WORD addr);

#endif
