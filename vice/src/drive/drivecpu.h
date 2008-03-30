/*
 * drivecpu.h - Definitions of the 6502 processor in the Commodore 1541,
 * 1571 and 1581 floppy disk drives.
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

#include "alarm.h"
#include "clkguard.h"
#include "mon.h"
#include "log.h"
#include "snapshot.h"
#include "types.h"


struct drive_context_s;

/* This defines the memory access for the drive CPU.  */
typedef BYTE REGPARM2 drive_read_func_t(struct drive_context_s *, ADDRESS);
typedef void REGPARM3 drive_store_func_t(struct drive_context_s *, ADDRESS,
                                         BYTE);

/* This table is used to approximate the sync between the main and the
   drive CPU, since the two clock rates are different.  */
#define MAX_TICKS 0x1000

#ifndef DRIVE_RAM_SIZE
#define DRIVE_RAM_SIZE	0x2000	/* FIXME -- move from drive.h? */
#endif


typedef struct drivecpu_context_s {

  int traceflg;
  /* This is non-zero each time a Read-Modify-Write instructions that accesses
     memory is executed.  We can emulate the RMW bug of the 6502 this way.  */
  int rmw_flag;	/* init to 0 */

  /* Interrupt/alarm status.  */
  struct cpu_int_status int_status;
  alarm_context_t alarm_context;
  /* Clk guard.  */
  clk_guard_t clk_guard;

  monitor_interface_t monitor_interface;

  /* Value of clk for the last time mydrive_cpu_execute() was called.  */
  CLOCK last_clk;
  /* Number of cycles in excess we executed last time mydrive_cpu_execute()
     was called.  */
  CLOCK last_exc_cycles;

  CLOCK cycle_accum;
  BYTE *d_bank_base;
  int d_bank_limit;	/* init to -1 */

  /* Information about the last executed opcode.  */
  opcode_info_t last_opcode_info;
  /* Public copy of the registers.  */
  mos6510_regs_t cpu_regs;

  BYTE *pageone;	/* init to NULL */

  MEMSPACE monspace;	/* init to e_disk[89]_space */

  unsigned long clk_conv_table[MAX_TICKS + 1];
  unsigned long clk_mod_table[MAX_TICKS + 1];

  /* FIXME. If all *core modules were reduced to shared code and private
     data sections, this wouldn't be necessary (and faster!) */
  void (*cia1571_reset)(void);
  void (*cia1581_reset)(void);
  void (*wd1770_reset)(void);
  void (*riot1_reset)(void);
  void (*riot2_reset)(void);
  BYTE REGPARM1 (*cia1571_read)(ADDRESS addr);
  void REGPARM2 (*cia1571_store)(ADDRESS addr, BYTE byte);
  BYTE REGPARM1 (*cia1581_read)(ADDRESS addr);
  void REGPARM2 (*cia1581_store)(ADDRESS addr, BYTE byte);
  BYTE REGPARM1 (*wd1770_read)(ADDRESS addr);
  void REGPARM2 (*wd1770_store)(ADDRESS addr, BYTE byte);
  BYTE REGPARM1 (*riot1_read)(ADDRESS addr);
  void REGPARM2 (*riot1_store)(ADDRESS addr, BYTE byte);
  BYTE REGPARM1 (*riot2_read)(ADDRESS addr);
  void REGPARM2 (*riot2_store)(ADDRESS addr, BYTE byte);
  void (*cia1571_init)(void);
  void (*cia1581_init)(void);
  void (*wd1770_init)(void);
  void (*riot1_init)(void);
  void (*riot2_init)(void);

  /* functions */
  drive_read_func_t  *read_func[0x101];
  drive_store_func_t *store_func[0x101];
  drive_read_func_t  *read_func_watch[0x101];
  drive_store_func_t *store_func_watch[0x101];
  drive_read_func_t  *read_func_nowatch[0x101];
  drive_store_func_t *store_func_nowatch[0x101];

  /* Drive RAM */
  BYTE drive_ram[DRIVE_RAM_SIZE];

  char snap_module_name[12];	/* init to "DRIVECPU[01]" */

  char identification_string[8];	/* init to "DRIVE#[89]" */

} drivecpu_context_t;


