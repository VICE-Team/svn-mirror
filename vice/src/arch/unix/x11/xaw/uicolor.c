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
#include <X11/Xlib.h>

#include "log.h"
#include "palette.h"
#include "types.h"
#include "uicolor.h"
#include "utils.h"
#include "video.h"

extern Display *display;
extern int screen;
extern int have_truecolor;
extern Colormap colormap;
extern Pixel drive_led_on_red_pixel, drive_led_on_green_pixel, drive_led_off_pixel;

static int n_allocated_pixels = 0;
static unsigned long allocated_pixels[0x100];

/* Allocate colors in the colormap. */
static int do_alloc_colors(const palette_t *palette, PIXEL *pixel_return,
                           int releasefl)
{
    int i, failed;
    XColor color;
    XImage *im;
    PIXEL *data = (PIXEL *)xmalloc(4);

    /* This is a kludge to map pixels to zimage values. Is there a better
       way to do this? //tvr */
    im = XCreateImage(display, visual, ui_get_display_depth(),
		      ZPixmap, 0, (char *)data, 1, 1, 8, 0);
    if (!im) {
        log_error(LOG_DEFAULT, "XCreateImage failed.");
        return -1;
    }

    n_allocated_pixels = 0;

    for (i = 0, failed = 0; i < palette->num_entries; i++) {
        color.flags = DoRed | DoGreen | DoBlue;
        color.red = palette->entries[i].red << 8;
        color.green = palette->entries[i].green << 8;
        color.blue = palette->entries[i].blue << 8;

        if (!XAllocColor(display, colormap, &color)) {
            failed = 1;
            log_error(LOG_DEFAULT, "Cannot allocate color \"#%04X%04X%04X\".",
                      color.red, color.green, color.blue);
        } else {
            allocated_pixels[n_allocated_pixels++] = color.pixel;
	}
        XPutPixel(im, 0, 0, color.pixel);
#if X_DISPLAY_DEPTH == 0
        video_convert_color_table(i, pixel_return, data, im, palette,
                                  (long)color.pixel, ui_get_display_depth());
#else
        pixel_return[i] = *data;
#endif
    }

    if (releasefl && failed && n_allocated_pixels) {
        XFreeColors(display, colormap, allocated_pixels, n_allocated_pixels, 0);
	n_allocated_pixels = 0;
    }

    XDestroyImage(im);

    if (!failed) {
        XColor screen, exact;

        if (!XAllocNamedColor(display, colormap, "black", &screen, &exact))
            failed = 1;
        else {
            drive_led_off_pixel = screen.pixel;
            allocated_pixels[n_allocated_pixels++] = screen.pixel;
        }

        if (!failed) {
            if (!XAllocNamedColor(display, colormap, "red", &screen, &exact))
                failed = 1;
            else {
                drive_led_on_red_pixel = screen.pixel;
                allocated_pixels[n_allocated_pixels++] = screen.pixel;
            }
            if (!failed) {
                if (!XAllocNamedColor(display, colormap, "green", &screen, &exact))
                    failed = 1;
                else {
                    drive_led_on_green_pixel = screen.pixel;
                    allocated_pixels[n_allocated_pixels++] = screen.pixel;
                }
            }
        }

    }

    return failed;
}

/* In here we try to allocate the given colors. This function is called from
 * 'ui_open_canvas_window()'.  The calling function sets the colormap
 * resource of the toplevel window.  If there is not enough place in the
 * colormap for all color entries, we allocate a new one.  If we someday open
 * two canvas windows, and the colormap fills up during the second one, we
 * might run into trouble, although I am not sure.  (setting the Toplevel
 * colormap will not change the colormap of already opened children)
 *
 * 20jan1998 A.Fachat */
int uicolor_alloc_colors(canvas_t *c, const palette_t *palette,
                         PIXEL pixel_return[])
{
    int failed;

    log_message(LOG_DEFAULT, "Color request for canvas %p.", c);

    failed = do_alloc_colors(palette, pixel_return, 1);
    if (failed) {
	if (colormap == DefaultColormap(display, screen)) {
            log_warning(LOG_DEFAULT, "Automagically using a private colormap.");
	    colormap = XCreateColormap(display, RootWindow(display, screen),
				       visual, AllocNone);
	    XtVaSetValues(_ui_top_level, XtNcolormap, colormap, NULL);
	    failed = do_alloc_colors(palette, pixel_return, 0);
	}
    }
    return failed ? -1 : 0;
}

/* Change the colormap of window `w' on the fly.  This only works for
   TrueColor visuals.  Otherwise, it would be too messy to re-allocate the
   new colormap.  */
int ui_canvas_set_palette(canvas_t *c, ui_window_t w, const palette_t *palette,
                          PIXEL *pixel_return)
{
    log_message(LOG_DEFAULT, "Change color request for canvas %p.", c);

    if (!have_truecolor) {
	int nallocp;
	PIXEL  *xpixel = xmalloc(sizeof(PIXEL) * palette->num_entries);
	unsigned long *ypixel = xmalloc(sizeof(unsigned long)
                                        * n_allocated_pixels);

#if X_DISPLAY_DEPTH == 0
        video_convert_save_pixel();
#endif

	/* save the list of already allocated X pixel values */
	nallocp = n_allocated_pixels;
	memcpy(ypixel, allocated_pixels, sizeof(unsigned long) * nallocp);
	n_allocated_pixels = 0;

	if (do_alloc_colors(palette, xpixel, 1)) { /* failed */

	    /* restore list of previously allocated X pixel values */
	    n_allocated_pixels = nallocp;
	    memcpy(allocated_pixels, ypixel, sizeof(unsigned long) * nallocp);

#if X_DISPLAY_DEPTH == 0
            video_convert_restore_pixel();
#endif
	    log_error(LOG_DEFAULT, "Cannot allocate enough colors.");
	} else {					/* successful */
#if 0
            unsigned int i;
#endif

	    /* copy the new return values to the real return values */
	    memcpy(pixel_return, xpixel, sizeof(PIXEL) * palette->num_entries);

	    /* free the previously allocated pixel values */
#if 0
            if (nallocp > 0) {
                for (i = 0; i < nallocp; i++) {
                    unsigned int j, color_exists;

                    color_exists = 0;
                    for (j = 0; j < n_allocated_pixels; j++) {
                        if (ypixel[i] == allocated_pixels[j]) {
                            color_exists |= 1;
                        }
                    }
                    if (color_exists == 0)
                        XFreeColors(display, colormap, &(ypixel[i]), 1, 0);
                }
            }
#endif
            XFreeColors(display, colormap, ypixel, nallocp, 0);
	}
	free(xpixel);

        return 0;
    }

    return uicolor_alloc_colors(c, palette, pixel_return);
}

