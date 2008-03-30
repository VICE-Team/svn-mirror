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

#define CANVAS_USES_TRIPLE_BUFFERING(c) 0

#if 0
typedef struct video_frame_buffer_s {
    LPDIRECTDRAWSURFACE dd_surface;
    DDSURFACEDESC dd_surface_desc;
} video_frame_buffer_t;
/* Warning: This assumes the surface has been properly locked and
   corresponding values have been copied in the `dd_surface_desc' member.
   This is guarranteed by the module itself.  */
#define VIDEO_FRAME_BUFFER_LINE_SIZE(f)     (f)->dd_surface_desc.dwWidth;
#define VIDEO_FRAME_BUFFER_LINE_START(f, n) ((PIXEL *)(f)->dd_surface_desc.lpSurface \
                                            + (n) * (f)->dd_surface_desc.lPitch)
#define VIDEO_FRAME_BUFFER_START(f)         (VIDEO_FRAME_BUFFER_LINE_START(f, 0))

#else

typedef struct video_frame_buffer_s {
    int     width;
    int     height;
    PIXEL   *buffer;
} video_frame_buffer_t;

#define VIDEO_FRAME_BUFFER_LINE_SIZE(f)     (f)->width
#define VIDEO_FRAME_BUFFER_LINE_START(f, n) ((f)->buffer + (n) * (f)->width)
#define VIDEO_FRAME_BUFFER_START(f)         (VIDEO_FRAME_BUFFER_LINE_START(f, 0))
#endif

typedef void (*canvas_redraw_t)(unsigned int width, unsigned int height);

struct palette_s;

typedef struct video_canvas_s {
    char *title;
    int width, height;
    int mapped;
    int depth;
    int videorendermode;
    canvas_redraw_t exposure_handler;
    const struct palette_s *palette;
    DWORD physical_colors[256];
    PIXEL *pixels;
    PIXEL *pixel_translate;
    HWND hwnd;
//    video_frame_buffer_t      frame_buffer;
    LPDIRECTDRAW        dd_object;
    LPDIRECTDRAW2       dd_object2;
    LPDIRECTDRAWSURFACE primary_surface;
    LPDIRECTDRAWSURFACE back_surface;
    LPDIRECTDRAWSURFACE temporary_surface;
    LPDIRECTDRAWCLIPPER clipper;
    LPDIRECTDRAWPALETTE dd_palette;
    int client_width;
    int client_height;
} video_canvas_t;

/* ------------------------------------------------------------------------ */

const char *dd_error(HRESULT ddrval);

int set_palette(video_canvas_t *c);
int set_physical_colors(video_canvas_t *c);
void video_frame_buffer_translate(video_canvas_t *c);


video_canvas_t *canvas_find_canvas_for_hwnd(HWND hwnd);

void canvas_set_border_color(video_canvas_t *canvas, BYTE color);

void canvas_render(video_canvas_t *c, video_frame_buffer_t *f,
                   int xs, int ys, int xi, int yi, int w, int h);

void canvas_update(HWND hwnd, HDC hdc, int xclient, int yclient, int w, int h);

#endif

