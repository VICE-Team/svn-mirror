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

extern int video_init_resources(void);
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

#endif

