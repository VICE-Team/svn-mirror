/** \file   src/arch/gtk3/widgets/base/resourcespinbutton.c
 * \brief   Spin buttons to control resources
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

#include "basewidget_types.h"
#include "debug_gtk3.h"
#include "lib.h"
#include "resources.h"

#include "resourcespinbutton.h"


/** \brief  Handler for the "destroy" event of \a widget
 *
 * Frees memory used by the copy of the resource name.
 *
 * \param[in]   widget      integer spin button
 * \param[in]   user_data   extra event data (unused)
 */
static void on_spin_button_destroy(GtkWidget *widget, gpointer user_data)
{
    char *res = (char *)g_object_get_data(G_OBJECT(widget), "ResourceName");
    lib_free(res);
}


/** \brief  Handler for the "value-changed" event of \a widget
 *
 * Updates the resource with the current value of the spin buttton
 *
 * \param[in]   widget      integer spin button
 * \param[in]   user_data   extra event data (unused)
 */
static void on_spin_button_value_changed(GtkWidget *widget, gpointer user_data)
{
    const char *res;
    int value;

    res = (const char *)g_object_get_data(G_OBJECT(widget), "ResourceName");
    value = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));
    debug_gtk3("setting %s to %d\n", res, value);
    resources_set_int(res, value);
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
GtkWidget *resource_spin_button_int_create(const char *resource,
                                           int lower, int upper, int step)
{
    GtkWidget *spin;
    int current;

    spin = gtk_spin_button_new_with_range((gdouble)lower, (gdouble)upper,
            (gdouble)step);

    /* set a copy of the resource name */
    g_object_set_data(G_OBJECT(spin), "ResourceName",
            (gpointer)lib_stralloc(resource));

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


/** \brief  Set "fake digits" for the integer spint button
 *
 * \param[in,out]   widget  integer spin button
 * \param[in]       digits  number of fake digits to display
 *
 */
void resource_spin_button_int_set_fake_digits(GtkWidget *widget, int digits)
{
    if (digits < 0) {
        digits = 0;
    }
    g_object_set_data(G_OBJECT(widget), "FakeDigits", GINT_TO_POINTER(digits));
    gtk_spin_button_set_digits(GTK_SPIN_BUTTON(widget), digits);
}


/** \brief  Get "fake digits" of the integer spint button
 *
 * \param[in]   widget  integer spin button
 *
 * \return  number of fake digits
 */
int resource_spin_button_int_get_fake_digits(GtkWidget *widget)
{
    return GPOINTER_TO_INT(g_object_get_data(G_OBJECT(widget), "FakeDigits"));
}


/** \brief  Update spin button with \a value
 *
 * \param[in,out]   widget  integer spin button
 * \param[in]       value   new value for the spin button
 */
void resource_spin_button_int_update(GtkWidget *widget, int value)
{
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(widget), (gdouble)value);
}
