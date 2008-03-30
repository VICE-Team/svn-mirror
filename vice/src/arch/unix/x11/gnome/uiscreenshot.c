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
#include "util.h"
#include "uifileentry.h"
#ifdef HAVE_FFMPEG
#include "gfxoutputdrv/ffmpegdrv.h"
#endif


extern GtkWidget *video_ctrl_checkbox;
static GtkWidget *screenshot_dialog, *fileentry;
static GtkWidget *drv_menu;
#ifdef HAVE_FFMPEG
static GtkWidget *ffmpg_opts, *ffmpg_audio, *ffmpg_video;
static GtkWidget *ffmpeg_omenu, *acmenu, *vcmenu;
static char *selected_driver;
static int selected_driver_allocated;
static int selected_ac, selected_vc;
static void ffmpeg_details (GtkWidget *w, gpointer data);
#endif

typedef struct 
{
    const char *driver;
    const char *ext;
    GtkWidget *w;
} img_type_buttons;

static img_type_buttons *buttons = NULL;

#ifdef HAVE_FFMPEG

enum { DRV_NAME, DRV_INDEX, DRV_ACMENU, DRV_VCMENU, DRV_N };

static void 
ffmpeg_widget (GtkWidget *w, gpointer data)
{
    int i;
    char *current_driver;

    g_return_if_fail(GTK_IS_COMBO_BOX(w));
    
    resources_get_value("FFMPEGFormat", 
			(void *)&current_driver);
    
    i = gtk_combo_box_get_active(GTK_COMBO_BOX(w));
    if (strcmp(buttons[i].driver, "FFMPEG") == 0) 
    {
	GtkListStore *store;
	GtkTreeIter iter;
	int index, found;
	char *drv_name = NULL;
	
	store = GTK_LIST_STORE(
	    gtk_combo_box_get_model(GTK_COMBO_BOX(ffmpeg_omenu)));
	if (gtk_tree_model_get_iter_first(GTK_TREE_MODEL(store), &iter))
	{
	    do 
	    {
		gtk_tree_model_get(GTK_TREE_MODEL(store), &iter, 
				   0, &drv_name, 
				   1, &index, -1);
		found = !strcmp(drv_name, current_driver);
		lib_free(drv_name);
	    } while (!found && 
		     gtk_tree_model_iter_next(GTK_TREE_MODEL(store), &iter));
	    if (found)
	    {
		selected_driver = current_driver;
		gtk_combo_box_set_active(GTK_COMBO_BOX(ffmpeg_omenu), index);
	    }
	    gtk_widget_set_sensitive(ffmpg_opts, TRUE);
	    return;
	}
    }
    gtk_widget_set_sensitive(ffmpg_opts, FALSE);
}

