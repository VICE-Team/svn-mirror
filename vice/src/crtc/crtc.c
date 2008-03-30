/*
 * crtc.c - A line-based CRTC emulation (under construction).
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
 *  André Fachat (fachat@physik.tu-chemnitz.de)
 *
 * 16/24bpp support added by
 *  Steven Tieu (stieu@physics.ubc.ca)
 *  Teemu Rantanen (tvr@cs.hut.fi)
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

#define _CRTC_C

#define CRTC_WINDOW_TITLE            MY_WINDOW_TITLE

/*
#define NDEBUG
#include <assert.h>
#define memset(a,b,c)   (assert((a)),memset((a),(b),(c)))
*/

#include "vice.h"

#include "cmdline.h"
#include "crtc.h"
#include "interrupt.h"
#include "log.h"
#include "machine.h"
#include "mem.h"
#include "raster.h"
#include "resources.h"
#include "snapshot.h"
#include "utils.h"
#include "alarm.h"
#include "maincpu.h"

#define	crtc_max(a,b)	(((a)>(b))?(a):(b))
#define	crtc_min(a,b)	(((a)<(b))?(a):(b))


/* CRTC alarms.  */
static alarm_t raster_draw_alarm;

static void crtc_init_dwg_tables(void);

#define IS_DOUBLE_WIDTH_ALLOWED(a)      \
        (((a) * 8 + 2 * SCREEN_BORDERWIDTH) <= (FRAMEB_WIDTH / MAX_PIXEL_WIDTH))
#define IS_DOUBLE_HEIGHT_ALLOWED(a)     \
        (((a) + 2 * SCREEN_BORDERHEIGHT) <= (FRAMEB_HEIGHT / MAX_PIXEL_HEIGHT))

log_t crtc_log = LOG_ERR;

static PIXEL4 dwg_table_0[256], dwg_table_1[256];
static PIXEL4 dwg_table2x_0[256], dwg_table2x_1[256];
static PIXEL4 dwg_table2x_2[256], dwg_table2x_3[256];

/* -------------------------------------------------------------------------- */

canvas_t crtc_init(void)
{
    static const char *color_names[CRTC_NUM_COLORS] = {
        "Background", "Foreground"
    };

    if (crtc_log == LOG_ERR)
        crtc_log = log_open("CRTC");

    if (init_raster(1, MAX_PIXEL_WIDTH, MAX_PIXEL_HEIGHT) < 0)
        return NULL;

    alarm_init(&raster_draw_alarm, &maincpu_alarm_context,
               "CrtcRasterDraw", int_rasterdraw);

    video_resize();

    palette = palette_create(CRTC_NUM_COLORS, color_names);
    if (palette == NULL)
        return NULL;
    if (palette_load(crtc_resources.palette_file_name, palette) < 0) {
        log_message(crtc_log, "Cannot load palette file `%s'.",
                    crtc_resources.palette_file_name);
        return NULL;
    }

    if (open_output_window(CRTC_WINDOW_TITLE,
                           SCREEN_WIDTH,
                           SCREEN_HEIGHT,
                           palette,
                           (canvas_redraw_t) crtc_arrange_window)) {
        log_error(crtc_log, "Cannot open window for CRTC emulation.");
        return NULL;
    }

    video_mode = CRTC_STANDARD_MODE;

    if (canvas) {
        refresh_changed();
        refresh_all();
    }

    chargen_rel = 0;
    chargen_ptr = chargen_rom + chargen_rel;
    border_color = 0;
    background_color = 0;
    display_ystart = SCREEN_BORDERHEIGHT;

    crtc_init_dwg_tables();

    if (canvas) {
/*
        store_crtc(0, 49);
        store_crtc(1, 40);
*/
        store_crtc(4, 49);
        store_crtc(5, 0);
        store_crtc(6, 25);
        store_crtc(9, 7);
        crtc_update_timing(1);
    }
    return canvas;
}

