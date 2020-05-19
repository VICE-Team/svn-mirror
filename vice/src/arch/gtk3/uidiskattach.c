/** \file   uidiskattach.c
 * \brief   GTK3 disk-attach dialog
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
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
 */

#include "vice.h"

#include <gtk/gtk.h>

#include "attach.h"
#include "autostart.h"
#include "drive.h"
#include "tape.h"
#include "debug_gtk3.h"
#include "basedialogs.h"
#include "contentpreviewwidget.h"
#include "imagecontents.h"
#include "diskcontents.h"
#include "filechooserhelpers.h"
#include "driveunitwidget.h"
#include "drivenowidget.h"
#include "ui.h"
#include "uistatusbar.h"
#include "uimachinewindow.h"
#include "lastdir.h"

#include "uidiskattach.h"

#ifndef SANDBOX_MODE
/** \brief  File type filters for the dialog
 */
static ui_file_filter_t filters[] = {
    { "Disk images", file_chooser_pattern_disk },
    { "Compressed files", file_chooser_pattern_compressed },
    { "All files", file_chooser_pattern_all },
    { NULL, NULL }
};
#endif

#ifndef SANDBOX_MODE
/** \brief  Array to keep track of drive numbers for each unit
 */
static int unit_drive_nums[DRIVE_NUM] = { 0, 0, 0, 0};
#endif

#ifndef SANDBOX_MODE
/** \brief  Preview widget reference
 */
static GtkWidget *preview_widget = NULL;

/** \brief  Drive number widget reference
 */
static GtkWidget *driveno_widget = NULL;
#endif


/** \brief  Last directory used
 *
 * When a disk is attached, this is set to the directory of that file. Since
 * it's heap-allocated by Gtk3, it must be freed with a call to
 * ui_disk_attach_shutdown() on emulator shutdown.
 */
static gchar *last_dir = NULL;


/** \brief  Unit number to attach disk to
 */
static int unit_number = DRIVE_UNIT_DEFAULT;

/** \brief  Drive number in unit to attach disk to
 */
static int drive_number = 0;

#if 0
/** \brief  Update the last directory reference
 *
 * \param[in]   widget  dialog
 */
static void update_last_dir(GtkWidget *widget)
{
    gchar *new_dir;

    new_dir = gtk_file_chooser_get_current_folder(GTK_FILE_CHOOSER(widget));
    debug_gtk3("new dir = '%s'.", new_dir);
    if (new_dir != NULL) {
        /* clean up previous value */
        if (last_dir != NULL) {
            g_free(last_dir);
        }
        last_dir = new_dir;
    }
}
#endif


#ifndef SANDBOX_MODE
/** \brief  Handler for the 'toggled' event of the 'show hidden files' checkbox
 *
 * \param[in]   widget      checkbox triggering the event
 * \param[in]   user_data   data for the event (the dialog)
 */
static void on_hidden_toggled(GtkWidget *widget, gpointer user_data)
{
    int state;

    state = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
    debug_gtk3("show hidden files: %s.", state ? "enabled" : "disabled");

    gtk_file_chooser_set_show_hidden(GTK_FILE_CHOOSER(user_data), state);
}
#endif


#if 0
/** \brief  Handler for the 'toggled' event of the 'show preview' checkbox
 *
 * \param[in]   widget      checkbox triggering the event
 * \param[in]   user_data   data for the event (unused)
 */
static void on_preview_toggled(GtkWidget *widget, gpointer user_data)
{
    int state;

    state = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
    debug_gtk3("preview %s.", state ? "enabled" : "disabled");
}
#endif


#ifndef SANDBOX_MODE
/** \brief  Handler for the "update-preview" event
 *
 * \param[in]   chooser file chooser dialog
 * \param[in]   data    extra event data (unused)
 */
static void on_update_preview(GtkFileChooser *chooser, gpointer data)
{
    GFile *file;
    gchar *path;

    file = gtk_file_chooser_get_preview_file(chooser);
    if (file != NULL) {
        path = g_file_get_path(file);
        if (path != NULL) {
            gchar *filename_locale = file_chooser_convert_to_locale(path);

            debug_gtk3("called with '%s'.", path);

            content_preview_widget_set_image(preview_widget, filename_locale);
            g_free(path);
            g_free(filename_locale);
        }
        g_object_unref(file);
    }
}
#endif


#ifndef SANDBOX_MODE
/** \brief  Callback for the drive unit widget
 *
 * Updates the drive number widget, depending on the drive unit's type:
 * 'greys-out' the drive number widget and sets drive number to 0 if the current
 * unit doesn't support dual drives.
 *
 * \param[in]   unit    drive unit number
 */
