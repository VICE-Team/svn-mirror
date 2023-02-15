/** \file   settings_midi.c
 * \brief   Settings widget controlling MIDI emulation settings
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 */

/*
 * $VICERES MIDIEnable      x64 x64sc xscpu64 x128 xvic
 * $VICERES MIDIMode        x64 x64sc xscpu64 x128
 * $VICERE$S MIDIDriver     x64 x64sc xscpu64 x128 xvic
 *  (Unix only, only if both OSS and ALSA are enabled)
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
#include "c64-midi.h"
#include "machine.h"
/* right now we only have a dropdown list on windows */
#if defined (WINDOWS_COMPILE)
#include "mididevicewidget.h"
#endif
#include "mididrv.h"
#include "resources.h"
#include "ui.h"
#include "vice_gtk3.h"

#include "settings_midi.h"


/** \brief  Modes for MIDI support
 *
 * Seems to be a list of MIDI expansions
 */
static const vice_gtk3_combo_entry_int_t midi_modes[] = {
    { "Sequential",         MIDI_MODE_SEQUENTIAL },
    { "Passport/Syntech",   MIDI_MODE_PASSPORT },
    { "DATEL/Siel/JMS",     MIDI_MODE_DATEL },
    { "Namesoft",           MIDI_MODE_NAMESOFT },
    { "Maplin",             MIDI_MODE_MAPLIN },
    { NULL,                 -1 }
};

#if defined(USE_OSS) && defined(USE_ALSA)
/** \brief  List of MIDI drivers
 *
 * Do not change the order of the items, glue logic depends on this order.
 */
static const vice_gtk3_combo_entry_int_t midi_drivers[]= {
    { "OSS",    MIDI_DRIVER_OSS },
    { "ALSA",   MIDI_DRIVER_ALSA },
    { NULL,     -1 }
};
#endif


/** \brief  MIDI enable checkbutton */
static GtkWidget *midi_enable_widget;
/** \brief  MIDI mode combobox */
static GtkWidget *midi_mode_widget;
/** \brief  MIDI in device  */
static GtkWidget *midi_in_widget;
/** \brief  MIDI out device */
static GtkWidget *midi_out_widget;
/** \brief  MIDI Client name entry */
static GtkWidget *midi_name_widget;
/** \brief  MIDI driver widget */
static GtkWidget *midi_driver_widget;


/** \brief  Create left-aligned label with Pango markup
 *
 * \param[in]   text    text for the label (uses Pango markup)
 *
 * \return  GtkLabel
 */
static GtkWidget *label_helper(const gchar *text)
{
    GtkWidget *label = gtk_label_new(NULL);

    gtk_label_set_markup(GTK_LABEL(label), text);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    return label;
}

/** \brief  Update sensitivity of the widgets
 *
 * Update sensitivity based on emulation-enabled and (on systems with OSS/ALSA)
 * OSS/ALSA availability and selection.
 */
static void set_widgets_sensitivity(void)
{
    gboolean midi_active;
    gboolean alsa_active = TRUE;    /* always true if ALSA not present */
    gboolean oss_active  = TRUE;    /* always true if OSS not present */
#if defined(USE_ALSA) && defined(USE_OSS)
    GtkComboBox *driver = GTK_COMBO_BOX(midi_driver_widget);
#endif

    midi_active = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(midi_enable_widget));
#if defined(USE_ALSA) && defined(USE_OSS)
    /* Both OSS and ALSA means we have a combo box to select between them: */
    alsa_active = gtk_combo_box_get_active(driver) == MIDI_DRIVER_ALSA;
    oss_active  = gtk_combo_box_get_active(driver) == MIDI_DRIVER_OSS;
#endif

    if (midi_driver_widget != NULL) {
        gtk_widget_set_sensitive(midi_driver_widget, midi_active);
    }
    if (midi_mode_widget != NULL) {
        gtk_widget_set_sensitive(midi_mode_widget,   midi_active);
    }
    if (midi_name_widget != NULL) {
        gtk_widget_set_sensitive(midi_name_widget,   midi_active && alsa_active);
    }
    if (midi_in_widget != NULL) {
        gtk_widget_set_sensitive(midi_in_widget,     midi_active && oss_active);
    }
    if (midi_out_widget != NULL) {
        gtk_widget_set_sensitive(midi_out_widget,    midi_active && oss_active);
    }
}

