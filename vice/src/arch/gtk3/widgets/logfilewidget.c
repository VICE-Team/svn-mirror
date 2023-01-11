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
#include "vice_gtk3.h"

#include "logfilewidget.h"


/** \brief  ID of the 'changed' signal handler of the "stdout" check button
 *
 * This ID is used to temporarily block the signal to avoid signal handlers of
 * the check button and the text entry triggering each other.
 */
static gulong check_handler;


/** \brief  Handler for the 'changed' event of the entry
 *
 * Set \a check state depending on whether \a entry contains "-".
 *
 * \param[in]   entry   GtkEntry containing the LogFileName contents
 * \param[in]   check   GtkCheckButton for "log to stdout"
 */
static void on_logfile_entry_changed(GtkWidget *entry, gpointer check)
{
    const char *text = gtk_entry_get_text(GTK_ENTRY(entry));

    /* avoid signals bouncing */
    g_signal_handler_block(check, check_handler);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check),
                                 g_strcmp0(text, "-") == 0);
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
    const char *logfilename;
    char       *path;
    int         row = 1;

    grid = vice_gtk3_grid_new_spaced_with_label(8, 8, "VICE log file", 1);

    path = g_build_path(ARCHDEP_DIR_SEP_STR,
                        archdep_user_config_path(),
                        "vice.log",
                        NULL);
    logfile_browser = vice_gtk3_resource_browser_save_new("LogFileName",
                                                          "Select log file",
                                                          NULL,
                                                          NULL,
                                                          NULL);
    entry = gtk_grid_get_child_at(GTK_GRID(logfile_browser), 0, 0);
    /* align with the CWD widget in the parent widget (host->environment) */
    gtk_widget_set_margin_start(entry, 8);
    gtk_widget_set_margin_end(logfile_browser, 8);
    gtk_grid_set_column_spacing(GTK_GRID(logfile_browser), 8);
    /* use the default log file path as a placeholder text */
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry), path);
    g_free(path);
    gtk_grid_attach(GTK_GRID(grid), logfile_browser, 0, row, 1, 1);
    row++;

    stdout_check = gtk_check_button_new_with_label("Log to stdout");
    logfilename = gtk_entry_get_text(GTK_ENTRY(entry));
    if (g_strcmp0(logfilename, "-") == 0) {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(stdout_check), TRUE);
    }
    gtk_grid_attach(GTK_GRID(grid), stdout_check, 0, row, 1, 1);

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
