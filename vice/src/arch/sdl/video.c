/*
 * video.c - SDL video
 *
 * Written by
 *  Hannu Nuotio <hannu.nuotio@tut.fi>
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

#include "vice.h"

#include <stdio.h>
#include <SDL/SDL.h>

#ifdef HAVE_HWSCALE
#include <SDL/SDL_opengl.h>
#endif

#include "cmdline.h"
#include "fullscreen.h"
#include "fullscreenarch.h"
#include "lib.h"
#include "lightpendrv.h"
#include "log.h"
#include "palette.h"
#include "raster.h"
#include "resources.h"
#include "translate.h"
#include "uimenu.h"
#include "uistatusbar.h"
#include "util.h"
#include "videoarch.h"
#include "vkbd.h"
#include "vsync.h"

static log_t sdlvideo_log = LOG_ERR;

static int sdl_bitdepth;

static int sdl_limit_mode;

/* Custom w/h, used for fullscreen and limiting*/
static int sdl_custom_width;
static int sdl_custom_height;

int sdl_active_canvas_num = 0;
static int sdl_num_screens = 0;
static video_canvas_t *sdl_canvaslist[MAX_CANVAS_NUM];
video_canvas_t *sdl_active_canvas = NULL;

int sdl_forced_resize = 0;

#ifdef HAVE_HWSCALE
static int sdl_gl_mode;
static GLint screen_texture;

static int sdl_gl_aspect_mode;
static char *aspect_ratio_s = NULL;
static double aspect_ratio;
#endif

/* ------------------------------------------------------------------------- */
/* Video-related resources.  */

static int set_sdl_bitdepth(int d, void *param)
{
    if ((d == 0) || (d == 8) || (d == 15) || (d == 16) || (d == 24) || (d == 32)) {
        if (sdl_bitdepth == d) {
            return 0;
        }
        sdl_bitdepth = d;
#ifdef HAVE_HWSCALE
        if (!((d == 0) || (d == 24) || (d == 32))) {
            resources_set_int("HwScalePossible", 0);
        }
#endif
        /* update */
        return 0;
    }
    return -1;
}

static int set_sdl_limit_mode(int v, void *param)
{
    if ((v < 0) || (v > 2)) {
        return -1;
    }

    sdl_limit_mode = v;
    return 0;
}

static int set_sdl_custom_width(int w, void *param)
{
    if (w < 0) {
        return -1;
    }

    sdl_custom_width = w;
    return 0;
}

static int set_sdl_custom_height(int h, void *param)
{
    if (h < 0) {
        return -1;
    }

    sdl_custom_height = h;
    return 0;
}

#ifdef HAVE_HWSCALE
static int set_sdl_gl_aspect_mode(int v, void *param)
{
    int old_v = sdl_gl_aspect_mode;

    if ((v < 0) || (v > 1)) {
        return -1;
    }

    sdl_gl_aspect_mode = v;

    if (old_v != v) {
        if (sdl_active_canvas && sdl_active_canvas->videoconfig->hwscale) {
            sdl_video_resize(sdl_active_canvas->actual_width, sdl_active_canvas->actual_height);
        }
    }

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

    if (old_aspect != aspect_ratio) {
        if (sdl_active_canvas && sdl_active_canvas->videoconfig->hwscale) {
            sdl_video_resize(sdl_active_canvas->actual_width, sdl_active_canvas->actual_height);
        }
    }

    return 0;
}
#endif

static const resource_string_t resources_string[] = {
#ifdef HAVE_HWSCALE
    { "AspectRatio", "1.0", RES_EVENT_NO, NULL,
      &aspect_ratio_s, set_aspect_ratio, NULL },
#endif
    { NULL }
};

static const resource_int_t resources_int[] = {
#ifdef WATCOM_COMPILE
    { "SDLBitdepth", 32, RES_EVENT_NO, NULL,
      &sdl_bitdepth, set_sdl_bitdepth, NULL },
#else
    { "SDLBitdepth", 0, RES_EVENT_NO, NULL,
      &sdl_bitdepth, set_sdl_bitdepth, NULL },
#endif
    { "SDLLimitMode", SDL_LIMIT_MODE_OFF, RES_EVENT_NO, NULL,
      &sdl_limit_mode, set_sdl_limit_mode, NULL },
    { "SDLCustomWidth", 800, RES_EVENT_NO, NULL,
      &sdl_custom_width, set_sdl_custom_width, NULL },
    { "SDLCustomHeight", 600, RES_EVENT_NO, NULL,
      &sdl_custom_height, set_sdl_custom_height, NULL },
#ifdef HAVE_HWSCALE
    { "SDLGLAspectMode", 0, RES_EVENT_NO, NULL,
      &sdl_gl_aspect_mode, set_sdl_gl_aspect_mode, NULL },
#endif
    { NULL }
};

