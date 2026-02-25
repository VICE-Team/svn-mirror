/** \file   joybuttonledwidget.c
 * \brief   Joystick button LED widget
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 *
 * Show joystick button state as a LED.
 */

#include "vice.h"
#include <gtk/gtk.h>
#include "log.h"

#include "joybuttonledwidget.h"


/** \brief  Width of widget including padding */
#define LED_WIDGET_WIDTH        28

/** \brief  Height of widget including padding */
#define LED_WIDGET_HEIGHT       14

/** \brief  Ratio of drawing area rectangle width to total widget widget */
#define LED_RECT_WIDTH_RATIO    0.666

/** \brief  Ratio of drawing area rectangle height to total widget height */
#define LED_RECT_HEIGHT_RATIO   0.666

/** \brief  Default color for ON/Pressed */
#define LED_COLOR_ON            "#20ff33"

/** \brief  Default color for OFF/Released */
#define LED_COLOR_OFF           "#333333"


/** \brief  InternaL state of a LED */
typedef struct led_state_s {
    GdkRGBA  color_on;  /**< color for ON */
    GdkRGBA  color_off; /**< color for OFF */
    gboolean active;    /**< LED active state */
} led_state_t;


/** \brief  Get internal LED state object
 *
 * \param[in]   self    joy button LED
 *
 * \return  internal state object
 */
static led_state_t *get_state(GtkWidget *self)
{
    return g_object_get_data(G_OBJECT(self), "LEDState");
}

/** \brief  Handler for the 'destroy' event of a LED
 *
 * Clean up extra state of widget.
 *
 * \param[in]   self    joy button LED
 * \param[in]   state   joy button state object
 */
static void on_destroy(GtkWidget *self, gpointer state)
{
    g_free(state);
}

/** \brief  Handler for the 'draw' event of a LED
 *
 * \param[in]   self    joy button LED
 * \param[in]   cr      cairo context
 * \param[in]   state   LED internal state object
 *
 * \return  \c FALSE to allow other events to propagate (do we need this?)
 */
static gboolean on_draw(GtkWidget *self, cairo_t *cr, gpointer state)
{
    GdkRGBA      color;
    int          area_w;
    int          area_h;
    gdouble      led_w;
    gdouble      led_h;
    gdouble      led_x;
    gdouble      led_y;
    led_state_t *st = state;

    color  = st->active ? st->color_on : st->color_off;
    area_w = gtk_widget_get_allocated_width(self);
    area_h = gtk_widget_get_allocated_height(self);
    led_w  = area_w * LED_RECT_WIDTH_RATIO;
    led_h  = area_h * LED_RECT_HEIGHT_RATIO;
    led_x  = (area_w - led_w) / 2.0;    /* center */
    led_y  = (area_h - led_h) / 2.0;    /* center */

    cairo_set_source_rgb(cr, color.red, color.green, color.blue);
    cairo_rectangle(cr, led_x, led_y, led_w, led_h);
    cairo_fill(cr);

    return FALSE;
}


/** \brief  Create joystick button LED widget
 *
 * Create LED-style indicator for joystick button presses.
 *
 * The optional \a on and \a off arguments are strings that are passed to
 * \c gdk_rgba_parse(), see https://docs.gtk.org/gdk3/method.RGBA.parse.html
 *
 * \param[in]   on  color for \b ON (pass \c NULL for default #LED_COLOR_ON)
 * \param[in]   off color for \b OFF (pass \c NULL for default #LED_COLOR_OFF)
 *
 * \return  new joystick button LED widget
 *
 * \see #LED_COLOR_ON
 * \see #LED_COLOR_OFF
 */
GtkWidget *joy_button_led_widget_new(const char *on, const char *off)
{
    GtkWidget   *led;
    led_state_t *state;

    if (on == NULL) {
        on = LED_COLOR_ON;
    }
    if (off == NULL) {
        off = LED_COLOR_OFF;
    }

    /* set up drawing area */
    led = gtk_drawing_area_new();
    gtk_widget_set_hexpand(led, FALSE);
    gtk_widget_set_vexpand(led, FALSE);
    gtk_widget_set_halign(led, GTK_ALIGN_START);
    gtk_widget_set_valign(led, GTK_ALIGN_CENTER);
    gtk_widget_set_size_request(led, LED_WIDGET_WIDTH, LED_WIDGET_HEIGHT);

    /* set up internal state */
    state = g_malloc(sizeof *state);
    state->active = FALSE;
    if (!gdk_rgba_parse(&state->color_on, on)) {
        log_warning(LOG_DEFAULT,
                    "joy button LED: failed to parse '%s' as a valid color for"
                    " ON, falling back to '%s'",
                    on, LED_COLOR_ON);
        gdk_rgba_parse(&state->color_on, LED_COLOR_ON);
    }
    if (!gdk_rgba_parse(&state->color_off, off)) {
        log_warning(LOG_DEFAULT,
                    "joy button LED: failed to parse '%s' as a valid color for"
                    " OFF, falling back to '%s'",
                    off, LED_COLOR_OFF);
        gdk_rgba_parse(&state->color_off, LED_COLOR_OFF);
    }
    g_object_set_data(G_OBJECT(led), "LEDState", (gpointer)state);

    g_signal_connect_unlocked(G_OBJECT(led),
                              "destroy",
                              G_CALLBACK(on_destroy),
                              (gpointer)state);
    g_signal_connect_unlocked(G_OBJECT(led),
                              "draw",
                              G_CALLBACK(on_draw),
                              (gpointer)state);
    return led;
}


/** \brief  Get current state of joystick button LED
 *
 * \param[in]   led joystick button LED
 *
 * \return  current state
 */
gboolean joy_button_led_widget_get_active(GtkWidget *led)
{
    led_state_t *state = get_state(led);

    return state->active;
}


/** \brief  Set state of joystick button LED
 *
 * \param[in]   led     joystick button LED
 * \param[in]   active  new state for \a led
 */
void joy_button_led_widget_set_active(GtkWidget *led, gboolean active)
{
    led_state_t *state = get_state(led);

    if (active != state->active) {
        state->active = active;
        /* state has changed: queue redraw of widget */
        gtk_widget_queue_draw(led);
    }
}
