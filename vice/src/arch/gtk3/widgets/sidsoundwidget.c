/** \file   src/arch/gtk3/widgets/sidsoundwidget.c
 * \brief   Settings for SID emulation
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
 */


#include "vice.h"

#include <gtk/gtk.h>

#include "lib.h"
#include "resources.h"
#include "basewidgets.h"
#include "debug_gtk3.h"

#include "sidsoundwidget.h"


static ui_radiogroup_entry_t sid_engines[] = {
    { "FastSID", 0 },
    { "ReSID", 1 },
    { "Catweasel MKIII", 2 },
    { "HardSID", 3 },
    { "ParSID port 1", 4 },
    { "ParSID port 2", 5 },
    { "ParSID port 3", 6 },
    { NULL, -1 }
};


static GtkWidget *create_sid_engine_widget(void)
{
    GtkWidget *grid;
    GtkWidget *label;
    GtkWidget *radio_group;

    grid = gtk_grid_new();
    g_object_set(grid, "margin-left", 8, NULL);

    label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(label), "<b>SID Engine</b>");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    g_object_set(label, "margin-bottom", 8, NULL);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);

    radio_group = resource_radiogroup_create("SidEngine", sid_engines,
            GTK_ORIENTATION_VERTICAL);
    g_object_set(radio_group, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), radio_group, 0, 1, 1, 1);

    gtk_widget_show_all(grid);
    return grid;
}



GtkWidget *sid_sound_widget_create(GtkWidget *parent)
{
    GtkWidget *layout;
    GtkWidget *label;
    GtkWidget *engine;

    layout = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(layout), 8);
    gtk_grid_set_row_spacing(GTK_GRID(layout), 8);

    label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(label), "<b>SID settings</b>");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(layout), label, 0, 0, 3, 1);

    engine = create_sid_engine_widget();
    gtk_grid_attach(GTK_GRID(layout), engine, 0, 1, 1,1);

    gtk_widget_show_all(layout);
    return layout;
}
