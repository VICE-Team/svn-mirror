/*
 * video.c - Common video functions for Win32
 *
 * Written by
 *  Tibor Biczo <crown@matavnet.hu>
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

#include "vice.h"

#include "cmdline.h"
#include "fullscrn.h"
#include "lib.h"
#include "log.h"
#include "palette.h"
#include "res.h"
#include "resources.h"
#include "translate.h"
#include "ui.h"
#include "uiapi.h"
#include "video.h"
#include "videoarch.h"
#include "viewport.h"


static int video_number_of_canvases;
static video_canvas_t *video_canvases[2];
static int dx9_available;

/* ------------------------------------------------------------------------ */
/* Video-related resources.  */

/* Flag: are we in fullscreen mode?  */
int fullscreen_enabled;
int dx_primary_surface_rendering;
int dx9_disable;

static int set_dx_primary_surface_rendering(int val, void *param)
{
    video_canvas_t *c;
    int i;

    dx_primary_surface_rendering = val;

    if (video_dx9_enabled()) {
        for (i = 0; i < video_number_of_canvases; i++)
            video_canvas_reset_dx9(video_canvases[i]);
    }

    return 0;
}

static int set_dx9_disable(int val, void *param)
{
    if (dx9_disable != val && video_number_of_canvases > 0) {
        ui_error("Sorry. Cannot change video engine on the fly. " \
                 "Please restart emulator to change engine.");
        return 0;
    }

    dx9_disable = val;
    return 0;
}

static const resource_int_t resources_int[] = {
    { "DXPrimarySurfaceRendering", 0, RES_EVENT_NO, NULL,
      &dx_primary_surface_rendering, set_dx_primary_surface_rendering, NULL },
    { "DX9Disable", 0, RES_EVENT_NO, NULL,
      &dx9_disable, set_dx9_disable, NULL },
    { NULL }
};

int video_arch_resources_init(void)
{
    return resources_register_int(resources_int);
}

void video_arch_resources_shutdown(void)
{
}

/* ------------------------------------------------------------------------ */

/* Video-related command-line options.  */

static const cmdline_option_t cmdline_options[] = {
    { "-fullscreen", SET_RESOURCE, 0,
      NULL, NULL, "FullScreenEnabled", (resource_value_t) 1,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDS_START_VICE_FULLSCREEN_MODE,
      NULL, NULL },
    { "-dx9disable", SET_RESOURCE, 0,
      NULL, NULL, "DX9Disable", (resource_value_t) 1,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDS_DISABLE_DX9,
      NULL, NULL },
    { NULL }
};


int video_init_cmdline_options(void)
{
    return cmdline_register_options(cmdline_options);
}


/* ------------------------------------------------------------------------ */

/* Initialization.  */
int video_init(void)
{
    return 0;
}

void video_shutdown(void)
{
    video_shutdown_dx9();
}

void video_arch_canvas_init(struct video_canvas_s *canvas)
{
    if (video_setup_dx9() < 0) {
        dx9_available = 0;
    } else {
        dx9_available = 1;
    }

    canvas->video_draw_buffer_callback = NULL;
}

int video_dx9_enabled(void)
{
    return (dx9_available && !dx9_disable);
}
/* ------------------------------------------------------------------------ */


video_canvas_t *video_canvas_for_hwnd(HWND hwnd)
{
    int i;

    for (i = 0; i < video_number_of_canvases; i++) {
        if (video_canvases[i]->hwnd == hwnd) {
            return video_canvases[i];
        }
    }

    return NULL;
}


void video_canvas_add(video_canvas_t *canvas)
{
    video_canvases[video_number_of_canvases++] = canvas;
}


video_canvas_t *video_canvas_create(video_canvas_t *canvas, unsigned int *width,
                                    unsigned int *height, int mapped)
{
    video_canvas_t *canvas_temp;

    fullscreen_transition = 1;

    canvas->title = lib_stralloc(canvas->viewport->title);
    canvas->width = *width;
    canvas->height = *height;

    if (canvas->videoconfig->doublesizex)
        canvas->width *= 2;

    if (canvas->videoconfig->doublesizey)
        canvas->height *= 2;

    ui_open_canvas_window(canvas);

    if (video_dx9_enabled()) {
        ui_canvas_child_window(canvas, 1);
        canvas_temp = video_canvas_create_dx9(canvas, width, height);
        if (canvas_temp == NULL) {
            log_debug("video: Falling back to DirectDraw canvas!");
            dx9_available = 0;
            ui_canvas_child_window(canvas, 0);
        } else {
            return canvas_temp;
        }
    }
    return video_canvas_create_ddraw(canvas, width, height);
}


