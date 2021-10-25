/** \file   tapeportdeviceswidget.c
 * \brief   Tape port devices widget
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 */

/*
 * $VICERES VirtualDevice1          -xscpu64 -vsid
 * $VICERES TapePort1Device         -xscpu64 -vsid
 * $VICERES DatasetteResetWithCPU   -xscpu64 -vsid
 * $VICERES DatasetteZeroGapDelay   -xscpu64 -vsid
 * $VICERES DatasetteSpeedTuning    -xscpu64 -vsid
 * $VICERES DatasetteTapeWobble     -xscpu64 -vsid
 * $VICERES CPClockF83Save          -xscpu64 -vsid
 * $VICERES TapecartUpdateTCRT      x64 x64sc x128
 * $VICERES TapecartOptimizeTCRT    x64 x64sc x128
 * $VICERES TapecartLogLevel        x64 x64sc x128
 * $VICERES TapecartTCRTFilename    x64 x64sc x128
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
#include <stdlib.h>

#include "basedialogs.h"
#include "basewidgets.h"
#include "debug_gtk3.h"
#include "lib.h"
#include "machine.h"
#include "resources.h"
#include "savefiledialog.h"
#include "tapeport.h"
#include "ui.h"
#include "widgethelpers.h"

#include "tapeportdeviceswidget.h"

/** \brief  Column indexes in the tapeport devices model
 */
enum {
    COL_DEVICE_ID,          /**< device ID (int) */
    COL_DEVICE_NAME,        /**< device name (str) */
    COL_DEVICE_TYPE_ID,     /**< device type (int) */
    COL_DEVICE_TYPE_DESC    /**< device type description (str) */
};

/** \brief  List of log levels and their descriptions for the Tapecart
 */
static vice_gtk3_combo_entry_int_t tcrt_loglevels[] = {
    { "0 (errors only)",                            0 },
    { "1 (0 plus mode changes and command bytes)",  1 },
    { "2 (1 plus command parameter details)",       2 },
    { NULL, -1 }
};


/*
 * Reference to widgets to be able to enable/disabled them through event
 * handlers
 */

/** \brief  Tape port #1 device combo */
static GtkWidget *port1_type = NULL;

/** \brief  Tape port #2 device combo */
static GtkWidget *port2_type = NULL;

/** \brief  Datasette device traps toggle button */
static GtkWidget *ds_traps = NULL;

/** \brief  Datasette reset toggle button */
static GtkWidget *ds_reset = NULL;

/** \brief  Datasette zerogap delay spine button */
static GtkWidget *ds_zerogap = NULL;

/** \brief  Datasette speed tuning spin button */
static GtkWidget *ds_speed = NULL;

/** \brief  Datasette wobble frequency spin button */
static GtkWidget *ds_wobblefreq = NULL;

/** \brief  Datasette wobble amplitude spin button */
static GtkWidget *ds_wobbleamp = NULL;

/** \brief  Datasette align spin button */
static GtkWidget *ds_align = NULL;

/** \brief  Datasette sound emulation toggle button */
static GtkWidget *ds_sound = NULL;

/** \brief  F83 RTC toggle button */
static GtkWidget *f83_rtc = NULL;

/** \brief  Tapecart save-when-changed toggle button */
static GtkWidget *tapecart_update = NULL;

/** \brief  Tapecart optimize-when-saving toggle button */
static GtkWidget *tapecart_optimize = NULL;

/** \brief  Tapecart log level radiogroup */
static GtkWidget *tapecart_loglevel = NULL;

/** \brief  Tapecart filename entry
 *
 * TODO:    Replace with resourcebrowser.c
 */
static GtkWidget *tapecart_filename = NULL;

/** \brief  Tapecart browse button
 *
 * TODO:    Replace with resourcebrowser.c
 */
static GtkWidget *tapecart_browse = NULL;

/** \brief  Tapecart flush button */
static GtkWidget *tapecart_flush = NULL;

/** \brief  Tapecart flush function pointer
 *
 * Required to work around VSID linking issues.
 */
static int (*tapecart_flush_func)(void) = NULL;


/** \brief  Set Datasette widget active/inactive
 *
 * \param[in]   state   status
 */
static void set_datasette_active(int state)
{
    gtk_widget_set_sensitive(ds_reset, state);
    gtk_widget_set_sensitive(ds_zerogap, state);
    gtk_widget_set_sensitive(ds_speed, state);
    gtk_widget_set_sensitive(ds_wobblefreq, state);
    gtk_widget_set_sensitive(ds_wobbleamp, state);
    gtk_widget_set_sensitive(ds_align, state);
    gtk_widget_set_sensitive(ds_sound, state);
    gtk_widget_set_sensitive(ds_traps, state);
}

