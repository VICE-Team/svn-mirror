/*
 * tape.h - (Guess what?) Tape unit emulation.
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

#ifndef _TAPE_H
#define _TAPE_H

#include <stdio.h>

#include "types.h"
#include "traps.h"

extern int tape_detach_image(void);
extern int tape_attach_image(const char *name);
extern int tape_init(int buffer_pointer_addr, int st_addr,
                     int verify_flag_addr, int irqtmp, int irqval,
                     int stal_addr, int eal_addr, int kbd_buf_addr,
                     int kbd_buf_pending_addr, const trap_t *trap_list);
extern int tape_deinstall(void);
extern void tape_find_header_trap(void);
extern void tape_receive_trap(void);
extern char *tape_get_file_name(void);

#endif /* ndef _TAPE_H */
