/** \file   src/arch/gtk3/widgets/resourcecheckbutton.c
 * \brief   Check button connected to a resource
 *
 * Written by
 *  Bas Wassink <b.wassink@ziggo.nl>
 *
 * Controls the following resource(s):
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

#include "debug_gtk3.h"
#include "resources.h"

#include "resourcecheckbutton.h"



/** \brief  Handler for the "toggled" event of the check button
 *
 * \param[in]   check       check button
 * \param[in]   user_data   resource name
 */
static void on_check_button_toggled(GtkWidget *check, gpointer user_data)
{
    const char *resource = (const char *)user_data;
    int state = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check));

    debug_gtk3("setting %s to %s\n", resource, state ? "ON": "OFF");
    resources_set_int(resource, state ? 1 : 0);
}


/** \brief  Create check button to toggle \a resource
 *
 * \param[in]   resource    resource name
 * \param[in]   label       label of the check button
 *
 * \note    Sets the "ResourceName" property on the button for convenience
 *
 * \return  new check button
 */
GtkWidget *resource_check_button_create(const char *resource,
                                        const char *label)
{
    GtkWidget *check;
    int state;

    /* get current resource value */
    resources_get_int(resource, &state);

    check = gtk_check_button_new_with_label(label);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check),
            state ? TRUE : FALSE);
    /* store resource name as an extra property */
    g_object_set_data(G_OBJECT(check), "ResourceName", (gpointer)resource);

    g_signal_connect(check, "toggled", G_CALLBACK(on_check_button_toggled),
            (gpointer)resource);

    gtk_widget_show(check);
    return check;
}
