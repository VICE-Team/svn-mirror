/*
 * uicolor.c - X11 color routines.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Teemu Rantanen <tvr@cs.hut.fi>
 *  Andreas Boose <viceteam@t-online.de>
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

#include <stdlib.h>

#ifdef ENABLE_NLS
#include <locale.h>
#endif

#include "color.h"
#include "lib.h"
#include "log.h"
#include "palette.h"
#include "types.h"
#include "uicolor.h"
#include "video.h"
#include "videoarch.h"


extern int screen;
extern GdkColor drive_led_on_red_pixel, drive_led_on_green_pixel,
drive_led_off_pixel, motor_running_pixel, tape_control_pixel;
extern GdkColor drive_led_on_red_pixels[16];
extern GdkColor drive_led_on_green_pixels[16];

static void uicolor_init_video_colors();

/*-----------------------------------------------------------------------*/

int uicolor_alloc_colors(video_canvas_t *c)
{
    int i;

    drive_led_off_pixel.red = 0;
    drive_led_off_pixel.green = 0;
    drive_led_off_pixel.blue = 0;

    drive_led_on_red_pixel.red = 0xff00;
    drive_led_on_red_pixel.green = 0;
    drive_led_on_red_pixel.blue = 0;

    drive_led_on_green_pixel.red = 0;
    drive_led_on_green_pixel.green = 0xff00;
    drive_led_on_green_pixel.blue = 0;

    motor_running_pixel.red = 0xff00;
    motor_running_pixel.green = 0xff00;
    motor_running_pixel.blue = 0x7f00;

    tape_control_pixel.red = 0xaf00;
    tape_control_pixel.green = 0xaf00;
    tape_control_pixel.blue = 0xaf00;

    /* different colors intensities for drive leds */
    for (i = 0; i < 16; i++)
    {
	drive_led_on_red_pixels[i].red = 0x1000*i + 0xf00;
	drive_led_on_red_pixels[i].green = 0;
	drive_led_on_red_pixels[i].blue = 0;

	drive_led_on_green_pixels[i].red = 0;
	drive_led_on_green_pixels[i].green =  0x1000*i + 0xf00;
	drive_led_on_green_pixels[i].blue = 0;
    }
    
    return uicolor_set_palette(c, c->palette);
}

int uicolor_set_palette(struct video_canvas_s *c, const palette_t *palette)
{
    unsigned int i;

    uicolor_init_video_colors();

    for (i = 0; i < palette->num_entries; i++) {
        palette_entry_t color = palette->entries[i];
        DWORD color_pixel = 
            (DWORD)color.red |
            (DWORD)color.green << 8 |
            (DWORD)color.blue << 16;
	    video_render_setphysicalcolor(((video_canvas_t*)c)->videoconfig, i,
            color_pixel, 24);
    }
    return 0;
}

static void uicolor_init_video_colors()
{
    short i;
    
    for (i = 0; i < 256; i++) {
	    video_render_setrawrgb(i, 
		    (DWORD)i, (DWORD)i << 8, (DWORD)i << 16
	    );
    }
    
    video_render_initraw();
}
