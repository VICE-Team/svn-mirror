/*
 * sid.c - MOS6581 (SID) emulation.
 *
 * Written by
 *   Teemu Rantanen (tvr@cs.hut.fi)
 *
 * AIX support added by
 *   Chris Sharp (sharpc@hursley.ibm.com)
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

/* FIXME: This header should be machine-independent, so no #ifdefs!  */

#ifndef _SID_H
#define _SID_H

#include "types.h"

extern int sid_init_resources(void);
extern int sid_init_cmdline_options(void);

#if defined(CBM64) || defined(C128) || defined(VIC20)
#define SID
#endif

#if defined(CBM64) || defined(C128)
void REGPARM2 store_sid(ADDRESS address, BYTE byte);
BYTE REGPARM1 read_sid(ADDRESS address);
void reset_sid(void);
#endif
#if defined(VIC20)
void store_vic20sid(ADDRESS address, BYTE byte);
#endif
#if defined(PET)
void store_petsnd_onoff(int value);
void store_petsnd_rate(CLOCK t);
void store_petsnd_sample(BYTE value);
#endif

#ifdef SOUND
int flush_sound(int relative_speed);
void close_sound(void);
void initialize_sound(unsigned int clock_rate, unsigned int ticks_per_frame);
void sid_prevent_clk_overflow(CLOCK sub);
void suspend_sound(void);
void resume_sound(void);
#endif

#endif /* !defined (_SID_H) */
