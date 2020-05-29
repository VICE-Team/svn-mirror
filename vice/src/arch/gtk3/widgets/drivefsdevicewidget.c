/** \file   drivefsdevicewidget.c
 * \brief   Drive file system device widget
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 */

/*
 * $VICERES FSDevice8ConvertP00     -vsid
 * $VICERES FSDevice9ConvertP00     -vsid
 * $VICERES FSDevice10ConvertP00    -vsid
 * $VICERES FSDevice11ConvertP00    -vsid
 * $VICERES FSDevice8SaveP00        -vsid
 * $VICERES FSDevice9SaveP00        -vsid
 * $VICERES FSDevice10SaveP00       -vsid
 * $VICERES FSDevice11SaveP00       -vsid
 * $VICERES FSDevice8HideCBMFiles   -vsid
 * $VICERES FSDevice9HideCBMFiles   -vsid
 * $VICERES FSDevice10HideCBMFiles  -vsid
 * $VICERES FSDevice11HideCBMFiles  -vsid
 * $VICERES FSDevice8Dir            -vsid
 * $VICERES FSDevice9Dir            -vsid
 * $VICERES FSDevice10Dir           -vsid
 * $VICERES FSDevice11Dir           -vsid
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

#include "basewidgets.h"
#include "widgethelpers.h"
#include "debug_gtk3.h"
#include "resources.h"
#include "drive.h"
#include "drive-check.h"
#include "machine.h"
#include "attach.h"
#include "selectdirectorydialog.h"

#include "drivefsdevicewidget.h"

#if 0
/** \brief  List of file system types
 */
static const vice_gtk3_combo_entry_int_t device_types[] = {
    { "None", ATTACH_DEVICE_NONE },
    { "File system", ATTACH_DEVICE_FS },
#ifdef HAVE_REALDEVICE
    { "Real device (OpenCBM)", ATTACH_DEVICE_REAL },
#endif
    { NULL, -1 }
};
#endif


/*
 * TODO:    refactor the fsdir entry code into a `resourceentrywidget` in
 *          src/arch/gtk3/widgets/base
 */


/** \brief  Handler for the "clicked" event of the fs dir browse button
 *
 * \param[in]   widget      fs dir browse button
 * \param[in]   user_data   extra event data (entry widget)
 */
static void on_fsdir_browse_clicked(GtkWidget *widget, gpointer user_data)
{
    GtkWidget *entry = GTK_WIDGET(user_data);
    gchar *filename;

    filename = vice_gtk3_select_directory_dialog("Select file system directory",
            NULL, TRUE, NULL);
    if (filename != NULL) {
        vice_gtk3_resource_entry_full_set(entry, filename);
        g_free(filename);
    }
}


#if 0
/** \brief  Create a combo box with device types
 *
 * \param[in]   unit    unit number
 *
 * \return  GtkComboBoxText
 */
static GtkWidget *create_device_type_widget(int unit)
{
    return vice_gtk3_resource_combo_box_int_new_sprintf(
            "FileSystemDevice%d", device_types, unit);
}
#endif


/** \brief  Create text entry for file system directory for \a unit
 *
 * \param[in]   unit    unit number
 *
 * \return  GtkEntry
 */
static GtkWidget *create_fsdir_entry_widget(int unit)
{
    GtkWidget *entry;
    char resource[256];

    g_snprintf(resource, 256, "FSDevice%dDir", unit);
    entry = vice_gtk3_resource_entry_full_new(resource);
    gtk_widget_set_tooltip_text(entry,
            "Set the host OS directory to use as a virtual drive");
    gtk_widget_set_hexpand(entry, TRUE);
    return entry;
}


/** \brief  Create widget to control P00-settings
 *
 * \param[in]   unit    unit number
 *
 * \return  GtkGrid
 */
static GtkWidget *create_p00_widget(int unit)
{
    GtkWidget *grid;
    GtkWidget *p00_convert;
    GtkWidget *p00_only;
    GtkWidget *p00_save;
    char resource[256];

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);

    /* Label texts have been converted to shorter strings, using vice.texi
     * So don't blame me.
     */

    g_snprintf(resource, 256, "FSDevice%dConvertP00", unit);
    p00_convert = vice_gtk3_resource_check_button_new(resource,
            "Access P00 files with their built-in name");
    gtk_grid_attach(GTK_GRID(grid), p00_convert, 0, 0, 1, 1);

    g_snprintf(resource, 256, "FSDevice%dSaveP00", unit);
    p00_save = vice_gtk3_resource_check_button_new(resource, 
            "Create P00 files on save");
    gtk_grid_attach(GTK_GRID(grid), p00_save, 0, 1, 1, 1);

    g_snprintf(resource, 256, "FSDevice%dHideCBMFiles", unit);
    p00_only = vice_gtk3_resource_check_button_new(resource,
            "Do not show non-P00 files | Only show P00 files");
    gtk_grid_attach(GTK_GRID(grid), p00_only, 0, 2, 1, 1);

    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Create widget to control file system device settings of \a unit
 *
 * \param[in]   unit    unit number
 *
 * \return  GtkGrid
 */
GtkWidget *drive_fsdevice_widget_create(int unit)
{
    GtkWidget *grid;
    GtkWidget *entry;
    GtkWidget *label;
    GtkWidget *browse;
    GtkWidget *p00;

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);

    label = gtk_label_new("Directory");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    g_object_set(label, "margin-left", 8, NULL);
    entry = create_fsdir_entry_widget(unit);
    browse = gtk_button_new_with_label("Browse ...");
    g_signal_connect(browse, "clicked", G_CALLBACK(on_fsdir_browse_clicked),
            (gpointer)entry);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entry, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), browse, 2, 1, 1, 1);

    p00 = create_p00_widget(unit);
    g_object_set(p00, "margin-left", 16, "margin-top", 8, NULL);
    gtk_grid_attach(GTK_GRID(grid), p00, 0, 2, 3, 1);

    gtk_widget_show_all(grid);
    return grid;
}
