/** \file   ramlinkwidget.c
 * \brief   Widget to control RAMLink resources
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 */

/*
 * $VICERES RAMLINK             x64 x64sc xscpu64 x128
 * $VICERES RAMLINKfilename     x64 x64sc xscpu64 x128
 * $VICERES RAMLINKImageWrite   x64 x64sc xscpu64 x128
 * $VICERES RAMLINKsize         x64 x64sc xscpu64 x128
 * $VICERES RAMLINKmode         x64 x64sc xscpu64 x128
 * $VICERES RAMLINKRTCSave      x64 x64sc xscpu64 x128
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

#include "carthelpers.h"
#include "cartridge.h"
#include "debug_gtk3.h"
#include "resources.h"
#include "vice_gtk3.h"

#include "ramlinkwidget.h"


/** \brief  RAMLINK modes
 */
static const vice_gtk3_radiogroup_entry_t ramlink_modes[] = {
    { "Normal",     1 },
    { "Disable",    0 },
    { NULL,         -1 }
};


/** \brief  Globs for RAMLink image files
 */
static const char * const image_patterns[] = { "*.img", ".bin", "*.raw", NULL };


/** \brief  Handler for the 'clicked' event of the 'Write image' button'
 *
 * \param[in]   widget  widget triggering the event
 * \param[in]   data    extra event data (unused)
 */
static void on_write_image_clicked(GtkWidget *widget, gpointer data)
{
    debug_gtk3("Attempting to write image to host OS");
    if (carthelpers_flush_func(CARTRIDGE_RAMLINK) < 0) {
        debug_gtk3("OOPS!");
    }
}



/** \brief  Create RAMLink widget
 *
 * \param[in]   parent  parent widget (unused)
 *
 * \return  GtkGrid
 */
GtkWidget *ramlink_widget_create(GtkWidget *parent)
{
    GtkWidget *grid;
    GtkWidget *label;
    GtkWidget *enable;
    GtkWidget *rtc_save;
    GtkWidget *mode;
    GtkWidget *size;
    GtkWidget *image;
    GtkWidget *write_detach;
    GtkWidget *write_image;

    /* use three columns for the label */
    grid = vice_gtk3_grid_new_spaced_with_label(-1, -1, "RAMLink settings", 2);

    /* create 'enable ramlink' checkbox */
    enable = vice_gtk3_resource_check_button_new("RAMLINK", "Enable RAMLink");
    g_object_set(enable, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), enable, 0, 1, 2, 1);

    /* create 'RTC Save' checkbox */
    rtc_save = vice_gtk3_resource_check_button_new("RAMLINKRTCSave", "RTC Save");
    g_object_set(rtc_save, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), rtc_save, 0, 2, 2, 1);

    /* create mode widget */
    mode = vice_gtk3_resource_radiogroup_new(
            "RAMLINKmode",
            ramlink_modes,
            GTK_ORIENTATION_VERTICAL);
    /* create mode label */
    label = gtk_label_new("Mode");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_widget_set_valign(label, GTK_ALIGN_START);
    g_object_set(label, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), mode, 1, 3, 1, 1);

    /* create size widget */
    size = vice_gtk3_resource_spin_int_new(
            "RAMLINKsize",
            0, 16, 1);
#if 0
    g_object_set(
            G_OBJECT(size),
            "max-width-chars", 4,
            NULL);
    gtk_entry_set_max_length(GTK_ENTRY(size), 2);
#endif
    label = gtk_label_new("Size (MiB)");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    g_object_set(label, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 4, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), size, 1, 4, 1, 1);
    /* fix size of the spin button */
    gtk_widget_set_hexpand(size, FALSE);
    gtk_widget_set_halign(size, GTK_ALIGN_START);

    /* create image browser */
    image = vice_gtk3_resource_browser_new(
            "RAMLINKfilename",
            image_patterns,
            "image files",
            "Select RAMLink image file",
            NULL,
            NULL);
    label = gtk_label_new("Image file");
    g_object_set(label, "margin-left", 16, NULL);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 5, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), image, 1, 5, 1, 1);

    /* create 'write-on-detach' checkbox*/
    write_detach = vice_gtk3_resource_check_button_new("RAMLINKImageWrite",
            "Write back image on detach and emulator exit");
    gtk_grid_attach(GTK_GRID(grid), write_detach, 1, 6, 1, 1);

    /* create 'Save now' button */
    write_image = gtk_button_new_with_label("Write image to host OS");
    g_signal_connect(
            write_image,
            "clicked",
            G_CALLBACK(on_write_image_clicked),
            NULL);
    gtk_widget_set_halign(write_image, GTK_ALIGN_START);
    gtk_widget_set_hexpand(write_image, FALSE);
    gtk_grid_attach(GTK_GRID(grid), write_image, 1, 7, 1, 1);

    gtk_widget_show_all(grid);
    return grid;
}
