/*
 * uiedisk.c - emptydisk dialog for the Gnome widget set.
 *
 * Written by
 *  Martin Pottendorfer (Martin.Pottendorfer@alcatel.at)
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
#include <gnome.h>

#include "uiedisk.h"
#include "ui.h"
#include "uiarch.h"
#include "diskimage.h"
#include "vdrive/vdrive.h"
#include "utils.h"

static struct {
    char *label;
    GtkWidget *w;
    int type;
} type_radio[] = { {"d64", NULL, DISK_IMAGE_TYPE_D64},
		   {"d71", NULL, DISK_IMAGE_TYPE_D71},
		   {"d80", NULL, DISK_IMAGE_TYPE_D80},
		   {"d81", NULL, DISK_IMAGE_TYPE_D81},
		   {"d82", NULL, DISK_IMAGE_TYPE_D82},
		   {"g64", NULL, DISK_IMAGE_TYPE_G64},
		   {"x64", NULL, DISK_IMAGE_TYPE_X64},
		   { NULL, NULL, 0} };

static GtkWidget *edisk_dialog, *fileentry, *diskname, *diskid;

static GtkWidget *build_empty_disk_dialog(void)
{
    GtkWidget *d, *box, *hbox, *tmp, *frame;
    int i;
    
    d = gnome_dialog_new(_("Create empty disk"),
			 GNOME_STOCK_BUTTON_OK, 
			 GNOME_STOCK_BUTTON_CANCEL,
			 NULL);
    box = gtk_hbox_new(0, FALSE);

    tmp = gtk_label_new(_("Filename: "));
    gtk_box_pack_start(GTK_BOX(box), tmp, FALSE, FALSE, 0);
    gtk_widget_show(tmp);

    fileentry = gnome_file_entry_new("vice: save snapshot", _("Save Snapshot"));
    gnome_dialog_editable_enters(GNOME_DIALOG(d), 
				 GTK_EDITABLE(gnome_file_entry_gtk_entry
					      (GNOME_FILE_ENTRY(fileentry))));
    gnome_dialog_set_default(GNOME_DIALOG(d), GNOME_OK);

    gtk_box_pack_start(GTK_BOX(box), fileentry,
		       TRUE, TRUE, GNOME_PAD);
    gtk_widget_show(fileentry);

    gtk_box_pack_start(GTK_BOX(GNOME_DIALOG(d)->vbox), box, TRUE, TRUE, 0);
    gtk_widget_show(box);
    
    frame = gtk_frame_new(_("Disk options"));
    box = gtk_vbox_new(0, FALSE);

    /* Diskname */
    hbox = gtk_hbox_new(0, FALSE);
    tmp = gtk_label_new(_("Diskname: "));
    gtk_box_pack_start(GTK_BOX(hbox), tmp, FALSE, FALSE, 0);
    gtk_widget_show(tmp);
    
    diskname = gtk_entry_new_with_max_length(16);
    gtk_entry_set_editable(GTK_ENTRY(diskname), TRUE);
    gtk_box_pack_start(GTK_BOX(hbox), diskname, FALSE, FALSE, 0);
    gtk_widget_show(diskname);
    
    tmp = gtk_label_new("ID: ");
    gtk_box_pack_start(GTK_BOX(hbox), tmp, FALSE, FALSE, 0);
    gtk_widget_show(tmp);
    diskid = gtk_entry_new_with_max_length(2);
    gtk_entry_set_editable(GTK_ENTRY(diskid), TRUE);
    gtk_box_pack_start(GTK_BOX(hbox), diskid, FALSE, FALSE, 0);
    gtk_widget_set_usize(diskid, 25, 22);
    gtk_widget_show(diskid);

    gtk_box_pack_start(GTK_BOX(box), hbox, FALSE, FALSE, 0);
    gtk_widget_show(hbox);

    hbox = gtk_hbox_new(0, FALSE);

    for (i = 0; type_radio[i].label; i++)
    {
	if (i == 0)
	{
	    type_radio[i].w = 
		gtk_radio_button_new_with_label(NULL, type_radio[i].label);
	    gtk_toggle_button_set_active(
		GTK_TOGGLE_BUTTON(type_radio[i].w ), TRUE);
	}
	else
	    type_radio[i].w = gtk_radio_button_new_with_label(
		gtk_radio_button_group(
		    GTK_RADIO_BUTTON(type_radio[i - 1].w)), 
		type_radio[i].label);
	
	gtk_box_pack_start(GTK_BOX(hbox), type_radio[i].w, FALSE, FALSE, 0);
	gtk_widget_show(type_radio[i].w);
    }

    gtk_box_pack_start(GTK_BOX(box), hbox, FALSE, FALSE, 0);
    gtk_widget_show(hbox);
    
    gtk_container_add(GTK_CONTAINER(frame), box);
    gtk_widget_show(box);

    gtk_box_pack_start(GTK_BOX(GNOME_DIALOG(d)->vbox), frame, 
		       FALSE, FALSE, GNOME_PAD);
    gtk_widget_show(frame);
    gtk_widget_show(d);
    
    return d;
}

int ui_empty_disk_dialog(char *name)
{
    gint res;
    char *fname, *id, *format_text;
    int i, type = 0, ret = 0;

    if (edisk_dialog)
    {
	gdk_window_show(edisk_dialog->window);
	gdk_window_raise(edisk_dialog->window);
	gtk_widget_show(edisk_dialog);
    }
    else
    {
	edisk_dialog = build_empty_disk_dialog();
	gtk_signal_connect(GTK_OBJECT(edisk_dialog),
			   "destroy",
			   GTK_SIGNAL_FUNC(gtk_widget_destroyed),
			   &edisk_dialog);
    }

    ui_popup(edisk_dialog, _("Create empty Diskimage"), FALSE);
    res = gnome_dialog_run(GNOME_DIALOG(edisk_dialog));
    ui_popdown(edisk_dialog);

    if (res != 0)
	return -1;

    /* filename */
    fname = gnome_file_entry_get_full_path(GNOME_FILE_ENTRY(fileentry), FALSE);
    if (!fname)
	return -1;
    
    strcpy(name, fname);

    /* format label */
    fname = gtk_entry_get_text(GTK_ENTRY(diskname));
    if (!fname)
	fname = "";

    /* disk ID */
    id = gtk_entry_get_text(GTK_ENTRY(diskid));
    if (!id)
	id = "00";

    /* type radio button */
    for (i = 0; type_radio[i].label; i++)
	if (GTK_TOGGLE_BUTTON(type_radio[i].w)->active)
	{
	    type = type_radio[i].type;
	    break;
	}

    format_text = concat(fname, ",", id, NULL);
    if (vdrive_internal_create_format_disk_image(name, format_text, type) < 0)
    {
	ui_error(_("Can't create image `%s'."));
	ret = -1;
    }
    free(format_text);

    return ret;
}

