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

#include "cmdline.h"
#include "kbd.h"
#include "resources.h"
#include "tui.h"

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

/* Flag: are we in graphics mode?  */
static int in_gfx_mode;

/* ------------------------------------------------------------------------- */

/* Video-related resources.  */

/* VGA Video mode to use.  */
static int vga_mode;

/* Flag: do we try to use triple buffering if possible?  */
static int try_triple_buffering;

static int set_vga_mode(resource_value_t v)
{
    /* FIXME: Sanity check!  */
    vga_mode = (int) v;
    return 0;
}

static int set_try_triple_buffering(resource_value_t v)
{
    /* FIXME: this has only effect when we switch to gfx mode.  This is OK
       for now, but is not the correct behavior.  */
    try_triple_buffering = (int) v;
    return 0;
}

static resource_t resources[] = {
    { "VGAMode", RES_INTEGER, (resource_value_t) VGA_320x200,
      (resource_value_t *) &vga_mode, set_vga_mode },
    { "TripleBuffering", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &try_triple_buffering, set_try_triple_buffering },
    { NULL }
};

int video_init_resources(void)
{
    return resources_register(resources);
}

/* ------------------------------------------------------------------------- */

/* Video-specific command-line options.  */

static cmdline_option_t cmdline_options[] = {
    { "-vgamode", SET_RESOURCE, 1, NULL, NULL,
      "VGAMode", NULL,
      "<mode>", "Set VGA mode to <mode>" },
    { "-triplebuf", SET_RESOURCE, 0, NULL, NULL,
      "TripleBuffering", (resource_value_t) 1,
      NULL, "Try to use triple buffering when possible" },
    { "+triplebuf", SET_RESOURCE, 0, NULL, NULL,
      "TripleBuffering", (resource_value_t) 1,
      NULL, "Disable usage of triple buffering" },
    { NULL }
};

int video_init_cmdline_options(void)
{
    return cmdline_register_options(cmdline_options);
}

/* ------------------------------------------------------------------------- */

int video_init(void)
{
    if (allegro_init()) {
	fprintf(stderr, "??? Cannot initialize the Allegro library ???\n");
	exit(-1);
    }
    printf("Allegro initialized.\n");
    in_gfx_mode = 0;

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
    int i;

    /* If the user wants triple buffering, try Mode X first of all, as that
       is (currently) the only reliable way to achieve the result.  Virtual
       height is twice visible height to allow smooth page flipping.  */
    if (try_triple_buffering
        && (set_gfx_mode(GFX_MODEX, c->width, c->height, 0, c->height * 2)
            >= 0)) {
        DEBUG(("GFX_MODEX successful with width=%d height=%d vheight=%d",
               c->width, c->height, c->height * 2));
        c->use_triple_buffering = 1;
    }
    /* If we don't want triple buffering, try to get a VESA linear mode
       first, which might not be the default. */
    else if (set_gfx_mode(GFX_VESA2L, c->width, c->height, 0, 0) >= 0) {
        DEBUG(("GFX_VESA2L successful with width=%d height=%d",
               c->width, c->height));
        c->use_triple_buffering = 0;
    } else if (set_gfx_mode(GFX_AUTODETECT, c->width, c->height, 0, 0) >= 0) {
        DEBUG(("GFX_AUTODETECT successful with width=%d height=%d",
               c->width, c->height));
        c->use_triple_buffering = 0;
    } else {
        fprintf(stderr, "Cannot enable %dx%dx256 graphics.\n",
                c->width, c->height);
        exit(-1);
    }

    printf("Using mode %dx%dx256 (%s)%s.\n",
	   c->width, c->height,
	   is_linear_bitmap(screen) ? "linear" : "planar",
           c->use_triple_buffering ? "; triple buffering possible" : "");
    in_gfx_mode = 1;

    /* If using triple buffering, setup the timer used by Allegro to emulate
       vertical retrace interrupts.  Wish I had $D012/$D011 on VGA.  */
    timer_simulate_retrace(c->use_triple_buffering);

    if (c->use_triple_buffering) {
        c->pages[0] = create_sub_bitmap(screen,
                                        0, 0, c->width, c->height);
        c->pages[1] = create_sub_bitmap(screen,
                                        0, c->height, c->width, c->height);
        c->back_page = 1;
    }

    for (i = 0; i < NUM_AVAILABLE_COLORS; i++)
        set_color(i, &c->colors[i]);
}

