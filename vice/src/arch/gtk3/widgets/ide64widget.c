/** \file   src/arch/gtk3/widgets/ide64widget.c
 * \brief   Widget to control IDE64 resources
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

#include "machine.h"
#include "resources.h"
#include "debug_gtk3.h"
#include "basewidgets.h"
#include "widgethelpers.h"
#include "basedialogs.h"
#include "openfiledialog.h"
#include "savefiledialog.h"
#if 0
#include "cartimagewidget.h"
#include "cartridge.h"
#endif
#include "ide64.h"

#include "ide64widget.h"

static ui_radiogroup_entry_t revisions[] = {
    { "Version 3", IDE64_VERSION_3 },
    { "Version 4.1", IDE64_VERSION_4_1 },
    { "Version 4.2", IDE64_VERSION_4_2 },
    { NULL, -1 }
};


/** \brief  Handler for the "destroy" event of the main widget
 *
 * \param[in]   widget      widget triggering the event
 * \param[in]   user_data   extra event data (unused
 */
static void on_ide64_widget_destroy(GtkWidget *widget, gpointer user_data)
{
}


static GtkWidget *create_ide64_revision_widget(void)
{
    GtkWidget *grid;
    GtkWidget *label;
    GtkWidget *group;

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 16);

    label = gtk_label_new("IDE64 revision");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);

    group = resource_radiogroup_create("IDE64version", revisions,
            GTK_ORIENTATION_HORIZONTAL);
    gtk_grid_set_column_spacing(GTK_GRID(group), 16);
    gtk_grid_attach(GTK_GRID(grid), group, 1, 0, 1, 1);

    gtk_widget_show_all(grid);
    return grid;
}


static void on_usb_enable_toggled(GtkWidget *widget, gpointer user_data)
{
    gtk_widget_set_sensitive(GTK_WIDGET(user_data),
            gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)));
}


static GtkWidget *create_ide64_usb_widget(void)
{
    GtkWidget *grid;
    GtkWidget *enable;
    GtkWidget *address;
    GtkWidget *label;
    int state;

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 16);

    enable = resource_check_button_create("IDE64USBServer", "Enable USB server");
    label = gtk_label_new("USB server address");
    address = resource_entry_create("IDE64USBServerAddress");
    gtk_widget_set_hexpand(address, TRUE);

    gtk_grid_attach(GTK_GRID(grid), enable, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), label, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), address, 2, 0, 1, 1);

    resources_get_int("IDE64USBServer", &state);
    gtk_widget_set_sensitive(address, state);

    g_signal_connect(enable, "toggled", G_CALLBACK(on_usb_enable_toggled),
            (gpointer)address);

    gtk_widget_show_all(grid);
    return grid;
}


static GtkWidget *create_ide64_clockport_widget(void)
{
    GtkWidget *grid;
    GtkWidget *label;
    GtkWidget *clockport;

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 16);

    label = gtk_label_new("ClockPort device");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    clockport = clockport_device_widget_create("IDE64ClockPort");

    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), clockport, 1, 0, 1, 1);

    gtk_widget_show_all(grid);
    return grid;
}

static GtkWidget *create_ide64_rtc_widget(void)
{
    return resource_check_button_create("IDE64RTCSave", "Enable RTC saving");
}


/** \brief  Handler for the "clicked" event of the HD image "browse" buttons
 *
 * \param[in]       widget      button
 * \param[in,out]   user_data   entry box to store the HD image file
 */
static void on_browse_clicked(GtkWidget *widget, gpointer user_data)
{
    gchar *filename;
    const char *filter_list[] = {
        "*.hdd", "*.iso", "*.fdd", "*.cfa", NULL
    };

    filename = ui_open_file_dialog(widget, "Select disk image file",
            "HD image files", filter_list, NULL);
    if (filename != NULL) {
        GtkEntry *entry = GTK_ENTRY(user_data);
        gtk_entry_set_text(entry, filename);
        g_free(filename);
    }
}



/** \brief  Create widget to select HD image and set geometry
 *
 * \param[in]   device  device number (1-4)
 *
 * \return  GtkGrid
 */
