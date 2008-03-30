/*
 * uicartridge.c - Cartridge save image dialog for the Xaw widget set.
 *
 * Written by
 *  Martin Pottendorfer <Martin.Pottendorfer@alcatel.at>
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

#include <stdio.h>

#include "ui.h"
#include "uiarch.h"
#include "cartridge.h"
#include "uifileentry.h"
#include "lib.h"

static GtkWidget *cartridge_dialog, *fileentry;

static GtkWidget *build_cartridge_dialog(void)
{
    GtkWidget *d, *box;
    
    d = gtk_dialog_new_with_buttons(_("Save Cartridge"), 
				    NULL,
				    GTK_DIALOG_DESTROY_WITH_PARENT,
				    GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
				    GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
				    NULL);
    box = gtk_hbox_new(0, FALSE);

    fileentry = vice_file_entry(_("Save Cartridge"), NULL, NULL, 
				GTK_FILE_CHOOSER_ACTION_SAVE);
    gtk_dialog_set_default_response(GTK_DIALOG(d), GTK_RESPONSE_ACCEPT);

    gtk_box_pack_start(GTK_BOX(box), fileentry,
		       TRUE, TRUE, GNOME_PAD);
    gtk_widget_show(fileentry);
    
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(d)->vbox), box, TRUE, TRUE, 0);
    gtk_widget_show(box);
    
    return d;
}

void ui_cartridge_dialog(void)
{
    gint res;
    char *name;
    
    if (cartridge_dialog)
    {
	gdk_window_show(cartridge_dialog->window);
	gdk_window_raise(cartridge_dialog->window);
	gtk_widget_show(cartridge_dialog);
    }
    else
    {
	cartridge_dialog = build_cartridge_dialog();
	g_signal_connect(G_OBJECT(cartridge_dialog),
			 "destroy",
			 G_CALLBACK(gtk_widget_destroyed),
			 &cartridge_dialog);
    }

    ui_popup(cartridge_dialog, _("Save Cartridge"), FALSE);
    res = gtk_dialog_run(GTK_DIALOG(cartridge_dialog));
    ui_popdown(cartridge_dialog);
    
    if (res != GTK_RESPONSE_ACCEPT)
	return;
    
    name = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(fileentry));
    if (!name)
    {
	ui_error(_("Invalid filename"));
	return;
    }
	    
    if (cartridge_save_image (name) < 0)
        ui_error(_("Cannot write cartridge image file\n`%s'\n"), name);
    else
	ui_message(_("Successfully wrote `%s'\n"), name);
    lib_free(name);
}

