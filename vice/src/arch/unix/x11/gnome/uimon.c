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

/* Work around an incompatible change in GDK header files
 * http://git.gnome.org/browse/gtk+/commit/gdk/gdkkeysyms.h?id=913cdf3be750a1e74c09b20edf55a57f9a919fcc */

#if defined GDK_KEY_0
#define GDK_KEY(symbol) GDK_KEY_##symbol
#else
#define GDK_KEY(symbol) GDK_##symbol
#endif

struct console_private_s {
    GtkWidget *window;
    GtkWidget *term;
    struct term_read_result {
        char *input_buffer;
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

static char* append_char_to_input_buffer(char *old_input_buffer, char new_char)
{
    char* new_input_buffer = lib_msprintf("%s%c",
        old_input_buffer ? old_input_buffer : "",
        new_char);
    lib_free(old_input_buffer);
    return new_input_buffer;
}

static char* append_string_to_input_buffer(char *old_input_buffer, char *new_string)
{
    char *new_input_buffer = lib_realloc(old_input_buffer, strlen(old_input_buffer) + strlen(new_string) + 1);
    char *char_in, *char_out = new_input_buffer;

    for (char_in = new_string; *char_in; char_in++) {
        if (*char_in < 0 || *char_in >= 32) {
            *char_out++ = *char_in;
        }
    }
    *char_out = 0;

    return new_input_buffer;
}

static gboolean plain_key_pressed(struct term_read_result *r, guint keyval)
{
    switch (keyval) {
    default:
        if(keyval >= GDK_KEY(space) && keyval <= GDK_KEY(ydiaeresis)){
            r->input_buffer = append_char_to_input_buffer(r->input_buffer, (char)keyval);
            return TRUE;
        }
        return FALSE;
    case GDK_KEY(Return):
        r->input_buffer = append_char_to_input_buffer(r->input_buffer, 13);
        return TRUE;
    case GDK_KEY(BackSpace):
        r->input_buffer = append_char_to_input_buffer(r->input_buffer, 127);
        return TRUE;
    case GDK_KEY(Left):
        r->input_buffer = append_char_to_input_buffer(r->input_buffer, 2);
        return TRUE;
    case GDK_KEY(Right):
        r->input_buffer = append_char_to_input_buffer(r->input_buffer, 6);
        return TRUE;
    case GDK_KEY(Up):
        r->input_buffer = append_char_to_input_buffer(r->input_buffer, 16);
        return TRUE;
    case GDK_KEY(Down):
        r->input_buffer = append_char_to_input_buffer(r->input_buffer, 14);
        return TRUE;
    case GDK_KEY(Tab):
        r->input_buffer = append_char_to_input_buffer(r->input_buffer, 9);
        return TRUE;
    case GDK_KEY(Delete):
        r->input_buffer = append_char_to_input_buffer(r->input_buffer, 4);
        return TRUE;
    case GDK_KEY(Home):
        r->input_buffer = append_char_to_input_buffer(r->input_buffer, 1);
        return TRUE;
    case GDK_KEY(End):
        r->input_buffer = append_char_to_input_buffer(r->input_buffer, 5);
        return TRUE;
    }
}

static gboolean ctrl_plus_key_pressed(struct term_read_result *r, guint keyval, GtkWidget *terminal)
{
    switch (keyval) {
    default:
        return FALSE;
    case GDK_KEY(h):
        r->input_buffer = append_char_to_input_buffer(r->input_buffer, 127);
        return TRUE;
    case GDK_KEY(b):
        r->input_buffer = append_char_to_input_buffer(r->input_buffer, 2);
        return TRUE;
    case GDK_KEY(f):
        r->input_buffer = append_char_to_input_buffer(r->input_buffer, 6);
        return TRUE;
    case GDK_KEY(p):
        r->input_buffer = append_char_to_input_buffer(r->input_buffer, 16);
        return TRUE;
    case GDK_KEY(n):
        r->input_buffer = append_char_to_input_buffer(r->input_buffer, 14);
        return TRUE;
    case GDK_KEY(t):
        r->input_buffer = append_char_to_input_buffer(r->input_buffer, 20);
        return TRUE;
    case GDK_KEY(d):
        r->input_buffer = append_char_to_input_buffer(r->input_buffer, 4);
        return TRUE;
    case GDK_KEY(u):
        r->input_buffer = append_char_to_input_buffer(r->input_buffer, 21);
        return TRUE;
    case GDK_KEY(k):
        r->input_buffer = append_char_to_input_buffer(r->input_buffer, 11);
        return TRUE;
    case GDK_KEY(a):
        r->input_buffer = append_char_to_input_buffer(r->input_buffer, 1);
        return TRUE;
    case GDK_KEY(e):
        r->input_buffer = append_char_to_input_buffer(r->input_buffer, 5);
        return TRUE;
    case GDK_KEY(c):
        vte_terminal_copy_clipboard(VTE_TERMINAL(terminal));
        return TRUE;
    case GDK_KEY(v):
        {
            GtkClipboard *clipboard = gtk_widget_get_clipboard(terminal, GDK_SELECTION_CLIPBOARD);
            gchar *new_string = gtk_clipboard_wait_for_text(clipboard);
            if (new_string != NULL) {
                r->input_buffer = append_string_to_input_buffer(r->input_buffer, new_string);
                g_free(new_string);
            }
        }
        return TRUE;
    }
}

static gboolean key_press_event (GtkWidget   *widget,
                                 GdkEventKey *event,
                                 gpointer     user_data)
{
    struct term_read_result *r = (struct term_read_result *)user_data;
    GdkModifierType state = 0;

    gdk_event_get_state((GdkEvent*)event, &state);

    if (!r->ended && event->type == GDK_KEY_PRESS){
        switch(state & (GDK_SHIFT_MASK | GDK_LOCK_MASK | GDK_CONTROL_MASK)) {
        case 0:
        case GDK_SHIFT_MASK:
            return plain_key_pressed(r, event->keyval);
        case GDK_CONTROL_MASK:
            return ctrl_plus_key_pressed(r, event->keyval, widget);
        default:
            return FALSE;
        }
    }
    return FALSE;
}

gboolean button_press_event(GtkWidget *widget,
                            GdkEvent  *event,
                            gpointer   user_data)
{
    struct term_read_result *r = (struct term_read_result *)user_data;
    GtkClipboard *clipboard;
    gchar *new_string;
    GdkEventButton *button_event = (GdkEventButton*)event;

    if (button_event->button != 2
     || button_event->type   != GDK_BUTTON_PRESS)
        return FALSE;
    clipboard = gtk_widget_get_clipboard(widget, GDK_SELECTION_PRIMARY);
    new_string = gtk_clipboard_wait_for_text(clipboard);
    if (new_string != NULL) {
        r->input_buffer = append_string_to_input_buffer(r->input_buffer, new_string);
        g_free(new_string);
    }
    return TRUE;
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
        int i;

        gtk_main_iteration();
        if (t->read_result.ended)
            return -1;
        for (i = 0; i < strlen(t->read_result.input_buffer) && retval < string_len; i++, retval++)
            string[retval]=t->read_result.input_buffer[i];
        memmove(t->read_result.input_buffer, t->read_result.input_buffer + i, strlen(t->read_result.input_buffer) + 1 - i);
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

    g_signal_connect(G_OBJECT(fixed.term), "key-press-event", 
        G_CALLBACK(key_press_event), &fixed.read_result);

    g_signal_connect(G_OBJECT(fixed.term), "button-press-event", 
        G_CALLBACK(button_press_event), &fixed.read_result);

    vte_console.console_xres = vte_terminal_get_column_count(VTE_TERMINAL(fixed.term));
    vte_console.console_yres = vte_terminal_get_row_count(VTE_TERMINAL(fixed.term));
    vte_console.console_can_stay_open = 1;
    return uimon_window_resume();
}

console_t *uimon_window_resume(void)
{
    gtk_window_set_modal(GTK_WINDOW(fixed.window), TRUE);
    gtk_widget_show_all(fixed.window);
    gtk_window_present (GTK_WINDOW(fixed.window));
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

    fixed.read_result.input_buffer = lib_stralloc("");;

    p = linenoise(prompt, &fixed);
    if (p) {
        if (*p) {
            linenoiseHistoryAdd(p);
        }
        ret_string = lib_stralloc(p);
        free(p);
    }
    else {
        ret_string = lib_stralloc("x");
    }
    lib_free(fixed.read_result.input_buffer);

    return ret_string;
}

int console_init(void)
{
    return 0;
}

int console_close_all(void)
{
    return 0;
}

