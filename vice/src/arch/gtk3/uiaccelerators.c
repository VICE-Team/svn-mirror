/**
 * \brief   Custom keyboard shortcuts
 *
 * \file    uiaccelerators.c
 *
 * XXX:     For some reason this doesn't work in VICE, Alt+r just adds an 'r'
 *          to the emulated machine's screen. This worked standalone, so I
 *          suspect VICE not using the normal Gtk main loop and scanning keys
 *          for the keyboard input screws with this code.
 *
 *          So as it stands right now, the below code is a non-working example
 *          of how we could implement custom shortcuts in Gtk3.
 *
 * Written by
 *  Bas Wassink <b.wassink@ziggo.nl>
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
#include <string.h>
#include <gtk/gtk.h>

#include "debug_gtk3.h"
#include "not_implemented.h"

#include "uiaccelerators.h"


/** \brief  Handler for Alt+r (for now)
 *
 * \param[in]   widget      widget that triggered the event (unused)
 * \param[in]   user_data   data related to the event (unused
 */
static void reset_handler(GtkWidget *widget, gpointer user_data)
{
    /* obviously should actually trigger a reset */
    g_print("Reset!\n");
}


/** \brief  A group of keyboard shutcuts, Gtk style
 *
 * If we want to connect different shortcuts to different windows (ie VICII,
 * VDC, Monitor), this won't do at all. Also, do I need to clean this up after
 * use?
 */
static GtkAccelGroup *group;


/** \brief  A Glib 'closure' object used to wrap around the `reset_handler`
 *
 * Though C doesn't support closures at all, GLib seems to emulate these with
 * a lot of magic. I have no idea if I need to clean this up or if Glib handles
 * this. So more research is definitely needed.
 */
static GClosure *reset_closure;


/** \brief  Add keyboard shortcuts to \a window
 *
 * This function should be expanded to read custom keyboard shortcuts from
 * a config file and add those. Right now it's a proof of concept that doesn't
 * even work, probably due to the keyboard handling somewhere else.
 * (It worked standalone)
 *
 * \param[in,out]   window  Gtk window to add keyboard shortcuts to
 */
void add_accelerators_to_window(GtkWidget *window)
{
    g_print("ADDING ACCELERATORS!\n");

   /* create a new accelerator group */
    group = gtk_accel_group_new();

    /*
     * Ideally here would be a loop adding any custom keyboard shortcut from
     * a config file, building closures and registering keys as we go along
     */

    /* create closure from the `reset_handler` function */
    reset_closure = g_cclosure_new(G_CALLBACK(reset_handler), NULL, NULL);

    /* add Alt+r to 'reset' the emu */
    gtk_accel_group_connect(group, GDK_KEY_r, GDK_MOD1_MASK, /* Alt */
            GTK_ACCEL_MASK, /* FIXME: not sure about this one, worked somehow */
            reset_closure);

    /* end loop */

    /* add finally add the custom shortcut(s) to the Gtk window */
    gtk_window_add_accel_group(GTK_WINDOW(window), group);
}

