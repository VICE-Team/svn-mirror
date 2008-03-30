/*
 * kbdbuf.h - Kernal keyboard buffer handling for VICE.
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

#ifndef _KBDBUF_H
#define _KBDBUF_H

/* Struct to access the kernal buffer.  */
typedef struct {
    
    /* First location of the buffer.  */
    int location;

    /* Location that stores the number of characters pending in the
       buffer.  */
    int num_pending_location;

    /* Maximum number of characters that fit in the buffer.  */
    int size;
    
} kernal_kbd_buf_t;

extern kernal_kbd_buf_t kernal_kbd_buf;

int kbd_buf_feed(const char *s);
void kbd_buf_flush(void);

#endif
