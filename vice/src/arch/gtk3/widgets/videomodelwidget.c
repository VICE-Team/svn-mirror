/** \file   src/arch/gtk3/widgets/videomodelwidget.c
 * \brief   Video chip model selection widget
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

#include "widgethelpers.h"
#include "debug_gtk3.h"
#include "resources.h"
#include "machine.h"

#include "videomodelwidget.h"


static const char *widget_title = NULL;
static const char *resource_name = NULL;
static const ui_text_int_pair_t *model_list = NULL;


/** \brief  Get index in model list for model-ID \a model
 *
 * \return  index in list or -1 when not found
 */
static int get_model_index(int model)
{
    int i;

    for (i = 0; model_list[i].text != NULL; i++) {
        if (model_list[i].value == model) {
            return i;
        }
    }
    return -1;
}


static void on_model_toggled(GtkWidget *widget, gpointer user_data)
{
    int model_id = GPOINTER_TO_INT(user_data);

    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
        debug_gtk3("setting %s to %d\n", resource_name, model_id);
        resources_set_int(resource_name, model_id);
    }
}

void video_model_widget_set_title(const char *title)
{
    widget_title = title;
}

void video_model_widget_set_resource(const char *resource)
{
    resource_name = resource;
}


void video_model_widget_set_models(const ui_text_int_pair_t *models)
{
    model_list = models;
}


GtkWidget *create_video_model_widget(void)
{
    GtkWidget *grid;
    GtkWidget *radio;
    GtkRadioButton *last = NULL;
    GSList *group = NULL;
    int i;

    grid = uihelpers_create_grid_with_label(widget_title, 1);

    if (model_list != NULL) {
        for (i = 0; model_list[i].text != NULL; i++) {
            radio = gtk_radio_button_new_with_label(group, model_list[i].text);
            gtk_radio_button_join_group(GTK_RADIO_BUTTON(radio), last);
            g_object_set(radio, "margin-left", 16, NULL);
            gtk_grid_attach(GTK_GRID(grid), radio, 0, i + 1, 1, 1);
            last = GTK_RADIO_BUTTON(radio);
        }

        /* now set the proper value */
        update_video_model_widget(grid);
    }

    gtk_widget_show_all(grid);
    return grid;
}



void update_video_model_widget(GtkWidget *widget)
{
    GtkWidget *radio;
    int index;
    int model_id;
    int i = 0;

    resources_get_int(resource_name, &model_id);
    index = get_model_index(model_id);
    debug_gtk3("got resource %d, index %d\n", model_id, index);

    while ((radio = gtk_grid_get_child_at(GTK_GRID(widget), 0, i + 1)) != NULL) {
        if (GTK_IS_RADIO_BUTTON(radio) && (index == i)) {
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio), TRUE);
            break;
        }
        i++;
    }
}


void connect_video_model_widget_signals(GtkWidget *widget)
{
    GtkWidget *radio;
    int i = 0;

    while ((radio = gtk_grid_get_child_at(GTK_GRID(widget), 0, i + 1)) != NULL) {
        if (GTK_IS_RADIO_BUTTON(radio)) {
            g_signal_connect(radio, "toggled", G_CALLBACK(on_model_toggled),
                    GINT_TO_POINTER(model_list[i].value));
        } else {
            break;
        }
        i++;
    }
}
