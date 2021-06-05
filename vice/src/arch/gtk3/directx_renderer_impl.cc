/**
 * \file directx_renderer_impl.cc
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

#include "vice.h"

#include "directx_renderer_impl.h"

#ifdef WIN32_COMPILE

#include <assert.h>
#include <math.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>

extern "C"
{
#include "archdep.h"
#include "log.h"
#include "render_queue.h"
#include "resources.h"
#include "videoarch.h"
}

#define CANVAS_LOCK() pthread_mutex_lock(&context->canvas_lock)
#define CANVAS_UNLOCK() pthread_mutex_unlock(&context->canvas_lock)
#define RENDER_LOCK() pthread_mutex_lock(&context->render_lock)
#define RENDER_UNLOCK() pthread_mutex_unlock(&context->render_lock)

void vice_directx_destroy_context_impl(vice_directx_renderer_context_t *context)
{
}

static void build_render_target(vice_directx_renderer_context_t *context)
{
    HRESULT result = S_OK;

    if (!context->factory)
    {
        result = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, &context->factory);
        if (FAILED(result))
        {
            vice_directx_impl_log_windows_error("D2D1CreateFactory");
            return;
        }
    }

    if (!context->render_target)
    {
        /*
        * Create the Direct2D render target through which we will draw
        */

        RECT rc;
        GetClientRect(context->window, &rc);

        D2D1_RENDER_TARGET_PROPERTIES render_target_properties = D2D1::RenderTargetProperties();
        render_target_properties.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_IGNORE);

        result =
            context->factory->CreateHwndRenderTarget(
                render_target_properties,
                D2D1::HwndRenderTargetProperties(context->window, D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top)),
                &context->render_target);

        if (FAILED(result))
        {
            vice_directx_impl_log_windows_error("CreateHwndRenderTarget");
            return;
        }
    }
}

static void build_render_bitmap(vice_directx_renderer_context_t *context, backbuffer_t *backbuffer)
{
    HRESULT result = S_OK;

    if (context->render_target) {
        /* If we have a bitmap, is it sill the right size? */
        if (context->render_bitmap && (context->bitmap_width != backbuffer->width || context->bitmap_height != backbuffer->height)) {
            /* Nope, release it and let another be created */
            context->render_bitmap->Release();
            context->render_bitmap = NULL;
        }

        /* Create a bitmap, if needed */
        if (!context->render_bitmap)
        {
            /*
             * Create the Direct2D bitmap used to get the emu display into the gpu
             */

            D2D1_BITMAP_PROPERTIES bitmap_properies;
            bitmap_properies.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_IGNORE);
            bitmap_properies.dpiX = 96.0f;
            bitmap_properies.dpiY = 96.0f;

            result =
                context->render_target->CreateBitmap(
                    D2D1::SizeU(backbuffer->width, backbuffer->height),
                    bitmap_properies,
                    &context->render_bitmap);

            if (FAILED(result))
            {
                vice_directx_impl_log_windows_error("CreateBitmap");
            }

            context->bitmap_width = backbuffer->width;
            context->bitmap_height = backbuffer->height;
            context->bitmap_pixel_aspect_ratio = backbuffer->pixel_aspect_ratio;
        }
    }
}

static void recalculate_layout(video_canvas_t *canvas, vice_directx_renderer_context_t *context)
{
    int keepaspect = 1;
    int trueaspect = 0;
    float scale_x;
    float scale_y;
    float dpi_x;
    float dpi_y;
    RECT rc;

    resources_get_int("KeepAspectRatio", &keepaspect);
    resources_get_int("TrueAspectRatio", &trueaspect);

    if (keepaspect) {
        float viewport_aspect;
        float emulated_aspect;

        viewport_aspect = (float)context->viewport_width / (float)context->viewport_height;
        emulated_aspect = (float)context->bitmap_width   / (float)context->bitmap_height;

        if (trueaspect) {
            emulated_aspect *= context->bitmap_pixel_aspect_ratio;
        }

        if (emulated_aspect < viewport_aspect) {
            scale_x = emulated_aspect / viewport_aspect;
            scale_y = 1.0f;
        } else {
            scale_x = 1.0f;
            scale_y = viewport_aspect / emulated_aspect;
        }
    } else {
        scale_x = 1.0f;
        scale_y = 1.0f;
    }

    /* Calculate the minimum drawing area size to be enforced by gtk */
    if (keepaspect && trueaspect) {
        context->window_min_width = ceil((float)context->bitmap_width * context->bitmap_pixel_aspect_ratio);
        context->window_min_height = context->bitmap_height;
    } else {
        context->window_min_width = context->bitmap_width;
        context->window_min_height = context->bitmap_height;
    }

    /* These values aren't used anywhere other than here in this renderer */
    canvas->screen_display_w = ((float)context->viewport_width  * scale_x);
    canvas->screen_display_h = ((float)context->viewport_height * scale_y);
    canvas->screen_origin_x = (((float)context->viewport_width  - canvas->screen_display_w) / 2.0);
    canvas->screen_origin_y = (((float)context->viewport_height - canvas->screen_display_h) / 2.0);

    /*
     * Direct2D thinks in terms of device indepenent pixels,
     * but GTK gives us pixel sizes in device dependent pixel sizes.
     *
     * When rendering, we convert from DDP to DIP, this ensures that
     * our filter choice is applied rather than whatever D2D is doing
     * under the hood when you set dpi on the bitmap itself.
     */

    context->factory->GetDesktopDpi(&dpi_x, &dpi_y);

    /* Update the detination rect used directly by the renderer */
    context->render_dest_rect.left   =  canvas->screen_origin_x                             * 96.0f / dpi_x;
    context->render_dest_rect.right  = (canvas->screen_origin_x + canvas->screen_display_w) * 96.0f / dpi_x;
    context->render_dest_rect.top    =  canvas->screen_origin_y                             * 96.0f / dpi_y;
    context->render_dest_rect.bottom = (canvas->screen_origin_y + canvas->screen_display_h) * 96.0f / dpi_y;

    /* Update the D2D render target size to match the UI size */
    if (context->render_target) {
        GetClientRect(context->window, &rc);
        context->render_target->Resize(D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top));
    }
}