/** \brief Set CP Clock F83 widget active/inactive
 *
 * \param[in]   state   status
 */
static void set_f83_active(int state)
{
    gtk_widget_set_sensitive(f83_rtc,state);
}


/** \brief  Set tapecart active/inactive
 *
 * \param[in]   state   status
 */
static void set_tapecart_active(int state)
{
    gtk_widget_set_sensitive(tapecart_update, state);
    gtk_widget_set_sensitive(tapecart_optimize, state);
    gtk_widget_set_sensitive(tapecart_loglevel, state);
    gtk_widget_set_sensitive(tapecart_filename, state);
    gtk_widget_set_sensitive(tapecart_browse, state);
    gtk_widget_set_sensitive(tapecart_flush, state);
}


/** \brief  Set individual options active/inactive
 *
 * \param[in]   id      id of active device
 */
static void set_options_widgets_sensitivity(int id)
{
    set_datasette_active(id == TAPEPORT_DEVICE_DATASETTE);
    set_f83_active(id == TAPEPORT_DEVICE_CP_CLOCK_F83);
     if (machine_class == VICE_MACHINE_C64 ||
         machine_class == VICE_MACHINE_C64SC||
         machine_class == VICE_MACHINE_C128) {
        set_tapecart_active(id == TAPEPORT_DEVICE_TAPECART);
     }
}


/** \brief  Callback for the tapecart file chooser dialog
 *
 * \param[in,out]   dialog      file chooser dialog
 * \param[in,out]   filename    filename (NULL cancels)
 * \param[in]       data        extra data (unused)
 *
 * TODO:    Replace with resourcebrowser.c
 */
static void browse_filename_callback(GtkDialog *dialog,
                                     gchar *filename,
                                     gpointer data)
{
    if (filename != NULL) {
        vice_gtk3_resource_entry_full_set(tapecart_filename, filename);
        g_free(filename);
    }
    gtk_widget_destroy(GTK_WIDGET(dialog));
}


/** \brief  Handler for the 'clicked' event of the tapecart browse button
 *
 * \param[in]   widget      tapecart browse button
 * \param[in]   user_data   unused
 *
 * TODO:    Replace with resourcebrowser.c
 */
static void on_tapecart_browse_clicked(GtkWidget *widget, gpointer user_data)
{
    GtkWidget *dialog;

    /* TODO: use existing filename, if any */
    dialog = vice_gtk3_open_file_dialog(
            "Select tapecart file",
            NULL, NULL, NULL,
            browse_filename_callback,
            NULL);
    gtk_widget_show(dialog);
}


/** \brief  Handler for the 'clicked' event of the tapecart flush button
 *
 * \param[in]   widget  button (unused)
 * \param[in]   data    extra event data (unused)
 */
static void on_tapecart_flush_clicked(GtkWidget *widget, gpointer data)
{
    tapecart_flush_func();
}


/** \brief  Create widgets for the datasette
 *
 * TODO:    Someone needs to check the spin button bounds and steps for sane
 *          values.
 *
 * \return  GtkGrid
 */
static GtkWidget *create_datasette_widget(void)
{
    GtkWidget *grid;
    GtkWidget *label;

    grid = vice_gtk3_grid_new_spaced(VICE_GTK3_DEFAULT, VICE_GTK3_DEFAULT);
//    g_object_set(G_OBJECT(grid), "margin-top", 16, NULL);

    label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(label), "<b>Datasette C2N</b>");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 4, 1);

    ds_traps = vice_gtk3_resource_check_button_new("VirtualDevice1",
            "Enable Virtual Device (required for t64)");
    g_object_set(ds_traps, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), ds_traps, 0, 1, 4, 1);

    ds_reset = vice_gtk3_resource_check_button_new("DatasetteResetWithCPU",
            "Reset datasette with CPU");
    g_object_set(ds_reset, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), ds_reset, 0, 2, 4, 1);

    ds_sound = vice_gtk3_resource_check_button_new("DatasetteSound",
            "Enable datasette sound");
    g_object_set(ds_sound, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), ds_sound, 0, 3, 4, 1);

    label = gtk_label_new("Zero gap delay:");
    g_object_set(label, "margin-left", 16, NULL);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    ds_zerogap = vice_gtk3_resource_spin_int_new("DatasetteZeroGapDelay",
            0, 50000, 100);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 4, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), ds_zerogap, 1, 4, 1, 1);

    label = gtk_label_new("TAP v0 gap speed tuning:");
    g_object_set(label, "margin-left", 16, NULL);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    ds_speed = vice_gtk3_resource_spin_int_new("DatasetteSpeedTuning",
            0, 50, 1);
    gtk_grid_attach(GTK_GRID(grid), label, 2, 4, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), ds_speed, 3, 4, 1, 1);

    label = gtk_label_new("Tape wobble frequency:");
    g_object_set(label, "margin-left", 16, NULL);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    ds_wobblefreq = vice_gtk3_resource_spin_int_new("DatasetteTapeWobbleFrequency",
            0, 5000, 10);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 5, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), ds_wobblefreq, 1, 5, 1, 1);

    label = gtk_label_new("Tape wobble amplitude:");
    g_object_set(label, "margin-left", 16, NULL);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    ds_wobbleamp = vice_gtk3_resource_spin_int_new("DatasetteTapeWobbleAmplitude",
            0, 5000, 10);
    gtk_grid_attach(GTK_GRID(grid), label, 2, 5, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), ds_wobbleamp, 3, 5, 1, 1);

    label = gtk_label_new("Tape alignment error");
    g_object_set(label, "margin-left", 16, NULL);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    ds_align = vice_gtk3_resource_spin_int_new("DatasetteTapeAzimuthError",
            0, 25000, 100);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 6, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), ds_align, 1, 6, 1, 1);

    return grid;
}


