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
#include "ui.h"

#include <gnome.h>

static void license_cb(GtkWidget *w, GdkEvent *event, gpointer data)
{
    ui_show_text("VICE is FREE software!", license_text, 500, 300);
}

static void warranty_cb(GtkWidget *w, GdkEvent *event, gpointer data)
{
    ui_show_text("No warranty!", warranty_text, 500, 300);
}

static void contrib_cb(GtkWidget *w, GdkEvent *event, gpointer data)
{
    ui_show_text("Contributors to the VICE project", contrib_text, 500, 300);
}


void ui_about(gpointer data)
{
    GtkWidget *button;
    
    const gchar *authors[] = {
	"The VICE Team", 
	"Copyright @ 1996-1999 Ettore Perazzoli\n",
	"Copyright @ 1996-1999 André Fachat\n",
	"Copyright @ 1993-1994, 1997-1999 Teemu Rantanen\n",
	"Copyright @ 1997-1999 Daniel Sladic\n",
	"Copyright @ 1998-1999 Andreas Boose\n",
	"Copyright @ 1998-1999 Tibor Biczo\n",
	"Copyright @ 1993-1996 Jouko Valta\n",
	"Copyright @ 1993-1994 Jarkko Sonninen\n",
#ifdef HAVE_RESID
	"reSID engine Copyright © 1999 Dag Lem",
#endif
	"",
	"Official VICE homepage:",
	"http://www.cs.cmu.edu/~dsladic/vice/vice.html",
	NULL};
    GtkWidget *about = 
	gnome_about_new("V I C E",
			VERSION,
			"", 
			authors,
			"",
			NULL);

    button = gnome_stock_or_ordinary_button ("License");
    gtk_box_pack_start (GTK_BOX (GNOME_DIALOG(about)->action_area), 
			button, TRUE, TRUE, 0);
    gtk_signal_connect(GTK_OBJECT(button), "clicked",
		       GTK_SIGNAL_FUNC(license_cb), NULL);
    gtk_widget_show(button);
    button = gnome_stock_or_ordinary_button ("Warranty");
    gtk_box_pack_start (GTK_BOX (GNOME_DIALOG(about)->action_area), 
			button, TRUE, TRUE, 0);
    gtk_signal_connect(GTK_OBJECT(button), "clicked",
		       GTK_SIGNAL_FUNC(warranty_cb), NULL);
    gtk_widget_show(button);
    button = gnome_stock_or_ordinary_button ("Contributors");
    gtk_box_pack_start (GTK_BOX (GNOME_DIALOG(about)->action_area), 
			button, TRUE, TRUE, 0);
    gtk_signal_connect(GTK_OBJECT(button), "clicked",
		       GTK_SIGNAL_FUNC(contrib_cb), NULL);
    gtk_widget_show(button);
    gtk_widget_show(about);
}

