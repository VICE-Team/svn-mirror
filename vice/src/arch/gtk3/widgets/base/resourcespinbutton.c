/** \file   src/arch/gtk3/widgets/base/resourcespinbutton.c
 * \brief   Spin buttons to control resources
 *
 * This file contains spin buttons to control resources. The integer resource
 * spin button in its default state should be clear: alter integers. But the
 * "fake digits" might require some explanation:
 *
 * With "fake digits" I mean displaying an integer resource as if it were a
 * floating point value. There are some VICE resources that are used a integers
 * but actually represent a different 'scale'.
 *
 * An example is the `Drive[8-11]RPM` resource, this represents a drive's RPM
 * in hundredths of the actually resource value, ie `31234` means `312.34` RPM.
 *
 * So to display this value properly, one would set "fake digits to `2`. This
 * way the widget divides the actual value by 10^2 when displaying, and
 * multiplies the input value by 10^2 when setting the value through the widget.
 *
 * I've decided to support 1-5 digits, that ought to be enough. And since we're
 * displaying integer values divided by a power of 10, the "fake digits" number
 * also sets the displayed digits, since that's the maximum accuracy.
 *
 * That behaviour can be changed by calling gtk_spin_button_set_digits() on
 * the widget.
 *
 * Written by
 *  Bas Wassink <b.wassink@ziggo.nl>
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

#include <gtk/gtk.h>
#include "math.h"

#include "basewidget_types.h"
#include "debug_gtk3.h"
#include "lib.h"
#include "resources.h"
#include "resourcehelpers.h"

#include "resourcespinbutton.h"


/** \brief  Handler for the "destroy" event of \a widget
 *
 * Frees memory used by the copy of the resource name.
 *
 * \param[in]   widget      integer spin button
 * \param[in]   user_data   extra event data (unused)
 */
static void on_spin_button_destroy(GtkWidget *spin, gpointer user_data)
{
    resource_widget_free_resource_name(spin);
}


/** \brief  Handler for the "input" event of the \a spin button
 *
 * Checks and converts input of the \a spin button.
 *
 * \param[in,out]   spin        integer spin button
 * \param[out]      new_value   pointer to the value of the spin button
 * \param[in]       user_data   extra event data (unused)
 *
 * \return  TRUE when input is valid, GTK_INPUT_ERROR on invalid input
 */
static gint on_spin_button_input(GtkWidget *spin,
                                 gpointer new_value,
                                 gpointer user_data)
{
    const gchar *text;
    gchar *endptr;
    gdouble value;
    gdouble multiplier;
    int digits;

    digits = resource_widget_get_int(spin, "FakeDigits");
    multiplier = pow(10.0, (gdouble)digits);

    text = gtk_entry_get_text(GTK_ENTRY(spin));
    value = g_strtod(text, &endptr);
    if (*endptr != '\0') {
        return GTK_INPUT_ERROR;
    }

    *(gdouble *)new_value = value * multiplier;
    return TRUE;
}


/** \brief  Handler for the "output" event of the \a spin button
 *
 * This outputs the spin button value as a floating point value, with digits
 * depending on the "fake digits" setting.
 *
 * \param[in,out]   spin        integer spin button
 * \param[in]       user_data   extra event data (unused)
 *
 * \return  TRUE
 */
static gboolean on_spin_button_output(GtkWidget *spin, gpointer user_data)
{
    GtkAdjustment *adj;
    gchar *text;
    gdouble value;
    gdouble divisor;
    int digits;
    /* silly, but avoids the 'format' is not const warnings */
    const char *fmt[] = {
        "%.1f", "%.2f", "%.3f","%.4f", "%.5f"
    };

    digits = resource_widget_get_int(spin, "FakeDigits");
    divisor = pow(10.0, (gdouble)digits);

    adj = gtk_spin_button_get_adjustment(GTK_SPIN_BUTTON(spin));
    value = gtk_adjustment_get_value(adj);
    text = g_strdup_printf(fmt[digits - 1], value / divisor);
    gtk_entry_set_text(GTK_ENTRY(spin), text);
    g_free(text);

    return TRUE;
}



/** \brief  Handler for the "value-changed" event of \a widget
 *
 * Updates the resource with the current value of the spin buttton
 *
 * \param[in]   spin        integer spin button
 * \param[in]   user_data   extra event data (unused)
 */
