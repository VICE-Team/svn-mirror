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

extern char *option_txt[];
extern char *blank_line;

extern void (*ui_handle_sidengine_resource)(int);
extern int (*ui_handle_X)(int);
extern void (*ui_draw_resid_string)(unsigned char *, int, int);
extern void (*ui_draw_memory_string)(unsigned char *, int, int, int);
extern int (*ui_set_ramblocks)(int);
extern void (*ui_attach_cart)(char *, int);

#define C64_BLACK  0
#define C64_WHITE  1
#define C64_YELLOW 7
#define C64_RED    2
#define C64_GREEN  3

#define AUTOSTART    0
#define START        1
#define ATTACH8      2
#define ATTACH9      3
#define X1           4
#define X2           5
#define X3           6
#define X4           7
#define X5           8
#define X6           9
#define X7          10
#define X8          11
#define RESET       12
#define BLANK1      13
#define SAVE_SNAP   14
#define JOYSTICK    15
#define FRAMESKIP   16
#define WARP        17
#define TRUEDRIVE   18
#define VDEVICES    19
#define SIDENGINE   20
#define SCALED      21
#define CENTRED     22
#define CPUSPEED    23
#define BLANK2      24
#define EXITVICE    25
#define NUM_OPTIONS 26
#define D64         27
#define LOADSTAR    28
#define LOADER      29
#define SOUND       30
#define LIMITSPEED  31
#define RCONTROL    32
#define BRIGHTNESS  33
#define CONTRAST    34
#define BLANK3      35
#define BLANK4      36
#define LOAD_SNAP   37
#define PRG         38
#define TVOUT       39

extern void draw_stats(unsigned char *screen);
extern void draw_prefs(unsigned char *screen);
extern void display_set();

#endif
