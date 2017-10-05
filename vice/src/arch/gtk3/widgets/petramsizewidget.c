/** \file   src/arch/gtk3/widgets/petramsizewidget.c
 * \brief   Widget to set the PET RAM size
 *
 * Written by
 *  Bas Wassink <b.wassink@ziggo.nl>
 *
 * Controls the following resource(s):
 *  RamSize - RAM in KB (4, 8, 16, 32, 96, 128)
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

#include "widgethelpers.h"
#include "debug_gtk3.h"
#include "resources.h"

#include "petramsizewidget.h"


/** \brief  Data for the radio buttons group
 */
static ui_radiogroup_entry_t ram_sizes[] = {
    { "4KB", 4 },
    { "8KB", 8 },
    { "16KB", 16 },
    { "32KB", 32 },
    { "96KB", 96 },
    { "128KB", 128 },
    { NULL, -1 }
};


/** \brief  Look up the index of \a size int the RAM sizes array
 *
 * \param[in]   size    RAM size in KB
 *
 * \return  index in the array or -1 when \a size is not found
 */
static int get_ram_size_index(int size)
{
    return uihelpers_radiogroup_get_index(ram_sizes, size);
}


/** \brief  Handler for the "toggled" event of the radio buttons
 *
 * Sets the RamSize resource when it has been changed.
 *
 * \param[in]   widget      radio button triggering the event
 * \param[in]   user_data   value for the resource (`int`)
 */
static void on_ram_size_toggled(GtkWidget *widget, gpointer user_data)
{
    int old_val;
    int new_val;

    resources_get_int("RamSize", &old_val);
    new_val = GPOINTER_TO_INT(user_data);

    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))
            && (new_val != old_val)) {
        debug_gtk3("setting RamSize to %d\n", new_val);
        resources_set_int("RamSize", new_val);
    }
}


/** \brief  Create PET RAM size widget
 *
 * Creates a widget to control the PET's RAM size
 *
 * \return  GtkGrid
 */
GtkWidget *pet_ram_size_widget_create(void)
{
    GtkWidget *grid;
    int size;
    int index;

    resources_get_int("RamSize", &size);
    index = get_ram_size_index(size);

    grid = uihelpers_radiogroup_create(
            "Memory size",
            ram_sizes,
            on_ram_size_toggled,
            index);
    gtk_widget_show_all(grid);
    return grid;
}