static void 
ffmpeg_details (GtkWidget *w, gpointer data)
{
    int current_ac_id, current_vc_id;
    char *current_driver;
    GtkTreeIter iter;
    GtkListStore *ac_store, *vc_store;

    g_return_if_fail(GTK_IS_COMBO_BOX(w));

    resources_get_value("FFMPEGAudioCodec", 
			(void *)&current_ac_id);
    resources_get_value("FFMPEGVideoCodec", 
			(void *)&current_vc_id);
    resources_get_value("FFMPEGFormat", 
			(void *)&current_driver);

    if (gtk_combo_box_get_active_iter(GTK_COMBO_BOX(w), &iter) == FALSE)
    {
	log_error(LOG_DEFAULT, "internal error. iter in combo box not found.");
	return;
    }

    if (selected_driver && selected_driver_allocated)
	lib_free(selected_driver); 
    
    gtk_tree_model_get(
	GTK_TREE_MODEL(gtk_combo_box_get_model(GTK_COMBO_BOX(w))), &iter, 
	DRV_NAME, &selected_driver, 
	DRV_ACMENU, &ac_store, 
	DRV_VCMENU, &vc_store, -1);
    selected_driver_allocated = 1;
    
    if (ac_store)
    {
	GtkListStore *store;
	GtkTreeIter iter;
	int index, id, found;

	store = ac_store;
	if (gtk_tree_model_get_iter_first(GTK_TREE_MODEL(store), &iter))
	{
	    do 
	    {
		gtk_tree_model_get(GTK_TREE_MODEL(store), &iter, 
				   1, &id, 
				   2, &index, -1);
		found = (id == current_ac_id);
	    } while (!found && 
		     gtk_tree_model_iter_next(GTK_TREE_MODEL(store), &iter));
	    gtk_combo_box_set_model(GTK_COMBO_BOX(acmenu), 
				GTK_TREE_MODEL(ac_store));
	    if (found)
	    {
		gtk_widget_set_sensitive(GTK_WIDGET(acmenu), TRUE);
		gtk_combo_box_set_active(GTK_COMBO_BOX(acmenu), index);
		selected_ac = id;
	    }
	    else
		gtk_widget_set_sensitive(GTK_WIDGET(acmenu), FALSE);
	}
	else
	    gtk_widget_set_sensitive(GTK_WIDGET(acmenu), FALSE);
    }
    else
	gtk_widget_set_sensitive(GTK_WIDGET(acmenu), FALSE);
    
    if (vc_store)
    {
	GtkListStore *store;
	GtkTreeIter iter;
	int index, id, found;

	store = vc_store;
	if (gtk_tree_model_get_iter_first(GTK_TREE_MODEL(store), &iter))
	{
	    do 
	    {
		gtk_tree_model_get(GTK_TREE_MODEL(store), &iter, 
				   1, &id, 
				   2, &index, -1);
		found = (id == current_vc_id);
	    } while (!found && 
		     gtk_tree_model_iter_next(GTK_TREE_MODEL(store), &iter));
	    gtk_combo_box_set_model(GTK_COMBO_BOX(vcmenu), 
				    GTK_TREE_MODEL(vc_store));
	    if (found)
	    {
		gtk_widget_set_sensitive(GTK_WIDGET(vcmenu), TRUE);
		gtk_combo_box_set_active(GTK_COMBO_BOX(vcmenu), index);
		selected_vc = id;
	    }
	    else
		gtk_widget_set_sensitive(GTK_WIDGET(vcmenu), FALSE);
	}
	else
	    gtk_widget_set_sensitive(GTK_WIDGET(vcmenu), FALSE);
    }
    else
	gtk_widget_set_sensitive(GTK_WIDGET(vcmenu), FALSE);
}

#endif

static GtkWidget *build_screenshot_dialog(void)
{
    GtkWidget *d, *box, *frame, *vbox, *omenu;
    int i, num_buttons;
    gfxoutputdrv_t *driver;
#ifdef HAVE_FFMPEG
    GtkWidget *l, *hbox, *tmp;
    GtkObject *adj;
    unsigned long v;
    ffmpegdrv_format_t *f;
    GtkListStore *drv_store;
    GtkTreeIter iter;
    GtkCellRenderer *renderer;
    GtkListStore *ac_store = NULL, *vc_store = NULL;
#endif

    num_buttons = gfxoutput_num_drivers();
    if (num_buttons < 1)
    {
	log_message(LOG_DEFAULT, _("No gfxoutputdrivers available"));
	return 0;
    }

    d = gtk_dialog_new_with_buttons(_("Save Screenshot"), 
			 NULL,
			 GTK_DIALOG_DESTROY_WITH_PARENT,
			 GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			 GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
			 NULL);
    box = gtk_hbox_new(0, FALSE);

    fileentry = vice_file_entry(_("Save Media File..."),  
				NULL, NULL, 
				GTK_FILE_CHOOSER_ACTION_SAVE);
    if (!fileentry)
    {
	gtk_widget_destroy(d);
	return 0;
    }
    gtk_dialog_set_default_response(GTK_DIALOG(d), GTK_RESPONSE_ACCEPT);

    gtk_box_pack_start(GTK_BOX(box), fileentry,
		       TRUE, TRUE, GNOME_PAD);
    gtk_widget_show(fileentry);

    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(d)->vbox), box, TRUE, TRUE, 0);
    gtk_widget_show(box);

    frame = gtk_frame_new(_("Image Format"));
    vbox = gtk_vbox_new(FALSE, 5);

    if (! buttons)
	buttons = lib_malloc(sizeof(img_type_buttons) * num_buttons);

    drv_menu = omenu = gtk_combo_box_new_text ();

    driver = gfxoutput_drivers_iter_init();
    for (i = 0; i < num_buttons; i++) {
	gtk_combo_box_append_text(GTK_COMBO_BOX(omenu), driver->displayname);
 	buttons[i].driver = driver->name;
	buttons[i].ext = driver->default_extension;
	driver = gfxoutput_drivers_iter_next();
    }
    gtk_combo_box_set_active(GTK_COMBO_BOX(omenu), 0);
    gtk_box_pack_start(GTK_BOX(vbox), omenu, FALSE, FALSE, 0);
    gtk_widget_show(omenu);

