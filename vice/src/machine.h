/*
 * machine.h  - Interface to machine-specific implementations.
 *
 * Written by
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

#ifndef _MACHINE_H
#define _MACHINE_H

#include "types.h"

/* The following stuff must be defined once per every emulated CBM machine.  */

/* Name of the machine.  */
extern const char machine_name[];

/* A little handier way to identify the machine: */
#define VICE_MACHINE_NONE      0
#define VICE_MACHINE_C64       1
#define VICE_MACHINE_C128      2
#define VICE_MACHINE_VIC20     3
#define VICE_MACHINE_PET       4
#define VICE_MACHINE_CBM2      5

extern int machine_class;
extern int psid_mode;

/* Initialize the machine's resources.  */
int machine_init_resources(void);

/* Initialize the machine's command-line options.  */
int machine_init_cmdline_options(void);

/* Initialize the machine.  */
int machine_init(void);

/* Reset the machine.  */
void machine_reset(void);

/* Power-up the machine.  */
void machine_powerup(void);

/* Shutdown th emachine.  */
void machine_shutdown(void);

/* Set the state of the RESTORE key (!=0 means pressed); returns 1 if key had
   been used.  */
int machine_set_restore_key(int);

/* Get the number of CPU cylces per second.  This is used in various parts.  */
long machine_get_cycles_per_second(void);

/* Set the screen refresh rate, as this is variable in the CRTC */
void machine_set_cycles_per_frame(long);

/* Write a snapshot.  */
int machine_write_snapshot(const char *name, int save_roms, int save_disks);

/* Read a snapshot.  */
int machine_read_snapshot(const char *name);

/* handle pending interrupts - needed by libsid.a */
void machine_handle_pending_alarms(int num_write_cycles);

/* Autodetect PSID file. */
int machine_autodetect_psid(const char *name);

#endif /* _MACHINE_H */
