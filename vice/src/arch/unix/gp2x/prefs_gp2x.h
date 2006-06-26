/*
 * prefs_gp2x.h
 *
 * Written by
 *  Mike Dawson <mike@gp2x.org>
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

#ifndef _PREFS_GP2X_H
#define _PREFS_GP2X_H

extern int vkeyb_open;
extern int prefs_open;
extern int stats_open;

extern int display_width;
extern int hwscaling;
extern int tvout;
extern int tvout_pal;

extern int menu_bg, menu_fg, menu_hl;
#define C64_BLACK 0
#define C64_WHITE 1
#define C64_YELLOW 7
#define C64_RED 2
#define C64_GREEN 3

extern void draw_stats(unsigned char *screen);
extern void draw_prefs(unsigned char *screen);
extern void display_set();

#endif
