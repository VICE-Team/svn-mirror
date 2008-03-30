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
#include "lib.h"
#include "screenshot.h"
#include "resources.h"
#include "ui.h"
#include "uiarch.h"
#include "uiscreenshot.h"


extern GtkWidget *video_ctrl_checkbox;
static GtkWidget *screenshot_dialog, *fileentry;
#ifdef HAVE_FFMPEG
static GtkWidget *ffmpg_opts, *ffmpg_audio, *ffmpg_video;
#endif

typedef struct 
{
    const char *driver;
    GtkWidget *w;
} img_type_buttons;

static img_type_buttons *buttons = NULL;

#ifdef HAVE_FFMPEG
static void ffmpg_widget (GtkWidget *w, gpointer data)
{
    int num_buttons, i;
    
    num_buttons = gfxoutput_num_drivers();
    for (i = 0; i < num_buttons; i++)
	if (GTK_TOGGLE_BUTTON(buttons[i].w)->active)
	    if (strcmp(buttons[i].driver, "FFMPEG") == 0) {
		gtk_widget_set_sensitive(ffmpg_opts, TRUE);
		return;
	    }
    gtk_widget_set_sensitive(ffmpg_opts, FALSE);
}
#endif

static GtkWidget *build_screenshot_dialog(void)
{
    GtkWidget *d, *box, *tmp, *frame, *hbox, *vbox;
#ifdef HAVE_FFMPEG
    GtkWidget *l;
    GtkObject *adj;
    unsigned long v;
#endif
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
    vbox = gtk_vbox_new(FALSE, 5);
    hbox = gtk_vbox_new(FALSE, 5);
    
    num_buttons = gfxoutput_num_drivers();
    if (! buttons)
	buttons = lib_malloc(sizeof(img_type_buttons) * num_buttons);
    
    driver = gfxoutput_drivers_iter_init();
    for (i = 0; i < num_buttons; i++) {
	if (i == 0) {
	    buttons[i].w = gtk_radio_button_new_with_label(NULL, driver->displayname);
	    
	    gtk_toggle_button_set_active(
		GTK_TOGGLE_BUTTON(buttons[i].w), TRUE);
	} else 
	    buttons[i].w = gtk_radio_button_new_with_label(
		gtk_radio_button_group(
		    GTK_RADIO_BUTTON(buttons[i - 1].w)),
		driver->displayname);
	gtk_box_pack_start(GTK_BOX(hbox), buttons[i].w, FALSE, FALSE, 0);
	gtk_widget_show(buttons[i].w);
	buttons[i].driver = driver->name;
#ifdef HAVE_FFMPEG
	gtk_signal_connect(GTK_OBJECT(buttons[i].w), "clicked",
			   GTK_SIGNAL_FUNC(ffmpg_widget),
			   0);
#endif
	driver = gfxoutput_drivers_iter_next();
	
    }

    gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
    gtk_widget_show(hbox);

#ifdef HAVE_FFMPEG    
    /* ffmpg options */
    resources_get_value("FFMPEGAudioBitrate", (resource_value_t *) &v);
    adj = gtk_adjustment_new ((gfloat) v, 
			      (gfloat) 16000, 
			      (gfloat) 128000,
			      (gfloat) 1000,
			      (gfloat) 10000,
			      (gfloat) 10000);
    ffmpg_opts = gtk_vbox_new(FALSE, 5);
    ffmpg_audio = gtk_spin_button_new(GTK_ADJUSTMENT(adj), (gfloat) 1000, 0);
    gtk_widget_set_usize(ffmpg_audio, 100, 16);
    l = gtk_label_new(_("Audio Bitrate"));
    tmp = gtk_hbox_new(FALSE, 5);
    gtk_box_pack_start(GTK_BOX(tmp), l, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(tmp), ffmpg_audio, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(ffmpg_opts), tmp, FALSE, FALSE, 0);
    
    resources_get_value("FFMPEGVideoBitrate", (resource_value_t *) &v);
    adj = gtk_adjustment_new ((gfloat) v, 
			      (gfloat) 100000, 
			      (gfloat) 10000000,
			      (gfloat) 10000,
			      (gfloat) 100000,
			      (gfloat) 100000);
    ffmpg_video = gtk_spin_button_new(GTK_ADJUSTMENT(adj), (gfloat) 10000, 0);
    gtk_widget_set_usize(ffmpg_video, 100, 16);
    l = gtk_label_new(_("Video Bitrate"));
    tmp = gtk_hbox_new(FALSE, 5);
    gtk_box_pack_start(GTK_BOX(tmp), l, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(tmp), ffmpg_video, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(ffmpg_opts), tmp, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(vbox), ffmpg_opts, FALSE, FALSE, 0);
    gtk_widget_show_all(ffmpg_opts);
    gtk_widget_set_sensitive(ffmpg_opts, FALSE);
#endif
    
    gtk_container_add(GTK_CONTAINER(frame), vbox);
    gtk_widget_show(vbox);

    gtk_box_pack_start(GTK_BOX(GNOME_DIALOG(d)->vbox), frame, TRUE, TRUE, 0);
    gtk_widget_show(frame);
    gtk_widget_show(d);
    
    return d;
}

int ui_screenshot_dialog(char *name, struct video_canvas_s *wid)
{
    int res, num_buttons, i;
    char *fn;
    const char *driver;
    
    if (screenshot_dialog) {
    	gdk_window_show(screenshot_dialog->window);
	gdk_window_raise(screenshot_dialog->window);
	gtk_widget_show(screenshot_dialog);
    } else {
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
    if (!fn) {
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
#ifdef HAVE_FFMPEG
    if (strcmp(driver, "FFMPEG") == 0)
    {
	unsigned int v;
	v = (unsigned int) 
	    gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(ffmpg_audio));
	resources_set_value("FFMPEGAudioBitrate", (resource_value_t) v);
	v = (unsigned int) 
	    gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(ffmpg_video));
	resources_set_value("FFMPEGVideoBitrate", (resource_value_t) v);
    }
#endif    
    strcpy(name, fn);		/* What for? */
    if (screenshot_save(driver, fn, wid) < 0) {
	ui_error(_("Couldn't write screenshot to `%s' with driver `%s'."), fn, 
		 driver);
	return -1;
    } else {
	if (screenshot_is_recording())
	    gtk_widget_show(video_ctrl_checkbox);
	ui_message(_("Successfully wrote `%s'"), fn);
    }

    return 0;
}

void uiscreenshot_shutdown(void)
{
    lib_free(driver_buttons);
}

