/** \file   uismartattach.c
 * \brief   GTK3 smart-attach dialog
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

/*
 * $VICERES AttachDevice8Readonly   -vsid
 */


#include "vice.h"

#include <gtk/gtk.h>

#include "attach.h"
#include "autostart.h"
#include "cartridge.h"
#include "drive.h"
#include "tape.h"
#include "debug_gtk3.h"
#include "basedialogs.h"
#include "contentpreviewwidget.h"
#include "diskcontents.h"
#include "tapecontents.h"
#include "machine.h"
#include "mainlock.h"
#include "resources.h"
#include "filechooserhelpers.h"
#include "ui.h"
#include "uiapi.h"
#include "uimachinewindow.h"
#include "lastdir.h"
#include "initcmdline.h"
#include "log.h"

#include "uismartattach.h"


#ifndef SANDBOX_MODE
/** \brief  File type filters for the dialog
 */
static ui_file_filter_t filters[] = {
    { "All files", file_chooser_pattern_all },
    { "Disk images", file_chooser_pattern_disk },
    { "Tape images", file_chooser_pattern_tape },
    { "Cartridge images", file_chooser_pattern_cart },
    { "Program files", file_chooser_pattern_program },
    { "Snapshot files", file_chooser_pattern_snapshot },
    { "Archives files", file_chooser_pattern_archive },
    { "Compressed files", file_chooser_pattern_compressed },
    { NULL, NULL }
};
#endif


#ifndef SANDBOX_MODE
/** \brief  Preview widget reference
 */
static GtkWidget *preview_widget = NULL;
#endif


static int autostart_on_doubleclick;



/** \brief  Last directory used
 *
 * When an image is attached, this is set to the directory of that file. Since
 * it's heap-allocated by Gtk3, it must be freed with a call to
 * ui_smart_attach_shutdown() on emulator shutdown.
 */
static gchar *last_dir = NULL;

/** \brief  Last file selected
 */
static gchar *last_file = NULL;


/** \brief  Reference to the custom 'Autostart' button
 */
static GtkWidget *autostart_button;


/** \brief  Trigger autostart
 *
 * \param[in]   widget  dialog
 * \param[in]   data    file index in the directory preview
 */
static void do_autostart(GtkWidget *widget, gpointer data)
{
    gchar *filename;
    gchar *filename_locale;

    int index = GPOINTER_TO_INT(data);

    lastdir_update(widget, &last_dir, &last_file);
    filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(widget));
    filename_locale = file_chooser_convert_to_locale(filename);

    /* if this function exists, why is there no attach_autodetect()
     * or something similar? -- compyx */
    if (autostart_autodetect(
                filename_locale,
                NULL,   /* program name */
                index,  /* Program number? Probably used when clicking
                           in the preview widget to load the proper
                           file in an image */
                AUTOSTART_MODE_RUN) < 0) {
        /* oeps
         *
         * I currently can't find a way to use a non-blocking Gtk Error dialog
         * to report the error, so this will have to do, for now. -- compyx
         */
        log_error(LOG_ERR, "Failed to smart attach '%s'", filename_locale);
        ui_error("Failed to smart attach '%s'", filename_locale);
    }
    g_free(filename);
    g_free(filename_locale);
    gtk_widget_destroy(widget);
}


/** \brief  Handler for 'selection-changed' event of the preview widget
 *
 * Checks if a proper selection was made and activates the 'Autostart' button
 * if so, disabled it otherwise
 *
 * \param[in]   chooser Parent dialog
 * \param[in]   data    Extra event data (unused)
 */
void on_selection_changed(GtkFileChooser *chooser, gpointer data)
{
    gchar *filename;

    filename = gtk_file_chooser_get_filename(chooser);
    if (filename != NULL) {
        gtk_widget_set_sensitive(autostart_button, TRUE);
        g_free(filename);
    } else {
        gtk_widget_set_sensitive(autostart_button, FALSE);
    }
}


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

            content_preview_widget_set_image(preview_widget, filename_locale);
            g_free(path);
            g_free(filename_locale);
        }
        g_object_unref(file);
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
    gtk_file_chooser_set_show_hidden(GTK_FILE_CHOOSER(user_data), state);
}
#endif


#ifndef SANDBOX_MODE
/** \brief  Handler for the 'toggled' event of the 'attach read-only' checkbox
 *
 * \param[in]   widget      checkbox triggering the event
 * \param[in]   user_data   data for the event (the dialog)
 */
static void on_readonly_toggled(GtkWidget *widget, gpointer user_data)
{
    int state;

    state = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
    resources_set_int_sprintf("AttachDevice%dReadonly", state, DRIVE_UNIT_DEFAULT);
}
#endif


/** \brief  Handler for 'response' event of the dialog
 *
 * This handler is called when the user clicks a button in the dialog.
 *
 * \param[in]   widget      the dialog
 * \param[in]   response_id response ID
 * \param[in]   user_data   index in the preview widget
 *
 * TODO:    proper (error) messages, which requires implementing ui_error() and
 *          ui_message() and moving them into gtk3/widgets to avoid circular
 *          references
 */
