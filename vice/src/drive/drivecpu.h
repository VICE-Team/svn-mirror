/*
 * drivecpu.h - Definitions of the 6502 processor in the Commodore 1541,
 * 1571 and 1581 floppy disk drives.
 *
 * Written by
 *   Ettore Perazzoli (ettore@comm2000.it)
 *   Andreas Boose (boose@unixserv.rz.fh-hannover.de)
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

#ifndef _DRIVECPU_H
#define _DRIVECPU_H

#include "types.h"

#include "alarm.h"
#include "clkguard.h"
#include "mos6510.h"
#include "mon.h"

/* drive0, device #8.  */

extern int drive0_traceflg;
extern int drive0_rmw_flag;
extern alarm_context_t drive0_alarm_context;
extern clk_guard_t drive0_clk_guard;

extern monitor_interface_t drive0_monitor_interface;

extern void drive0_cpu_init(int type);
extern void drive0_cpu_reset(void);
extern void drive0_cpu_sleep(void);
extern void drive0_cpu_wake_up(void);
extern CLOCK drive0_cpu_prevent_clk_overflow(CLOCK sub);
extern void drive0_cpu_set_sync_factor(unsigned int factor);
extern void drive0_cpu_early_init(void);

extern void REGPARM2 drive0_store(ADDRESS addr, BYTE value);
extern BYTE REGPARM1 drive0_read(ADDRESS addr);
extern void drive0_toggle_watchpoints(int flag);
extern void drive0_cpu_execute(void);
extern void drive0_set_bank_base(void);
extern int drive0_cpu_write_snapshot_module(snapshot_t *s);
extern int drive0_cpu_read_snapshot_module(snapshot_t *s);

/* drive1, device #9.  */

extern int drive1_traceflg;
extern int drive1_rmw_flag;
extern alarm_context_t drive1_alarm_context;
extern clk_guard_t drive1_clk_guard;

extern monitor_interface_t drive1_monitor_interface;

extern void drive1_cpu_init(int type);
extern void drive1_cpu_reset(void);
extern void drive1_cpu_sleep(void);
extern void drive1_cpu_wake_up(void);
extern CLOCK drive1_cpu_prevent_clk_overflow(CLOCK sub);
extern void drive1_cpu_set_sync_factor(unsigned int factor);
extern void drive1_cpu_early_init(void);

extern void REGPARM2 drive1_store(ADDRESS addr, BYTE value);
extern BYTE REGPARM1 drive1_read(ADDRESS addr);
extern void drive1_toggle_watchpoints(int flag);
extern void drive1_cpu_execute(void);
extern void drive1_set_bank_base(void);
extern int drive1_cpu_write_snapshot_module(snapshot_t *s);
extern int drive1_cpu_read_snapshot_module(snapshot_t *s);

#endif

