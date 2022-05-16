/** \file   midiwidget.c
 * \brief   MIDI emulation settings widget
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 */

/*
 * $VICERES MIDIEnable      x64 x64sc xscpu64 x128 xvic
 * $VICERES MIDIMode        x64 x64sc xscpu64 x128
 * $VICERE$S MIDIDriver     x64 x64sc xscpu64 x128 xvic
 *  (Unix only, only if both OSS and ALSA is enabled)
 * $VICERES MIDIInDev       x64 x64sc xscpu64 x128 xvic
 *  (OSS and Windows only)
 * $VICERES MIDIOutDev      x64 x64sc xscpu64 x128 xvic
 *  (OSS and Windows  only)
 * $VICERES MIDIName        x64 x64sc xscpu64 x128 xvic
 *  (ALSA and MacOS only)
 * $VICEREs MIDIInName      x64 x64sc xscpu64 x128 xvic
 *  (MacOS only)
 * $VICERES MIDIOutName     x64 x64sc xscpu64 x128 xvic
 *  (MacOS only)
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

#ifdef HAVE_MIDI

#if defined (UNIX_COMPILE) && !defined(MACOS_COMPILE)
# if !defined (USE_OSS) && !defined (USE_ALSA)
# error "MIDI needs either OSS or ALSA"
# endif
#endif

#include <gtk/gtk.h>

#include "archdep_defs.h"
#include "machine.h"
#include "resources.h"
#include "ui.h"
#include "vice_gtk3.h"

#include "midiwidget.h"


/** \brief  MIDI enable checkbutton */
static GtkWidget *midi_enable;
/** \brief  MIDI mode combobox */
static GtkWidget *midi_mode;
#if defined (WINDOWS_COMPILE) || defined (USE_OSS)
/** \brief  MIDI in device  */
static GtkWidget *midi_in_entry;
/** \brief  MIDI out device */
static GtkWidget *midi_out_entry;
#endif
#if defined (MACOS_COMPILE) || defined (USE_ALSA)
/** \brief  MIDI Client name entry */
static GtkWidget *midi_name_entry;
#endif
#if defined (USE_OSS) && defined (USE_ALSA)
/** \brief  MIDI driver widget */
static GtkWidget *midi_driver;
#endif
#if defined (USE_OSS)
/** \brief  MIDI in file browser */
static GtkWidget *midi_in_browse;
/** \brief  MIDI out file browser */
static GtkWidget *midi_out_browse;
#endif


/** \brief  Modes for MIDI support
 *
 * Seems to be a list of MIDI expansions
 */
static const vice_gtk3_combo_entry_int_t midi_modes[] = {
    { "Sequential",         0 },
    { "Passport/Syntech",   1 },
    { "DATEL/Siel/JMS",     2 },
    { "Namesoft",           4 },
    { "Maplin",             5 },
    { NULL, -1 }
};


#if defined(USE_OSS) && defined(USE_ALSA)
/** \brief  List of MIDI drivers
 */
static const vice_gtk3_combo_entry_int_t midi_drivers[]= {
    { "OSS",    0 },
    { "ALSA",   1 },
    { NULL,     -1 }
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
#if defined (USE_ALSA)
    int alsaactive = 1;
#endif
#if defined (USE_OSS)
    int ossactive = 1;
#endif

    if (machine_class != VICE_MACHINE_VIC20) {
        gtk_widget_set_sensitive(midi_mode, state);
    }

#if defined (MACOS_COMPILE)
    /* ALSA or macOS */
    gtk_widget_set_sensitive(midi_name_entry, state);
    /* Windows, OSS or macOS */
    gtk_widget_set_sensitive(midi_in_entry, state);
    gtk_widget_set_sensitive(midi_out_entry, state);
#endif

#if defined (WINDOWS_COMPILE)
    /* Windows, OSS or macOS */
    gtk_widget_set_sensitive(midi_in_entry, state);
    gtk_widget_set_sensitive(midi_out_entry, state);
#endif

#if defined (USE_OSS) && defined (USE_ALSA)
    /* only needed if OSS and ALSA is enabled */
    gtk_widget_set_sensitive(midi_driver, state);
    ossactive = (gtk_combo_box_get_active(GTK_COMBO_BOX(midi_driver)) == 0);
    alsaactive = (gtk_combo_box_get_active(GTK_COMBO_BOX(midi_driver)) == 1);
#endif

#if defined(USE_ALSA)
    /* ALSA or macOS */
    gtk_widget_set_sensitive(midi_name_entry, state && alsaactive);
#endif

#if defined(USE_OSS)
    /* Windows, OSS or macOS */
    gtk_widget_set_sensitive(midi_in_entry, state && ossactive);
    gtk_widget_set_sensitive(midi_out_entry, state && ossactive);
    /* OSS only */
    gtk_widget_set_sensitive(midi_in_browse, state && ossactive);
    gtk_widget_set_sensitive(midi_out_browse, state && ossactive);
#endif
}