int video_arch_resources_init(void)
{
#ifdef SDL_DEBUG
fprintf(stderr,"%s\n",__func__);
#endif
    if (resources_register_string(resources_string) < 0)
        return -1;

    return resources_register_int(resources_int);
}

void video_arch_resources_shutdown(void)
{
#ifdef SDL_DEBUG
fprintf(stderr,"%s\n",__func__);
#endif
#ifdef HAVE_HWSCALE
    lib_free(aspect_ratio_s);
#endif
}

/* ------------------------------------------------------------------------- */
/* Video-related command-line options.  */

static const cmdline_option_t cmdline_options[] = {
    { "-sdlbitdepth", SET_RESOURCE, 1, NULL, NULL, "SDLBitdepth", NULL,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING, IDCLS_UNUSED, IDCLS_UNUSED,
      "<bpp>", "Set bitdepth (0 = current, 8, 15, 16, 24, 32)" },
    { "-sdllimitmode", SET_RESOURCE, 1, NULL, NULL, "SDLLimitMode", NULL,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING, IDCLS_UNUSED, IDCLS_UNUSED,
      "<mode>", "Set resolution limiting mode (0 = off, 1 = max, 2 = fixed)" },
    { "-sdlcustomw", SET_RESOURCE, 1, NULL, NULL, "SDLCustomWidth", NULL,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING, IDCLS_UNUSED, IDCLS_UNUSED,
      "<width>", "Set custom resolution width" },
    { "-sdlcustomh", SET_RESOURCE, 1, NULL, NULL, "SDLCustomHeight", NULL,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING, IDCLS_UNUSED, IDCLS_UNUSED,
      "<height>", "Set custom resolution height" },
#ifdef HAVE_HWSCALE
    { "-sdlaspectmode", SET_RESOURCE, 1, NULL, NULL, "SDLGLAspectMode", NULL,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING, IDCLS_UNUSED, IDCLS_UNUSED,
      "<mode>", "Set aspect ratio mode (0 = any, 1 = fixed)" },
    { "-aspect", SET_RESOURCE, 1, NULL, NULL, "AspectRatio", NULL,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING, IDCLS_UNUSED, IDCLS_UNUSED,
      "<aspect ratio>", "Set aspect ratio (0.5 - 2.0)" },
#endif
    { NULL }
};

int video_init_cmdline_options(void)
{
#ifdef SDL_DEBUG
fprintf(stderr,"%s\n",__func__);
#endif
    return cmdline_register_options(cmdline_options);
}

/* ------------------------------------------------------------------------- */

int video_init(void)
{
    sdlvideo_log = log_open("SDLVideo");
    return 0;
}

void video_shutdown(void)
{
#ifdef SDL_DEBUG
fprintf(stderr,"%s\n",__func__);
#endif
    sdl_active_canvas = NULL;
}

/* ------------------------------------------------------------------------- */
/* static helper functions */

static int sdl_video_canvas_limit(video_canvas_t *canvas, unsigned int w, unsigned int h, int mode)
{
    int limiting = 0;
    unsigned int new_w, new_h;
    unsigned int limit_w = (unsigned int)sdl_custom_width;
    unsigned int limit_h = (unsigned int)sdl_custom_height;

#ifdef SDL_DEBUG
fprintf(stderr,"%s\n",__func__);
#endif
    switch (mode & 3) {
        case SDL_LIMIT_MODE_MAX:
            if ((w > limit_w) || (h > limit_h)) {
                limiting = 1;
                new_w = MIN(w, limit_w);
                new_h = MIN(h, limit_h);
            }
            break;
        case SDL_LIMIT_MODE_FIXED:
            if ((w != limit_w) || (h != limit_h)) {
                limiting = 1;
                new_w = limit_w;
                new_h = limit_h;
            }
            break;
        case SDL_LIMIT_MODE_OFF:
        default:
            break;
    }

    if (limiting) {
        log_warning(sdlvideo_log, "Resolution %ux%u doesn't follow limit %ux%u, resizing...", w, h, limit_w, limit_h);
        sdl_forced_resize = 1;
        video_canvas_redraw_size(canvas, new_w, new_h);
        sdl_forced_resize = 0;
    }

    return limiting;
}

