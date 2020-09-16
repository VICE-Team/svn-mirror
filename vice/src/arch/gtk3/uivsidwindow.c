/** \file   uivsidwindow.c
 * \brief   Native GTK3 main vsid window code
 *
 * \author  Marcus Sutton <loggedoubt@gmail.com>
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 */

/*
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

/* XXX: It should be possible to compile, link and run all emulators while this
 *      entire file (amongst others) is contained inside an #if 0 wrapper.
 *
 * Well, it doesn't, removing. --compyx
 */

#include "vice.h"

#include <gtk/gtk.h>

#include "videoarch.h"

#include "vice_gtk3.h"
#include "hvscstilwidget.h"
#include "machine.h"
#include "psid.h"
#include "ui.h"
#include "uiapi.h"
#include "uivsidmenu.h"
#include "vsidmainwidget.h"
#include "vsidtuneinfowidget.h"
#include "vsync.h"

#include "uivsidwindow.h"


/** \brief  Main widget of VSID
 */
static GtkWidget *main_widget = NULL;

static gulong frame_clock_update_handler;

/** \brief  Called once per frame for frequent asynchronous UI updates.
 * 
 * For example, vsid.c provides an updated play time from the VICE thread.
 * It's not safe for the VICE thread to call GTK code - so instead we just
 * store the value and update the UI each frame in here.
 */
static void on_frame_clock_update(GdkFrameClock *clock, gpointer user_data)
{
    vsid_main_widget_update();
}

static void on_widget_realized(GtkWidget *widget, gpointer data)
{
    GdkFrameClock *frame_clock;

    /* Use a GTK frame clock to periodically update the ui */
    frame_clock = gdk_window_get_frame_clock(gtk_widget_get_window(widget));
    frame_clock_update_handler = g_signal_connect_unlocked(frame_clock, "update", G_CALLBACK(on_frame_clock_update), NULL);
    gdk_frame_clock_begin_updating(frame_clock);
}

static void on_widget_unrealized(GtkWidget *widget, gpointer data)
{
    GdkFrameClock *frame_clock;

    frame_clock = gdk_window_get_frame_clock(gtk_widget_get_window(widget));
    g_signal_handler_disconnect(frame_clock, frame_clock_update_handler);
    gdk_frame_clock_end_updating(frame_clock);
}

/** \brief  Create  VSID window
 *
 * \param[in]   canvas  something
 */
static void vsid_window_create(video_canvas_t *canvas)
{
    GtkWidget *menu_bar;

    canvas->renderer_backend = NULL;
    canvas->event_box = NULL;

    main_widget = vsid_main_widget_create();
    gtk_widget_set_size_request(main_widget, 400, 300);
    gtk_widget_set_hexpand(main_widget, TRUE);
    gtk_widget_set_vexpand(main_widget, TRUE);
    gtk_widget_show(main_widget);

    menu_bar = ui_vsid_menu_bar_create();
    gtk_container_add(GTK_CONTAINER(canvas->grid), menu_bar);
    gtk_container_add(GTK_CONTAINER(canvas->grid), main_widget);

    /* Set up the frame clock when the top level grid is realized. */
    g_signal_connect (canvas->grid, "realize", G_CALLBACK (on_widget_realized), NULL);
    g_signal_connect (canvas->grid, "unrealize", G_CALLBACK (on_widget_unrealized), NULL);
}


/** \brief  Load and play PSID/SID file \a filename
 *
 * \param[in]   filename    file to play
 *
 * \return  0 on success, -1 on failure
 */
int ui_vsid_window_load_psid(const char *filename)
{
    vsync_suspend_speed_eval();

    if (machine_autodetect_psid(filename) < 0) {
        debug_gtk3("'%s' is not a valid PSID file.", filename);
        ui_error("'%s' is not a valid PSID file.", filename);
        return -1;
    }
    psid_init_driver();
    machine_play_psid(0);
    machine_trigger_reset(MACHINE_RESET_MODE_SOFT);
    vsid_tune_info_widget_set_song_lengths(filename);
    hvsc_stil_widget_set_psid(filename);
    ui_pause_disable();

    return 0;
}


/** \brief  Initialize VSID window
 */
void ui_vsid_window_init(void)
{
    ui_set_create_window_func(vsid_window_create);

    ui_set_handle_dropped_files_func(ui_vsid_window_load_psid);
}
