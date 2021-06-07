/** \file   ltkernalwidget.c
 * \brief   Widget to control Lt. Kernal resources
 *
 * Lt. Kernal cartridge settings.
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 */

/*
 * $VICERES LTKio           x64 x64sc x128
 * $VICERES LTKserial       x64 x64sc x128
 * $VICERES LTKimage0       x64 x64sc x128
 * $VICERES LTKimage1       x64 x64sc x128
 * $VICERES LTKimage2       x64 x64sc x128
 * $VICERES LTKimage3       x64 x64sc x128
 * $VICERES LTKimage4       x64 x64sc x128
 * $VICERES LTKimage5       x64 x64sc x128
 * $VICERES LTKimage6       x64 x64sc x128
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

#include "resources.h"
#include "vice_gtk3.h"

#include "ltkernalwidget.h"


/** \brief  CSS used to mark 'serial number' invalid
 */
#define SERIAL_INVALID_CSS \
    "entry {\n" \
    "  background-color: crimson;\n" \
    "}"


/** \brief  Filter patterns for HD image files
 *
 * Used in the HD image browser widgets.
 */
static const char *filter_list[] = {
        "*.hdd", "*.iso", "*.fdd", "*.cfa", "*.dsk", "*.img", NULL
};


/** \brief  List of I/O addresses for the cartridge
 */
static vice_gtk3_radiogroup_entry_t io_entries[] = {
    { "$DExx",  0 },
    { "$DFxx",  1 },
    { NULL,     -1 }
};


/** \brief  Reference to the CSS provider used to mark 'serial number' invalid'
 */
static GtkCssProvider *serial_css_provider_invalid;



/** \brief  Create widget to set port number
 *
 *
 * \return  GtkGrid
 */
