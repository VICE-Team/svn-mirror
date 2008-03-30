/*
 * video.h - MS-DOS graphics handling, based on the Allegro library by Shawn
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

#ifndef _VIDEO_DOS_H
#define _VIDEO_DOS_H

#include <pc.h>			/* inportb(), outportb() */
#include <allegro.h>

#include "types.h"
#include "palette.h"

#define NUM_AVAILABLE_COLORS	0x100

typedef void (*canvas_redraw_t)();

struct _canvas {
    unsigned int width, height;
    RGB colors[NUM_AVAILABLE_COLORS];
    canvas_redraw_t exposure_handler;

    /* If set to nonzero, it means we are doing triple buffering on this
       canvas.  */
    int use_triple_buffering;

    /* Pages for triple buffering (the third page is the off-screen frame
       buffer).  */
    BITMAP *pages[2];

    /* Currently invisible page.  */
    int back_page;
};
typedef struct _canvas *canvas_t;

#define CANVAS_USES_TRIPLE_BUFFERING(c) (c->use_triple_buffering)

/* ------------------------------------------------------------------------- */

typedef BITMAP *frame_buffer_t;

#define FRAME_BUFFER_LINE_SIZE(f)	((f)->w)
#define FRAME_BUFFER_LINE_START(f, n)   ((f)->line[(n)])
#define FRAME_BUFFER_START(f)		(FRAME_BUFFER_LINE_START(f, 0))

/* ------------------------------------------------------------------------- */

struct _color_def {
    unsigned short red;
    unsigned short green;
    unsigned short blue;
    unsigned char dither;
};
typedef struct _color_def color_def_t;

/* ------------------------------------------------------------------------- */

struct _vga_mode {
    int width, height;
    char *description;
};
typedef struct _vga_mode vga_mode_t;

extern vga_mode_t vga_modes[];

enum vga_mode_id {
    VGA_320x200,
    VGA_360x240,
    VGA_360x270,
    VGA_376x282,
    VGA_400x300,
    VGA_640x480
};

#define NUM_VGA_MODES ((int)VGA_640x480 + 1)

/* ------------------------------------------------------------------------- */

extern int video_init_resources(void);
extern int video_init_cmdline_options(void);
extern int video_init(void);
extern int frame_buffer_alloc(frame_buffer_t * i, unsigned int width,
			      unsigned int height);
extern void frame_buffer_free(frame_buffer_t * i);
extern void frame_buffer_clear(frame_buffer_t * i, BYTE value);
extern canvas_t canvas_create(const char *win_name, unsigned int *width,
			      unsigned int *height, int mapped,
			      canvas_redraw_t exposure_handler,
			      const palette_t *palette, PIXEL *pixel_return);
extern void canvas_destroy(canvas_t s);
extern void canvas_map(canvas_t s);
extern void canvas_unmap(canvas_t s);
extern void canvas_resize(canvas_t s, unsigned int width, unsigned int height);
extern int canvas_set_palette(canvas_t c, const palette_t *p, PIXEL *pixel_return);
extern void enable_text(void);
extern void disable_text(void);
extern int num_text_lines(void);

extern void video_ack_vga_mode(void);

/* ------------------------------------------------------------------------- */

inline static void canvas_refresh(canvas_t c, frame_buffer_t f,
				  int xs, int ys, int xi, int yi, int w, int h)
{
    /* Just to be sure...  */
    if (screen == NULL)
        return;

    if (c->use_triple_buffering) {
#if 0
        /* (This should be theoretically correct, but in practice it makes us
           loose time, and sometimes click.  So it's better to just discard
           the frame if this happens, as we do in the #else case.  */
        while (poll_modex_scroll())
            /* Make sure we have finished flipping the previous frame.  */ ;
#else
        if (poll_modex_scroll())
            return;
#endif
        blit(f, c->pages[c->back_page], xs, ys, xi, yi, w, h);
        request_modex_scroll(0, c->back_page * c->height);
        c->back_page = 1 - c->back_page;
    } else {
        blit(f, screen, xs, ys, xi, yi, w, h);
    }
}

inline static void canvas_set_border_color(canvas_t canvas, BYTE color)
{
    inportb(0x3da);
    outportb(0x3c0, 0x31);
    outportb(0x3c0, color);
}

#endif	/* ndef _VIDEO_DOS_H */