#ifdef HAVE_HWSCALE
static void sdl_gl_set_viewport(unsigned int src_w, unsigned int src_h, unsigned int dest_w, unsigned int dest_h)
{
    int dest_x = 0, dest_y = 0;

    if (sdl_gl_aspect_mode != 0) {
        /* Keep aspect ratio of src image. */
        if (dest_w*src_h < src_w*aspect_ratio*dest_h) {
            dest_y = dest_h;
            dest_h = (unsigned int)(dest_w*src_h/(src_w*aspect_ratio));
            dest_y = (dest_y - dest_h)/2;
        } else {
            dest_x = dest_w;
            dest_w = (unsigned int)(dest_h*src_w*aspect_ratio/src_h);
            dest_x = (dest_x - dest_w)/2;
        }
    }

    /* Update lightpen adjustment parameters */
    sdl_lightpen_adjust.offset_x = dest_x;
    sdl_lightpen_adjust.offset_y = dest_y;

    sdl_lightpen_adjust.max_x = dest_w;
    sdl_lightpen_adjust.max_y = dest_h;

    sdl_lightpen_adjust.scale_x = (double)(sdl_active_canvas->real_width) / (double)(dest_w);
    sdl_lightpen_adjust.scale_y = (double)(sdl_active_canvas->real_height) / (double)(dest_h);

    glViewport(dest_x, dest_y, dest_w, dest_h);
}
#endif

/* ------------------------------------------------------------------------- */
/* Main API */

video_canvas_t *video_canvas_create(video_canvas_t *canvas,
                                    unsigned int *width, unsigned int *height,
                                    int mapped)
{
    SDL_Surface *new_screen;
    unsigned int new_width, new_height;
    unsigned int actual_width, actual_height;
    unsigned int temp_width, temp_height;
    int flags;
    int fullscreen = 0;
    int limit = sdl_limit_mode;
    int hwscale = 0;
    int lightpen_updated = 0;
#ifdef HAVE_HWSCALE
    int rbits, gbits, bbits;
    const Uint32
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
        rmask = 0xff000000, gmask = 0x00ff0000, bmask = 0x0000ff00, amask = 0x000000ff;
#else
        rmask = 0x000000ff, gmask = 0x0000ff00, bmask = 0x00ff0000, amask = 0xff000000;
#endif
#endif

#ifdef SDL_DEBUG
fprintf(stderr,"%s: %i,%i (%i)\n",__func__,*width,*height,canvas->index);
#endif

    flags = SDL_SWSURFACE | SDL_RESIZABLE;

    /* initialize real size and dsize state on first call */
    if (canvas->real_width == 0) {
        canvas->real_width = *width;
        canvas->real_height = *height;
        canvas->dsizex = canvas->videoconfig->doublesizex;
        canvas->dsizey = canvas->videoconfig->doublesizey;
#ifdef SDL_DEBUG
fprintf(stderr,"%s: init real size to %i,%i (%i)\n",__func__,*width,*height,canvas->index);
#endif
    }

    new_width = *width;
    new_height = *height;

    if (canvas->videoconfig->doublesizex) {
        new_width *= 2;
    }

    if (canvas->videoconfig->doublesizey) {
        new_height *= 2;
    }

    if ((canvas == sdl_active_canvas) && (canvas->fullscreenconfig->enable)) {
        fullscreen = 1;
    }

#ifdef HAVE_HWSCALE
    if ((canvas == sdl_active_canvas) && (canvas->videoconfig->hwscale)) {
        hwscale = 1;
        limit = SDL_LIMIT_MODE_OFF;
    }
#endif

    /* set real size unless the resize was forced (f.ex by resizing the window) */
    if (!sdl_forced_resize) {
        canvas->real_width = *width;
        canvas->real_height = *height;
#ifdef SDL_DEBUG
fprintf(stderr,"%s: setting real size to %i,%i (%i)\n",__func__,*width,*height,canvas->index);
#endif
    }

    if (fullscreen) {
        flags = SDL_FULLSCREEN | SDL_HWSURFACE;

        if (canvas->fullscreenconfig->mode == FULLSCREEN_MODE_CUSTOM) {
            if (hwscale) {
                temp_width = canvas->real_width;
                temp_height = canvas->real_height;

                if (canvas->videoconfig->doublesizex) {
                    temp_width *= 2;
                }

                if (canvas->videoconfig->doublesizey) {
                    temp_height *= 2;
                }

                if ((new_width != temp_width) || (new_height != temp_height)) {
                    new_width = temp_width;
                    new_height = temp_height;
                    limit = SDL_LIMIT_MODE_FIXED;
                }
            } else {
                limit = SDL_LIMIT_MODE_FIXED;
            }
        }
    }

    if ((canvas == sdl_active_canvas) && sdl_video_canvas_limit(canvas, new_width, new_height, limit)) {
        return canvas;
    }

    sdl_forced_resize = 0;

#ifdef HAVE_HWSCALE
    if (hwscale) {
        flags = SDL_OPENGL | SDL_RESIZABLE;

        if (fullscreen) {
            flags |= SDL_FULLSCREEN;
        }

        switch (sdl_bitdepth) {
            case 0:
                log_warning(sdlvideo_log, "bitdepth not set for OpenGL, trying 32...");
                sdl_bitdepth = 32;
                /* fall through */
            case 32:
                rbits = gbits = bbits = 8;
                sdl_gl_mode = GL_RGBA;
                break;
            case 24:
                rbits = gbits = bbits = 8;
                sdl_gl_mode = GL_RGB;
                break;
            default:
                log_error(sdlvideo_log, "%i bpp not supported in OpenGL.", sdl_bitdepth);
                resources_set_int("HwScalePossible", 0);
                hwscale = 0;
                canvas->videoconfig->hwscale = 0;
                flags = SDL_SWSURFACE;
                break;
        }

        SDL_GL_SetAttribute(SDL_GL_RED_SIZE, rbits);
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, gbits);
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, bbits);
    }
