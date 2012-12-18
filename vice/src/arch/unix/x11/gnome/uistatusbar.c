/*
 * uistatusbar.c - GTK only, Statusbar handling
 *
 * Written by
 *  Ettore Perazzoli
 *  Oliver Schaertel
 *  pottendo <pottendo@gmx.net>
 *  groepaz <groepaz@gmx.net>
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

/* #define DEBUG_X11UI */

#ifdef DEBUG_X11UI
#define DBG(_x_) log_debug _x_
#else
#define DBG(_x_)
#endif

#include "vice.h"

#include "lib.h"
#include "machine.h"
#include "ui.h"
#include "uiarch.h"
#include "uicontents.h"
#include "uidrivestatus.h"
#include "uimenu.h"
#include "uitapestatus.h"
#include "util.h"
#include "video.h"
#include "resources.h"
#include "screenshot.h"
#include "vice-event.h"
#include "videoarch.h"

/* FIXME: we want these to be static */
GtkWidget *video_ctrl_checkbox;
GtkWidget *event_rec_checkbox;
GtkWidget *event_playback_checkbox;
GtkWidget *video_ctrl_checkbox_label;
GtkWidget *event_rec_checkbox_label;
GtkWidget *event_playback_checkbox_label;

static GtkWidget *speed_menu;
static int statustext_display_time = 0;

static GtkStyle *ui_style_red;
static GtkStyle *ui_style_green;

/* ------------------------------------------------------------------------- */
static void ui_update_video_checkbox (GtkWidget *w, gpointer data);
static void ui_update_event_checkbox (GtkWidget *w, gpointer data);

static void statusbar_setstatustext(const char *t)
{
    int i;
    int num_app_shells = get_num_shells();

    for (i = 0; i < num_app_shells; i++) {
        gtk_label_set_text(app_shells[i].statustext, t);
    }
}

void ui_display_statustext(const char *text, int fade_out)
{
    statusbar_setstatustext(text);
    if (fade_out) {
        statustext_display_time = 5;
    } else {
        statustext_display_time = 0;
    }
}

/* ------------------------------------------------------------------------- */

/* Show the speed index to the user.  */
void ui_display_speed(float percent, float framerate, int warp_flag)
{
    int i;
    char str[256];
    int percent_int = (int)(percent + 0.5);
    int framerate_int = (int)(framerate + 0.5);
    int num_app_shells = get_num_shells();

    for (i = 0; i < num_app_shells; i++) {
        if (!percent) {
            gtk_label_set_text(app_shells[i].speed_label, warp_flag ? _("(warp)") : "");
        } else {
            sprintf(str, "%d%%, %d fps %s", percent_int, framerate_int, warp_flag ? _("(warp)") : "");
            gtk_label_set_text(app_shells[i].speed_label, str);
        }
    }
    if (statustext_display_time > 0) {
        statustext_display_time--;
        if (statustext_display_time == 0) {
            statusbar_setstatustext("");
        }
    }
    if (machine_class != VICE_MACHINE_VSID) {
        if (!screenshot_is_recording()) {
            ui_update_video_checkbox(video_ctrl_checkbox, NULL);
        }
    }
}

void ui_set_speedmenu(ui_menu_entry_t *menu)
{
    if (speed_menu) {
        gtk_widget_destroy(speed_menu);
    }
    speed_menu = gtk_menu_new();
    ui_menu_create(speed_menu, NULL, "SpeedMenu", menu);
}

/* ------------------------------------------------------------------------- */
void ui_display_recording(int recording_status)
{
    if (recording_status) {
        gtk_widget_show(event_rec_checkbox);
    } else {
        gtk_widget_hide(event_rec_checkbox);
    }
}

void ui_display_playback(int playback_status, char *version)
{
    if (playback_status) {
        gtk_widget_show(event_playback_checkbox);
    } else {
        gtk_widget_hide(event_playback_checkbox);
    }
}

/* called from ui_open_canvas_window */
void ui_init_checkbox_style(void)
{
    ui_style_red = gtk_style_new();
    ui_style_red->fg[GTK_STATE_NORMAL] = drive_led_on_red_pixel;
    ui_style_red->fg[GTK_STATE_ACTIVE] = drive_led_on_red_pixel;
    ui_style_red->fg[GTK_STATE_SELECTED] = drive_led_on_red_pixel;
    ui_style_red->fg[GTK_STATE_PRELIGHT] = drive_led_on_red_pixel;
    gtk_widget_set_style(video_ctrl_checkbox_label, ui_style_red);
    gtk_widget_set_style(event_rec_checkbox_label, ui_style_red);

    ui_style_green = gtk_style_new();
    ui_style_green->fg[GTK_STATE_NORMAL] = drive_led_on_green_pixel;
    ui_style_green->fg[GTK_STATE_ACTIVE] = drive_led_on_green_pixel;
    ui_style_green->fg[GTK_STATE_SELECTED] = drive_led_on_green_pixel;
    ui_style_green->fg[GTK_STATE_PRELIGHT] = drive_led_on_green_pixel;
    gtk_widget_set_style(event_playback_checkbox_label, ui_style_green);
}

