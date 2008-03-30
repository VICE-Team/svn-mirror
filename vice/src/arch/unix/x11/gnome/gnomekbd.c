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


gboolean kbd_event_handler(GtkWidget *w, GdkEvent *report, gpointer gp)
{
    gint key;

    key = report->key.keyval;
    switch (report->type) {
      case GDK_KEY_PRESS:
        keyboard_key_pressed((signed long)key);
        return FALSE;

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
    return TRUE;
}

