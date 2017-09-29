/** \file   src/arch/gtk3/widgets/sidmodelwidget.c
 * \brief   Widget to set the SID model
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
#include <glib/gstdio.h>

#include "lib.h"
#include "widgethelpers.h"
#include "debug_gtk3.h"
#include "resources.h"
#include "machine.h"
#include "machinemodelwidget.h"

#include "sidmodelwidget.h"


static ui_text_int_pair_t sid_models_all[] = {
    { "6581", 0 },
    { "8580", 1 },
    { "8580D", 2 },
    { "65181R4", 3 },
    { "DTVSID", 4 },
    { NULL, -1 }
};


static ui_text_int_pair_t sid_models_c64[] = {
    { "6581", 0 },
    { "8580", 1 },
    { "8580D", 2 },
    { NULL, -1 }
};


static ui_text_int_pair_t sid_models_cbm5x0[] = {
    { "6581", 0 },
    { NULL, -1 }
};


/** \brief  Reference to the machine model widget
 *
 * Used to update the widget when the SID model changes
 */
static GtkWidget *machine_widget = NULL;


static void on_sid_model_toggled(GtkWidget *widget, gpointer user_data)
{
    int old_model = 0;
    int new_model = 0;

    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
        resources_get_int("SidModel", &old_model);
        new_model = GPOINTER_TO_INT(user_data);
        if (old_model != new_model) {
            resources_set_int("SidModel", new_model);

            /* now trigger `model` update */
            if (machine_widget != NULL) {
                /* this causes the 'machine model' widget to be updated: it
                 * will inspect various resources to determine a valid model.
                 * if invalid the 'unknown' radio button will be activated
                 */
                update_machine_model_widget(machine_widget);
            }
        }
    }
}


/** \brief  Create SID model widget
 *
 * Creates a SID model widget, depending on `machine_class`. Also sets a
 * callback to force an update of the 'machine model' widget.
 *
 * \param[in,out]   machine_model_widget    reference to machine model widget
 *
 * \return  GtkGrid
 */
GtkWidget *sid_model_widget_create(GtkWidget *machine_model_widget)
{
    GtkWidget *grid;
    int current_model;
    ui_text_int_pair_t *models;

    machine_widget = machine_model_widget;

    resources_get_int("SidModel", &current_model);

    switch (machine_class) {

        case VICE_MACHINE_C64:      /* fall through */
        case VICE_MACHINE_C64SC:    /* fall through */
        case VICE_MACHINE_SCPU64:   /* fall through */
        case VICE_MACHINE_C128:     /* fall through */
        case VICE_MACHINE_VSID:
            models = sid_models_c64;
            break;

        case VICE_MACHINE_CBM5x0:
        case VICE_MACHINE_CBM6x0:
            models = sid_models_cbm5x0;
            break;

        case VICE_MACHINE_PLUS4:
        case VICE_MACHINE_PET:
            models = sid_models_c64;
            break;


        default:
            /* DTV probably */
            models = sid_models_all;
    }


    grid = uihelpers_create_int_radiogroup_with_label("SID model",
            models,
            on_sid_model_toggled,
            current_model);

    /* does the Plus4 or PET actually have a SidCart? */
    if (machine_class == VICE_MACHINE_PLUS4
            || machine_class == VICE_MACHINE_PET) {
        int sidcart;

        resources_get_int("SidCart", &sidcart);
        gtk_widget_set_sensitive(grid, sidcart);
    }

    return grid;
}