/* Display a message in the title bar indicating that the emulation is
   paused.  */
void ui_display_paused(int flag)
{
    int i;
    char str[1024];
    int num_app_shells = get_num_shells();

    for (i = 0; i < num_app_shells; i++) {
        if (flag) {
            sprintf(str, _("%s (paused)"), app_shells[i].title);
            gtk_window_set_title(GTK_WINDOW(app_shells[i].shell), str);
        } else {
            gtk_window_set_title(GTK_WINDOW(app_shells[i].shell), app_shells[i].title);
        }
    }
}


static void ui_update_pal_checkbox (GtkWidget *w, gpointer data)
{
    app_shell_type *appshell = ((app_shell_type*) data);

    if (!w || !GTK_IS_TOGGLE_BUTTON(w)) {
        return;
    }

    if (GTK_TOGGLE_BUTTON(w)->active) {
        gtk_widget_show(appshell->pal_ctrl);
    } else {
        gtk_widget_hide(appshell->pal_ctrl);
    }

}

static void ui_update_video_checkbox (GtkWidget *w, gpointer data)
{
    gtk_widget_hide(video_ctrl_checkbox);
    screenshot_stop_recording();
}

static void ui_update_event_checkbox (GtkWidget *w, gpointer data)
{
    if (data) {
        event_playback_stop();
    } else {
        event_record_stop();
    }
}

static gboolean speed_popup_cb(GtkWidget *w, GdkEvent *event, gpointer data)
{
    if (event->type == GDK_BUTTON_PRESS) {
        GdkEventButton *bevent = (GdkEventButton*) event;
        
        if (speed_menu) {
            ui_menu_update_all_GTK();
            gtk_menu_popup(GTK_MENU(speed_menu), NULL, NULL, NULL, NULL, bevent->button, bevent->time);
        }
    }
    return 0;
}

