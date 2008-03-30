/*
 * video.h - Common video API.
 *
 * Written by
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

#ifndef _VIDEO_H
#define _VIDEO_H

#include "types.h"

typedef void (*void_t)(void);

struct palette_s;
struct video_frame_buffer_s;

extern int video_init_cmdline_options(void);
extern int video_init(void);
extern void video_free(void);

extern int video_frame_buffer_alloc(struct video_frame_buffer_s **i,
                                    unsigned int width, unsigned int height);
extern void video_frame_buffer_free(struct video_frame_buffer_s *i);
extern void video_frame_buffer_clear(struct video_frame_buffer_s *i,
                                     PIXEL value);

extern struct canvas_s *canvas_create(const char *win_name, unsigned int *width,
                                      unsigned int *height, int mapped,
                                      void_t exposure_handler,
                                      const struct palette_s *palette,
                                      PIXEL *pixel_return
#ifdef USE_GNOMEUI
                                      , struct video_frame_buffer_s *fb
#endif
                                      );
extern void canvas_refresh(struct canvas_s *canvas,
                           struct video_frame_buffer_s *frame_buffer,
                           unsigned int xs, unsigned int ys,
                           unsigned int xi, unsigned int yi,
                           unsigned int w, unsigned int h);
extern int canvas_set_palette(struct canvas_s *c,
                              const struct palette_s *palette,
                              PIXEL *pixel_return);
extern void canvas_destroy(struct canvas_s *s);
extern void canvas_map(struct canvas_s *s);
extern void canvas_unmap(struct canvas_s *s);
extern void canvas_resize(struct canvas_s *s, unsigned int width,
                          unsigned int height);


extern int video_resources_init(void);
extern int video_arch_init_resources(void);


/* Video render interface */

/* Commodore VIC/VIC-II/TED related color/palette types */
typedef struct video_cbm_color_s {
        float luminance;        /* luminance                      */
        float angle;            /* angle on color wheel           */
        int direction;          /* +1 (pos), -1 (neg) or 0 (grey) */
        char *name;             /* name of this color             */
} video_cbm_color_t;

typedef struct video_cbm_palette_s {
    unsigned int num_entries;           /* number of colors in palette */
    video_cbm_color_t *entries;         /* array of colors             */
    float saturation; /* base saturation of all colors except the grey tones */
    float phase;      /* color phase (will be added to all color angles) */
} video_cbm_palette_t;

struct raster_s;

extern void video_color_set_palette(video_cbm_palette_t *palette);
extern int video_color_update_palette(void);
extern void video_color_set_raster(struct raster_s *raster);

extern void video_render_main(DWORD *colortab, BYTE *src, BYTE *trg, int width,
                              int height, int xs, int ys, int xt, int yt,
                              int pitchs, int pitcht, int depth);
extern int video_render_get_fake_pal_state(void);

#endif

