/*
 * video.c - MS-DOS graphics handling, based on the Allegro library by Shawn
 * Hargreaves (http://www.talula.demon.co.uk/allegro).
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
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

#include "video.h"
#include "kbd.h"
#include "tui.h"
#include "resources.h"

/* #define DEBUG_VIDEO */

vga_mode_t vga_modes[] = {
    /* VGA_320x200 */ {	320, 200, "320x200" },
    /* VGA_360x240 */ { 360, 240, "360x240" },
    /* VGA_360x270 */ { 360, 270, "360x270" },
    /* VGA_376x282 */ { 376, 282, "376x282" },
    /* VGA_400x300 */ { 400, 300, "400x300" },
    /* VGA_640x480 */ { 640, 480, "640x480" }
};

canvas_t last_canvas;

#ifdef DEBUG_VIDEO
#define DEBUG(x)					\
    do {						\
        printf("%s, %d: ", __FUNCTION__, __LINE__);	\
        printf x;					\
        putchar('\n');					\
    } while (0)
#else
#define DEBUG(x)
#endif

/* ------------------------------------------------------------------------- */

int video_init(void)
{
    if (allegro_init()) {
	fprintf(stderr, "??? Cannot initialize the Allegro library ???\n");
	exit(-1);
    }
    printf("Allegro initialized.\n");

    return 0;
}

int frame_buffer_alloc(frame_buffer_t *i, unsigned int width,
		       unsigned int height)
{
    DEBUG(("Allocating bitmap width=%d, height=%d", width, height));
    *i = create_bitmap(width, height);
    if (*i == NULL) {
	DEBUG(("Bitmap allocation failed"));
	return -1;
    } else {
	DEBUG(("Bitmap allocation successful"));
	return 0;
    }
}

void frame_buffer_free(frame_buffer_t *i)
{
    DEBUG(("Freeing frame buffer 0x%x", (unsigned int)i));
    destroy_bitmap(*i);
}

void frame_buffer_clear(frame_buffer_t * f, BYTE value)
{
    int i;

    DEBUG(("Clearing frame buffer 0x%x with value 0x%x",
	   (unsigned int)f, value));
    for (i = 0; i < (*f)->h; i++)
	memset((*f)->line[i], value, (*f)->w);
}

static void canvas_set_vga_mode(canvas_t c)
{
    /* Try to get a VESA linear mode first, which might not be the
       default. */
    if (set_gfx_mode(GFX_VESA2L, c->width, c->height, 0, 0) >= 0) {
	DEBUG(("GFX_VESA2L successful with width=%d height=%d",
	       c->width, c->height));
    } else if (set_gfx_mode(GFX_AUTODETECT, c->width, c->height, 0, 0) >= 0) {
	DEBUG(("GFX_AUTODETECT successful with width=%d height=%d",
	       c->width, c->height));
    } else {
	fprintf(stderr, "Cannot enable %dx%dx256 graphics.\n",
		c->width, c->height);
	exit(-1);
    }

    printf("Using mode %dx%dx256 (%s).\n",
	   c->width, c->height,
	   is_linear_bitmap(screen) ? "linear" : "planar");
}

/* Note: `mapped' is ignored. */
canvas_t canvas_create(const char *win_name, unsigned int *width,
		       unsigned int *height, int mapped,
		       canvas_redraw_t exposure_handler, int num_colors,
		       const color_def_t color_defs[], BYTE * pixel_return)
{
    int i;
    canvas_t new_canvas;

    DEBUG(("Creating canvas width=%d height=%d", *width, *height));
    if (num_colors > NUM_AVAILABLE_COLORS) {
	fprintf(stderr, "??? Too many colors requested! ???\n");
	return (canvas_t) NULL;
    }
    new_canvas = (canvas_t) malloc(sizeof(struct _canvas));
    if (!new_canvas)
	return (canvas_t) NULL;

    /* Set width and height. */
    canvas_resize(new_canvas, 0, 0);
    *width = new_canvas->width;
    *height = new_canvas->height;

    DEBUG(("Setting VGA mode"));
    canvas_set_vga_mode(new_canvas);

    DEBUG(("Allocating colors"));
    for (i = 0; i < num_colors; i++) {
	new_canvas->colors[i].r = color_defs[i].red >> 10;
	new_canvas->colors[i].g = color_defs[i].green >> 10;
	new_canvas->colors[i].b = color_defs[i].blue >> 10;
	set_color(i, &(new_canvas->colors[i]));
	pixel_return[i] = i;
    }

    new_canvas->exposure_handler = exposure_handler;
    last_canvas = new_canvas;

    return new_canvas;
}

void canvas_map(canvas_t c)
{
    /* Not implemented. */
}

void canvas_unmap(canvas_t c)
{
    /* Not implemented. */
}

/* Warning: this does not do what you would expect from it.  It just sets the
   canvas size according to `app_resources.vgaMode'. */
void canvas_resize(canvas_t c, unsigned int width, unsigned int height)
{
    DEBUG(("Resizing, app_resources.vgaMode=%d", app_resources.vgaMode));
    c->width = vga_modes[app_resources.vgaMode].width;
    c->height = vga_modes[app_resources.vgaMode].height;
}

void video_ack_vga_mode(void)
{
    if (last_canvas != NULL) {
        canvas_resize(last_canvas, last_canvas->width, last_canvas->height);
        last_canvas->exposure_handler(last_canvas->width, last_canvas->height);
        DEBUG(("acknowledged vgaMode %d", app_resources.vgaMode));
    }
}

/* ------------------------------------------------------------------------- */

static PALETTE palette;

void enable_text(void)
{
    DEBUG(("Enabling text mode"));
    get_palette(palette);
    set_gfx_mode(GFX_TEXT, 80, 25, 0, 0);
    _set_screen_lines(25);
    kbd_uninstall();
    DEBUG(("Successful"));
}

void disable_text(void)
{
    DEBUG(("Enabling gfx mode"));
    kbd_install();
    if (last_canvas->width > 0 && last_canvas->height > 0) {
	video_ack_vga_mode();
	canvas_set_vga_mode(last_canvas);
	set_palette(palette);
	last_canvas->exposure_handler(last_canvas->width, last_canvas->height);
    }
    DEBUG(("Successful"));
}

int num_text_lines(void)
{
    return tui_num_lines();
}