#endif

    actual_width = new_width;
    actual_height = new_height;

    if (canvas == sdl_active_canvas) {
#ifndef HAVE_HWSCALE
        new_screen = SDL_SetVideoMode(new_width, new_height, sdl_bitdepth, flags);
#else
        if (hwscale) {
            if (fullscreen && (canvas->fullscreenconfig->mode == FULLSCREEN_MODE_CUSTOM)) {
                actual_width = sdl_custom_width;
                actual_height = sdl_custom_height;
            }

            new_screen = SDL_SetVideoMode(actual_width, actual_height, sdl_bitdepth, flags);

            /* free the old rendering surface when staying in hwscale mode */
            if ((canvas->hwscale_screen)&&(canvas->screen)) {
                SDL_FreeSurface(canvas->screen);
            }

            canvas->hwscale_screen = new_screen;
            new_screen = SDL_CreateRGBSurface(SDL_SWSURFACE, new_width, new_height, sdl_bitdepth, rmask, gmask, bmask, amask);
            sdl_gl_set_viewport(new_width, new_height, actual_width, actual_height);
            lightpen_updated = 1;
        } else {
            new_screen = SDL_SetVideoMode(new_width, new_height, sdl_bitdepth, flags);

            /* free the old rendering surface when leaving hwscale mode */
            if ((canvas->hwscale_screen)&&(canvas->screen)) {
                SDL_FreeSurface(canvas->screen);
                SDL_FreeSurface(canvas->hwscale_screen);
                canvas->hwscale_screen = NULL;
            }
        }
#endif
    } else {
        if (canvas->screen) {
            SDL_FreeSurface(canvas->screen);
        }
        new_screen = SDL_CreateRGBSurface(SDL_SWSURFACE, new_width, new_height, sdl_bitdepth, 0, 0, 0, 0);
    }

    if (!new_screen) {
        log_error(sdlvideo_log, "SDL_SetVideoMode failed!");
        return NULL;
    }
    sdl_bitdepth = new_screen->format->BitsPerPixel;

    canvas->depth = sdl_bitdepth;
    canvas->width = new_width;
    canvas->height = new_height;
    canvas->screen = new_screen;
    canvas->actual_width = actual_width;
    canvas->actual_height = actual_height;
    canvas->dsizex = canvas->videoconfig->doublesizex;
    canvas->dsizey = canvas->videoconfig->doublesizey;

    log_message(sdlvideo_log, "%ix%i %ibpp %s%s", actual_width, actual_height, sdl_bitdepth, hwscale?"OpenGL ":"", (canvas->fullscreenconfig->enable)?"(fullscreen)":"");