static void on_response(GtkWidget *widget, gint response_id, gpointer user_data)
{
    gchar *filename;
    gchar *filename_locale;

#ifdef HAVE_DEBUG_GTK3UI
    int index = GPOINTER_TO_INT(user_data);
    debug_gtk3("got response ID %d, index %d.", response_id, index);
#endif

    /* gonna needs this in multiple checks */
    filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(widget));

    switch (response_id) {

        /* 'Open' button, double-click on file */
        case GTK_RESPONSE_ACCEPT:
            lastdir_update(widget, &last_dir, &last_file);
            filename_locale = file_chooser_convert_to_locale(filename);

            /* Smart attach for C64/C128
             *
             * This tries to attach a file as a cartridge image, which is only
             * valid for C64/C128
             */
            if ((machine_class == VICE_MACHINE_C64)
                    || (machine_class == VICE_MACHINE_C64SC)
                    || (machine_class == VICE_MACHINE_SCPU64)
                    || (machine_class == VICE_MACHINE_C128)) {
                if (file_system_attach_disk(DRIVE_UNIT_DEFAULT, 0, filename_locale) < 0
                        && tape_image_attach(1, filename_locale) < 0
                        && autostart_snapshot(filename_locale, NULL) < 0
                        && cartridge_attach_image(CARTRIDGE_CRT, filename_locale) < 0
                        && autostart_prg(filename_locale, AUTOSTART_MODE_LOAD) < 0) {
                    /* failed (TODO: perhaps a proper error message?) */
                    debug_gtk3("smart attach failed.");
                }
            } else {
                /* Smart attach for other emulators: don't try to attach a file
                 * as a cartidge, it'll result in false positives
                 */
                if (file_system_attach_disk(DRIVE_UNIT_DEFAULT, 0, filename_locale) < 0
                        && tape_image_attach(1, filename_locale) < 0
                        && autostart_snapshot(filename_locale, NULL) < 0)
                {
                    /* failed (TODO: perhaps a proper error message?) */
                    log_error(LOG_ERR, "Failed to smart attach '%s'",
                            filename_locale);
                }
            }
            g_free(filename_locale);
            gtk_widget_destroy(widget);
            break;

        /* 'Autostart' button clicked */
        case VICE_RESPONSE_AUTOSTART:
            /* do we actually have a file to autostart? */
            if (filename != NULL) {
                do_autostart(widget, user_data);
                mainlock_release();
                gtk_widget_destroy(widget);
                mainlock_obtain();
            }
            break;

        /* 'Close'/'X' button */
        case GTK_RESPONSE_REJECT:
            mainlock_release();
            gtk_widget_destroy(widget);
            mainlock_obtain();
            break;
        default:
            break;
    }

    if (filename != NULL) {
        g_free(filename);
    }
}


#ifndef SANDBOX_MODE
/** \brief  Create the 'extra' widget
 *
 * \param[in]   parent  parent widget (unused)
 *
 * \return  GtkGrid
 */
static GtkWidget *create_extra_widget(GtkWidget *parent)
{
    GtkWidget *grid;
    GtkWidget *hidden_check;
    GtkWidget *readonly_check;
    int readonly_state;

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);

    hidden_check = gtk_check_button_new_with_label("Show hidden files");
    g_signal_connect(hidden_check, "toggled", G_CALLBACK(on_hidden_toggled),
            (gpointer)(parent));
    gtk_grid_attach(GTK_GRID(grid), hidden_check, 0, 0, 1, 1);

    readonly_check = gtk_check_button_new_with_label("Attach read-only");
    g_signal_connect(readonly_check, "toggled", G_CALLBACK(on_readonly_toggled),
            (gpointer)(parent));
    gtk_grid_attach(GTK_GRID(grid), readonly_check, 1, 0, 1, 1);
    resources_get_int_sprintf("AttachDevice%dReadonly", &readonly_state, DRIVE_UNIT_DEFAULT);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(readonly_check), readonly_state);

    gtk_widget_show_all(grid);
    return grid;
}
#endif


#ifndef SANDBOX_MODE
/** \brief  Wrapper around disk/tape contents readers
 *
 * First treats \a path as disk image file and when that fails it falls back
 * to treating \a path as a tape image, when that fails as well, it gives up.
 *
 * \param[in]   path    path to image file
 *
 * \return  image contents or `NULL` on failure
 */
static image_contents_t *read_contents_wrapper(const char *path)
{
    image_contents_t *content;

    /* try disk contents first */
    content = diskcontents_filesystem_read(path);
    if (content == NULL) {
        /* fall back to tape */
        content = tapecontents_read(path);
    }
    return content;
}
#endif



#ifndef SANDBOX_MODE
/** \brief  Create the smart-attach dialog
 *
 * \param[in]   parent  parent widget, used to get the top level window
 *
 * \return  GtkFileChooserDialog
 *
 * \todo    Figure out how to only enable the 'Autostart' button when an actual
 *          file/image has been selected. And when I do, make sure it's somehow
 *          reusable for other 'open file' dialogs'.
 */
