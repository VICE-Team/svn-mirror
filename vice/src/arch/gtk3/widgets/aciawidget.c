/** \file   src/arch/gtk3/widgets/aciawidget.c
 * \brief   Widget to control various ACIA related resources
 *
 * Written by
 *  Bas Wassink <b.wassink@ziggo.nl>
 *
 * Controls the following resource(s):
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

#include "lib.h"
#include "widgethelpers.h"
#include "debug_gtk3.h"
#include "resources.h"
#include "openfiledialog.h"

#include "aciawidget.h"


/** \brief  Reference to baud rates list
 */
static int *acia_baud_rates;


/** \brief  List of ACIA devices
 */
static ui_radiogroup_entry_t acia_device_list[] = {
    { "Serial 1", 0 },
    { "Serial 2", 1 },
    { "Dump to file", 2 },
    { "Exec process", 3 },
    { NULL, -1 }
};


/** \brief  Get index in the baud rates list
 *
 * \param[in]   baud    baud rate
 *
 * \return  index or -1 when not found
 */
static int get_baud_rate_index(int baud)
{
    for (int i = 0; acia_baud_rates[i] > 0; i++) {
        if (acia_baud_rates[i] == baud) {
            return i;
        }
    }
    return -1;
}


/** \brief  Handler for the "toggled" event of the ACIA device toggle buttons
 *
 * \param[in]   widget      radio button triggering the event
 * \param[in]   user_data   index of the device in the list (`int`)
 */
static void on_acia_device_changed(GtkWidget *widget, gpointer user_data)
{
    int old_val;
    int new_val;

    resources_get_int("Acia1Dev", &old_val);
    new_val = GPOINTER_TO_INT(user_data);

    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))
            && new_val != old_val) {
        debug_gtk3("setting Acia1Dev to %d\n", new_val);
        resources_set_int("Acia1Dev", new_val);
    }
}


/** \brief  Handler for the "changed" event of a serial device text box
 *
 * \param[in]   widget      text box triggering the event
 * \param[in]   user_data   serial device number (`int`)
 */
static void on_serial_device_changed(GtkWidget *widget, gpointer user_data)
{
    int device = GPOINTER_TO_INT(user_data);
    const gchar *text = gtk_entry_get_text(GTK_ENTRY(widget));

    /* debug_gtk3("got RsDevice %d\n", device); */
    debug_gtk3("setting RsDevice%d to '%s'\n", device, text);
    resources_set_string_sprintf("RsDevice%d", text, device);
}


/** \brief  Handler for the changed event for the baud rate combo boxes
 *
 * \param[in]   widget      combo box triggering the event
 * \param[in]   user_data   serial device number (`int`)
 */
static void on_serial_baud_changed(GtkWidget *widget, gpointer user_data)
{
    int device = GPOINTER_TO_INT(user_data);
    int index = gtk_combo_box_get_active(GTK_COMBO_BOX(widget));

    if (index > 0) {
        debug_gtk3("setting RsDevice%dBaud to %d\n",
                device, acia_baud_rates[index]);
        resources_set_int_sprintf("RsDevice%dBaud",
                acia_baud_rates[index], device);
    }
}


/** \brief  Handler for the "clicked" event of the "browse" buttons
 *
 * \param[in]   widget      button triggering the event
 * \param[in]   user_data   device number (`int`)
 */
static void on_browse_clicked(GtkWidget *widget, gpointer user_data)
{
    int device;
    const char *fdesc = "Serial ports";
    const char *flist[] = { "ttyS*", NULL };
    gchar *filename;
    gchar title[256];

    device = GPOINTER_TO_INT(user_data);
    g_snprintf(title, 256, "Select serial device #%d", device);

    filename = ui_open_file_dialog(widget, title, fdesc, flist, "/dev");
    if (filename != NULL) {

        GtkWidget *grid;
        GtkWidget *entry;

        debug_gtk3("setting RsDevice%d to '%s'\n", device, filename);
        /* resources_set_string_sprintf("RsDevice%d", filename, device); */

        /* update text entry box, forces an update of the resource */
        grid = gtk_widget_get_parent(widget);
        entry = gtk_grid_get_child_at(GTK_GRID(grid), 0, 1);
        gtk_entry_set_text(GTK_ENTRY(entry), filename);

        g_free(filename);
    }
}


