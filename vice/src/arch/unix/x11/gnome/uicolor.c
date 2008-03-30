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

#include <gnome.h>
#include <gdk/gdkx.h>
#ifdef ENABLE_NLS
#include <locale.h>
#endif

#include <X11/Xlib.h>

#include "color.h"
#include "log.h"
#include "palette.h"
#include "types.h"
#include "uicolor.h"
#include "utils.h"
#include "video.h"
#include "videoarch.h"
#include "x11ui.h"


extern int screen;
extern GdkColormap *colormap;
extern GdkColor *drive_led_on_red_pixel, *drive_led_on_green_pixel,
    *drive_led_off_pixel, *motor_running_pixel, *tape_control_pixel;

#define NUM_ENTRIES 5

static int uicolor_alloc_system_colors(void)
{
    palette_t *p = (palette_t *)xmalloc(sizeof(palette_t));
    unsigned long color_return[NUM_ENTRIES];

    p->num_entries = NUM_ENTRIES;
    p->entries = xmalloc(sizeof(palette_entry_t) * NUM_ENTRIES);
    memset(p->entries, 0, sizeof(palette_entry_t) * NUM_ENTRIES);

    p->entries[0].red = 0;
    p->entries[0].green = 0;
    p->entries[0].blue = 0;

    p->entries[1].red = 0xff;
    p->entries[1].green = 0;
    p->entries[1].blue = 0;

    p->entries[2].red = 0;
    p->entries[2].green = 0xff;
    p->entries[2].blue = 0;

    p->entries[3].red = 0xff;
    p->entries[3].green = 0xff;
    p->entries[3].blue = 0x7f;

    p->entries[4].red = 0xaf;
    p->entries[4].green = 0xaf;
    p->entries[4].blue = 0xaf;

    color_alloc_colors(NULL, p, color_return);

    drive_led_off_pixel = (GdkColor *)color_return[0];
    drive_led_on_red_pixel = (GdkColor *)color_return[1];
    drive_led_on_green_pixel = (GdkColor *)color_return[2];
    motor_running_pixel = (GdkColor *)color_return[3];
    tape_control_pixel = (GdkColor *)color_return[4];

    free(p->entries);
    free(p);

    return 0;
}

/*-----------------------------------------------------------------------*/

int uicolor_alloc_colors(video_canvas_t *c)
{
    if (uicolor_alloc_system_colors() < 0
        || color_alloc_colors(c, c->palette, NULL) < 0) {
/*
        if (colormap == DefaultColormap(display, screen)) {
            log_warning(LOG_DEFAULT,
                        "Automatically using a private colormap.");
            colormap = XCreateColormap(display, RootWindow(display, screen),
                                       visual, AllocNone);
            XtVaSetValues(_ui_top_level, XtNcolormap, colormap, NULL);
            return color_alloc_colors(c, c->palette, NULL);
        }
*/
    }
    return 0;
}

int uicolor_set_palette(struct video_canvas_s *c, const palette_t *palette)
{
/*
    if (uicolor_alloc_system_colors() < 0);
        return -1;
*/
    return color_alloc_colors(c, palette, NULL);
}

/*-----------------------------------------------------------------------*/

int uicolor_alloc_color(unsigned int red, unsigned int green,
                        unsigned int blue, unsigned long *color_pixel,
                        BYTE *pixel_return)
{
    GdkColor *color;
    XImage *im;
    BYTE *data = (BYTE *)xmalloc(4);
    Display *display = x11ui_get_display_ptr();

    color = (GdkColor *)xmalloc(sizeof(GdkColor));

    /* This is a kludge to map pixels to zimage values. Is there a better
       way to do this? //tvr */
    im = XCreateImage(display, GDK_VISUAL_XVISUAL(visual),
                      x11ui_get_display_depth(),
                      ZPixmap, 0, (char *)data, 1, 1, 8, 1);
    if (!im) {
        log_error(LOG_DEFAULT, _("XCreateImage failed."));
        return -1;
    }

    color->red =  red << 8;
    color->green =  green << 8;
    color->blue = blue << 8;

    if (!gdk_color_alloc(colormap, color)) {
        log_error(LOG_DEFAULT, _("Cannot allocate color \"#%04X%04X%04X\"."),
                  color->red, color->green, color->blue);
        XDestroyImage(im);
        return -1;
    }
    XPutPixel(im, 0, 0, color->pixel);

    *pixel_return = *data;
    *color_pixel = (unsigned long)color /*color.pixel*/;

    XDestroyImage(im);

    return 0;
}

void uicolor_free_color(unsigned int red, unsigned int green,
                        unsigned int blue, unsigned long color_pixel)
{
    GdkColor *color;

    color = (GdkColor *)color_pixel;

    gdk_colors_free(colormap, &(color->pixel), 1, 0);
/*
    if (color_pixel != 0)
        free((unsigned char *)color_pixel);
*/
}

void uicolor_convert_color_table(unsigned int colnr, BYTE *data,
                                 unsigned int dither, long color_pixel,
                                 void *c)
{
    if (c == NULL)
        return;

    video_convert_color_table(colnr, data, dither,
                              (long)(((GdkColor *)(color_pixel))->pixel),
                              (video_canvas_t *)c);
}

void uicolor_init_video_colors()
{
    short i;
    GdkColor colorr, colorg, colorb;
    
    for (i = 0; i < 256; i++)
    {
	colorr.red =  i << 8;
	colorr.green =  0;
	colorr.blue = 0;
	
	colorg.red =  0;
	colorg.green =  i << 8;
	colorg.blue = 0;
	
	colorb.red =  0;
	colorb.green =  0;
	colorb.blue = i << 8;
	
	if (!gdk_color_alloc(colormap, &colorr)) 
	{
	    log_error(LOG_DEFAULT, _("Cannot allocate color \"#%04X%04X%04X\"."),
		      colorr.red, colorr.green, colorr.blue);
	}
	if (!gdk_color_alloc(colormap, &colorg)) 
	{
	    log_error(LOG_DEFAULT, _("Cannot allocate color \"#%04X%04X%04X\"."),
		      colorg.red, colorg.green, colorg.blue);
	}
	if (!gdk_color_alloc(colormap, &colorb)) 
	{
	    log_error(LOG_DEFAULT, _("Cannot allocate color \"#%04X%04X%04X\"."),
		      colorb.red, colorb.green, colorb.blue);
	}
	video_render_setrawrgb(i, 
			       (DWORD) colorr.pixel, 
			       (DWORD) colorg.pixel, 
			       (DWORD) colorb.pixel);
    }
    
    video_render_initraw();
}
