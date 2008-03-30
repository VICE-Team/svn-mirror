/*
 * tape.h - Tape unit emulation.
 *
 * Written by
 *  Jouko Valta <jopi@stekt.oulu.fi>
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
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

#include "types.h"

#define TAPE_TYPE_T64 0
#define TAPE_TYPE_TAP 1

typedef struct tape_image_s {
    char *name;
    unsigned int read_only;
    unsigned int type;
    void *data;
} tape_image_t;

struct tape_file_record_s {
    BYTE name[17];
    BYTE type;
    ADDRESS start_addr;
    ADDRESS end_addr;
};
typedef struct tape_file_record_s tape_file_record_t;


extern tape_image_t *tape_image_dev1;

struct trap_s;

extern int tape_init(int buffer_pointer_addr, int st_addr,
                     int verify_flag_addr, int irqtmp, int irqval,
                     int stal_addr, int eal_addr, int kbd_buf_addr,
                     int kbd_buf_pending_addr, const struct trap_s *trap_list);
extern int tape_deinstall(void);
extern void tape_get_header(tape_image_t *tape_image, BYTE *name);
extern void tape_find_header_trap(void);
extern void tape_receive_trap(void);
extern void tape_find_header_trap_plus4(void);
extern void tape_receive_trap_plus4(void);
extern const char *tape_get_file_name(void);
extern int tape_tap_attched(void);

extern void tape_traps_install(void);
extern void tape_traps_deinstall(void);

extern tape_file_record_t *tape_get_current_file_record(
                          tape_image_t *tape_image);
extern int tape_seek_to_next_file(tape_image_t *tape_image,
                                  unsigned int allow_rewind);

extern int tape_internal_close_tape_image(tape_image_t *tape_image);
extern tape_image_t *tape_internal_open_tape_image(const char *name,
                                                   unsigned int read_only);
/* External tape image interface.  */
extern int tape_image_detach(unsigned int unit);
extern int tape_image_attach(unsigned int unit, const char *name);
extern int tape_image_open(tape_image_t *tape_image);
extern int tape_image_close(tape_image_t *tape_image);

#endif

