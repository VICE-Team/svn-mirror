/*
 * video.c - MS-DOS graphics handling, based on the Allegro library by Shawn
 * Hargreaves (http://www.talula.demon.co.uk/allegro).
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
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

#include <conio.h>
#include <stdio.h>
#include <string.h>

#include "cmdline.h"
#include "kbd.h"
#include "log.h"
#include "palette.h"
#include "resources.h"
#include "tui.h"
#include "tui_backend.h"
#include "types.h"
#include "utils.h"
#include "video.h"
#include "videoarch.h"

/* #define DEBUG_VIDEO */

vga_mode_t vga_modes[] = {
    /* VGA_320x200 */ { 320, 200, "320x200" },
    /* VGA_360x240 */ { 360, 240, "360x240" },
    /* VGA_360x270 */ { 360, 270, "360x270" },
    /* VGA_376x282 */ { 376, 282, "376x282" },
    /* VGA_400x300 */ { 400, 300, "400x300" },
    /* VGA_640x480 */ { 640, 480, "640x480" }
};

canvas_t *last_canvas;

#ifdef DEBUG_VIDEO
#define DEBUG(x) log_debug x
#else
#define DEBUG(x)
#endif

/* Flag: are we in graphics mode?  */
static int in_gfx_mode;

/* Logging goes here.  */
static log_t video_log = LOG_ERR;

/* ------------------------------------------------------------------------- */

/* Video-related resources.  */

/* VGA Video mode to use.  */
static int vga_mode;

int video_in_gfx_mode() {
    return in_gfx_mode;
}

static int set_vga_mode(resource_value_t v, void *param)
{
    /* FIXME: Sanity check!  */
    vga_mode = (int) v;
    return 0;
}

#ifndef USE_MIDAS_SOUND

/* Flag: do we try to use triple buffering if possible?  */
static int try_triple_buffering;

static int set_try_triple_buffering(resource_value_t v, void *param)
{
    /* FIXME: this has only effect when we switch to gfx mode.  This is OK
       for now, but is not the correct behavior.  */
    try_triple_buffering = (int) v;
    return 0;
}

#endif

static resource_t resources[] = {
    { "VGAMode", RES_INTEGER, (resource_value_t)VGA_320x200,
      (resource_value_t *)&vga_mode,
      set_vga_mode, NULL },
#ifndef USE_MIDAS_SOUND
    { "TripleBuffering", RES_INTEGER, (resource_value_t)0,
      (resource_value_t *)&try_triple_buffering,
      set_try_triple_buffering, NULL },
#endif
    { NULL }
};

int video_arch_init_resources(void)
{
    return resources_register(resources);
}

/* ------------------------------------------------------------------------- */

/* Video-specific command-line options.  */

static cmdline_option_t cmdline_options[] = {
    { "-vgamode", SET_RESOURCE, 1, NULL, NULL,
      "VGAMode", NULL,
      "<mode>", "Set VGA mode to <mode>" },
#ifndef USE_MIDAS_SOUND
    { "-triplebuf", SET_RESOURCE, 0, NULL, NULL,
      "TripleBuffering", (resource_value_t)1,
      NULL, "Try to use triple buffering when possible" },
    { "+triplebuf", SET_RESOURCE, 0, NULL, NULL,
      "TripleBuffering", (resource_value_t)1,
      NULL, "Disable usage of triple buffering" },
#endif
    { NULL }
};

int video_init_cmdline_options(void)
{
    return cmdline_register_options(cmdline_options);
}

/* ------------------------------------------------------------------------- */

int video_init(void)
{
    if (video_log == LOG_ERR)
        video_log = log_open("Video");

    if (allegro_init())
        log_error(video_log, "Cannot initialize Allegro.");

    log_message(video_log, "Allegro initialized.");

    in_gfx_mode = 0;

    return 0;
}

