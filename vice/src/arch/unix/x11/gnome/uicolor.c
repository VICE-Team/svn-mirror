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

#ifndef USE_COLOR_MANAGEMENT
static int n_allocated_pixels = 0;
static unsigned long allocated_pixels[0x100];
#endif

extern Display *display;
extern int depth;
extern int have_truecolor;
extern GdkColormap *colormap;
extern GdkColor *drive_led_on_red_pixel, *drive_led_on_green_pixel,
    *drive_led_off_pixel, *motor_running_pixel, *tape_control_pixel;

#ifdef USE_COLOR_MANAGEMENT
#define NUM_ENTRIES 5

void uicolor_alloc_system_colors(void)
{
    palette_t *p = (palette_t*)xmalloc(sizeof(palette_t));
    PIXEL pixel_return[NUM_ENTRIES];

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

    color_alloc_colors((void *)-1, p, pixel_return);
/*
    drive_led_off_pixel = pixel_return[0];
    drive_led_on_red_pixel = pixel_return[1];
    drive_led_on_green_pixel = pixel_return[2];
    motor_running_pixel = pixel_return[3];
    tape_control_pixel = pixel_return[4];
*/
    free(p->entries);
    free(p);
}
#endif

/* Allocate colors in the colormap. */
static int do_alloc_colors(const palette_t *palette, PIXEL pixel_return[],
                           int releasefl)
{
#ifndef USE_COLOR_MANAGEMENT
    int i, failed;
    GdkColor color;
    XImage *im;
    PIXEL *data = (PIXEL *)xmalloc(4);


    /* This is a kludge to map pixels to zimage values. Is there a better
       way to do this? //tvr */
      /*    im = gdk_image_new(GDK_IMAGE_NORMAL,visual,1,8);*/
    im = XCreateImage(display,GDK_VISUAL_XVISUAL(visual),depth,
                      ZPixmap, 0, (char *)data, 1, 1, 8, 0);
    if (!im)
        return -1;

    n_allocated_pixels = 0;

    for (i = 0, failed = 0; i < palette->num_entries; i++) {
        color.red = palette->entries[i].red << 8;
        color.green = palette->entries[i].green << 8;
        color.blue = palette->entries[i].blue << 8;
        if (!gdk_color_alloc(colormap, &color)) {
            failed = 1;
            log_warning(LOG_DEFAULT, _("Cannot allocate color \"#%04X%04X%04X\"."),
                        color.red, color.green, color.blue);
        } else {
            allocated_pixels[n_allocated_pixels++] = color.pixel;
        }
        XPutPixel(im, 0, 0, color.pixel);
#if X_DISPLAY_DEPTH == 0
        video_convert_color_table(i, pixel_return, data, im, palette,
                                  (long)color.pixel, depth);
#else
        pixel_return[i] = *data;
#endif
    }

    if (releasefl && failed && n_allocated_pixels) {
        gdk_colors_free(colormap, allocated_pixels, n_allocated_pixels, 0);
        n_allocated_pixels = 0;
    }

    XDestroyImage(im);

    if (!failed) {
        GdkColor* exact;
        exact = (GdkColor*) xmalloc(sizeof(GdkColor));
        exact->red = 0;
        exact->green = 0;
        exact->blue = 0;
        if (!gdk_color_alloc(colormap,exact))
            return 1;
        else {
            drive_led_off_pixel = exact;
            allocated_pixels[n_allocated_pixels++] = exact->pixel;
        }

        exact = (GdkColor*) xmalloc(sizeof(GdkColor));
        exact->red = 0xffff;
        exact->green = 0;
        exact->blue = 0;
        if (!gdk_color_alloc(colormap,exact))
            return 1;
        else {
            drive_led_on_red_pixel = exact;
            allocated_pixels[n_allocated_pixels++] = exact->pixel;
        }

        exact = (GdkColor*) xmalloc(sizeof(GdkColor));
        exact->red = 0;
        exact->green = 0xffff;
        exact->blue = 0;
        if (!gdk_color_alloc(colormap,exact))
            return 1;
        else {
            drive_led_on_green_pixel = exact;
            allocated_pixels[n_allocated_pixels++] = exact->pixel;
        }

        exact = (GdkColor*) xmalloc(sizeof(GdkColor));
        exact->red = 0xffff;
        exact->green = 0xffff;
        exact->blue = 0x7fff;
        if (!gdk_color_alloc(colormap,exact))
            return 1;
        else {
            motor_running_pixel = exact;
            allocated_pixels[n_allocated_pixels++] = exact->pixel;
        }

        exact = (GdkColor*) xmalloc(sizeof(GdkColor));
        exact->red = 0xafff;
        exact->green = 0xafff;
        exact->blue = 0xafff;
        if (!gdk_color_alloc(colormap,exact))
            return 1;
        else {
            tape_control_pixel = exact;
            allocated_pixels[n_allocated_pixels++] = exact->pixel;
        }
    }
    return failed;
#else
    return 0;
#endif
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
#ifndef USE_COLOR_MANAGEMENT
    int failed;
#endif

    log_message(LOG_DEFAULT, "Color request for canvas %p.", c);
#ifdef USE_COLOR_MANAGEMENT
    color_alloc_colors(c, palette, pixel_return);
    return 0;
#else
    failed = do_alloc_colors(palette, pixel_return, 1);
    if (failed) {
        if (colormap == gdk_colormap_get_system()) {
            log_warning(LOG_DEFAULT, _("Automagically using a private colormap."));
            colormap = gdk_colormap_new(visual, AllocNone);
            gdk_window_set_colormap(_ui_top_level->window,colormap);
            failed = do_alloc_colors(palette, pixel_return, 0);
        }
    }
    return failed ? -1 : 0;
#endif
}

