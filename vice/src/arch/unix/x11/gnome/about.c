/*
 * about.c - Info about the VICE project, including the GPL.
 *
 * Written by
 *  Martin Pottendorfer
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

#include "version.h"
#include "info.h"
#include "uiarch.h"

GtkWidget *about;

static void license_cb(GtkWidget *w, GdkEvent *event, gpointer data)
{
    ui_show_text(_("VICE is FREE software!"), info_license_text, 500, 300);
}

static void warranty_cb(GtkWidget *w, GdkEvent *event, gpointer data)
{
    ui_show_text(_("No warranty!"), info_warranty_text, 500, 300);
}

static void contrib_cb(GtkWidget *w, GdkEvent *event, gpointer data)
{
    ui_show_text(_("Contributors to the VICE project"), info_contrib_text,
                 500, 300);
}


void ui_about(gpointer data)
{
    GtkWidget *button;
    
    const gchar *authors[] = {
#ifdef __GNUC__
	_("The VICE Team"), 
#else
	"The VICE Team",
#endif
        "Copyright @ 1998-2006 Andreas Boose",
        "Copyright @ 1998-2006 Tibor Biczo",
        "Copyright @ 1999-2006 Andreas Dehmel",
        "Copyright @ 1999-2006 Andreas Matthies",
        "Copyright @ 1999-2006 Martin Pottendorfer",
        "Copyright @ 1998-2006 Dag Lem",
        "Copyright @ 2000-2006 Spiro Trikaliotis",
        "Copyright @ 2005-2006 Marco van den Heuvel",
        "Copyright @ 1999-2005 Thomas Bretz",
        "Copyright @ 2003-2005 David Hansel",
        "Copyright @ 2000-2004 Markus Brenner",
        "",
#ifdef __GNUC__
	_("Official VICE homepage:"),
#else
	"Official VICE homepage:",
#endif
	"http://www.viceteam.org/",
	NULL};
    const gchar *docs[] = {
	"Ettore Perazzoli et al.", 
	NULL};
    const gchar *transl = _(
	"Martin Pottendorfer - German\n"
	"Peter Karlsson - Swedish\n"
	"Andrea Musuruane - Italian\n"
	"Paul Dube - French\n"
	"Marco van den Heuvel - Dutch\n"
	"Flooder - Polish\n"
	"Karai Csaba - Hungarian\n");
    if (!about)
    {
	/* GdkPixbuf *logo = gdk_pixbuf_new_from_file ("logo.png", NULL); */
	about = g_object_new(GTK_TYPE_ABOUT_DIALOG,
			     "name", "V I C E", 
			     "version", VERSION, 
			     "copyright", _("(c) 1998 - 2006 The Vice Team"), 
			     "comments", "Versatile Commodore Emulator",
			     "authors", authors, 
			     "documenters", docs, 
			     "translator-credits", transl, 
/* 			     "logo", logo, */
			     NULL);
	g_signal_connect(G_OBJECT(about),
			 "destroy",
			 G_CALLBACK(gtk_widget_destroyed),
			 &about);
	button = gtk_dialog_add_button(GTK_DIALOG(about), _("License"), 
				       GTK_RESPONSE_OK);
	g_signal_connect(GTK_OBJECT(button), "clicked",
			   GTK_SIGNAL_FUNC(license_cb), NULL);
	button = gtk_dialog_add_button(GTK_DIALOG(about), _("Warranty"),
				       GTK_RESPONSE_OK);
	g_signal_connect(GTK_OBJECT(button), "clicked",
			 GTK_SIGNAL_FUNC(warranty_cb), NULL);
	button = gtk_dialog_add_button(GTK_DIALOG(about),_("Contributors"),
				       GTK_RESPONSE_OK);
	g_signal_connect(GTK_OBJECT(button), "clicked",
			 GTK_SIGNAL_FUNC(contrib_cb), NULL);
    }
    else
    {
	gdk_window_show(about->window);
	gdk_window_raise(about->window);
    }

    ui_make_window_transient(_ui_top_level, about);
    gtk_widget_show(about);
}

