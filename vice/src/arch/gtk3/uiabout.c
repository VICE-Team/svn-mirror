/*
 * uiabout.c - GTK3 about dialog
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
 */


#include "vice.h"

#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>

#include "info.h"
#if 0
#include" infocontrib.h"
#endif
#include "lib.h"

#include "uiabout.h"



static const char **create_current_team_list(void)
{
    const char **list = lib_malloc(sizeof *list * 256); /* horseshit! */
    size_t i = 0;

    while (core_team[i].name != NULL) {
        list[i] = core_team[i].name;
        i++;
    }
    list[i] = NULL;
    return list;
}


static void destroy_current_team_list(char **list)
{
    lib_free(list);
}


static void about_destroy_callback(GtkWidget *widget, gpointer user_data)
{
    destroy_current_team_list((char **)user_data);
}



GtkWidget *ui_about_create_dialog(void)
{
    GtkWidget *about = gtk_about_dialog_new();

    const char **authors = create_current_team_list();

    g_print("[debug] %s() called\n", __func__);

    gtk_window_set_title(GTK_WINDOW(about), "About VICE");
    gtk_about_dialog_set_license(GTK_ABOUT_DIALOG(about), info_license_text);
    gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(about),
            "http://vice.pokefinder.org");
    gtk_about_dialog_set_website_label(GTK_ABOUT_DIALOG(about),
            "http://vice.pokefinder.org");
    gtk_about_dialog_set_authors(GTK_ABOUT_DIALOG(about), authors);
    gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(about),
            "Copyright 1996-2017 VICE TEAM");

    g_signal_connect(about, "destroy", G_CALLBACK(about_destroy_callback),
            (gpointer)authors);


    return about;
}