int video_frame_buffer_alloc(video_frame_buffer_t **i, unsigned int width,
                             unsigned int height)
{
    if (sizeof(BITMAP) != sizeof(video_frame_buffer_t)) {
        log_error(video_log,
                  "Allegro BITMAP size does not match! (see videoarch.h)");
        return -1;
    }

    DEBUG(("Allocating bitmap width=%d, height=%d", width, height));
    *i = (video_frame_buffer_t *)create_bitmap(width, height);

#ifdef DEBUG_VIDEO
    {
        int j;
        for (j = 0; j < height; j++) {
            DEBUG(("Checking line %d at 0x%x",j,(unsigned int)((*i)->line[j])));        }
    }
#endif

    if (*i == NULL) {
        DEBUG(("Bitmap allocation failed."));
        return -1;
    } else {
        DEBUG(("Bitmap allocation successful. Buffer at %p", *i));
        return 0;
    }
}

void video_frame_buffer_free(video_frame_buffer_t *i)
{
    if (!i)
        return;

    DEBUG(("Freeing frame buffer 0x%x", (unsigned int)i));
    destroy_bitmap((BITMAP *)i);
}

void video_frame_buffer_clear(video_frame_buffer_t *f, PIXEL value)
{
    int i;

    DEBUG(("Clearing frame buffer 0x%x with value 0x%x",
           (unsigned int)f, value));
    DEBUG(("width=%d, height=%d", f->w, f->h));
    for (i = 0; i < f->h; i++) {
        DEBUG(("Clearing line %d at 0x%x",i,(unsigned int)(f->line[i])));
        memset(f->line[i], value, f->w);
    }
}

static void canvas_set_vga_mode(canvas_t *c)
{
    int i;

    statusbar_reset_bitmaps_to_update();

#ifndef USE_MIDAS_SOUND
    /* If the user wants triple buffering, try Mode X first of all, as that
       is (currently) the only reliable way to achieve the result.  Virtual
       height is twice visible height to allow smooth page flipping.  */
    if (try_triple_buffering
        && (set_gfx_mode(GFX_MODEX, c->width, c->height, 0, c->height * 2)
            >= 0)) {
        DEBUG(("GFX_MODEX successful with width=%d height=%d vheight=%d",
               c->width, c->height, c->height * 2));
        c->use_triple_buffering = 1;
    } else
#endif
    /* If we don't want triple buffering, try to get a VESA linear mode
       first, which might not be the default. */
    if (set_gfx_mode(GFX_VESA2L, c->width, c->height, 0, 0) >= 0) {
        DEBUG(("GFX_VESA2L successful with width=%d height=%d",
               c->width, c->height));
        c->use_triple_buffering = 0;
        statusbar_append_bitmap_to_update(screen);
    } else if (set_gfx_mode(GFX_AUTODETECT, c->width, c->height, 0, 0) >= 0) {
        DEBUG(("GFX_AUTODETECT successful with width=%d height=%d",
               c->width, c->height));
        c->use_triple_buffering = 0;
        statusbar_append_bitmap_to_update(screen);
    } else {
        log_error(video_log, "Cannot enable %dx%dx256 graphics.",
                  c->width, c->height);
        exit(-1);
    }

    log_message(video_log, "Using mode %dx%dx256 (%s)%s.",
                c->width, c->height,
                is_linear_bitmap(screen) ? "linear" : "planar",
                c->use_triple_buffering ? "; triple buffering possible" : "");
    in_gfx_mode = 1;

#ifndef USE_MIDAS_SOUND
    /* If using triple buffering, setup the timer used by Allegro to emulate
       vertical retrace interrupts.  Wish I had $D012/$D011 on VGA.  */
    timer_simulate_retrace(c->use_triple_buffering);
#endif

    if (c->use_triple_buffering) {
        c->pages[0] = create_sub_bitmap(screen,
                                        0, 0, c->width, c->height);
        c->pages[1] = create_sub_bitmap(screen,
                                        0, c->height, c->width, c->height);
        c->back_page = 1;

        statusbar_append_bitmap_to_update(c->pages[0]);
        statusbar_append_bitmap_to_update(c->pages[1]);
    }
    statusbar_set_width(c->width);

    for (i = 0; i < NUM_AVAILABLE_COLORS; i++)
        set_color(i, &c->colors[i]);
}

