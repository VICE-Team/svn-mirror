/*
 * video_sdl2.c - SDL2 video
 *
 * Written by
 *  Hannu Nuotio <hannu.nuotio@tut.fi>
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
 *  Michael C. Martin <mcmartin@gmail.com>
 *  June Tate-Gans <june@theonelab.com>
 *
 * Based on code by
 *  Ettore Perazzoli
 *  Andre Fachat
 *  Oliver Schaertel
 *  Martin Pottendorfer
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

/* This file is a work in progress, and it is being refactored,
 * rewritten, and extended to take the maximum advantage of SDL2's
 * capabilities while still properly providing the special
 * capabilities that SDL VICE would like to generally support. Current
 * gaps:
 *
 * - The menu display sometimes ends up having a bad display until the
 *   user moves the cursor or resizes the window.
 * - The user's selected window size is not preserved across runs.
 */

/* #define SDL_DEBUG */

#include "vice.h"

#include <stdio.h>
#include "vice_sdl.h"

#include "archdep.h"
#include "cmdline.h"
#include "fullscreen.h"
#include "fullscreenarch.h"
#include "icon.h"
#include "joy.h"
#include "joystick.h"
#include "lib.h"
#include "log.h"
#include "machine.h"
#include "mousedrv.h"
#include "palette.h"
#include "raster.h"
#include "render-queue.h"
#include "resources.h"
#include "ui.h"
#include "uimenu.h"
#include "uistatusbar.h"
#include "util.h"
#include "videoarch.h"
#include "vkbd.h"
#include "vsidui_sdl.h"
#include "vsync.h"

#ifdef SDL_DEBUG
#define DBG(x)  log_debug x
#else
#define DBG(x)
#endif

#define CANVAS_LOCK() archdep_mutex_lock(canvas->lock)
#define CANVAS_UNLOCK() archdep_mutex_unlock(canvas->lock)

static log_t sdlvideo_log = LOG_ERR;
static int drv_index;

/* Initial w/h for windowed display */
static int sdl_initial_width[2] = { 0, 0 };
static int sdl_initial_height[2] = { 0, 0 };

/* Custom w/h, used for non-desktop fullscreen */
static int sdl_custom_width = 0;
static int sdl_custom_height = 0;

int sdl_active_canvas_num = 0;
static int sdl_num_screens = 0;
static video_canvas_t *sdl_canvaslist[MAX_CANVAS_NUM];
video_canvas_t *sdl_active_canvas = NULL;

static int sdl_gl_aspect_mode;
static char *aspect_ratio_s = NULL;
static char *aspect_ratio_factory_value_s = NULL;
static double aspect_ratio;

static int sdl_gl_flipx;
static int sdl_gl_flipy;

static int sdl_gl_filter_res;
static int sdl_gl_filter;
static int sdl2_dual_window;
static int sdl_vsync;

static char *sdl2_renderer_name = NULL;
static SDL_RendererFlip flip;
static Uint32 rmask = 0, gmask = 0, bmask = 0, amask = 0;
static int texformat = SDL_PIXELFORMAT_ARGB8888;

/* ------------------------------------------------------------------------- */
/* Video-related resources.  */

static void sdl_correct_logical_and_minimum_size(video_canvas_t *canvas, backbuffer_t *backbuffer);

static int set_sdl_custom_width(int w, void *param)
{
    if (w <= 0) {
        return -1;
    }

    if (sdl_custom_width != w) {
        sdl_custom_width = w;
        if (sdl_active_canvas && sdl_active_canvas->fullscreenconfig->enable
            && sdl_active_canvas->fullscreenconfig->mode == FULLSCREEN_MODE_CUSTOM) {
            video_viewport_resize(sdl_active_canvas, 1);
        }
    }
    return 0;
}

static int set_sdl_custom_height(int h, void *param)
{
    if (h <= 0) {
        return -1;
    }

    if (sdl_custom_height != h) {
        sdl_custom_height = h;
        if (sdl_active_canvas && sdl_active_canvas->fullscreenconfig->enable
            && sdl_active_canvas->fullscreenconfig->mode == FULLSCREEN_MODE_CUSTOM) {
            video_viewport_resize(sdl_active_canvas, 1);
        }
    }
    return 0;
}

static int set_sdl_initial_width(int w, void *param)
{
    int idx = vice_ptr_to_int(param);
    if (w < 0) {
        return -1;
    }

    sdl_initial_width[idx] = w;
    return 0;
}

static int set_sdl_initial_height(int h, void *param)
{
    int idx = vice_ptr_to_int(param);
    if (h < 0) {
        return -1;
    }

    sdl_initial_height[idx] = h;
    return 0;
}

static int set_sdl_gl_aspect_mode(int v, void *param)
{
    int old_v = sdl_gl_aspect_mode;

    switch (v) {
        case SDL_ASPECT_MODE_OFF:
        case SDL_ASPECT_MODE_CUSTOM:
        case SDL_ASPECT_MODE_TRUE:
            break;
        default:
            return -1;
    }

    sdl_gl_aspect_mode = v;

//    if (old_v != v) {
//        sdl_correct_logical_and_minimum_size();
//    }

    return 0;
}

static int set_aspect_ratio(const char *val, void *param)
{
    double old_aspect = aspect_ratio;
    char buf[20];

    if (val) {
        char *endptr;

        util_string_set(&aspect_ratio_s, val);

        aspect_ratio = strtod(val, &endptr);
        if (val == endptr) {
            aspect_ratio = 1.0;
        } else if (aspect_ratio < 0.5) {
            aspect_ratio = 0.5;
        } else if (aspect_ratio > 2.0) {
            aspect_ratio = 2.0;
        }
    } else {
        aspect_ratio = 1.0;
    }

    sprintf(buf, "%f", aspect_ratio);
    util_string_set(&aspect_ratio_s, buf);

//    if (old_aspect != aspect_ratio) {
//        if (sdl_active_canvas) {
//            video_viewport_resize(sdl_active_canvas, 1);
//            sdl_correct_logical_and_minimum_size();
//        }
//    }

    return 0;
}