/** \brief  Extra handler for the "toggled" event of the "Enable" check button
 *
 * \param[in]   widget      widget triggering the event
 * \param[in]   user_data   extra event data (unused)
 */
static void on_midi_enable_widget_toggled(GtkWidget *widget, gpointer user_data)
{
    set_widgets_sensitivity();
}

#if defined(USE_OSS) && defined(USE_ALSA)
/** \brief  Handler for the "changed" event of the midi driver combo box
 *
 * \param[in]   combo       combo box
 * \param[in]   user_data   extra event data (unused)
 */
static void on_combo_changed(GtkComboBox *combo, gpointer user_data)
{
    set_widgets_sensitivity();
}
#endif

#if defined(USE_OSS)
#if 0
static void midi_in_filename_callback(GtkDialog *dialog,
                                      gchar *filename,
                                      gpointer data)
{
    if (filename != NULL) {
        vice_gtk3_resource_entry_set(midi_in_widget, filename);
        g_free(filename);
    }
    gtk_widget_destroy(GTK_WIDGET(dialog));
}
#endif

#if 0
/** \brief  Handler for the "clicked" event of the MIDI-In "Browse" button
 *
 * \param[in]   widget      button
 * \param[in]   user_data   text entry to store new filename
 */
static void on_midi_in_browse(GtkWidget *widget, gpointer user_data)
{
    const char *filters[] = { "midi*", "seq*", NULL };

    vice_gtk3_open_file_dialog(
            "Select MIDI In device",
            "MIDI devices", filters, "/dev",
            midi_in_filename_callback,
            NULL);
}
#endif

#if 0
static void midi_out_filename_callback(GtkDialog *dialog,
                                       gchar *filename,
                                       gpointer data)
{
    if (filename != NULL) {
        vice_gtk3_resource_entry_set(midi_out_widget, filename);
        g_free(filename);
    }
    gtk_widget_destroy(GTK_WIDGET(dialog));
}
#endif

#if 0
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
    g_signal_connect_unlocked(check,
                              "toggled",
                              G_CALLBACK(on_midi_enable_widget_toggled),
                              NULL);
    return check;
}

/** \brief  Create MIDI emulation mode widget
 *
 * \return  GtkComboBoxText
 */
static GtkWidget *create_midi_mode_widget(void)
{
    return vice_gtk3_resource_combo_int_new("MIDIMode", midi_modes);
}



#if defined(USE_OSS) && defined(USE_ALSA)
/** \brief  Create MIDI driver selection widget
 *
 * \return  GtkComboBoxText
 */
static GtkWidget *create_midi_driver_widget(void)
{
    GtkWidget *combo;

    combo = vice_gtk3_resource_combo_int_new("MIDIDriver", midi_drivers);
    g_signal_connect_unlocked(combo,
                              "changed",
                              G_CALLBACK(on_combo_changed),
                              NULL);
    return combo;
}
#endif

#if defined(USE_OSS)
/** \brief  Create resource file chooser for OSS MIDI In/Out device selection
 *
 * \param[in]   resource    resource name
 * \param[in]   title       title for the file chooser dialog
 *
 * \return  GtkEntry
 */
static GtkWidget *create_oss_dev_chooser(const char *resource, const char *title)
{
    GtkWidget  *chooser;
    const char *patterns[] = { "midi*", "seq*", NULL };

    chooser = vice_gtk3_resource_filechooser_new(resource,
                                                 GTK_FILE_CHOOSER_ACTION_OPEN);
    vice_gtk3_resource_filechooser_set_custom_title(chooser, title);
    vice_gtk3_resource_filechooser_set_directory(chooser, "/dev");
    vice_gtk3_resource_filechooser_set_filter(chooser,
                                              "MIDI devices",
                                              patterns,
                                              TRUE);
    gtk_widget_set_hexpand(chooser, TRUE);
    return chooser;
}
#endif

