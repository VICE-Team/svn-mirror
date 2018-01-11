/*
 * uiautostart.c - GTK3 autostart settings central widget
 *
 * Controls the following resource(s):
 *  AutostartDelay                    - delay in seconds (0-1000) (integer)
 *  AutostartDelayRandom              - add random delay to autostart (boolean)
 *  AutostartPrgMode                  - autostart mode for PRG/P00 files:
 *                                      VirtualFS (0), InjectIntoRam (1),
 *                                      Disk (2) (integer)
 *  AutostartPrgDiskImage             - disk image to use in case of
 *                                      AutostartPrgMode disk (string)
 *  AutostartRunWithColon             - add ':' after RUN (boolean)
 *  AutostartBasicLoad                - load PRG into BASIC (LOAD"*",8) (boolean)
 *  AutostartWarp                     - use warp during autostart (boolean)
 *  AutostartHandleTrueDriveEmulation - use True Drive Emulation during
 *                                      autostart (boolean)
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
 *
 */

#include "vice.h"

#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>

#include "debug_gtk3.h"
#include "not_implemented.h"

#include "machine.h"
#include "resources.h"
#include "autostart-prg.h"
#include "resourcecheckbutton.h"
#include "widgethelpers.h"
#include "openfiledialog.h"

#include "uiautostart.h"


/** \brief  Autostart modes for PRG files
 */
static ui_radiogroup_entry_t autostart_modes[] = {
    { "Virtual FS", AUTOSTART_PRG_MODE_VFS /* 0 */ },
    { "Inject into RAM", AUTOSTART_PRG_MODE_INJECT /* 1 */ },
    { "Copy to D64", AUTOSTART_PRG_MODE_DISK /* 2 */ },
    { NULL, -1 }
};


/*
 * Event handlers
 */


/** \brief  Handler for the 'AutostartDelay' resource
 *
 * \param[in]   wiget       spin button triggering the event
 * \param[in]   user_data   data for the event (unused)
 */
static void on_fixed_delay_changed(GtkWidget *widget, gpointer user_data)
{
    int delay;

    delay = (int)gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget));
    resources_set_int("AutostartDelay", delay);
}


/** \brief  Handler for the 'AutostartPrgMode' resource
 *
 * \param[in]   wiget       radio button triggering the event
 * \param[in]   user_data   new value for resource (`int`)
 */
static void on_autostartprg_mode_changed(GtkWidget *widget, gpointer user_data)
{
    int mode;

    mode = GPOINTER_TO_INT(user_data);
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
        resources_set_int("AutostartPrgMode", mode);
    }
}


/** \brief  Handler for the 'AutostartPrgDiskImage' resource browser
 *
 * \param[in]   wiget       button triggering the event
 * \param[in]   user_data   reference to the GtkEntry containing the image name
 */
static void on_diskimage_browse_clicked(GtkWidget *widget, gpointer user_data)
{
    char *filename;
    const char *filters[] = { "*.d64", NULL };

    filename = ui_open_file_dialog(widget, "Select D64 image",
            "D64 images", filters, NULL);
    if (filename != NULL) {
        GtkEntry *entry = GTK_ENTRY(user_data);
        GtkEntryBuffer *buffer;

        /* set resource */
        resources_set_string("AutostartPrgDiskImage", filename);
        /* update text entry box */
        buffer = gtk_entry_buffer_new(filename, -1);
        gtk_entry_set_buffer(entry, buffer);
    }
}


/*
 * Widget helpers
 */


/** \brief  Create widget to control "AutostartDelay" resource
 *
 * \return  grid
 */
static GtkWidget *create_fixed_delay_widget(void)
{
    GtkWidget *layout = gtk_grid_new();
    GtkWidget *label = gtk_label_new("Autostart fixed delay:");
    GtkWidget *spin = gtk_spin_button_new_with_range(0.0, 1000.0, 1.0);
    GtkWidget *help = gtk_label_new(
            "0 = machine-specific delay for KERNAL boot");
    int delay;

    gtk_widget_set_halign(label, GTK_ALIGN_START);
    g_object_set(label, "margin", 8, NULL);

    gtk_spin_button_set_digits(GTK_SPIN_BUTTON(spin), 0);

    gtk_grid_attach(GTK_GRID(layout), label, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(layout), spin, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(layout),
            uihelpers_create_indented_label("seconds"), 2, 0, 1, 1);

    resources_get_int("AutostartDelay", &delay);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin), (gdouble)(delay));

    gtk_grid_attach(GTK_GRID(layout), help, 0, 2, 3, 1);

    g_signal_connect(spin, "changed", G_CALLBACK(on_fixed_delay_changed),
            NULL);

    gtk_widget_show_all(layout);
    return layout;
}



