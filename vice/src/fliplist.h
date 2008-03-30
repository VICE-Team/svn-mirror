/*
 * fliplist.h
 *
 * Written by
 *  Martin Pottendorfer <Martin.Pottendorfer@aut.alcatel.at>
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

#ifndef _FLIPLIST_H
#define _FLIPLIST_H

#define FLIP_NEXT 1
#define FLIP_PREV 0


extern void flip_shutdown(void);
extern void flip_set_current(unsigned int unit, const char *image);
extern void flip_add_image(unsigned int unit);
extern void flip_remove(unsigned int unit, char *image);
extern void flip_attach_head(unsigned int unit, int direction);
extern void *flip_init_iterate(unsigned int unit);
extern void *flip_next_iterate(unsigned int unit);
extern char *flip_get_head(unsigned int unit);
extern char *flip_get_next(unsigned int unit);
extern char *flip_get_prev(unsigned int unit);
extern char *flip_get_image(void *fl);
extern unsigned int flip_get_unit(void *fl);

extern void flip_clear_list(unsigned int unit);
extern int flip_save_list(unsigned int unit, const char *filename);
extern int flip_load_list(unsigned int unit, const char *filename,
                          int autoattach);

#endif

