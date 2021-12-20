/** \file   statusbarledwidget.c
 * \brief   Small LED widgets for the status bar
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 *
 * A small widget with a label and a drawing area displaying on/off status.
 *
 * Can be used to display statuses like 'Pause' and 'Warp', and perhaps also
 * key states such as 'ShiftLock', '40/80 Column' etc.
 *
 * The widget is structured as follows:
 *
 * +-GtkEventBox-------------------+
 * | +-GtkGrid--+----------------+ |
 * | | GtkLabel | GtkDrawingArea | |
 * | +----------+----------------+ |
 * +-------------------------------+
 *
 */

/*
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
 */

#include "vice.h"
#include <gtk/gtk.h>
#include <stdint.h>

#include "vice_gtk3.h"
#include "lib.h"
#include "log.h"

#include "statusbarledwidget.h"


/** \brief  Default color for ON */
#define COLOR_DEFAULT_ON    "#00ff00"

/** \brief  Default color for OFF */
#define COLOR_DEFAULT_OFF   "#ff0000"

/* column indexes of the grid */
enum {
    COLUMN_LABEL,   /**< column of label */
    COLUMN_LED      /**< column of led */
};


/** \brief  CSS to reduce size of the label */
#define LABEL_CSS \
    "label {\n" \
    "    font-size: 90%;\n" \
    "}\n"


/** \brief  LED state object
 */
typedef struct led_state_s {
    GdkRGBA     color_on;   /**< color for ON */
    GdkRGBA     color_off;  /**< color for OFF */
    gboolean    active;     /**< LED is ON */
} led_state_t;



/** \brief  Get grid inside the event box
 *
 * \param[in]   widget  event box
 *
 * \return  GtkGrid
 */
static GtkWidget *get_grid(GtkWidget *widget)
{
    return gtk_bin_get_child(GTK_BIN(widget));
}


/** \brief  Get internal state object of the widget
 *
 * Small helper function to get the internal state object of \a grid.
 *
 * \param[in]   grid    status bar led widget grid
 *
 * \return  state object
 */
static led_state_t *get_state(GtkWidget *grid)
{
    return g_object_get_data(G_OBJECT(grid), "InternalState");
}


/** \brief  Get the drawing area presenting the LED
 *
 * \param[in]   widget  status bar led widget
 *
 * \return  GtkDrawingArea
 */
static GtkWidget *get_led_widget(GtkWidget *widget)
{
    GtkWidget *grid = get_grid(widget);

    if (grid != NULL) {
        return gtk_grid_get_child_at(GTK_GRID(grid), COLUMN_LED, 0);
    }
    return NULL;
}


/** \brief  Handler for the 'destroy' event of the **grid**
 *
 * Deallocates memory used for the internal state.
 *
 * \param[in]   widget  status bar led widget grid
 * \param[in]   data    extra event data (unused)
 */
static void on_destroy(GtkWidget *grid, gpointer data)
{
    led_state_t *state = get_state(grid);

    if (state != NULL) {
        lib_free(state);
    }
}


/** \brief  Handler for the 'draw' event of the  widget
 *
 * \param[in]   widget  LED GtkDrawingArea
 * \param[in]   cr      cairo context that handles the drawing
 * \param[in]   data    extra event data (unused)
 *
 *  \return FALSE, telling GTK to continue event processing
 */
static gboolean on_led_draw(GtkWidget *widget, cairo_t *cr, gpointer data)
{
    led_state_t *state;
    GdkRGBA rgba;
    int area_width;
    int area_height;
    double led_width;
    double led_height;
    double led_xpos;
    double led_ypos;

    state = get_state(gtk_widget_get_parent(widget));
    if (state->active) {
        rgba = state->color_on;
    } else {
        rgba = state->color_off;
    }
    area_width = gtk_widget_get_allocated_width(widget);
    area_height = gtk_widget_get_allocated_height(widget);

    cairo_set_source_rgb(cr, rgba.red, rgba.green, rgba.blue);

    led_height = area_height / 2.0;                 /* half the widget height */
    led_width = area_width / 2.0;                   /* 2x1 ratio */
    led_xpos = (area_width - led_width) / 2.0;      /* centered horizontally */
    led_ypos = area_height / 4.0;

    cairo_rectangle(cr, led_xpos, led_ypos, led_width, led_height);
    cairo_fill(cr);

    return FALSE;
}


