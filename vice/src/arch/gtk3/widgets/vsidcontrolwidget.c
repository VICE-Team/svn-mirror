/** \file   vsidcontrolwidget.c
 * \brief   GTK3 control widget for VSID
 *
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


#include "vice.h"

#include <stdbool.h>
#include <gtk/gtk.h>

#include "vice_gtk3.h"
#include "debug.h"
#include "machine.h"
#include "lib.h"
#include "log.h"
#include "resources.h"
#include "psid.h"
#include "uicommands.h"

#include "uisidattach.h"

#include "vsidcontrolwidget.h"


/** \brief  Object containing icon and callback
 */
typedef struct vsid_ctrl_button_s {
    const char *icon_name;                      /**< icon name */
    void (*callback)(GtkWidget *, gpointer);    /**< callback */
} vsid_ctrl_button_t;


static int tune_count;
static int tune_current;
static int tune_default;


/** \brief  Temporary callback for the media buttons
 *
 * \param[in]   widget  widget
 * \param[in]   data    icon name
 */
static void fake_callback(GtkWidget *widget, gpointer data)
{
    debug_gtk3("got callback for '%s;\n", (const char *)data);
}



static void next_tune_callback(GtkWidget *widget, gpointer data)
{
    if (tune_current >= tune_count) {
        tune_current = 1;
    } else {
        tune_current++;
    }
    psid_init_driver();
    machine_play_psid(tune_current);
    machine_trigger_reset(MACHINE_RESET_MODE_SOFT);
}


static void prev_tune_callback(GtkWidget *widget, gpointer data)
{
    if (tune_current == 1) {
        tune_current = tune_count;
    } else {
        tune_current--;
    }
    psid_init_driver();
    machine_play_psid(tune_current);
    machine_trigger_reset(MACHINE_RESET_MODE_SOFT);
}



static void ffwd_callback(GtkWidget *widget, gpointer data)
{
    ui_toggle_resource(NULL, "WarpMode");
}




/** \brief  List of media control buttons
 */
static const vsid_ctrl_button_t buttons[] = {
    { "media-skip-backward", prev_tune_callback },  /* select prev tune */
    { "media-playback-start", fake_callback },  /* probably switch play/stop,
                                                   not two separate functions */
    { "media-playback-stop", fake_callback },
    { "media-seek-forward", ffwd_callback },
    { "media-skip-forward", next_tune_callback },   /* select next tune */
    { "media-eject", uisidattach_show_dialog },   /* active file-open dialog */
    { "media-record", fake_callback },  /* start recording with current settings*/
    { NULL, NULL }
};



GtkWidget *vsid_control_widget_create(void)
{
    GtkWidget *grid;
    int i;

    grid = vice_gtk3_grid_new_spaced(0, VICE_GTK3_DEFAULT);

    for (i = 0; buttons[i].icon_name != NULL; i++) {
        GtkWidget *button;

        button = gtk_button_new_from_icon_name(buttons[i].icon_name,
                GTK_ICON_SIZE_DIALOG);
        gtk_grid_attach(GTK_GRID(grid), button, i, 0, 1,1);
        if (buttons[i].callback != NULL) {
            g_signal_connect(button, "clicked",
                    G_CALLBACK(buttons[i].callback),
                    (gpointer)(buttons[i].icon_name));
        }
    }

    gtk_widget_show_all(grid);
    return grid;
}


void vsid_control_widget_set_tune_count(int n)
{
    tune_count = n;
}


void vsid_control_widget_set_tune_current(int n)
{
    tune_current = n;
}


void vsid_control_widget_set_tune_default(int n)
{
    tune_default = n;
}