/** \brief  Create widget to handler the CP Clock F83 resources
 *
 * \return  GtkGrid
 */
static GtkWidget *create_f83_widget(void)
{
    GtkWidget *grid;
    GtkWidget *label;

    grid = vice_gtk3_grid_new_spaced(16, 8);
//    g_object_set(G_OBJECT(grid), "margin-top", 16, NULL);

    label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(label), "<b>CP Clock F83</b>");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);

    f83_rtc = vice_gtk3_resource_check_button_new("CPClockF83Save",
            "Save RTC data when changed");
    g_object_set(f83_rtc, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), f83_rtc, 0, 1, 1, 1);

    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Create widget to handle the tapecart resources
 *
 * \return  GtkGrid
 */
static GtkWidget *create_tapecart_widget(void)
{
    GtkWidget *grid;
    GtkWidget *label;
    GtkWidget *wrapper;
    int row = 0;

    grid = vice_gtk3_grid_new_spaced(16, 8);
    /* add some extra vertical spacing */
//    g_object_set(G_OBJECT(grid), "margin-top", 16, NULL);

    /* Tapecart label */
    label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(label), "<b>Tapecart</b>");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label, 0, row, 4, 1);
    row++;

    /* wrapper for update/optimize check buttons */
    wrapper = gtk_grid_new();

    /* TapecartUpdateTCRT */
    tapecart_update = vice_gtk3_resource_check_button_new(
            "TapecartUpdateTCRT", "Save data when changed");
    g_object_set(tapecart_update, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(wrapper), tapecart_update, 0, 0, 1, 1);

    /* TapecartOptimizeTCRT */
    tapecart_optimize = vice_gtk3_resource_check_button_new(
            "TapecartOptimizeTCRT", "Optimize data when changed");
    g_object_set(tapecart_optimize, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(wrapper), tapecart_optimize, 1, 0, 1, 1);

    gtk_grid_attach(GTK_GRID(grid), wrapper, 0, row, 4, 1);
    row++;

    label = gtk_label_new("Log level:");
    g_object_set(label, "margin-left", 16, NULL);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label, 0, row, 1, 1);
    tapecart_loglevel = vice_gtk3_resource_combo_box_int_new(
            "TapecartLogLevel", tcrt_loglevels);
    g_object_set(tapecart_loglevel, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), tapecart_loglevel, 1, row, 3, 1);
    row++;

    /* TapecartTCRTFilename */
    label = gtk_label_new("TCRT Filename:");
    g_object_set(label, "margin-left", 16, NULL);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label, 0, row, 1, 1);
    tapecart_filename = vice_gtk3_resource_entry_full_new(
            "TapecartTCRTFilename");
    g_object_set(tapecart_filename, "margin-left", 16, NULL);
    gtk_widget_set_hexpand(tapecart_filename, TRUE);
    gtk_grid_attach(GTK_GRID(grid), tapecart_filename, 1, row, 1, 1);

    tapecart_browse = gtk_button_new_with_label("Browse ...");
    gtk_grid_attach(GTK_GRID(grid), tapecart_browse, 2, row, 1, 1);

    /* Flush button */
    tapecart_flush = gtk_button_new_with_label("Save image");
    gtk_grid_attach(GTK_GRID(grid), tapecart_flush, 3, row, 1, 1);

    g_signal_connect(tapecart_browse, "clicked",
            G_CALLBACK(on_tapecart_browse_clicked), NULL);
    g_signal_connect(tapecart_flush, "clicked",
            G_CALLBACK(on_tapecart_flush_clicked), NULL);

    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Handler for the 'changed' event of the device combobox
 *
 * Sets the active tapeport device via the "TapePort[12]Device" resource.
 *
 * \param[in]   combo       device combo box
 * \param[in]   portnum     tape port number
 */
