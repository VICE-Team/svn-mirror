/*
 * gp2x_funcs.c - GP2X specific functions.
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
#include "prefs_gp2x.h"

/* clock setting code from:
  cpuctrl for GP2X
    Copyright (C) 2005  Hermes/PS2Reality 
	Modified by Vimacs using Robster's Code and god_at_hell(LCD)
*/
#define SYS_CLK_FREQ 7372800
void set_FCLK(unsigned MHZ)
{
    unsigned v;
    unsigned mdiv,pdiv = 3,scale = 0;

    MHZ *= 1000000;
    mdiv = (MHZ * pdiv) / SYS_CLK_FREQ;
    mdiv = ((mdiv - 8) << 8) & 0xff00;
    pdiv = ((pdiv - 2) << 2) & 0xfc;
    scale &= 3;
    v = mdiv | pdiv | scale;
    gp2x_memregs[0x910 >> 1] = v;

    //GnoStiC: (hopefully) prevent fast speed change crashes
    asm volatile ("nop"::);
    asm volatile ("nop"::);
    asm volatile ("nop"::);
    asm volatile ("nop"::);
}

int display_setting(int hwscale, int old_width)
{
    int width;

    if (hwscale) {
        width = 384; // 384*272
        gp2x_memregs[0x290c >> 1] = width;		/* screen width */
        if (tvout) {
            gp2x_memregs[0x2906 >> 1] = 614;			/* scale horizontal */
            if (tvout_pal) {
                gp2x_memregs[0x2908 >> 1] = 384;	/* scale vertical PAL */
            } else {
                gp2x_memregs[0x2908>>1] = 460;		/* scale vertical NTSC */
            }
        } else {
            gp2x_memregs[0x2906 >> 1] = 1228;			/* scale horizontal */
            gp2x_memregs[0x2908>>1] = 430;			/* vertical */
        }
    } else {
        width = 320; //320*240
        gp2x_memregs[0x290c >> 1] = width;		/* screen width */
        if (tvout) {
            gp2x_memregs[0x2906 >> 1] = 420;			/* scale horizontal */
        } else {
            gp2x_memregs[0x2906 >> 1] = 1024;			/* scale horizontal */
        }
        gp2x_memregs[0x2908 >> 1] = 320;			/* scale vertical */
    }
    return width;
}

void gp2x_sprintf(char *command, char *cwd, char *name)
{
    sprintf(command, "./unzip -o -d ./tmp %s/%s", cwd, name);
}

void gp2x_draw_scaling_string(unsigned char *screen, int width, int hwscale)
{
    if (hwscale) {
        draw_ascii_string(screen, width, MENU_X + (8 * 21), MENU_Y + (8 * SCALED), "scaled", menu_fg, menu_bg);
    } else {
        draw_ascii_string(screen, width, MENU_X + (8 * 21), MENU_Y + (8 * SCALED), "1:1", menu_fg, menu_bg);
    }
}

void gp2x_draw_centred_string(unsigned char *screen, int width, int centred)
{
   if (centred) {
        draw_ascii_string(screen, width, MENU_X + (8 * 21), MENU_Y + (8 * CENTRED), "on", menu_fg, menu_bg);
    } else {
        draw_ascii_string(screen, width, MENU_X + (8 * 21), MENU_Y + (8 * CENTRED), "off", menu_fg, menu_bg);
    }
}

void gp2xsys_init(int ticks_per_second, int bpp, int rate, int bits, int stereo, int Hz)
{
    gp2x_init(ticks_per_second, bpp, rate, bits, stereo, Hz, 1);
}

void gp2x_battery_led(int status)
{
    if (status) {
        gp2x_memregs[0x106e >> 1] &= ~16; /* switch battery led on */
    } else {
        gp2x_memregs[0x106e >> 1] |= 16; /* switch battery led off */
    }
}

void gp2x_tvout_pal(void)
{
    tvout_pal = 1;
    if (gp2x_memregs[0x2800 >> 1] & 0x100) {
        tvout = 1;
        hwscaling = 1;
        if (gp2x_memregs[0x2818 >> 1] == 239) {
            tvout_pal = 0;
        }
        display_set();
    }
}

void gp2xsys_video_flip(void)
{
    gp2x_video_RGB_flip(1);
}

void gp2x_screen_source(register unsigned long *source, register unsigned long *screen, register int xoff, register int yoff, register int buf_width, int hwscale)
{
    int x, y;

    if (hwscale) {
        for (y = 272; y--;) {
            for (x = 384 / 4; x--;) {
                screen[(y * (384 / 4)) + x] = source[((y + (yoff - 16)) * (buf_width)) + x + ((xoff - 32) / 4)];
            }
        }
    } else {
        for (y = 240; y--;) {
            for (x = 320 / 4; x--;) {
                screen[(y * (320 / 4)) + x] = source[((y + yoff) * (buf_width)) + x + (xoff / 4)];
            }
        }
    }
}

void gp2xsys_video_color8(int i, BYTE red, BYTE green, BYTE blue)
{
    gp2x_video_RGB_color8(i, red, green, blue);
}

void gp2xsys_shutdown(void)
{
}
