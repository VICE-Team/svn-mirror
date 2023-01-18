/** \file   logfilewidget.c
 * \brief   Widget to set the log file
 *
 * /author  Bas Wassink <b.wassink@ziggo.nl>
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

/*
 * Resources manipulated by this widget:
 *
 * $VICERES LogFileName -vsid
 */

#include "vice.h"
#include <gtk/gtk.h>

#include "archdep.h"
#include "lib.h"
#include "log.h"
#include "resources.h"
#include "ui.h"
#include "vice_gtk3.h"

#include "logfilewidget.h"


/** \brief  ID of the 'changed' signal handler of the "stdout" check button
 *
 * This ID is used to temporarily block the signal to avoid signal handlers of
 * the check button and the text entry triggering each other.
 */
static gulong check_handler;

static GtkWidget *launcher;


/** \brief  Handler for the 'changed' event of the entry
 *
 * Set \a check state depending on whether \a entry contains "-".
 *
 * \param[in]   entry   GtkEntry containing the LogFileName contents
 * \param[in]   check   GtkCheckButton for "log to stdout"
 */
static void on_logfile_entry_changed(GtkWidget *entry, gpointer check)
{
    const char *text    = gtk_entry_get_text(GTK_ENTRY(entry));
    bool        enabled = g_strcmp0(text, "-") == 0;

    /* avoid signals bouncing */
    g_signal_handler_block(check, check_handler);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check), enabled);
    gtk_widget_set_sensitive(launcher, !enabled);
    g_signal_handler_unblock(check, check_handler);
}

/** \brief  Handler for the 'toggled' event of the check button
 *
 * Set \a browser contents to "-" when \a check is toggled on, clear \a browser
 * contents when \a check is toggled off.
 *
 * \param[in]   check   GtkCheckButton for "log to stdout"
 * \param[in]   browser resource browser containing the LogFileName contents
 */
static void on_stdout_check_toggled(GtkWidget *check, gpointer browser)
{
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check))) {
        vice_gtk3_resource_browser_set(GTK_WIDGET(browser), "-");
    } else {
        vice_gtk3_resource_browser_set(GTK_WIDGET(browser), NULL);
    }
}

/** \brief  Open directory containing the log file
 *
 * \param[in]   button  button (unused)
 * \param[in]   data    extra event data (unused)
 */
static void on_launcher_clicked(GtkWidget *button, gpointer data)
{
    const char *logfile = NULL;
    char       *path;
    char       *dir;
    char       *uri;
    GError     *error = NULL;
#ifdef WINDOWS_COMPILE
    char       *child_argv[3] = { "explorer.exe", NULL, NULL };
#endif

    resources_get_string("LogFileName", &logfile);
    if (g_strcmp0(logfile, "-") == 0) {
        /* stdout isn't in any directory ;) */
        return;
    }

    if (logfile == NULL || *logfile == '\0') {
        path = archdep_default_logfile();
    } else {
        path = lib_strdup(logfile);
    }

    /* get dirname */
    dir = g_path_get_dirname(path);
    lib_free(path);
    uri = g_strdup_printf("file://%s/", dir);
#if 0
    g_print("%s(): logfile location = %s\n", __func__, uri);
#endif

#ifdef WINDOWS_COMPILE
    /* Set directory as argument to explorer.exe, *DO NOT QUOTE*, GIO handles
       the quoting and unquoting mess for Windows: adding quotes around the
       directory actually results in explorer opening "Documents" for some
       obscene reason.
    */
    child_argv[1] = dir;
    if (!g_spawn_async(NULL,                /* working directory */
                       child_argv,          /* argv[], first element is executable */
                       NULL,                /* environment */
                       G_SPAWN_SEARCH_PATH, /* search PATH for executable */
                       NULL,                /* child setup function */
                       NULL,                /* user data for child setup */
                       NULL,                /* child process ID */
                       &error)) {
        log_error(LOG_ERR,
                  "failed to launch file manager to view directory: %s",
                  error->message);
        g_error_free(error);
    }
#else
    if (!gtk_show_uri_on_window(ui_get_active_window(),
                                uri,
                                GDK_CURRENT_TIME,
                                &error)) {
        log_error(LOG_ERR,
                  "failed to lauch file manager to view directory: %s",
                  error->message);
        g_error_free(error);
    }
#endif
    g_free(dir);
    g_free(uri);
}

/** \brief  Create widget to set the LogFileName resource
 *
 * \return  GtkGrid
 */
GtkWidget *logfile_widget_create(void)
{
    GtkWidget  *grid;
    GtkWidget  *logfile_browser;
    GtkWidget  *entry;
    GtkWidget  *stdout_check;
    const char *logfilename = NULL;
    char       *logfile_default;

    int         row = 1;

    grid = vice_gtk3_grid_new_spaced_with_label(8, 8, "VICE log file", 1);

    logfile_default = archdep_default_logfile();
    resources_get_string("LogFileName", &logfilename);
    logfile_browser = vice_gtk3_resource_browser_save_new("LogFileName",
                                                          "Select log file",
                                                          NULL,
                                                          NULL,
                                                          NULL);
    if (logfilename == NULL || *logfilename == '\0') {
        vice_gtk3_resource_browser_set_directory(logfile_browser,
                                                archdep_user_state_path());
    }
    entry = gtk_grid_get_child_at(GTK_GRID(logfile_browser), 0, 0);
    /* align with the CWD widget in the parent widget (host->environment) */
    gtk_widget_set_margin_start(entry, 8);
    gtk_widget_set_margin_end(logfile_browser, 8);
    gtk_grid_set_column_spacing(GTK_GRID(logfile_browser), 8);
    /* use the default log file path as a placeholder text */
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry), logfile_default);
    lib_free(logfile_default);
    gtk_grid_attach(GTK_GRID(grid), logfile_browser, 0, row, 1, 1);
    row++;

    stdout_check = gtk_check_button_new_with_label("Log to stdout");
    logfilename = gtk_entry_get_text(GTK_ENTRY(entry));
    if (g_strcmp0(logfilename, "-") == 0) {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(stdout_check), TRUE);
    }
    gtk_grid_attach(GTK_GRID(grid), stdout_check, 0, row, 1, 1);
    row++;

    launcher = gtk_button_new_with_label("Open directory containing log file");
    gtk_grid_attach(GTK_GRID(grid), launcher, 0, row, 1, 1);
    g_signal_connect(launcher,
                     "clicked",
                     G_CALLBACK(on_launcher_clicked),
                     NULL);

    check_handler = g_signal_connect(stdout_check,
                                     "toggled",
                                     G_CALLBACK(on_stdout_check_toggled),
                                     (gpointer)logfile_browser);
    g_signal_connect_unlocked(entry,
                              "changed",
                              G_CALLBACK(on_logfile_entry_changed),
                              (gpointer)stdout_check);


    gtk_widget_show_all(grid);
    return grid;
}