static void on_device_changed(GtkComboBox *combo, gpointer portnum)
{
    GtkTreeModel *model;
    GtkTreeIter iter;

    model = gtk_combo_box_get_model(combo);
    if (gtk_combo_box_get_active_iter(combo, &iter)) {
        gint id;
        gchar *name;
        int port;

        port = GPOINTER_TO_INT(portnum);

        gtk_tree_model_get(model,
                           &iter,
                           COL_DEVICE_ID, &id,
                           COL_DEVICE_NAME, &name,
                           -1);
        debug_gtk3("Got device #%d (%s) for port #%d", id, name, port);
        resources_set_int_sprintf("TapePort%dDevice", id, port, NULL);

        set_options_widgets_sensitivity(id);

        g_free(name);
    }
}


/** \brief  Set tapeport device ID
 *
 * Sets the currently selected combobox item via device ID.
 *
 * To avoid updating the related resource via the combobox' event handler, use
 * the \a blocked argument.
 *
 * \param[in]   combo   device combo box
 * \param[in]   id      device ID
 * \param[in]   blocked block 'changed' signal handler
 */
static gboolean set_device_id(GtkComboBox *combo, gint id, gboolean blocked)
{
    GtkTreeModel *model;
    GtkTreeIter iter;
    gulong handler_id;
    gboolean result = FALSE;

    /* do we need to block the 'changed' event handler? */
    if (blocked) {
        /* look up handler ID by callback */
        handler_id = g_signal_handler_find(combo,
                                           G_SIGNAL_MATCH_FUNC,
                                           0,       /* signal_id */
                                           0,       /* detail */
                                           NULL,    /* closure */
                                           on_device_changed,   /* func */
                                           NULL);
        if (handler_id > 0) {
            g_signal_handler_block(combo, handler_id);
        }
    }

    /* iterate the model until we find the device ID */
    model = gtk_combo_box_get_model(combo);
    if (gtk_tree_model_get_iter_first(model, &iter)) {
        do {
            gint current;

            gtk_tree_model_get(model, &iter, COL_DEVICE_ID, &current, -1);
            if (id == current) {
                gtk_combo_box_set_active_iter(combo, &iter);
                result = TRUE;
                break;
            }
        } while (gtk_tree_model_iter_next(model, &iter));
    }

    /* set options checkboxes "greyed-out" state */
    set_options_widgets_sensitivity(id);

    /* unblock signal, if blocked */
    if (blocked) {
        g_signal_handler_unblock(combo, handler_id);
    }

    return result;
}


/** \brief  Create model for the device combobox
 *
 * Create a model with (dev-id, dev-name, dev-type-id, dev-type-desc).
 *
 * \return  model
 */
static GtkListStore *create_device_model(void)
{
    GtkListStore *model;
    GtkTreeIter iter;
    tapeport_desc_t *devices;
    tapeport_desc_t *dev;

    model = gtk_list_store_new(4,
                               G_TYPE_INT,      /* ID */
                               G_TYPE_STRING,   /* name */
                               G_TYPE_INT,      /* type ID */
                               G_TYPE_STRING    /* type description */
                               );
    devices = tapeport_get_valid_devices(TRUE);
    for (dev = devices; dev->name != NULL; dev++) {
        gtk_list_store_append(model, &iter);
        gtk_list_store_set(model,
                           &iter,
                           COL_DEVICE_ID, dev->id,
                           COL_DEVICE_NAME, dev->name,
                           COL_DEVICE_TYPE_ID, dev->device_type,
                           COL_DEVICE_TYPE_DESC, tapeport_get_device_type_desc(dev->device_type),
                           -1);
    }
    lib_free(devices);

    return model;
}


/** \brief  Create combobox for the tapeport devices
 *
 * Create a combobox with valid tapeport devices for current machine.
 *
 * The model of the combobox contains device ID, name and type, of which name
 * is shown and ID is used to set the related resource.
 *
 * \param[in]   port    tape port number (1 or 2 (PET only))
 *
 * \return  GtkComboBox
 *
 * \todo    Try using the device type to create little headers in the combobox,
 *          grouping the devices by type. Might be overkill for some machines
 *          that only have a few tapeport devices, we'll see.
 *          I tried using a second column for the device type description, and
 *          althought it doesn't look bad in the popup list, when the popup
 *          isn't active it looks weird ;)
 *          So for now the device type isn't used.
 */
