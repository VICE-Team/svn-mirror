/** \file video.c
 * \brief Native GTK3 UI video stuff
 *
 * \author Marco van den Heuvel <blackystardust68@yahoo.com>
 * \author Michael C. Martin <mcmartin@gmail.com>
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

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "debug_gtk3.h"

#include "archdep.h"
#include "cmdline.h"
#include "lib.h"
#include "log.h"
#include "machine.h"
#include "palette.h"
#include "raster.h"
#include "resources.h"
#include "ui.h"
#include "videoarch.h"

/** \brief Enable extra debugging info */
#define VICE_DEBUG_NATIVE_GTK3

/** \brief  Log for Gtk3-native video messages
 */
static log_t    gtk3video_log = LOG_ERR;

/** \brief  Keep aspect ratio when resizing */
static int keepaspect = 0;

/** \brief  Use true aspect ratio */
static int trueaspect = 0;

/** \brief  Prevent screen tearing */
static int vsync = 1;

/** \brief  Display filter (0: nearest 1: bilinear) */
static int display_filter = 1;

/** \brief  Set KeepAspectRatio resource (bool)
 *
 * The display will be updated to reflect any changes this makes.
 *
 * \param[in]   val     new value
 * \param[in]   param   extra parameter (unused)
 *
 * \return 0
 */
static int set_keepaspect(int val, void *param)
{
    keepaspect = val ? 1 : 0;
    ui_trigger_resize();
    return 0;
}


/** \brief  Set TrueAspectRatio resource (bool)
 *
 * The display will be updated to reflect any changes this makes.
 *
 * \param[in]   val     new value
 * \param[in]   param   extra parameter (unused)
 *
 * \return 0
 */
static int set_trueaspect(int val, void *param)
{
    trueaspect = val ? 1 : 0;
    ui_trigger_resize();
    return 0;
}


/** \brief  Set VSync resource (bool)
 *
 * The display will be updated to reflect any changes this makes.
 *
 * \param[in]   val     new value
 * \param[in]   param   extra parameter (unused)
 *
 * \return 0
 */
static int set_vsync(int val, void *param)
{
    vsync = val ? 1 : 0;
    return 0;
}

/** \brief Set the display filter for scaling.
 *  \param     val   new filter (0: nearest, 1: bilinear)
 *  \param[in] param extra parameter (unused).
 *  \return  0
 */
static int set_display_filter(int val, void *param)
{
    if (val < 0) {
        val = 0;
    }
    if (val > 2) {
        val = 2;
    }
    display_filter = val;
    return 0;
}

/** \brief  Command line options related to generic video output
 */
static const cmdline_option_t cmdline_options[] =
{
    { "-trueaspect", SET_RESOURCE, CMDLINE_ATTRIB_NONE,
      NULL, NULL, "TrueAspectRatio", (resource_value_t)1,
      NULL, "Enable true aspect ratio" },
    { "+trueaspect", SET_RESOURCE, CMDLINE_ATTRIB_NONE,
      NULL, NULL, "TrueAspectRatio", (resource_value_t)0,
      NULL, "Disable true aspect ratio" },
    { "-keepaspect", SET_RESOURCE, CMDLINE_ATTRIB_NONE,
      NULL, NULL, "KeepAspectRatio", (resource_value_t)1,
      NULL, "Keep aspect ratio when scaling" },
    { "+keepaspect", SET_RESOURCE, CMDLINE_ATTRIB_NONE,
      NULL, NULL, "KeepAspectRatio", (resource_value_t)0,
      NULL, "Do not keep aspect ratio when scaling (freescale)" },
    { "-vsync", SET_RESOURCE, CMDLINE_ATTRIB_NONE,
      NULL, NULL, "VSync", (resource_value_t)1,
      NULL, "Enable vsync to prevent screen tearing" },
    { "+vsync", SET_RESOURCE, CMDLINE_ATTRIB_NONE,
      NULL, NULL, "VSync", (resource_value_t)0,
      NULL, "Disable vsync to allow screen tearing" },
    { "-gtkfilter", SET_RESOURCE, CMDLINE_ATTRIB_NEED_ARGS,
      NULL, NULL, "GTKFilter", NULL,
      "<mode>", "Set filtering mode (0 = nearest, 1 = bilinear, 2 = cubic" },
    CMDLINE_LIST_END
};

/** \brief  Integer/boolean resources related to video output
 */
static const resource_int_t resources_int[] = {
    { "KeepAspectRatio", 1, RES_EVENT_NO, NULL,
      &keepaspect, set_keepaspect, NULL },
    { "TrueAspectRatio", 1, RES_EVENT_NO, NULL,
      &trueaspect, set_trueaspect, NULL },
    { "VSync", 1, RES_EVENT_NO, NULL,
      &vsync, set_vsync, NULL },
    { "GTKFilter", 2, RES_EVENT_NO, NULL,
      &display_filter, set_display_filter, NULL },
    RESOURCE_INT_LIST_END
};

