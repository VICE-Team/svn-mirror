/*
 * uiscreenshot.c - screenshot dialog for the Gnome widget set.
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

#include "gfxoutput.h"
#include "screenshot.h"
#include "ui.h"
#include "uiarch.h"
#include "utils.h"


static GtkWidget *screenshot_dialog, *fileentry;

typedef struct 
{
    const char *driver;
    GtkWidget *w;
} img_type_buttons;

static img_type_buttons *buttons = NULL;

static GtkWidget *build_screenshot_dialog(void)
{
    GtkWidget *d, *box, *tmp, *frame, *hbox;
    int i, num_buttons;
    gfxoutputdrv_t *driver;
    
    d = gnome_dialog_new(_("Save Screenshot"), 
			 GNOME_STOCK_BUTTON_OK, 
			 GNOME_STOCK_BUTTON_CANCEL,
			 NULL);
    box = gtk_hbox_new(0, FALSE);

    tmp = gtk_label_new(_("Filename: "));
    gtk_box_pack_start(GTK_BOX(box), tmp, FALSE, FALSE, 0);
    gtk_widget_show(tmp);

    fileentry = gnome_file_entry_new("vice: save screenshot", 
				     _("Save Screenshot"));
    gnome_dialog_editable_enters(GNOME_DIALOG(d), 
				 GTK_EDITABLE(gnome_file_entry_gtk_entry
					      (GNOME_FILE_ENTRY(fileentry))));
    gnome_dialog_set_default(GNOME_DIALOG(d), GNOME_OK);

    gtk_box_pack_start(GTK_BOX(box), fileentry,
		       TRUE, TRUE, GNOME_PAD);
    gtk_widget_show(fileentry);

    gtk_box_pack_start(GTK_BOX(GNOME_DIALOG(d)->vbox), box, TRUE, TRUE, 0);
    gtk_widget_show(box);

    frame = gtk_frame_new(_("Image Format"));
    hbox = gtk_hbox_new(0, FALSE);
    
    num_buttons = gfxoutput_num_drivers();
    if (! buttons)
	buttons = xmalloc(sizeof (img_type_buttons) * num_buttons);
    
    driver = gfxoutput_drivers_iter_init();
    for (i = 0; i < num_buttons; i++)
    {
	if (i == 0)
	{
	    buttons[i].w = gtk_radio_button_new_with_label(NULL, driver->name);
	    
	    gtk_toggle_button_set_active(
		GTK_TOGGLE_BUTTON(buttons[i].w), TRUE);
	}
	else
	    buttons[i].w = gtk_radio_button_new_with_label(
		gtk_radio_button_group(
		    GTK_RADIO_BUTTON(buttons[i - 1].w)),
		driver->name);
	gtk_box_pack_start(GTK_BOX(hbox), buttons[i].w, FALSE, FALSE, 0);
	gtk_widget_show(buttons[i].w);
	buttons[i].driver = driver->name;
	driver = gfxoutput_drivers_iter_next();
    }

    gtk_container_add(GTK_CONTAINER(frame), hbox);
    gtk_widget_show(hbox);

    gtk_box_pack_start(GTK_BOX(GNOME_DIALOG(d)->vbox), frame, TRUE, TRUE, 0);
    gtk_widget_show(frame);
    gtk_widget_show(d);
    
    return d;
}

int ui_screenshot_dialog(char *name, int wid)
{
    int res, num_buttons, i;
    char *fn;
    const char *driver;
    
    if (screenshot_dialog)
    {
    	gdk_window_show(screenshot_dialog->window);
	gdk_window_raise(screenshot_dialog->window);
	gtk_widget_show(screenshot_dialog);
    }
    else
    {
	screenshot_dialog = build_screenshot_dialog();
	gtk_signal_connect(GTK_OBJECT(screenshot_dialog),
			   "destroy",
			   GTK_SIGNAL_FUNC(gtk_widget_destroyed),
			   &screenshot_dialog);
    }

    ui_popup(screenshot_dialog, _("Save Screenshot"), FALSE);
    res = gnome_dialog_run(GNOME_DIALOG(screenshot_dialog));
    ui_popdown(screenshot_dialog);
    
    if (res != 0)
	return -1;
    
    fn = gnome_file_entry_get_full_path(GNOME_FILE_ENTRY(fileentry), FALSE);
    if (!fn)
    {
	ui_error(_("Invalid filename"));
	return -1;
    }

    driver = NULL;
    num_buttons = gfxoutput_num_drivers();
    for (i = 0; i < num_buttons; i++)
	if (GTK_TOGGLE_BUTTON(buttons[i].w)->active)
	{
	    driver = buttons[i].driver;
	    break;
	}
    
    if (!driver)
	return -1;
    
    strcpy (name, fn);		/* What for? */
    if (screenshot_save(driver, fn, wid) < 0)
    {
	ui_error(_("Couldn't write screenshot to `%s' with driver `%s'."), fn, 
		 driver);
	return -1;
    }
    else
	ui_message(_("Successfully wrote `%s'"), fn);
        
    return 0;
}
