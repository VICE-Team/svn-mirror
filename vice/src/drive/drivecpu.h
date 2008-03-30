/*
 * drivecpu.h - 6502 processor emulation of the Commodore 1541, 1541-II,
 *              1571, 1581, 2031 and 1001 floppy disk drive.
 *
 * Written by
 *   Ettore Perazzoli <ettore@comm2000.it>
 *   Andreas Boose <boose@linux.rz.fh-hannover.de>
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

#include "mon.h"
#include "snapshot.h"
#include "types.h"


/* avoid having to include drivetypes.h */
struct drive_context_s;

extern void drive_cpu_setup_context(struct drive_context_s *drv);

extern void drive_cpu_init(struct drive_context_s *drv, int type);
extern void drive_cpu_reset(struct drive_context_s *drv);
extern void drive_cpu_sleep(struct drive_context_s *drv);
extern void drive_cpu_wake_up(struct drive_context_s *drv);
extern CLOCK drive_cpu_prevent_clk_overflow(struct drive_context_s *drv, CLOCK sub);
extern void drive_cpu_set_sync_factor(struct drive_context_s *drv, unsigned int factor);
extern void drive_cpu_early_init(struct drive_context_s *drv);
extern void drive_cpu_reset_clk(struct drive_context_s *drv);

extern void REGPARM3 drive_store(struct drive_context_s *drv, ADDRESS addr, BYTE value);
extern BYTE REGPARM2 drive_read(struct drive_context_s *drv, ADDRESS addr);
extern void drive_toggle_watchpoints(struct drive_context_s *drv, int flag);
extern void drivex_cpu_execute(struct drive_context_s *drv, CLOCK clk_value);
extern void drive_set_bank_base(struct drive_context_s *drv);
extern int drive_cpu_write_snapshot_module(struct drive_context_s *drv, snapshot_t *s);
extern int drive_cpu_read_snapshot_module(struct drive_context_s *drv, snapshot_t *s);

/* to minimize changes in other modules */
#define drive0_cpu_execute(c)	drivex_cpu_execute(&drive0_context, c)
#define drive1_cpu_execute(c)	drivex_cpu_execute(&drive1_context, c)
#define drive0_cpu_early_init()	drive_cpu_early_init(&drive0_context)
#define drive1_cpu_early_init()	drive_cpu_early_init(&drive1_context)
#define drive0_set_bank_base()	drive_set_bank_base(&drive0_context)
#define drive1_set_bank_base()	drive_set_bank_base(&drive1_context)

/* don't use these pointers before the context is set up! */
extern monitor_interface_t *drive0_monitor_interface_ptr;
extern monitor_interface_t *drive1_monitor_interface_ptr;
extern cpu_int_status_t *drive0_int_status_ptr;
extern cpu_int_status_t *drive1_int_status_ptr;

#endif

