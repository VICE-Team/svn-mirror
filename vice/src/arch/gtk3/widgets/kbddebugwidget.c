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

#include <string.h>

#include <gtk/gtk.h>
#include "keyboard.h"
#include "log.h"
#include "vice_gtk3.h"

#include "kbddebugwidget.h"

#define LINES   3
#define BUFSIZE 32

/** \brief  Column indexes of the various widgets
 */
enum {
    COL_TITLE = 0,      /**< title */
    COL_KEYTYPE,        /**< type of event */
    COL_KEYVAL,         /**< raw key value */
    COL_KEYSYM,         /**< key symbol as string */
    COL_KEYMOD          /**< modifiers */
};


/** \brief  Label displaying type of event (press/release) */
static GtkWidget *keytype_widget[LINES];
/** \brief  Label displaying the key value */
static GtkWidget *keyval_widget[LINES];
/** \brief  Label displaying the string representation of the key value */
static GtkWidget *keysym_widget[LINES];
/** \brief  Label displaying the various modifiers for a key */
static GtkWidget *keymod_widget[LINES];


/** \brief  Create Gtk3 keyboard debugging widget
 *
 * \return  GtkGrid
 */
GtkWidget *kbd_debug_widget_create(void)
{
    GtkWidget *grid;
    GtkWidget *label;
    int line;

    grid = vice_gtk3_grid_new_spaced(VICE_GTK3_DEFAULT, VICE_GTK3_DEFAULT);
    g_object_set(grid, "margin-left", 8, NULL);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 1);

    label = gtk_label_new("KBD debug:");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label, COL_TITLE, 0, 1, 1);

    for (line = 0; line < LINES; line++) {
        keytype_widget[line] = gtk_label_new("-");
        gtk_grid_attach(GTK_GRID(grid), keytype_widget[line], COL_KEYTYPE, line , 1, 1);

        keyval_widget[line] = gtk_label_new("-");
        gtk_grid_attach(GTK_GRID(grid), keyval_widget[line], COL_KEYVAL, line , 1, 1);

        keysym_widget[line] = gtk_label_new("-");
        gtk_grid_attach(GTK_GRID(grid), keysym_widget[line], COL_KEYSYM, line , 1, 1);

        keymod_widget[line] = gtk_label_new("-----");
        gtk_grid_attach(GTK_GRID(grid), keymod_widget[line], COL_KEYMOD, line , 1, 1);
    }

    return grid;
}


/** \brief  Update keyboard debug widget with \a event
 *
 * \param[in]   event   GDK event structure
 */
void kdb_debug_widget_update(GdkEvent *event)
{
    int line;
    static gchar keytype_buffer[LINES][BUFSIZE];
    static gchar keyval_buffer[LINES][BUFSIZE];
    static gchar keysym_buffer[LINES][BUFSIZE];
    static gchar keymod_buffer[LINES][BUFSIZE];
    static gchar buffer[BUFSIZE];
    guint keyval = event->key.keyval;
    guint mods = event->key.state;
    GdkDisplay *display = gdk_display_get_default();
    GdkKeymap *keymap = gdk_keymap_get_for_display(display);
    int capslock = gdk_keymap_get_caps_lock_state(keymap);

    /* early exit if we are not visible */
    if (!gtk_widget_is_visible(keytype_widget[0])) {
        return;
    }

    for (line = 0; line < (LINES - 1); line++) {
        memcpy(keytype_buffer[line], keytype_buffer[line + 1], BUFSIZE);
        memcpy(keyval_buffer[line], keyval_buffer[line + 1], BUFSIZE);
        memcpy(keysym_buffer[line], keysym_buffer[line + 1], BUFSIZE);
        memcpy(keymod_buffer[line], keymod_buffer[line + 1], BUFSIZE);
    }

    switch(event->type) {
        case GDK_KEY_PRESS:
            g_snprintf(keytype_buffer[LINES - 1], BUFSIZE - 1, "press  ");
            break;
        case GDK_KEY_RELEASE:
            g_snprintf(keytype_buffer[LINES - 1], BUFSIZE - 1, "release");
            break;
        default:
            g_snprintf(keytype_buffer[LINES - 1], BUFSIZE - 1, "unknown");
            break;
    }

    g_snprintf(keyval_buffer[LINES -1], BUFSIZE - 1, "%5u, 0x%04x", keyval, keyval);
    g_snprintf(keysym_buffer[LINES -1], BUFSIZE - 1, "%s", gdk_keyval_name(keyval));
    g_snprintf(keymod_buffer[LINES -1], BUFSIZE - 1, "%c%c%c %c%c%c%c%c %c%c",
            mods & GDK_SHIFT_MASK ? 'S' : '-',    /* shift (left or right) */
            mods & GDK_LOCK_MASK ? 'L' : '-',     /* shift-lock */
            mods & GDK_CONTROL_MASK ? 'C' : '-',  /* control */
                                                  /* the additional ones seem to be different on */
                                                  /* windows        vs linux                     */
            mods & GDK_MOD1_MASK ? '1' : '-',     /* alt left          alt-left                  */
            mods & GDK_MOD2_MASK ? '2' : '-',     /* alt-gr (Alt_R)    num-lock                  */
            mods & GDK_MOD3_MASK ? '3' : '-',
            mods & GDK_MOD4_MASK ? '4' : '-',     /*                   windows right             */
            mods & GDK_MOD5_MASK ? '5' : '-',     /*                   alt-gr (ISO_Level3_Shift) */
            capslock ? 'L' : '-',                 /* host keyboard shift lock */
            keyboard_get_shiftlock() ? 'L' : '-'  /* emulated keyboard shift lock */
            );

    log_message(LOG_DEFAULT, "%s %s %s %s", keytype_buffer[LINES - 1],
                keyval_buffer[LINES -1], keymod_buffer[LINES -1], keysym_buffer[LINES -1]);

    for (line = 0; line < LINES; line++) {
        g_snprintf(buffer, BUFSIZE - 1, "<tt>%s</tt>", keytype_buffer[line]);
        gtk_label_set_markup(GTK_LABEL(keytype_widget[line]), buffer);
        g_snprintf(buffer, BUFSIZE - 1, "<tt>%s</tt>", keyval_buffer[line]);
        gtk_label_set_markup(GTK_LABEL(keyval_widget[line]), buffer);
        g_snprintf(buffer, BUFSIZE - 1, "<tt>%s</tt>", keysym_buffer[line]);
        gtk_label_set_markup(GTK_LABEL(keysym_widget[line]), buffer);
        g_snprintf(buffer, BUFSIZE - 1, "<tt>%s</tt>", keymod_buffer[line]);
        gtk_label_set_markup(GTK_LABEL(keymod_widget[line]), buffer);
    }
}
