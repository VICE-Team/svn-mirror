/** \file   uimon.c
 * \brief   Native GTK3 UI monitor stuff
 *
 * \author  Fabrizio Gennari <fabrizio.ge@tiscali.it>
 * \author  David Hogan <david.q.hogan@gmail.com>
 *
 * TODO:    Properly document this, please.
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

#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>

#include "novte/novte.h"

#define vte_terminal_new novte_terminal_new
#define vte_terminal_feed novte_terminal_feed
#define vte_terminal_get_column_count novte_terminal_get_column_count
#define vte_terminal_copy_clipboard novte_terminal_copy_clipboard
#define vte_terminal_get_row_count novte_terminal_get_row_count
#define vte_terminal_set_scrollback_lines novte_terminal_set_scrollback_lines
#define vte_terminal_set_scroll_on_output novte_terminal_set_scroll_on_output
#define vte_terminal_get_char_width novte_terminal_get_char_width
#define vte_terminal_get_char_height novte_terminal_get_char_height

#define VTE_TERMINAL(x) NOVTE_TERMINAL(x)
#define VteTerminal NoVteTerminal

#include <dirent.h>
#include <ctype.h>
#include <unistd.h>
#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif

#include "console.h"
#include "debug_gtk3.h"
#include "machine.h"
#include "monitor.h"
#include "mainlock.h"
#include "resources.h"
#include "lib.h"
#include "log.h"
#include "ui.h"
#include "linenoise.h"
#include "tick.h"
#include "uimon.h"
#include "uimonarch.h"
#include "uimon-fallback.h"
#include "mon_command.h"
#include "vsync.h"
#include "vsyncapi.h"
#include "uidata.h"

static gboolean uimon_window_open_impl(gpointer user_data);
static gboolean uimon_window_resume_impl(gpointer user_data);

#define VTE_CSS \
    "vte-terminal { font-size: 300%; }"

/** \brief  Monitor console window object
 *
 * Again, guess work. Someone, not me, should have documented this.
 */
static struct console_private_s {
    pthread_mutex_t lock;

    GtkWidget *window;  /**< windows */
    GtkWidget *term;    /**< could be a VTE instance? */
    char *input_buffer;
    char *output_buffer;
    size_t output_buffer_allocated_size;
    size_t output_buffer_used_size;
} fixed;

static console_t vte_console;
static linenoiseCompletions command_lc = {0, NULL};
static linenoiseCompletions need_filename_lc = {0, NULL};


/* FIXME: this should perhaps be done using some function from archdep */
static int is_dir(struct dirent *de)
{
    return 0;
}

static int native_monitor(void)
{
    int res = 0;
    resources_get_int("NativeMonitor", &res);
    return res;
}

static gboolean write_to_terminal(gpointer _)
{
    pthread_mutex_lock(&fixed.lock);

    if (!fixed.term) {
        /* Terminal hasn't been created yet, queue up the write for now. */
        goto done;
    }

    if (fixed.output_buffer) {
        vte_terminal_feed(VTE_TERMINAL(fixed.term), fixed.output_buffer, fixed.output_buffer_used_size);

        lib_free(fixed.output_buffer);
        fixed.output_buffer = NULL;
        fixed.output_buffer_allocated_size = 0;
        fixed.output_buffer_used_size = 0;
    }

done:
    pthread_mutex_unlock(&fixed.lock);

    return FALSE;
}

void uimon_write_to_terminal(struct console_private_s *t,
                             const char *data,
                             glong length)
{
    size_t output_buffer_required_size;
    bool write_scheduled = false;

    pthread_mutex_lock(&fixed.lock);

    /*
     * If the output buffer exists, we've already scheduled a write
     * so we just append to the existing buffer.
     */

    output_buffer_required_size = fixed.output_buffer_used_size + length;

    if (output_buffer_required_size > fixed.output_buffer_allocated_size) {
        output_buffer_required_size += 4096;

        if (fixed.output_buffer) {
            write_scheduled = true;
            fixed.output_buffer = lib_realloc(fixed.output_buffer, output_buffer_required_size);
        } else {
            fixed.output_buffer = lib_malloc(output_buffer_required_size);
        }

        fixed.output_buffer_allocated_size = output_buffer_required_size;
    }

    memcpy(fixed.output_buffer + fixed.output_buffer_used_size, data, length);
    fixed.output_buffer_used_size += length;

    if (!write_scheduled) {
        /* schedule a call on the ui thread */
        gdk_threads_add_timeout(0, write_to_terminal, NULL);
    }

    pthread_mutex_unlock(&fixed.lock);
}

