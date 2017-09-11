/*
 * soundfragmentizewidget.c - GTK3 sound fragment size widget
 *
 * Written by
 *  Bas Wassink <b.wassink@ziggo.nl>
 *
 * Controls the following resource(s)"
 *  SoundFragmentSize - sound fragment size (0-5) (int)
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
#include "ui.h"
#include "resources.h"
#include "vsync.h"
#include "sound.h"
#include "widgethelpers.h"
#include "debug_gtk3.h"

#include "soundfragmentsizewidget.h"


/** \brief  Sound buffer fragment sizes table
 */
static ui_text_int_pair_t fragment_sizes[] = {
    { "Very small", 0 },
    { "Small", 1 },
    { "Medium", 2 },
    { "Large", 3 },
    { "Very large", 4 },
    { NULL, -1 }
};


/** \brief  Callback to set the new value of the "SoundFragmentSize" resource
 *
 * \param[in]   widget      radio button triggering the event
 * \param[in]   user_data   index in the radio button grid (`int`)
 */
static void on_fragment_size_changed(GtkWidget *widget, gpointer user_data)
{
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
        resources_set_int("SoundFragmentSize", GPOINTER_TO_INT(user_data));
    }
}


/** \brief  Create widget to set the "SoundFragmentSize" resource
 *
 * \return  GtkGrid
 */
GtkWidget *create_sound_fragment_size_widget(void)
{
    GtkWidget *grid;
    int size;

    resources_get_int("SoundFragmentSize", &size);

    grid = uihelpers_create_int_radiogroup_with_label(
            "Sound fragment size", fragment_sizes,
            on_fragment_size_changed, size);

    gtk_widget_show_all(grid);
    return grid;
}