void vice_directx_impl_async_render(void *job_data, void *pool_data)
{
    render_job_t job = (render_job_t)(int)(long long int)job_data;
    video_canvas_t *canvas = (video_canvas_t *)pool_data;
    vice_directx_renderer_context_t *context = (vice_directx_renderer_context_t *)canvas->renderer_context;
    HRESULT result = S_OK;
    HDC device_context;
    PAINTSTRUCT ps;
    backbuffer_t *backbuffer;
    int filter;

    if (job == render_thread_init) {
        archdep_thread_init();

        /* Make sure the render thread wakes up and does its thing asap. */
        SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);

        log_message(LOG_DEFAULT, "Render thread initialised");
        return;
    }

    if (job == render_thread_shutdown) {
        archdep_thread_shutdown();
        log_message(LOG_DEFAULT, "Render thread shutdown");
        return;
    }

    resources_get_int("GTKFilter", &filter);

    CANVAS_LOCK();

    backbuffer = render_queue_dequeue_for_display(context->render_queue);

    RENDER_LOCK();

    build_render_target(context);
    if (backbuffer) {
        build_render_bitmap(context, backbuffer);
    }

    recalculate_layout(canvas, context);

    CANVAS_UNLOCK();

    if (backbuffer && context->render_bitmap)
    {
        /* Copy the emulated screen to the Bitmap */
        D2D1_RECT_U bitmap_rect = D2D1::RectU(0, 0, backbuffer->width, backbuffer->height);
        result =
            context->render_bitmap->CopyFromMemory(
                &bitmap_rect,
                backbuffer->pixel_data,
                backbuffer->width * 4);

        if (FAILED(result))
        {
            vice_directx_impl_log_windows_error("CopyFromMemory");
        }
    }

    if (!context->render_target) {
        log_message(LOG_DEFAULT, "no render target, not rendering this frame");
        goto render_unlock_and_return_backbuffer;
    }

    device_context = BeginPaint(context->window, &ps);
    if (device_context == NULL) {
        log_message(LOG_DEFAULT, "no device context available, not rendering");
        goto render_unlock_and_return_backbuffer;
    }

    context->render_target->BeginDraw();
    context->render_target->SetTransform(D2D1::Matrix3x2F::Identity());
    context->render_target->Clear(&context->render_bg_colour);

    if (context->render_bitmap)
    {
        context->render_target->DrawBitmap(
            context->render_bitmap,
            context->render_dest_rect,
            1.0,
            filter ? D2D1_BITMAP_INTERPOLATION_MODE_LINEAR : D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);
    }

    result = context->render_target->EndDraw();
    EndPaint(context->window, &ps);
    ValidateRect(context->window, NULL);

    if (result == D2DERR_RECREATE_TARGET) {
        if (context->render_bitmap) {
            context->render_bitmap->Release();
            context->render_bitmap = NULL;
        }

        if (context->render_target) {
            context->render_target->Release();
            context->render_target = NULL;
        }
    }

render_unlock_and_return_backbuffer:
    RENDER_UNLOCK();

    /* Return the backbuffer to the pool */
    if (backbuffer) {
        render_queue_return_to_pool(context->render_queue, backbuffer);
    }
}

/*
 * Implemented here because we can't include the vice headers in a cpp file
 */
void vice_directx_impl_log_windows_error(const char *prefix)
{
    char *error_message;
    DWORD last_error = GetLastError();

    FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        last_error,
        MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
        (LPSTR)&error_message,
        0,
        NULL);

    log_error(LOG_ERR, "%s: %s", prefix, error_message);

    LocalFree(error_message);
}

#endif /* #ifdef WIN32_COMPILE */
