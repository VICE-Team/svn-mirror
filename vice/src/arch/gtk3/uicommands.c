/** \file   uicommands.c
 * \brief   Simple commands triggered from the menu
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 *
 */

/*
 * $VICERES WarpMode        all
 * $VICERES JoyDevice1      -vsid
 * $VICERES JoyDevice2      -vsid
 * $VICERES JoyDevice3      -vsid
 * $VICERES JoyDevice4      -vsid
 * $VICERES Mouse           -vsid
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

#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>

#include "archdep.h"
#include "resources.h"
#include "debug_gtk3.h"
#include "basedialogs.h"
#include "drive.h"
#include "log.h"
#include "machine.h"
#include "mainlock.h"
#include "uimenu.h"
#include "util.h"
#include "uiactions.h"
#include "vsync.h"
#include "uiapi.h"
#include "ui.h"
#include "uicommands.h"
#include "uimachinewindow.h"
#include "widgethelpers.h"


static gboolean controlport_swapped = FALSE;
static gboolean userport_swapped = FALSE;


/** \brief  Callback for the confirm-on-exit dialog
 *
 * Exit VICE if \a result is TRUE.
 *
 * \param[in]   dialog  dialog reference (unused)
 * \param[in]   result  dialog result
 */
static void confirm_exit_callback(GtkDialog *dialog, gboolean result)
{
    if (result) {
        mainlock_release();
        archdep_vice_exit(0);
        mainlock_obtain();
    }
}


/** \brief  Determine if control ports 1 & 2 are currently swapped.
 *
 * \return  bool
 */
gboolean ui_get_controlport_swapped(void)
{
    return controlport_swapped;
}


/** \brief  Determine if user ports 1 & 2 are currently swapped.
 *
 * \return  bool
 */
gboolean ui_get_userport_swapped(void)
{
    return userport_swapped;
}


/** \brief  Swap controlport devices 1 & 2
 *
 * \return  TRUE
 */
gboolean ui_action_toggle_controlport_swap(void)
{
    int joy1 = -1;
    int joy2 = -1;
    int type1 = -1;
    int type2 = -1;

    resources_get_int("JoyPort1Device", &type1);
    resources_get_int("JoyPort2Device", &type2);

    /* unset both resources first to avoid assigning for example the mouse to
     * two ports. here might be dragons!
     */
    resources_set_int("JoyPort1Device", 0);
    resources_set_int("JoyPort2Device", 0);

    /* try setting port #2 first, some devices only work in port #1 */
    if (resources_set_int("JoyPort2Device", type1) < 0) {
        /* restore config */
        resources_set_int("JoyPort1Device", type1);
        resources_set_int("JoyPort2Device", type2);
        return TRUE;
    }
    if (resources_set_int("JoyPort1Device", type2) < 0) {
        /* restore config */
        resources_set_int("JoyPort1Device", type1);
        resources_set_int("JoyPort2Device", type2);
        return TRUE;
    }

    resources_get_int("JoyDevice1", &joy1);
    resources_get_int("JoyDevice2", &joy2);
    resources_set_int("JoyDevice1", joy2);
    resources_set_int("JoyDevice2", joy1);

    controlport_swapped = !controlport_swapped;

    ui_set_gtk_check_menu_item_blocked_by_name(ACTION_SWAP_CONTROLPORT_TOGGLE,
                                               controlport_swapped);
    return TRUE;
}


/** \brief  Swap userport joysticks
 *
 * \return  TRUE
 */
gboolean ui_action_toggle_userport_swap(void)
{
    int joy3 = -1;
    int joy4 = -1;

    resources_get_int("JoyDevice3", &joy3);
    resources_get_int("JoyDevice4", &joy4);
    resources_set_int("JoyDevice3", joy4);
    resources_set_int("JoyDevice4", joy3);

    userport_swapped = !userport_swapped;
    ui_set_gtk_check_menu_item_blocked_by_name(ACTION_SWAP_USERPORT_TOGGLE,
                                               userport_swapped);
    return TRUE;
}


/** \brief  Toggle resource 'KeySetEnable'
 *
 * \param[in]   widget
 * \param[in]   data    (unused?)
 *
 * \return  TRUE (so the UI eats the event)
 *
 * TODO:    refactor into `gboolean ui_action_toggle_keyset_swap(void)`
 */
gboolean ui_toggle_keyset_joysticks(GtkWidget *widget, gpointer data)
{
    int enable;

    resources_get_int("KeySetEnable", &enable);
    resources_set_int("KeySetEnable", !enable);

    return TRUE;    /* don't let any shortcut key end up in the emulated machine */
}


/** \brief  Toggle resource 'Mouse' (mouse-grab)
 *
 * \param[in]   widget  menu item triggering the event (unused)
 * \param[in]   data    extra event data (unused)
 *
 * \return  TRUE (so the UI eats the event)
 */
