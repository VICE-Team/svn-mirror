/** \file   src/arch/gtk3/widgets/rrnetmk3widget.c
 * \brief   Widget to control RRNet MK3 resourcs
 *
 * Written by
 *  Bas Wassink <b.wassink@ziggo.nl>
 *
 * Controls the following resource(s):
 *  RRNETMK3_flashjumper
 *  RRNETMK3_bios_write
 *
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
#include <stdlib.h>

#include "machine.h"
#include "resources.h"
#include "debug_gtk3.h"
#include "basewidgets.h"
#include "widgethelpers.h"
#include "basedialogs.h"
#include "savefiledialog.h"
#include "cartridge.h"

#include "rrnetmk3widget.h"


static int (*save_func)(int, const char *) = NULL;
static int (*flush_func)(int) = NULL;


/** \brief  Handler for the "clicked" event of the "Save As" button
 *
 * \param[in]   widget      button
 * \param[in]   user_data   extra event data (unused)
 */
static void on_save_clicked(GtkWidget *widget, gpointer user_data)
{
    gchar *filename;

    filename = ui_save_file_dialog(widget, "Save image as", NULL, TRUE, NULL);
    if (filename != NULL) {
        debug_gtk3("writing RRNetMk3 image file as '%s'\n", filename);
        if (save_func(CARTRIDGE_RRNETMK3, filename) < 0) {
            /* ui_error("Failed to save RRNetMk3 image '%s'", filename); */
        }
        g_free(filename);
    }
}


/** \brief  Handler for the "clicked" event of the "Flush now" button
 *
 * \param[in]   widget      button
 * \param[in]   user_data   extra event data (unused)
 */
static void on_flush_clicked(GtkWidget *widget, gpointer user_data)
{
    debug_gtk3("flushing RRNetMk3 image\n");
    if (flush_func(CARTRIDGE_RRNETMK3) < 0) {
        /* TODO: report error */
    }
}


/** \brief  Create widget to control RRNet Mk3 resources
 *
 * \param[in]   parent  parent widget
 *
 * \return  GtkGrid
 */
GtkWidget *rrnetmk3_widget_create(GtkWidget *parent)
{
    GtkWidget *grid;
    GtkWidget *flash_jumper;
    GtkWidget *bios_write;
    GtkWidget *save_button;
    GtkWidget *flush_button;

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);

    flash_jumper = resource_check_button_create("RRNETMK3_flashjumper",
            "Enable flash jumper");
    gtk_grid_attach(GTK_GRID(grid), flash_jumper, 0, 0, 1, 1);

    /* RRBiosWrite */
    bios_write = resource_check_button_create("RRNETMK3_bios_write",
            "Write back RRNetMk3 Flash ROM image automatically");
    gtk_grid_attach(GTK_GRID(grid), bios_write, 0, 1, 1, 1);

    /* Save image as... */
    save_button = gtk_button_new_with_label("Save image as ...");
    gtk_grid_attach(GTK_GRID(grid), save_button, 1, 1, 1, 1);
    g_signal_connect(save_button, "clicked", G_CALLBACK(on_save_clicked),
            NULL);

    /* Flush image now */
    flush_button = gtk_button_new_with_label("Flush image now");
    gtk_grid_attach(GTK_GRID(grid), flush_button, 1, 2, 1, 1);
    g_signal_connect(flush_button, "clicked", G_CALLBACK(on_flush_clicked),
            NULL);

    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Set function to save RRNetMk3 image to disk
 *
 * \param[in]   func    function to save image
 */
void rrnetmk3_widget_set_save_func(int (*func)(int, const char *))
{
    save_func = func;
}


/** \brief  Set function to flush RRNetMk3 image to disk
 *
 * \param[in]   func    function to flush image
 */
void rrnetmk3_widget_set_flush_func(int (*func)(int))
{
    flush_func = func;
}