static GtkWidget *create_device_combobox(int port)
{
    GtkWidget *combo;
    GtkListStore *model;
    GtkCellRenderer *name_renderer;
#if 0
    GtkCellRenderer *type_renderer;
#endif

    /* TODO:    Check if the model can be shared for both ports, perhaps the
     *          second tape port has a different set of supported devices
     */
    model = create_device_model();

    /* create combobox with a single cell renderer for the device name column */
    combo = gtk_combo_box_new_with_model(GTK_TREE_MODEL(model));
    name_renderer = gtk_cell_renderer_text_new();
#if 0
    type_renderer = gtk_cell_renderer_text_new();
#endif
    gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(combo),
                               name_renderer,
                               TRUE);
    gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(combo),
                                   name_renderer,
                                   "text", COL_DEVICE_NAME,
                                   NULL);
#if 0
    gtk_cell_layout_pack_end(GTK_CELL_LAYOUT(combo),
                             type_renderer,
                             TRUE);
    gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(combo),
                                   type_renderer,
                                   "text", COL_DEVICE_TYPE_DESC,
                                   NULL);
#endif

    g_signal_connect(combo,
                     "changed",
                     G_CALLBACK(on_device_changed),
                     GINT_TO_POINTER(port));

    return combo;
}


/** \brief  Create combobox(es) to select device type for port 1 (and 2 for PET)
 *
 * \return  GtkGrid
 */
static GtkWidget *create_device_types_widget(void)
{
    GtkWidget *grid;
    GtkWidget *label;

    grid = vice_gtk3_grid_new_spaced(16, 8);

    /* header */
    label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(label), "<b>Tape port device types</b>");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 2, 1);

    /* first tape port */
    label = gtk_label_new("Tape port #1:");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_widget_set_hexpand(label, FALSE);
    g_object_set(label, "margin-left", 16, NULL);
    port1_type = create_device_combobox(1);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), port1_type, 1, 1, 1, 1);

    /* PET has a second tape port */
    if (machine_class == VICE_MACHINE_PET) {
        label = gtk_label_new("Tape port #2:");
        gtk_widget_set_halign(label, GTK_ALIGN_START);
        gtk_widget_set_hexpand(label, FALSE);
        g_object_set(label, "margin-left", 16, NULL);
        port2_type = create_device_combobox(2);
        gtk_grid_attach(GTK_GRID(grid), label, 0, 2, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), port2_type, 1, 2, 1, 1);
    }

    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Create widget to select/control tape port devices
 *
 * \param[in]   parent  parent widget
 *
 * \return  GtkGrid
 */
GtkWidget *tapeport_devices_widget_create(GtkWidget *parent)
{
    GtkWidget *grid;
    int device_id = 0;

    grid = vice_gtk3_grid_new_spaced(VICE_GTK3_DEFAULT, 32);

    /* comboboxes with the tapeport devices */
    gtk_grid_attach(GTK_GRID(grid), create_device_types_widget(), 0, 0, 1, 1);

    /* datasette device settings */
    gtk_grid_attach(GTK_GRID(grid), create_datasette_widget(), 0, 1, 1, 1);

    /* Cassette Port Clock F83 */
    gtk_grid_attach(GTK_GRID(grid), create_f83_widget(), 0, 2, 1, 1);

    /* TapeCart settings */
    if (machine_class == VICE_MACHINE_C64 ||
        machine_class == VICE_MACHINE_C64SC||
        machine_class == VICE_MACHINE_C128) {
        gtk_grid_attach(GTK_GRID(grid), create_tapecart_widget(), 0, 3, 1, 1);
    }

    /* these need to happen here, after the above widgets are created */
    /* set port1 type using the resource */
    if (resources_get_int("TapePort1Device", &device_id) == 0) {
        set_device_id(GTK_COMBO_BOX(port1_type), device_id, TRUE);
    }
    if (machine_class == VICE_MACHINE_PET) {
        /* set port2 type using the resource */
        if (resources_get_int("TapePort2Device", &device_id) == 0) {
            set_device_id(GTK_COMBO_BOX(port2_type), device_id, TRUE);
        }
    }

    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Set the tapecart flush function
 *
 * This is required to work around vsid not linking against tapecart.
 *
 * \param[in]   func    tapecart flush function
 */
void tapeport_devices_widget_set_tapecart_flush_func(int (*func)(void))
{
    tapecart_flush_func = func;
}