static GtkWidget *create_port_number_widget(void)
{
    GtkWidget *grid;
    GtkWidget *spin;

    /* create grid with label */
    grid = vice_gtk3_grid_new_spaced_with_label(8, 16, "Port Number", 1);

    /* create spin button */
    spin = vice_gtk3_resource_spin_int_new("LTKport", 0, 15, 1);
    g_object_set(G_OBJECT(spin), "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), spin, 0, 1, 1, 1);

    return grid;
}


/** \brief  Create widget to select I/O address range
 *
 * \return  GtkGrid
 */
static GtkWidget *create_io_address_widget(void)
{
    GtkWidget *grid;
    GtkWidget *group;

    /* create grid with label */
    grid = vice_gtk3_grid_new_spaced_with_label(8, 16, "I/O Address", 1);

    /* create radio group for I/O address */
    group = vice_gtk3_resource_radiogroup_new("LTKio",
                                              io_entries,
                                              GTK_ORIENTATION_HORIZONTAL);
    g_object_set(G_OBJECT(group), "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), group, 0, 1, 1, 1);
    return grid;
}



/** \brief  Handler for the 'focus-out' event of the serial number entry
 *
 * \param[in]   entry   entry box
 * \param[in]   event   event object
 * \param[in]   data    extra event data (unused)
 *
 * \return  TRUE
 */
static gboolean on_serial_focus_out_event(
        GtkEntry *entry,
        GdkEvent *event,
        gpointer data)
{
    debug_gtk3("Focus-Out: entry text = '%s'", gtk_entry_get_text(entry));
    /* attempt to update resource, use CSS to mark widget background to
     * indicate whether the value was valid
     */
    if (resources_set_string("LTKserial", gtk_entry_get_text(entry)) != 0) {
        debug_gtk3("Invalid input!\n");
        vice_gtk3_css_provider_add(GTK_WIDGET(entry),
                                   serial_css_provider_invalid);
    } else {
        vice_gtk3_css_provider_remove(GTK_WIDGET(entry),
                                      serial_css_provider_invalid);
    }
    return TRUE;
}


/** \brief  Handler for the 'on-key-press' event for the serial number entry
 *
 * \param[in]   entry   entry box
 * \param[in]   event   event object
 * \param[in]   data    extra event data (unused)
 *
 * \return  TRUE if Enter was pushed, FALSE otherwise (makes the pushed key
 *          propagate to the entry)
 */
static gboolean on_serial_key_press_event(
        GtkEntry *entry,
        GdkEvent *event,
        gpointer data)
{
    GdkEventKey *keyev = (GdkEventKey *)event;

    if (keyev->type == GDK_KEY_PRESS && keyev->keyval == GDK_KEY_Return) {
        /*
         * We handled Enter/Return for Gtk3/GLib, whether or not the
         * resource actually gets updated is another issue.
         */
        debug_gtk3("Enter pressed: entry text = '%s'", gtk_entry_get_text(entry));

        /* attempt to update resource, use CSS to mark widget background to
         * indicate whether the value was valid
         */
        if (resources_set_string("LTKserial", gtk_entry_get_text(entry)) != 0) {
            debug_gtk3("Invalid input!\n");
            vice_gtk3_css_provider_add(GTK_WIDGET(entry),
                                       serial_css_provider_invalid);
        } else {
            vice_gtk3_css_provider_remove(GTK_WIDGET(entry),
                                          serial_css_provider_invalid);
        }
        return TRUE;
    }

    return FALSE;
}


/** \brief  Create entry box for the Lt. Kernal serial number
 *
 * The serial number should be a string of 8 decimal digits.
 *
 * TODO:    Check input and set resource on focus loss or Enter pressed
 *
 * \return  GtkWidget
 */
static GtkWidget *create_serial_number_widget(void)
{
    GtkWidget *grid;
    GtkWidget *entry;
    GtkWidget *label;
    const char *value;

    /* create grid with label */
    grid = vice_gtk3_grid_new_spaced_with_label(8, 16, "Serial Number", 2);

    /* create text entry box */
    entry = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(entry), 8);
    g_object_set(G_OBJECT(entry), "margin-left", 16, NULL);
    resources_get_string("LTKserial", &value);
    gtk_entry_set_text(GTK_ENTRY(entry), value);
    gtk_grid_attach(GTK_GRID(grid), entry, 0, 1, 1, 1);
    /* add event handlers */
    g_signal_connect(entry, "focus-out-event",
            G_CALLBACK(on_serial_focus_out_event), NULL);
    g_signal_connect(entry, "key-press-event",
            G_CALLBACK(on_serial_key_press_event), NULL);

    /* create CSS provider to mark input invalid */
    serial_css_provider_invalid = vice_gtk3_css_provider_new(
            SERIAL_INVALID_CSS);


    /* add info label */
    label = gtk_label_new("The serial number must be 8 decimal digits.");
    gtk_grid_attach(GTK_GRID(grid), label, 1, 1, 1, 1);

    return grid;
}



/** \brief  Create widget with resource browsers for the HD images
 *
 * Creates a widget with 7 file browser widgets for HD image 0-6.
 *
 * \return  GtkGrid;
 */
static GtkWidget *create_hd_images_widget(void)
{
    GtkWidget *grid;
    int i;

    /* create grid with label */
    grid = vice_gtk3_grid_new_spaced_with_label(8, 8, "HD Images", 1);

    /* add 7 browser widgets */
    for (i = 0; i <= 6; i++) {
        GtkWidget *browser;
        gchar resource[256];
        gchar title[256];
        gchar label[256];

        /* generate resource name, dialog title and widget label */
        g_snprintf(resource, sizeof(resource), "LTKimage%d", i);
        g_snprintf(title, sizeof(title), "Select HD%d image file", i);
        g_snprintf(label, sizeof(label), "HD%d image", i);
        /* create browser widget and add to grid */
        browser = vice_gtk3_resource_browser_new(
                resource,
                filter_list,
                "HD image files",
                title,
                label,
                NULL);
        g_object_set(G_OBJECT(browser), "margin-left", 16, NULL);
        gtk_grid_attach(GTK_GRID(grid), browser, 0, i + 1, 1, 1);
    }
    return grid;
}


/** \brief  Create main Lt. Kernal settings widget
 *
 * \param[in]   parent  parent widget (unused)
 *
 * \return  GtkGrid
 */
GtkWidget *ltkernal_widget_create(GtkWidget *parent)
{
    GtkWidget *grid;
    GtkWidget *hd_images;
    GtkWidget *serial;
    GtkWidget *io_address;
    GtkWidget *port_number;

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 32);

    /* add HD images */
    hd_images = create_hd_images_widget();
    gtk_grid_attach(GTK_GRID(grid), hd_images, 0, 0, 2, 1);

    /* add serial number */
    serial = create_serial_number_widget();
    gtk_grid_attach(GTK_GRID(grid), serial, 0, 1, 2, 1);

    /* add I/O address */
    io_address = create_io_address_widget();
    gtk_grid_attach(GTK_GRID(grid), io_address, 0, 2, 2, 1);

    /* add port number */
    port_number = create_port_number_widget();
    gtk_grid_attach(GTK_GRID(grid), port_number, 1, 2, 1, 1);

    gtk_widget_show_all(grid);
    return grid;
}
