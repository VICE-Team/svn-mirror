/*
 * sounddriverwidget.c - GTK3 sound driver widget
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
#include "ui.h"
#include "resources.h"
#include "vsync.h"
#include "sound.h"
#include "widgethelpers.h"
#include "debug_gtk3.h"

#include "sounddriverwidget.h"


#define LABEL_BUFFER_SIZE   256


/*
 * Event handlers
 */


/** \brief  Handler for the "changed" event of the device combobox
 *
 * \param[in]   widget      widget triggering the event
 * \param[in]   user_data   data for the event (unused)
 */
static void on_device_changed(GtkWidget *widget, gpointer user_data)
{
    const char *id;

    id = gtk_combo_box_get_active_id(GTK_COMBO_BOX(widget));
    debug_gtk3("device ID = '%s'\n", id);
    resources_set_string("SoundDeviceName", id);
}


/** \brief  Handler for the "changed" event of the driver argument entry
 *
 * \param[in]   widget      widget triggering the event
 * \param[in]   user_data   data for the event (unused)
 *
 * FIXME:   responds on every character added or removed and thus calls
 *          resources_set_string() every time. Better to use some state machine
 *          that gets activated when the widget gets focus and then checks if
 *          the entry value changed when the widget looses focus and only then
 *          sets the resource.
 */
static void on_argument_changed(GtkWidget *widget, gpointer user_data)
{
    GtkEntryBuffer *buffer;
    const char *text;

    buffer = gtk_entry_get_buffer(GTK_ENTRY(widget));
    text = gtk_entry_buffer_get_text(buffer);

    debug_gtk3("driver argument: '%s'\n", text);
    resources_set_string("SoundDeviceArg", text);
}


/*
 * Helper functions
 */


/** \brief  Create the bold title label
 *
 * \return  label
 *
 * TODO:    perhaps move into widgethelpers.c, I'll be using this a lot
 */
static GtkWidget *create_title_label(void)
{
    GtkWidget *label;
    gchar buffer[LABEL_BUFFER_SIZE];

    label = gtk_label_new(NULL);
    g_snprintf(buffer, LABEL_BUFFER_SIZE, "<b>Driver</b>");
    gtk_label_set_markup(GTK_LABEL(label), buffer);
    gtk_label_set_xalign(GTK_LABEL(label), 0.0);
    g_object_set(label, "margin-bottom", 8, NULL);
    return label;
}


/** \brief  Create combobox with sound devices
 *
 * \return  combobox
 */
static GtkWidget *create_device_combobox(void)
{
    GtkWidget *combo;
    int i;
    int count = sound_device_num();
    const char *current_device = NULL;

    debug_gtk3("%d sound devices\n", count);

    resources_get_string("SoundDeviceName", &current_device);
    debug_gtk3("current device: '%s'\n", current_device);

    combo = gtk_combo_box_text_new();
    for (i = 0; i < count; i++) {
        const char *device = sound_device_name(i);
        gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(combo),
                device, device);
    }

    /* set active device */
    gtk_combo_box_set_active_id(GTK_COMBO_BOX(combo), current_device);

    /* now connect event handler */
    g_signal_connect(combo, "changed", G_CALLBACK(on_device_changed), NULL);

    return combo;
}


/** \brief  Create a left-aligned, 16 units indented label
 *
 * \param[in]   text    label text
 *
 * \return  label
 *
 * TODO:    perhaps move into widgethelpers.c, I'll be using this a lot
 */
static GtkWidget *create_indented_label(const char *text)
{
    GtkWidget *label = gtk_label_new(text);

    gtk_label_set_xalign(GTK_LABEL(label), 0.0);
    g_object_set(label, "margin-left", 16, NULL);
    return label;
}


/** \brief  Create the 'device driver argument' text entry box
 *
 * \return  entry
 */
static GtkWidget *create_argument_entry(void)
{
    GtkWidget *entry;
    GtkEntryBuffer *buffer;
    const char *argument;

    buffer = gtk_entry_buffer_new(NULL, -1);
    resources_get_string("SoundDeviceArg", &argument);
    gtk_entry_buffer_set_text(buffer, argument, -1);

    entry = gtk_entry_new_with_buffer(buffer);

    /* TODO: get resource value */
    g_signal_connect(entry, "changed", G_CALLBACK(on_argument_changed), NULL);
    return entry;
}


/** \brief  Create the sound driver/device widget
 *
 * \return  driver widget (GtkGrid)
 */
GtkWidget *create_sound_driver_widget(void)
{
    GtkWidget *grid;

    debug_gtk3("called\n");

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);

    gtk_grid_attach(GTK_GRID(grid), create_title_label(), 0, 0, 2, 1);

    gtk_grid_attach(GTK_GRID(grid),
            create_indented_label("Device name"), 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), create_device_combobox(), 1, 1, 1, 1);

    gtk_grid_attach(GTK_GRID(grid),
            create_indented_label("Driver argument"), 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), create_argument_entry(), 1, 2, 1, 1);

    gtk_widget_show_all(grid);
    return grid;
}