static void on_spin_button_value_changed(GtkWidget *spin, gpointer user_data)
{
    const char *res;
    int value;

    res = resource_widget_get_resource_name(spin);
    value = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin));
    debug_gtk3("setting %s to %d\n", res, value);
    resources_set_int(res, value);
}


/** \brief  Create integer spin button to control a resource - helper
 *
 * \param[in]   spin    spin button
 *
 * \return  GtkSpinButton
 */
static GtkWidget *resource_spin_button_int_create_helper(GtkWidget *spin)
{
    int current;
    const char *resource;

    resource = resource_widget_get_resource_name(spin);

    /* set fake digits to 0 */
    g_object_set_data(G_OBJECT(spin), "FakeDigits", GINT_TO_POINTER(0));

    /* set real digits to 0 */
    gtk_spin_button_set_digits(GTK_SPIN_BUTTON(spin), 0);

    resources_get_int(resource, &current);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin), (gdouble)current);

    g_signal_connect(spin, "value-changed",
            G_CALLBACK(on_spin_button_value_changed),NULL);
    g_signal_connect(spin, "destroy",
            G_CALLBACK(on_spin_button_destroy), NULL);

    gtk_widget_show(spin);
    return spin;
}



/** \brief  Create integer spin button to control \a resource
 *
 * \param[in]   resource    resource name
 * \param[in]   lower       lowest value
 * \param[in]   upper       highest value
 * \param[in]   step        step for the +/- buttons
 *
 * \return  GtkSpinButton
 */
GtkWidget *vice_gtk3_resource_spin_button_int_create(
        const char *resource,
        int lower, int upper, int step)
{
    GtkWidget *spin;

    spin = gtk_spin_button_new_with_range((gdouble)lower, (gdouble)upper,
            (gdouble)step);

    /* set a copy of the resource name */
    resource_widget_set_resource_name(spin, resource);

    return resource_spin_button_int_create_helper(spin);
}


/** \brief  Create integer spin button to control \a resource
 *
 * Allows setting the resource name with a variable argument list
 *
 * \param[in]   fmt     resource name format string
 * \param[in]   lower   lowest value
 * \param[in]   upper   highest value
 * \param[in]   step    step for the +/- buttons
 *
 * \return  GtkSpinButton
 */
GtkWidget *vice_gtk3_resource_spin_button_int_create_sprintf(
        const char *fmt,
        int lower, int upper, int step,
        ...)
{
    GtkWidget *spin;
    char *resource;
    va_list args;

    spin = gtk_spin_button_new_with_range((gdouble)lower, (gdouble)upper,
            (gdouble)step);

    va_start(args, step);
    resource = lib_mvsprintf(fmt, args);
    g_object_set_data(G_OBJECT(spin), "ResourceName", (gpointer)resource);
    va_end(args);

    return resource_spin_button_int_create_helper(spin);
}


/** \brief  Set "fake digits" for the integer spint button
 *
 * \param[in,out]   spin    integer spin button
 * \param[in]       digits  number of fake digits to display
 *
 */
void vice_gtk3_resource_spin_button_int_set_fake_digits(
        GtkWidget *spin,
        int digits)
{
    if (digits <= 0 || digits > 5) {
        return;
    }
    resource_widget_set_int(spin, "FakeDigits", digits);
    gtk_spin_button_set_digits(GTK_SPIN_BUTTON(spin), digits);
    g_signal_connect(spin, "input", G_CALLBACK(on_spin_button_input), NULL);
    g_signal_connect(spin, "output", G_CALLBACK(on_spin_button_output), NULL);
}


#if 0
/** \brief  Get "fake digits" of the integer spint button
 *
 * \param[in]   spin    integer spin button
 *
 * \return  number of fake digits
 */
int resource_spin_button_int_get_fake_digits(GtkSpinButton *spin)
{
    return GPOINTER_TO_INT(g_object_get_data(G_OBJECT(spin), "FakeDigits"));
}
#endif

/** \brief  Update spin button with \a value
 *
 * \param[in,out]   widget  integer spin button
 * \param[in]       value   new value for the spin button
 */
void vice_gtk3_resource_spin_button_int_update(GtkWidget *widget, int value)
{
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(widget), (gdouble)value);
}
