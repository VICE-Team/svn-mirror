/** \file   petreuwidget.c
 * \brief   PET RAM expansion module widget
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 */

/*
 * $VICERES PETREU          xpet
 * $VICERES PETREUsize      xpet
 * $VICERES PETREUfilename  xpet
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

#include "basedialogs.h"
#include "basewidgets.h"
#include "debug_gtk3.h"
#include "machine.h"
#include "openfiledialog.h"
#include "resources.h"
#include "ui.h"
#include "widgethelpers.h"

#include "petreuwidget.h"


/** \brief  List of REU sizes
 */
static const vice_gtk3_radiogroup_entry_t reu_sizes[] = {
    { "128KiB",     128 },
    { "512KiB",     512 },
    { "1024KiB",    1024 },
    { "2048KiB",    2048 },
    { NULL,         -1 }
};


/* references to widgets to be able to toggle sensitive state depending on the
 * REU Enable check button
 */

/** \brief  REU file browser */
static GtkWidget *browser = NULL;

/** \brief  REU size radio group */
static GtkWidget *group = NULL;


/** \brief  Handler for the "toggled" event of the REU Enable check button
 *
 * Toggles sensitive state of other widgets
 *
 * \param[in]   widget      REU check button
 * \param[in]   user_data   extra event data (unused)
 */
static void on_reu_toggled(GtkWidget *widget, gpointer user_data)
{
    int state = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));

    gtk_widget_set_sensitive(browser, state);
    gtk_widget_set_sensitive(group, state);
}


/** \brief  Create widget to control PET REU settings
 *
 * \param[in]   parent  parent widget
 *
 * \return  GtkGrid
 */
GtkWidget *pet_reu_widget_create(GtkWidget *parent)
{
    GtkWidget *grid;
    GtkWidget *enable;
    GtkWidget *label;

    grid = vice_gtk3_grid_new_spaced(VICE_GTK3_DEFAULT, VICE_GTK3_DEFAULT);

    /* REU enable */
    enable = vice_gtk3_resource_check_button_new("PETREU",
            "Enable PET RAM Expansion Unit");
    g_signal_connect(enable, "toggled", G_CALLBACK(on_reu_toggled), NULL);
    gtk_grid_attach(GTK_GRID(grid), enable, 0, 0, 3, 1);

    /* REU size */
    label = gtk_label_new("REU size");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    g_object_set(label, "margin-left", 16, NULL);
    group = vice_gtk3_resource_radiogroup_new("PETREUsize", reu_sizes,
            GTK_ORIENTATION_HORIZONTAL);
    gtk_grid_set_column_spacing(GTK_GRID(group), 16);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), group, 1, 1, 2, 1);

    /* REU filename */

    /* Don't use the resourcebrowser widget's label, but a separate one to
     * properly align with the previous widgets */
    label = gtk_label_new("REU image file");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    g_object_set(label, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 2, 1, 1);

    browser = vice_gtk3_resource_browser_new(
            "PETREUfilename",
            NULL,
            NULL,
            "Select REU image file",
            NULL,
            NULL);
    gtk_grid_attach(GTK_GRID(grid), browser, 1, 2, 2, 1);

    /* set initial sensitive state of widgets */
    on_reu_toggled(enable, NULL);

    gtk_widget_show_all(grid);
    return grid;
}
