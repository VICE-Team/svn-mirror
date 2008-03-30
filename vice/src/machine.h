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

/* Initialize the machine's resources.  */
int machine_init_resources(void);

/* Initialize the machine's command-line options.  */
int machine_init_cmdline_options(void);

/* Initialize the machine.  */
int machine_init(void);

/* Reset the machine.  */
void machine_reset(void);

/* Shutdown th emachine.  */
void machine_shutdown(void);

#endif /* _MACHINE_H */
