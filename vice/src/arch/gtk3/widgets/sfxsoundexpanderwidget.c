/** \file   src/arch/gtk3/widgets/sfxsoundexpanderwidget.c
 * \brief   SFX Sound Expander widget
 *
 * Written by
 *  Bas Wassink <b.wassink@ziggo.nl>
 *
 * Controls the following resource(s):
 *  SFXSoundExpander
 *  SFXSoundExpanderChip
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
#include <stdlib.h>

#include "machine.h"
#include "resources.h"
#include "debug_gtk3.h"
#include "basewidgets.h"
#include "widgethelpers.h"
#include "basedialogs.h"
#include "cartio.h"
#include "cartridge.h"

#include "sfxsoundexpanderwidget.h"


static ui_radiogroup_entry_t chip_models[] = {
    { "3526", 3526 },
    { "3812", 3812 },
    { NULL, -1 }
};


static GtkWidget *chip_group = NULL;

/** \brief  Handler for the "toggled" event of the DIGIMAX check button
 *
 * \param[in]       widget      check button
 * \param[in,out]   user_data   DIGIMAXbase combo box
 */
static void on_sfx_sound_expander_toggled(GtkWidget *widget, gpointer user_data)
{
    GtkWidget *group = GTK_WIDGET(user_data);

    gtk_widget_set_sensitive(group,
            gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)));
}


static GtkWidget *create_sfx_chip_widget(void)
{
    GtkWidget *grid;

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 16);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);

    label = gtk_label_new("YM chip model");
    g_object_set(label, "margin-left", 16, NULL);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);

    chip_group = resource_radiogroup_create("SFXSoundExpanderChip",
            chip_models, GTK_ORIENTATION_HORIZONTAL);
    gtk_grid_attach(GTK_GRID(grid), chip_group, 1, 0, 1, 1);

    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Create SFX Sound Expander widget
 *
 * \param[in]   parent  parent widget
 *
 * \return  GtkGrid
 */
GtkWidget *sfx_sound_expander_widget_create(GtkWidget *parent)
{
    GtkWidget *grid;
    GtkWidget *label;
    GtkWidget *sfx_enable;

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 16);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);

    sfx_enable = resource_check_button_create("SFXSoundExpander",
            "Enable SFX Sound Expander");
    gtk_grid_attach(GTK_GRID(grid), sfx_enable, 0, 0, 1, 1);

    gtk_grid_attach(GTK_GRID(grid), create_sfx_chip_widget(), 0, 1, 1,1);

    g_signal_connect(sfx_enable, "toggled",
            G_CALLBACK(on_sfx_sound_expander_toggled), (gpointer)chip_group);
    /* set proper state */
    on_sfx_sound_expander_toggled(sfx_enable, (gpointer)chip_group);
    gtk_widget_show_all(grid);
    return grid;
}
