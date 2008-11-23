/*
 * prefs_gp2x.h
 *
 * Written by
 *  Mike Dawson <mike@gp2x.org>
 *  Mustafa 'GnoStiC' Tufan <mtufan@gmail.com>
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

#ifndef VICE_PREFS_GP2X_H
#define VICE_PREFS_GP2X_H

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
#define RESET        9
#define BLANK1      10

#define SAVE_SNAP   12
#define KEYMAP      13
#define JOYSTICK    14
#define USBJOYSTICK1    15
#define USBJOYSTICK2    16
#define FRAMESKIP   17
#define WARP        18
#define TRUEDRIVE   19
#define VDEVICES    20
#define SIDENGINE   21
#define SCALED      22
#define CENTRED     23
#define CPUSPEED    24
//#define BLANK2      24
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

extern void draw_stats(unsigned char *screen);
extern void draw_prefs(unsigned char *screen);
extern void display_set();
extern unsigned int cur_portusb1;
extern unsigned int cur_portusb2;

extern unsigned int mapped_up;
extern unsigned int mapped_down;
extern unsigned int mapped_left;
extern unsigned int mapped_right;

#endif