/* generic VIA-data */
typedef struct drivevia_context_s {

  BYTE via[16];
  int ifr;
  int ier;
  unsigned int tal;
  unsigned int tbl;
  CLOCK tau;
  CLOCK tbu;
  CLOCK tai;
  CLOCK tbi;
  int pb7;
  int pb7x;
  int pb7o;
  int pb7xx;
  int pb7sx;
  BYTE oldpa;
  BYTE oldpb;
  BYTE ila;
  BYTE ilb;
  int ca2_state;
  int cb2_state;
  alarm_t t1_alarm;
  alarm_t t2_alarm;
  log_t log;			/* init to LOG_ERR */

  CLOCK read_clk;		/* init to 0 */
  int read_offset;		/* init to 0 */
  BYTE last_read;		/* init to 0 */

  int irq_type;			/* I_... */
  int irq_line;			/* IK_... */

  char myname[12];		/* init to "DriveXViaY" */
  char my_module_name[8];	/* init to "VIAXDY" */

} drivevia_context_t;


/* VIA1 additional data */
typedef struct drivevia1_context_s {

  /* sigh... */
  void (*iec_write)(BYTE);
  BYTE (*iec_read)(void);
  void (*parallel_set_bus)(BYTE);
  void (*parallel_set_eoi)(char);	/* we may be able to eleminate these... */
  void (*parallel_set_dav)(char);
  void (*parallel_set_ndac)(char);
  void (*parallel_set_nrfd)(char);
  void (*parallel_cable_write)(BYTE, int);

  int parallel_id;

  int v_parieee_is_out;		/* init to 1 */
  struct iec_info_s *v_iec_info;

} drivevia1_context_t;


typedef struct drive_context_s {

  int mynumber;		/* init to [01] */
  CLOCK *clk_ptr;	/* shortcut to drive_clk[mynumber] */
  struct drive_s *drive_ptr;	/* shortcut to drive[mynumber] */

  drivecpu_context_t cpu;
  drivevia_context_t via1;
  drivevia1_context_t via1p;
  drivevia_context_t via2;

} drive_context_t;



extern drive_context_t drive0_context;
extern drive_context_t drive1_context;


extern void drive_cpu_setup_context(drive_context_t *drv);

extern void drive_cpu_init(drive_context_t *drv, int type);
extern void drive_cpu_reset(drive_context_t *drv);
extern void drive_cpu_sleep(drive_context_t *drv);
extern void drive_cpu_wake_up(drive_context_t *drv);
extern CLOCK drive_cpu_prevent_clk_overflow(drive_context_t *drv, CLOCK sub);
extern void drive_cpu_set_sync_factor(drive_context_t *drv, unsigned int factor);
extern void drive_cpu_early_init(drive_context_t *drv);
extern void drive_cpu_reset_clk(drive_context_t *drv);

extern void REGPARM3 drive_store(drive_context_t *drv, ADDRESS addr, BYTE value);
extern BYTE REGPARM2 drive_read(drive_context_t *drv, ADDRESS addr);
extern void drive_toggle_watchpoints(drive_context_t *drv, int flag);
extern void drivex_cpu_execute(drive_context_t *drv, CLOCK clk_value);
extern void drive_set_bank_base(drive_context_t *drv);
extern int drive_cpu_write_snapshot_module(drive_context_t *drv, snapshot_t *s);
extern int drive_cpu_read_snapshot_module(drive_context_t *drv, snapshot_t *s);

extern void drive_mem_init(drive_context_t *drv, int type);

/* to minimize changes in other modules */
#define drive0_cpu_execute(c)	drivex_cpu_execute(&drive0_context, c)
#define drive1_cpu_execute(c)	drivex_cpu_execute(&drive1_context, c)
#define drive0_cpu_early_init()	drive_cpu_early_init(&drive0_context)
#define drive1_cpu_early_init()	drive_cpu_early_init(&drive1_context)
#define drive0_set_bank_base()	drive_set_bank_base(&drive0_context)
#define drive1_set_bank_base()	drive_set_bank_base(&drive1_context)

#define drive0_monitor_interface	(drive0_context.cpu.monitor_interface)
#define drive1_monitor_interface	(drive1_context.cpu.monitor_interface)
#define drive0_int_status		(drive0_context.cpu.int_status)
#define drive1_int_status		(drive1_context.cpu.int_status)

#endif

