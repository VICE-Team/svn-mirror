/*
 * uicolor.c - X11 color routines.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Teemu Rantanen <tvr@cs.hut.fi>
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
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

extern int screen;
extern GdkColormap *colormap;
extern GdkColor *drive_led_on_red_pixel, *drive_led_on_green_pixel,
    *drive_led_off_pixel, *motor_running_pixel, *tape_control_pixel;

#define NUM_ENTRIES 5

static int uicolor_alloc_system_colors(void)
{
    palette_t *p = (palette_t *)xmalloc(sizeof(palette_t));
    PIXEL pixel_return[NUM_ENTRIES];
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

    color_alloc_colors((void *)-1, p, pixel_return, color_return);

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

int uicolor_alloc_colors(canvas_t *c, const palette_t *palette,
                         PIXEL pixel_return[])
{
    if (uicolor_alloc_system_colors() < 0
        || color_alloc_colors(c, palette, pixel_return, NULL) < 0) {
/*
        if (colormap == DefaultColormap(display, screen)) {
            log_warning(LOG_DEFAULT,
                        "Automatically using a private colormap.");
            colormap = XCreateColormap(display, RootWindow(display, screen),
                                       visual, AllocNone);
            XtVaSetValues(_ui_top_level, XtNcolormap, colormap, NULL);
            return color_alloc_colors(c, palette, pixel_return, NULL);
        }
*/
    }
    return 0;
}

int ui_canvas_set_palette(canvas_t *c, ui_window_t w, const palette_t *palette,
                          PIXEL *pixel_return)
{
/*
    if (uicolor_alloc_system_colors() < 0);
        return -1;
*/
    return color_alloc_colors(c, palette, pixel_return, NULL);
}

/*-----------------------------------------------------------------------*/

static unsigned int bits_per_pixel;

int uicolor_alloc_color(unsigned int red, unsigned int green,
                        unsigned int blue, unsigned long *color_pixel,
                        PIXEL *pixel_return)
{
    GdkColor *color;
    XImage *im;
    PIXEL *data = (PIXEL *)xmalloc(4);
    Display *display = ui_get_display_ptr();

    color = (GdkColor *)xmalloc(sizeof(GdkColor));

    /* This is a kludge to map pixels to zimage values. Is there a better
       way to do this? //tvr */
    im = XCreateImage(display, GDK_VISUAL_XVISUAL(visual),
                      ui_get_display_depth(),
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

    bits_per_pixel = im->bits_per_pixel;

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

void uicolor_convert_color_table(unsigned int colnr, PIXEL *pixel_return,
                                 PIXEL *data, unsigned int dither,
                                 long color_pixel)
{
#if X_DISPLAY_DEPTH == 0
    video_convert_color_table(colnr, pixel_return, data, bits_per_pixel,
                              dither,
                              (long)(((GdkColor *)(color_pixel))->pixel));
#endif
}

