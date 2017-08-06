/*
 * kbd.c - Native GTK3 UI keyboard stuff.
 *
 * Written by
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
 *  Michael C. Martin <mcmartin@gmail.com>
 * Adapted from the GNOME/X11 version by
 *  Oliver Schaertel
 *  pottendo <pottendo@gmx.net>
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
#include <gtk/gtk.h>

#include "keyboard.h"
#include "kbd.h"

#include "not_implemented.h"

int kbd_arch_get_host_mapping(void)
{
    NOT_IMPLEMENTED_WARN_ONLY();
    return KBD_MAPPING_US;
}

void kbd_arch_init(void)
{
}

signed long kbd_arch_keyname_to_keynum(char *keyname)
{
    guint sym = gdk_keyval_from_name(keyname);

    if (sym == GDK_KEY_VoidSymbol) {
        return -1;
    }

    return (signed long)sym;
}

const char *kbd_arch_keynum_to_keyname(signed long keynum)
{
    return gdk_keyval_name((guint)keynum);
}

void kbd_initialize_numpad_joykeys(int *joykeys)
{
    joykeys[0] = GDK_KEY_KP_0;
    joykeys[1] = GDK_KEY_KP_1;
    joykeys[2] = GDK_KEY_KP_2;
    joykeys[3] = GDK_KEY_KP_3;
    joykeys[4] = GDK_KEY_KP_4;
    joykeys[5] = GDK_KEY_KP_6;
    joykeys[6] = GDK_KEY_KP_7;
    joykeys[7] = GDK_KEY_KP_8;
    joykeys[8] = GDK_KEY_KP_9;
}

static gboolean kbd_event_handler(GtkWidget *w, GdkEvent *report, gpointer gp)
{
    gint key;

    key = report->key.keyval;
    switch (report->type) {
        case GDK_KEY_PRESS:
            /* fprintf(stderr, "KeyPress: %d.\n", key); */
            keyboard_key_pressed((signed long)key);
            return TRUE;
        case GDK_KEY_RELEASE:
            /* fprintf(stderr, "KeyRelease: %d.\n", key); */
            /* An old bug #549 reported that pressing and releasing
             * the shift keys produced different key codes in
             * GDK. This is no longer true as of GTK3's version of
             * GDK, but if it comes up again, the fix is to check for
             * those keycodes and then call keyboard_key_clear()
             * first, just for them. */
            keyboard_key_released(key);
            break;
        case GDK_ENTER_NOTIFY:
        case GDK_LEAVE_NOTIFY:
        case GDK_FOCUS_CHANGE:
            keyboard_key_clear();
            break;
        default:
            break;
    }                           /* switch */
    return FALSE;
}

void kbd_connect_handlers(GtkWidget *widget, void *data)
{
    g_signal_connect(G_OBJECT(widget), "key-press-event", G_CALLBACK(kbd_event_handler), data);
    g_signal_connect(G_OBJECT(widget), "key-release-event", G_CALLBACK(kbd_event_handler), data);
    g_signal_connect(G_OBJECT(widget), "enter-notify-event", G_CALLBACK(kbd_event_handler), data);
    g_signal_connect(G_OBJECT(widget), "leave-notify-event", G_CALLBACK(kbd_event_handler), data);
}
