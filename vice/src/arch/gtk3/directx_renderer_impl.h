/**
 * \file directx_renderer.h
 * \brief   DirectX-based renderer for the GTK3 backend.
 *
 * \author David Hogan <david.q.hogan@gmail.com>
 */

/* This file is part of VICE, the Versatile Commodore Emulator.
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
#ifndef VICE_DIRECTX_RENDERER_IMPL_H
#define VICE_DIRECTX_RENDERER_IMPL_H

#ifdef WIN32_COMPILE

#include <windows.h>

#include <d2d1.h>
#include <glib.h>
#include <pthread.h>
#include <stdbool.h>

#include "render_thread.h"

#ifdef __cplusplus  
extern "C" { 
#endif

/** \brief Rendering context for the DirectX backend.
 *  \sa video_canvas_s::renderer_context */
typedef struct vice_directx_renderer_context_s {
    /** \brief needed to coordinate access to the context between vice and main threads */
    pthread_mutex_t canvas_lock;

    /** \brief used to coordinate access to native rendering resources */
    pthread_mutex_t render_lock;

    /** \brief A 'pool' of one thread used to render backbuffers via directx */
    render_thread_t render_thread;

    /** \brief A queue of backbuffers ready for painting to the widget */
    void *render_queue;

    /** \brief native child window for DirectX to draw on */
    HWND window;

    /** \brief minimum size for the drawing area, based on emu and aspect ratio settings */
    unsigned int window_min_width;

    /** \brief minimum size for the drawing area, based on emu and aspect ratio settings */
    unsigned int window_min_height;

    /** \brief Direct2D factory. x128 is weird if shared between VDC and VICII. */
    ID2D1Factory* factory;

    /** \brief Direct2D render target that renders on window */
    ID2D1HwndRenderTarget *render_target;

    /** \brief Direct2D bitmap used to get emu bitmap into the GPU */
    ID2D1Bitmap *render_bitmap;

    /** \brief Where emu bitmap gets placed on the target surface */
    D2D1_RECT_F render_dest_rect;

    /** \brief Background colour */
    D2D1_COLOR_F render_bg_colour;

    /** \brief location of the directx viewport in window pixels */
    unsigned int viewport_x;
    
    /** \brief size of the directx viewport in window pixels */
    unsigned int viewport_y;

    /** \brief size of the directx viewport in window pixels */
    unsigned int viewport_width;
    
    /** \brief size of the directx viewport in window pixels */
    unsigned int viewport_height;

    /** \brief size of the current gpu bitmap in pixels */
    unsigned int bitmap_width;
    
    /** \brief size of the current gpu bitmap in pixels */
    unsigned int bitmap_height;

    /** \brief aspect ratio of each pixel in the current gpu bitmap */
    float bitmap_pixel_aspect_ratio;

    /** \brief size of the next emulated frame */
    unsigned int emulated_width_next;
    
    /** \brief size of the NEXT emulated frame */
    unsigned int emulated_height_next;

    /** \brief pixel aspect ratio of the next emulated frame */
    float pixel_aspect_ratio_next;

} vice_directx_renderer_context_t;

void vice_directx_impl_log_windows_error(const char *prefix);

void vice_directx_impl_on_window_resize(vice_directx_renderer_context_t *context);
void vice_directx_destroy_context_impl(vice_directx_renderer_context_t *context);

void vice_directx_impl_async_render(void *pool_data, void *job_data);

#ifdef __cplusplus 
} 
#endif 

#endif /* #ifdef WIN32_COMPILE */

#endif /* #ifndef VICE_DIRECTX_RENDERER_IMPL_H */