#ifdef HAVE_FFMPEG
    if (!ffmpegdrv_formatlist[0].name)
	goto no_ffmpeg;
    
    g_signal_connect(G_OBJECT(omenu), "changed",
		     G_CALLBACK(ffmpeg_widget), (gpointer) omenu);

    /* ffmpg options */
    ffmpg_opts = gtk_vbox_new(FALSE, 5);

    drv_store = gtk_list_store_new(DRV_N, G_TYPE_STRING, G_TYPE_INT, 
				   G_TYPE_OBJECT, G_TYPE_OBJECT);
    for (i = 0; ffmpegdrv_formatlist[i].name != NULL; i++)
    {
	int j;
	GtkTreeIter ac_iter, vc_iter;
	
	f = &ffmpegdrv_formatlist[i];

	gtk_list_store_append(drv_store, &iter);
	gtk_list_store_set(drv_store, &iter, DRV_NAME, f->name, 
			   DRV_INDEX, i, -1);

	log_message(LOG_DEFAULT, _("FFMPEG Driver: %s"), f->name);

	ac_store = gtk_list_store_new(3, G_TYPE_STRING, G_TYPE_INT, G_TYPE_INT);
	for (j = 0; f->audio_codecs && f->audio_codecs[j].name; j++)
	{
	    gtk_list_store_append(ac_store, &ac_iter);
	    gtk_list_store_set(ac_store, &ac_iter, 
			       0, f->audio_codecs[j].name,
			       1, f->audio_codecs[j].id, 
			       2, j, -1);
	    log_message(LOG_DEFAULT, _("\tAC: %s(%d)"), 
			f->audio_codecs[j].name,
			f->audio_codecs[j].id);
	}

	vc_store = gtk_list_store_new(3, G_TYPE_STRING, G_TYPE_INT, G_TYPE_INT);
	for (j = 0; f->video_codecs && f->video_codecs[j].name; j++)
	{
	    gtk_list_store_append(vc_store, &vc_iter);
	    gtk_list_store_set(vc_store, &vc_iter, 
			       0, f->video_codecs[j].name,
			       1, f->video_codecs[j].id, 
			       2, j, -1);
	    log_message(LOG_DEFAULT, _("\tVC: %s(%d)"), 
			f->video_codecs[j].name,
			f->video_codecs[j].id);
	}

	gtk_list_store_set(drv_store, &iter, 
			   DRV_ACMENU, ac_store, 
			   DRV_VCMENU, vc_store, -1);
    }

    ffmpeg_omenu = omenu = gtk_combo_box_new_with_model(GTK_TREE_MODEL(drv_store));    
    g_signal_connect(G_OBJECT(omenu), "changed",
		     G_CALLBACK(ffmpeg_details), (gpointer) omenu);
    renderer = gtk_cell_renderer_text_new();
    gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (omenu), renderer, TRUE);
    gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (omenu), renderer,
				    "text", DRV_NAME, NULL);

    hbox = gtk_hbox_new(0, FALSE);
    tmp = gtk_label_new(_("Format"));
    gtk_box_pack_start(GTK_BOX(hbox), tmp, FALSE, FALSE, 0);
    gtk_widget_show(tmp);
    gtk_box_pack_start(GTK_BOX(hbox), omenu, FALSE, FALSE, 0);
    gtk_widget_show(omenu);
    gtk_box_pack_start(GTK_BOX(ffmpg_opts), hbox, FALSE, FALSE, 0);
    gtk_widget_show(hbox);
    
    acmenu = gtk_combo_box_new_with_model(GTK_TREE_MODEL(ac_store));    
    renderer = gtk_cell_renderer_text_new();
    gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (acmenu), renderer, TRUE);
    gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (acmenu), renderer,
				    "text", 0, NULL);
    hbox = gtk_hbox_new(0, FALSE);
    tmp = gtk_label_new(_("Audio Codec"));
    gtk_box_pack_start(GTK_BOX(hbox), tmp, FALSE, FALSE, 0);
    gtk_widget_show(tmp);
    gtk_box_pack_start(GTK_BOX(hbox), acmenu, FALSE, FALSE, 0);
    gtk_widget_show(acmenu);
    gtk_box_pack_start(GTK_BOX(ffmpg_opts), hbox, FALSE, FALSE, 0);
    gtk_widget_show(hbox);

    vcmenu = gtk_combo_box_new_with_model(GTK_TREE_MODEL(vc_store));    
    renderer = gtk_cell_renderer_text_new();
    gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (vcmenu), renderer, TRUE);
    gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (vcmenu), renderer,
				    "text", 0, NULL);
    hbox = gtk_hbox_new(0, FALSE);
    tmp = gtk_label_new(_("Video Codec"));
    gtk_box_pack_start(GTK_BOX(hbox), tmp, FALSE, FALSE, 0);
    gtk_widget_show(tmp);
    gtk_box_pack_start(GTK_BOX(hbox), vcmenu, FALSE, FALSE, 0);
    gtk_widget_show(vcmenu);
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
    gtk_widget_set_size_request(ffmpg_audio, 100, 16);
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
    gtk_widget_set_size_request(ffmpg_video, 100, 16);
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

    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(d)->vbox), frame, TRUE, TRUE, 0);
    gtk_widget_show(frame);
    gtk_widget_show(d);

    return d;
}