/** \brief  Create widget to control "AutostartDelay[Random]" resources
 *
 * \return  grid
 */
static GtkWidget *create_delay_widget(void)
{
    GtkWidget *grid;
    GtkWidget *rnd_delay;
    GtkWidget *fix_delay;

    grid = uihelpers_create_grid_with_label("Delay settings", 3);
    g_object_set(grid, "margin-top", 8, NULL);

    rnd_delay = vice_gtk3_resource_check_button_create( "AutostartDelayRandom",
            "Add random delay");
    g_object_set(rnd_delay, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), rnd_delay, 0, 2, 1, 1);

    fix_delay = create_fixed_delay_widget();
    g_object_set(fix_delay, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), fix_delay, 0, 3, 1, 1);

    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Create widget to control "AutostartPrgDiskImage"
 *
 * \return grid
 */
static GtkWidget *create_prg_diskimage_widget(void)
{
    GtkWidget *grid;
    GtkWidget *inner;
    GtkWidget *entry;
    GtkWidget *button;
    GtkEntryBuffer *buffer;
    const char *path;

    grid = uihelpers_create_grid_with_label("Autostart disk image", 1);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);

    inner = gtk_grid_new();
    resources_get_string("AutostartPrgDiskImage", &path);
    buffer = gtk_entry_buffer_new(path, -1);
    entry = gtk_entry_new_with_buffer(buffer);
    gtk_widget_set_hexpand(entry, TRUE);

    gtk_grid_attach(GTK_GRID(inner), entry, 0, 0, 1, 1);

    button = gtk_button_new_with_label("Browse ...");
    g_signal_connect(button, "clicked", G_CALLBACK(on_diskimage_browse_clicked),
            (gpointer)(entry));
    gtk_grid_attach(GTK_GRID(inner), button, 1, 0, 1, 1);

    gtk_grid_attach(GTK_GRID(grid), inner, 0, 1, 1, 1);

    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Create widget to control PRG-related autostart resources
 *
 * \return  grid
 */
static GtkWidget *create_prg_widget(void)
{
    GtkWidget *grid;
    GtkWidget *colon;
    GtkWidget *basic;
    GtkWidget *mode;
    GtkWidget *image;
    int mode_value;


    grid = uihelpers_create_grid_with_label("PRG settings", 3);
    g_object_set(grid, "margin-top", 8, NULL);

    colon = vice_gtk3_resource_check_button_create("AutostartRunWithColon",
            "Use ':' with RUN");
    g_object_set(colon, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), colon, 0, 1, 1, 1);

    basic = vice_gtk3_resource_check_button_create("AutostartBasicLoad",
            "Load to BASIC start");
    g_object_set(basic, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), basic, 0, 2, 1, 1);

    resources_get_int("AutostartPrgMode", &mode_value);
    mode = uihelpers_radiogroup_create(
            "Autostart PRG mode", autostart_modes,
            on_autostartprg_mode_changed,
            mode_value);
    g_object_set(mode, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), mode, 0, 3, 1, 1);

    image = create_prg_diskimage_widget();
    gtk_grid_attach(GTK_GRID(grid), image, 0, 4, 3, 1);

    gtk_widget_show_all(grid);
    return grid;
}



/** \brief  Create widget to use in the settings dialog for autostart resources
 *
 * \param[in[   parent  parent widget (unused)
 *
 * \return  grid
 */
GtkWidget *uiautostart_create_central_widget(GtkWidget *parent)
{
    GtkWidget *grid;
    GtkWidget *tde;
    GtkWidget *warp;

    grid = gtk_grid_new();
    g_object_set(grid, "margin", 8, NULL);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);

    tde = vice_gtk3_resource_check_button_create(
            "AutostartHandleTrueDriveEmulation",
            "Handle True Drive Emulation on autostart");
    gtk_grid_attach(GTK_GRID(grid), tde, 0, 0, 1, 1);

    warp = vice_gtk3_resource_check_button_create("AutostartWarp",
            "Warp on autostart");
    gtk_grid_attach(GTK_GRID(grid), warp, 0, 1, 1, 1);

    gtk_grid_attach(GTK_GRID(grid), create_delay_widget(),
            0, 2, 1, 1);

    gtk_grid_attach(GTK_GRID(grid), create_prg_widget(),
            0, 3, 1, 1);

    gtk_widget_show_all(grid);

    return grid;
}
