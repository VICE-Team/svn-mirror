/*
 * cia.h - Definitions for MOS6526 (CIA) chip emulation.
 *
 * Written by
 *  André Fachat (fachat@physik.tu-chemnitz.de)
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

#ifndef _C64_CIA_H
#define _C64_CIA_H

#include "types.h"

#include "cia.h"
#include "snapshot.h"

extern void cia1_init(void);
extern void reset_cia1(void);
extern void REGPARM2 store_cia1(ADDRESS addr, BYTE value);
extern BYTE REGPARM1 read_cia1(ADDRESS addr);
extern BYTE REGPARM1 peek_cia1(ADDRESS addr);
extern void cia1_prevent_clk_overflow(CLOCK sub);
extern void cia1_set_flag(void);
extern void cia1_set_sdr(BYTE received_byte);
extern void cia1_set_extended_keyboard_rows_mask(BYTE value);
extern void cia1_enable_extended_keyboard_rows(int flag);
extern int cia1_write_snapshot_module(snapshot_t *p);
extern int cia1_read_snapshot_module(snapshot_t *p);

extern void cia2_init(void);
extern void reset_cia2(void);
extern void REGPARM2 store_cia2(ADDRESS addr, BYTE value);
extern BYTE REGPARM1 read_cia2(ADDRESS addr);
extern BYTE REGPARM1 peek_cia2(ADDRESS addr);
extern void cia2_prevent_clk_overflow(CLOCK sub);
extern void cia2_set_flag(void);
extern void cia2_set_sdr(BYTE received_byte);
extern int cia2_write_snapshot_module(snapshot_t *p);
extern int cia2_read_snapshot_module(snapshot_t *p);

#endif				/* _C64_CIA_H */
