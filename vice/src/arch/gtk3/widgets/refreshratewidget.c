/*
 * refreshratewidget.c - GTK3 refresh rate widget
 *
 * Written by
 *  Bas Wassink <b.wassink@ziggo.nl>
 *
 * Controls the following rescource(s):
 *  RefreshRate - 0 == auto, 1-10 == 1/[1-10] (int)
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
 */


#include "vice.h"

#include <gtk/gtk.h>

#include "lib.h"
#include "resources.h"
#include "vsync.h"
#include "debug_gtk3.h"
#include "widgethelpers.h"

#include "refreshratewidget.h"


/** \brief  List of text/id pairs for the refresh rates
 */
static ui_text_int_pair_t refresh_rates[] = {
    { "Automatic", 0 },
    { "1/1", 1 },
    { "1/2", 2 },
    { "1/3", 3 },
    { "1/4", 4 },
    { "1/5", 5 },
    { "1/6", 6 },
    { "1/7", 7 },
    { "1/8", 8 },
    { "1/9", 9 },
    { "1/10", 10 },
    { NULL, -1 }
};


/** \brief  Event handler for the radio buttons in the widget
 *
 * Updates the 'RefreshRate' resource
 *
 * \param[in]   widget      radio button triggering the event
 * \param[in]   user_data   new refresh rate
 */
static void refreshrate_callback(GtkWidget *widget, gpointer user_data)
{
    gint rate = GPOINTER_TO_INT(user_data);

    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
        debug_gtk3("got refresh rate %d\n", rate);
        vsync_suspend_speed_eval();
        resources_set_int("RefreshRate", rate);
    }
}


/** \brief  Create 'fresh rate' widget
 *
 * \todo    Get current refresh rate from resources and set proper radio button
 *
 * \return  GtkGrid
 */
GtkWidget *refreshrate_widget_create(void)
{
    GtkWidget *layout;

    int index;
    int value = 0;

    resources_get_int("RefreshRate", &value);

    for (index = 0; refresh_rates[index].text != NULL; index++) {
        if (refresh_rates[index].value == value) {
            break;
        }
    }

    layout = uihelpers_create_int_radiogroup_with_label("Refresh rate",
            refresh_rates, refreshrate_callback, index);

    gtk_widget_show(layout);
    return layout;
}


