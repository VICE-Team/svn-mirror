/*
 * ieeeflash64pia.h -- PIA chip emulation for IEEE Flash! 64
 *
 * Written by
 *  Jouko Valta <jopi@stekt.oulu.fi>
 *  Andre Fachat <fachat@physik.tu-chemnitz.de>
 *  Christopher Bongaarts <cab@bongalow.net>
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

#ifndef VICE_IEEFLASH64PIA_H
#define VICE_IEEFLASH64PIA_H

#include "types.h"

/* Signal values (for signaling edges on the control lines) */

#define PIA_SIG_CA1     0
#define PIA_SIG_CA2     1
#define PIA_SIG_CB1     2
#define PIA_SIG_CB2     3

#define PIA_SIG_FALL    0
#define PIA_SIG_RISE    1

/* ------------------------------------------------------------------------- */

struct snapshot_s;

extern void pia_if64_init(void);
extern void pia_if64_reset(void);
extern void pia_if64_signal(int line, int edge);
extern void pia_if64_store(uint16_t addr, uint8_t value);
extern uint8_t pia_if64_read(uint16_t addr);
extern uint8_t pia_if64_peek(uint16_t addr);

extern int pia1_dump(void);
extern int pia_if64_dump(void);

extern int pia_if64_snapshot_read_module(struct snapshot_s *);
extern int pia_if64_snapshot_write_module(struct snapshot_s *);

#endif
