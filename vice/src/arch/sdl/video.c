/*
 * video.c - SDL video
 *
 * Written by
 *  Hannu Nuotio <hannu.nuotio@tut.fi>
 *
 * Based on code by
 *  Ettore Perazzoli
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
#include "log.h"
#include "palette.h"
#include "raster.h"
#include "resources.h"
#include "translate.h"
#include "uimenu.h"
#include "uistatusbar.h"
#include "videoarch.h"
#include "vkbd.h"

static log_t sdlvideo_log = LOG_ERR;

static int sdl_bitdepth;

static int sdl_limit_mode;
static int sdl_limit_width;
static int sdl_limit_height;

int sdl_active_canvas_num = 0;
static int sdl_num_screens = 0;
static video_canvas_t *sdl_canvaslist[MAX_CANVAS_NUM];
video_canvas_t *sdl_active_canvas = NULL;
#ifdef HAVE_HWSCALE
int sdl_gl_mode;
GLint screen_texture;
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

static int set_sdl_limit_width(int w, void *param)
{
    if (w < 0) {
        return -1;
    }

    sdl_limit_width = w;
    return 0;
}

static int set_sdl_limit_height(int h, void *param)
{
    if (h < 0) {
        return -1;
    }

    sdl_limit_height = h;
    return 0;
}

static const resource_string_t resources_string[] = {
    { NULL }
};

static const resource_int_t resources_int[] = {
    { "SDLBitdepth", 0, RES_EVENT_NO, NULL,
      &sdl_bitdepth, set_sdl_bitdepth, NULL },
    { "SDLLimitMode", 0, RES_EVENT_NO, NULL,
      &sdl_limit_mode, set_sdl_limit_mode, NULL },
    { "SDLLimitWidth", 800, RES_EVENT_NO, NULL,
      &sdl_limit_width, set_sdl_limit_width, NULL },
    { "SDLLimitHeight", 600, RES_EVENT_NO, NULL,
      &sdl_limit_height, set_sdl_limit_height, NULL },
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
    { "-sdllimitw", SET_RESOURCE, 1, NULL, NULL, "SDLLimitWidth", NULL,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING, IDCLS_UNUSED, IDCLS_UNUSED,
      "<width>", "Set resolution limit width" },
    { "-sdllimith", SET_RESOURCE, 1, NULL, NULL, "SDLLimitHeight", NULL,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING, IDCLS_UNUSED, IDCLS_UNUSED,
      "<height>", "Set resolution limit height" },
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

video_canvas_t *video_canvas_create(video_canvas_t *canvas,
                                    unsigned int *width, unsigned int *height,
                                    int mapped)
{
    SDL_Surface *new_screen;
    unsigned int new_width, new_height;
    int flags;
    int hwscale = 0;
#ifdef HAVE_HWSCALE
    int rbits, gbits, bbits;
    const Uint32
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
        rmask = 0xff000000, gmask = 0x00ff0000, bmask = 0x0000ff00, amask = 0x000000ff;
#else
        rmask = 0x000000ff, gmask = 0x0000ff00, bmask = 0x00ff0000, amask = 0xff000000;
#endif
#endif

    flags = SDL_SWSURFACE;  /* TODO: | SDL_RESIZABLE */

    new_width = *width;
    new_height = *height;

    if (canvas->fullscreenconfig->enable) {
        flags = SDL_FULLSCREEN | SDL_HWSURFACE;
    }

    if (canvas->videoconfig->doublesizex) {
        new_width *= 2;
    }

    if (canvas->videoconfig->doublesizey) {
        new_height *= 2;
    }

    if ((canvas == sdl_active_canvas)
       && (((sdl_limit_mode == 1) && ((new_width > sdl_limit_width) || (new_height > sdl_limit_height)))
       || ((sdl_limit_mode == 2) && ((new_width != sdl_limit_width) || (new_height != sdl_limit_height))))) {
        log_warning(sdlvideo_log, "Resolution %ix%i doesn't follow limit %ix%i, resizing...", new_width, new_height, sdl_limit_width, sdl_limit_height);
        video_canvas_redraw_size(canvas, sdl_limit_width, sdl_limit_height);
        return canvas;
    }

