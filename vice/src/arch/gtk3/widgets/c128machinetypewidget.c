/** \file   src/arch/gtk3/widgets/c128machinettypewidget.c
 * \brief   C128 machine type widget
 *
 * Written by
 *  Bas Wassink <b.wassink@ziggo.nl>
 *   MachineType
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

#include "widgethelpers.h"
#include "debug_gtk3.h"
#include "resources.h"
#include "c128.h"

#include "c128machinetypewidget.h"


static ui_radiogroup_entry_t machine_types[] = {
    { "International",  C128_MACHINE_INT },
    /* any of these cause a KERNAL corrupted message from c128mem */
    { "Finish",         C128_MACHINE_FINNISH },
    { "French",         C128_MACHINE_FRENCH },
    { "German",         C128_MACHINE_GERMAN },
    { "Italian",        C128_MACHINE_ITALIAN },
    { "Norwegian",      C128_MACHINE_NORWEGIAN },
    { "Swedish",        C128_MACHINE_SWEDISH },
    { "Swiss",          C128_MACHINE_SWISS },
    { NULL, -1 }
};


/** \brief  Get index of machine \a type in array
 *
 * Right now the C128 machine type are just like an enum (0..n), but if those
 * defines change, this widget will still work
 *
 * \param[in]   type    C128 machine type (\see src/c128/c128.h)
 *
 * \return  index in array or -1 when not found
 */
static int get_machine_type_index(int type)
{
    return uihelpers_radiogroup_get_index(machine_types, type);
}


/** \brief  Handler for the "toggled" event of the radio buttons
 *
 * \param[in]   widget      radio button triggering the event
 * \param[in]   user_data   machine type ID (`int`)
 */
static void on_machine_type_toggled(GtkWidget *widget, gpointer user_data)
{
    int old_type;
    int new_type;

    resources_get_int("MachineType", &old_type);
    new_type = GPOINTER_TO_INT(user_data);

    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))
            && old_type != new_type) {
        /* update resource */
        debug_gtk3("setting MachineType to %d\n", new_type);
        resources_set_int("MachineType", new_type);
    }
}


/** \brief  Create C128 machine type widget
 *
 * \return  GtkGrid
 */
GtkWidget * c128_machine_type_widget_create(void)
{
    GtkWidget *grid;
    int type;
    int index;

    resources_get_int("MachineType", &type);
    index = get_machine_type_index(type);

    grid = uihelpers_radiogroup_create("Machine type",
            machine_types,
            on_machine_type_toggled,
            index);
    gtk_widget_show_all(grid);
    return grid;
}