static GtkWidget *create_smart_attach_dialog(GtkWidget *parent)
{
    GtkWidget *dialog;
    size_t i;

    /* create new dialog */
    dialog = gtk_file_chooser_dialog_new(
            "Smart-attach a file",
            ui_get_active_window(),
            GTK_FILE_CHOOSER_ACTION_OPEN,
            /* buttons */
            NULL, NULL);

    /* We need to manually add the buttons here, not using the constructor
     * above, to keep the order of the buttons the same as in the other
     * 'attach' dialogs. Gtk doesn't allow getting references to buttons when
     * added via the constructor, meaning we cannot get a reference to the
     * "Autostart" button in order to "grey it out".
     *
     * Also rename VICE_RESPONSE_AUTOSTART to VICE_RESPONSE_CUSTOM or so, we're
     * flipping responses around here thanks the the flexibility of Gtk
     */
    if (!autostart_on_doubleclick) {
        gtk_dialog_add_button(GTK_DIALOG(dialog), "Open", GTK_RESPONSE_ACCEPT);
        autostart_button = gtk_dialog_add_button(GTK_DIALOG(dialog),
                                                 "Autostart",
                                                 VICE_RESPONSE_AUTOSTART);
    } else{
        gtk_dialog_add_button(GTK_DIALOG(dialog), "Open", VICE_RESPONSE_AUTOSTART);
        autostart_button = gtk_dialog_add_button(GTK_DIALOG(dialog),
                                                 "Autostart",
                                                 GTK_RESPONSE_ACCEPT);
    }
    gtk_widget_set_sensitive(autostart_button, FALSE);
    gtk_dialog_add_button(GTK_DIALOG(dialog), "Close", GTK_RESPONSE_REJECT);

    /* set modal so mouse-grab doesn't get triggered */
    gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);

    /* set last used directory */
    lastdir_set(dialog, &last_dir, &last_file);

    /* add 'extra' widget: 'readony' and 'show preview' checkboxes */
    gtk_file_chooser_set_extra_widget(GTK_FILE_CHOOSER(dialog),
            create_extra_widget(dialog));

    preview_widget = content_preview_widget_create(dialog,
            read_contents_wrapper, on_response);
    gtk_file_chooser_set_preview_widget(GTK_FILE_CHOOSER(dialog),
            preview_widget);

    /* add filters */
    for (i = 0; filters[i].name != NULL; i++) {
        gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog),
                create_file_chooser_filter(filters[i], FALSE));
    }

    /* connect "reponse" handler: the `user_data` argument gets filled in when
     * the "response" signal is emitted: a response ID */
    g_signal_connect(dialog, "response", G_CALLBACK(on_response), NULL);
    g_signal_connect_unlocked(dialog, "update-preview",
            G_CALLBACK(on_update_preview), NULL);
    g_signal_connect_unlocked(dialog, "selection-changed",
            G_CALLBACK(on_selection_changed), NULL);

#if 0
    /* Autostart on double-click */
    g_signal_connect(dialog, "file-activated",
            G_CALLBACK(on_file_activated), NULL);
#endif
    return dialog;

}

#else

/** \brief  Create the smart-attach dialog
 *
 * \param[in]   parent  parent widget, used to get the top level window
 *
 * \return  GtkFileChooserNative
 *
 * \todo    Figure out how to only enable the 'Autostart' button when an actual
 *          file/image has been selected. And when I do, make sure it's somehow
 *          reusable for other 'open file' dialogs'.
 */
static GtkFileChooserNative *create_smart_attach_dialog(void *parent)
{
    GtkFileChooserNative *dialog;

    /* create new dialog */
    dialog = gtk_file_chooser_native_new(
            "Smart-attach a file",
            ui_get_active_window(),
            GTK_FILE_CHOOSER_ACTION_OPEN,
            /* buttons */
            NULL, NULL);

    /* connect "reponse" handler: the `user_data` argument gets filled in when
     * the "response" signal is emitted: a response ID */
    g_signal_connect(dialog, "response", G_CALLBACK(on_response), NULL);

    return dialog;
}
#endif


/** \brief  Callback for the File menu's "smart-attach" item
 *
 * Creates the smart-dialog and runs it.
 *
 * \param[in]   widget      menu item triggering the callback
 * \param[in]   user_data   data for the event (unused)
 *
 * \return  TRUE
 */
gboolean ui_smart_attach_dialog_show(GtkWidget *widget, gpointer user_data)
{
#ifndef SANDBOX_MODE
    GtkWidget *dialog;

    if (resources_get_int("AutostartOnDoubleclick", &autostart_on_doubleclick) < 0) {
        debug_gtk3("Failed to get resource value for 'AutostartOnDoubleclick");
        autostart_on_doubleclick = 0;
    }


    dialog = create_smart_attach_dialog(widget);
    gtk_widget_show(dialog);
#else
    GtkFileChooserNative *dialog;
    dialog = create_smart_attach_dialog((gpointer)widget);
    gtk_native_dialog_show(GTK_NATIVE_DIALOG(dialog));
#endif
    return TRUE;

}


/** \brief  Clean up last used directory string
 */
void ui_smart_attach_shutdown(void)
{
    lastdir_shutdown(&last_dir, &last_file);
}