static void on_unit_changed(int unit)
{
    int dual = drive_is_dualdrive_by_devnr(unit);
    
    debug_gtk3(
        "Unit #%d: type %d, dual-drive: %s.",
        unit,
        drive_get_type_by_devnr(unit),
        dual ? "TRUE" : "FALSE");
    
    gtk_widget_set_sensitive(driveno_widget, dual);
    drive_no_widget_update(driveno_widget,
                           unit_drive_nums[unit - DRIVE_UNIT_MIN]);
}
#endif


#ifndef SANDBOX_MODE
/** \brief  Callback for the drive number widget
 *
 * Update drive number for the current unit, to later be used to restore the
 * drive number widget when changing units.
 *
 * \param[in]   drive   drive number
 */
static void on_drive_num_changed(int drive)
{
    debug_gtk3("Got drive number %d for unit #%d.", drive, unit_number);

    unit_drive_nums[unit_number - DRIVE_UNIT_MIN] = drive;

}
#endif


#ifndef SANDBOX_MODE
/** \brief  Trigger autostarting a disk image
 *
 * \param[in,out]   widget      dialog
 * \param[in]       user_data   file index in the image
 */
static void do_autostart(GtkWidget *widget, gpointer user_data)
{
    gchar *filename;
    gchar *filename_locale;
    int index = GPOINTER_TO_INT(user_data);

    lastdir_update(widget, &last_dir);
    filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(widget));
    /* convert filename to current locale */
    filename_locale = file_chooser_convert_to_locale(filename);

    debug_gtk3("Autostarting file '%s'.", filename);
    /* if this function exists, why is there no attach_autodetect()
     * or something similar? -- compyx */
    if (autostart_disk(
                filename_locale,
                NULL,   /* program name */
                index,  /* Program number? Probably used when clicking
                           in the preview widget to load the proper
                           file in an image */
                AUTOSTART_MODE_RUN) < 0) {
        /* oeps */
        debug_gtk3("autostart disk attach failed.");
    }
    g_free(filename);
    g_free(filename_locale);
    gtk_widget_destroy(widget);
}
#endif

#if 0
static void on_file_activated(GtkWidget *chooser, gpointer data)
{
    do_autostart(chooser, data);
}
#endif


#ifndef SANDBOX_MODE
/** \brief  Handler for 'response' event of the dialog
 *
 * This handler is called when the user clicks a button in the dialog.
 *
 * \param[in]   widget      the dialog
 * \param[in]   response_id response ID
 * \param[in]   user_data   extra data (unused)
 *
 * TODO:    proper (error) messages, which requires implementing ui_error() and
 *          ui_message() and moving them into gtk3/widgets to avoid circular
 *          references
 */
static void on_response(GtkWidget *widget, gint response_id,
                        gpointer user_data)
{
    gchar *filename;
    gchar *filename_locale;
    gchar buffer[1024];

#ifdef HAVE_DEBUG_GTK3UI
    int index = GPOINTER_TO_INT(user_data);
    debug_gtk3("got response ID %d, index %d.", response_id, index);
#endif

    filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(widget));

    switch (response_id) {

        /* 'Open' button, double-click on file */
        case GTK_RESPONSE_ACCEPT:

            lastdir_update(widget, &last_dir);
            filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(widget));
            /* convert filename to current locale */
            filename_locale = file_chooser_convert_to_locale(filename);

            /* ui_message("Opening file '%s' ...", filename); */
            debug_gtk3("Attaching file '%s' to unit #%d.",
                    filename, unit_number);

            /* copied from Gtk2: I fail to see how brute-forcing your way
             * through file types is 'smart', but hell, it works */
            if (file_system_attach_disk(unit_number, drive_number, filename_locale) < 0) {
                /* failed */
                debug_gtk3("disk attach failed.");
                g_snprintf(buffer, 1024, "Unit #%d: failed to attach '%s'",
                        unit_number, filename);
            } else {
                g_snprintf(buffer, 1024, "Unit #%d: attached '%s'",
                        unit_number, filename);
            }
            ui_display_statustext(buffer, 1);
            g_free(filename_locale);
            gtk_widget_destroy(widget);
            break;

        /* 'Autostart' button clicked */
        case VICE_RESPONSE_AUTOSTART:
            /* did we actually get a filename? */
            if (filename != NULL) {
                do_autostart(widget, user_data);
                gtk_widget_destroy(widget);
            }
            break;

        /* 'Close'/'X' button */
        case GTK_RESPONSE_REJECT:
            gtk_widget_destroy(widget);
            break;
        default:
            break;
    }

    if (filename != NULL) {
        g_free(filename);
    }
    ui_set_ignore_mouse_hide(FALSE);
}