static int set_sdl_gl_flipx(int v, void *param)
{
    sdl_gl_flipx = v ? 1 : 0;

    if (sdl_gl_flipx) {
        flip |= SDL_FLIP_HORIZONTAL;
    } else {
        flip &= ~SDL_FLIP_HORIZONTAL;
    }

    return 0;
}

static int set_sdl_gl_flipy(int v, void *param)
{
    sdl_gl_flipy = v ? 1 : 0;

    if (sdl_gl_flipy) {
        flip |= SDL_FLIP_VERTICAL;
    } else {
        flip &= ~SDL_FLIP_VERTICAL;
    }

    return 0;
}

static SDL_Texture *create_texture(SDL_Renderer* renderer, backbuffer_t *backbuffer)
{
    SDL_Texture *texture;

    texture = SDL_CreateTexture(renderer, texformat, SDL_TEXTUREACCESS_STREAMING, backbuffer->width, backbuffer->height);
    if (!texture) {
        log_error(sdlvideo_log, "SDL_CreateTexture() failed: %s\n", SDL_GetError());
        archdep_vice_exit(1);
    }
    
    return texture;
}

static void recreate_all_textures(void)
{
    int i;
    video_canvas_t *canvas;

    for (i = 0; i < sdl_num_screens; ++i) {
        canvas = sdl_canvaslist[i];
        if (!canvas) {
            continue;
        }

        CANVAS_LOCK();
        if (canvas->sdl_window) {
            canvas->sdl_window->recreate_resources = true;
        }
        CANVAS_UNLOCK();
    }
}

static int set_sdl_gl_filter(int v, void *param)
{
    switch (v) {
        case SDL_FILTER_NEAREST:
            sdl_gl_filter = GL_NEAREST;
            break;

        case SDL_FILTER_LINEAR:
            sdl_gl_filter = GL_LINEAR;
            break;

        default:
            return -1;
    }

    sdl_gl_filter_res = v;
    
    recreate_all_textures();
    
    return 0;
}

static int set_sdl2_renderer_name(const char *val, void *param)
{
    if (!val || val[0] == '\0') {
        util_string_set(&sdl2_renderer_name, "");
    } else {
        util_string_set(&sdl2_renderer_name, val);
    }
    return 0;
}

static int set_sdl2_dual_window(int v, void *param)
{
    sdl2_dual_window = v ? 1 : 0;

    return 0;
}

static int set_sdl_vsync(int v, void *param)
{
    sdl_vsync = v ? 1 : 0;

    return 0;
}

static resource_string_t resources_string[] = {
    /* CAUTION: position hardcoded below */
    { "AspectRatio", NULL, RES_EVENT_NO, NULL,
      &aspect_ratio_s, set_aspect_ratio, NULL },
    { "SDL2Backend", "", RES_EVENT_NO, NULL,
      &sdl2_renderer_name, set_sdl2_renderer_name, NULL },
    RESOURCE_STRING_LIST_END
};

#define VICE_DEFAULT_BITDEPTH 32

#define SDLCUSTOMWIDTH_DEFAULT   800
#define SDLCUSTOMHEIGHT_DEFAULT  600

/* FIXME: more resources should have the same name as their GTK counterparts,
          and the SDL prefix removed */
static const resource_int_t resources_int[] = {
    { "SDLCustomWidth", SDLCUSTOMWIDTH_DEFAULT, RES_EVENT_NO, NULL,
      &sdl_custom_width, set_sdl_custom_width, NULL },
    { "SDLCustomHeight", SDLCUSTOMHEIGHT_DEFAULT, RES_EVENT_NO, NULL,
      &sdl_custom_height, set_sdl_custom_height, NULL },
    { "Window0Width", 0, RES_EVENT_NO, NULL,
      &sdl_initial_width[0], set_sdl_initial_width, (void*)0 },
    { "Window0Height", 0, RES_EVENT_NO, NULL,
      &sdl_initial_height[0], set_sdl_initial_height, (void*)0 },
    { "SDLGLAspectMode", SDL_ASPECT_MODE_TRUE, RES_EVENT_NO, NULL,
      &sdl_gl_aspect_mode, set_sdl_gl_aspect_mode, NULL },
    { "SDLGLFlipX", 0, RES_EVENT_NO, NULL,
      &sdl_gl_flipx, set_sdl_gl_flipx, NULL },
    { "SDLGLFlipY", 0, RES_EVENT_NO, NULL,
      &sdl_gl_flipy, set_sdl_gl_flipy, NULL },
    { "SDLGLFilter", SDL_FILTER_LINEAR, RES_EVENT_NO, NULL,
      &sdl_gl_filter_res, set_sdl_gl_filter, NULL },
#ifdef USE_SDL2UI
    { "DualWindow", 0, RES_EVENT_NO, NULL,
      &sdl2_dual_window, set_sdl2_dual_window, NULL },
#endif
    { "VSync", 1, RES_EVENT_NO, NULL,
      &sdl_vsync, set_sdl_vsync, NULL },
    RESOURCE_INT_LIST_END
};

static const resource_int_t resources_int_c128[] = {
    { "Window1Width", 0, RES_EVENT_NO, NULL,
      &sdl_initial_width[1], set_sdl_initial_width, (void*)1 },
    { "Window1Height", 0, RES_EVENT_NO, NULL,
      &sdl_initial_height[1], set_sdl_initial_height, (void*)1 },
    RESOURCE_INT_LIST_END
};


int video_arch_resources_init(void)
{
    char buf[0x10];
    DBG(("%s", __func__));

    if (machine_class == VICE_MACHINE_VSID) {
        if (joy_sdl_resources_init() < 0) {
            return -1;
        }
    }

    /* KLUDGES: setup the factory default with a string, needs to be done at
       runtime since float format depends on locale */
    sprintf(buf, "%f", 1.0f);
    util_string_set(&aspect_ratio_factory_value_s, buf);
    resources_string[0].factory_value = aspect_ratio_factory_value_s;

    if (resources_register_string(resources_string) < 0) {
        return -1;
    }

    if (machine_class == VICE_MACHINE_C128) {
        if (resources_register_int(resources_int_c128) < 0) {
            return -1;
        }
    }

    return resources_register_int(resources_int);
}