static void crtc_init_dwg_tables(void)
{
    int byte, p;
    BYTE msk;

    for (byte = 0; byte < 0x0100; byte++) {
        *((PIXEL *) (dwg_table2x_0 + byte))
            = *((PIXEL *) (dwg_table2x_0 + byte) + 1)
            = PIXEL(byte & 0x80 ? 1 : 0);
        *((PIXEL *) (dwg_table2x_0 + byte) + 2)
            = *((PIXEL *) (dwg_table2x_0 + byte) + 3)
            = PIXEL(byte & 0x40 ? 1 : 0);
        *((PIXEL *) (dwg_table2x_1 + byte))
            = *((PIXEL *) (dwg_table2x_1 + byte) + 1)
            = PIXEL(byte & 0x20 ? 1 : 0);
        *((PIXEL *) (dwg_table2x_1 + byte) + 2)
            = *((PIXEL *) (dwg_table2x_1 + byte) + 3)
            = PIXEL(byte & 0x10 ? 1 : 0);
        *((PIXEL *) (dwg_table2x_2 + byte))
            = *((PIXEL *) (dwg_table2x_2 + byte) + 1)
            = PIXEL(byte & 0x08 ? 1 : 0);
        *((PIXEL *) (dwg_table2x_2 + byte) + 2)
            = *((PIXEL *) (dwg_table2x_2 + byte) + 3)
            = PIXEL(byte & 0x04 ? 1 : 0);
        *((PIXEL *) (dwg_table2x_3 + byte))
            = *((PIXEL *) (dwg_table2x_3 + byte) + 1)
            = PIXEL(byte & 0x02 ? 1 : 0);
        *((PIXEL *) (dwg_table2x_3 + byte) + 2)
            = *((PIXEL *) (dwg_table2x_3 + byte) + 3)
            = PIXEL(byte & 0x01 ? 1 : 0);
    }

    for (byte = 0; byte < 0x0100; byte++) {
        for (msk = 0x80, p = 0; p < 4; msk >>= 1, p++)
            *((PIXEL *)(dwg_table_0 + byte) + p) = PIXEL(byte & msk ? 1 : 0);
        for (p = 0; p < 4; msk >>= 1, p++)
            *((PIXEL *)(dwg_table_1 + byte) + p) = PIXEL(byte & msk ? 1 : 0);
    }
}

/* -------------------------------------------------------------------------- */

/* Set proper functions and constants for the current video settings. */
void crtc_resize(void)
{
    static int old_size = 0;

    if (DOUBLE_SIZE_ENABLED()) {
        if (IS_DOUBLE_WIDTH_ALLOWED(memptr_inc)) {
            pixel_width = 2;
            video_modes[CRTC_STANDARD_MODE].fill_cache = fill_cache;
            video_modes[CRTC_STANDARD_MODE].draw_line_cached = draw_standard_line_cached_2x;
            video_modes[CRTC_STANDARD_MODE].draw_line = draw_standard_line_2x;
            video_modes[CRTC_REVERSE_MODE].fill_cache = fill_cache;
            video_modes[CRTC_REVERSE_MODE].draw_line_cached = draw_reverse_line_cached_2x;
            video_modes[CRTC_REVERSE_MODE].draw_line = draw_reverse_line_2x;
            if (old_size == 1)
                window_width *= 2;
        } else {
            /* When in 80 column mode, only the height is doubled. */
            pixel_width = 1;
            video_modes[CRTC_STANDARD_MODE].fill_cache = fill_cache;
            video_modes[CRTC_STANDARD_MODE].draw_line_cached = draw_standard_line_cached;
            video_modes[CRTC_STANDARD_MODE].draw_line = draw_standard_line;
            video_modes[CRTC_REVERSE_MODE].fill_cache = fill_cache;
            video_modes[CRTC_REVERSE_MODE].draw_line_cached = draw_reverse_line_cached;
            video_modes[CRTC_REVERSE_MODE].draw_line = draw_reverse_line;
        }
        if (IS_DOUBLE_HEIGHT_ALLOWED(crtc_screen_textlines*screen_charheight)) {
            pixel_height = 2;
            if (old_size == 1) {
                window_height *= 2;
            }
        } else {
            pixel_height = 1;
        }
    } else {
        pixel_width = 1;
        pixel_height = 1;
        video_modes[CRTC_STANDARD_MODE].fill_cache = fill_cache;
        video_modes[CRTC_STANDARD_MODE].draw_line_cached = draw_standard_line_cached;
        video_modes[CRTC_STANDARD_MODE].draw_line = draw_standard_line;
        video_modes[CRTC_REVERSE_MODE].fill_cache = fill_cache;
        video_modes[CRTC_REVERSE_MODE].draw_line_cached = draw_reverse_line_cached;
        video_modes[CRTC_REVERSE_MODE].draw_line = draw_reverse_line;
        if (old_size == 2) {
            if (IS_DOUBLE_WIDTH_ALLOWED(memptr_inc))
                window_width /= 2;
            if (IS_DOUBLE_HEIGHT_ALLOWED(crtc_screen_textlines*screen_charheight))
                window_height /= 2;
        }
    }
    old_size = DOUBLE_SIZE_ENABLED() ? 2 : 1;

    if (canvas) {
        resize(window_width, window_height);
        frame_buffer_clear(&frame_buffer, pixel_table[0]);
        force_repaint();
        refresh_changed();
        refresh_all();
    }
}


/* -------------------------------------------------------------------------- */

#if 0	/* def USE_VIDMODE_EXTENSION */
void video_setfullscreen(int v,int width, int height) {
    fullscreen = v;
    fullscreen_width = width;
    fullscreen_height = height;

    video_resize();
    if(v) {
        resize(width, height);
	refresh_changed();
	refresh_all();
    }
    video_resize();
}

void fullscreen_forcerepaint() {
    if(fullscreen) {
	video_resize();
        resize(fullscreen_width, fullscreen_height);
	refresh_changed();
	refresh_all();
	video_resize();
    }
}
#endif

