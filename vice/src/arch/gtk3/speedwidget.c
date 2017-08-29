/*
 * speedwidget.c - GTK3 speed control widget
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
 */


#include "vice.h"

#include <gtk/gtk.h>

#include "lib.h"
#include "resources.h"
#include "vsync.h"
#include "uihelpers.h"

#include "speedwidget.h"


/** \brief  List of text/id pairs for the refresh rates
 */
static ui_text_int_pair_t speed_rates[] = {
    { "Unlimited", 0 }, /* this one works a little weird, probably wrong value */
    { "200%", 200 },
    { "150%", 150 },
    { "100%", 100 },
    { "50%", 50 },
    { "20%", 20 },
    { "10%", 10 },
    { NULL, -1 }
};


/** \brief  Event handler to alter the "Speed" resource
 *
 * \param[in]   widget      widget triggering the callback
 * \param[in]   user_data   the speed setting (`int`)
 *
 */
static void speed_callback(GtkWidget *widget, gpointer user_data)
{
    gint speed = GPOINTER_TO_INT(user_data);

    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
#ifdef HAVE_DEBUG_GTK3UI
        g_print("[debug-gtk3ui] %s(): got speed %d%%\n", __func__, speed);
#endif
        vsync_suspend_speed_eval();
        resources_set_int("Speed", speed);
    }
}


#if 0
static GtkWidget *create_custom_refresh_widget(void)
{
}
#endif


/** \brief  Create 'fresh rate' widget
 *
 * \todo    Get current refresh rate from resources and set proper radio button
 *
 * \return  GtkWidget
 */
GtkWidget *create_speed_widget(void)
{
    GtkWidget *layout;

    layout = uihelpers_create_int_radiogroup_with_label("Speed",
            speed_rates, speed_callback);

    gtk_widget_show(layout);
    return layout;
}