/* Note: `mapped' is ignored.  */
canvas_t *canvas_create(const char *win_name, unsigned int *width,
                        unsigned int *height, int mapped,
                        void_t exposure_handler,
                        const palette_t *palette, PIXEL *pixel_return)
{
    canvas_t *new_canvas;

    DEBUG(("Creating canvas width=%d height=%d", *width, *height));
    if (palette->num_entries > NUM_AVAILABLE_COLORS) {
        log_error(video_log, "Too many colors requested.");
        return (canvas_t *) NULL;
    }
    new_canvas = (canvas_t *)xmalloc(sizeof(struct canvas_s));
    if (!new_canvas)
        return (canvas_t *) NULL;

    canvas_set_palette(new_canvas, palette, pixel_return);

    /* Set width and height.  */
    canvas_resize(new_canvas, 0, 0);
    *width = new_canvas->width;
    *height = new_canvas->height;

    DEBUG(("Setting VGA mode"));
    canvas_set_vga_mode(new_canvas);

    new_canvas->exposure_handler = (canvas_redraw_t)exposure_handler;
    new_canvas->back_page = 1;

    last_canvas = new_canvas;
    return new_canvas;
}

void canvas_destroy(canvas_t *c)
{
        /* FIXME: Just a dummy so far */
}

int canvas_set_palette(canvas_t *c, const palette_t *palette,
                       PIXEL *pixel_return)
{
    int i;
    RGB rgb_white = {55, 55, 55};
    RGB rgb_black = {0, 0, 0};
    RGB rgb_red = {63, 0, 0};
    RGB rgb_green = {0, 63, 0};
    RGB rgb_yellow = {63, 63, 0};
    RGB rgb_grey = {31, 31, 31};
    RGB rgb_darkgrey = {15, 15, 15};
    RGB rgb_blue = {10, 10, 50 };

    DEBUG(("Allocating colors"));

    for (i = 0; i < palette->num_entries; i++) {
        c->colors[i].r = palette->entries[i].red >> 2;
        c->colors[i].g = palette->entries[i].green >> 2;
        c->colors[i].b = palette->entries[i].blue >> 2;
        pixel_return[i] = i;
        if (in_gfx_mode)
            set_color(i, &c->colors[i]);
    }
    /* colors for the statusbar */
    c->colors[STATUSBAR_COLOR_BLACK] = rgb_black;
    c->colors[STATUSBAR_COLOR_WHITE] = rgb_white;
    c->colors[STATUSBAR_COLOR_RED] = rgb_red;
    c->colors[STATUSBAR_COLOR_YELLOW] = rgb_yellow;
    c->colors[STATUSBAR_COLOR_GREEN] = rgb_green;
    c->colors[STATUSBAR_COLOR_GREY] = rgb_grey;
    c->colors[STATUSBAR_COLOR_DARKGREY] = rgb_darkgrey;
    c->colors[STATUSBAR_COLOR_BLUE] = rgb_blue;

    for (i = STATUSBAR_COLOR_BLACK; i < NUM_AVAILABLE_COLORS; i++)
        set_color(i, &c->colors[i]);

    return 0;
}

void canvas_map(canvas_t *c)
{
    /* Not implemented. */
}

void canvas_unmap(canvas_t *c)
{
    /* Not implemented. */
}

/* Warning: this does not do what you would expect from it.  It just sets the
   canvas size according to the `VGAMode' resource. */
void canvas_resize(canvas_t *c, unsigned int width, unsigned int height)
{
    /*
    FIXME: the possible height for the statusbar isn't calculated,
    it's only checked whether VGA-mode has >200 lines
    */
    statusbar_set_height(vga_modes[vga_mode].height>200 ?
        STATUSBAR_HEIGHT : 0);

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
    statusbar_update();
    }
    DEBUG(("Successful"));
}