#else

static void on_response(GtkWidget *widget, gint response_id,
                        gpointer user_data)
{
    gchar *filename;
    gchar *filename_locale;
    gchar buffer[1024];
    GtkFileChooser *chooser = GTK_FILE_CHOOSER(widget);

    debug_gtk3("Reponse-ID: %d", response_id);

    filename = gtk_file_chooser_get_filename(chooser);
    debug_gtk3("FIlename: '%s'", filename);

    switch (response_id) {
        case GTK_RESPONSE_ACCEPT:

            lastdir_update(widget, &last_dir);
            filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(widget));
            /* convert filename to current locale */
            filename_locale = file_chooser_convert_to_locale(filename);

            /* ui_message("Opening file '%s' ...", filename); */
            debug_gtk3("Attaching file '%s' to unit #%d.",
                    filename, unit_number);

            /* copied from Gtk2: I fail to see how brute-forcing your way
             * through file types is 'smart', but hell, it works */
            if (file_system_attach_disk(unit_number, filename_locale) < 0) {
                /* failed */
                debug_gtk3("disk attach failed.");
                g_snprintf(buffer, 1024, "Unit #%d: failed to attach '%s'",
                        unit_number, filename);
            } else {
                g_snprintf(buffer, 1024, "Unit #%d: attached '%s'",
                        unit_number, filename);
            }
            ui_display_statustext(buffer, 1);
            g_free(filename_locale);
            break;
        default:
            break;
    }

    if (filename != NULL) {
        g_free(filename);
    }
    gtk_native_dialog_destroy(GTK_NATIVE_DIALOG(chooser));
}
#endif


#ifndef SANDBOX_MODE
/** \brief  Create the 'extra' widget
 *
 * \param[in]   parent  parent widget
 * \param[in]   unit    unit number
 *
 * \return  GtkGrid
 *
 * TODO: 'grey-out'/disable units without a proper drive attached
 */
static GtkWidget *create_extra_widget(GtkWidget *parent, int unit)
{
    GtkWidget *grid;
    GtkWidget *hidden_check;
    GtkWidget *readonly_check;

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);

    hidden_check = gtk_check_button_new_with_label("Show hidden files");
    g_signal_connect(hidden_check, "toggled", G_CALLBACK(on_hidden_toggled),
            (gpointer)(parent));
    gtk_grid_attach(GTK_GRID(grid), hidden_check, 0, 0, 1, 1);

    readonly_check = gtk_check_button_new_with_label("Attach read-only");
    gtk_grid_attach(GTK_GRID(grid), readonly_check, 1, 0, 1, 1);

#if 0
    preview_check = gtk_check_button_new_with_label("Show image contents");
    g_signal_connect(preview_check, "toggled", G_CALLBACK(on_preview_toggled),
            NULL);
    gtk_grid_attach(GTK_GRID(grid), preview_check, 2, 0, 1, 1);
#endif
    /* second row, three cols wide */
    gtk_grid_attach(GTK_GRID(grid),
            drive_unit_widget_create(unit, &unit_number, on_unit_changed),
            0, 1, 3, 1);

    /* add drive number widget */
    driveno_widget = drive_no_widget_create(0, &drive_number, on_drive_num_changed);
#if 0
    drive_type = drive_get_type_by_devnr(unit);
    debug_gtk3("Drive type for unit #%d: %d.", unit, drive_type);
#endif
    gtk_widget_set_sensitive(driveno_widget, drive_is_dualdrive_by_devnr(unit));

    gtk_grid_attach(GTK_GRID(grid),
            driveno_widget,
            3, 1, 3, 1);

    gtk_widget_show_all(grid);
    return grid;
}
#endif


#ifndef SANDBOX_MODE

/** \brief  Create the disk attach dialog
 *
 * \param[in]   parent  parent widget, used to get the top level window
 * \param[in]   unit    drive unit
 *
 * \return  GtkFileChooserDialog
 */
