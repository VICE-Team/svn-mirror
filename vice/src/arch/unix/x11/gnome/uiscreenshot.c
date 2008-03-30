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

#include "log.h"
#include "gfxoutput.h"
#include "lib.h"
#include "screenshot.h"
#include "resources.h"
#include "ui.h"
#include "uiarch.h"
#include "uiscreenshot.h"
#ifdef HAVE_FFMPEG
#include "gfxoutputdrv/ffmpegdrv.h"
#endif


extern GtkWidget *video_ctrl_checkbox;
static GtkWidget *screenshot_dialog, *fileentry;
#ifdef HAVE_FFMPEG
static GtkWidget *ffmpg_opts, *ffmpg_audio, *ffmpg_video;
static GtkWidget *ffmpeg_omenu, *acomenu, *vcomenu;
static char *selected_driver;
static int selected_ac, selected_vc;
static void ffmpeg_details (GtkWidget *w, gpointer data);
#endif

typedef struct 
{
    const char *driver;
    GtkWidget *w;
} img_type_buttons;

static img_type_buttons *buttons = NULL;

#ifdef HAVE_FFMPEG

static void 
ffmpg_widget (GtkWidget *w, gpointer data)
{
    int num_buttons, i;
    GSList *l;
    GtkWidget *mi;
    char *current_driver;

    resources_get_value("FFMPEGFormat", 
			(void *)&current_driver);

    num_buttons = gfxoutput_num_drivers();
    for (i = 0; i < num_buttons; i++)
	if (((GtkRadioMenuItem *)(buttons[i].w))->check_menu_item.active)
	    if (strcmp(buttons[i].driver, "FFMPEG") == 0) 
	    {
		gtk_widget_set_sensitive(ffmpg_opts, TRUE);
		l = gtk_radio_menu_item_group(((GtkRadioMenuItem *)(GTK_OPTION_MENU(ffmpeg_omenu)->menu_item)));
		while (l && (mi = GTK_WIDGET(l->data)))
		{
		    char *driver = (char *) gtk_object_get_data(GTK_OBJECT(mi),
								"driver");
		    if (strcmp(driver, current_driver) == 0)
		    {
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(mi), TRUE);
			gtk_option_menu_set_history(GTK_OPTION_MENU(ffmpeg_omenu),
						    (int) gtk_object_get_data(GTK_OBJECT(mi), "ffmpeg_index"));
			ffmpeg_details(mi, 0);
		    }
		    else
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(mi), FALSE);
		    l = l->next;
		}
		return;
	    }
    gtk_widget_set_sensitive(ffmpg_opts, FALSE);
}

static void 
ffmpeg_details (GtkWidget *w, gpointer data)
{
    int current_ac_id, current_vc_id;
    GSList *l; 
    GtkWidget *mi, *acmenu, *vcmenu;

    resources_get_value("FFMPEGAudioCodec", 
			(void *)&current_ac_id);
    resources_get_value("FFMPEGVideoCodec", 
			(void *)&current_vc_id);

    selected_driver = (char *)gtk_object_get_data(GTK_OBJECT(w), "driver");

    acmenu = (GtkWidget *) gtk_object_get_data(GTK_OBJECT(w), "acmenu");
    vcmenu = (GtkWidget *) gtk_object_get_data(GTK_OBJECT(w), "vcmenu");
    
    if (acmenu)
    {
	gtk_option_menu_set_menu(GTK_OPTION_MENU (acomenu), acmenu);
	l = gtk_radio_menu_item_group(((GtkRadioMenuItem *)(GTK_OPTION_MENU(acomenu)->menu_item)));
	while (l && (mi = GTK_WIDGET(l->data)))
	{
	    int id = * ((int *) gtk_object_get_data(GTK_OBJECT(mi), "ac_id"));
		
	    if (id == current_ac_id)
	    {
		gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(mi), TRUE);
		gtk_option_menu_set_history(GTK_OPTION_MENU(acomenu),
					    (int) gtk_object_get_data(GTK_OBJECT(mi), "ac_index"));
	    }
	    else
		gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(mi), FALSE);
	    gtk_widget_set_sensitive(GTK_WIDGET(mi), TRUE);
	    l = l->next;
	}
	gtk_widget_set_sensitive(GTK_WIDGET(acomenu), TRUE);
    }
    else
	gtk_widget_set_sensitive(GTK_WIDGET(acomenu), FALSE);
	
    if (vcmenu)
    {
	gtk_option_menu_set_menu(GTK_OPTION_MENU (vcomenu), vcmenu);
	l = gtk_radio_menu_item_group(((GtkRadioMenuItem *)(GTK_OPTION_MENU(vcomenu)->menu_item)));
	while (l && (mi = GTK_WIDGET(l->data)))
	{
	    int id = * ((int *) gtk_object_get_data(GTK_OBJECT(mi), "vc_id"));
		
	    if (id == current_vc_id)
	    {
		gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(mi), TRUE);
		gtk_option_menu_set_history(GTK_OPTION_MENU(vcomenu),
					    (int) gtk_object_get_data(GTK_OBJECT(mi), "vc_index"));
	    }
	    else
		gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(mi), FALSE);
	    gtk_widget_set_sensitive(GTK_WIDGET(mi), TRUE);
	    l = l->next;
	}
	gtk_widget_set_sensitive(GTK_WIDGET(vcomenu), TRUE);
    }
    else
	gtk_widget_set_sensitive(GTK_WIDGET(vcomenu), FALSE);
}

