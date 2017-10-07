/** \file   src/arch/gtk3/widgets/cbm2memorysizewidget.c
 * \brief   CBM-II memory size widget
 *
 * Controls the following resource(s):
 *  RamSize     - installed RAM in KB
 *
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

#include "widgethelpers.h"
#include "debug_gtk3.h"
#include "resources.h"

#include "cbm2memorysizewidget.h"


/** \brief  List of RAM sizes
 */
static ui_radiogroup_entry_t ram_sizes[] = {
    { "64KB",   64 },
    { "128KB",  128 },
    { "256KB",  256 },
    { "512KB",  512 },
    { "1024KB", 1024 },
    { NULL, -1 }
};


/** \brief  Find index in ram sizes array of \a size
 *
 * \param[in]   size    RAM size in KB
 *
 * \return  index in array or -1 when not found
 */
static int get_ram_size_index(int size)
{
    int i;

    for (i = 0; ram_sizes[i].text != NULL; i++) {
        if (ram_sizes[i].value == size) {
            return i;
        }
    }
    return -1;
}


/** \brief  Handler for the "toggled" event of the radio buttons
 *
 * \param[in]   widget      radio button
 * \param[in]   user_data   RAM size in KB (`int`)
 */
static void on_ram_size_toggled(GtkWidget *widget, gpointer user_data)
{
    int old_val;
    int new_val;

    resources_get_int("RamSize", &old_val);
    new_val = GPOINTER_TO_INT(user_data);

    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))
            && new_val != old_val) {
        debug_gtk3("setting RamSize to %dKB\n", new_val);
        resources_set_int("RamSize", new_val);
    }
}


/** \brief  Create CBM-II memory size widget
 *
 * \return  GtkGrid
 */
GtkWidget *cbm2_memory_size_widget_create(void)
{
    GtkWidget *grid;
    int size = 0;
    int index = 0;

    resources_get_int("RamSize", &size);
    index = get_ram_size_index(size);

    grid = uihelpers_radiogroup_create("RAM size",
            ram_sizes, on_ram_size_toggled, index);

    gtk_widget_show_all(grid);
    return grid;
}



