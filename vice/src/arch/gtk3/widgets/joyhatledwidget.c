/** \file   joyhatledwidget.c
 * \brief   Joystick hat LED widget
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 * \author  groepaz <groepaz@gmx.net>
 *
 * Show joystick hat state as a LED.
 */

#include "vice.h"
#include <gtk/gtk.h>
#include "log.h"

#include "joyhatledwidget.h"


#define LED_DRAW_WIDTH      28
#define LED_DRAW_HEIGHT     14

/** \brief  Width of widget including padding */
#define LED_WIDGET_WIDTH        (LED_DRAW_WIDTH * 3)

/** \brief  Height of widget including padding */
#define LED_WIDGET_HEIGHT       (LED_DRAW_HEIGHT * 3)

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
    gboolean up;        /**< up LED state */
    gboolean down;      /**< down LED state */
    gboolean left;      /**< left LED state */
    gboolean right;     /**< right LED state */
} map_state_t;


/** \brief  Get internal LED state object
 *
 * \param[in]   self    joy button LED
 *
 * \return  internal state object
 */
static map_state_t *get_state(GtkWidget *self)
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
    GdkRGBA      color_up;
    GdkRGBA      color_down;
    GdkRGBA      color_left;
    GdkRGBA      color_right;
    int          area_w;
    int          area_h;
    gdouble      led_w;
    gdouble      led_h;
    gdouble      led_x;
    gdouble      led_y;
    map_state_t *st = state;

    color_up = st->up ? st->color_on : st->color_off;
    color_down = st->down ? st->color_on : st->color_off;
    color_left = st->left ? st->color_on : st->color_off;
    color_right = st->right ? st->color_on : st->color_off;

    area_w = gtk_widget_get_allocated_width(self);
    area_h = gtk_widget_get_allocated_height(self);
    led_w  = area_w * LED_RECT_WIDTH_RATIO;
    led_h  = area_h * LED_RECT_HEIGHT_RATIO;
    led_x  = (area_w - led_w) / 2.0;    /* center */
    led_y  = (area_h - led_h) / 2.0;    /* center */

    led_w  = LED_DRAW_WIDTH * LED_RECT_WIDTH_RATIO;
    led_h  = LED_DRAW_HEIGHT * LED_RECT_HEIGHT_RATIO;

    cairo_set_source_rgb(cr, color_up.red, color_up.green, color_up.blue);
    cairo_rectangle(cr, led_x + (led_w * 1), led_y + (led_h * 0), led_w, led_h);
    cairo_fill(cr);

    cairo_set_source_rgb(cr, color_down.red, color_down.green, color_down.blue);
    cairo_rectangle(cr, led_x + (led_w * 1), led_y + (led_h * 2), led_w, led_h);
    cairo_fill(cr);

    cairo_set_source_rgb(cr, color_left.red, color_left.green, color_left.blue);
    cairo_rectangle(cr, led_x + (led_w * 0), led_y + (led_h * 1), led_w, led_h);
    cairo_fill(cr);

    cairo_set_source_rgb(cr, color_right.red, color_right.green, color_right.blue);
    cairo_rectangle(cr, led_x + (led_w * 2), led_y + (led_h * 1), led_w, led_h);
    cairo_fill(cr);

    return FALSE;
}


/** \brief  Create joystick hat LED widget
 *
 * Create LED-style indicator for joystick hat presses.
 *
 * The optional \a on and \a off arguments are strings that are passed to
 * \c gdk_rgba_parse(), see https://docs.gtk.org/gdk3/method.RGBA.parse.html
 *
 * \param[in]   on  color for \b ON (pass \c NULL for default #LED_COLOR_ON)
 * \param[in]   off color for \b OFF (pass \c NULL for default #LED_COLOR_OFF)
 *
 * \return  new joystick hat LED widget
 *
 * \see #LED_COLOR_ON
 * \see #LED_COLOR_OFF
 */
GtkWidget *joy_hat_led_widget_new(const char *on, const char *off)
{
    GtkWidget   *led;
    map_state_t *state;

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
    state->up = FALSE;
    state->down = FALSE;
    state->left = FALSE;
    state->right = FALSE;
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
gboolean joy_hat_led_widget_get_active_up(GtkWidget *led)
{
    map_state_t *state = get_state(led);

    return state->up;
}


/** \brief  Get current state of joystick button LED
 *
 * \param[in]   led joystick button LED
 *
 * \return  current state
 */
gboolean joy_hat_led_widget_get_active_down(GtkWidget *led)
{
    map_state_t *state = get_state(led);

    return state->down;
}


/** \brief  Get current state of joystick button LED
 *
 * \param[in]   led joystick button LED
 *
 * \return  current state
 */
gboolean joy_hat_led_widget_get_active_left(GtkWidget *led)
{
    map_state_t *state = get_state(led);

    return state->left;
}


/** \brief  Get current state of joystick button LED
 *
 * \param[in]   led joystick button LED
 *
 * \return  current state
 */
gboolean joy_hat_led_widget_get_active_right(GtkWidget *led)
{
    map_state_t *state = get_state(led);

    return state->right;
}


/** \brief  Set state of joystick hat LED
 *
 * \param[in]   led     joystick hat LED
 * \param[in]   active  new state for \a led
 */
void joy_hat_led_widget_set_active(GtkWidget *led, gboolean up, gboolean down, gboolean left, gboolean right)
{
    map_state_t *state = get_state(led);

    if ((up != state->up) ||
        (down != state->down) ||
        (left != state->left) ||
        (right != state->right)){
        /* state has changed: queue redraw of widget */
        gtk_widget_queue_draw(led);
    }
    state->up = up;
    state->down = down;
    state->left = left;
    state->right = right;
}
