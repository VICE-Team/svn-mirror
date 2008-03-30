/*
 * gnomekbd.c - Simple Gnome-based graphical user interface.
 *
 * Written by
 *  Oliver Schaertel
 *  Martin Pottendorfer <Martin.Pottendorfer@alcatel.at>
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
 * GTK Keyboard driver
 */

#include "vice.h"

#include <string.h>             /* memset() */
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#include "kbd.h"
#include "keyboard.h"
#include "machine.h"

void kbd_arch_init(void)
{
}

signed long kbd_arch_keyname_to_keynum(char *keyname)
{
    guint sym = gdk_keyval_from_name(keyname);

    if (sym == GDK_VoidSymbol)
        return -1;

    return (signed long)sym;
}

const char *kbd_arch_keynum_to_keyname(signed long keynum)
{
    return gdk_keyval_name((guint)keynum);
}


gboolean kbd_event_handler(GtkWidget *w, GdkEvent *report, gpointer gp)
{
    gint key;

    key = report->key.keyval;
    switch (report->type) {
      case GDK_KEY_PRESS:
        keyboard_key_pressed((signed long)key);
        return TRUE;

      case GDK_KEY_RELEASE:
        if (key == GDK_Shift_L
         || key == GDK_Shift_R
         || key == GDK_ISO_Level3_Shift
        )
            keyboard_key_clear();
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

void kbd_initialize_numpad_joykeys(int* joykeys)
{
    joykeys[0] = GDK_KP_0;
    joykeys[1] = GDK_KP_1;
    joykeys[2] = GDK_KP_2;
    joykeys[3] = GDK_KP_3;
    joykeys[4] = GDK_KP_4;
    joykeys[5] = GDK_KP_6;
    joykeys[6] = GDK_KP_7;
    joykeys[7] = GDK_KP_8;
    joykeys[8] = GDK_KP_9;
}
