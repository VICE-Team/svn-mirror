/**
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
#include "quartz_renderer.h"
#include "lightpen.h"
#include "mousedrv.h"
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

    if (event->type == GDK_MOTION_NOTIFY) {
        GdkEventMotion *motion = (GdkEventMotion *)event;
        double render_w = canvas->geometry->screen_size.width;
        double render_h = canvas->geometry->last_displayed_line - canvas->geometry->first_displayed_line + 1;
        int pen_x = (motion->x - canvas->screen_origin_x) * render_w / canvas->screen_display_w;
        int pen_y = (motion->y - canvas->screen_origin_y) * render_h / canvas->screen_display_h;
        if (pen_x < 0 || pen_y < 0 || pen_x >= render_w || pen_y >= render_h) {
            /* Mouse pointer is offscreen, so the light pen is disabled. */
            canvas->pen_x = -1;
            canvas->pen_y = -1;
            canvas->pen_buttons = 0;
        } else {
            canvas->pen_x = pen_x;
            canvas->pen_y = pen_y;
        }
    }
    return FALSE;
}

static gboolean event_box_mouse_button_cb(GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
    video_canvas_t *canvas = (video_canvas_t *)user_data;

    if (event->type == GDK_BUTTON_PRESS) {
        int button = ((GdkEventButton *)event)->button;
        if (button == 1) {
            /* Left mouse button */
            canvas->pen_buttons |= LP_HOST_BUTTON_1;
        } else if (button == 3) {
            /* Right mouse button */
            canvas->pen_buttons |= LP_HOST_BUTTON_2;
        }
        mouse_button(button-1, 1);
    } else if (event->type == GDK_BUTTON_RELEASE) {
        int button = ((GdkEventButton *)event)->button;
        if (button == 1) {
            /* Left mouse button */
            canvas->pen_buttons &= ~LP_HOST_BUTTON_1;
        } else if (button == 3) {
            /* Right mouse button */
            canvas->pen_buttons &= ~LP_HOST_BUTTON_2;
        }        
        mouse_button(button-1, 0);
    }
    /* Ignore all other mouse button events, though we'll be sent things like double- and triple-click. */
    return FALSE;
}

static gboolean event_box_scroll_cb(GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
    GdkScrollDirection dir = ((GdkEventScroll *)event)->direction;
    gdouble smooth_x = 0.0, smooth_y = 0.0;
    switch (dir) {
    case GDK_SCROLL_UP:
        mouse_button(3, 1);
        break;
    case GDK_SCROLL_DOWN:
        mouse_button(4, 1);
        break;
    case GDK_SCROLL_SMOOTH:
        /* Isolate the Y component of a smooth scroll */
        if (gdk_event_get_scroll_deltas(event, &smooth_x, &smooth_y)) {
            if (smooth_y < 0) {
                mouse_button(3, 1);
            } else if (smooth_y > 0) {
                mouse_button(4, 1);
            }
        }
        break;
    default:
        /* Ignore left and right scroll */
        break;
    }
    return FALSE;
}    

static GdkCursor *make_cursor(GtkWidget *widget, const char *name)
{
    GdkDisplay *display = gtk_widget_get_display(widget);
    GdkCursor *result = NULL;

    if (display) {
        result = gdk_cursor_new_from_name(display, name);
        if (result != NULL) {
            g_object_ref_sink(G_OBJECT(result));
        }
    }
    return result;
}

static gboolean event_box_stillness_tick_cb(GtkWidget *widget, GdkFrameClock *clock, gpointer user_data)
{
    video_canvas_t *canvas = (video_canvas_t *)user_data;

    ++canvas->still_frames;
    if (!lightpen_enabled && canvas->still_frames > 60) {
        if (canvas->blank_ptr == NULL) {
            canvas->blank_ptr = make_cursor(widget, "none");
        }
        if (canvas->blank_ptr != NULL) {
            GdkWindow *window = gtk_widget_get_window(widget);

            if (window) {
                gdk_window_set_cursor(window, canvas->blank_ptr);
            }
        }
    } else {
        GdkWindow *window = gtk_widget_get_window(widget);
        if (canvas->pen_ptr == NULL) {
            canvas->pen_ptr = make_cursor(widget, "crosshair");
        }
        if (window) {
            if (lightpen_enabled && canvas->pen_ptr) {
                gdk_window_set_cursor(window, canvas->pen_ptr);
            } else {
                gdk_window_set_cursor(window, NULL);
            }
        }
    }
    return G_SOURCE_CONTINUE;
}

static gboolean event_box_cross_cb(GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
    video_canvas_t *canvas = (video_canvas_t *)user_data;
    GdkEventCrossing *crossing = (GdkEventCrossing *)event;

    if (!canvas || !event ||
        (event->type != GDK_ENTER_NOTIFY && event->type != GDK_LEAVE_NOTIFY) ||
        crossing->mode != GDK_CROSSING_NORMAL) {
        /* Spurious event */
        return FALSE;
    }
    
    if (crossing->type == GDK_ENTER_NOTIFY) {
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
        canvas->pen_x = -1;
        canvas->pen_y = -1;
        canvas->pen_buttons = 0;
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
    gtk_widget_add_events(new_event_box, GDK_BUTTON_PRESS_MASK);
    gtk_widget_add_events(new_event_box, GDK_BUTTON_RELEASE_MASK);
    gtk_widget_add_events(new_event_box, GDK_SCROLL_MASK);
    g_signal_connect(new_event_box, "enter-notify-event", G_CALLBACK(event_box_cross_cb), canvas);
    g_signal_connect(new_event_box, "leave-notify-event", G_CALLBACK(event_box_cross_cb), canvas);
    g_signal_connect(new_event_box, "motion-notify-event", G_CALLBACK(event_box_motion_cb), canvas);
    g_signal_connect(new_event_box, "button-press-event", G_CALLBACK(event_box_mouse_button_cb), canvas);
    g_signal_connect(new_event_box, "button-release-event", G_CALLBACK(event_box_mouse_button_cb), canvas);
    g_signal_connect(new_event_box, "scroll-event", G_CALLBACK(event_box_scroll_cb), canvas);

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