void video_arch_resources_shutdown(void)
{
    DBG(("%s", __func__));

    if (machine_class == VICE_MACHINE_VSID) {
        joy_arch_resources_shutdown();
    }

    lib_free(aspect_ratio_s);
    lib_free(sdl2_renderer_name);
    lib_free(aspect_ratio_factory_value_s);
}

/* ------------------------------------------------------------------------- */
/* Video-related command-line options.  */

/* FIXME: more options should have the same name as their GTK counterparts,
          and the SDL prefix removed */
static const cmdline_option_t cmdline_options[] =
{
    { "-sdlcustomw", SET_RESOURCE, CMDLINE_ATTRIB_NEED_ARGS,
      NULL, NULL, "SDLCustomWidth", NULL,
      "<width>", "Set custom fullscreen resolution width" },
    { "-sdlcustomh", SET_RESOURCE, CMDLINE_ATTRIB_NEED_ARGS,
      NULL, NULL, "SDLCustomHeight", NULL,
      "<height>", "Set custom fullscreen resolution height" },
    { "-sdlinitialw", SET_RESOURCE, CMDLINE_ATTRIB_NEED_ARGS,
      NULL, NULL, "Window0Width", NULL,
      "<width>", "Set initial window width" },
    { "-sdlinitialh", SET_RESOURCE, CMDLINE_ATTRIB_NEED_ARGS,
      NULL, NULL, "Window0Height", NULL,
      "<height>", "Set initial window height" },
    { "-sdlaspectmode", SET_RESOURCE, CMDLINE_ATTRIB_NEED_ARGS,
      NULL, NULL, "SDLGLAspectMode", NULL,
      "<mode>", "Set aspect ratio mode (0 = off, 1 = custom, 2 = true)" },
    { "-aspect", SET_RESOURCE, CMDLINE_ATTRIB_NEED_ARGS,
      NULL, NULL, "AspectRatio", NULL,
      "<aspect ratio>", "Set custom aspect ratio (0.5 - 2.0)" },
    { "-sdlflipx", SET_RESOURCE, CMDLINE_ATTRIB_NONE,
      NULL, NULL, "SDLGLFlipX", (resource_value_t)1,
      NULL, "Enable X flip" },
    { "+sdlflipx", SET_RESOURCE, CMDLINE_ATTRIB_NONE,
      NULL, NULL, "SDLGLFlipX", (resource_value_t)0,
      NULL, "Disable X flip" },
    { "-sdlflipy", SET_RESOURCE, CMDLINE_ATTRIB_NONE,
      NULL, NULL, "SDLGLFlipY", (resource_value_t)1,
      NULL, "Enable Y flip" },
    { "+sdlflipy", SET_RESOURCE, CMDLINE_ATTRIB_NONE,
      NULL, NULL, "SDLGLFlipY", (resource_value_t)0,
      NULL, "Disable Y flip" },
    { "-sdlglfilter", SET_RESOURCE, CMDLINE_ATTRIB_NEED_ARGS,
      NULL, NULL, "SDLGLFilter", NULL,
      "<mode>", "Set OpenGL filtering mode (0 = nearest, 1 = linear)" },
    { "-sdl2backend", SET_RESOURCE, CMDLINE_ATTRIB_NEED_ARGS,
      NULL, NULL, "SDL2Backend", NULL,
      "<backend name>", "Set the preferred SDL2 backend" },
#ifdef USE_SDL2UI
    { "-dualwindow", SET_RESOURCE, CMDLINE_ATTRIB_NONE,
      NULL, NULL, "DualWindow", (void *)1,
      NULL, "Enable dual window rendering"},
    { "+dualwindow", SET_RESOURCE, CMDLINE_ATTRIB_NONE,
      NULL, NULL, "DualWindow", (void *)0,
      NULL, "Disable dual window rendering"},
#endif
    { "-vsync", SET_RESOURCE, CMDLINE_ATTRIB_NONE,
      NULL, NULL, "VSync", (void *)1,
      NULL, "Enable vsync to prevent tearing"},
    { "+vsync", SET_RESOURCE, CMDLINE_ATTRIB_NONE,
      NULL, NULL, "VSync", (void *)0,
      NULL, "Disable vsync"},
    CMDLINE_LIST_END
};

static const cmdline_option_t cmdline_options_c128[] =
{
    { "-sdlinitialw1", SET_RESOURCE, CMDLINE_ATTRIB_NEED_ARGS,
      NULL, NULL, "Window1Width", NULL,
      "<width>", "Set initial window width" },
    { "-sdlinitialh1", SET_RESOURCE, CMDLINE_ATTRIB_NEED_ARGS,
      NULL, NULL, "Window1Height", NULL,
      "<height>", "Set initial window height" },
    CMDLINE_LIST_END
};

int video_arch_cmdline_options_init(void)
{
    DBG(("%s", __func__));

    if (machine_class == VICE_MACHINE_VSID) {
        if (joystick_cmdline_options_init() < 0) {
            return -1;
        }
    }

    if (machine_class == VICE_MACHINE_C128) {
        if (cmdline_register_options(cmdline_options_c128) < 0) {
            return -1;
        }
    }

    return cmdline_register_options(cmdline_options);
}

/* ------------------------------------------------------------------------- */

int video_init(void)
{
    char rendername[256] = { 0 };
    char **renderlist = NULL;
    int renderamount = SDL_GetNumRenderDrivers();
    int it, l;
    SDL_RendererInfo info;

    sdlvideo_log = log_open("SDLVideo");

    /* Determine the SDL backend driver to use */
    /* Allocate renderlist strings */
    renderlist = lib_malloc((renderamount + 1) * sizeof(char *));

    /* Fill in the renderlist and render info string */
    for (it = 0; it < renderamount; ++it) {
        SDL_GetRenderDriverInfo(it, &info);

        strcat(rendername, info.name);
        strcat(rendername, " ");
        renderlist[it] = lib_strdup(info.name);
    }
    renderlist[it] = NULL;

    /* Check for resource preferred renderer */
    drv_index = -1;
    if (sdl2_renderer_name != NULL && *sdl2_renderer_name != '\0') {
        for (it = 0; it < renderamount; ++it) {
            if (!strcmp(sdl2_renderer_name, renderlist[it])) {
                drv_index = it;
            }
        }
        if (drv_index == -1) {
            log_warning(sdlvideo_log, "Resource preferred backend %s not available, trying arch default backend(s)", sdl2_renderer_name);
        }
    }

    for (l = 0; l < renderamount; ++l) {
        lib_free(renderlist[l]);
    }
    lib_free(renderlist);
    renderlist = NULL;

    log_message(sdlvideo_log, "Available backends: %s", rendername);

    return 0;
}