gboolean ui_action_toggle_mouse_grab(void)
{
    GtkWindow *window;
    int mouse;
    gchar title[256];

    resources_get_int("Mouse", &mouse);
    resources_set_int("Mouse", !mouse);
    mouse = !mouse;

    if (mouse) {
       g_snprintf(title, sizeof(title),
               /* TODO: get proper key+modifier string from ui data */
               "VICE (%s) (Use %s+M to disable mouse grab)",
               machine_get_name(), VICE_MOD_MASK_TEXT);
    } else {
       g_snprintf(title, sizeof(title),
               "VICE (%s)",
               machine_get_name());
    }

    window = ui_get_active_window();
    gtk_window_set_title(window, title);

    ui_set_gtk_check_menu_item_blocked_by_name("toggle-mouse-grab", mouse);

    return TRUE;    /* don't let any shortcut key end up in the emulated machine */
}


/** \brief  Callback for the soft/hard reset items
 *
 * \param[in]   widget      menu item triggering the event (unused)
 * \param[in]   user_data   MACHINE_RESET_MODE_SOFT/MACHINE_RESET_MODE_HARD
 *
 * \return  TRUE to indicate the event has been handled
 */
gboolean ui_machine_reset_callback(GtkWidget *widget, gpointer user_data)
{
    machine_trigger_reset(GPOINTER_TO_INT(user_data));
    ui_pause_disable();
    return TRUE;
}


/** \brief  Callback for the drive reset items
 *
 * \param[in]   widget      menu item triggering the event (unused)
 * \param[in]   user_data   drive unit number (8-11) (int)
 *
 * \return  TRUE
 */
gboolean ui_drive_reset_callback(GtkWidget *widget, gpointer user_data)
{
    vsync_suspend_speed_eval();
    drive_cpu_trigger_reset(GPOINTER_TO_INT(user_data) - 8);
    return TRUE;
}


/** \brief  Ask the user to confirm to exit the emulator if ConfirmOnExit is set
 *
 * \return  TRUE if the emulator should be exited, FALSE if not
 */
static gboolean confirm_exit(void)
{
    int confirm = FALSE;

    resources_get_int("ConfirmOnExit", &confirm);
    if (!confirm) {
        return TRUE;
    }

    vice_gtk3_message_confirm(
            confirm_exit_callback,
            "Exit VICE",
            "Do you really wish to exit VICE?`");

    return FALSE;
}


/** \brief  Callback for the File->Exit menu item
 *
 * \param[in]   widget      menu item triggering the event (unused)
 * \param[in]   user_data   unused
 *
 * \return  TRUE
 */
gboolean ui_close_callback(GtkWidget *widget, gpointer user_data)
{
    if (confirm_exit()) {
        archdep_vice_exit(0);
    }
    return TRUE;
}


/** \brief  Handler for the 'delete-event' of a main window
 *
 * \param[in]   widget      window triggering the event (unused)
 * \param[in]   event       event details (unused)
 * \param[in]   user_data   extra data for the event (unused)
 *
 * \return  TRUE, if the function returns at all
 */
gboolean ui_main_window_delete_event(GtkWidget *widget,
                                     GdkEvent *event,
                                     gpointer user_data)
{
    if (confirm_exit()) {
        /* if we reach this point, the function doesn't return */
        archdep_vice_exit(0);
    }
    return TRUE;
}


/** \brief  Callback for the 'destroy' event of a main window
 *
 * \param[in]   widget      widget triggering the event
 * \param[in]   user_data   extra data for the callback (unused)
 */
void ui_main_window_destroy_callback(GtkWidget *widget, gpointer user_data)
{
    GtkWidget *grid;

    /*
     * This should not be needed, destroying a GtkWindow should trigger
     * destruction of all widgets it contains.
     */
    debug_gtk3("Manually calling destroy() on the CRT widgets. This should not"
            " be necesarry, but right now it is.");
    grid = gtk_bin_get_child(GTK_BIN(widget));
    if (grid != NULL) {
        GtkWidget *crt = gtk_grid_get_child_at(GTK_GRID(grid), 0, 2);
        if (crt != NULL) {
            gtk_widget_destroy(crt);
        }
    }
}


/** \brief  Toggle boolean resource from the menu
 *
 * Toggles \a resource when a valid.
 *
 * \param[in]   widget      menu item triggering the event
 * \param[in]   resource    resource name
 *
 * \return  TRUE if succesful, FALSE otherwise
 */
gboolean ui_toggle_resource(GtkWidget *widget, gpointer resource)
{
    const char *res = (const char *)resource;

    if (res != NULL) {
        int new_state;

        /* attempt to toggle resource */
        if (resources_toggle(res, &new_state) < 0) {
            return FALSE;
        }
        return TRUE;
    }
    return FALSE;
}


