/*
 * drivecpu1.c - Template file of the 6502 processor in the Commodore
 * 1541, 1541-II, 1571, 1581 and 2031 floppy disk drive.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
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

#define DRIVE_CPU

#define mynumber 1

/* Define this to enable tracing of drive CPU instructions.
   Warning: this slows it down!  */
#undef TRACE

/* Force `TRACE' in unstable versions.  */
#if 0 && defined UNSTABLE && !defined TRACE
#define TRACE
#endif

#ifdef TRACE
/* Flag: do we trace instructions while they are executed?  */
int drive1_traceflg;
#endif

/* snapshot name */
#define MYCPU_NAME      "DRIVECPU1"

#define mydrive_alarm_context drive1_alarm_context
#define mydrive_rmw_flag drive1_rmw_flag
#define mydrive_bank_read drive1_bank_read
#define mydrive_bank_peek drive1_bank_peek
#define mydrive_bank_store drive1_bank_store
#define mydrive_toggle_watchpoints drive1_toggle_watchpoints

#define mydrive_read drive1_read
#define mydrive_store drive1_store
#define mydrive_trigger_reset drive1_trigger_reset
#define mydrive_set_bank_base drive1_set_bank_base
#define mydrive_monitor_interface drive1_monitor_interface
#define mydrive_cpu_execute drive1_cpu_execute
#define mydrive_int_status drive1_int_status
#define mydrive_mem_init drive1_mem_init
#define mydrive_cpu_init drive1_cpu_init
#define mydrive_cpu_wake_up drive1_cpu_wake_up
#define mydrive_cpu_sleep drive1_cpu_sleep
#define mydrive_cpu_reset drive1_cpu_reset
#define mydrive_cpu_prevent_clk_overflow drive1_cpu_prevent_clk_overflow
#define mydrive_cpu_set_sync_factor drive1_cpu_set_sync_factor
#define mydrive_cpu_write_snapshot_module drive1_cpu_write_snapshot_module
#define mydrive_cpu_read_snapshot_module drive1_cpu_read_snapshot_module
#define mydrive_traceflg drive1_traceflg
#define mydrive_clk_guard drive1_clk_guard
#define mydrive_cpu_early_init drive1_cpu_early_init
#define mydrive_cpu_reset_clk drive1_cpu_reset_clk

#define mymonspace e_disk9_space
#define IDENTIFICATION_STRING "DRIVE#9"

#define myvia1_reset via1d1_reset
#define myvia2_reset via2d1_reset
#define mycia1571_reset cia1571d1_reset
#define mycia1581_reset cia1581d1_reset
#define mywd1770_reset wd1770d1_reset
#define myriot1_reset riot1d1_reset
#define myriot2_reset riot2d1_reset

#define myvia1_read via1d1_read
#define myvia1_store via1d1_store
#define myvia2_read via2d1_read
#define myvia2_store via2d1_store
#define mycia1571_read cia1571d1_read
#define mycia1571_store cia1571d1_store
#define mycia1581_read cia1581d1_read
#define mycia1581_store cia1581d1_store
#define mywd1770_read wd1770d1_read
#define mywd1770_store wd1770d1_store
#define myriot1_read riot1d1_read
#define myriot1_store riot1d1_store
#define myriot2_read riot2d1_read
#define myriot2_store riot2d1_store

#define myvia1_init via1d1_init
#define myvia2_init via2d1_init
#define mycia1571_init cia1571d1_init
#define mycia1581_init cia1581d1_init
#define mywd1770_init wd1770d1_init
#define myriot1_init riot1d1_init
#define myriot2_init riot2d1_init

#define myfdc_reset fdc1_reset
#define myfdc_init fdc1_init

#include "drivecpucore.c"

