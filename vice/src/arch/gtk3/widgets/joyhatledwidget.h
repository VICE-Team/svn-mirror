/** \file   joyhatledwidget.h
 * \brief   Joystick hat LED widget - header
 */

#ifndef VICE_JOY_HAT_LED_WIDGET_H
#define VICE_JOY_HAT_LED_WIDGET_H

#include <gtk/gtk.h>

GtkWidget *joy_hat_led_widget_new       (const char *on, const char *off);
gboolean   joy_hat_led_widget_get_active_up(GtkWidget *led);
gboolean   joy_hat_led_widget_get_active_down(GtkWidget *led);
gboolean   joy_hat_led_widget_get_active_left(GtkWidget *led);
gboolean   joy_hat_led_widget_get_active_right(GtkWidget *led);
void       joy_hat_led_widget_set_active(GtkWidget *led, gboolean up, gboolean down, gboolean left, gboolean right);

#endif
