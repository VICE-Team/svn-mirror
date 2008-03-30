/*
 * resid.h - reSID interface code
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

#ifndef _RESID_H
#define _RESID_H

sound_t *resid_sound_machine_open(int speed, int cycles_per_sec,
				  int filters_enabled, BYTE *sidstate,
				  CLOCK clk);
void resid_sound_machine_close(sound_t *psid);
BYTE resid_sound_machine_read(sound_t *psid, ADDRESS addr, CLOCK clk);
void resid_sound_machine_store(sound_t *psid, ADDRESS addr, BYTE byte,
			       CLOCK clk);
int resid_sound_machine_calculate_samples(sound_t *psid, SWORD *pbuf, int nr);
void resid_sound_machine_init(void);
void resid_sound_machine_prevent_clk_overflow(sound_t *psid, CLOCK sub);
char *resid_sound_machine_dump_state(sound_t *psid);

#endif