/** \brief  Create a Acia device widget for ACIA \a num
 *
 * Creates a widget to select an ACIA device. Uses a custom GObject propery
 * 'AciaNum' on the containing GtkGrid to be able to distingish between which
 * Acia[num]Dev resource needs to be altered.
 *
 * \param[in]   acia_num    ACIA number (1 or 2)
 *
 * \return  GtkGrid
 */
static GtkWidget *create_acia_device_widget(void)
{
    GtkWidget *grid;
    int device;

    resources_get_int("Acia1Dev", &device);

    grid = uihelpers_radiogroup_create("ACIA device",
            acia_device_list,
            on_acia_device_changed,
            device);

    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Create a widget to set an ACIA serial device (path + baud rate)
 *
 * \param[in]   num     serial device number
 *
 * \return  GtkGrid
 */
static GtkWidget *create_acia_serial_device_widget(int num)
{
    GtkWidget *grid;
    GtkWidget *entry;
    GtkWidget *browse;
    GtkWidget *label;
    GtkWidget *combo;
    char *title;
    const char *path;
    int rate;
    int index;

    title = lib_msprintf("Serial %d device", num);
    grid = uihelpers_create_grid_with_label(title, 2);
    g_object_set_data(G_OBJECT(grid), "SerialDevice", GINT_TO_POINTER(num));
    lib_free(title);

    /* add "RsDevice" property to widget to allow the event handlers to set
     * the proper resources
     */
    g_object_set_data(G_OBJECT(grid), "RsDevice", GINT_TO_POINTER(num));

    entry = gtk_entry_new();
    gtk_widget_set_hexpand(entry, TRUE);
    g_object_set(entry, "margin-left", 16, NULL);
    browse = gtk_button_new_with_label("Browse ...");
    g_signal_connect(browse, "clicked", G_CALLBACK(on_browse_clicked),
            GINT_TO_POINTER(num));

    gtk_grid_attach(GTK_GRID(grid), entry, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), browse, 1, 1, 1, 1);

    label = gtk_label_new("Baud rate");
    g_object_set(label, "margin-left", 16, NULL);
    gtk_widget_set_halign(label, GTK_ALIGN_START);

    combo = gtk_combo_box_text_new();
    gtk_widget_set_hexpand(combo, TRUE);
    for (int i = 0; acia_baud_rates[i] > 0; i++) {
        gchar buffer[64];

        g_snprintf(buffer, 64, "%d", acia_baud_rates[i]);
        gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(combo),
                NULL, buffer);
    }


    gtk_grid_attach(GTK_GRID(grid), label, 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), combo, 1, 2, 1, 1);


    /* set resources*/

    resources_get_string_sprintf("RsDevice%d", &path, num);
    if (path != NULL && *path != '\0') {
        gtk_entry_set_text(GTK_ENTRY(entry), path);
    }

    resources_get_int_sprintf("RsDevice%dBaud", &rate, num);
    index = get_baud_rate_index(rate);
    gtk_combo_box_set_active(GTK_COMBO_BOX(combo),index);

    /* connect handlers */
    g_signal_connect(entry, "changed", G_CALLBACK(on_serial_device_changed),
            GINT_TO_POINTER(num));

    /* connect handlers */
    g_signal_connect(combo, "changed", G_CALLBACK(on_serial_baud_changed),
            GINT_TO_POINTER(num));



    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Create ACIA settings widget
 *
 * XXX: currently designed for PET, might need updating when used in other UI's
 *
 * \param[in]   baud    list of baud rates (list of `int`'s, terminated by -1)
 *
 * \return  GtkGrid
 */
GtkWidget *acia_widget_create(int *baud)
{
    GtkWidget *grid;
    GtkWidget *device_widget;
    GtkWidget *serial1_widget;
    GtkWidget *serial2_widget;

    acia_baud_rates = baud;

    grid = uihelpers_create_grid_with_label("ACIA settings", 3);

    device_widget = create_acia_device_widget();
    g_object_set(device_widget, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), device_widget, 0, 1, 1, 1);

    serial1_widget = create_acia_serial_device_widget(1);
    gtk_grid_attach(GTK_GRID(grid), serial1_widget, 1, 1, 1, 1);

    serial2_widget = create_acia_serial_device_widget(2);
    gtk_grid_attach(GTK_GRID(grid), serial2_widget, 2, 1, 1, 1);


    gtk_widget_show_all(grid);
    return grid;
}