#ifdef SDL_DEBUG
    log_message(sdlvideo_log, "Canvas %ix%i, real %ix%i", new_width, new_height, canvas->real_width, canvas->real_height);
#endif

    /* Update lightpen adjustment parameters */
    if (canvas == sdl_active_canvas && !lightpen_updated) {
        int scaled_width = canvas->real_width * ((canvas->dsizex) ? 2 : 1);
        int scaled_height = canvas->real_height * ((canvas->dsizey) ? 2 : 1);

        sdl_lightpen_adjust.offset_x = (new_width - scaled_width) / 2;
        sdl_lightpen_adjust.offset_y = (new_height - scaled_height) / 2;

        sdl_lightpen_adjust.max_x = scaled_width;
        sdl_lightpen_adjust.max_y = scaled_height;

        sdl_lightpen_adjust.scale_x = (canvas->dsizex) ? 0.5f : 1.0f;
        sdl_lightpen_adjust.scale_y = (canvas->dsizey) ? 0.5f : 1.0f;

    }

    video_canvas_set_palette(canvas, canvas->palette);

    return canvas;
}

void video_canvas_refresh(struct video_canvas_s *canvas,
                          unsigned int xs, unsigned int ys,
                          unsigned int xi, unsigned int yi,
                          unsigned int w, unsigned int h)
{
    if (sdl_vkbd_state & SDL_VKBD_ACTIVE) {
        sdl_vkbd_draw();
    }

    if (uistatusbar_state & UISTATUSBAR_ACTIVE) {
        uistatusbar_draw();
    }

    if (canvas->videoconfig->doublesizex) {
        xi *= 2;
        w *= 2;
    }

    if (canvas->videoconfig->doublesizey) {
        yi *= 2;
        h *= 2;
    }

    w = MIN(w, canvas->width);
    h = MIN(h, canvas->height);

    /* FIXME attempt to draw outside canvas */
    if ((xi + w > canvas->width)||(yi + h > canvas->height)) {
        return;
    }

    if (SDL_MUSTLOCK(canvas->screen)) {
        if (SDL_LockSurface(canvas->screen) < 0) {
            return;
        }
    }

    video_canvas_render(canvas, (BYTE *)canvas->screen->pixels,
                        w, h, xs, ys, xi, yi,
                        canvas->screen->pitch,
                        canvas->screen->format->BitsPerPixel);

    if (SDL_MUSTLOCK(canvas->screen)) {
        SDL_UnlockSurface(canvas->screen);
    }

#ifdef HAVE_HWSCALE
    if (canvas->videoconfig->hwscale) {
        if (!(canvas->hwscale_screen)) {
#ifdef SDL_DEBUG
fprintf(stderr,"%s: hwscale refresh without hwscale screen, ignoring\n",__func__);
#endif
            return;
        }

/* XXX make use of glXBindTexImageEXT aka texture from pixmap extension */

        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);

/* GL_TEXTURE_RECTANGLE is standardised as _EXT in OpenGL 1.4. Here's some
 * aliases in the meantime. */
#ifndef GL_TEXTURE_RECTANGLE_EXT
    #if defined(GL_TEXTURE_RECTANGLE_NV)
        #define GL_TEXTURE_RECTANGLE_EXT GL_TEXTURE_RECTANGLE_NV
    #elif defined(GL_TEXTURE_RECTANGLE_ARB)
        #define GL_TEXTURE_RECTANGLE_EXT GL_TEXTURE_RECTANGLE_ARB
    #else
        #error "Your headers do not supply GL_TEXTURE_RECTANGLE. Disable HWSCALE and try again."
    #endif
#endif

        glEnable(GL_TEXTURE_RECTANGLE_EXT);
        glBindTexture(GL_TEXTURE_RECTANGLE_EXT, screen_texture);
        glTexParameteri(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D (GL_TEXTURE_RECTANGLE_EXT, 0, sdl_gl_mode,
            canvas->width, canvas->height,
            0, sdl_gl_mode, GL_UNSIGNED_BYTE, canvas->screen->pixels);

        glBegin(GL_QUADS);

        /* Lower Right Of Texture */
        glTexCoord2f(0.0f, 0.0f);
        glVertex2f(-1.0f, 1.0f);

        /* Upper Right Of Texture */
        glTexCoord2f(0.0f, (float)(canvas->height));
        glVertex2f(-1.0f, -1.0f);

        /* Upper Left Of Texture */
        glTexCoord2f((float)(canvas->width), (float)(canvas->height));
        glVertex2f(1.0f, -1.0f);

        /* Lower Left Of Texture */
        glTexCoord2f((float)(canvas->width), 0.0f);
        glVertex2f(1.0f, 1.0f);

        glEnd();

        SDL_GL_SwapBuffers();
    } else
#endif
    SDL_UpdateRect(canvas->screen, xi, yi, w, h);
}