GtkWidget *ui_create_status_bar(GtkWidget *pane)
{
    /* Create the status bar on the bottom.  */
    GdkWindow *window = get_active_toplevel()->window;
    GtkWidget *speed_label, *drive_box, *frame, *event_box, *pcb, *vcb, *tmp, *pal_ctrl_checkbox, *status_bar;
    int i;
    app_shell_type *as;
    char *empty = util_concat("<", _("empty"), ">", NULL);
    int num_app_shells = get_num_shells();

    status_bar = gtk_hbox_new(FALSE, 0);

    gtk_box_pack_end(GTK_BOX(pane), status_bar, FALSE, FALSE, 0);
    gtk_widget_show(status_bar);

    event_box = gtk_event_box_new();
    gtk_box_pack_start(GTK_BOX(status_bar), event_box, TRUE, TRUE,0);
    gtk_widget_show(event_box);
    g_signal_connect(G_OBJECT(event_box), "button-press-event", G_CALLBACK(speed_popup_cb), (gpointer)NULL);

    frame = gtk_frame_new(NULL);
    gtk_frame_set_shadow_type (GTK_FRAME(frame), GTK_SHADOW_IN);
    gtk_container_add(GTK_CONTAINER(event_box), frame);
    gtk_widget_show(frame);

    speed_label = gtk_label_new("");
    app_shells[num_app_shells - 1].speed_label = (GtkLabel*)speed_label;
    gtk_misc_set_alignment (GTK_MISC(speed_label), 0, -1);
    gtk_container_add(GTK_CONTAINER(frame), speed_label);
    gtk_widget_show(speed_label);

    /* spacer */
    frame = gtk_frame_new(NULL);
    gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_IN);

    tmp = gtk_label_new("");
    app_shells[num_app_shells - 1].statustext = (GtkLabel*)tmp;
    gtk_misc_set_alignment(GTK_MISC (tmp), 0, -1);
    gtk_container_add(GTK_CONTAINER(frame), tmp);
    gtk_widget_show(tmp);
    gtk_box_pack_start(GTK_BOX(status_bar), frame, TRUE, TRUE,0);
    gtk_widget_show(frame);

    /* additional controls */
    if (machine_class != VICE_MACHINE_VSID) {
        as = &app_shells[num_app_shells - 1];

        /* PAL Control checkbox */
        pal_ctrl_checkbox = gtk_frame_new(NULL);
        gtk_frame_set_shadow_type(GTK_FRAME(pal_ctrl_checkbox), GTK_SHADOW_IN);
        pcb = gtk_check_button_new_with_label(_("CRT Controls"));
        GTK_WIDGET_UNSET_FLAGS(pcb, GTK_CAN_FOCUS);
        g_signal_connect(G_OBJECT(pcb), "toggled", G_CALLBACK(ui_update_pal_checkbox), as);
        gtk_container_add(GTK_CONTAINER(pal_ctrl_checkbox), pcb);
        gtk_widget_show(pcb);
        gtk_box_pack_start(GTK_BOX(status_bar), pal_ctrl_checkbox, FALSE, FALSE, 0);
        gtk_widget_show(pal_ctrl_checkbox);

        /* Video Control checkbox */
        video_ctrl_checkbox = gtk_frame_new(NULL);
        gtk_frame_set_shadow_type(GTK_FRAME(video_ctrl_checkbox), GTK_SHADOW_IN);

        video_ctrl_checkbox_label = gtk_label_new(_("audio/video recording"));
        vcb = gtk_button_new();
        gtk_container_add(GTK_CONTAINER(vcb), video_ctrl_checkbox_label);
        gtk_widget_show(video_ctrl_checkbox_label);
        GTK_WIDGET_UNSET_FLAGS(pcb, GTK_CAN_FOCUS);
        g_signal_connect(G_OBJECT(vcb), "clicked", G_CALLBACK(ui_update_video_checkbox), vcb);
        gtk_container_add(GTK_CONTAINER(video_ctrl_checkbox), vcb);
        gtk_widget_show(vcb);
        gtk_box_pack_start(GTK_BOX(status_bar), video_ctrl_checkbox, FALSE, FALSE, 0);
        gtk_widget_set_tooltip_text(GTK_WIDGET(vcb), _("click to stop recording"));

        /* Event record control checkbox */
        event_rec_checkbox = gtk_frame_new(NULL);
        gtk_frame_set_shadow_type(GTK_FRAME(event_rec_checkbox), GTK_SHADOW_IN);

        event_rec_checkbox_label = gtk_label_new(_("event recording"));
        vcb = gtk_button_new();
        gtk_container_add(GTK_CONTAINER(vcb), event_rec_checkbox_label);
        gtk_widget_show(event_rec_checkbox_label);
        GTK_WIDGET_UNSET_FLAGS(pcb, GTK_CAN_FOCUS);
        g_signal_connect(G_OBJECT(vcb), "clicked", G_CALLBACK(ui_update_event_checkbox), (gpointer)0);
        gtk_container_add(GTK_CONTAINER(event_rec_checkbox), vcb);
        gtk_widget_show(vcb);
        gtk_box_pack_start(GTK_BOX(status_bar), event_rec_checkbox, FALSE, FALSE, 0);
        gtk_widget_set_tooltip_text(GTK_WIDGET(vcb), _("click to stop recording"));

        /* Event playback control checkbox */
        event_playback_checkbox = gtk_frame_new(NULL);
        gtk_frame_set_shadow_type(GTK_FRAME(event_playback_checkbox), GTK_SHADOW_IN);

        event_playback_checkbox_label = gtk_label_new(_("event playback"));
        vcb = gtk_button_new();
        gtk_container_add(GTK_CONTAINER(vcb), event_playback_checkbox_label);
        gtk_widget_show(event_playback_checkbox_label);
        GTK_WIDGET_UNSET_FLAGS(pcb, GTK_CAN_FOCUS);
        g_signal_connect(G_OBJECT(vcb), "clicked", G_CALLBACK(ui_update_event_checkbox), (gpointer)1);
        gtk_container_add(GTK_CONTAINER(event_playback_checkbox), vcb);
        gtk_widget_show(vcb);
        gtk_box_pack_start(GTK_BOX(status_bar), event_playback_checkbox, FALSE, FALSE, 0);
        gtk_widget_set_tooltip_text(GTK_WIDGET(vcb), _("click to stop playback"));

        drive_box = build_drive_status_widget(as, window);
        gtk_widget_show(drive_box);
        gtk_box_pack_start(GTK_BOX(status_bar), drive_box, FALSE, FALSE, 0);

        build_tape_status_widget(as, window);

        gtk_box_pack_start(GTK_BOX(status_bar), as->tape_status.event_box, FALSE, FALSE, 0);
        gtk_widget_show(as->tape_status.event_box);
        gtk_widget_show(status_bar);

        for (i = 0; i < NUM_DRIVES; i++) {
#if 0
            int ih, iw;

            gdk_window_get_size_request(((GtkWidget *)as->drive_status[i].image)->window, &iw, &ih);
            gtk_widget_set_size_request(as->drive_status[i].image, width / 3, ih);
#endif
            gtk_widget_hide(as->drive_status[i].event_box);     /* Hide Drive widget */
            gdk_window_set_cursor(as->drive_status[i].event_box->window, gdk_cursor_new (GDK_HAND1)); 
        }
#if 0
        gtk_widget_hide(as->tape_status.event_box);     /* Hide Tape widget */
        gdk_window_set_cursor(as->tape_status.event_box->window, gdk_cursor_new(GDK_HAND1)); 
#endif
    }

    /* finalize event-box */
    gdk_window_set_cursor(event_box->window, gdk_cursor_new(GDK_HAND1)); 

    lib_free(empty);

    return status_bar;
}

 
