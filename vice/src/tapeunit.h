/*
 * tapeunit.c - (Guess what?) Tape unit emulation.
 *
 * Written by
 *  Jouko Valta (jopi@stekt.oulu.fi)
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

#ifndef _TAPEUNIT_H
#define _TAPEUNIT_H

#include "tape.h"
#include "traps.h"

/* Global functions */
extern void tape_detach_image(TAPE *tape);
extern int tape_attach_image(TAPE *tape, const char *name, int mode);
extern int check_tape(void);
extern int tape_init(const trap_t *tape);
extern void findheader (void);
extern void writeheader (void);
/* extern void checkplay (void); */
extern void tapereceive (void);

#endif /* ndef _TAPEUNIT_H */
