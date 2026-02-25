/** \file   joybuttonledwidget.h
 * \brief   Joystick button LED widget - header
 */

#ifndef VICE_JOY_BUTTON_LED_WIDGET_H
#define VICE_JOY_BUTTON_LED_WIDGET_H

#include <gtk/gtk.h>

GtkWidget *joy_button_led_widget_new       (const char *on, const char *off);
gboolean   joy_button_led_widget_get_active(GtkWidget *led);
void       joy_button_led_widget_set_active(GtkWidget *led, gboolean active);

#endif
