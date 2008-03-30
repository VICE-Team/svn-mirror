/*
 * drivecpu.h - 6502 processor emulation of the Commodore 1541, 1541-II,
 *              1571, 1581, 2031 and 1001 floppy disk drive.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Andreas Boose <viceteam@t-online.de>
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

struct drive_context_s;
struct interrupt_cpu_status_s;
struct monitor_interface_s;
struct snapshot_s;

extern void drive_cpu_setup_context(struct drive_context_s *drv);

extern void drive_cpu_init(struct drive_context_s *drv, int type);
extern void drive_cpu_reset(struct drive_context_s *drv);
extern void drive_cpu_sleep(struct drive_context_s *drv);
extern void drive_cpu_wake_up(struct drive_context_s *drv);
extern CLOCK drive_cpu_prevent_clk_overflow(struct drive_context_s *drv,
                                            CLOCK sub);
extern void drive_cpu_early_init(struct drive_context_s *drv);
extern void drive_cpu_shutdown(struct drive_context_s *drv);
extern void drive_cpu_reset_clk(struct drive_context_s *drv);

extern void REGPARM3 drive_store(struct drive_context_s *drv, WORD addr,
                                 BYTE value);
extern BYTE REGPARM2 drive_read(struct drive_context_s *drv, WORD addr);
extern void drive_toggle_watchpoints(struct drive_context_s *drv, int flag);
extern void drivex_cpu_execute(struct drive_context_s *drv, CLOCK clk_value);
extern void drive_set_bank_base(struct drive_context_s *drv);
extern int drive_cpu_snapshot_write_module(struct drive_context_s *drv,
                                           struct snapshot_s *s);
extern int drive_cpu_snapshot_read_module(struct drive_context_s *drv,
                                          struct snapshot_s *s);

/* to minimize changes in other modules */
#define drive0_cpu_execute(c)   drivex_cpu_execute(&drive0_context, c)
#define drive1_cpu_execute(c)   drivex_cpu_execute(&drive1_context, c)

/* don't use these pointers before the context is set up! */
extern struct monitor_interface_s *drive0_get_monitor_interface_ptr(void);
extern struct monitor_interface_s *drive1_get_monitor_interface_ptr(void);

extern struct interrupt_cpu_status_s *drive0_int_status_ptr;
extern struct interrupt_cpu_status_s *drive1_int_status_ptr;

#endif

