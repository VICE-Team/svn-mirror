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

#include <stdlib.h>
#include <gtk/gtk.h>

#include "vice_gtk3.h"
#include "debug.h"
#include "machine.h"
#include "lib.h"
#include "log.h"

#include "uisidattach.h"

#include "vsidcontrolwidget.h"


typedef struct vsid_ctrl_button_s {
    const char *icon_name;
    void (*callback)(GtkWidget *, gpointer);
} vsid_ctrl_button_t;



static void fake_callback(GtkWidget *widget, gpointer data)
{
    debug_gtk3("got callback for '%s;\n", (const char *)data);
}


static const vsid_ctrl_button_t buttons[] = {
    { "media-skip-backward", fake_callback },  /* select prev tune */
    { "media-seek-backward", fake_callback },  /* fast rewind */
    { "media-playback-start", fake_callback },  /* probably switch play/stop,
                                                   not two separate functions */
    { "media-playback-pause", fake_callback },
    { "media-playback-stop", fake_callback },
    { "media-seek-forward", fake_callback },
    { "media-skip-forward", fake_callback },   /* select next tune */
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
                GTK_ICON_SIZE_BUTTON);
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
