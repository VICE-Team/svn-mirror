/*
 * maincpu.c - Emulation of the 6502 processor in the Commodore 1541 floppy
 * disk drive.
 *
 * Written by
 *   Ettore Perazzoli (ettore@comm2000.it)
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

/* This is the header for emulation of the MOS 6502 CPU in the 1541 drive. */

#ifndef _1541CPU_H
#define _1541CPU_H

#include "types.h"
#include "mos6510.h"
#include "mon.h"

extern mos6510_regs_t true1541_cpu_regs;
extern int true1541_rmw_flag;

extern CLOCK true1541_clk;
extern int true1541_traceflg;
extern int true1541_cpu_running;

extern monitor_interface_t true1541_monitor_interface;

extern void true1541_cpu_init(void);
extern void true1541_cpu_reset(void);
extern void true1541_cpu_sleep(void);
extern void true1541_cpu_wake_up(void);
extern int true1541_cpu_prevent_clk_overflow(void);

extern void REGPARM2 true1541_store(ADDRESS addr, BYTE value);
extern BYTE REGPARM1 true1541_read(ADDRESS addr);
extern void true1541_toggle_watchpoints(int flag);

#endif
