/*
 * machspec.h  - Interface to machine-specific implementations.
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

#ifndef _MACHSPEC_H
#define _MACHSPEC_H

typedef struct {

    /* Machine name (e.g. "C64").  */
    const char *machine_name;

    /* Flag: does this machine have joysticks?  */
    int has_joystick;

    /* Flag: how many colors does this machine have?  */
    int num_colors;

    /* Flag: does this machine allow 1541 emulation?  */
    int has_true1541;

    /* Flag: does this machine have a tape recorder?  */
    int has_tape;

    /* Flag: does this machine have a IEEE488 emulation?  */
    int has_ieee4888;

    /* Flag: does this machine have sound capabilities?  */
    int has_sound;

    /* Flag: does this machine have a RAM Expansion unit?  */
    int has_reu;

    /* Flag: does this machine have hardware sprites?  */
    int has_sprites;

} machdesc_t;


/* The following stuff must be defined once per every emulated CBM machine.  */

/* Description of the machine's features.  */
machdesc_t machdesc;

/* Initialize the machine.  */
void machine_init(void);

/* Reset the machine.  */
void machine_reset(void);

/* Return nonzero if `addr' is in a trappable address space (i.e. BRK could
   trigger a trap).  */
int rom_trap_allowed(ADDRESS addr);

#endif /* _MACHSPEC_H */