/** \brief  Handler for button press events
 *
 * \param[in]   self    widget triggering the event
 * \param[in]   event   event data
 * \param[in]   data    extra event data (unused)
 *
 * \return  FALSE, don't propagate further
 */
static gboolean on_button_press_event(GtkWidget *self, GdkEvent *event, gpointer data)
{
    if (((GdkEventButton *)event)->button == GDK_BUTTON_PRIMARY) {

        GtkWidget *grid;
        gboolean toggleable;
        gboolean active;
        void (*callback)(GtkWidget *, gboolean);

        grid = get_grid(self);
        toggleable = statusbar_led_widget_get_toggleable(self);
        active = statusbar_led_widget_get_active(self);
        callback = g_object_get_data(G_OBJECT(grid), "ToggleCallback");

        if (toggleable) {
            active = !active;
            statusbar_led_widget_set_active(self, active);
            if (callback != NULL) {
                callback(self, active);
            }
        }
    }
    return FALSE;
}


/** \brief  Create LED widget for status bar
 *
 * Create a small widget with a label and a two-state LED.
 *
 * The colors are parsed with gdk_rgba_parse(), and if parsing fails default to
 * \#00ff00 (green) for 'on' and \#ff0000 for 'off'.
 *
 * \param[in]   text    label text
 * \param[in]   on      color for on (use NULL for default green)
 * \param[in]   off     color for off (used NULL for default red)
 *
 * \return  GtkGrid
 */
GtkWidget *statusbar_led_widget_create(const gchar *text,
                                       const gchar *on,
                                       const gchar *off)
{
    GtkWidget *event_box;
    GtkWidget *grid;
    GtkWidget *label;
    GtkWidget *led;
    GtkCssProvider *css;
    led_state_t *state;

    grid = gtk_grid_new();
    css = vice_gtk3_css_provider_new(LABEL_CSS);

    label = gtk_label_new(text);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_widget_set_valign(label, GTK_ALIGN_CENTER);
    gtk_widget_set_hexpand(label, FALSE);
    gtk_widget_set_vexpand(label, FALSE);
    vice_gtk3_css_provider_add(label, css);
    gtk_grid_attach(GTK_GRID(grid), label, COLUMN_LABEL, 0, 1, 1);

    led = gtk_drawing_area_new();
    gtk_widget_set_halign(led, GTK_ALIGN_START);
    gtk_widget_set_valign(led, GTK_ALIGN_CENTER);
    gtk_widget_set_hexpand(led, FALSE);
    gtk_widget_set_vexpand(led, FALSE);
    gtk_widget_set_size_request(led, 28, 14);
    /*  drawing areas don't respond to button clicks by default */
    /* gtk_widget_add_events(led, GDK_BUTTON_PRESS_MASK|GDK_BUTTON_RELEASE_MASK); */
    gtk_grid_attach(GTK_GRID(grid), led, COLUMN_LED, 0, 1, 1);
    g_signal_connect_unlocked(led, "draw", G_CALLBACK(on_led_draw), NULL);

    /* set defaults if requested */
    if (on == NULL || *on == '\0') {
        on = COLOR_DEFAULT_ON;
    }
    if (off == NULL || *off == '\0') {
        off = COLOR_DEFAULT_OFF;
    }

    /* wrap grid in event box to capture button press events */
    event_box = gtk_event_box_new();
    gtk_container_add(GTK_CONTAINER(event_box), grid);

    /* create internal state object */
    state = lib_malloc(sizeof *state);
    if (!gdk_rgba_parse(&(state->color_on), on)) {
        log_warning(LOG_DEFAULT,
                    "statusbar LED: failed to parse '%s' as a valid color for"
                    " ON, defaulting to '%s'.",
                    on, COLOR_DEFAULT_ON);
        gdk_rgba_parse(&(state->color_on), COLOR_DEFAULT_ON);
    }
    if (!gdk_rgba_parse(&(state->color_off), off)) {
        log_warning(LOG_DEFAULT,
                    "statusbar LED: failed to parse '%s' as a valid color for"
                    " OFF, defaulting to '%s'.",
                    off, COLOR_DEFAULT_OFF);
        gdk_rgba_parse(&(state->color_off), COLOR_DEFAULT_OFF);
    }
    state->active = FALSE;

    g_object_set_data(G_OBJECT(grid), "InternalState", (gpointer)state);
    g_object_set_data(G_OBJECT(grid), "IsToggleable", GINT_TO_POINTER(FALSE));
    g_object_set_data(G_OBJECT(grid), "ToggleCallback", NULL);

    g_signal_connect(event_box, "button-press-event", G_CALLBACK(on_button_press_event), NULL);
    g_signal_connect(grid, "destroy", G_CALLBACK(on_destroy), NULL);

    gtk_widget_show_all(grid);
    return event_box;
}