int video_canvas_set_palette(struct video_canvas_s *canvas,
                             struct palette_s *palette)
{
    unsigned int i, col;
    SDL_PixelFormat *fmt;
    SDL_Color colors[256];

    canvas->palette = palette;

    fmt = canvas->screen->format;

    for (i = 0; i < palette->num_entries; i++) {
        if (canvas->depth == 8) {
            colors[i].r = palette->entries[i].red;
            colors[i].b = palette->entries[i].blue;
            colors[i].g = palette->entries[i].green;
            col = i;
        } else {
            col = SDL_MapRGB(fmt, palette->entries[i].red, palette->entries[i].green, palette->entries[i].blue);
        }
        video_render_setphysicalcolor(canvas->videoconfig, i, col, canvas->depth);
    }

    if (canvas->depth == 8) {
        SDL_SetColors(canvas->screen, colors, 0, palette->num_entries);
    } else {
        for (i = 0; i < 256; i++) {
            video_render_setrawrgb(i, SDL_MapRGB(fmt, (Uint8)i, 0, 0), SDL_MapRGB(fmt, 0, (Uint8)i, 0), SDL_MapRGB(fmt, 0, 0, (Uint8)i));
        }
        video_render_initraw();
    }

    return 0;
}

static inline int check_resize(struct video_canvas_s *canvas)
{
#ifdef HAVE_HWSCALE
    /* Resize when enabling hwscale */
    if ((canvas->videoconfig->hwscale) && !(canvas->hwscale_screen)) {
#ifdef SDL_DEBUG
fprintf(stderr,"%s: hwscale resize\n",__func__);
#endif
        return 1;
    }
#endif
    /* Resize when toggling double size */
    if ((canvas->videoconfig->doublesizex != (int)canvas->dsizex) || (canvas->videoconfig->doublesizey != (int)canvas->dsizey)) {
#ifdef SDL_DEBUG
fprintf(stderr,"%s: dsize resize (x:%i->%i, y:%i->%i)\n",__func__,canvas->videoconfig->doublesizex,canvas->dsizex,canvas->videoconfig->doublesizey,canvas->dsizey);
#endif
        return 1;
    }
    return 0;
}

void video_canvas_resize(struct video_canvas_s *canvas,
                         unsigned int width, unsigned int height)
{
#ifdef SDL_DEBUG
fprintf(stderr,"%s: %ix%i (%i)\n",__func__,width,height,canvas->index);
#endif
    /* Check if canvas needs to be resized to real size first */
    if (check_resize(canvas)) {
        sdl_video_resize(0, 0);
        width = canvas->real_width;
        height = canvas->real_height;
    }
    video_canvas_create(canvas, &width, &height, 0);
}

