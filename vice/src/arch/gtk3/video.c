/*
 * video.c - Native GTK3 UI video stuff.
 *
 * Written by
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
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

#include "not_implemented.h"

#include "log.h"
#include "machine.h"
#include "raster.h"
#include "resources.h"
#include "videoarch.h"

#ifdef HAVE_OPENGL_SYNC
# include "openGL_sync.h"
#endif


/** \brief  Log for Gtk3-native video messages
 */
static log_t    gtk3video_log = LOG_ERR;


/** \brief  Keep aspect ratio when resizing */
static int keepaspect;

/** \brief  Use true aspect ratio */
static int trueaspect;

/** \brief  Display depth in bits (8, 15, 16, 24, 32) */
static int display_depth;


/** \brief  Set KeepAspectRatio resource (bool)
 *
 * \param[in]   val     new value
 * \param[in]   param   extra parameter (unused)
 *
 * \return 0
 */
static int set_keepaspect(int val, void *param)
{
    keepaspect = val ? 1 : 0;
    /* ui_trigger_resize(); */
    return 0;
}


/** \brief  Set TrueAspectRatio resource (bool)
 *
 * \param[in]   val     new value
 * \param[in]   param   extra parameter (unused)
 *
 * \return 0
 */
static int set_trueaspect(int val, void *param)
{
    trueaspect = val ? 1 : 0;
    /* ui_trigger_resize(); */
    return 0;
}


static int set_display_depth(int val, void *param)
{
    if (val != 0 && val != 8 && val != 15 && val != 16 && val != 24 && val != 32) {
        return -1;
    }
    display_depth = val;
    return 0;
}


static const cmdline_option_t cmdline_options[] = {
    { "-trueaspect", SET_RESOURCE, 0,
      NULL, NULL, "TrueAspectRatio", (resource_value_t)1,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      IDCLS_UNUSED, IDCLS_UNUSED,
      NULL, N_("Enable true aspect ratio") },
    { "+trueaspect", SET_RESOURCE, 0,
      NULL, NULL, "TrueAspectRatio", (resource_value_t)0,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      IDCLS_UNUSED, IDCLS_UNUSED,
      NULL, N_("Disable true aspect ratio") },
    { "-keepaspect", SET_RESOURCE, 0,
      NULL, NULL, "KeepAspectRatio", (resource_value_t)1,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      IDCLS_UNUSED, IDCLS_UNUSED,
      NULL, N_("Keep aspect ratio when scaling") },
    { "+keepaspect", SET_RESOURCE, 0,
      NULL, NULL, "KeepAspectRatio", (resource_value_t)0,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      IDCLS_UNUSED, IDCLS_UNUSED,
      NULL, N_("Do not keep aspect ratio when scaling (freescale)") },
    { "-displaydepth", CALL_FUNCTION, 1,
        set_display_depth, NULL, "DisplayDepth", NULL,
        IDCLS_UNUSED, IDCLS_UNSED,
        NULL, NULL },
    CMDLINE_LIST_END
};

/** \brief  Integer/boolean resources related to video output
 */
static const resource_int_t resources_int[] = {
    { "KeepAspectRatio", 1, RES_EVENT_NO, NULL,
      &keepaspect, set_keepaspect, NULL },
    { "TrueAspectRatio", 1, RES_EVENT_NO, NULL,
      &trueaspect, set_trueaspect, NULL },
    { "DisplayDepth", 0, RES_EVENT_NO, NULL,
      &display_depth, set_display_depth, NULL },
    RESOURCE_INT_LIST_END
};





void video_arch_canvas_init(struct video_canvas_s *canvas)
{
    /* copy/paste from gnomevideo.c */
    canvas->video_draw_buffer_callback = NULL;
#ifdef HAVE_FULLSCREEN
    if (machine_class != VICE_MACHINE_VSID) {
        canvas->fullscreenconfig = lib_calloc(1, sizeof(fullscreenconfig_t));
        fullscreen_init_alloc_hooks(canvas);
    }
#endif
}


int video_arch_cmdline_options_init(void)
{
    NOT_IMPLEMENTED();
    return 0;
}


/** \brief  Initialize video-related resources
 *
 * \return  0 on success, < on failure
 */
int video_arch_resources_init(void)
{
#ifdef HAVE_OPENGL_SYNC
    openGL_register_resources();
#endif
    if (machine_class != VICE_MACHINE_VSID) {
        return resources_register_int(resources_int);
    }
    return 0;
}


void video_arch_resources_shutdown(void)
{
    NOT_IMPLEMENTED();
}


char video_canvas_can_resize(video_canvas_t *canvas)
{
    NOT_IMPLEMENTED();
    return 0;
}


video_canvas_t *video_canvas_create(video_canvas_t *canvas,
                                    unsigned int *width, unsigned int *height,
                                    int mapped)
{
    NOT_IMPLEMENTED();
    return NULL;
}


void video_canvas_destroy(struct video_canvas_s *canvas)
{
    NOT_IMPLEMENTED();
}


void video_canvas_refresh(struct video_canvas_s *canvas,
                          unsigned int xs, unsigned int ys,
                          unsigned int xi, unsigned int yi,
                          unsigned int w, unsigned int h)
{
    NOT_IMPLEMENTED();
}


void video_canvas_resize(struct video_canvas_s *canvas, char resize_canvas)
{
    NOT_IMPLEMENTED();
}


int video_canvas_set_palette(struct video_canvas_s *canvas,
                             struct palette_s *palette)
{
    NOT_IMPLEMENTED();
    return 0;
}


int video_init(void)
{
    if (gtk3video_log == LOG_ERR) {
        gtk3video_log = log_open("Gtk3Video");
    }
    return 0;
}


void video_shutdown(void)
{
    NOT_IMPLEMENTED();
}

