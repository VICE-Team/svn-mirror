/*
 * video.c - Native GTK3 UI video stuff.
 *
 * Written by
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
 *  Michael C. Martin <mcmartin@gmail.com>
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

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "debug_gtk3.h"
#include "not_implemented.h"

#include "cmdline.h"
#include "lib.h"
#include "log.h"
#include "machine.h"
#include "palette.h"
#include "raster.h"
#include "resources.h"
#include "translate.h"
#include "ui.h"
#include "videoarch.h"



#define VICE_DEBUG_NATIVE_GTK3

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

/** \brief  Command line options related to generic video output
 */
static const cmdline_option_t cmdline_options[] = {
    { "-trueaspect", SET_RESOURCE, 0,
      NULL, NULL, "TrueAspectRatio", (resource_value_t)1,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      IDCLS_UNUSED, IDCLS_UNUSED,
      NULL, "Enable true aspect ratio" },
    { "+trueaspect", SET_RESOURCE, 0,
      NULL, NULL, "TrueAspectRatio", (resource_value_t)0,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      IDCLS_UNUSED, IDCLS_UNUSED,
      NULL, "Disable true aspect ratio" },
    { "-keepaspect", SET_RESOURCE, 0,
      NULL, NULL, "KeepAspectRatio", (resource_value_t)1,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      IDCLS_UNUSED, IDCLS_UNUSED,
      NULL, "Keep aspect ratio when scaling" },
    { "+keepaspect", SET_RESOURCE, 0,
      NULL, NULL, "KeepAspectRatio", (resource_value_t)0,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      IDCLS_UNUSED, IDCLS_UNUSED,
      NULL, "Do not keep aspect ratio when scaling (freescale)" },
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

/** \brief  Redraw the screen from the canvas.
 */

/* Note that the ::draw signal receives a ready-to-be-used cairo_t
 * that is already clipped to only draw the exposed areas of the
 * widget */
static gboolean
draw_canvas_cb (GtkWidget *widget, cairo_t *cr, gpointer data)
{
    video_canvas_t *canvas = (video_canvas_t *)data;

    /* Half-grey background for those parts of the window that aren't
     * video, or black if it's fullscreen. 
     * TODO: configurable? */
    if (ui_is_fullscreen()) {
        cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
    } else {
        cairo_set_source_rgb(cr, 0.5, 0.5, 0.5);
    }
    cairo_paint(cr);

    if (canvas && canvas->backing_surface) {
        cairo_pattern_t *pattern = cairo_pattern_create_for_surface(canvas->backing_surface);
        cairo_pattern_set_matrix(pattern, &canvas->transform);
        cairo_set_source(cr, pattern);
        cairo_paint(cr);
        cairo_pattern_destroy(pattern);
    }

    return FALSE;
}

/** \brief  Callback to handle cases where the window is resized but
 *          the canvas is not
 */
static gboolean
resize_canvas_container_cb (GtkWidget *widget, GdkEventConfigure *event, gpointer data)
{
    /* The GtkDrawingArea that holds the canvas is "widget." */
    /* "canvas" is the data structure that holds the information we
     * need to do the render. Resizing its container only really
     * requires that we alter the transfomration matrix. */
    video_canvas_t *canvas = (video_canvas_t *)data;
    if (canvas && canvas->backing_surface) {
        /* Size of source canvas */
        double source_width = (double)cairo_image_surface_get_width(canvas->backing_surface);
        double source_height = (double)cairo_image_surface_get_height(canvas->backing_surface);
        /* Size of widget */
        double width = (double)gtk_widget_get_allocated_width(widget);
        double height = (double)gtk_widget_get_allocated_height(widget);
        /* TODO: This is the keepaspect = TRUE, trueaspect = FALSE
         * case. Each of these (and the entirely unscaled case) will
         * need their own transformation matrices. It's possible some
         * of them can be built out of the others so these end up
         * incremental. That would be nice, but it isn't guaranteed at
         * this time. */
        /* Try the Y-fit first */
        double scale = source_height / height;
        double offset_x = 0.0, offset_y = 0.0;
        if (source_width / scale > width) {
            /* Need to X-fit instead */
            scale = (double)source_width / width;
            offset_y = ((source_height / scale) - height) / 2.0;
        } else {
            offset_x = ((source_width / scale) - width) / 2.0;
        }
        /* Apply the computed scaling factor to both dimensions */
        cairo_matrix_init_scale(&canvas->transform, scale, scale);
        /* Center the result in the widget */
        cairo_matrix_translate(&canvas->transform, offset_x, offset_y);
    }
    /* No further processing should be needed */
    return FALSE;
}


/** \brief  Initialize video canvas
 */
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


void video_arch_resources_shutdown(void)
{
}


char video_canvas_can_resize(video_canvas_t *canvas)
{
    return 1;
}

/* FIXME: temporary hack */
extern void ui_set_toplevel_widget(GtkWidget *win, GtkWidget *status);

video_canvas_t *video_canvas_create(video_canvas_t *canvas,
                                    unsigned int *width, unsigned int *height,
                                    int mapped)
{
    VICE_GTK3_FUNC_ENTERED();
    canvas->initialized = 0;
    canvas->created = 0;
    canvas->backbuffer = NULL;
    canvas->backing_surface = NULL;

    if (width && *width && height && *height) {
        int stride = cairo_format_stride_for_width(CAIRO_FORMAT_RGB24, *width);
        if (stride <= 0) {
            fprintf(stderr, "Could not compute backbuffer size for %dx%d\n", *width, *height);
            return NULL;
        }
        canvas->backbuffer = lib_malloc(stride * *height);
        memset(canvas->backbuffer, 0, stride * *height);
        canvas->backing_surface = cairo_image_surface_create_for_data(canvas->backbuffer, CAIRO_FORMAT_RGB24, *width, *height, stride);
        cairo_matrix_init_translate(&canvas->transform, 0, 0);
    } else {
        /* This isn't a bug; basic initialization will quite happily
         * send us a 0x0 initial canvas until it knows more about what
         * exactly will be running and how */
        canvas->backbuffer = NULL;
        canvas->backing_surface = NULL;
    }

    ui_create_toplevel_window(canvas);
    if (width && height && *width && *height) {
        gtk_widget_set_size_request(canvas->drawing_area, *width, *height);
    }
    g_signal_connect(canvas->drawing_area, "draw", G_CALLBACK(draw_canvas_cb), canvas);
    g_signal_connect(canvas->drawing_area, "configure_event", G_CALLBACK(resize_canvas_container_cb), canvas);
    ui_display_toplevel_window(canvas);

    canvas->created = 1;
    canvas->initialized = 1;
    return canvas;
}

void video_canvas_destroy(struct video_canvas_s *canvas)
{
    if (canvas != NULL) {
#ifdef HAVE_FULLSCREEN
        fullscreen_shutdown_alloc_hooks(canvas);
        if (canvas->fullscreenconfig != NULL) {
            lib_free(canvas->fullscreenconfig);
        }
#endif
        if (canvas->backing_surface) {
            cairo_surface_finish(canvas->backing_surface);
            cairo_surface_destroy(canvas->backing_surface);
            canvas->backing_surface = NULL;
        }
        if (canvas->backbuffer) {
            lib_free(canvas->backbuffer);
            canvas->backbuffer = NULL;
        }
    }
}


void video_canvas_refresh(struct video_canvas_s *canvas,
                          unsigned int xs, unsigned int ys,
                          unsigned int xi, unsigned int yi,
                          unsigned int w, unsigned int h)
{
    unsigned int backing_surface_w, backing_surface_h;
    if (console_mode || video_disabled_mode || !canvas || !canvas->backing_surface) {
        return;
    }

    /* TODO: Do these even make sense? Are we making the renderer do
     *       more work than it needs to? */
    xi *= canvas->videoconfig->scalex;
    w *= canvas->videoconfig->scalex;

    yi *= canvas->videoconfig->scaley;
    h *= canvas->videoconfig->scaley;

    backing_surface_w = cairo_image_surface_get_width(canvas->backing_surface);
    backing_surface_h = cairo_image_surface_get_height(canvas->backing_surface);

    if (((xi + w) > backing_surface_w) || ((yi+h) > backing_surface_h)) {
        /* Trying to draw outside canvas? */
        fprintf(stderr, "Attempt to draw outside canvas!\nXI%u YI%u W%u H%u CW%u CH%u\n", xi, yi, w, h, backing_surface_w, backing_surface_h);
        return;
    }

    /* fprintf(stderr, "Xsc%d Ysc%d XS%u YS%u XI%u YI%u W%u H%u CW%u CH%u\n", canvas->videoconfig->scalex, canvas->videoconfig->scaley, xs, ys, xi, yi, w, h, backing_surface_w, backing_surface_h); */

    cairo_surface_flush(canvas->backing_surface);
    video_canvas_render(canvas, canvas->backbuffer, w, h, xs, ys, xi, yi, cairo_image_surface_get_stride(canvas->backing_surface), 32);
    cairo_surface_mark_dirty_rectangle(canvas->backing_surface, xi, yi, w, h);
    gtk_widget_queue_draw(canvas->drawing_area);
}

/* TODO: This reuses, with only slight modifications, logic we also
 *       see in video_canvas_create and video_canvas_destroy. It also
 *       directly calls GTK callbacks in ugly ways to avoid further
 *       replication. All of this would be better off split out into
 *       cleaner utility functions. */
void video_canvas_resize(struct video_canvas_s *canvas, char resize_canvas)
{
    if (!canvas || !canvas->drawing_area) {
        return;
    } else {
        int new_width = canvas->draw_buffer->canvas_physical_width;
        int new_height = canvas->draw_buffer->canvas_physical_height;
        int stride;

        if (new_width <= 0 || new_height <= 0) {
            /* Ignore impossible dimensions, but complain about it */
            fprintf(stderr, "%s:%d: warning: function %s called with impossible dimensions\n", __FILE__, __LINE__, __func__);
            return;
        }

        /* Clear out the old cairo material */
        if (canvas->backing_surface) {
            cairo_surface_finish(canvas->backing_surface);
            cairo_surface_destroy(canvas->backing_surface);
            canvas->backing_surface = NULL;
        }
        if (canvas->backbuffer) {
            lib_free(canvas->backbuffer);
            canvas->backbuffer = NULL;
        }

        /* Recreate a new one */
        stride = cairo_format_stride_for_width(CAIRO_FORMAT_RGB24, new_width);
        if (stride <= 0) {
            fprintf(stderr, "%s:%d: Cairo has exploded during %s\n", __FILE__, __LINE__, __func__);
            return;
        }
        canvas->backbuffer = lib_malloc(stride * new_height);
        memset(canvas->backbuffer, 0, stride * new_height);
        canvas->backing_surface = cairo_image_surface_create_for_data(canvas->backbuffer, CAIRO_FORMAT_RGB24, new_width, new_height, stride);

        /* And configure the matrix to fit it in the widget as it exists */
        resize_canvas_container_cb (canvas->drawing_area, NULL, canvas);

        /* Set the palette */
        if (video_canvas_set_palette(canvas, canvas->palette) < 0) {
            fprintf(stderr, "Setting palette for this mode failed. (Try 16/24/32 bpp.)");
            exit(-1);
        }

        /* Finally alter our minimum size so the GUI may react */
        gtk_widget_set_size_request(canvas->drawing_area, new_width, new_height);
    }
}

int video_canvas_set_palette(struct video_canvas_s *canvas,
                             struct palette_s *palette)
{
    int i;
    if (!canvas || !palette) {
        return 0; /* No palette, nothing to do */
    }
    canvas->palette = palette;
    /* We use CAIRO_FORMAT_RGB24, which is defined as follows: "Each
     * pixel is a 32-bit quantity, with the upper 8 bits unused. Red,
     * Green, and Blue are stored in the remaining 24 bits in that
     * order." */
    for (i = 0; i < palette->num_entries; i++) {
        palette_entry_t color = palette->entries[i];
        uint32_t cairo_color = (color.red << 16) | (color.green << 8) | color.blue;
        video_render_setphysicalcolor(canvas->videoconfig, i, cairo_color, 32);
    }

    for (i = 0; i < 256; i++) {
        video_render_setrawrgb(i, i << 16, i << 8, i);
    }
    video_render_initraw(canvas->videoconfig);

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
    /* It's a no-op */
}