void sdl_video_resize(unsigned int w, unsigned int h)
{
#ifdef SDL_DEBUG
fprintf(stderr,"%s: %ix%i\n",__func__,w,h);
#endif
    vsync_suspend_speed_eval();

    if ((w == 0) && (h == 0)) {
        w = sdl_active_canvas->real_width;
        h = sdl_active_canvas->real_height;

        if (sdl_active_canvas->videoconfig->doublesizex) {
            w *= 2;
        }

        if (sdl_active_canvas->videoconfig->doublesizey) {
            h *= 2;
        }
    }

    sdl_forced_resize = 1;
#ifdef HAVE_HWSCALE
    if (sdl_active_canvas->videoconfig->hwscale && sdl_active_canvas->hwscale_screen) {
        int flags = SDL_OPENGL | SDL_RESIZABLE;

        if (sdl_active_canvas->fullscreenconfig->enable) {
            flags |= SDL_FULLSCREEN;
        }

        sdl_active_canvas->hwscale_screen = SDL_SetVideoMode((int)w, (int)h, sdl_bitdepth, flags);
        sdl_gl_set_viewport(sdl_active_canvas->width, sdl_active_canvas->height, w, h);
    } else
#endif
    {
        video_canvas_redraw_size(sdl_active_canvas, w, h);
    }
    sdl_forced_resize = 0;
}

void sdl_video_canvas_switch(int index)
{
    int old_active;
    unsigned int w, h;
    struct video_canvas_s *canvas;

#ifdef SDL_DEBUG
fprintf(stderr,"%s: %i->%i\n",__func__,sdl_active_canvas_num, index);
#endif

    if (sdl_active_canvas_num == index) {
        return;
    }

    if (index >= sdl_num_screens) {
        return;
    }


    if (sdl_canvaslist[index]->screen != NULL) {
        SDL_FreeSurface(sdl_canvaslist[index]->screen);
        sdl_canvaslist[index]->screen = NULL;
    }

    old_active = sdl_active_canvas_num;
    sdl_active_canvas_num = index;

    canvas = sdl_canvaslist[sdl_active_canvas_num];
    sdl_active_canvas = canvas;

    w = canvas->width;
    h = canvas->height;

    if (canvas->videoconfig->doublesizex) {
        w /= 2;
    }

    if (canvas->videoconfig->doublesizey) {
        h /= 2;
    }

    sdl_forced_resize = 1;
    video_canvas_create(canvas, &w, &h, 0);

    canvas = sdl_canvaslist[old_active];

    if(!canvas->initialized) {
        return;
    }

    w = canvas->width;
    h = canvas->height;

    if (canvas->videoconfig->doublesizex) {
        w /= 2;
    }

    if (canvas->videoconfig->doublesizey) {
        h /= 2;
    }

    sdl_forced_resize = 1;
    video_canvas_create(canvas, &w, &h, 0);
}

void video_arch_canvas_init(struct video_canvas_s *canvas)
{
#ifdef SDL_DEBUG
fprintf(stderr,"%s: (%08x, %i)\n",__func__,(unsigned int)canvas, sdl_num_screens);
#endif

    if (sdl_num_screens == MAX_CANVAS_NUM) {
        log_error(sdlvideo_log,"Too many canvases!");
        exit(-1);
    }

    canvas->video_draw_buffer_callback = NULL;

    canvas->fullscreenconfig
        = (fullscreenconfig_t *)lib_calloc(1, sizeof(fullscreenconfig_t));
    fullscreen_init_alloc_hooks(canvas);

    if (sdl_active_canvas_num == sdl_num_screens) {
        sdl_active_canvas = canvas;
    }

    canvas->index = sdl_num_screens;

    sdl_canvaslist[sdl_num_screens++] = canvas;

    canvas->screen = NULL;
#ifdef HAVE_HWSCALE
    canvas->hwscale_screen = NULL;
#endif
    canvas->real_width = 0;
    canvas->real_height = 0;
}

void video_canvas_destroy(struct video_canvas_s *canvas)
{
    int i;

#ifdef SDL_DEBUG
fprintf(stderr,"%s: (%08x, %i)\n",__func__,(unsigned int)canvas, canvas->index);
#endif

    for (i=0; i<sdl_num_screens; ++i) {
        if ((sdl_canvaslist[i] == canvas) && (i != sdl_active_canvas_num)) {
            SDL_FreeSurface(sdl_canvaslist[i]->screen);
            sdl_canvaslist[i]->screen = NULL;
        }
    }
}

void video_add_handlers(void)
{
#ifdef SDL_DEBUG
fprintf(stderr,"%s\n",__func__);
#endif
}