#if defined(USE_OSS)
static void midi_in_filename_callback(GtkDialog *dialog,
                                      gchar *filename,
                                      gpointer data)
{
    if (filename != NULL) {
        vice_gtk3_resource_entry_full_set(midi_in_entry, filename);
        g_free(filename);
    }
    gtk_widget_destroy(GTK_WIDGET(dialog));
}



/** \brief  Handler for the "clicked" event of the MIDI-In "Browse" button
 *
 * \param[in]   widget      button
 * \param[in]   user_data   text entry to store new filename
 */
static void on_midi_in_browse(GtkWidget *widget, gpointer user_data)
{
    const char *filters[] = { "mi*", NULL };

    vice_gtk3_open_file_dialog(
            "Select MIDI In device",
            "MIDI devices", filters, "/dev",
            midi_in_filename_callback,
            NULL);
}


static void midi_out_filename_callback(GtkDialog *dialog,
                                       gchar *filename,
                                       gpointer data)
{
    if (filename != NULL) {
        vice_gtk3_resource_entry_full_set(midi_out_entry, filename);
        g_free(filename);
    }
    gtk_widget_destroy(GTK_WIDGET(dialog));
}


/** \brief  Handler for the 'clicked' event of the MIDI-Out "Browse" button
 *
 * \param[in]   widget      button
 * \param[in]   user_data   text entry to store new filename
 */
static void on_midi_out_browse(GtkWidget *widget, gpointer user_data)
{
    const char *filters[] = { "mi*", NULL };

    vice_gtk3_open_file_dialog(
            "Select MIDI Out device",
            "MIDI devices", filters, "/dev",
            midi_out_filename_callback,
            NULL);
}
#endif


/** \brief  Create check button to enable/disable MIDI emulation
 *
 * \return  GtkCheckButton
 */
static GtkWidget *create_midi_enable_widget(void)
{
    GtkWidget *check;

    check = vice_gtk3_resource_check_button_new("MIDIEnable",
            "Enable MIDI emulation");
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
    return vice_gtk3_resource_combo_box_int_new("MIDIMode", midi_modes);
}


#if defined(USE_OSS) && defined(USE_ALSA)

/** \brief  Handler for the "changed" event of the midi driver combo box
 *
 * \param[in]   combo       combo box
 * \param[in]   user_data   extra event data (unused)
 */
static void on_combo_changed(GtkComboBox *combo, gpointer user_data)
{
    int state = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(midi_enable));
    int ossactive = (gtk_combo_box_get_active(combo) == 0);
    int alsaactive = (gtk_combo_box_get_active(combo) == 1);
    /* ALSA or macOS */
    gtk_widget_set_sensitive(midi_name_entry, state && alsaactive);
    /* Windows, OSS or macOS */
    gtk_widget_set_sensitive(midi_in_entry, state && ossactive);
    gtk_widget_set_sensitive(midi_out_entry, state && ossactive);
    /* OSS only */
    gtk_widget_set_sensitive(midi_in_browse, state && ossactive);
    gtk_widget_set_sensitive(midi_out_browse, state && ossactive);
}

/** \brief  Create MIDI driver selection widget
 *
 * \return  GtkComboBoxText
 */
static GtkWidget *create_midi_driver_widget(void)
{
    GtkWidget *combo;
    combo = vice_gtk3_resource_combo_box_int_new("MIDIDriver", midi_drivers);
    g_signal_connect(combo, "changed", G_CALLBACK(on_combo_changed), NULL);
    return combo;
}
#endif


/** \brief  Create MIDI settings widget
 *
 * \param[in]   parent  parent widget (unused)
 *
 * \return  GtkGrid
 */
GtkWidget *midi_widget_create(GtkWidget *parent)
{
    GtkWidget *grid;
    GtkWidget *label;
    int row;

    grid = vice_gtk3_grid_new_spaced(VICE_GTK3_DEFAULT, VICE_GTK3_DEFAULT);

    midi_enable = create_midi_enable_widget();
    gtk_grid_attach(GTK_GRID(grid), midi_enable, 0, 0, 3, 1);

    if (machine_class != VICE_MACHINE_VIC20) {
        label = gtk_label_new("MIDI mode");
        gtk_widget_set_halign(label, GTK_ALIGN_START);
        g_object_set(label, "margin-left", 16, NULL);
        gtk_grid_attach(GTK_GRID(grid), label, 0, 1, 1, 1);
        midi_mode = create_midi_mode_widget();
        gtk_grid_attach(GTK_GRID(grid), midi_mode, 1, 1, 1, 1);
    }

    row = 2;

#if defined (USE_OSS) && defined (USE_ALSA)
    /* Unix only, use ALSA or OSS? */
    label = gtk_label_new("MIDI driver");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    g_object_set(label, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), label, 0, row, 1, 1);
    midi_driver = create_midi_driver_widget();
    gtk_grid_attach(GTK_GRID(grid), midi_driver, 1, row, 1, 1);
    row++;
