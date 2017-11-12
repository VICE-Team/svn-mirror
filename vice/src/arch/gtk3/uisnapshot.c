/** \file   src/arch/gtk3/uisnapshot.c
 * \brief   Snapshot dialogs and menu item handlers
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
#include <stdbool.h>
#include <stdlib.h>

#include "lib.h"
#include "basewidgets.h"
#include "widgethelpers.h"
#include "debug_gtk3.h"
#include "machine.h"
#include "resources.h"
#include "filechooserhelpers.h"
#include "openfiledialog.h"
#include "savefiledialog.h"
#include "basedialogs.h"
#include "interrupt.h"
#include "vsync.h"
#include "snapshot.h"

#include "uisnapshot.h"

/* this function lives in ui.c */
extern int ui_emulation_is_paused(void);


static void save_snapshot_dialog(GtkWidget *parent)
{
    GtkWidget *dialog;
    GtkWidget *extra;
    GtkWidget *roms_widget;
    GtkWidget *disks_widget;
    gint response_id;
    int save_roms;
    int save_disks;

    dialog = gtk_file_chooser_dialog_new("Save snapshot file",
            GTK_WINDOW(gtk_widget_get_toplevel(parent)),
            GTK_FILE_CHOOSER_ACTION_SAVE,
            "Save", GTK_RESPONSE_ACCEPT,
            "Cancel", GTK_RESPONSE_CANCEL,
            NULL, NULL);
    gtk_window_set_transient_for(GTK_WINDOW(dialog),
            GTK_WINDOW(gtk_widget_get_toplevel(parent)));

    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog),
            create_file_chooser_filter(file_chooser_filter_snapshot, TRUE));

    /* create extras widget */
    extra = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(extra), 16);

    disks_widget = gtk_check_button_new_with_label("Save attached disks");
    roms_widget = gtk_check_button_new_with_label("Save attached ROMs");
    gtk_grid_attach(GTK_GRID(extra), disks_widget, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(extra), roms_widget, 1, 0, 1, 1);
    gtk_widget_show_all(extra);

    gtk_file_chooser_set_extra_widget(GTK_FILE_CHOOSER(dialog), extra);

    response_id = gtk_dialog_run(GTK_DIALOG(dialog));
    save_roms = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(roms_widget));
    save_disks = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(disks_widget));

    debug_gtk3("response_id = %d\n", response_id);
    debug_gtk3("save disks = %s\n", save_disks ? "YES" : "NO");
    debug_gtk3("save ROMs = %s\n", save_roms ? "YES" : "NO");

    if (response_id == GTK_RESPONSE_ACCEPT) {
        gchar *filename;

        filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        if (filename != NULL) {
            if (machine_write_snapshot(filename, save_roms, save_disks, 0) < 0) {
                snapshot_display_error();
            } else {
                debug_gtk3("Wrote snapshot file '%s'\n", filename);
            }
            g_free(filename);
        }
    }
    gtk_widget_destroy(dialog);
}


static void load_snapshot_trap(uint16_t addr, void *data)
{
    const char *filters[] = { ".vsf", NULL };
    gchar *filename;

    vsync_suspend_speed_eval();

    filename = ui_open_file_dialog(GTK_WIDGET(data), "Open snapshot file",
            "Snapshot files", filters, NULL);
    if (filename != NULL) {
        /* load snapshot */
        if (machine_read_snapshot(filename, 0) < 0) {
            snapshot_display_error();
        }
        g_free(filename);
    }
}


static void save_snapshot_trap(uint16_t addr, void *data)
{
    vsync_suspend_speed_eval();
    save_snapshot_dialog(data);
}

void uisnapshot_open_file(GtkWidget *parent, gpointer user_data)
{
    if (!ui_emulation_is_paused()) {
        interrupt_maincpu_trigger_trap(load_snapshot_trap, (void *)parent);
    } else {
        load_snapshot_trap(0, (void *)parent);
    }
}


void uisnapshot_save_file(GtkWidget *parent, gpointer user_data)
{
    if (!ui_emulation_is_paused()) {
        interrupt_maincpu_trigger_trap(save_snapshot_trap, (void *)parent);
    } else {
        save_snapshot_trap(0, (void *)parent);
    }
}


