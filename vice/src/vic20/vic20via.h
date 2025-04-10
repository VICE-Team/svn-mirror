/*
 * via20via.h - VIC20 VIA emulation.
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
 *  Andre Fachat <fachat@physik.tu-chemnitz.de>
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

#ifndef VICE_VIC20VIA_H
#define VICE_VIC20VIA_H

#include "types.h"

struct machine_context_s;
struct via_context_s;

void vic20via1_setup_context(struct machine_context_s *machine_context);
void via1_init(struct via_context_s *via_context);
void via1_store(uint16_t addr, uint8_t byte);
uint8_t via1_read(uint16_t addr);
uint8_t via1_peek(uint16_t addr);

void vic20via2_setup_context(struct machine_context_s *machine_context);
void via2_init(struct via_context_s *via_context);
void via2_store(uint16_t addr, uint8_t byte);
uint8_t via2_read(uint16_t addr);
uint8_t via2_peek(uint16_t addr);

void via1_set_tape_sense(int v);
void via1_set_tape_write_in(int v);
void via1_set_tape_motor_in(int v);
void via1_check_lightpen(void);

extern int vic20_vflihack_userport;

#endif