void video_canvas_destroy(video_canvas_t *canvas)
{
    if (video_dx9_enabled()) {
        video_device_release_dx9(canvas);
    }

    if (canvas != NULL) {
        if (canvas->hwnd !=0) {
            DestroyWindow(canvas->hwnd);
        }
        if (canvas->title != NULL) {
            lib_free(canvas->title);
        }
        video_canvas_shutdown(canvas);
    }
}


int video_canvas_set_palette(video_canvas_t *canvas, palette_t *p)
{
    canvas->palette = p;
    if (canvas->depth == 8) {
        video_canvas_set_palette_ddraw_8bit(canvas);
    }

    video_set_palette(canvas);
    video_set_physical_colors(canvas);
    return 0;
}


int video_set_physical_colors(video_canvas_t *c)
{
    unsigned int i;
    int rshift;
    int rbits;
    int gshift;
    int gbits;
    int bshift;
    int bbits;
    DWORD rmask;
    DWORD gmask;
    DWORD bmask;

    if (video_dx9_enabled()) {
        /* Use hard coded D3DFMT_X8R8G8B8 format, driver does conversion */
        rshift = 16;
        rmask = 0xff;
        rbits = 0;

        gshift = 8;
        gmask = 0xff;
        gbits = 0;

        bshift = 0;
        bmask = 0xff;
        bbits = 0;
    } else {
        video_set_physical_colors_get_format_ddraw(c, &rshift, &rbits, &rmask,
                                                      &gshift, &gbits, &gmask,
                                                      &bshift, &bbits, &bmask);
    }

    if (c->depth > 8) {
        for (i = 0; i < 256; i++) {
            video_render_setrawrgb(i,
                ((i & (rmask << rbits)) >> rbits) << rshift,
                ((i & (gmask << gbits)) >> gbits) << gshift,
                ((i & (bmask << bbits)) >> bbits) << bshift);
        }
        video_render_initraw();
    }

    for (i = 0; i < c->palette->num_entries; i++) {
        DWORD p;

        if (c->depth == 8 /*&& !dx9_enabled*/) {
            p = video_get_color_from_palette_ddraw(c, i);
        } else {
            p = (((c->palette->entries[i].red&(rmask << rbits)) >> rbits)
                << rshift) +
                (((c->palette->entries[i].green&(gmask << gbits)) >> gbits)
                << gshift) +
                (((c->palette->entries[i].blue&(bmask << bbits)) >> bbits)
                << bshift);
        }
        video_render_setphysicalcolor(c->videoconfig, i, p, c->depth);
    }
    return 0;
}


/* Change the size of `s' to `width' * `height' pixels.  */
void video_canvas_resize(video_canvas_t *canvas, unsigned int width,
                         unsigned int height)
{
	int device;
    int fullscreen_width;
    int fullscreen_height;
    int bitdepth;
    int refreshrate;

    if (canvas->videoconfig->doublesizex)
        width *= 2;

    if (canvas->videoconfig->doublesizey)
        height *= 2;

    canvas->width = width;
    canvas->height = height;
    if (IsFullscreenEnabled()) {
        GetCurrentModeParameters(&device, &fullscreen_width,
								 &fullscreen_height, &bitdepth, &refreshrate);
    } else {
        canvas->client_width = width;
        canvas->client_height = height;
        ui_resize_canvas_window(canvas);
    }

    if (video_dx9_enabled()) {
        video_canvas_reset_dx9(canvas);
    }
}


/* Raster code has updated display */
void video_canvas_refresh(video_canvas_t *canvas,
                          unsigned int xs, unsigned int ys,
                          unsigned int xi, unsigned int yi,
                          unsigned int w, unsigned int h)
{
    if (video_dx9_enabled()) {
        video_canvas_refresh_dx9(canvas, xs, ys, xi, yi, w, h);
    } else {
        video_canvas_refresh_ddraw(canvas, xs, ys, xi, yi, w, h);
    }
}


/* Window got a WM_PAINT and needs a refresh */
void video_canvas_update(HWND hwnd, HDC hdc, int xclient, int yclient,
                               int w, int h)
{
    if (video_dx9_enabled()) {
        video_canvas_update_dx9(hwnd, hdc, xclient, yclient, w, h);
    } else {
        video_canvas_update_ddraw(hwnd, hdc, xclient, yclient, w, h);
    }
}