int ui_screenshot_dialog(char *name, struct video_canvas_s *wid)
{
    int res, i;
    char *fn, *tmp;
    const char *driver, *ext;

    if (screenshot_dialog) {
    	gdk_window_show(screenshot_dialog->window);
	gdk_window_raise(screenshot_dialog->window);
	gtk_widget_show(screenshot_dialog);
    } else {
	screenshot_dialog = build_screenshot_dialog();
	if (screenshot_dialog)
	    g_signal_connect(G_OBJECT(screenshot_dialog),
			     "destroy",
			     G_CALLBACK(gtk_widget_destroyed),
			     &screenshot_dialog);
	else
	    return -1;
    }

    ui_popup(screenshot_dialog, _("Save Screenshot"), FALSE);
    res = gtk_dialog_run(GTK_DIALOG(screenshot_dialog));
    ui_popdown(screenshot_dialog);
    
    if (res != GTK_RESPONSE_ACCEPT)
	return -1;
    
    fn = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(fileentry));
    if (!fn) {
	ui_error(_("Invalid filename"));
	return -1;
    }

    i = gtk_combo_box_get_active(GTK_COMBO_BOX(drv_menu));
    if (i < 0)
	return -1;

    driver = buttons[i].driver;
    ext = buttons[i].ext;
    
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
    tmp = lib_stralloc(fn);
    lib_free(fn);
    if (!util_get_extension(tmp))
	util_add_extension(&tmp, ext);
    if (screenshot_save(driver, tmp, wid) < 0) {
	ui_error(_("Couldn't write screenshot to `%s' with driver `%s'."), 
		 tmp, driver);
	lib_free(tmp);
	return -1;
    } else {
	if (screenshot_is_recording())
	    gtk_widget_show(video_ctrl_checkbox);
	ui_message(_("Successfully wrote `%s'"), tmp);
	lib_free(tmp);
    }

    return 0;
}

void uiscreenshot_shutdown(void)
{
    if (buttons)
	lib_free(buttons);
#ifdef HAVE_FFMPEG
    if (selected_driver_allocated)
	lib_free(selected_driver);
#endif
    if (screenshot_dialog && GTK_IS_WIDGET(screenshot_dialog))
	gtk_widget_destroy(screenshot_dialog);
    screenshot_dialog = NULL;
}