#endif

#if defined (MACOS_COMPILE) || defined (USE_ALSA)
    /* macOS, Linux (ALSA) - name of the MIDI client */
#if defined (MACOS_COMPILE)
    label = gtk_label_new("MIDI Client Name");
#else
    label = gtk_label_new("ALSA Client Name");
#endif
    g_object_set(label, "margin-left", 16, NULL);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label, 0, row, 1, 1);
    midi_name_entry = vice_gtk3_resource_entry_full_new("MIDIName");
    gtk_widget_set_hexpand(midi_name_entry, TRUE);
    gtk_grid_attach(GTK_GRID(grid), midi_name_entry, 1, row, 1, 1);
    row++;
#endif

    /* TODO: For Windows we need to use a dropdown box instead and query the
     *       list of available devices. For now we use a Text Entry box where
     *       the device number can be entered.
     */
#ifdef WINDOWS_COMPILE
    label = gtk_label_new("MIDI In");
    g_object_set(label, "margin-left", 16, NULL);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label, 0, row, 1, 1);
    midi_in_entry = vice_gtk3_resource_entry_full_new("MIDIInDev");
    gtk_widget_set_hexpand(midi_in_entry, TRUE);
    gtk_grid_attach(GTK_GRID(grid), midi_in_entry, 1, row, 1, 1);
#endif

#ifdef MACOS_COMPILE
    label = gtk_label_new("MIDI In Name");
    g_object_set(label, "margin-left", 16, NULL);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label, 0, row, 1, 1);
    midi_in_entry = vice_gtk3_resource_entry_full_new("MIDIInName");
    gtk_widget_set_hexpand(midi_in_entry, TRUE);
    gtk_grid_attach(GTK_GRID(grid), midi_in_entry, 1, row, 1, 1);
#endif

#if defined(USE_OSS)
    label = gtk_label_new("OSS MIDI In");
    g_object_set(label, "margin-left", 16, NULL);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label, 0, row, 1, 1);
    midi_in_entry = vice_gtk3_resource_entry_full_new("MIDIInDev");
    gtk_widget_set_hexpand(midi_in_entry, TRUE);
    gtk_grid_attach(GTK_GRID(grid), midi_in_entry, 1, row, 1, 1);

    /* the browse button, only needed for OSS (for browsing to the device file) */
    midi_in_browse = gtk_button_new_with_label("Browse ...");
    g_signal_connect(midi_in_browse, "clicked", G_CALLBACK(on_midi_in_browse),
            (gpointer)midi_in_entry);
    gtk_grid_attach(GTK_GRID(grid), midi_in_browse, 2, row, 1, 1);
#endif
    row++;

    /* TODO: For Windows we need to use a dropdown box instead and query the
     *       list of available devices. For now we use a Text Entry box where
     *       the device number can be entered.
     */
#ifdef WINDOWS_COMPILE
    label = gtk_label_new("MIDI Out");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    g_object_set(label, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), label, 0, row, 1, 1);
    midi_out_entry = vice_gtk3_resource_entry_full_new("MIDIOutDev");
    gtk_widget_set_hexpand(midi_out_entry, TRUE);
    gtk_grid_attach(GTK_GRID(grid), midi_out_entry, 1, row, 1, 1);
#endif

#ifdef MACOS_COMPILE
    label = gtk_label_new("MIDI Out Name");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    g_object_set(label, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), label, 0, row, 1, 1);
    midi_out_entry = vice_gtk3_resource_entry_full_new("MIDIOutName");
    gtk_widget_set_hexpand(midi_out_entry, TRUE);
    gtk_grid_attach(GTK_GRID(grid), midi_out_entry, 1, row, 1, 1);
#endif

#if defined(USE_OSS)
    label = gtk_label_new("OSS MIDI Out");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    g_object_set(label, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), label, 0, row, 1, 1);
    midi_out_entry = vice_gtk3_resource_entry_full_new("MIDIOutDev");
    gtk_widget_set_hexpand(midi_out_entry, TRUE);
    gtk_grid_attach(GTK_GRID(grid), midi_out_entry, 1, row, 1, 1);

    /* the browse button, only needed for OSS (for browsing to the device file) */
    midi_out_browse = gtk_button_new_with_label("Browse ...");
    g_signal_connect(midi_out_browse, "clicked",
            G_CALLBACK(on_midi_out_browse), (gpointer)midi_out_entry);
    gtk_grid_attach(GTK_GRID(grid), midi_out_browse, 2, row, 1, 1);
#endif
    row++;

    on_midi_enable_toggled(midi_enable, NULL);

    gtk_widget_show_all(grid);
    return grid;
}

#endif /* HAVE_MIDI */
