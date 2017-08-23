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
#include <stdarg.h>

#include "info.h"
#if 0
#include" infocontrib.h"
#endif
#include "lib.h"
#include "version.h"
#include "vicefeatures.h"
#ifdef USE_SVN_REVISION
#include "svnversion.h"
#endif

#include "uiabout.h"


static char **authors;


static char **create_current_team_list(void)
{
    char **list = lib_malloc(sizeof *list * 256); /* TODO: get proper
                                                           length */
    size_t i = 0;

    while (core_team[i].name != NULL) {
        list[i] = core_team[i].name;
        i++;
    }
    list[i] = NULL;
    return list;
}


#if 0
static char **create_copyright_list(void)
{
    char **list = lib_malloc(sizeof *list * 256);
    size_t i;

    while (core_team[i].name != NULL) {
        char *member = lib_malloc(256);
        snprintf(member, 256, "%s - %s", core_team[i].years, core_team[i].name);
        list[i++] = member;
    }
    /* TODO: add ex-team */
    list[i] = NULL;
    return list;
}
#endif

#if 0
static char **create_translators_list(void)
{
    char **list = lib_malloc(sizeof *list * 256);
    size_t i;

    while (trans_team[i].name != NULL) {
        char *member = lib_malloc(256);
        snprintf(member, 256, "%s - %s (%s)",
                trans_team[i].years,
                trans_team[i].name,
                trans_team[i].language);
        list[i++] = member;
    }
    list[i] = NULL;
    return list;
}
#endif


static void destroy_current_team_list(char **list)
{
    lib_free(list);
}


#if 0
static void destroy_copyright_list(char **list)
{
    size_t i = 0;

    while (list[i] != NULL) {
        lib_free(list[i]);
        i++;
    }
    lib_free(list);
}
#endif


/** \brief  Create VICE logo
 *
 * \return  GdkPixbuf instance
 */
static GdkPixbuf *get_vice_logo(void)
{
    GdkPixbuf *logo;
    GError *err = NULL;

    logo = gdk_pixbuf_new_from_file("/usr/local/lib64/vice/doc/vice-logo.jpg", &err);
    if (logo == NULL || err != NULL) {
        g_print("%s(): %s\n", __func__, err->message);
    }
    return logo;
}



static void about_destroy_callback(GtkWidget *widget, gpointer user_data)
{
    destroy_current_team_list(authors);
}

void ui_about_dialog_callback(GtkWidget *widget, gpointer user_data)
{
    GtkWidget *about = gtk_about_dialog_new();
    authors = create_current_team_list();

    g_print("[debug] %s() called\n", __func__);

    gtk_window_set_title(GTK_WINDOW(about), "About VICE");

    gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(about),
#ifdef USE_SVN_REVISION
            VERSION " r" VICE_SVN_REV_STRING " (Gkt3)");
#else
            VERSION " (Gtk3)");
#endif



    gtk_about_dialog_set_license(GTK_ABOUT_DIALOG(about), info_license_text);
    gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(about),
            "http://vice.pokefinder.org");
    gtk_about_dialog_set_website_label(GTK_ABOUT_DIALOG(about),
            "http://vice.pokefinder.org");
    gtk_about_dialog_set_authors(GTK_ABOUT_DIALOG(about), (const gchar **)authors);
    gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(about),
            "Copyright 1996-2017 VICE TEAM");

    gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(about), get_vice_logo());


    g_signal_connect(about, "destroy", G_CALLBACK(about_destroy_callback),
            NULL);

    gtk_widget_show(about);
}
