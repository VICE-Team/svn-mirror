/*
 * video.h - MS-DOS graphics handling, based on the Allegro library by Shawn
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

#ifndef _VIDEOARCH_H
#define _VIDEOARCH_H

#include <pc.h>			/* inportb(), outportb() */
#include <allegro.h>

typedef struct video_frame_buffer_s /* a bitmap structure */
{
   int w, h;                     /* width and height in pixels */
   int clip;                     /* flag if clipping is turned on */
   int cl, cr, ct, cb;           /* clip left, right, top and bottom values */
   GFX_VTABLE *vtable;           /* drawing functions */
   void (*write_bank)();         /* write bank selector, see bank.s */
   void (*read_bank)();          /* read bank selector, see bank.s */
   void *dat;                    /* the memory we allocated for the bitmap */
   int bitmap_id;                /* for identifying sub-bitmaps */
   void *extra;                  /* points to a structure with more info */
   int line_ofs;                 /* line offset (for screen sub-bitmaps) */
   int seg;                      /* bitmap segment */
   unsigned char *line[0];       /* pointers to the start of each line */
} video_frame_buffer_t;

#include "types.h"
#include "statusbar.h"

#define NUM_AVAILABLE_COLORS	0x100

typedef void (*canvas_redraw_t)();

struct canvas_s {
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
typedef struct canvas_s canvas_t;

#define CANVAS_USES_TRIPLE_BUFFERING(c) (c->use_triple_buffering)

/* ------------------------------------------------------------------------- */

#define VIDEO_FRAME_BUFFER_LINE_SIZE(f)     ((f)->w)
#define VIDEO_FRAME_BUFFER_LINE_START(f, n) ((f)->line[(n)])
#define VIDEO_FRAME_BUFFER_START(f)         (VIDEO_FRAME_BUFFER_LINE_START(f, 0))

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

extern void enable_text(void);
extern void disable_text(void);

extern void video_ack_vga_mode(void);
extern int video_in_gfx_mode(void);

/* ------------------------------------------------------------------------- */

inline static void canvas_refresh(canvas_t *c, video_frame_buffer_t *f,
				  unsigned int xs, unsigned int ys,
                                  unsigned int xi, unsigned int yi,
                                  unsigned int w, unsigned int h)
{
    int y_diff;

    /* Just to be sure...  */
    if (screen == NULL)
        return;

    /* don't overwrite statusbar */
    if (statusbar_enabled() && (yi < STATUSBAR_HEIGHT)) {
        y_diff = STATUSBAR_HEIGHT - yi;
        ys += y_diff;
        yi += y_diff;
        h -= y_diff;
    }
    if (statusbar_enabled() && (c->use_triple_buffering)
        && (yi >= c->height) && (yi < c->height + STATUSBAR_HEIGHT)) {
        y_diff = STATUSBAR_HEIGHT + c->height - yi;
        ys += y_diff;
        yi += y_diff;
        h -= y_diff;
    }

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
        blit((BITMAP *)f, c->pages[c->back_page], xs, ys, xi, yi, w, h);
        request_modex_scroll(0, c->back_page * c->height);
        c->back_page = 1 - c->back_page;
    } else {
        blit((BITMAP *)f, screen, xs, ys, xi, yi, w, h);
    }
}

inline static void canvas_set_border_color(canvas_t *canvas, BYTE color)
{
    inportb(0x3da);
    outportb(0x3c0, 0x31);
    outportb(0x3c0, color);
}

#endif