void video_shutdown(void)
{
    DBG(("%s", __func__));

    sdl_active_canvas = NULL;
}

/* ------------------------------------------------------------------------- */
/* static helper functions */

/** \brief  Given a canvas, generate a set of SDL_WindowFlags
 *
 * \return  A set of SDL_WindowFlags appropriate to the window resources.
 */
static SDL_WindowFlags sdl2_ui_generate_flags_for_canvas(const video_canvas_t* canvas)
{
    SDL_WindowFlags flags = 0;
    int minimized = 0;
    int hide_vdc = 0;

    if (machine_class == VICE_MACHINE_C128) {
        resources_get_int("C128HideVDC", &hide_vdc);
    }
    resources_get_int("StartMinimized", &minimized);

    if (minimized) {
      flags |= SDL_WINDOW_MINIMIZED;
    }

    if (hide_vdc && (canvas->index == VIDEO_CANVAS_IDX_VDC)) {
      flags |= SDL_WINDOW_HIDDEN;
    }

    if (canvas->fullscreenconfig->enable) {
      flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    } else {
      flags |= SDL_WINDOW_RESIZABLE;
    }

    return flags;
}

/** \brief Destroys an sdl_container_t that was previously created using
 *         sdl_container_create.
 */
static void sdl_window_destroy(sdl_window_t* sdl_window)
{
    if (!sdl_window) {
        return;
    }
    
    if (sdl_window->previous_frame_texture) {
        SDL_DestroyTexture(sdl_window->previous_frame_texture);
        sdl_window->previous_frame_texture = NULL;
    }
    
    if (sdl_window->texture) {
        SDL_DestroyTexture(sdl_window->texture);
        sdl_window->texture = NULL;
    }
    
    sdl_window->texture_width = 0;
    sdl_window->texture_height = 0;
    
    if (sdl_window->sdl_surface) {
        SDL_FreeSurface(sdl_window->sdl_surface);
        sdl_window->sdl_surface = NULL;
    }

    if (sdl_window->renderer) {
        SDL_DestroyRenderer(sdl_window->renderer);
        sdl_window->renderer = NULL;
    }

    if (sdl_window->window) {
        SDL_DestroyWindow(sdl_window->window);
        sdl_window->window = NULL;
    }

    lib_free(sdl_window);
}

/** \brief  Given a canvas index, create an sdl_container_t and return it.
 *
 * This creates a window using the dimensions from the canvas referred to by
 * canvas_idx, and finally allocates a renderer. This does not allocate the
 * texture -- that is done JIT as needed.
 *
 * \return  a fully initialized and allocated sdl_container_t struct, or NULL on
 *          failure.
 */
static sdl_window_t* sdl_window_create(int canvas_idx)
{
    unsigned int window_width = 0, window_height = 0;
    video_canvas_t* canvas = sdl_canvaslist[canvas_idx];
    sdl_window_t* sdl_window = NULL;
    SDL_WindowFlags flags = sdl2_ui_generate_flags_for_canvas(canvas);
    
    CANVAS_LOCK();

    /*
     * Detemine the initial window size. Overridable by Window[01]Width / Window[01]Height resources
     */
    
    if (sdl_initial_width[canvas_idx]) {
        window_width = sdl_initial_width[canvas_idx];
    } else {
        window_width = canvas->draw_buffer->visible_width * canvas->videoconfig->scalex;
        if (sdl_gl_aspect_mode == SDL_ASPECT_MODE_CUSTOM) {
            window_width *= aspect_ratio;
        } else if (sdl_gl_aspect_mode == SDL_ASPECT_MODE_TRUE) {
            window_width *= canvas->geometry->pixel_aspect_ratio;
        }
    }

    if (sdl_initial_height[canvas_idx]) {
        window_height = sdl_initial_height[canvas_idx];
    } else {
        window_height = canvas->draw_buffer->visible_height * canvas->videoconfig->scaley;
    }

    sdl_window = lib_calloc(1, sizeof(*sdl_window));
    sdl_window->window = SDL_CreateWindow("",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          window_width, window_height,
                                          flags);
    if (sdl_window->window == NULL) {
        sdl_window_destroy(sdl_window);
        log_error(sdlvideo_log, "SDL_CreateWindow() failed: %s\n", SDL_GetError());
        archdep_vice_exit(1);
    }

    SDL_SetWindowData(sdl_window->window, VIDEO_SDL2_CANVAS_INDEX_KEY, (void*)(canvas));
    sdl_ui_set_window_icon(sdl_window->window);

    sdl_window->last_width = window_width;
    sdl_window->last_height = window_height;

    sdl_window->recreate_resources = true;

    /* Enable file/text drag and drop support */
    SDL_EventState(SDL_DROPFILE, SDL_ENABLE);

    /* Explicitly minimize if the window was created minimized */
    if ((flags & SDL_WINDOW_MINIMIZED) != 0) {
        SDL_MinimizeWindow(sdl_window->window);
    }
    
    CANVAS_UNLOCK();

    return sdl_window;
}

/** \brief Predicate function to determine if a canvas is visible to the user.
 *
 * \returns 1 if the canvas is currently visible to the user, or 0 if not.
 */
static int sdl_canvas_is_visible(struct video_canvas_s *canvas)
{
    if (canvas == sdl_active_canvas) {
        return 1;
    }

    int other_canvas_idx = canvas->index ^ 1;
    video_canvas_t* other_canvas = sdl_canvaslist[other_canvas_idx];

    if (canvas->sdl_window != other_canvas->sdl_window) {
        return 1;
    }

    return 0;
}

