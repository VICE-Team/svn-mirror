/*
 * statusbar.h
 *
 * Written by
 *  Andreas Matthies <andreas.matthies@arcormail.de>
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

#ifndef _STATUSBAR_H
#define _STATUSBAR_H

#include <allegro.h>

#define STATUSBAR_HEIGHT    12
#define STATUSBAR_WIDTH     320

/* current use of the bar:
    speed info:       2 -  98
    tape info:      112 - 166
    drive 1:        180 - 240
    drive 2:        250 - 310
*/

enum {
    STATUSBAR_COLOR_BLACK  = 230, /* this should be free */
    STATUSBAR_COLOR_WHITE,
    STATUSBAR_COLOR_GREY,
    STATUSBAR_COLOR_DARKGREY,
    STATUSBAR_COLOR_BLUE,
    STATUSBAR_COLOR_YELLOW,
    STATUSBAR_COLOR_RED,
    STATUSBAR_COLOR_GREEN
};

extern void statusbar_update(void);
extern void statusbar_disable(void);
extern int statusbar_init(void);
extern void statusbar_exit(void);
extern int statusbar_enabled(void);
extern void statusbar_reset_bitmaps_to_update(void);
extern void statusbar_append_bitmap_to_update(BITMAP *b);
extern void statusbar_set_width(int w);

#endif
