/*
 * sid.h - MOS6581 (SID) emulation, hooks to actual implementation.
 *
 * Written by
 *  Dag Lem <resid@nimrod.no>
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

#ifndef _SID_ENGINE_H
#define _SID_ENGINE_H

#include "types.h"
#include "sound.h"

BYTE REGPARM1 sid_read(ADDRESS address);
BYTE REGPARM1 sid2_read(ADDRESS address);
void REGPARM2 sid_store(ADDRESS address, BYTE byte);
void REGPARM2 sid2_store(ADDRESS address, BYTE byte);
void sid_reset(void);

extern BYTE siddata[];

struct sid_engine_s {
    sound_t* (*open)(int speed, int cycles_per_sec, BYTE *sidstate);
    void (*close)(sound_t *psid);
    BYTE (*read)(sound_t *psid, ADDRESS addr);
    void (*store)(sound_t *psid, ADDRESS addr, BYTE val);
    void (*reset)(sound_t *psid, CLOCK cpu_clk);
    int (*calculate_samples)(sound_t *psid, SWORD *pbuf, int nr,
			     int *delta_t);
    void (*prevent_clk_overflow)(sound_t *psid, CLOCK sub);
    char* (*dump_state)(sound_t *psid);
};

typedef struct sid_engine_s sid_engine_t;

#endif /* _SID_ENGINE_H */