/* ------------------------------------------------------------------------- */
/* Main API */

/* called from raster/raster.c:realize_canvas */
video_canvas_t *video_canvas_create(video_canvas_t *canvas, unsigned int *width, unsigned int *height, int mapped)
{
    /* nothing to do here, the real work is done in sdl_ui_init_finalize */
    return canvas;
}

void video_canvas_new_frame_hook(struct video_canvas_s *canvas)
{
    if (sdl_vsid_state & SDL_VSID_ACTIVE) {
        sdl_vsid_draw();
    }

    if (sdl_vkbd_state & SDL_VKBD_ACTIVE) {
        sdl_vkbd_draw();
    }

    if (uistatusbar_state & UISTATUSBAR_ACTIVE) {
        uistatusbar_draw();
    }
}

void video_canvas_on_new_backbuffer(video_canvas_t *canvas)
{
    SDL_Event sdl_new_frame_event;
    
    sdl_new_frame_event.type        = sdl_event_new_video_frame;
    sdl_new_frame_event.user.data1  = canvas;
    SDL_PushEvent(&sdl_new_frame_event);
}

static void recreate_sdl_resources(video_canvas_t *canvas, backbuffer_t *backbuffer)
{
    sdl_window_t *sdl_window;
    SDL_RendererInfo info;
    int vsync = 0;
    
    CANVAS_LOCK();
    
    sdl_window = canvas->sdl_window;
    
    DBG(("%s: %ux%u (%i)", __func__, width, height, canvas->index));
    
    /*
     * Clean up previous resources
     */
    
    if (sdl_window->previous_frame_texture) {
        SDL_DestroyTexture(sdl_window->previous_frame_texture);
        sdl_window->previous_frame_texture = NULL;
    }

    if (sdl_window->texture) {
        SDL_DestroyTexture(sdl_window->texture);
        sdl_window->texture = NULL;
    }
    
    if (sdl_window->sdl_surface) {
        SDL_FreeSurface(sdl_window->sdl_surface);
        sdl_window->sdl_surface = NULL;
    }

    if (sdl_window->renderer) {
        SDL_DestroyRenderer(sdl_window->renderer);
        sdl_window->sdl_surface = NULL;
    }

    /* setup vsync */
    resources_get_int("VSync", &vsync);
    if (vsync) {
        if (canvas != sdl_active_canvas) {
            /*
             * With SDL, we only want vsync on the active window because all windows will be rendered with the same thread.
             * If both VIC and VDC have vsync enabled, they they will alternately block each other waiting for vsync.
             */

            log_message(sdlvideo_log, "VSync is disabled (inactive canvas)");
            vsync = 0;
        } else {
            log_message(sdlvideo_log, "VSync is enabled");
        }
    } else {
        log_message(sdlvideo_log, "VSync is disabled");
    }
    
    if (vsync) {
        SDL_SetHintWithPriority(SDL_HINT_RENDER_VSYNC, "1", SDL_HINT_OVERRIDE);
        sdl_window->renderer = SDL_CreateRenderer(sdl_window->window,
                                                drv_index,
                                                SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    } else {
        SDL_SetHintWithPriority(SDL_HINT_RENDER_VSYNC, "0", SDL_HINT_OVERRIDE);
        sdl_window->renderer = SDL_CreateRenderer(sdl_window->window,
                                                drv_index,
                                                SDL_RENDERER_ACCELERATED);
    }

    if (!sdl_window->renderer) {
        log_error(sdlvideo_log, "SDL_CreateRenderer() failed: %s", SDL_GetError());
        archdep_vice_exit(1);
    }

    SDL_GetRendererInfo(sdl_window->renderer, &info);
    log_message(sdlvideo_log, "SDL2 backend driver selected: %s", info.name);
//    SDL_SetRenderDrawColor(sdl_window->renderer, 0, 0, 0, 255);
//    SDL_RenderClear(sdl_window->renderer);
//    SDL_RenderPresent(sdl_window->renderer);
    
    /*
     * Update the fullscreen status
     */
    
    if (canvas == sdl_active_canvas) {
        if (canvas->fullscreenconfig->enable) {
            if (canvas->fullscreenconfig->mode == FULLSCREEN_MODE_CUSTOM) {
                SDL_SetWindowSize(sdl_window->window, sdl_custom_width, sdl_custom_height);
                SDL_SetWindowFullscreen(sdl_window->window, SDL_WINDOW_FULLSCREEN);
            } else {
                SDL_SetWindowFullscreen(sdl_window->window, SDL_WINDOW_FULLSCREEN_DESKTOP);
            }
        } else {
            int flags = SDL_GetWindowFlags(sdl_window->window);
            if (flags & (SDL_WINDOW_FULLSCREEN | SDL_WINDOW_FULLSCREEN_DESKTOP)) {
                SDL_SetWindowFullscreen(sdl_window->window, 0);
                sdl_window->leaving_fullscreen = 1;
            }
        }
    }
    
    /*
     * Surface
     */
        
    sdl_window->sdl_surface = SDL_CreateRGBSurface(0, backbuffer->width, backbuffer->height, 32, rmask, gmask, bmask, amask);
    if (!sdl_window->sdl_surface) {
        log_error(sdlvideo_log, "SDL_CreateRGBSurface() failed: %s\n", SDL_GetError());
        archdep_vice_exit(1);
    }
    
    /*
     * Textures
     */
    
    sdl_window->texture_width = 0;
    sdl_window->texture_height = 0;

    /* This hint controls the scaling mode of textures created afterwards */
    if (sdl_gl_filter_res == SDL_FILTER_LINEAR) {
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    } else {
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
    }

    sdl_window->texture = create_texture(sdl_window->renderer, backbuffer);

    if (canvas->videoconfig->interlaced) {
        sdl_window->previous_frame_texture = create_texture(sdl_window->renderer, backbuffer);
    }
    
    sdl_window->texture_width = backbuffer->width;
    sdl_window->texture_height = backbuffer->height;
    
    log_message(sdlvideo_log, "%s (%s) %ux%u %ibpp %s", canvas->videoconfig->chip_name, (canvas == sdl_active_canvas) ? "active" : "inactive", backbuffer->width, backbuffer->height, 32, (canvas->fullscreenconfig->enable) ? " (fullscreen)" : "");

    video_canvas_set_palette(canvas, canvas->palette);
    
    sdl_correct_logical_and_minimum_size(canvas, backbuffer);

    CANVAS_UNLOCK();
}

void video_canvas_display_backbuffer(video_canvas_t *canvas)
{
    backbuffer_t *backbuffer;
    void *pixels;
    int pitch;
    SDL_Texture *texture_swap;
    sdl_window_t *sdl_window;
    
    CANVAS_LOCK();
    
    if (!sdl_canvas_is_visible(canvas)) {
        CANVAS_UNLOCK();
        return;
    }
    
    sdl_window = canvas->sdl_window;
    
    /* Process any new frame to render */
    backbuffer = render_queue_dequeue_for_display(canvas->render_queue);
    if (backbuffer) {
        
        if (    sdl_window->texture_width != backbuffer->width
            ||  sdl_window->texture_height != backbuffer->height
            ||  sdl_window->recreate_resources
            ) {
            recreate_sdl_resources(canvas, backbuffer);
            sdl_window->recreate_resources = false;

        } else if (canvas->videoconfig->interlaced && !sdl_menu_state) {
            /* Swap the interlaced textures so we can easily re-render previous frame under this one */
            texture_swap = sdl_window->previous_frame_texture;
            sdl_window->previous_frame_texture = sdl_window->texture;
            sdl_window->texture = texture_swap;
        }

        /* Render the deferred frame onto the texture */
        SDL_LockTexture(sdl_window->texture, NULL, &pixels, &pitch);
        video_canvas_render_backbuffer(backbuffer, pixels, pitch);
        render_queue_return_to_pool(canvas->render_queue, backbuffer);
        SDL_UnlockTexture(sdl_window->texture);
    }
    
    /*
     * Display on host.
     */
    
    SDL_RenderClear(sdl_window->renderer);

    if (canvas->videoconfig->interlaced && !sdl_menu_state) {
        /*
         * Interlaced mode: Re-render last frame to render new frame over.
         * We don't do this if the SDL menu is showing, otherwise the first
         * render of the menu shows the emu screen behind it!
         */
        SDL_SetTextureBlendMode(sdl_window->previous_frame_texture, SDL_BLENDMODE_NONE);
        SDL_RenderCopyEx(sdl_window->renderer, sdl_window->previous_frame_texture, NULL, NULL, 0, NULL, flip);
        SDL_SetTextureBlendMode(sdl_window->texture, SDL_BLENDMODE_BLEND);
    } else {
        SDL_SetTextureBlendMode(sdl_window->texture, SDL_BLENDMODE_NONE);
    }
    SDL_RenderCopyEx(sdl_window->renderer, sdl_window->texture, NULL, NULL, 0, NULL, flip);

    SDL_RenderPresent(sdl_window->renderer);

    if (sdl_window->leaving_fullscreen) {
        int curr_w, curr_h, flags;
        int last_width = sdl_window->last_width;
        int last_height = sdl_window->last_height;

        SDL_GetWindowSize(sdl_window->window, &curr_w, &curr_h);
        flags = SDL_GetWindowFlags(sdl_window->window);
        sdl_window->leaving_fullscreen = 0;

        if ((curr_w != last_width || curr_h != last_height) &&
            (flags & (SDL_WINDOW_FULLSCREEN | SDL_WINDOW_FULLSCREEN_DESKTOP |
                      SDL_WINDOW_MAXIMIZED)) == 0) {
            log_message(sdlvideo_log, "Resolution anomaly leaving fullscreen: expected %dx%d, got %dx%d", last_width, last_height, curr_w, curr_h);
            SDL_SetWindowSize(sdl_window->window, last_width, last_height);
        }
    }

    ui_autohide_mouse_cursor();

    CANVAS_UNLOCK();
}

int video_canvas_set_palette(struct video_canvas_s *canvas, struct palette_s *palette)
{
    unsigned int i, col = 0;
    video_render_color_tables_t *color_tables = &canvas->videoconfig->color_tables;
    SDL_PixelFormat *fmt;

    DBG(("video_canvas_set_palette canvas: %p", canvas));

    if (palette == NULL) {
        return 0; /* no palette, nothing to do */
    }

    canvas->palette = palette;

    if (canvas->sdl_window->sdl_surface == NULL) {
        log_error(LOG_DEFAULT, "FIXME: video_canvas_set_palette() canvas->screen is NULL");
        return 0;
    }
    fmt = canvas->sdl_window->sdl_surface->format;

//    /* Fixme: needs further investigation how it can reach here without being fully initialized */
//    if (canvas != sdl_active_canvas || canvas->draw_buffer->width != canvas->sdl_window->sdl_surface->w) {
//        DBG(("video_canvas_set_palette not active canvas or window not created, don't update hw palette"));
//        return 0;
//    }

    for (i = 0; i < palette->num_entries; i++) {
        col = SDL_MapRGB(fmt, palette->entries[i].red, palette->entries[i].green, palette->entries[i].blue);
        video_render_setphysicalcolor(canvas->videoconfig, i, col);
    }

    for (i = 0; i < 256; i++) {
        video_render_setrawrgb(color_tables, i, SDL_MapRGB(fmt, (Uint8)i, 0, 0), SDL_MapRGB(fmt, 0, (Uint8)i, 0), SDL_MapRGB(fmt, 0, 0, (Uint8)i));
    }
    video_render_initraw(canvas->videoconfig);

    return 0;
}

//static void sdl_correct_logical_size(void)
//{
//    for (int i = 0; i < sdl_num_screens; ++i) {
//        video_canvas_t* canvas = sdl_canvaslist[i];
//        sdl_window_t* sdl_window = canvas->sdl_window;
//
//        if (sdl_window && sdl_window->window) {
//            int corrected_width, corrected_height;
//
//            if (sdl_gl_aspect_mode == SDL_ASPECT_MODE_OFF) {
//                SDL_GetWindowSize(sdl_window->window, &corrected_width, &corrected_height);
//            } else {
//                double aspect = (sdl_gl_aspect_mode == SDL_ASPECT_MODE_CUSTOM) ? aspect_ratio : canvas->geometry->pixel_aspect_ratio;
//                corrected_width = canvas->draw_buffer->width * aspect;
//                corrected_height = canvas->draw_buffer->height;
//            }
//
//            SDL_RenderSetLogicalSize(sdl_window->renderer, corrected_width, corrected_height);
//        }
//    }
//}

static void sdl_correct_logical_and_minimum_size(video_canvas_t *canvas, backbuffer_t *backbuffer)
{
    sdl_window_t *sdl_window = canvas->sdl_window;
    int aspect_adjusted_width;
    int aspect_adjusted_height;
    double aspect;
    
    if (sdl_gl_aspect_mode == SDL_ASPECT_MODE_OFF) {
        aspect_adjusted_width = backbuffer->width;
        aspect_adjusted_height = backbuffer->height;
    } else {
        aspect = (sdl_gl_aspect_mode == SDL_ASPECT_MODE_CUSTOM) ? aspect_ratio : canvas->geometry->pixel_aspect_ratio;
        aspect_adjusted_width = backbuffer->width * aspect;
        aspect_adjusted_height = backbuffer->height;
    }
    
    SDL_RenderSetLogicalSize(sdl_window->renderer, aspect_adjusted_width, aspect_adjusted_height);
    SDL_SetWindowMinimumSize(sdl_window->window, aspect_adjusted_width, aspect_adjusted_height);
}

/* Resize window to w/h. */
void sdl2_video_resize_event(int canvas_idx, unsigned int w, unsigned int h)
{
    sdl_window_t* container = sdl_canvaslist[canvas_idx]->sdl_window;
    SDL_Window* window = container->window;
    int flags = SDL_GetWindowFlags(window);

    if ((flags & (SDL_WINDOW_FULLSCREEN | SDL_WINDOW_FULLSCREEN_DESKTOP |
                  SDL_WINDOW_MAXIMIZED)) == 0) {
        /* We aren't in some fullscreen-or-close-to-it mode, and so this is
         * a "legitimate" resize. Record that size for comparison against
         * what we see when we leave fullscreen. */
        container->last_width = w;
        container->last_height = h;

        resources_set_int_sprintf("Window%dWidth", w, canvas_idx);
        resources_set_int_sprintf("Window%dHeight", h, canvas_idx);
    }

//    sdl_correct_logical_size();
}

/* Resize window to stored real size */
void sdl2_video_restore_size(void)
{
    SDL_Event event;
    event.type = sdl_event_restore_window_size;
    
    SDL_PushEvent(&event);
}

void sdl2_video_restore_size_impl(void)
{
    video_canvas_t *canvas = sdl_active_canvas;
    int width;
    int height;
    
    log_message(LOG_DEFAULT, "restore start");
    
    CANVAS_LOCK();
    
    SDL_RenderGetLogicalSize(canvas->sdl_window->renderer, &width, &height);
    SDL_SetWindowSize(canvas->sdl_window->window, width, height);
    
    CANVAS_UNLOCK();
    
    log_message(LOG_DEFAULT, "restore end");
}

void sdl_video_canvas_switch(int index)
{
    struct video_canvas_s *canvas;
    int i;

    DBG(("%s: %i->%i", __func__, sdl_active_canvas_num, index));

    if (sdl_active_canvas_num == index) {
        return;
    }

    if (index >= sdl_num_screens) {
        return;
    }

    /* Trigger a rebuild of all SDL video resources */
    for (i = 0; i < MAX_CANVAS_NUM; i++) {
        canvas = sdl_canvaslist[i];
        if (!canvas) {
            continue;
        }
        
        /* Recreate the renderer via a repaint */
        canvas->sdl_window->recreate_resources = 1;

        /* Use a high priorty refresh to return all queued buffers to the pool */
        video_canvas_refresh_all(canvas, true);
    }

    sdl_active_canvas_num = index;

    canvas = sdl_canvaslist[sdl_active_canvas_num];
    sdl_active_canvas = canvas;

    if (sdl_active_canvas->sdl_window) {
        SDL_SetWindowData(sdl_active_canvas->sdl_window->window,
                          VIDEO_SDL2_CANVAS_INDEX_KEY,
                          (void*)(sdl_active_canvas));
    }

    video_viewport_resize(canvas, 1);
}

int video_arch_get_active_chip(void)
{
    if (sdl_active_canvas_num == VIDEO_CANVAS_IDX_VDC) {
        return VIDEO_CHIP_VDC;
    } else {
        return VIDEO_CHIP_VICII;
    }
}

void video_arch_canvas_init(struct video_canvas_s *canvas)
{
    DBG(("%s: (%p, %i)", __func__, canvas, sdl_num_screens));

    if (sdl_num_screens == MAX_CANVAS_NUM) {
        log_error(sdlvideo_log, "Too many canvases!");
        archdep_vice_exit(-1);
    }

    canvas->fullscreenconfig = lib_calloc(1, sizeof(fullscreenconfig_t));

    if (sdl_active_canvas_num == sdl_num_screens) {
        sdl_active_canvas = canvas;
    }

    canvas->index = sdl_num_screens;
    sdl_canvaslist[sdl_num_screens++] = canvas;
}

void video_canvas_destroy(struct video_canvas_s *canvas)
{
    int i;

    DBG(("%s: (%p, %i)", __func__, canvas, canvas->index));

    for (i = 0; i < sdl_num_screens; ++i) {
        if (sdl_canvaslist[i] == canvas) {
            /* If the second window isn't visible, then both canvas lists should
               be sharing the same container. Set the other one to NULL
               directly so we don't accidentally double free. */
            if (sdl_canvaslist[i ^ 1]) {
                if (sdl_canvaslist[i]->sdl_window == sdl_canvaslist[i ^ 1]->sdl_window) {
                    sdl_canvaslist[i ^ 1]->sdl_window = NULL;
                }
            }
            sdl_window_destroy(sdl_canvaslist[i]->sdl_window);
            sdl_canvaslist[i]->sdl_window = NULL;
        }
    }

    lib_free(canvas->fullscreenconfig);
}

void sdl2_hide_second_window(void)
{
    SDL_Event event;
    event.type = sdl_event_second_window_hide;

    SDL_PushEvent(&event);
}

/** \brief  Hides the secondary window.
 *
 * Internally this just destroys the window and its textures.
 */
void sdl2_hide_second_window_impl(void)
{
    int inactive_canvas_idx = sdl_active_canvas->index ^ 1;
    video_canvas_t* inactive_canvas = sdl_canvaslist[inactive_canvas_idx];
    
    sdl_window_t* inactive_sdl_window = inactive_canvas->sdl_window;
    sdl_window_t* active_sdl_window = sdl_active_canvas->sdl_window;

    if (active_sdl_window != inactive_sdl_window) {
        DBG(("%s active: %d, inactive: %d", __func__,
             sdl_active_canvas->index, inactive_canvas_idx));

        inactive_canvas->sdl_window = active_sdl_window;

        sdl_window_destroy(inactive_sdl_window);

        /* Force a recretion of the textures since we have effectively changed
           our renderer, and SDL textures can't be shared between renderers. */
        active_sdl_window->recreate_resources = true;

        sdl_ui_refresh();

        SDL_RaiseWindow(active_sdl_window->window);
    }
}

void sdl2_show_second_window(void)
{
    SDL_Event event;
    event.type = sdl_event_second_window_show;

    SDL_PushEvent(&event);
}

/** \brief  Shows the secondary window.
 *
 * Internally, this creates a new window by calling `sdl_container_create`.
 */
void sdl2_show_second_window_impl(void)
{
    int inactive_canvas_idx = sdl_active_canvas->index ^ 1;
    video_canvas_t* inactive_canvas = sdl_canvaslist[inactive_canvas_idx];
    
    sdl_window_t* inactive_sdl_window = inactive_canvas->sdl_window;
    sdl_window_t* active_sdl_window = sdl_active_canvas->sdl_window;

    if (active_sdl_window == inactive_sdl_window) {
        sdl_window_t* new_sdl_window = sdl_window_create(inactive_canvas_idx);

        DBG(("%s active: %d, inactive: %d", __func__,
             sdl_active_canvas->index, inactive_canvas_idx));

        inactive_canvas->sdl_window = new_sdl_window;
        
        sdl_ui_refresh();

        SDL_RaiseWindow(active_sdl_window->window);
    }
}

void sdl_ui_init_finalize(void)
{
    int minimized = 0;
    int dual_windows = 0;
    int hide_vdc = 0;
    sdl_window_t* sdl_window = NULL;

    resources_get_int("DualWindow", &dual_windows);
    if (machine_class == VICE_MACHINE_C128) {
        resources_get_int("C128HideVDC", &hide_vdc);
    }
    resources_get_int("StartMinimized", &minimized);

    /* Setup the primary sdl window using the active canvas */
    sdl_window = sdl_window_create(sdl_active_canvas->index);

    for (int i = 0; i < sdl_num_screens; i++) {
        sdl_canvaslist[i]->sdl_window = sdl_window;
    }

    /* If we're setup for dual windows, then we need to allocate a new container
     * for the VDC. We do that here, but only associate the new window with the
     * VDC canvas.
     */
    if (dual_windows && !hide_vdc) {
        video_canvas_t* vdc_canvas = sdl_canvaslist[VIDEO_CANVAS_IDX_VDC];

        sdl_window = sdl_window_create(VIDEO_CANVAS_IDX_VDC);
        if (!sdl_window) {
            fprintf(stderr, "error: unable to create canvas container\n");
            archdep_vice_exit(-1);
        }

        vdc_canvas->sdl_window = sdl_window;

        /* Explicitly raise the VIC-II window in dual head mode -- creating the
         * windows in reverse order still results in the VDC window being on top
         * because SDL does not raise windows on creation.
         */
        if (!minimized) {
            video_canvas_t* vic_canvas = sdl_canvaslist[VIDEO_CANVAS_IDX_VICII];
            sdl_window = vic_canvas->sdl_window;
            SDL_RaiseWindow(sdl_window->window);
        }
    }

    mousedrv_mouse_changed();
}

static int last_mouse_x = -1;
static int last_mouse_y = -1;

int sdl_ui_get_mouse_state(int *px, int *py, unsigned int *pbuttons)
{
    SDL_Window* window = sdl_active_canvas->sdl_window->window;
    SDL_Renderer* renderer = sdl_active_canvas->sdl_window->renderer;
    int x, y, w, h;
    Uint32 buttons;
    double ratio;

    if (!window || !renderer || !sdl_active_canvas) {
        /* Not initialized yet */
        return 0;
    }

    if (!(SDL_GetWindowFlags(window) & SDL_WINDOW_MOUSE_FOCUS)) {
        /* We don't have mouse focus */
        return 0;
    }

    buttons = SDL_GetMouseState(&x, &y);
    SDL_RenderGetLogicalSize(renderer, &w, &h);
    x = last_mouse_x;
    y = last_mouse_y;
    ratio = (double) w / (double)sdl_active_canvas->draw_buffer->width;
    if (x < 0 || x > w || y < 0 || y > h) {
        return 0;
    }
    if (px) {
        *px = x / (ratio * sdl_active_canvas->videoconfig->scalex);
    }
    if (py) {
        *py = y / sdl_active_canvas->videoconfig->scaley;
    }
    if (pbuttons) {
        *pbuttons = buttons;
    }
    return 1;
}

void sdl_ui_consume_mouse_event(SDL_Event *event)
{
    SDL_Event post_event;
    
    if (event && event->type == SDL_MOUSEMOTION) {
        last_mouse_x = event->motion.x;
        last_mouse_y = event->motion.y;
    }
    
    post_event.type = sdl_event_mouse_move_processed;
    SDL_PushEvent(&post_event);
}

void sdl_ui_set_window_title(char *title)
{
    if (sdl_active_canvas->sdl_window) {
        SDL_SetWindowTitle(sdl_active_canvas->sdl_window->window, title);
    }
}
