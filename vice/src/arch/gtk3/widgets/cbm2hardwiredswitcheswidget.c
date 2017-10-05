/** \file   src/arch/gtk3/widgets/cbm2hardwareswitchesmodel.c
 * \brief   Widget to select ....
 *
 * Written by
 *  Bas Wassink <b.wassink@ziggo.nl>
 *
 * Controls the following resource(s):
 *  ModelLine
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
#include <glib/gstdio.h>

#include "lib.h"
#include "widgethelpers.h"
#include "debug_gtk3.h"
#include "resources.h"
#include "machine.h"

#include "cbm2hardwiredswitcheswidget.h"


static ui_radiogroup_entry_t models_cbm5x0[] = {
    { "50Hz 5x0", 2 },
    { "60Hz 5x0", 1 },
    { NULL, -1 }
};


static ui_radiogroup_entry_t models_cbm6x0[] = {
    { "50Hz 6x0", 2 },
    { "60Hz 6x0", 1 },
    { "50Hz 7x0", 0 },
    { NULL, -1 }
};


static int get_model_index(ui_radiogroup_entry_t *list, int model)
{
    return uihelpers_radiogroup_get_index(list, model);
}


static void on_model_toggled(GtkWidget *widget, gpointer user_data)
{
    int old_val;
    int new_val;

    resources_get_int("ModelLine", &old_val);
    new_val = GPOINTER_TO_INT(user_data);

    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))
            && new_val != old_val) {
        debug_gtk3("setting ModelLine to %d\n", new_val);
        resources_set_int("ModelLine", new_val);
    }
}



GtkWidget *cbm2_hardwired_switches_widget_create(void)
{
    GtkWidget *grid;
    int model;
    int index;
    ui_radiogroup_entry_t *list;

    resources_get_int("ModelLine", &model);
    if (machine_class == VICE_MACHINE_CBM5x0) {
        list = models_cbm5x0;
    } else {
        list = models_cbm6x0;
    }
    index = get_model_index(list, model);


    grid = uihelpers_radiogroup_create("Hardwired switches",
            list, on_model_toggled, index);

    gtk_widget_show_all(grid);
    return grid;
}
