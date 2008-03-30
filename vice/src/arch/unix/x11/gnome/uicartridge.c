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


static GtkWidget *cartridge_dialog, *fileentry;

static GtkWidget *build_cartridge_dialog(void)
{
    GtkWidget *d, *box, *tmp;
    
    d = gnome_dialog_new(_("Save Cartridge"), 
			 GNOME_STOCK_BUTTON_OK, 
			 GNOME_STOCK_BUTTON_CANCEL,
			 NULL);
    box = gtk_hbox_new(0, FALSE);

    tmp = gtk_label_new(_("Filename: "));
    gtk_box_pack_start(GTK_BOX(box), tmp, FALSE, FALSE, 0);
    gtk_widget_show(tmp);

    fileentry = gnome_file_entry_new("vice: Cartridge", _("Save Cartridge"));
    gnome_dialog_editable_enters(GNOME_DIALOG(d), 
				 GTK_EDITABLE(gnome_file_entry_gtk_entry
					      (GNOME_FILE_ENTRY(fileentry))));
    gnome_dialog_set_default(GNOME_DIALOG(d), GNOME_OK);

    gtk_box_pack_start(GTK_BOX(box), fileentry,
		       TRUE, TRUE, GNOME_PAD);
    gtk_widget_show(fileentry);

    gtk_box_pack_start(GTK_BOX(GNOME_DIALOG(d)->vbox), box, TRUE, TRUE, 0);
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
	gtk_signal_connect(GTK_OBJECT(cartridge_dialog),
			   "destroy",
			   GTK_SIGNAL_FUNC(gtk_widget_destroyed),
			   &cartridge_dialog);
    }

    ui_popup(cartridge_dialog, _("Save Cartridge"), FALSE);
    res = gnome_dialog_run(GNOME_DIALOG(cartridge_dialog));
    ui_popdown(cartridge_dialog);
    
    if (res != 0)
	return;
    
    name = gnome_file_entry_get_full_path(GNOME_FILE_ENTRY(fileentry), FALSE);
    if (!name)
    {
	ui_error(_("Invalid filename"));
	return;
    }
	    
    if (cartridge_save_image (name) < 0)
        ui_error(_("Cannot write cartridge image file\n`%s'\n"), name);
    else
	ui_message(_("Successfully wrote `%s'\n"), name);
}