#ifdef HAVE_HWSCALE
    if ((canvas == sdl_active_canvas) && (canvas->videoconfig->hwscale)) {
        flags = SDL_OPENGL | SDL_RESIZABLE;
        hwscale = 1;

        if (canvas->fullscreenconfig->enable) {
            flags |= SDL_FULLSCREEN;
        }

        switch(sdl_bitdepth) {
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

/*fprintf(stderr,"%s: %ix%i,%i,%i (%08x)\n",__func__,new_width,new_height,hwscale,mapped,(unsigned int)canvas);*/
    if (canvas == sdl_active_canvas) {
        new_screen = SDL_SetVideoMode(new_width, new_height, sdl_bitdepth, flags);
#ifdef HAVE_HWSCALE
        if (hwscale) {
            /* free the old rendering surface when staying in hwscale mode */
            if ((canvas->hwscale_screen)&&(canvas->screen)) {
                SDL_FreeSurface(canvas->screen);
            }
            canvas->hwscale_screen = new_screen;
            new_screen = SDL_CreateRGBSurface(SDL_SWSURFACE, new_width, new_height, sdl_bitdepth, rmask, gmask, bmask, amask);
            glViewport(0, 0, new_width, new_height);
        } else {
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

    log_message(sdlvideo_log, "%ix%i %ibpp %s%s", new_width, new_height, sdl_bitdepth, hwscale?"OpenGL ":"", (canvas->fullscreenconfig->enable)?"(fullscreen)":"");

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
/* XXX make use of glXBindTexImageEXT aka texture from pixmap extension */

        glClear(GL_COLOR_BUFFER_BIT);
        glDisable (GL_DEPTH_TEST);

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

        glEnable (GL_TEXTURE_RECTANGLE_EXT);
        glBindTexture (GL_TEXTURE_RECTANGLE_EXT, screen_texture);
        glTexParameteri (GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri (GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D  (GL_TEXTURE_RECTANGLE_EXT, 0, sdl_gl_mode,
            canvas->width, canvas->height,
            0, sdl_gl_mode, GL_UNSIGNED_BYTE, canvas->screen->pixels);

        glBegin (GL_QUADS);

        /* Lower Right Of Texture */
        glTexCoord2f(0.0f, 0.0f);
        glVertex2f(-1.0f, 1.0f);

        /* Upper Right Of Texture */
        glTexCoord2f(0.0f, canvas->height);
        glVertex2f(-1.0f, -1.0f);

        /* Upper Left Of Texture */
        glTexCoord2f(canvas->width, canvas->height);
        glVertex2f(1.0f, -1.0f);

        /* Lower Left Of Texture */
        glTexCoord2f(canvas->width, 0.0f);
        glVertex2f(1.0f, 1.0f);

        glEnd ();

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

/*fprintf(stderr,"%s: %i (%08x)\n",__func__,canvas->videoconfig->hwscale,(unsigned int)canvas);*/

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
            video_render_setrawrgb(i, SDL_MapRGB(fmt, i, 0, 0), SDL_MapRGB(fmt, 0, i, 0), SDL_MapRGB(fmt, 0, 0, i));
        }
        video_render_initraw();
    }

    return 0;
}

void video_canvas_resize(struct video_canvas_s *canvas,
                         unsigned int width, unsigned int height)
{
#ifdef SDL_DEBUG
fprintf(stderr,"%s: %i,%i (%08x)\n",__func__,width,height,(unsigned int)canvas);
#endif

    video_canvas_create(canvas, &width, &height, 0);
}

void sdl_video_resize(int w, int h)
{
#ifdef SDL_DEBUG
fprintf(stderr,"%s: %i,%i\n",__func__,w,h);
#endif
#ifdef HAVE_HWSCALE
    if (sdl_active_canvas->videoconfig->hwscale) {
        sdl_active_canvas->hwscale_screen = SDL_SetVideoMode(w, h, sdl_bitdepth, SDL_OPENGL | SDL_RESIZABLE);
        glViewport(0, 0, w, h);

        if (sdl_menu_state) {
            sdl_ui_refresh();
        } else {
            raster_force_repaint(sdl_active_canvas->parent_raster);
        }
    } else
#endif
    {
        /* FIXME this crashes on some situations */
        /*video_canvas_redraw_size(sdl_active_canvas, w, h);*/
    }
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

    video_canvas_create(canvas, &w, &h, 0);
}

void video_arch_canvas_init(struct video_canvas_s *canvas)
{
#ifdef SDL_DEBUG
fprintf(stderr,"%s: (%08x)\n",__func__,(unsigned int)canvas);
#endif

    if (sdl_num_screens == MAX_CANVAS_NUM) {
        log_error(sdlvideo_log,"Too many canvases!");
        exit(-1);
    }

    canvas->video_draw_buffer_callback=NULL;

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
}

void video_canvas_destroy(struct video_canvas_s *canvas)
{
    int i;

#ifdef SDL_DEBUG
fprintf(stderr,"%s: (%08x)\n",__func__,(unsigned int)canvas);
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
