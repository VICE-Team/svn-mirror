/*
 * true1541.h - Hardware-level Commodore 1541 disk drive emulation.
 *
 * Written by
 *  Daniel Sladic (sladic@eecg.toronto.edu)
 *  Ettore Perazzoli (ettore@comm2000.it)
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

#ifndef _TRUE1541_H
#define _TRUE1541_H

#include "types.h"
#include "drive.h"		/* DRIVE */
#include "1541cpu.h"

/* VIA 1 alarms */
#define	A_VIAD1T1	0
#define	A_VIAD1T2	1
/* VIA 2 alarms */
#define	A_VIAD2T1	2
#define	A_VIAD2T2	3

#define TRUE1541_NUMOFALRM 4

/* VIA 1 interrupts. */
#define	I_VIAD1T1	0
#define	I_VIAD1T2	1
#define	I_VIAD1SR	2
#define	I_VIAD1FL	3
#define I_VIAD1CB1	4
#define I_VIAD1CB2	5
#define I_VIAD1CA1	6
#define I_VIAD1CA2	7
/* VIA 2 interrupts. */
#define	I_VIAD2T1	8
#define	I_VIAD2T2	9
#define	I_VIAD2SR	10
#define	I_VIAD2FL	11
#define I_VIAD2CB1	12
#define I_VIAD2CB2	13
#define I_VIAD2CA1	14
#define I_VIAD2CA2	15

#define TRUE1541_NUMOFINT 16

/*
   1541 sync factor.

   The ``sync factor'' is computed approximately as:

     65536 * clk_1541 / clk_[c64|vic20]

   where `clk_1541' is fixed to 1 MHz, while `clk_[c64|vic20]' depends on the
   video timing (PAL or NTSC).

 */

/* Are these exact?  */

#if defined(CBM64) || defined(C128)
#define TRUE1541_PAL_SYNC_FACTOR	66516   /* Maybe 66517? */
#define TRUE1541_NTSC_SYNC_FACTOR	64094
#endif
#if defined(VIC20)
#define TRUE1541_PAL_SYNC_FACTOR	59126
#define TRUE1541_NTSC_SYNC_FACTOR	64079
#endif

#define TRUE1541_ROM_SIZE		16384
#define TRUE1541_RAM_SIZE		2048

#define TRUE1541_ROM_CHECKSUM		1976666

/* Extended disk image handling.  */
#define TRUE1541_EXTEND_NEVER		0
#define TRUE1541_EXTEND_ASK		1	
#define TRUE1541_EXTEND_ACCESS		2

/* 1541 idling methods.  */
#define TRUE1541_IDLE_SKIP_CYCLES	0
#define TRUE1541_IDLE_TRAP_IDLE		1

/* Define this to enable full emulation of disk rotation.  */
#define TRUE1541_ROTATE

/* Number of cycles before an attached disk becomes visible to the R/W head.
   This is mostly to make routines that auto-detect disk changes happy.  */
#define TRUE1541_ATTACH_DELAY           500000

/* Number of cycles the write protection is activated on detach.  */
#define TRUE1541_DETACH_DELAY           300000

/* -------------------------------------------------------------------------- */

extern int initialize_true1541(void);

#if defined(CBM64) || defined(C128)
extern void serial_bus_cpu_write(BYTE data);
extern BYTE serial_bus_cpu_read(void);
extern void parallel_cable_cpu_write(BYTE data, int handshake);
extern BYTE parallel_cable_cpu_read(void);
extern void parallel_cable_drive_write(BYTE data, int handshake);
extern BYTE parallel_cable_drive_read(int handshake);
#endif
#if defined(VIC20)
extern void serial_bus_pa_write(BYTE data);
extern void serial_bus_pcr_write(BYTE data);
extern BYTE serial_bus_pa_read(void);
#endif
extern void serial_bus_drive_write(BYTE data);
extern BYTE serial_bus_drive_read(void);
extern int true1541_trap_handler(void);
extern void true1541_set_half_track(int num);
extern void true1541_move_head(int step);
extern int true1541_sync_found(void);
extern int true1541_byte_ready(void);
extern void true1541_set_byte_ready(int val);
extern void true1541_rotate_disk(int mode_changed);
extern BYTE true1541_read_disk_byte(void);
extern void true1541_write_gcr(BYTE val);
extern int true1541_write_protect_sense(void);
extern void true1541_cpu_execute(void);
extern void true1541_cpu_reset(void);
extern void true1541_set_sync_factor(unsigned int factor);
extern void true1541_set_ntsc_sync_factor(void);
extern void true1541_set_pal_sync_factor(void);
extern void true1541_ack_sync_factor(void);
extern int true1541_enable(void);
extern void true1541_disable(void);
extern void true1541_ack_switch(void);
extern void true1541_reset(void);
extern int true1541_attach_floppy(DRIVE *floppy);
extern int true1541_detach_floppy(void);
extern void true1541_update_zone_bits(int zone);
extern void true1541_update_viad2_pcr(int pcrval);
extern void true1541_prevent_clk_overflow(void);
extern void true1541_motor_control(int flag);

extern BYTE true1541_rom[TRUE1541_ROM_SIZE];
extern BYTE true1541_ram[TRUE1541_RAM_SIZE];
extern int true1541_led_status;
extern int true1541_current_half_track;

#endif /* !_TRUE1541_H */