/** \brief  Open the Manual
 *
 * Event handler for the 'Manual' menu item
 *
 * \param[in]   widget      parent widget triggering the event (unused)
 * \param[in]   user_data   extra event data (unused)
 *
 * \return  TRUE if opening the manual succeeded, FALSE otherwise
 *          (unreliable: gtk_show_uri_on_window() will return TRUE if the
 *           associated application could be openened but not the actual
 *           manual file)
 *
 * \note    Keep the debug_gtk3() calls for now, this code hardly works on
 *          Windows at all and needs work.
 */
gboolean ui_open_manual_callback(GtkWidget *widget, gpointer user_data)
{
    GError *error = NULL;
    gboolean res;
    char *uri;
    const char *path;
    gchar *final_uri;

    /*
     * Get arch-dependent documentation dir (doesn't contain the HTML docs
     * on Windows, but that's an other issue to fix.
     */
    path = archdep_get_vice_docsdir();

    /* first try opening the pdf */
    uri = archdep_join_paths(path, "vice.pdf", NULL);
    debug_gtk3("URI before GTK3: %s", uri);
    final_uri = g_filename_to_uri(uri, NULL, &error);
    debug_gtk3("final URI (pdf): %s", final_uri);
    if (final_uri == NULL) {
        /*
         * This is a fatal error, if a proper URI can't be built something is
         * wrong and should be looked at. This is different from failing to
         * load the PDF or not having a program to show the PDF
         */
        log_error(LOG_ERR, "failed to construct a proper URI from '%s',"
                " not trying the HTML fallback, this is an error that"
                " should not happen.",
                uri);
        g_clear_error(&error);
        lib_free(uri);
        return FALSE;
    }

    debug_gtk3("pdf uri: '%s'.", final_uri);

    /* NOTE:
     *
     * On Windows this at least opens Acrobat reader with a file-not-found
     * error message, any other URI/path given to this call results in a
     * "Operation not supported" message, which doesn't help much.
     *
     * Since Windows (or perhaps Gtk3 on Windows) fails, I've removed the
     * Windows-specific code that didn't work anyway
     */
    res = gtk_show_uri_on_window(NULL, final_uri, GDK_CURRENT_TIME, &error);
    if (!res) {
        /* will contain the args for the archep_spawn() call */
        char *args[3];
        char *tmp_name;

        debug_gtk3("gtk_show_uri_on_window Failed!");

        /* fallback to xdg-open */
        args[0] = lib_strdup("xdg-open");
        args[1] = lib_strdup(uri);
        args[2] = NULL;

        debug_gtk3("Calling xgd-open");
        if (archdep_spawn("xdg-open", args, &tmp_name, NULL) < 0) {
            debug_gtk3("xdg-open Failed!");
            vice_gtk3_message_error(
                    "Failed to load PDF",
                    "Error message: %s",
                    error != NULL ? error->message : "<no message>");
        } else {
            debug_gtk3("OK");
            res = TRUE;
        }
        /* clean up */
        lib_free(args[0]);
        lib_free(args[1]);
    }

    lib_free(uri);
    g_free(final_uri);
    g_clear_error(&error);

    return res;
}


/** \brief  Attempt to restore the active window's size to its "natural" size
 *
 * Also unmaximizes and unfullscreens the window.
 *
 * \param[in]   widget  widget triggering the event (ignored)
 * \param[in]   data    extra event data (unused)
 *
 * \return  TRUE to signal the event was handled
 */
gboolean ui_restore_display(GtkWidget *widget, gpointer data)
{
    GtkWindow *window = ui_get_active_window();

    if (window != NULL) {
        /* disable fullscreen if active */
        if (ui_is_fullscreen()) {
            ui_action_toggle_fullscreen();
        }
        /* unmaximize */
        gtk_window_unmaximize(window);
        /* requesting a 1x1 window forces the window to resize to its natural
         * size, ie the minimal size required to display the window's
         * decorations and contents without wasting space
         */
        gtk_window_resize(window, 1, 1);
    }
    return TRUE;
}


/** \brief  Callback for the confirmation dialog to restore settings
 *
 * Restore settings to their factory values if \a result is TRUE.
 *
 * \param[in]   dialog  confirm-dialog reference
 * \param[in]   result  result
 */
static void restore_default_callback(GtkDialog *dialog, gboolean result)
{
    if (result) {
        mainlock_obtain();
        resources_set_defaults();
        mainlock_release();
    }
    gtk_widget_destroy(GTK_WIDGET(dialog));
}


/** \brief  Restore default settings
 *
 * Resets settings to their defaults, asking the user to confirm.
 *
 * \param[in]   widget  widget triggering event (ignored)
 * \param[in]   data    extra event data
 *
 * \return  TRUE (UI 'consumed' the keypress so it doesn't end up in the emu)
 */
gboolean ui_restore_default_settings(GtkWidget *widget, gpointer data)
{
    vice_gtk3_message_confirm(
            restore_default_callback,
            "Reset all settings to default",
            "Are you sure you wish to reset all settings to their default"
            " values?\n\n"
            "The new settings will not be saved until using the 'Save"
            " settings' menu item, or having 'Save on exit' enabled and"
            " exiting VICE.");
    return TRUE;
}