static GtkWidget *create_ide64_device_widget(int device)
{
    GtkWidget *grid;
    GtkWidget *label;
    GtkWidget *entry;
    GtkWidget *browse;
    GtkWidget *autosize;
    GtkWidget *cylinders;
    GtkWidget *heads;
    GtkWidget *sectors;

    char buffer[256];
    char resource[256];

    g_snprintf(buffer, 256, "Device %d settings", device);

    grid = uihelpers_create_grid_with_label(buffer, 3);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 16);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);

    label = gtk_label_new("Image file");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    g_object_set(label, "margin-left", 16, NULL);

    g_snprintf(resource, 256, "IDE64image%d", device);
    entry = resource_entry_create(resource);
    gtk_widget_set_hexpand(entry, TRUE);

    browse = gtk_button_new_with_label("Browse ...");
    g_signal_connect(browse, "clicked", G_CALLBACK(on_browse_clicked),
            (gpointer)entry);

    gtk_grid_attach(GTK_GRID(grid), label, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entry, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), browse, 2, 1, 1, 1);

    /* TODO: enable/disable the cylinders/heads/sectors widget depending on the
     *       state of this toggle button
     */
    autosize = resource_check_button_create_sprintf(
            "IDE64AutodetectSize%d", "Autodetect image size", device);
    g_object_set(autosize, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), autosize, 0, 2, 3, 1);

    label = gtk_label_new("Cylinders");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    g_object_set(label, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 3, 1, 1);

    cylinders = resource_spin_button_int_create_sprintf("IDE64cylinders%d",
            0, 65536, 1024, device);
    gtk_widget_set_hexpand(cylinders, FALSE);
    gtk_grid_attach(GTK_GRID(grid), cylinders, 1, 3, 1, 1);

    label = gtk_label_new("Heads");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    g_object_set(label, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 4, 1, 1);

    heads = resource_spin_button_int_create_sprintf("IDE64heads%d",
            0, 16, 1, device);
    gtk_widget_set_hexpand(heads, FALSE);
    gtk_grid_attach(GTK_GRID(grid), heads, 1, 4, 1, 1);

    label = gtk_label_new("Sectors");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    g_object_set(label, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 5, 1, 1);

    sectors = resource_spin_button_int_create_sprintf("IDE64sectors%d",
            0, 63, 1, device);
    gtk_widget_set_hexpand(heads, FALSE);
    gtk_grid_attach(GTK_GRID(grid), sectors, 1, 5, 1, 1);



    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Create widget to control IDE64 resources
 *
 * \param[in]   parent  parent widget
 *
 * \return  GtkGrid
 */
GtkWidget *ide64_widget_create(GtkWidget *parent)
{
    GtkWidget *grid;
    GtkWidget *wrapper;
    GtkWidget *stack;
    GtkWidget *stack_switcher;
    int d;

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);

    gtk_grid_attach(GTK_GRID(grid), create_ide64_revision_widget(), 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), create_ide64_usb_widget(), 0, 1, 1, 1);

    wrapper = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(wrapper), 16);
    gtk_grid_attach(GTK_GRID(wrapper), create_ide64_rtc_widget(), 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(wrapper), create_ide64_clockport_widget(), 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), wrapper, 0, 2, 1, 1);

    stack = gtk_stack_new();
    gtk_stack_set_transition_type(GTK_STACK(stack),
            GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT);
    gtk_stack_set_transition_duration(GTK_STACK(stack), 1000);

    for (d = 1; d <= 4; d++) {
        char buffer[256];

        g_snprintf(buffer, 256, "Device %d", d);
        gtk_stack_add_titled(GTK_STACK(stack), create_ide64_device_widget(d),
                buffer, buffer);
    }

    stack_switcher = gtk_stack_switcher_new();
    gtk_stack_switcher_set_stack(GTK_STACK_SWITCHER(stack_switcher),
            GTK_STACK(stack));
    gtk_widget_set_halign(stack_switcher, GTK_ALIGN_CENTER);

    gtk_widget_show_all(stack);
    gtk_widget_show_all(stack_switcher);

    gtk_grid_attach(GTK_GRID(grid), stack_switcher, 0, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), stack, 0, 4, 1, 1);

    g_signal_connect(grid, "destroy", G_CALLBACK(on_ide64_widget_destroy),
            NULL);

    gtk_widget_show_all(grid);
    return grid;
}
