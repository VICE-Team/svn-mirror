/** \file   src/arch/gtk3/widgets/base/resourcescale.c
 * \brief   Scale to control an integer resource
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

#include "resourcescale.h"


/** \brief  Handler for the "destroy" event of the \a scale widget
 *
 * Frees memory used by the copy of the resource name
 *
 * \param[in]   scale       integer scale widget
 * \param[in]   user_data   extra event data (unused)
 */
static void on_scale_int_destroy(GtkScale *scale, gpointer user_data)
{
    char *resource;

    resource = (char*)g_object_get_data(G_OBJECT(scale), "ResourceName");
    lib_free(resource);
}


/** \brief  Handler for the "value-changed" event of the \a scale widget
 *
 * Updates resource value
 *
 * \param[in]   scale       integer scale widget
 * \param[in]   user_data   extra event data (unused)
 */
static void on_scale_int_changed(GtkScale *scale, gpointer user_data)
{
    const char *resource;
    int old_val;
    int new_val;

    resource = (const char*)g_object_get_data(G_OBJECT(scale), "ResourceName");
    resources_get_int(resource, &old_val);
    new_val = (int)gtk_range_get_value(GTK_RANGE(scale));
    /* only update resource when required */
    if (old_val != new_val) {
            debug_gtk3("setting %s to %d\n", resource, new_val);
        resources_set_int(resource, new_val);
    }
}


/** \brief  Create a scale for an integer resource
 *
 * \param[in]   resource    resource name
 * \param[in]   orientation scale orientation (\see GtkOrientation)
 * \param[in]   low         lowest value for scale
 * \param[in]   high        highest value for scale
 * \param[in]   step        value to incr/decr value with cursor keys
 *
 * \return  GtkScale
 */
GtkWidget *resource_scale_int_create(const char *resource,
                                     GtkOrientation orientation,
                                     int low, int high, int step)
{
    GtkWidget *scale;
    int value;

    scale = gtk_scale_new_with_range(orientation,
            (gdouble)low, (gdouble)high, (gdouble)step);
    gtk_scale_set_digits(GTK_SCALE(scale), 0);

    /* store copy of resource name */
    g_object_set_data(G_OBJECT(scale), "ResourceName",
            (gpointer)lib_stralloc(resource));

    /* set current value */
    resources_get_int(resource, &value);
    gtk_range_set_value(GTK_RANGE(scale), (gdouble)value);

    g_signal_connect(scale, "value-changed", G_CALLBACK(on_scale_int_changed),
            NULL);
    g_signal_connect(scale, "destroy", G_CALLBACK(on_scale_int_destroy), NULL);

    gtk_widget_show(scale);
    return scale;
}


/** \brief  Add marks to integer \a scale widget at each \a step increment
 *
 * \param[in,out]   scale   integer scale widget
 * \param[in]       step    distance between marks
 */
void resource_scale_int_set_marks(GtkWidget *scale, int step)
{
    GtkAdjustment *adj;
    int lower;
    int upper;
    int i;

    adj = gtk_range_get_adjustment(GTK_RANGE(scale));
    lower = (int)gtk_adjustment_get_lower(adj);
    upper = (int)gtk_adjustment_get_upper(adj);

    for (i = lower; i <= upper; i += step) {
        gtk_scale_add_mark(GTK_SCALE(scale), (gdouble)i, GTK_POS_BOTTOM, NULL);
    }
}


/** \brief  Update the \a scale widget with a new \a value
 *
 * \param[in,out]   scale   integer scale widget
 * \param[in]       value   new value for \a scale
 */
void resource_scale_int_update(GtkWidget *scale, int value)
{
    gtk_range_set_value(GTK_RANGE(scale), (gdouble)value);
}