/** \brief  Set status bar LED widget state
 *
 * \param[in]   widget  status bar led widget
 * \param[in]   active  new state (TRUE = on, FALSE = off)
 */
void statusbar_led_widget_set_active(GtkWidget *widget, gboolean active)
{
    led_state_t *state;
    gboolean old_state;

    state = get_state(get_grid(widget));
    old_state = state->active;

    state->active = active;
    if (old_state != active) {
       gtk_widget_queue_draw(get_led_widget(widget));
    }
}


/** \brief  Get status bar LED widget state
 *
 * \param[in]   widget  status bar led widget
 *
 * \return  TRUE for on, FALSE for off
 */
gboolean statusbar_led_widget_get_active(GtkWidget *widget)
{
    led_state_t *state = get_state(get_grid(widget));

    return state->active;
}


/** \brief  Make \a widget toggleable, toggling state on mouse click
 *
 * Make the widget accept mouse clicks, toggling its state.
 *
 * \param[in]   widget      status bar led widget
 * \param[in]   toggleable  accept mouse clicks to toggle state
 */
void statusbar_led_widget_set_toggleable(GtkWidget *widget, gboolean toggleable)
{
    GtkWidget *grid = get_grid(widget);

    g_object_set_data(G_OBJECT(grid), "IsToggleable", GINT_TO_POINTER(toggleable));
}


/** \brief  Get toggleable state of \a widget
 *
 * \return  LED widget is toggleable via mouse click
 */
gboolean statusbar_led_widget_get_toggleable(GtkWidget *widget)
{
    GtkWidget *grid = gtk_bin_get_child(GTK_BIN(widget));
    gpointer data = g_object_get_data(G_OBJECT(grid), "IsToggleable");

    return (gboolean)GPOINTER_TO_INT(data);
}


/** \brief  Set callback to trigger on state change
 *
 * Set callback function \a cb to be triggered on LED status change, which
 * means through mouse clicks (enabled via statusbar_led_widget_set_toggleable())
 * but not through statusbar_led_widget_set_active().
 *
 * \param[in]   widget  status bar led widget
 * \param[in]   cb      callback function
 */
void statusbar_led_widget_set_toggle_callback(GtkWidget *widget,
                                              void (*cb)(GtkWidget *, gboolean))
{
    GtkWidget *grid = gtk_bin_get_child(GTK_BIN(widget));

    g_object_set_data(G_OBJECT(grid), "ToggleCallback", (gpointer)cb);
}