/** \fn create_arch_layout
 * \brief   Create arch-dependent widget layout
 *
 * Add widgets to the main grid, dependent on the host OS.
 *
 * \param[in]   grid    main grid
 * \param[in]   row     row in \a grid to start adding widgets
 * \param[in]   columns number of columns in \a grid, for proper column span
 *
 * \return  row in \a grid for next widgets
 */

/* MacOS */
#if defined(MACOS_COMPILE)
static int create_arch_layout(GtkWidget *grid, int row, int columns)
{
    GtkWidget *label;

    /* MIDI client name */
    label = label_helper("MIDI client name");
    midi_name_widget = vice_gtk3_resource_entry_new("MIDIName");
    gtk_widget_set_hexpand(midi_name_widget, TRUE);
    gtk_grid_attach(GTK_GRID(grid), label,            0, row, 1,           1);
    gtk_grid_attach(GTK_GRID(grid), midi_name_widget, 1, row, columns - 1, 1);
    row++;

    /* A text entry field to input the port name */
    label = label_helper("MIDI In Name");
    midi_in_widget = vice_gtk3_resource_entry_new("MIDIInName");
    gtk_widget_set_hexpand(midi_in_widget, TRUE);
    gtk_grid_attach(GTK_GRID(grid), label,          0, row, 1,           1);
    gtk_grid_attach(GTK_GRID(grid), midi_in_widget, 1, row, columns - 1, 1);
    row++;

    /* A text entry field to input the port name */
    label = label_helper("MIDI Out Name");
    midi_out_widget = vice_gtk3_resource_entry_new("MIDIOutName");
    gtk_widget_set_hexpand(midi_out_widget, TRUE);
    gtk_grid_attach(GTK_GRID(grid), label,           0, row, 1,           1);
    gtk_grid_attach(GTK_GRID(grid), midi_out_widget, 1, row, columns - 1, 1);
    row++;

    return row;
}

#elif defined(WINDOWS_COMPILE)
static int create_arch_layout(GtkWidget *grid, int row, int columns)
{
    GtkWidget *label;

    /* A drop down list containing the available devices */
    label = label_helper("MIDI In");
    midi_in_widget = midi_device_widget_create(0);
    gtk_widget_set_hexpand(midi_in_widget, TRUE);
    gtk_grid_attach(GTK_GRID(grid), label,          0, row, 1,           1);
    gtk_grid_attach(GTK_GRID(grid), midi_in_widget, 1, row, columns - 1, 1);
    row++;

    /* A drop down list containing the available devices */
    label = label_helper("MIDI Out");
    midi_out_widget = midi_device_widget_create(1);
    gtk_widget_set_hexpand(midi_out_widget, TRUE);
    gtk_grid_attach(GTK_GRID(grid), label,           0, row, 1,           1);
    gtk_grid_attach(GTK_GRID(grid), midi_out_widget, 1, row, columns - 1, 1);
    row++;

    return row;
}

