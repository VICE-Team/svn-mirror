/*
 * wiz_funcs.c - WIZ specific functions.
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

#include "vice.h"

#include <stdio.h>
#include <string.h>

#include "gp2xsys.h"

void gp2x_poll_usb_input(void)
{
}

void gp2x_handle_usb_cursor_pos(int cursor_pos, int val)
{
}

void gp2x_draw_usb_strings(unsigned char *screen, int width)
{
}

void set_FCLK(unsigned MHZ)
{
    wiz_set_clock(MHZ);
}

int display_setting(int hwscale, int old_width)
{
    return old_width;
}

void gp2x_sprintf(char *command, char *cwd, char *name)
{
    sprintf(command, "unzip -o %s/%s -d ./tmp ", cwd, name);
}

void gp2x_draw_scaling_string(unsigned char *screen, int width, int hwscale)
{
    draw_ascii_string(screen, width, MENU_X + (8 * 21), MENU_Y + (8 * SCALED), "1:1", menu_fg, menu_bg);
}

void gp2x_draw_centred_string(unsigned char *screen, int width, int centred)
{
    draw_ascii_string(screen, width, MENU_X + (8 * 21), MENU_Y + (8 * CENTRED), "on", menu_fg, menu_bg);
}

void gp2xsys_init(int ticks_per_second, int bpp, int rate, int bits, int stereo, int Hz)
{
    wiz_init(bpp, rate * 2, bits, stereo);
}

void gp2x_battery_led(int status)
{
}

void gp2x_tvout_pal(void)
{
}

void gp2xsys_video_flip(void)
{
    wiz_video_flip_single();
}

void gp2x_screen_source(register unsigned long *source, register unsigned long *screen, register int xoff, register int yoff, register int buf_width, int hwscale)
{
    for (y = 240; y--;) {
        for (x = 320 / 4; x--;) {
            screen[(y * (320 / 4)) + x] = source[((y + yoff) * (buf_width)) + x + (xoff / 4)];
        }
    }
}

void gp2xsys_video_color8(int i, BYTE red, BYTE green, BYTE blue)
{
    wiz_video_color8(i, red, green, blue);
}

void gp2xsys_shutdown(void)
{
    wiz_deinit();
}
