/*
 * video.h - BEOS graphics handling
 *
 * Written by
 *  Andreas Matthies <andreas.matthies@gmx.net>
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

#ifdef EXACT_TYPE_NEEDED
#include "vicewindow.h"
#endif

#include "types.h"

typedef void (*canvas_redraw_t)();

struct canvas_s {
    char *title;
    unsigned int width, height;
    canvas_redraw_t exposure_handler;
    const struct palette_s *palette;
	
	/* Pointer for the ViceWindows for BWindow, BView and BBitmap for the canvas */
#ifdef EXACT_TYPE_NEEDED
    ViceWindow *vicewindow;
#else
    void *vicewindow;
#endif
 
    int use_triple_buffering;
};
typedef struct canvas_s canvas_t;

#define CANVAS_USES_TRIPLE_BUFFERING(c) (c->use_triple_buffering)

/* ------------------------------------------------------------------------- */

typedef struct video_frame_buffer_s {
    PIXEL   *buffer;
    int     width;
    int     height;
#ifdef EXACT_TYPE_NEEDED
    BBitmap *bitmap;
    ViceWindow *vicewindow;
#else
    void 	*bitmap;
    void	*vicewindow;
#endif
    int		real_width;		/* differs for widht%8 != 0 */
} video_frame_buffer_t;

#define VIDEO_FRAME_BUFFER_LINE_SIZE(f)     (f)->real_width
#define VIDEO_FRAME_BUFFER_LINE_START(f, n) ((f)->buffer+(n)*(f)->real_width)
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

#endif

