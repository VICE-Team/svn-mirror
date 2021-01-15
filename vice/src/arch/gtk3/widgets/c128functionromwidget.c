/** \file   c128functionromwidget.c
 * \brief   Widget to control C128 function roms
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 */

/*
 * $VICERES InternalFunctionROM         x128
 * $VICERES InternalFunctionName        x128
 * $VICERES InternalFunctionROMRTCSave  x128
 * $VICERES ExternalFunctionROM         x128
 * $VICERES ExternalFunctionName        x128
 * $VICERES ExternalFunctionROMRTCSave  x128
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
 */

#include "vice.h"
#include <gtk/gtk.h>

#include "vice_gtk3.h"
#include "functionrom.h"
#include "ui.h"

#include "c128functionromwidget.h"


/** \brief  List of possible ROM bla things
 *
 * Seems to be the same for ext ROMS
 */
static const vice_gtk3_radiogroup_entry_t rom_types[] = {
    { "None",   INT_FUNCTION_NONE },    /* this one probably requires the
                                           text entry/browse button to be
                                           disabled */
    { "ROM",    INT_FUNCTION_ROM },
    { "RAM",    INT_FUNCTION_RAM },
    { "RTC",    INT_FUNCTION_RTC },
    { NULL, - 1 },
};


/** \brief  Callback for the open-file dialog
 *
 * \param[in,out]   dialog      open-file dialog
 * \param[in]       filename    filename or NULL on cancel
 * \param[in]       data        entry widget reference
 */
static void browse_filename_callback(GtkDialog *dialog,
                                     gchar *filename,
                                     gpointer data)
{
    if (filename != NULL) {
        GtkWidget *entry = data;
        vice_gtk3_resource_entry_full_set(entry, filename);
        g_free(filename);
    }
    gtk_widget_destroy(GTK_WIDGET(dialog));
}


/** \brief  Handler for the "clicked" event of a "browse" button
 *
 * \param[in]   widget  browse button
 * \param[in]   data    entry to store filename
 */
static void on_browse_clicked(GtkWidget *widget, gpointer data)
{
    vice_gtk3_open_file_dialog(
            "Open ROM file",
            NULL,
            NULL,
            NULL,
            browse_filename_callback,
            data);  /* resourceentry widget for the filename */
}


/*
 * This was supposed to be used to grey-out the ROM filename entry when the
 * type was None. But the callback only gets triggered when there already is
 * a valid ROM file, so we can't do the grey-out since an empty resource means
 * setting the ROM type fails.
 * This has to do with the base 'resourceradiogroup' widget triggering the
 * callback when setting the resource fails.
 *
 * I could change the resourceradiogroup widget to trigger the callback even
 * on errors, but that might break other things.
 *
 * Another option would be custom code, not using the base resource widgets,
 * but that's a lot of work for something that appears to basically never get
 * used.
 */
#if 0
/** \brief  Extra callback for the ROM type radio buttons
 *
 * \param[in,out]   widget  resource radiogroup
 * \param[in]       id      current select ID
 */
static void rom_type_callback(GtkWidget *widget, int id)
{
    debug_gtk3("Got ID %d", id);
}
#endif

/** \brief  Create ROM type widget
 *
 * \param[in]   prefix  resource prefix
 *
 * \return  GtkGrid
 */
static GtkWidget *create_rom_type_widget(const char *prefix)
{
    GtkWidget *widget;

    widget = vice_gtk3_resource_radiogroup_new_sprintf("%sFunctionROM",
            rom_types, GTK_ORIENTATION_HORIZONTAL, prefix);
    gtk_grid_set_column_spacing(GTK_GRID(widget), 16);
    return widget;
}


/** \brief  Create ROM file selection widget
 *
 * \param[in]   prefix  resource prefix
 *
 * \return  GtkGrid
 */
static GtkWidget *create_rom_file_widget(const char *prefix)
{
    GtkWidget *grid;
    GtkWidget *browse;
    GtkWidget *entry;

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);

    entry = vice_gtk3_resource_entry_full_new_sprintf("%sFunctionName",
                                                      prefix);
    gtk_widget_set_hexpand(entry, TRUE);
    gtk_grid_attach(GTK_GRID(grid), entry, 0, 0, 1, 1);

    browse = gtk_button_new_with_label("Browse ...");
    gtk_grid_attach(GTK_GRID(grid), browse, 1, 0, 1,1);
    g_signal_connect(browse, "clicked", G_CALLBACK(on_browse_clicked),
            entry);

    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Create External/Internal ROM widget
 *
 * \param[in]   parent  parent widget
 * \param[in]   prefix  resource prefix
 *
 * \return GtkGrid
 */
static GtkWidget *create_rom_widget(GtkWidget *parent, const char *prefix)
{
    GtkWidget *grid;
    GtkWidget *type;
    GtkWidget *label;
    GtkWidget *rtc;
    char buffer[256];

    g_snprintf(buffer, sizeof(buffer), "%s Function ROM", prefix);
    grid = vice_gtk3_grid_new_spaced_with_label(-1, -1, buffer, 1);


    label = gtk_label_new("ROM type");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    g_object_set(label, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 1, 1, 1);
    type = create_rom_type_widget(prefix);
#if 0
    vice_gtk3_resource_radiogroup_add_callback(type, rom_type_callback);
#endif
    gtk_grid_attach(GTK_GRID(grid), type, 1, 1, 1, 1);

    label = gtk_label_new("ROM file");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    g_object_set(label, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), create_rom_file_widget(prefix), 1, 2, 1, 1);

    rtc = vice_gtk3_resource_check_button_new_sprintf("%sFunctionROMRTCSave",
            "Save RTC data", prefix);
    g_object_set(rtc, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), rtc, 0, 3, 3, 1);

    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Create widget to select Internal/External function ROMs
 *
 * \param[in]   parent  parent widget
 *
 * \return  GtkGrid
 */
GtkWidget *c128_function_rom_widget_create(GtkWidget *parent)
{
    GtkWidget *grid;
    GtkWidget *internal_widget;
    GtkWidget *external_widget;

    grid = vice_gtk3_grid_new_spaced(16, 32);

    internal_widget = create_rom_widget(parent, "Internal");
    gtk_grid_attach(GTK_GRID(grid), internal_widget, 0, 0, 1, 1);
    external_widget = create_rom_widget(parent, "External");
    gtk_grid_attach(GTK_GRID(grid), external_widget, 0, 1, 1, 1);

    gtk_widget_show_all(grid);
    return grid;
}
