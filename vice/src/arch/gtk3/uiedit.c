/*
 * uiedit.c - "Edit" submenu (copy / paste) for GTK3
 *
 * Written by
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

#include "vice.h"

#include <string.h>

#include "charset.h"
#include "clipboard.h"
#include "uiedit.h"
#include "lib.h"
#include "kbdbuf.h"

static void paste_callback(GtkClipboard *clipboard, const gchar *text, gpointer data)
{
    char *text_in_petscii;
    if (text == NULL) {
        return;
    }
    text_in_petscii = lib_stralloc(text);

    charset_petconvstring((unsigned char*)text_in_petscii, 0);
    kbdbuf_feed(text_in_petscii);
    lib_free(text_in_petscii);
}

void ui_copy_callback(GtkWidget *widget, gpointer user_data)
{
    char * text = clipboard_read_screen_output("\n");
    if (text != NULL) {
        gtk_clipboard_set_text(gtk_clipboard_get(GDK_SELECTION_CLIPBOARD), text, strlen(text));
    }
}

void ui_paste_callback(GtkWidget *widget, gpointer user_data)
{
    gtk_clipboard_request_text(gtk_clipboard_get(GDK_NONE), paste_callback, NULL);
}
