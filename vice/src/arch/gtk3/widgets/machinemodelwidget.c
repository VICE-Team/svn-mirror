/** \file   machinemodelwidget.c
 * \brief   Machine model selection widget
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 */

/*
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
#include "machine.h"
#include "resources.h"
#include "widgethelpers.h"

#include "machinemodelwidget.h"


/** \brief  Machine-specific model get function */
static int  (*model_get)(void) = NULL;

/** \brief  Machine-specific model set function */
static void (*model_set)(int) = NULL;

/** \brief  Machine-specific List of supported models */
static const char **model_list = NULL;

/** \brief  Extra callback
 *
 * This callback is called with the new model ID when the model changes
 */
static void (*user_callback)(int) = NULL;


/** \brief  Handler for 'toggled' events of the radio buttons in the widget
 *
 * \param[in]   widget      radio button triggering the event
 * \param[in]   user_data   model ID (int)
 */
static void on_model_toggled(GtkWidget *widget, gpointer user_data)
{
    int model = GPOINTER_TO_INT(user_data);

#ifdef HAVE_DEBUG_GTK3UI
    if (machine_class == VICE_MACHINE_C128) {
        int res_board_type = -1;
        int res_vdc_revision = -1;
        int res_vdc_64kb = -1;
        int res_machine_type = -1;
        int res_video_standard = -1;
        int res_cia1 = -1;
        int res_cia2 = -1;
        int res_sid = -1;

        debug_gtk3("Got model change for C128: %d.", model);

        printf("=== %s ===\n", __func__);
        resources_get_int("BoardType",      &res_board_type);
        resources_get_int("VDCRevision",    &res_vdc_revision);
        resources_get_int("VDC64KB",        &res_vdc_64kb);
        resources_get_int("MachineType",    &res_machine_type);
        resources_get_int("MachineVideoStandard",    &res_video_standard);
        resources_get_int("CIA1Model",      &res_cia1);
        resources_get_int("CIA2Model",      &res_cia2);
        resources_get_int("SIDModel",       &res_sid);

        printf("    BoardType             : %d\n", res_board_type);
        printf("    VDCRevision           : %d\n", res_vdc_revision);
        printf("    VDC64KB               : %d\n", res_vdc_64kb);
        printf("    MachineType           : %d\n", res_machine_type);
        printf("    MachineVideoStandard: : %d\n", res_video_standard);
        printf("    CIA1                  : %d\n", res_cia1);
        printf("    CIA2                  : %d\n", res_cia2);
        printf("    SIDModel              : %d\n", res_sid);
    }
#endif


    if (model_set != NULL &&
            gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
        model_set(model);
        if (user_callback != NULL) {
            user_callback(model);
        }
    }
}


/** \brief  Set machine-specific function to get the model
 *
 * \param[in]   f   model getter function
 */
void machine_model_widget_getter(int (*f)(void)) {
    model_get = f;
}


/** \brief  Set machine-specific function to set the model
 *
 * \param[in]   f   model setter function
 */
void machine_model_widget_setter(void (*f)(int model))
{
    model_set = f;
}


/** \brief  Set machine-specific list of supported models
 *
 * \param[in]   list    list of models, NULL-terminatd
 */
void machine_model_widget_set_models(const char **list)
{
    model_list = list;
}


/** \brief  Create machine model widget
 *
 * Radio buttons for the models start at row 2 in the grid, while a special
 * 'Unknown' radio button is added at row 1, to allow displaying that no valid
 * model could be found for the current settings.
 *
 * \return  GtkGrid
 */
GtkWidget *machine_model_widget_create(void)
{
    GtkWidget *grid;
    GtkWidget *radio;
    GtkRadioButton *last;
    GSList *group;
    const char **list;
    int i;

    grid = uihelpers_create_grid_with_label("Model", 1);

    /* add 'unknown' model radio */
    group = NULL;
    radio = gtk_radio_button_new_with_label(group, "Unknown");
    g_object_set(radio, "margin-left", 16, NULL);
    gtk_widget_set_sensitive(radio, FALSE);
    gtk_grid_attach(GTK_GRID(grid), radio, 0, 1, 1, 1);

    last = GTK_RADIO_BUTTON(radio);
    list = model_list;
    if (list != NULL) {
        for (i = 0; list[i] != NULL; i++) {
            radio = gtk_radio_button_new_with_label(group, list[i]);
            gtk_radio_button_join_group(GTK_RADIO_BUTTON(radio), last);
            g_object_set(radio, "margin-left", 16, NULL);
            gtk_grid_attach(GTK_GRID(grid), radio, 0, i + 2, 1, 1);
            last = GTK_RADIO_BUTTON(radio);
        }

        machine_model_widget_update(grid);
    }
    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Update the machine model widget
 *
 * \param[in,out]   widget  machine model widget
 */
void machine_model_widget_update(GtkWidget *widget)
{
    GtkWidget *radio;
    int model = 99;

    if (model_get != NULL) {
        model = model_get();
        if (model < 0) {
            /* error retrieving resources */
            model = 99;
        }
    }
    if (machine_class == VICE_MACHINE_CBM6x0) {
        if (model != 99) {
            model -= 2; /*adjust since cbm2/cbm5 share defines */
        }
    }
#if 0
    debug_gtk3("model ID = %d.", model);
#endif
    if (model == 99) {
        /* invalid model, make all radio buttons unselected
         *
         * XXX: doesn't appear to actually work on my box, so perhaps an
         *      'uknown' radio button should be added, but then I'd have to
         *      guard against the user selecting that one
         */
        radio = gtk_grid_get_child_at(GTK_GRID(widget), 0, 1);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio), TRUE);
        return;
    }

    radio = gtk_grid_get_child_at(GTK_GRID(widget), 0, model + 2);
    if (radio != NULL && GTK_IS_RADIO_BUTTON(radio)) {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio), TRUE);
    }
}


/** \brief  Connect signal handlers
 *
 * \param[in,out]   widget  machine model widget
 */
void machine_model_widget_connect_signals(GtkWidget *widget)
{
    size_t i = 0;

    while (1) {
        GtkWidget *radio = gtk_grid_get_child_at(GTK_GRID(widget), 0, i + 2);
        int value;
        if (radio == NULL) {
            break;
        }

        /* xcbm2 (CMB6x0) and xcbm5x0 (CBM5x0) and  use the same 'enum', with
         * the first two values reserved for CBM5x0, and the others for CBM6x0
         */
        if (machine_class == VICE_MACHINE_CBM6x0) {
            value = i + 2;
        } else {
            value = i;
        }
        g_signal_connect(radio, "toggled", G_CALLBACK(on_model_toggled),
                GINT_TO_POINTER(value));

        i++;
    }
}


/** \brief  Set function to call when the model ID changes
 *
 * \param[in]   callback    function to call on model change
 */
void machine_model_widget_set_callback(void (*callback)(int))
{
    user_callback = callback;
}
