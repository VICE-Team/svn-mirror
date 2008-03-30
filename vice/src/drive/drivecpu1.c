/*
 * drivecpu1.c - Template file of the 6502 processor in the Commodore
 * 1541, 1541-II, 1571, 1581 and 2031 floppy disk drive.
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
 *  Andreas Boose (boose@unixserv.rz.fh-hannover.de)
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

#include "vice.h"

#ifdef STDC_HEADERS
#include <stdio.h>
#endif

#include "6510core.h"
#include "alarm.h"
#include "ciad.h"
#include "drive.h"
#include "interrupt.h"
#include "mon.h"
#include "resources.h"
#include "snapshot.h"
#include "types.h"
#include "ui.h"
#include "viad.h"
#include "riotd.h"
#include "wd1770.h"
#include "fdc.h"

#include "drivecpu.h"

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

#define read_myvia1 read_via1d1
#define store_myvia1 store_via1d1
#define read_myvia2 read_via2d1
#define store_myvia2 store_via2d1
#define read_mycia1571 read_cia1571d1
#define store_mycia1571 store_cia1571d1
#define read_mycia1581 read_cia1581d1
#define store_mycia1581 store_cia1581d1
#define read_mywd1770 read_wd1770d1
#define store_mywd1770 store_wd1770d1
#define read_myriot1 read_riot1d1
#define store_myriot1 store_riot1d1
#define read_myriot2 read_riot2d1
#define store_myriot2 store_riot2d1

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