static void 
ffmpeg_codecs (GtkWidget *w, gpointer data)
{
    if (data)
	selected_vc = *((int *) gtk_object_get_data(GTK_OBJECT(w), "vc_id"));
    else
	selected_ac = *((int *) gtk_object_get_data(GTK_OBJECT(w), "ac_id"));
}

#endif

static GtkWidget *build_screenshot_dialog(void)
{
    GtkWidget *d, *box, *tmp, *frame, *vbox, *hbox, *omenu, *menu, *menu_item;
    int i, num_buttons;
    gfxoutputdrv_t *driver;
    GSList *group;
#ifdef HAVE_FFMPEG
    GtkWidget *l;
    GtkObject *adj;
    unsigned long v;
    ffmpegdrv_format_t *f;
    GSList *acgroup, *vcgroup;
    GtkWidget *acmenu, *vcmenu;
    int current_ac_id, current_vc_id;
    char *current_driver;
#endif

    num_buttons = gfxoutput_num_drivers();
    if (num_buttons < 1)
    {
	log_message(LOG_DEFAULT, _("No gfxoutputdrivers available"));
	return 0;
    }
    
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

    if (! buttons)
	buttons = lib_malloc(sizeof(img_type_buttons) * num_buttons);

    group = NULL;
    omenu = gtk_option_menu_new ();
    menu = gtk_menu_new ();

    driver = gfxoutput_drivers_iter_init();
    for (i = 0; i < num_buttons; i++) {
	menu_item =  gtk_radio_menu_item_new_with_label (group, driver->displayname);
	group = gtk_radio_menu_item_group (GTK_RADIO_MENU_ITEM (menu_item));
	gtk_menu_append(GTK_MENU (menu), menu_item);
	gtk_widget_show(menu_item);
 	buttons[i].driver = driver->name;
	buttons[i].w = menu_item;
#ifdef HAVE_FFMPEG
	gtk_signal_connect(GTK_OBJECT(menu_item), "activate",
			   GTK_SIGNAL_FUNC(ffmpg_widget),
			   0);
#endif
	driver = gfxoutput_drivers_iter_next();
    }
    gtk_option_menu_set_menu (GTK_OPTION_MENU (omenu), menu);
    gtk_option_menu_set_history (GTK_OPTION_MENU (omenu), 0);

    gtk_box_pack_start(GTK_BOX(vbox), omenu, FALSE, FALSE, 0);
    gtk_widget_show(omenu);

#ifdef HAVE_FFMPEG
    if (!ffmpegdrv_formatlist[0].name)
	goto no_ffmpeg;
    
    /* ffmpg options */
    ffmpg_opts = gtk_vbox_new(FALSE, 5);

    group = acgroup = vcgroup = NULL;
    ffmpeg_omenu = omenu = gtk_option_menu_new ();
    menu = gtk_menu_new ();

    for (i = 0; ffmpegdrv_formatlist[i].name != NULL; i++)
    {
	int j;
	GtkWidget *menu_item_drv;
	
	resources_get_value("FFMPEGAudioCodec", 
			    (void *)&current_ac_id);
	selected_ac = current_ac_id;
	resources_get_value("FFMPEGVideoCodec", 
			    (void *)&current_vc_id);
	selected_vc = current_vc_id;
	resources_get_value("FFMPEGFormat", 
			    (void *)&current_driver);
	selected_driver = current_driver;
	
	f = &ffmpegdrv_formatlist[i];
	
	menu_item_drv =  gtk_radio_menu_item_new_with_label (group, f->name);
	gtk_object_set_data(GTK_OBJECT(menu_item_drv), "driver", 
			    (gpointer) f->name);
	gtk_object_set_data(GTK_OBJECT(menu_item_drv), "ffmpeg_index", 
			    (gpointer) i); /* URGH, sizeof(int) <= pointer */
	gtk_signal_connect (GTK_OBJECT (menu_item_drv), "activate",
			    (GtkSignalFunc) ffmpeg_details, 0);
	
	group = gtk_radio_menu_item_group (GTK_RADIO_MENU_ITEM (menu_item_drv));
	gtk_menu_append(GTK_MENU (menu), menu_item_drv);
	gtk_widget_show(menu_item_drv);
	log_message(LOG_DEFAULT, _("FFMPEG Driver: %s"), f->name);
	acmenu = NULL;
	for (j = 0; f->audio_codecs && f->audio_codecs[j].name; j++)
	{
	    if (!acgroup)
	    {
		acomenu = gtk_option_menu_new();
		acmenu = gtk_menu_new();
	    }
	    menu_item = 
		gtk_radio_menu_item_new_with_label(acgroup, 
						   f->audio_codecs[j].name);
	    gtk_object_set_data(GTK_OBJECT(menu_item), "ac_id", 
				(gpointer) &f->audio_codecs[j].id);
	    gtk_object_set_data(GTK_OBJECT(menu_item), "ac_index", 
				(gpointer) j); /* URGH, sizeof(int) <= pointer */
	    
	    gtk_signal_connect (GTK_OBJECT (menu_item), "activate",
				(GtkSignalFunc) ffmpeg_codecs, (gpointer) 0);
	    acgroup = gtk_radio_menu_item_group (GTK_RADIO_MENU_ITEM (menu_item));
	    gtk_menu_append(GTK_MENU (acmenu), menu_item);
	    gtk_widget_show(menu_item);
	    log_message(LOG_DEFAULT, _("\tAC: %s(%d)"), 
			f->audio_codecs[j].name,
			f->audio_codecs[j].id);
	}
	
	vcmenu = NULL;
	for (j = 0; f->video_codecs && f->video_codecs[j].name; j++)
	{
	    if (!vcgroup)
	    {
		vcomenu = gtk_option_menu_new();
		vcmenu = gtk_menu_new();
	    }
	    menu_item = 
		gtk_radio_menu_item_new_with_label(vcgroup, 
						   f->video_codecs[j].name);
	    gtk_object_set_data(GTK_OBJECT(menu_item), "vc_id", 
				(gpointer) &f->video_codecs[j].id);
	    gtk_object_set_data(GTK_OBJECT(menu_item), "vc_index", 
				(gpointer) j); /* URGH, sizeof(int) <= pointer */
	    gtk_signal_connect (GTK_OBJECT (menu_item), "activate",
				(GtkSignalFunc) ffmpeg_codecs, (gpointer) 1);
	    vcgroup = gtk_radio_menu_item_group (GTK_RADIO_MENU_ITEM (menu_item));
	    gtk_menu_append(GTK_MENU (vcmenu), menu_item);
	    gtk_widget_show(menu_item);
	    log_message(LOG_DEFAULT, _("\tVC: %s(%d)"), 
			f->video_codecs[j].name,
			f->video_codecs[j].id);
	}
	gtk_object_set_data(GTK_OBJECT(menu_item_drv), "acmenu", (gpointer) acmenu);
	gtk_object_set_data(GTK_OBJECT(menu_item_drv), "vcmenu", (gpointer) vcmenu);
    }
    gtk_option_menu_set_menu (GTK_OPTION_MENU (omenu), menu);
    gtk_option_menu_set_history (GTK_OPTION_MENU (omenu), 0);

    hbox = gtk_hbox_new(0, FALSE);
    tmp = gtk_label_new(_("Format"));
    gtk_box_pack_start(GTK_BOX(hbox), tmp, FALSE, FALSE, 0);
    gtk_widget_show(tmp);
    gtk_box_pack_start(GTK_BOX(hbox), omenu, FALSE, FALSE, 0);
    gtk_widget_show(omenu);
    gtk_box_pack_start(GTK_BOX(ffmpg_opts), hbox, FALSE, FALSE, 0);
    gtk_widget_show(hbox);

    hbox = gtk_hbox_new(0, FALSE);
    tmp = gtk_label_new(_("Audio Codec"));
    gtk_box_pack_start(GTK_BOX(hbox), tmp, FALSE, FALSE, 0);
    gtk_widget_show(tmp);
    gtk_box_pack_start(GTK_BOX(hbox), acomenu, FALSE, FALSE, 0);
    gtk_widget_show(acomenu);
    gtk_box_pack_start(GTK_BOX(ffmpg_opts), hbox, FALSE, FALSE, 0);
    gtk_widget_show(hbox);

    hbox = gtk_hbox_new(0, FALSE);
    tmp = gtk_label_new(_("Video Codec"));
    gtk_box_pack_start(GTK_BOX(hbox), tmp, FALSE, FALSE, 0);
    gtk_widget_show(tmp);
    gtk_box_pack_start(GTK_BOX(hbox), vcomenu, FALSE, FALSE, 0);
    gtk_widget_show(vcomenu);
    gtk_box_pack_start(GTK_BOX(ffmpg_opts), hbox, FALSE, FALSE, 0);
    gtk_widget_show(hbox);

    resources_get_value("FFMPEGAudioBitrate", (void *) &v);
    adj = gtk_adjustment_new ((gfloat) v, 
			      (gfloat) 16000, 
			      (gfloat) 128000,
			      (gfloat) 1000,
			      (gfloat) 10000,
			      (gfloat) 10000);
    ffmpg_audio = gtk_spin_button_new(GTK_ADJUSTMENT(adj), (gfloat) 1000, 0);
    gtk_widget_set_usize(ffmpg_audio, 100, 16);
    l = gtk_label_new(_("Audio Bitrate"));
    tmp = gtk_hbox_new(FALSE, 5);
    gtk_box_pack_start(GTK_BOX(tmp), l, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(tmp), ffmpg_audio, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(ffmpg_opts), tmp, FALSE, FALSE, 0);
    
    resources_get_value("FFMPEGVideoBitrate", (void *) &v);
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
  no_ffmpeg:
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
	if (screenshot_dialog)
	    gtk_signal_connect(GTK_OBJECT(screenshot_dialog),
			       "destroy",
			       GTK_SIGNAL_FUNC(gtk_widget_destroyed),
			       &screenshot_dialog);
	else
	    return -1;
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
	if (GTK_RADIO_MENU_ITEM(buttons[i].w)->check_menu_item.active)
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
	
	resources_set_value("FFMPEGFormat", (resource_value_t) selected_driver);
	resources_set_value("FFMPEGAudioCodec", (resource_value_t) selected_ac);
	resources_set_value("FFMPEGVideoCodec", (resource_value_t) selected_vc);
	log_message(LOG_DEFAULT, "FFMPEG: Driver: %s, ac: %d, vc: %d\n",
		    selected_driver, selected_ac, selected_vc);
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
    lib_free(buttons);
    gtk_widget_destroy(screenshot_dialog);
    screenshot_dialog = NULL;
}

