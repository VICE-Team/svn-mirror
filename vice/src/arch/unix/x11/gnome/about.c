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

#include <gnome.h>

void ui_about(gpointer data)
{
    const gchar *authors[] = {
	"The VICE Team", NULL 
    };
    GtkWidget *about = 
	gnome_about_new("V I C E",
			VERSION,
			"", 
			authors,
			"Copyright @ 1996-1999 Ettore Perazzoli\n"
			"Copyright @ 1996-1999 André Fachat\n"
			"Copyright @ 1993-1994, 1997-1999 Teemu Rantanen\n"
			"Copyright @ 1997-1999 Daniel Sladic\n"
			"Copyright @ 1998-1999 Andreas Boose\n"
			"Copyright @ 1998-1999 Tibor Biczo\n"
			"Copyright @ 1993-1996 Jouko Valta\n"
			"Copyright @ 1993-1994 Jarkko Sonninen\n",
			NULL);
    gtk_window_set_modal(GTK_WINDOW(about), TRUE);
    gtk_widget_show(about);
}

