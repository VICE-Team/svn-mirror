/*
 * gp2xsys.h
 *
 * Written by
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
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

#ifndef VICE_GP2XSYS_H
#define VICE_GP2XSYS_H

#include "vice.h"

#include "types.h"

#ifdef GP2X
#define MINIMAL_940T
#include "minimal.h"

#define gp2x_video_setpalette gp2x_video_RGB_setpalette
#define gp2x_screen8 gp2x_video_RGB[0].screen

#define GP2X_SYS_CPU_SPEED 200
#define MIN_CPU_SPEED 150
#define MAX_CPU_SPEED 280
#define CPU_DELTA 5

extern volatile unsigned short *gp2x_memregs;
#endif

#ifdef WIZ
#include "wiz_lib.h"

#define GP2X_SYS_CPU_SPEED 650
#define MIN_CPU_SPEED 300
#define MAX_CPU_SPEED 800
#define CPU_DELTA 50

#endif

#define MENU_HEIGHT 25
#define MENU_X      32
#define MENU_Y      16
#define MENU_LS     MENU_Y + 8

extern void gp2x_poll_usb_input(void);
extern void gp2x_handle_usb_cursor_pos(int cursor_pos, int val);
extern void gp2x_draw_usb_strings(unsigned char *screen, int width);

extern void set_FCLK(unsigned MHZ);
extern int display_setting(int hwscale, int width);
extern void gp2x_sprintf(char *command, char *cwd, char *name);
extern void gp2x_draw_scaling_string(unsigned char *screen, int width, int hwscale);

extern void gp2xsys_init(int ticks_per_second, int bpp, int rate, int bits, int stereo, int Hz);
extern void gp2xsys_shutdown(void);

extern void gp2x_battery_led(int status);
extern void gp2x_tvout_pal(void);
extern void gp2xsys_video_flip(void);
extern void gp2x_screen_source(register unsigned long *source, register unsigned long *screen, register int xoff, register int yoff, register int buf_width, int hwscale);
extern void gp2xsys_video_color8(int i, BYTE red, BYTE green, BYTE blue);

#endif
