/** \file   src/arch/gtk3/widgets/midiwidget.c
 * \brief   MIDI emulation settings widget
 *
 * Written by
 *  Bas Wassink <b.wassink@ziggo.nl>
 *
 * Controls the following resource(s):
 *  MIDIEnable (x64/x64sc/xscpu64/x128/xvic)
 *  MIDIMode (x64/x64sc/xscpu64/x128/xvic)
 *  MIDIDriver (x64/x64sc/xscpu64/x128/xvic)
 *  MIDIInDev (x64/x64sc/xscpu64/x128/xvic)
 *  MIDIOutDev (x64/x64sc/xscpu64/x128/xvic)
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

#include "machine.h"
#include "resources.h"
#include "debug_gtk3.h"
#include "basewidgets.h"
#include "widgethelpers.h"
#include "basedialogs.h"
#include "openfiledialog.h"

#include "midiwidget.h"


static GtkWidget *midi_enable;
static GtkWidget *midi_mode;
#if defined(UNIX_COMPILE) && !defined(MACOSX_SUPPORT)
static GtkWidget *midi_driver;
#endif
static GtkWidget *midi_in_entry;
static GtkWidget *midi_in_browse;
static GtkWidget *midi_out_entry;
static GtkWidget *midi_out_browse;



static ui_combo_entry_int_t midi_modes[] = {
    { "Sequential", 0 },
    { "Passport/Syntech", 1 },
    { "DATEL/Siel/JMS", 2 },
    { "Namesoft", 4 },
    { "Maplin", 5 },
    { NULL, -1 }
};

#if defined(UNIX_COMPILE) && !defined(MACOSX_SUPPORT)
static ui_combo_entry_int_t midi_drivers[]= {
    { "OSS", 0 },
    { "ALSA", 1 },
    { NULL, -1 }
};
#endif


/** \brief  Extra handler for the "toggled" event of the "Enable" check button
 *
 * \param[in]   widget      widget triggering the event
 * \param[in]   user_data   extra event data (unused)
 */
static void on_midi_enable_toggled(GtkWidget *widget, gpointer user_data)
{
    int state = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));

    gtk_widget_set_sensitive(midi_mode, state);
#if defined(UNIX_COMPILE) && !defined(MACOSX_SUPPORT)
    gtk_widget_set_sensitive(midi_driver, state);
#endif
    gtk_widget_set_sensitive(midi_in_entry, state);
    gtk_widget_set_sensitive(midi_in_browse, state);
    gtk_widget_set_sensitive(midi_out_entry, state);
    gtk_widget_set_sensitive(midi_out_browse, state);
}


/** \brief  Handler for the "clicked" event of the MIDI-In "Browse" button
 *
 * \param[in]   widget      button
 * \param[in]   user_data   text entry to store new filename
 */
static void on_midi_in_browse(GtkWidget *widget, gpointer user_data)
{
    char *filename;
    const char *filters[] = { "mi*", NULL };

    filename = ui_open_file_dialog(widget, "Select MIDI In device",
            "MIDI devices", filters, "/dev");
    if (filename != NULL) {
        debug_gtk3("Setting MIDIInDev to '%s'", filename);
        gtk_entry_set_text(GTK_ENTRY(user_data), filename);
        g_free(filename);
    }
}


/** \brief  Handler for the "clicked" event of the MIDI-Out "Browse" button
 *
 * \param[in]   widget      button
 * \param[in]   user_data   text entry to store new filename
 */
static void on_midi_out_browse(GtkWidget *widget, gpointer user_data)
{
    char *filename;
    const char *filters[] = { "mi*", NULL };

    filename = ui_open_file_dialog(widget, "Select MIDI Out device",
            "MIDI devices", filters, "/dev");
    if (filename != NULL) {
        debug_gtk3("Setting MIDIOutDev to '%s'", filename);
        gtk_entry_set_text(GTK_ENTRY(user_data), filename);
        g_free(filename);
    }

}


/** \brief  Create check button to enable/disable MIDI emulation
 *
 * \return  GtkCheckButton
 */
static GtkWidget *create_midi_enable_widget(void)
{
    GtkWidget *check;

    check = resource_check_button_create("MIDIEnable", "Enable MIDI emulation");
    g_signal_connect(check, "toggled", G_CALLBACK(on_midi_enable_toggled),
            NULL);
    return check;
}


/** \brief  Create MIDI emulation mode widget
 *
 * \return  GtkComboBoxText
 */
static GtkWidget *create_midi_mode_widget(void)
{
    return resource_combo_box_int_create("MIDIMode", midi_modes);
}


#if defined(UNIX_COMPILE) && !defined(MACOSX_SUPPORT)
/** \brief  Create MIDI driver selection widget
 *
 * \return  GtkComboBoxText
 */
static GtkWidget *create_midi_driver_widget(void)
{
    return resource_combo_box_int_create("MIDIDriver", midi_drivers);
}
#endif


/** \brief  Create MIDI settings widget
 *
 * \param[in]   parent  parent widget
 *
 * \return  GtkGrid
 */
GtkWidget *midi_widget_create(GtkWidget *parent)
{
    GtkWidget *grid;
    GtkWidget *label;
    int row;

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 16);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);

    midi_enable = create_midi_enable_widget();
    gtk_grid_attach(GTK_GRID(grid), midi_enable, 0, 0, 3, 1);

    label = gtk_label_new("MIDI mode");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    g_object_set(label, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 1, 1, 1);
    midi_mode = create_midi_mode_widget();
    gtk_grid_attach(GTK_GRID(grid), midi_mode, 1, 1, 1, 1);

    row = 2;

#if defined(UNIX_COMPILE) && !defined(MACOSX_SUPPORT)
    label = gtk_label_new("MIDI driver");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    g_object_set(label, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 2, 1, 1);
    midi_driver = create_midi_driver_widget();
    gtk_grid_attach(GTK_GRID(grid), midi_driver, 1, 2, 1, 1);

    row++;
#endif

    /* TODO: seems like Windows uses a combobox with a list of drivers, so this
     *       code only works for Unix (and not OSX probably)
     */

    label = gtk_label_new("MIDI In");
    g_object_set(label, "margin-left", 16, NULL);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label, 0, row, 1, 1);
    midi_in_entry = resource_entry_create("MIDIInDev");
    gtk_widget_set_hexpand(midi_in_entry, TRUE);
    gtk_grid_attach(GTK_GRID(grid), midi_in_entry, 1, row, 1, 1);
    midi_in_browse = gtk_button_new_with_label("Browse ...");
    g_signal_connect(midi_in_browse, "clicked", G_CALLBACK(on_midi_in_browse),
            (gpointer)midi_in_entry);
    gtk_grid_attach(GTK_GRID(grid), midi_in_browse, 2, row, 1, 1);
    row++;

    label = gtk_label_new("MIDI Out");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    g_object_set(label, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), label, 0, row, 1, 1);
    midi_out_entry = resource_entry_create("MIDIOutDev");
    gtk_widget_set_hexpand(midi_out_entry, TRUE);
    gtk_grid_attach(GTK_GRID(grid), midi_out_entry, 1, row, 1, 1);
    midi_out_browse = gtk_button_new_with_label("Browse ...");
    g_signal_connect(midi_out_browse, "clicked", G_CALLBACK(on_midi_out_browse),
            (gpointer)midi_out_entry);
    gtk_grid_attach(GTK_GRID(grid), midi_out_browse, 2, row, 1, 1);
    row++;

    on_midi_enable_toggled(midi_enable, NULL);

    gtk_widget_show_all(grid);
    return grid;
}
