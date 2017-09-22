/** \file   src/arch/gtk3/widgets/machinemodelwidget.c
 * \brief   Machine model selection widget
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
/* nasty, I know, but I need get around the link-everything-into massive blobs
 * and not having a VICE_MACHINE constant */
#include "ui.h"

#include "machinemodelwidget.h"


static int  (*model_get)(void) = NULL;
static void (*model_set)(int) = NULL;



static const char *c64_model_list[] = {
    "C64 PAL", "C64C PAL", "C64 old PAL", "C64 NTSC", "C64C NTSC",
    "C64 old NTSC", "Drean" "C64 SX PAL", "C64 SX NTSC", "Japanese", "C64 GS",
    "PET64 PAL", "PET64 NTSC", "Ultimax", NULL };


static const char *c128_model_list[] = {
    "C128 PAL", "C128D PAL", "C128 NTSC", "C128D NTSC", NULL
};


static const char **get_model_list(void)
{
    switch (machine_class) {
        case VICE_MACHINE_C64:  /* fall through */
        case VICE_MACHINE_C64SC:
            return c64_model_list;
        case VICE_MACHINE_C128:
            return c128_model_list;
        default:
            return NULL;
    }
}


void machine_model_widget_getter(int (*f)(void)) {
    model_get = f;
}

void machine_model_widget_setter(void (*f)(int model))
{
    model_set = f;
}


GtkWidget *create_machine_model_widget(void)
{
    GtkWidget *grid;
    GtkRadioButton *last;
    GSList *group;
    const char **list;
    int i;

    grid = uihelpers_create_grid_with_label("Model", 1);
    list = get_model_list();
    if (list != NULL) {
        last = NULL;
        group = NULL;
        for (i = 0; list[i] != NULL; i++) {
            GtkWidget *radio = gtk_radio_button_new_with_label(group, list[i]);
            gtk_radio_button_join_group(GTK_RADIO_BUTTON(radio), last);
            g_object_set(radio, "margin-left", 16, NULL);
            gtk_grid_attach(GTK_GRID(grid), radio, 0, i + 1, 1, 1);
            last = GTK_RADIO_BUTTON(radio);
        }

        update_machine_model_widget(grid);
    }
    gtk_widget_show_all(grid);
    return grid;
}


void update_machine_model_widget(GtkWidget *widget)
{
    int model;

    if (model_get != NULL) {
        model = model_get();
    } else {
        model = -1;
    }

    if (model < 0) {
        /* TODO: make all radio buttons 'empty'/unselected */
        return;
    }

    GtkWidget *radio = gtk_grid_get_child_at(GTK_GRID(widget), 0, model + 1);
    if (radio != NULL && GTK_IS_RADIO_BUTTON(radio)) {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio), TRUE);
    }
}