/* Note: `mapped' is ignored.  */
canvas_t canvas_create(const char *win_name, unsigned int *width,
		       unsigned int *height, int mapped,
		       canvas_redraw_t exposure_handler,
		       const palette_t *palette, PIXEL *pixel_return)
{
    int i;
    canvas_t new_canvas;

    DEBUG(("Creating canvas width=%d height=%d", *width, *height));
    if (palette->num_entries > NUM_AVAILABLE_COLORS) {
	fprintf(stderr, "??? Too many colors requested! ???\n");
	return (canvas_t) NULL;
    }
    new_canvas = (canvas_t) malloc(sizeof(struct _canvas));
    if (!new_canvas)
	return (canvas_t) NULL;

    canvas_set_palette(new_canvas, palette, pixel_return);

    /* Set width and height.  */
    canvas_resize(new_canvas, 0, 0);
    *width = new_canvas->width;
    *height = new_canvas->height;

    DEBUG(("Setting VGA mode"));
    canvas_set_vga_mode(new_canvas);

    new_canvas->exposure_handler = exposure_handler;
    new_canvas->back_page = 1;

    last_canvas = new_canvas;
    return new_canvas;
}

int canvas_set_palette(canvas_t c, const palette_t *palette,
                       PIXEL *pixel_return)
{
    int i;

    DEBUG(("Allocating colors"));
    for (i = 0; i < palette->num_entries; i++) {
	c->colors[i].r = palette->entries[i].red >> 2;
	c->colors[i].g = palette->entries[i].green >> 2;
	c->colors[i].b = palette->entries[i].blue >> 2;
	pixel_return[i] = i;
        if (in_gfx_mode)
            set_color(i, &c->colors[i]);
    }

    return 0;
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
   canvas size according to the `VGAMode' resource. */
void canvas_resize(canvas_t c, unsigned int width, unsigned int height)
{
    DEBUG(("Resizing, vga_mode=%d", vga_mode));
    c->width = vga_modes[vga_mode].width;
    c->height = vga_modes[vga_mode].height;
}

void video_ack_vga_mode(void)
{
    if (last_canvas != NULL) {
        canvas_resize(last_canvas, last_canvas->width, last_canvas->height);
        last_canvas->exposure_handler(last_canvas->width, last_canvas->height);
        DEBUG(("Acknowledged vgaMode %d", vga_mode));
    }
}

/* ------------------------------------------------------------------------- */

void enable_text(void)
{
    DEBUG(("Enabling text mode"));
    kbd_uninstall();
    set_gfx_mode(GFX_TEXT, 80, 25, 0, 0);
    _set_screen_lines(25);
    DEBUG(("Successful"));
    in_gfx_mode = 0;
}

void disable_text(void)
{
    DEBUG(("Enabling gfx mode"));
    kbd_install();
    if (last_canvas->width > 0 && last_canvas->height > 0) {
        int i;

	video_ack_vga_mode();
	canvas_set_vga_mode(last_canvas);

	for (i = 0; i < NUM_AVAILABLE_COLORS; i++)
	    set_color(i, &last_canvas->colors[i]);

	last_canvas->exposure_handler(last_canvas->width, last_canvas->height);
	in_gfx_mode = 1;
    }
    DEBUG(("Successful"));
}

int num_text_lines(void)
{
    return tui_num_lines();
}
