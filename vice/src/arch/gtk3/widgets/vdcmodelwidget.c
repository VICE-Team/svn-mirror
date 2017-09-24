/** \file   src/arch/gtk3/widgets/vdcmodelwidget.c
 * \brief   VDC settings widget
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

#include "vdcmodelwidget.h"


static const ui_text_int_pair_t vdc_revs[] = {
    { "Revision 0", 0 },
    { "Revision 1", 1 },
    { "Revision 2", 2 },
    { NULL, -1 }
};


static int get_revision_index(int revision)
{
    int i;

    for (i = 0; vdc_revs[i].text != NULL; i++) {
        if (vdc_revs[i].value == revision) {
            return i;
        }
    }
    return -1;
}


static void on_64kb_toggled(GtkWidget *widget, gpointer user_data)
{
    int active = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));

    debug_gtk3("setting VDC64KB to %s\n", active ? "ON" : "OFF");
    resources_set_int("VDC64KB", active);
}


static void on_revision_toggled(GtkWidget *widget, gpointer user_data)
{
    int rev = GPOINTER_TO_INT(user_data);

    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
        debug_gtk3("setting VDCRevision to %d\n", rev);
        resources_set_int("VDCRevision", rev);
    }
}


static GtkWidget *create_64kb_widget(void)
{
    GtkWidget *check;
    int value;

    check = gtk_check_button_new_with_label("Enable 64KB video ram");
    resources_get_int("VDC64KB", &value);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check), value);
    g_object_set(check, "margin-left", 16, NULL);

    return check;
}


GtkWidget *vdc_model_widget_create(void)
{
    GtkWidget *grid;
    GtkWidget *radio;
    GtkRadioButton *last = NULL;
    GSList *group = NULL;
    int i;

    grid = uihelpers_create_grid_with_label("VDC settings", 1);

    gtk_grid_attach(GTK_GRID(grid), create_64kb_widget(), 0, 1, 1, 1);

    /* add VDC revisions */
    for (i = 0; vdc_revs[i].text != NULL; i++) {
        radio = gtk_radio_button_new_with_label(group, vdc_revs[i].text);
        gtk_radio_button_join_group(GTK_RADIO_BUTTON(radio), last);
        g_object_set(radio, "margin-left", 16, NULL);
        gtk_grid_attach(GTK_GRID(grid), radio, 0, i + 2, 1, 1);
        last = GTK_RADIO_BUTTON(radio);
    }

    /* set values */
    vdc_model_widget_update(grid);

    gtk_widget_show_all(grid);
    return grid;
}


void vdc_model_widget_update(GtkWidget *widget)
{
    int rev;
    int index;

    resources_get_int("VDCRevision", &rev);
    index = get_revision_index(rev);
    debug_gtk3("got VDCRevision %d\n", rev);

    if (index >= 0) {
        int i = 0;
        GtkWidget *radio;

        /* +2: skip title & 64KB checkbox */
        while ((radio = gtk_grid_get_child_at(GTK_GRID(widget), 0, i + 2)) != NULL) {
            if (GTK_IS_RADIO_BUTTON(radio)) {
                if (i == index) {
                    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio), TRUE);
                    break;
                }
            }
            i++;
        }
    }
}



void vdc_model_widget_connect_signals(GtkWidget *widget)
{
    GtkWidget *check;
    GtkWidget *radio;
    int i = 0;

    check = gtk_grid_get_child_at(GTK_GRID(widget), 0, 1);
    g_signal_connect(check, "toggled", G_CALLBACK(on_64kb_toggled), NULL);

    while ((radio = gtk_grid_get_child_at(GTK_GRID(widget), 0, i + 2)) != NULL) {
        if (GTK_IS_RADIO_BUTTON(radio)) {
            g_signal_connect(radio, "toggled", G_CALLBACK(on_revision_toggled),
                    GINT_TO_POINTER(vdc_revs[i].value));
        }
        i++;
    }
}