static GtkWidget *create_disk_attach_dialog(GtkWidget *parent, int unit)
{
    GtkWidget *dialog;
    size_t i;

    ui_set_ignore_mouse_hide(TRUE);

    /* create new dialog */
    dialog = gtk_file_chooser_dialog_new(
            "Attach a disk image",
            ui_get_active_window(),
            GTK_FILE_CHOOSER_ACTION_OPEN,
            /* buttons */
            "Open", GTK_RESPONSE_ACCEPT,
            "Autostart", VICE_RESPONSE_AUTOSTART,
            "Close", GTK_RESPONSE_REJECT,
            NULL, NULL);

    /* set modal so mouse-grab doesn't get triggered */
    gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);

    /* set last directory */
    lastdir_set(dialog, &last_dir);

    /* add 'extra' widget: 'readony' and 'show preview' checkboxes */
    if (unit < DRIVE_UNIT_MIN || unit > DRIVE_UNIT_MAX) {
        unit = DRIVE_UNIT_DEFAULT;
    }
    gtk_file_chooser_set_extra_widget(GTK_FILE_CHOOSER(dialog),
                                      create_extra_widget(dialog, unit));

    preview_widget = content_preview_widget_create(
            dialog, diskcontents_filesystem_read, on_response);
    gtk_file_chooser_set_preview_widget(GTK_FILE_CHOOSER(dialog),
            preview_widget);

    /* add filters */
    for (i = 0; filters[i].name != NULL; i++) {
        gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog),
                create_file_chooser_filter(filters[i], FALSE));
    }

    /* connect "reponse" handler: the `user_data` argument gets filled in when
     * the "response" signal is emitted: a response ID */
    g_signal_connect(dialog, "response",
            G_CALLBACK(on_response), GINT_TO_POINTER(0));
    g_signal_connect(dialog, "update-preview",
            G_CALLBACK(on_update_preview), NULL);
#if 0
    /* Double click: autostart */
    g_signal_connect(dialog, "file-activated",
            G_CALLBACK(on_file_activated), NULL);
#endif
    return dialog;

}

#else

static GtkFileChooserNative *create_disk_attach_dialog_native(GtkWidget *parent,
                                                              int unit)
{
    GtkFileChooserNative *dialog;

    ui_set_ignore_mouse_hide(TRUE);

    /* create new dialog */
    dialog = gtk_file_chooser_native_new(
            "Attach a disk image (NATIVE)",
            ui_get_active_window(),
            GTK_FILE_CHOOSER_ACTION_OPEN,
            NULL,
            NULL);

    g_signal_connect(dialog, "response",
            G_CALLBACK(on_response),
            GINT_TO_POINTER(0));    /* index of file to start? */

    return dialog;
}
#endif


/** \brief  Callback for the "smart-attach" and "attach to #%d" menu items
 *
 * Creates the smart-dialog and runs it.
 *
 * \param[in]   widget      menu item triggering the callback
 * \param[in]   user_data   integer from 8-11 for the default drive to attach
 *                          (for some reason auto-attach always ultra uses #8)
 *
 * \return  TRUE
 */
gboolean ui_disk_attach_callback(GtkWidget *widget, gpointer user_data)
{
#ifndef SANDBOX_MODE
    GtkWidget *dialog;

    dialog = create_disk_attach_dialog(widget, GPOINTER_TO_INT(user_data));
    gtk_widget_show(dialog);
#else
    GtkFileChooserNative *dialog;
    dialog = create_disk_attach_dialog_native(widget, GPOINTER_TO_INT(user_data));
    gtk_native_dialog_show(GTK_NATIVE_DIALOG(dialog));
#endif
    return TRUE;
}


/** \brief  Callback for "detach from #%d" menu items
 *
 * Removes any disk from the specified drive. No additional UI is
 * presented.
 *
 * \param[in]   widget      menu item triggering the callback
 * \param[in]   user_data   integer from 8-11 for the drive to
 *                          close, or -1 to detach all disks
 *
 * \return  TRUE
 */
gboolean ui_disk_detach_callback(GtkWidget *widget, gpointer user_data)
{
    /* This function does its own interpretation and input validation,
     * so we can simply forward the call directly. */
    int unit;
    int drive;

    unit = GPOINTER_TO_INT(user_data) & 0xff;
    drive = GPOINTER_TO_INT(user_data) >> 8;

    debug_gtk3("Detaching unit #%d, drive %d.", unit, drive);
    file_system_detach_disk(unit, drive);
    return TRUE;
}


/** \brief  Detach all disks
 *
 * \param[in]   widget  widget (unused)
 * \param[in]   data    extra event data (unused)
 *
 * \return  TRUE
 */
gboolean ui_disk_detach_all_callback(GtkWidget *widget, gpointer data)
{
    int unit;

    for (unit = DRIVE_UNIT_MIN; unit <= DRIVE_UNIT_MAX; unit++) {
        file_system_detach_disk(unit, 0);
        file_system_detach_disk(unit, 1);
    }
    return TRUE;
}


/** \brief  Clean up the last directory string
 */
void ui_disk_attach_shutdown(void)
{
    lastdir_shutdown(&last_dir);
}