/* Change the colormap of window `w' on the fly.  This only works for
   TrueColor visuals.  Otherwise, it would be too messy to re-allocate the
   new colormap.  */
int ui_canvas_set_palette(canvas_t *c, ui_window_t w, const palette_t *palette,
                          PIXEL *pixel_return)
{
    log_message(LOG_DEFAULT, "Change color request for canvas %p.", c);
#ifdef USE_COLOR_MANAGEMENT
    uicolor_alloc_system_colors();
    color_alloc_colors(c, palette, pixel_return);
    return 0;
#else
    if (!have_truecolor) {
        int nallocp;
        PIXEL  *xpixel=malloc(sizeof(PIXEL)*palette->num_entries);
        unsigned long *ypixel=malloc(sizeof(unsigned long)*n_allocated_pixels);

#if X_DISPLAY_DEPTH == 0
        video_convert_save_pixel();
#endif

        /* save the list of already allocated X pixel values */
        nallocp = n_allocated_pixels;
        memcpy(ypixel, allocated_pixels, sizeof(unsigned long)*nallocp);
        n_allocated_pixels = 0;

        if( do_alloc_colors(palette, xpixel, 1) ) {     /* failed */

            /* restore list of previously allocated X pixel values */
            n_allocated_pixels = nallocp;
            memcpy(allocated_pixels, ypixel, sizeof(unsigned long)*nallocp);

#if X_DISPLAY_DEPTH == 0
            video_convert_restore_pixel();
#endif
            log_error(LOG_DEFAULT, _("Cannot allocate enough colors."));
        } else {                                        /* successful */
            /* copy the new return values to the real return values */
            memcpy(pixel_return, xpixel, sizeof(PIXEL) * palette->num_entries);

            /* free the previously allocated pixel values */
            XFreeColors(display, GDK_COLORMAP_XCOLORMAP(colormap),
                        ypixel, nallocp, 0);
        }
        free(xpixel);

        return 0;
    }

    return uicolor_alloc_colors(c, palette, pixel_return);
#endif
}

#ifdef USE_COLOR_MANAGEMENT
static unsigned int bits_per_pixel;

int uicolor_alloc_color(unsigned int red, unsigned int green,
                        unsigned int blue, unsigned long *color_pixel,
                        PIXEL *pixel_return)
{
    GdkColor color;
    XImage *im;
    PIXEL *data = (PIXEL *)xmalloc(4);

    /* This is a kludge to map pixels to zimage values. Is there a better
       way to do this? //tvr */
    im = XCreateImage(display, GDK_VISUAL_XVISUAL(visual), ui_get_display_depth(),
                      ZPixmap, 0, (char *)data, 1, 1, 8, 1);
    if (!im) {
        log_error(LOG_DEFAULT, "XCreateImage failed.");
        return -1;
    }

    color.red =  red << 8;
    color.green =  green << 8;
    color.blue = blue << 8;

    if (!gdk_color_alloc(colormap, &color)) {
        log_error(LOG_DEFAULT, "Cannot allocate color \"#%04X%04X%04X\".",
                  color.red, color.green, color.blue);
    }
    XPutPixel(im, 0, 0, color.pixel);

    bits_per_pixel = im->bits_per_pixel;

    *pixel_return = *data;
    *color_pixel = color.pixel;

    XDestroyImage(im);

    return 0;
}

void uicolor_free_color(unsigned int red, unsigned int green,
                        unsigned int blue, unsigned long color_pixel)
{
    gdk_colors_free(colormap, &color_pixel, 1, 0);
}

void uicolor_convert_color_table(unsigned int colnr, PIXEL *pixel_return,
                                 PIXEL *data, unsigned int dither,
                                 long color_pixel)
{
#if X_DISPLAY_DEPTH == 0
    video_convert_color_table(colnr, pixel_return, data, bits_per_pixel,
                              dither, color_pixel);
#endif
}
#endif

