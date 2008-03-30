/*
 * video.h - Video implementation for Win32, using DirectDraw.
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

/*  Nasty patch to avoid defining BYTE, WORD, DWORD more than once.  */

#undef BYTE
#undef WORD
#undef DWORD
#include <ddraw.h>

#include "types.h"
#include "palette.h"

#define CANVAS_USES_TRIPLE_BUFFERING(c) 0

#if 0
typedef struct _frame_buffer {
    LPDIRECTDRAWSURFACE dd_surface;
    DDSURFACEDESC dd_surface_desc;
} *frame_buffer_t;
/* Warning: This assumes the surface has been properly locked and
   corresponding values have been copied in the `dd_surface_desc' member.
   This is guarranteed by the module itself.  */
#define FRAME_BUFFER_LINE_SIZE(f)        (f)->dd_surface_desc.dwWidth;
#define FRAME_BUFFER_LINE_START(f, n)    ((PIXEL *) (f)->dd_surface_desc.lpSurface \
                                          + (n) * (f)->dd_surface_desc.lPitch)
#define FRAME_BUFFER_START(f)            (FRAME_BUFFER_LINE_START(f, 0))

#else

typedef struct _frame_buffer {
    int     width;
    int     height;
    PIXEL   *buffer;
} *frame_buffer_t;

#define FRAME_BUFFER_LINE_SIZE(f)        (f)->width
#define FRAME_BUFFER_LINE_START(f, n)    ((f)->buffer+(n)*(f)->width)
#define FRAME_BUFFER_START(f)            (FRAME_BUFFER_LINE_START(f, 0))
#endif

typedef void (*canvas_redraw_t)(unsigned int width, unsigned int height);

typedef struct _canvas {
    char *title;
    int width, height;
    int mapped;
    int depth;
    canvas_redraw_t exposure_handler;
    const palette_t *palette;
    DWORD physical_colors[256];
    PIXEL *pixels;
    HWND hwnd;
//    frame_buffer_t      frame_buffer;
    LPDIRECTDRAWSURFACE primary_surface;
    LPDIRECTDRAWSURFACE back_surface;
    LPDIRECTDRAWSURFACE temporary_surface;
    LPDIRECTDRAWCLIPPER clipper;
    LPDIRECTDRAWPALETTE dd_palette;
} *canvas_t;

/* ------------------------------------------------------------------------ */

extern void canvas_set_border_color(canvas_t canvas, BYTE color);

//extern  frame_buffer_t  main_fbuff;
//extern  canvas_t        main_canvas;

extern void canvas_render(canvas_t c, frame_buffer_t f,
                           int xs, int ys, int xi, int yi, int w, int h);

void canvas_update(HWND hwnd, HDC hdc, int xclient, int yclient, int w, int h);

#endif

