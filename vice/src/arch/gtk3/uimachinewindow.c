/** \file   src/arch/gtk3/uimachinewindow.c
 * \brief   Native GTK3 main emulator window code.
 *
 * Written by
 *  Marcus Sutton <loggedoubt@gmail.com>
 *
 * based on code by
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

/* XXX: It should be possible to compile, link and run vsid while this
 *      entire file (amongst others) is contained inside an #if 0 wrapper.
 */
#if 1

#include "vice.h"

#include <gtk/gtk.h>

#include "cairo_renderer.h"
#include "opengl_renderer.h"
#include "videoarch.h"

#include "ui.h"
#include "uimachinemenu.h"
#include "uimachinewindow.h"

#if 0
static void event_box_no_cleanup_needed(gpointer ignored)
{
    /* Yep, ignored */
}
#endif

static gboolean event_box_motion_cb(GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
    video_canvas_t *canvas = (video_canvas_t *)user_data;

    canvas->still_frames = 0;
    return FALSE;
}

static gboolean event_box_stillness_tick_cb(GtkWidget *widget, GdkFrameClock *clock, gpointer user_data)
{
    video_canvas_t *canvas = (video_canvas_t *)user_data;

    ++canvas->still_frames;
    if (canvas->still_frames > 60) {
        GdkDisplay *display = gtk_widget_get_display(widget);

        if (display != NULL && canvas->blank_ptr == NULL) {
            canvas->blank_ptr = gdk_cursor_new_from_name(display, "none");
            if (canvas->blank_ptr != NULL) {
                g_object_ref_sink(G_OBJECT(canvas->blank_ptr));
            } else {
                /* FIXME: This can fill a terminal with repeated text */
                fprintf(stderr, "GTK3 CURSOR: Could not allocate blank pointer for canvas\n");
            }
        }
        if (canvas->blank_ptr != NULL) {
            GdkWindow *window = gtk_widget_get_window(widget);

            if (window) {
                gdk_window_set_cursor(window, canvas->blank_ptr);
            }
        }
    } else {
        GdkWindow *window = gtk_widget_get_window(widget);
        if (window) {
            gdk_window_set_cursor(window, NULL);
        }
    }
    return G_SOURCE_CONTINUE;
}

static gboolean event_box_cross_cb(GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
    video_canvas_t *canvas = (video_canvas_t *)user_data;

    if (event && event->type == GDK_ENTER_NOTIFY) {
        canvas->still_frames = 0;
        if (canvas->still_frame_callback_id == 0) {
            canvas->still_frame_callback_id = gtk_widget_add_tick_callback(canvas->drawing_area,
                                                                           event_box_stillness_tick_cb,
                                                                           canvas, NULL);
        }
    } else {
        GdkWindow *window = gtk_widget_get_window(canvas->drawing_area);

        if (window) {
            gdk_window_set_cursor(window, NULL);
        }
        if (canvas->still_frame_callback_id != 0) {
            gtk_widget_remove_tick_callback(canvas->drawing_area, canvas->still_frame_callback_id);
            canvas->still_frame_callback_id = 0;
        }
    }
    return FALSE;
}

static void machine_window_create(video_canvas_t *canvas)
{
    GtkWidget *new_drawing_area, *new_event_box;
    GtkWidget *menu_bar;

    /* TODO: Make the rendering process transparent enough that this can be selected and altered as-needed */
#ifdef HAVE_GTK3_OPENGL
    canvas->renderer_backend = &vice_opengl_backend;
#else
    canvas->renderer_backend = &vice_cairo_backend;
#endif

    new_drawing_area = canvas->renderer_backend->create_widget(canvas);
    canvas->drawing_area = new_drawing_area;

    new_event_box = gtk_event_box_new();
    gtk_container_add(GTK_CONTAINER(new_event_box), new_drawing_area);

    gtk_widget_add_events(new_event_box, GDK_POINTER_MOTION_MASK);
    g_signal_connect(new_event_box, "enter-notify-event", G_CALLBACK(event_box_cross_cb), canvas);
    g_signal_connect(new_event_box, "leave-notify-event", G_CALLBACK(event_box_cross_cb), canvas);
    g_signal_connect(new_event_box, "motion-notify-event", G_CALLBACK(event_box_motion_cb), canvas);

    /* I'm pretty sure when running x128 we get two menu instances, so this
     * should go somewhere else: call ui_menu_bar_create() once and attach the
     * result menu to each GtkWindow instance
     */
    menu_bar = ui_machine_menu_bar_create();

    gtk_container_add(GTK_CONTAINER(canvas->grid), menu_bar);
    gtk_container_add(GTK_CONTAINER(canvas->grid), new_event_box);

    return;
}

void ui_machine_window_init(void)
{
    ui_set_create_window_func(machine_window_create);
    return;
}

#endif