int uimon_out(const char *buffer)
{
    const char *line;
    const char *line_end;

    if (native_monitor()) {
        return uimonfb_out(buffer);
    }

    /* Substitute \n for \r\n when feeding the terminal */

    line = buffer;
    while (*line != '\0') {
        line_end = strchr(line, '\n');

        if (line_end == NULL) {
            /* buffer ends without a \n */
            uimon_write_to_terminal(&fixed, line, strlen(line));
            break;
        }

        uimon_write_to_terminal(&fixed, line, line_end - line);
        uimon_write_to_terminal(&fixed, "\r\n", 2);

        line = line_end + 1;
    }

    return 0;
}


int uimon_get_columns(struct console_private_s *t)
{
    if(t->term) {
        return (int)vte_terminal_get_column_count(VTE_TERMINAL(t->term));
    }
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

static char* append_string_to_input_buffer(char *old_input_buffer, GtkWidget *terminal, GdkAtom clipboard_to_use)
{
    GtkClipboard *clipboard = gtk_widget_get_clipboard(terminal, clipboard_to_use);
    gchar *new_string = gtk_clipboard_wait_for_text(clipboard);

    if (new_string != NULL) {
        char *new_input_buffer = lib_realloc(old_input_buffer, strlen(old_input_buffer) + strlen(new_string) + 1);
        char *char_in, *char_out = new_input_buffer + strlen(new_input_buffer);

        for (char_in = new_string; *char_in; char_in++) {
#if CHAR_MIN < 0
            if (*char_in < 0 || *char_in >= 32) {
#else
            /* char is unsigned on raspberry Pi 2B with GCC */
            if (*char_in >= 32) {
#endif
                *char_out++ = *char_in;
            } else if (*char_in == 10) {
                *char_out++ = 13;
            }
        }
        *char_out = 0;
        g_free(new_string);

        return new_input_buffer;
    }
    return old_input_buffer;
}

static gboolean plain_key_pressed(char **input_buffer, guint keyval)
{
    switch (keyval) {
        default:
            if(keyval >= GDK_KEY_space && keyval <= GDK_KEY_ydiaeresis) {
                *input_buffer = append_char_to_input_buffer(*input_buffer, (char)keyval);
                return TRUE;
            }
            if(keyval >= GDK_KEY_KP_0 && keyval <= GDK_KEY_KP_9) {
                *input_buffer =
                    append_char_to_input_buffer(
                        *input_buffer,
                        (char)keyval - GDK_KEY_KP_0 + 48);
                return TRUE;
            }
            return FALSE;
        case GDK_KEY_Home:
        case GDK_KEY_KP_Home:
            *input_buffer = append_char_to_input_buffer(*input_buffer, 1);
            return TRUE;
        case GDK_KEY_Left:
        case GDK_KEY_KP_Left:
            *input_buffer = append_char_to_input_buffer(*input_buffer, 2);
            return TRUE;
        case GDK_KEY_Delete:
        case GDK_KEY_KP_Delete:
            /* We use Ctrl+W here to signal linenoise to not exit the monitor
             * when pressing Delete on an empty line, see comment at
             * src/arch/gtk3/linenoise.c:308 --compyx */
            *input_buffer = append_char_to_input_buffer(*input_buffer, 23);
            return TRUE;
        case GDK_KEY_End:
        case GDK_KEY_KP_End:
            *input_buffer = append_char_to_input_buffer(*input_buffer, 5);
            return TRUE;
        case GDK_KEY_Right:
        case GDK_KEY_KP_Right:
            *input_buffer = append_char_to_input_buffer(*input_buffer, 6);
            return TRUE;
        case GDK_KEY_Tab:
            *input_buffer = append_char_to_input_buffer(*input_buffer, 9);
            return TRUE;
        case GDK_KEY_Return:
        case GDK_KEY_KP_Enter:
            *input_buffer = append_char_to_input_buffer(*input_buffer, 13);
            return TRUE;
        case GDK_KEY_Down:
        case GDK_KEY_KP_Down:
            *input_buffer = append_char_to_input_buffer(*input_buffer, 14);
            return TRUE;
        case GDK_KEY_Up:
        case GDK_KEY_KP_Up:
            *input_buffer = append_char_to_input_buffer(*input_buffer, 16);
            return TRUE;
        case GDK_KEY_dead_diaeresis:
            *input_buffer = append_char_to_input_buffer(*input_buffer, 34);
            return TRUE;
        case GDK_KEY_dead_acute:
            *input_buffer = append_char_to_input_buffer(*input_buffer, 39);
            return TRUE;
        case GDK_KEY_KP_Multiply:
            *input_buffer = append_char_to_input_buffer(*input_buffer, 42);
            return TRUE;
        case GDK_KEY_KP_Add:
            *input_buffer = append_char_to_input_buffer(*input_buffer, 43);
            return TRUE;
        case GDK_KEY_KP_Subtract:
            *input_buffer = append_char_to_input_buffer(*input_buffer, 45);
            return TRUE;
        case GDK_KEY_KP_Decimal:
            *input_buffer = append_char_to_input_buffer(*input_buffer, 46);
            return TRUE;
        case GDK_KEY_KP_Divide:
            *input_buffer = append_char_to_input_buffer(*input_buffer, 47);
            return TRUE;
        case GDK_KEY_dead_circumflex:
            *input_buffer = append_char_to_input_buffer(*input_buffer, 94);
            return TRUE;
        case GDK_KEY_dead_grave:
            *input_buffer = append_char_to_input_buffer(*input_buffer, 96);
            return TRUE;
        case GDK_KEY_dead_tilde:
            *input_buffer = append_char_to_input_buffer(*input_buffer, 126);
            return TRUE;
        case GDK_KEY_BackSpace:
            *input_buffer = append_char_to_input_buffer(*input_buffer, 127);
            return TRUE;
    }
}

static gboolean ctrl_plus_key_pressed(char **input_buffer, guint keyval, GtkWidget *terminal)
{
    switch (keyval) {
        default:
            return FALSE;
        case GDK_KEY_h:
        case GDK_KEY_H:
            *input_buffer = append_char_to_input_buffer(*input_buffer, 127);
            return TRUE;
        case GDK_KEY_b:
        case GDK_KEY_B:
            *input_buffer = append_char_to_input_buffer(*input_buffer, 2);
            return TRUE;
        case GDK_KEY_f:
        case GDK_KEY_F:
            *input_buffer = append_char_to_input_buffer(*input_buffer, 6);
            return TRUE;
        case GDK_KEY_p:
        case GDK_KEY_P:
            *input_buffer = append_char_to_input_buffer(*input_buffer, 16);
            return TRUE;
        case GDK_KEY_n:
        case GDK_KEY_N:
            *input_buffer = append_char_to_input_buffer(*input_buffer, 14);
            return TRUE;
        case GDK_KEY_t:
        case GDK_KEY_T:
            *input_buffer = append_char_to_input_buffer(*input_buffer, 20);
            return TRUE;
        case GDK_KEY_d:
        case GDK_KEY_D:
            /* ctrl-d, remove char at right of cursor */
            *input_buffer = append_char_to_input_buffer(*input_buffer, 4);
            return TRUE;
        case GDK_KEY_u:
        case GDK_KEY_U:
            /* Ctrl+u, delete the whole line. */
            *input_buffer = append_char_to_input_buffer(*input_buffer, 21);
            return TRUE;
        case GDK_KEY_k:
        case GDK_KEY_K:
            /* Ctrl+k, delete from current to end of line. */
            *input_buffer = append_char_to_input_buffer(*input_buffer, 11);
            return TRUE;
        case GDK_KEY_a:
        case GDK_KEY_A:
            /* Ctrl+a, go to the start of the line */
            *input_buffer = append_char_to_input_buffer(*input_buffer, 1);
            return TRUE;
        case GDK_KEY_e:
        case GDK_KEY_E:
            /* ctrl+e, go to the end of the line */
            *input_buffer = append_char_to_input_buffer(*input_buffer, 5);
            return TRUE;
#ifndef MACOSX_SUPPORT
        case GDK_KEY_c:
        case GDK_KEY_C:
            vte_terminal_copy_clipboard(VTE_TERMINAL(terminal));
            /* _format only exists in bleeding edge VTE 0.50 */
            /* vte_terminal_copy_clipboard_format(VTE_TERMINAL(terminal), VTE_FORMAT_TEXT); */
            return TRUE;
        case GDK_KEY_v:
        case GDK_KEY_V:
            *input_buffer = append_string_to_input_buffer(*input_buffer, terminal, GDK_SELECTION_CLIPBOARD);
            return TRUE;
#endif
    }
}

#ifdef MACOSX_SUPPORT
static gboolean cmd_plus_key_pressed(char **input_buffer, guint keyval, GtkWidget *terminal)
{
    switch (keyval) {
        default:
            return FALSE;
        case GDK_KEY_c:
        case GDK_KEY_C:
            vte_terminal_copy_clipboard(VTE_TERMINAL(terminal));
            /* _format only exists in bleeding edge VTE 0.50 */
            /* vte_terminal_copy_clipboard_format(VTE_TERMINAL(terminal), VTE_FORMAT_TEXT); */
            return TRUE;
        case GDK_KEY_v:
        case GDK_KEY_V:
            *input_buffer = append_string_to_input_buffer(*input_buffer, terminal, GDK_SELECTION_CLIPBOARD);
            return TRUE;
    }
}
#endif

static gboolean key_press_event (GtkWidget   *widget,
                                 GdkEventKey *event,
                                 gpointer     user_data)
{
    GdkModifierType state = 0;
    gboolean retval = FALSE;

    gdk_event_get_state((GdkEvent*)event, &state);

    pthread_mutex_lock(&fixed.lock);

    if (event->type == GDK_KEY_PRESS) {
        if (state & GDK_CONTROL_MASK) {
            retval = ctrl_plus_key_pressed(&fixed.input_buffer, event->keyval, widget);
            goto done;
        }
#ifdef MACOSX_SUPPORT
        if (state & GDK_MOD2_MASK) {
            retval = cmd_plus_key_pressed(&fixed.input_buffer, event->keyval, widget);
            goto done;
        }
#endif
        retval = plain_key_pressed(&fixed.input_buffer, event->keyval);
        goto done;
    }

done:
    pthread_mutex_unlock(&fixed.lock);

    return retval;
}


static gboolean button_press_event(GtkWidget *widget,
                            GdkEvent  *event,
                            gpointer   user_data)
{
    GdkEventButton *button_event = (GdkEventButton*)event;

    if (button_event->button != 2
     || button_event->type   != GDK_BUTTON_PRESS) {
        return FALSE;
    }

    pthread_mutex_lock(&fixed.lock);
    fixed.input_buffer = append_string_to_input_buffer(fixed.input_buffer, widget, GDK_SELECTION_PRIMARY);
    pthread_mutex_unlock(&fixed.lock);

    return TRUE;
}

static gboolean close_window(GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
    pthread_mutex_lock(&fixed.lock);

    lib_free(fixed.input_buffer);
    fixed.input_buffer = NULL;

    pthread_mutex_unlock(&fixed.lock);

    return gtk_widget_hide_on_delete(widget);
}

/* \brief Block until some monitor input event happens */
int uimon_get_string(struct console_private_s *t, char* string, int string_len)
{
    int retval=0;
    while(retval<string_len) {
        int i;

        pthread_mutex_lock(&t->lock);

        if (!t->input_buffer) {
            /* TODO: Not sure if this check makes sense anymore, needs testing without */
            pthread_mutex_unlock(&t->lock);
            return -1;
        }

        if (strlen(t->input_buffer) == 0) {
            /* There's no input yet, so have a little sleep and look again. */
            pthread_mutex_unlock(&t->lock);
            tick_sleep(tick_per_second() / 60);
            continue;
        }

        for (i = 0; i < strlen(t->input_buffer) && retval < string_len; i++, retval++) {
            string[retval]=t->input_buffer[i];
        }
        memmove(t->input_buffer, t->input_buffer + i, strlen(t->input_buffer) + 1 - i);
        pthread_mutex_unlock(&t->lock);
    }
    return retval;
}

static void get_terminal_size_in_chars(VteTerminal *terminal,
                           glong *width,
                           glong *height)
{
    *width = vte_terminal_get_column_count(terminal);
    *height = vte_terminal_get_row_count(terminal);
}

static void screen_resize_window_cb (VteTerminal *terminal,
                         gpointer* window)
{
    glong width, height;
    get_terminal_size_in_chars(terminal, &width, &height);
    vte_console.console_xres = (unsigned int)width;
    vte_console.console_yres = (unsigned int)height;
}

/* resize the terminal when the window is resized */
static void screen_resize_window_cb2 (VteTerminal *terminal,
                         gpointer* window)
{
    gint width, height;
    gint xpos;
    gint ypos;
    glong cwidth, cheight;
    glong newwidth, newheight;

    gtk_window_get_size (GTK_WINDOW(fixed.window), &width, &height);
    gtk_window_get_position(GTK_WINDOW(fixed.window), &xpos, &ypos);
    cwidth = vte_terminal_get_char_width (VTE_TERMINAL(fixed.term));
    cheight = vte_terminal_get_char_height (VTE_TERMINAL(fixed.term));

    newwidth = width / cwidth;
    newheight = height / cheight;
    if (newwidth < 1) {
        newwidth = 1;
    }
    if (newheight < 1) {
        newheight = 1;
    }

    if (xpos >= 0 && ypos >= 0) {
        resources_set_int("MonitorXPos", xpos);
        resources_set_int("MonitorYPos", ypos);
    }
    if (width > 0 && height > 0) {
        resources_set_int("MonitorWidth", width);
        resources_set_int("MonitorHeight", height);
    }


    vte_terminal_set_size(VTE_TERMINAL(fixed.term), newwidth, newheight);
}

/** \brief  Create an icon by loading it from the vice.gresource file
 *
 * \return  Current emulator's icon
 *
 * \note    If we want something else, we should ask whoever created the current
 *          icon set.
 */
static GdkPixbuf *get_default_icon(void)
{
    char buffer[1024];

    g_snprintf(buffer, sizeof(buffer), "%s.svg", machine_name);
    return uidata_get_pixbuf(buffer);
}

console_t *uimonfb_window_open(void);


/** \brief  Try to set VTE monitor font
 *
 * \return  boolean
 */
bool uimon_set_font(void)
{
    const PangoFontDescription *desc_tmp;
    PangoFontDescription* desc;
    const char *monitor_font = NULL;
    GList *widgets;
    GList *box;
    const char *bg;
    const char *fg;
    GdkRGBA color;

    if (resources_get_string("MonitorFont", &monitor_font) < 0) {
        log_error(LOG_ERR, "Failed to read 'MonitorFont' resource.");
        return false;
    }

    if (fixed.term == NULL) {
        log_error(LOG_ERR, "No monitor instance found.");
        return false;
    }

    /* try to set monitor font */
    desc = pango_font_description_from_string(monitor_font);
    if (desc == NULL) {
        /* fall back */
        log_warning(LOG_ERR, "Failed to parse Pango font description, falling"
                " back to default font.");

        desc_tmp = vte_terminal_get_font(VTE_TERMINAL(fixed.term));
        desc = pango_font_description_copy_static(desc_tmp);
        pango_font_description_set_family(desc, "Consolas,monospace");
        pango_font_description_set_size(desc, 11 * PANGO_SCALE);
    }
    vte_terminal_set_font(VTE_TERMINAL(fixed.term), desc);
    pango_font_description_free(desc);

    /* try background color */
    if (resources_get_string("MonitorBG", &bg) < 0) {
        bg = NULL;
    }
    if (gdk_rgba_parse(&color, bg)) {
        vte_terminal_set_color_background(VTE_TERMINAL(fixed.term), &color);
    }

    /* try foreground color */
    if (resources_get_string("MonitorFG", &fg) < 0) {
        fg = NULL;
    }
    if (gdk_rgba_parse(&color, fg)) {
        vte_terminal_set_color_foreground(VTE_TERMINAL(fixed.term), &color);
    }

    gtk_widget_set_size_request(GTK_WIDGET(fixed.window), -1, -1);
    gtk_widget_set_size_request(GTK_WIDGET(fixed.term), -1, -1);

    /* get GtkBox */
    widgets = gtk_container_get_children(GTK_CONTAINER(fixed.window));
    box = g_list_first(widgets);

    gtk_widget_set_size_request(GTK_WIDGET(box->data), -1 , -1);
    return true;
}


/** \brief  Set foreground color
 *
 * Set VTE terminal foreground color to \a color.
 *
 * \param[in]   color   Gdk RGBA color string
 *
 * \return  bool
 *
 * \see     https://docs.gtk.org/gdk3/method.RGBA.parse.html
 */
bool uimon_set_foreground_color(const char *color)
{
    if (fixed.term != NULL) {
        GdkRGBA rgba;

        if (gdk_rgba_parse(&rgba, color)) {
            vte_terminal_set_color_foreground(VTE_TERMINAL(fixed.term), &rgba);
            return true;
        }
    }
    return false;
}


/** \brief  Set foreground color
 *
 * Set VTE terminal foreground color to \a color.
 *
 * \param[in]   color   Gdk RGBA color string
 *
 * \return  bool
 *
 * \see     https://docs.gtk.org/gdk3/method.RGBA.parse.html
 */
bool uimon_set_background_color(const char *color)
{
    if (fixed.term != NULL) {
        GdkRGBA rgba;

        if (gdk_rgba_parse(&rgba, color)) {
            vte_terminal_set_color_background(VTE_TERMINAL(fixed.term), &rgba);
            return true;
        }
    }
    return false;
}


static gboolean uimon_window_open_impl(gpointer user_data)
{
    bool display_now = (bool)user_data;
    GtkWidget *scrollbar, *horizontal_container;
    GdkGeometry hints;
    GdkPixbuf *icon;
    int sblines;
    int xpos = -1;
    int ypos = -1;
    int width = -1;
    int height = -1;

    pthread_mutex_lock(&fixed.lock);

    resources_get_int("MonitorScrollbackLines", &sblines);
    resources_get_int("MonitorXPos", &xpos);
    resources_get_int("MonitorYPos", &ypos);
    resources_get_int("MonitorHeight", &height);
    resources_get_int("MonitorWidth", &width);

    if (fixed.window == NULL) {
        fixed.window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        gtk_window_set_title(GTK_WINDOW(fixed.window), "VICE monitor");
        if (xpos < 0 && ypos < 0) {
            /* Only center if we didn't get either a previous position or
             * the position was set via the command line.
             */
            gtk_window_set_position(GTK_WINDOW(fixed.window), GTK_WIN_POS_CENTER);
        }
        gtk_widget_set_app_paintable(fixed.window, TRUE);
        gtk_window_set_deletable(GTK_WINDOW(fixed.window), TRUE);

        /* set a default C= icon for now */
        icon = get_default_icon();
        if (icon != NULL) {
            gtk_window_set_icon(GTK_WINDOW(fixed.window), icon);
        }

        fixed.term = vte_terminal_new();
        vte_terminal_set_scrollback_lines (VTE_TERMINAL(fixed.term), sblines);
        vte_terminal_set_scroll_on_output (VTE_TERMINAL(fixed.term), TRUE);

        /* allowed window widths are base_width + width_inc * N
         * allowed window heights are base_height + height_inc * N
         */
        hints.width_inc = (gint)vte_terminal_get_char_width (VTE_TERMINAL(fixed.term));
        hints.height_inc = (gint)vte_terminal_get_char_height (VTE_TERMINAL(fixed.term));
        /* min size should be multiple of .._inc, else we get funky effects */
        hints.min_width = hints.width_inc;
        hints.min_height = hints.height_inc;
        /* base size should be multiple of .._inc, else we get funky effects */
        hints.base_width = hints.width_inc;
        hints.base_height = hints.height_inc;
        gtk_window_set_geometry_hints (GTK_WINDOW (fixed.window),
                                     fixed.term,
                                     &hints,
                                     GDK_HINT_RESIZE_INC |
                                     GDK_HINT_MIN_SIZE |
                                     GDK_HINT_BASE_SIZE);

         if (xpos > INT_MIN && ypos > INT_MIN) {
            gtk_window_move(GTK_WINDOW(fixed.window), xpos, ypos);
        }
        if (width >= 0 && height >= 0) {
            gtk_window_resize(GTK_WINDOW(fixed.window), width, height);
        }


        scrollbar = gtk_scrollbar_new(GTK_ORIENTATION_VERTICAL,
                gtk_scrollable_get_vadjustment(GTK_SCROLLABLE(fixed.term)));

        horizontal_container = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
        gtk_container_add(GTK_CONTAINER(fixed.window), horizontal_container);

#if 0
        gtk_container_add(GTK_CONTAINER(horizontal_container), fixed.term);
        gtk_container_add(GTK_CONTAINER(horizontal_container), scrollbar);
#else
        gtk_box_pack_start(GTK_BOX(horizontal_container), fixed.term,
                TRUE, TRUE, 0);
        gtk_box_pack_end(GTK_BOX(horizontal_container), scrollbar,
                FALSE, FALSE, 0);
#endif

        g_signal_connect(G_OBJECT(fixed.window), "delete-event",
            G_CALLBACK(close_window), NULL);

        g_signal_connect_unlocked(G_OBJECT(fixed.term), "key-press-event",
            G_CALLBACK(key_press_event), NULL);

        g_signal_connect_unlocked(G_OBJECT(fixed.term), "button-press-event",
            G_CALLBACK(button_press_event), NULL);

        g_signal_connect_unlocked(G_OBJECT(fixed.term), "text-modified",
            G_CALLBACK (screen_resize_window_cb), NULL);

        g_signal_connect_unlocked(G_OBJECT(fixed.window), "configure-event",
            G_CALLBACK (screen_resize_window_cb2), NULL);

        vte_console.console_can_stay_open = 1;

        uimon_set_font();
    } else {
        vte_terminal_set_scrollback_lines (VTE_TERMINAL(fixed.term), sblines);
    }

    pthread_mutex_unlock(&fixed.lock);

    if (display_now) {
        uimon_window_resume_impl(NULL);
    }

    /* Ensure any queued monitor output is displayed */
    gdk_threads_add_timeout(0, write_to_terminal, NULL);

    return FALSE;
}

static gboolean uimon_window_resume_impl(gpointer user_data)
{
    gtk_widget_show_all(fixed.window);
    screen_resize_window_cb (VTE_TERMINAL(fixed.term), NULL);

    /*
     * Make the monitor window appear on top of the active emulated machine
     * window. This makes the monitor window show when the emulated machine
     * window is in fullscreen mode. (only tested on Windows 10)
     */
    gtk_window_present(GTK_WINDOW(fixed.window));

    return FALSE;
}

static gboolean uimon_window_suspend_impl(gpointer user_data)
{
    if (fixed.window != NULL) {
        int keep_open = 0;

        /* do need to keep the monitor window open? */
        resources_get_int("KeepMonitorOpen", &keep_open);
        if (!keep_open) {
            gtk_widget_hide(fixed.window);
        } else {
            /* move monitor window behind the emu window */
            GtkWidget *window = ui_get_window_by_index(ui_get_main_window_index());
            gtk_window_present(GTK_WINDOW(window));
        }
    }

    return FALSE;
}

static gboolean uimon_window_close_impl(gpointer user_data)
{
    /* Flush any queued writes */
    write_to_terminal(NULL);

    /* only close window if there is one: this avoids a GTK_CRITICAL warning
     * when using a remote monitor */
    if (fixed.window != NULL) {
        gtk_widget_hide(fixed.window);
    }

    return FALSE;
}

console_t *uimon_window_open(bool display_now)
{
    if (native_monitor()) {
        return uimonfb_window_open();
    }

    /* call from ui thread */
    gdk_threads_add_timeout(0, uimon_window_open_impl, (gpointer)display_now);

    return &vte_console;
}

console_t *uimon_window_resume(void)
{
    if (native_monitor()) {
        return uimonfb_window_resume();
    }

    /* call from ui thread */
    gdk_threads_add_timeout(0, uimon_window_resume_impl, NULL);

    return &vte_console;
}

void uimon_window_suspend(void)
{
    if (native_monitor()) {
        uimonfb_window_suspend();
        return;
    }

    /* call from ui thread */
    gdk_threads_add_timeout(0, uimon_window_suspend_impl, NULL);
}

void uimon_window_close(void)
{
    if (native_monitor()) {
        uimonfb_window_close();
        return;
    }

    /* call from ui thread */
    gdk_threads_add_timeout(0, uimon_window_close_impl, NULL);
}

void uimon_notify_change(void)
{
    if (native_monitor()) {
        uimonfb_notify_change();
        return;
    }
}

void uimon_set_interface(struct monitor_interface_s **interf, int i)
{
    if (native_monitor()) {
        uimonfb_set_interface(interf, i);
        return;
    }
}

static char* concat_strings(const char *string1, int nchars, const char *string2)
{
    char *ret = lib_malloc(nchars + strlen(string2) + 1);
    memcpy(ret, string1, nchars);
    strcpy(ret + nchars, string2);
    return ret;
}

static void fill_completions(const char *string_so_far, int initial_chars, int token_len, const linenoiseCompletions *possible_lc, linenoiseCompletions *lc)
{
    int word_index;

    lc->len = 0;
    for(word_index = 0; word_index < possible_lc->len; word_index++) {
        int i;
        for(i = 0; i < token_len; i++) {
            if (string_so_far[initial_chars + i] != possible_lc->cvec[word_index][i]) {
                break;
            }
        }
        if (i == token_len && possible_lc->cvec[word_index][token_len] != 0) {
            char *string_to_append = concat_strings(string_so_far, initial_chars, possible_lc->cvec[word_index]);
            vte_linenoiseAddCompletion(lc, string_to_append);
            lib_free(string_to_append);
        }
    }
}

static void find_next_token(const char *string_so_far, int start_of_search, int *start_of_token, int *token_len)
{
    for(*start_of_token = start_of_search; string_so_far[*start_of_token] && isspace((int)(string_so_far[*start_of_token])); (*start_of_token)++);
    for(*token_len = 0; string_so_far[*start_of_token + *token_len] && !isspace((int)(string_so_far[*start_of_token + *token_len])); (*token_len)++);
}

static gboolean is_token_in(const char *string_so_far, int token_len, const linenoiseCompletions *lc)
{
    int i;
    for(i = 0; i < lc->len; i++) {
        if(strlen(lc->cvec[i]) == token_len && !strncmp(string_so_far, lc->cvec[i], token_len)) {
            return TRUE;
        }
    }
    return FALSE;
}

static void monitor_completions(const char *string_so_far, linenoiseCompletions *lc)
{
    int start_of_token, token_len;
    char *help_commands[] = {"help", "?"};
    const linenoiseCompletions help_lc = {
         sizeof(help_commands)/sizeof(*help_commands),
         help_commands
    };

    find_next_token(string_so_far, 0, &start_of_token, &token_len);
    if (!string_so_far[start_of_token + token_len]) {
         fill_completions(string_so_far, start_of_token, token_len, &command_lc, lc);
         return;
    }
    if (is_token_in(string_so_far + start_of_token, token_len, &help_lc)) {
        find_next_token(string_so_far, start_of_token + token_len, &start_of_token, &token_len);
        if (!string_so_far[start_of_token + token_len]){
             fill_completions(string_so_far, start_of_token, token_len, &command_lc, lc);
             return;
        }
    }
    if (is_token_in(string_so_far + start_of_token, token_len, &need_filename_lc)) {
        int start_of_path;
        DIR* dir;
        struct dirent *direntry;
        struct linenoiseCompletions files_lc = {0, NULL};
        int i;

        for (start_of_token += token_len; string_so_far[start_of_token] && isspace((int)(string_so_far[start_of_token])); start_of_token++);
        if (string_so_far[start_of_token] != '"') {
            char *string_to_append = concat_strings(string_so_far, start_of_token, "\"");
            vte_linenoiseAddCompletion(lc, string_to_append);
            lib_free(string_to_append);
            return;
        }
        for (start_of_path = ++start_of_token, token_len = 0; string_so_far[start_of_token + token_len]; token_len++) {
            if(string_so_far[start_of_token + token_len] == '"'
            && string_so_far[start_of_token + token_len - 1] != '\\') {
                return;
            }
            if(string_so_far[start_of_token + token_len] == '/') {
                start_of_token += token_len + 1;
                token_len = -1;
            }
        }
        if (start_of_token == start_of_path) {
            dir = opendir(".");
        } else {
            char *path = concat_strings(string_so_far + start_of_path, start_of_token - start_of_path, "");
            dir = opendir(path);
            lib_free(path);
        }
        if (dir) {
            for (direntry = readdir(dir); direntry; direntry = readdir(dir)) {
                if (strcmp(direntry->d_name, ".") && strcmp(direntry->d_name, "..")) {
                    char *entryname = lib_msprintf("%s%s", direntry->d_name, is_dir(direntry) ? "/" : "\"");
                    vte_linenoiseAddCompletion(&files_lc, entryname);
                    lib_free(entryname);
                }
            }
            fill_completions(string_so_far, start_of_token, token_len, &files_lc, lc);
            for(i = 0; i < files_lc.len; i++) {
                free(files_lc.cvec[i]);
            }
            closedir(dir);
            return;
        }
    }
}

char *uimon_get_in(char **ppchCommandLine, const char *prompt)
{
    char *p, *ret_string;

    if (native_monitor()) {
        return uimonfb_get_in(ppchCommandLine, prompt);
    }

    pthread_mutex_lock(&fixed.lock);
    if (!fixed.input_buffer) {
        fixed.input_buffer = lib_strdup("");
    }
    pthread_mutex_unlock(&fixed.lock);

    vte_linenoiseSetCompletionCallback(monitor_completions);
    p = vte_linenoise(prompt, &fixed);
    if (p) {
        if (*p) {
            vte_linenoiseHistoryAdd(p);
        }
        ret_string = lib_strdup(p);
        free(p);
    } else {
        ret_string = NULL;
    }

    return ret_string;
}

int console_init(void)
{
    int i = 0;
    char *full_name;
    char *short_name;
    int takes_filename_as_arg;
    pthread_mutexattr_t lock_attributes;

    /* our console lock needs to be recursive */
    pthread_mutexattr_init(&lock_attributes);
    pthread_mutexattr_settype(&lock_attributes, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&fixed.lock, &lock_attributes);

    if (native_monitor()) {
        return consolefb_init();
    }

    while (mon_get_nth_command(i++,
                &full_name,
                &short_name,
                &takes_filename_as_arg)) {
        if (strlen(full_name)) {
            vte_linenoiseAddCompletion(&command_lc, full_name);
            if (strlen(short_name)) {
                vte_linenoiseAddCompletion(&command_lc, short_name);
            }
            if (takes_filename_as_arg) {
                vte_linenoiseAddCompletion(&need_filename_lc, full_name);
                if (strlen(short_name)) {
                    vte_linenoiseAddCompletion(&need_filename_lc, short_name);
                }
            }
        }
    }
    return 0;
}

int console_close_all(void)
{
    int i;

    pthread_mutex_lock(&fixed.lock);

    if (fixed.input_buffer) {
        /* This happens if the application exits with the monitor open, as the VICE thread
         * exits while the monitor is waiting for user input into this buffer.
         */
        lib_free(fixed.input_buffer);
        fixed.input_buffer = NULL;
    }

    pthread_mutex_unlock(&fixed.lock);

    if (native_monitor()) {
        return consolefb_close_all();
    }

    for(i = 0; i < command_lc.len; i++) {
        free(command_lc.cvec[i]);
    }
    for(i = 0; i < need_filename_lc.len; i++) {
        free(need_filename_lc.cvec[i]);
    }

    return 0;
}

/** \brief  Callback to activate the ML-monitor
 *
 * \param[in,out]   widget      widget triggering the event
 * \param[in]       user_data   data for the event (unused)
 *
 * \return  TRUE
 */
gboolean ui_monitor_activate_callback(GtkWidget *widget, gpointer user_data)
{
    int v;
    int native = 0;

    /*
     * Determine if we use the spawing terminal or the (yet to write) Gtk3
     * base monitor
     */
    resources_get_int("NativeMonitor", &native);
    resources_get_int("MonitorServer", &v);

    if (v == 0) {
        vsync_suspend_speed_eval();
        /* ui_autorepeat_on(); */

#ifdef HAVE_MOUSE
        /* FIXME: restore mouse in case it was grabbed */
        /* ui_restore_mouse(); */
#endif

        if (ui_pause_active()) {
            ui_pause_enter_monitor();
        } else {
            monitor_startup_trap();
        }
    }
    return TRUE;
}