int video_arch_get_active_chip(void)
{
    int window_idx = ui_get_main_window_index();

    switch (window_idx) {
        case SECONDARY_WINDOW:
            return VIDEO_CHIP_VDC;
            break;

        case PRIMARY_WINDOW:
        default:
            return VIDEO_CHIP_VICII;
            break;
    }
}

/** \brief  Arch-specific initialization for a video canvas
 *  \param[inout] canvas The canvas being initialized
 *  \sa video_canvas_create
 */
void video_arch_canvas_init(struct video_canvas_s *canvas)
{
}


/** \brief  Initialize command line options for generic video resouces
 *
 * \return  0 on success, < 0 on failure
 */
int video_arch_cmdline_options_init(void)
{
    if (machine_class != VICE_MACHINE_VSID) {
        return cmdline_register_options(cmdline_options);
    }
    return 0;
}


/** \brief  Initialize video-related resources
 *
 * \return  0 on success, < on failure
 */
int video_arch_resources_init(void)
{
    if (machine_class != VICE_MACHINE_VSID) {
        return resources_register_int(resources_int);
    }
    return 0;
}

/** \brief Clean up any memory held by arch-specific video resources. */
void video_arch_resources_shutdown(void)
{
}

/** \brief  Create a new video_canvas_s.
 *
 *  \param[in,out]  canvas  A freshly allocated canvas object.
 *  \param[in]      width   Pointer to a width value. May be NULL if canvas
 *                          size is not yet known.
 *  \param[in]      height  Pointer to a height value. May be NULL if canvas
 *                          size is not yet known.
 *  \param          mapped  Unused.
 *
 *  \return The completely initialized canvas. The window that holds
 *          it will be visible in the UI at time of return.
 */
video_canvas_t *video_canvas_create(video_canvas_t *canvas,
                                    unsigned int *width, unsigned int *height,
                                    int mapped)
{
    canvas->renderer_context = NULL;
    canvas->blank_ptr = NULL;
    canvas->pen_ptr = NULL;
    canvas->still_frame_callback_id = 0;
    canvas->pen_x = -1;
    canvas->pen_y = -1;
    canvas->pen_buttons = 0;

    if (!console_mode) {
        ui_create_main_window(canvas);
        ui_display_main_window(canvas->window_index);
    }

    canvas->created = 1;
    return canvas;
}

void video_canvas_new_frame_hook(struct video_canvas_s *canvas)
{
}

void video_canvas_on_new_backbuffer(video_canvas_t *canvas)
{
    canvas->renderer_backend->on_new_backbuffer(canvas);
}

/** \brief Free a previously created video canvas and all its
 *         components.
 *  \param[in] canvas The canvas to destroy.
 */
void video_canvas_destroy(struct video_canvas_s *canvas)
{
    if (canvas != NULL) {
        if (canvas->renderer_backend) {
            canvas->renderer_backend->destroy_context(canvas);
        }
        if (canvas->blank_ptr) {
            g_object_unref(G_OBJECT(canvas->blank_ptr));
            canvas->blank_ptr = NULL;
        }
        if (canvas->pen_ptr) {
            g_object_unref(G_OBJECT(canvas->pen_ptr));
            canvas->pen_ptr = NULL;
        }
    }
}

/** \brief React to changes of aspect ratio in the physical system.
 *
 *  For GTK3 this means we need to compute a new minimum size for our
 *  display windows.
 *
 *  \param canvas The canvas whose widgets need their sizes updated.
 */
void video_canvas_adjust_aspect_ratio(struct video_canvas_s *canvas)
{
    int width = canvas->draw_buffer->visible_width;
    int height = canvas->draw_buffer->visible_height;
    if (keepaspect && trueaspect) {
        width *= canvas->geometry->pixel_aspect_ratio;
    }

    /* Finally alter our minimum size so the GUI may respect the new minima/maxima */
    gtk_widget_set_size_request(canvas->event_box, width, height);
}

/** \brief Assign a palette to the canvas.
 * \param canvas The canvas to update the palette
 * \param palette The new palette to assign
 * \return Zero on success, nonzero on failure
 */
int video_canvas_set_palette(struct video_canvas_s *canvas,
                             struct palette_s *palette)
{
    if (!canvas || !palette) {
        return 0; /* No palette, nothing to do */
    }
    canvas->palette = palette;
    if (canvas->renderer_backend) {
        canvas->renderer_backend->set_palette(canvas);
    }
    return 0;
}

/** \brief Perform any frontend-specific initialization.
 *  \return 0 on success, nonzero on failure
 */
int video_init(void)
{
    if (gtk3video_log == LOG_ERR) {
        gtk3video_log = log_open("Gtk3Video");
    }
    return 0;
}

/** \brief Perform any frontend-specific uninitialization. */
void video_shutdown(void)
{
    /* It's a no-op */
}