#else
/* UNIX (Linux, BSD, (Haiku?), but not MacOS) */
static int create_arch_layout(GtkWidget *grid, int row, int columns)
{
#if defined(USE_ALSA) || defined(USE_OSS)
    GtkWidget  *label;
#endif

    /* Driver selection if both ALSA and OSS are configured */
#if defined(USE_ALSA) && defined(USE_OSS)
    label = label_helper("MIDI driver");
    midi_driver_widget = create_midi_driver_widget();
    gtk_grid_attach(GTK_GRID(grid), label,              0, row, 1,           1);
    gtk_grid_attach(GTK_GRID(grid), midi_driver_widget, 1, row, columns - 1, 1);
    row++;
#endif

    /* ALSA client name */
#if defined(USE_ALSA)
    label = label_helper("ALSA client name");
    midi_name_widget = vice_gtk3_resource_entry_new("MIDIName");
    gtk_widget_set_hexpand(midi_name_widget, TRUE);
    gtk_grid_attach(GTK_GRID(grid), label,            0, row, 1,           1);
    gtk_grid_attach(GTK_GRID(grid), midi_name_widget, 1, row, columns - 1, 1);
    row++;
#endif

    /* OSS MIDI In device at /dev */
#if defined(USE_OSS)
    label = label_helper("OSS MIDI In");
    midi_in_widget = create_oss_dev_chooser("MIDIInDev",
                                            "Select OSS MIDI In device");
    gtk_grid_attach(GTK_GRID(grid), label,          0, row, 1,           1);
    gtk_grid_attach(GTK_GRID(grid), midi_in_widget, 1, row, columns - 1, 1);
    row++;

    /* OSS MIDI Out device at /dev */
    label = label_helper("OSS MIDI Out");
    midi_out_widget = create_oss_dev_chooser("MIDIOutDev",
                                             "Select OSS MIDI Out device");
    gtk_grid_attach(GTK_GRID(grid), label,           0, row, 1,           1);
    gtk_grid_attach(GTK_GRID(grid), midi_out_widget, 1, row, columns - 1, 1);
    row++;
#endif

    return row;
}
#endif


/** \brief  Create MIDI settings widget
 *
 * \param[in]   parent  parent widget (unused)
 *
 * \return  GtkGrid
 */
GtkWidget *settings_midi_widget_create(GtkWidget *parent)
{
#define NUM_COLS 2
    GtkWidget *grid;
    GtkWidget *label;
    int        row = 0;

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);

    midi_enable_widget = create_midi_enable_widget();
    gtk_grid_attach(GTK_GRID(grid), midi_enable_widget, 0, row, NUM_COLS, 1);
    row++;

    if (machine_class != VICE_MACHINE_VIC20) {
        label = label_helper("MIDI mode");
        midi_mode_widget = create_midi_mode_widget();
        gtk_grid_attach(GTK_GRID(grid), label,            0, row, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), midi_mode_widget, 1, row, NUM_COLS - 1, 1);
        row++;
    }

    row = create_arch_layout(grid, row, NUM_COLS);
#undef NUM_COLS
    set_widgets_sensitivity();
    gtk_widget_show_all(grid);
    return grid;
}

#if 0
/** \brief  Create MIDI settings widget
 *
 * \param[in]   parent  parent widget (unused)
 *
 * \return  GtkGrid
 */
GtkWidget *settings_midi_widget_create_old(GtkWidget *parent)
{
#define NUM_COLS 3
    GtkWidget *grid;
    GtkWidget *label;
    int        row = 0;

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);

    midi_enable_widget = create_midi_enable_widget();
    gtk_grid_attach(GTK_GRID(grid), midi_enable_widget, 0, row, NUM_COLS, 1);
    row++;

    if (machine_class != VICE_MACHINE_VIC20) {
        label = label_helper("MIDI mode");
        gtk_grid_attach(GTK_GRID(grid), label, 0, row, 1, 1);
        midi_mode_widget = create_midi_mode_widget();
        gtk_grid_attach(GTK_GRID(grid), midi_mode_widget, row, 1, 1, 1);
        row++;
    }

#if defined (USE_OSS) && defined (USE_ALSA)
    /* Unix only, use ALSA or OSS? */
    label = label_helper("MIDI driver");
    gtk_grid_attach(GTK_GRID(grid), label, 0, row, 1, 1);
    midi_driver_widget = create_midi_driver_widget();
    gtk_grid_attach(GTK_GRID(grid), midi_driver_widget, 1, row, 1, 1);
    row++;
#endif

#if defined (MACOS_COMPILE) || defined (USE_ALSA)
    /* macOS, Linux (ALSA) - name of the MIDI client */
#if defined (MACOS_COMPILE)
    label = label_helper("MIDI Client Name");
#else
    label = label_helper("ALSA Client Name");
