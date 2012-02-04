/*
 * uimon.c - Implementation of the terminal window for the GTK+ UI,
 * based on the VTE terminal widget.
 *
 * Written by
 *  Fabrizio Gennari <fabrizio.ge@tiscali.it>
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

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <stdlib.h>
#include <string.h>
#include <vte/vte.h>

#include "console.h"
#include "lib.h"
#include "linenoise.h"
#include "uiarch.h"
#include "uimon.h"

struct console_private_s {
    GtkWidget *window;
    GtkWidget *term;
    struct term_read_result {
        char pressed_key;
        gboolean key_pressed;
        gboolean ended;
    } read_result;
} fixed;

static console_t vte_console;

void write_to_terminal(struct console_private_s *t,
                       const char *data,
                       glong length){
    if(!t->read_result.ended)
        vte_terminal_feed(VTE_TERMINAL(t->term), data, length);
}

int getColumns(struct console_private_s *t)
{
    if(!t->read_result.ended)
        return vte_terminal_get_column_count(VTE_TERMINAL(t->term));
    return 80;
}

static gboolean key_press_event (GtkWidget   *widget,
                                 GdkEventKey *event,
                                 gpointer     user_data)
{
    struct term_read_result *r = (struct term_read_result *)user_data;
    if (!r->ended && event->type == GDK_KEY_PRESS){
        r->key_pressed = TRUE;
        switch (event->keyval) {
        default:
            if(event->keyval >= GDK_KEY_space && event->keyval <= GDK_KEY_ydiaeresis){
                r->pressed_key = (char)event->keyval;
            }
            else {
                r->key_pressed = FALSE;
            }
            break;
        case GDK_KEY_Return:
            r->pressed_key = 13;
            break;
        case GDK_KEY_BackSpace:
            r->pressed_key = 127;
            break;
        case GDK_KEY_Left:
            r->pressed_key = 2;
            break;
        case GDK_KEY_Right:
            r->pressed_key = 6;
            break;
        case GDK_KEY_Up:
            r->pressed_key = 16;
            break;
        case GDK_KEY_Down:
            r->pressed_key = 14;
            break;
        case GDK_KEY_Tab:
            r->pressed_key = 8;
            break;
        case GDK_KEY_Delete:
            r->pressed_key = 4;
            break;
        case GDK_KEY_Home:
            r->pressed_key = 1;
            break;
        case GDK_KEY_End:
            r->pressed_key = 5;
            break;
        }
    }
    return r->key_pressed;
}

static void close_window(GtkObject *object, gpointer user_data)
{
    struct term_read_result *r = (struct term_read_result *)user_data;
    r->ended = TRUE;
    vte_console.console_can_stay_open = 0;
}

int get_string(struct console_private_s *t, char* string, int string_len)
{
    int retval=0;
    while(retval<string_len){
        do{
            gtk_main_iteration();
            if (t->read_result.ended)
                return -1;
        }while(!t->read_result.key_pressed);
      t->read_result.key_pressed = FALSE;
      string[retval++]=t->read_result.pressed_key;
    }
    return retval;
}

console_t *uimon_window_open(void)
{
    fixed.window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(fixed.window), "VICE monitor");
    gtk_window_set_position(GTK_WINDOW(fixed.window), GTK_WIN_POS_CENTER);
    gtk_widget_set_app_paintable(fixed.window, TRUE);
    gtk_window_set_deletable(GTK_WINDOW(fixed.window), FALSE);
    gtk_window_set_transient_for(GTK_WINDOW(fixed.window), GTK_WINDOW(get_active_toplevel()));
    fixed.term = vte_terminal_new();
    gtk_container_add(GTK_CONTAINER(fixed.window), fixed.term);

    g_signal_connect(G_OBJECT(fixed.window), "destroy", 
        G_CALLBACK(close_window), &fixed.read_result);

    g_signal_connect(G_OBJECT(fixed.window), "key-press-event", 
        G_CALLBACK(key_press_event), &fixed.read_result);

    vte_console.console_xres = vte_terminal_get_column_count(VTE_TERMINAL(fixed.term));
    vte_console.console_yres = vte_terminal_get_row_count(VTE_TERMINAL(fixed.term));
    vte_console.console_can_stay_open = 1;
    return uimon_window_resume();
}

console_t *uimon_window_resume(void)
{
    gtk_window_set_modal(GTK_WINDOW(fixed.window), TRUE);
    gtk_widget_show_all(fixed.window);
    fixed.read_result.key_pressed = FALSE;
    fixed.read_result.ended = FALSE;
    return &vte_console;
}

void uimon_window_suspend(void)
{
    gtk_widget_hide(fixed.window);
    gtk_window_set_modal(GTK_WINDOW(fixed.window), FALSE);
}

int uimon_out(const char *buffer)
{
    const char *c;
    for(c = buffer; *c; c++){
        if(*c == '\n')
            write_to_terminal(&fixed, "\r", 1);
        write_to_terminal(&fixed, c, 1);
    }
    return 0;
}

void uimon_window_close(void)
{
    fixed.window = fixed.term = NULL;
}

void uimon_notify_change(void)
{
}

void uimon_set_interface(struct monitor_interface_s **interf, int i)
{
}

char *uimon_get_in(char **ppchCommandLine, const char *prompt)
{
    char *p, *ret_string;

    p = linenoise(prompt, &fixed);
    if (p) {
        linenoiseHistoryAdd(p);
        ret_string = lib_stralloc(p);
        free(p);
    }
    else {
        ret_string = lib_stralloc("x");
    }

    return ret_string;
}

int console_init(void){}
int console_close_all(void){}

