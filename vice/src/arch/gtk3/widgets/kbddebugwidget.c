/** \file   kbddebugwidget.c
 * \brief   GTK3 keyboard debugging widget
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
 */


#include "vice.h"

#include <gtk/gtk.h>
#include "vice_gtk3.h"

#include "kbddebugwidget.h"


/** \brief  Column indici of the various widgets
 */
enum {
    COL_TITLE = 0,      /**< title */
    COL_KEYVAL,         /**< raw key value */
    COL_KEYSYM,         /**< key symbol as string */
    COL_KEYMOD          /**< modifiers */
};


/** \brief  Label displaying the key value */
static GtkWidget *keyval_widget;
/** \brief  Label displaying the string representation of the key value */
static GtkWidget *keysym_widget;
/** \brief  Label displaying the various modifiers for a key */
static GtkWidget *keymod_widget;


/** \brief  Create Gtk3 keyboard debugging widget
 *
 * \return  GtkGrid
 */
GtkWidget *kbd_debug_widget_create(void)
{
    GtkWidget *grid;
    GtkWidget *label;

    grid = vice_gtk3_grid_new_spaced(VICE_GTK3_DEFAULT, VICE_GTK3_DEFAULT);
    g_object_set(grid, "margin-left", 8, NULL);

    label = gtk_label_new("KBD debug:");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label, COL_TITLE, 0, 1, 1);

    keyval_widget = gtk_label_new("-");
    gtk_grid_attach(GTK_GRID(grid), keyval_widget, COL_KEYVAL, 0 , 1, 1);

    keysym_widget = gtk_label_new("-");
    gtk_grid_attach(GTK_GRID(grid), keysym_widget, COL_KEYSYM, 0 , 1, 1);

    keymod_widget = gtk_label_new("-----");
    gtk_grid_attach(GTK_GRID(grid), keymod_widget, COL_KEYMOD, 0 , 1, 1);

    return grid;
}


/** \brief  Update keyboard debug widget with \a event
 *
 * \param[in]   event   GDK event structure
 */
void kdb_debug_widget_update(GdkEvent *event)
{
    gchar buffer[666];
    guint keyval = event->key.keyval;
    guint mods = event->key.state;

    g_snprintf(buffer, sizeof(buffer), "<tt>%u, 0x%04x</tt>", keyval, keyval);
    gtk_label_set_markup(GTK_LABEL(keyval_widget), buffer);

    g_snprintf(buffer, 666, "<tt>%s</tt>", gdk_keyval_name(keyval));
    gtk_label_set_markup(GTK_LABEL(keysym_widget), buffer);

    g_snprintf(buffer, 666, "<tt>%c%c%c %c%c%c%c%c</tt>",
            mods & GDK_SHIFT_MASK ? 'S' : '-',    /* shift (left or right) */
            mods & GDK_LOCK_MASK ? 'L' : '-',     /* shift-lock */
            mods & GDK_CONTROL_MASK ? 'C' : '-',  /* control */
                                                  /* the additional ones seem to be different on */
                                                  /* windows        vs linux                     */
            mods & GDK_MOD1_MASK ? '1' : '-',     /* alt left          alt-left                  */
            mods & GDK_MOD2_MASK ? '2' : '-',     /* alt-gr (Alt_R)    num-lock                  */
            mods & GDK_MOD3_MASK ? '3' : '-',
            mods & GDK_MOD4_MASK ? '4' : '-',     /*                   windows right             */
            mods & GDK_MOD5_MASK ? '5' : '-'      /*                   alt-gr (ISO_Level3_Shift) */
            );

    gtk_label_set_markup(GTK_LABEL(keymod_widget), buffer);


}