#endif
    gtk_grid_attach(GTK_GRID(grid), label, 0, row, 1, 1);
    midi_name_widget = vice_gtk3_resource_entry_new("MIDIName");
    gtk_widget_set_hexpand(midi_name_widget, TRUE);
    gtk_grid_attach(GTK_GRID(grid), midi_name_widget, 1, row, 1, 1);
    row++;
#endif

#ifdef WINDOWS_COMPILE
    /* A drop down list containing the available devices */
    label = label_helper("MIDI In");
    gtk_grid_attach(GTK_GRID(grid), label, 0, row, 1, 1);
    midi_in_widget = midi_device_widget_create(0);
    gtk_widget_set_hexpand(midi_in_widget, TRUE);
    gtk_grid_attach(GTK_GRID(grid), midi_in_widget, 1, row, 1, 1);
#endif

#ifdef MACOS_COMPILE
    /* A text entry field to input the port name */
    label = label_helper("MIDI In Name");
    gtk_grid_attach(GTK_GRID(grid), label, 0, row, 1, 1);
    midi_in_widget = vice_gtk3_resource_entry_new("MIDIInName");
    gtk_widget_set_hexpand(midi_in_widget, TRUE);
    gtk_grid_attach(GTK_GRID(grid), midi_in_widget, 1, row, 1, 1);
#endif

#if defined(USE_OSS)
    /* A text enrty field+browse button to enter the device file */
    label = label_helper("OSS MIDI In");
    gtk_grid_attach(GTK_GRID(grid), label, 0, row, 1, 1);
    midi_in_widget = vice_gtk3_resource_entry_new("MIDIInDev");
    gtk_widget_set_hexpand(midi_in_widget, TRUE);
    gtk_grid_attach(GTK_GRID(grid), midi_in_widget, 1, row, 1, 1);

    /* the browse button, only needed for OSS (for browsing to the device file) */
    midi_in_browse = gtk_button_new_with_label("Browse ...");
    g_signal_connect(midi_in_browse, "clicked", G_CALLBACK(on_midi_in_browse),
            (gpointer)midi_in_widget);
    gtk_grid_attach(GTK_GRID(grid), midi_in_browse, 2, row, 1, 1);
#endif
    row++;

#ifdef WINDOWS_COMPILE
    /* A drop down list containing the available devices */
    label = label_helper("MIDI Out");
    gtk_grid_attach(GTK_GRID(grid), label, 0, row, 1, 1);
    midi_out_widget = midi_device_widget_create(1);
    gtk_widget_set_hexpand(midi_out_widget, TRUE);
    gtk_grid_attach(GTK_GRID(grid), midi_out_widget, 1, row, 1, 1);
#endif

#ifdef MACOS_COMPILE
    /* A text entry field to input the port name */
    label = label_helper("MIDI Out Name");
    gtk_grid_attach(GTK_GRID(grid), label, 0, row, 1, 1);
    midi_out_widget = vice_gtk3_resource_entry_new("MIDIOutName");
    gtk_widget_set_hexpand(midi_out_widget, TRUE);
    gtk_grid_attach(GTK_GRID(grid), midi_out_widget, 1, row, 1, 1);
#endif

#if defined(USE_OSS)
    /* A text enrty field+browse button to enter the device file */
    label = label_helper("OSS MIDI Out");
    gtk_grid_attach(GTK_GRID(grid), label, 0, row, 1, 1);
    midi_out_widget = vice_gtk3_resource_entry_new("MIDIOutDev");
    gtk_widget_set_hexpand(midi_out_widget, TRUE);
    gtk_grid_attach(GTK_GRID(grid), midi_out_widget, 1, row, 1, 1);

    /* the browse button, only needed for OSS (for browsing to the device file) */
    midi_out_browse = gtk_button_new_with_label("Browse ...");
    g_signal_connect(midi_out_browse, "clicked",
            G_CALLBACK(on_midi_out_browse), (gpointer)midi_out_widget);
    gtk_grid_attach(GTK_GRID(grid), midi_out_browse, 2, row, 1, 1);
#endif
    row++;
#undef NUM_COLS
    gtk_widget_show_all(grid);
    return grid;
}
#endif

#endif /* HAVE_MIDI */
