/*
 * uistatusbar.c - Native GTK3 UI statusbar stuff.
 *
 * Written by
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
 *  Michael C. Martin <mcmartin@gmail.com>
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

#include "not_implemented.h"

#include "vice_gtk3.h"
#include "datasette.h"
#include "drive.h"
#include "joyport.h"
#include "lib.h"
#include "machine.h"
#include "resources.h"
#include "types.h"
#include "uiapi.h"
#include "uidatasette.h"
#include "uidiskattach.h"
#include "uifliplist.h"
#include "userport/userport_joystick.h"

#include "uistatusbar.h"

#define MAX_STATUS_BARS 3

/* Global data that custom status bar widgets base their rendering
 * on. */
static struct ui_sb_state_s {
    intptr_t statustext_msgid;
    /* TODO: The PET can have 2 tape drives */
    int tape_control, tape_motor_status, tape_counter;
    /* TODO: does not cover two-unit drives */
    int drives_enabled, drives_tde_enabled;
    int drive_led_types[DRIVE_NUM];
    unsigned int current_drive_leds[DRIVE_NUM][2];
    int current_joyports[JOYPORT_MAX_PORTS];
    int joyports_enabled;
} sb_state;

typedef struct ui_statusbar_s {
    GtkWidget *bar;
    GtkLabel *msg;
    GtkWidget *tape, *tape_menu;
    GtkWidget *joysticks;
    GtkWidget *drives[DRIVE_NUM], *drive_popups[DRIVE_NUM];
    GdkCursor *hand_ptr;
} ui_statusbar_t;

static ui_statusbar_t allocated_bars[MAX_STATUS_BARS];

void ui_statusbar_init(void)
{
    int i, j;

    for (i = 0; i < MAX_STATUS_BARS; ++i) {
        allocated_bars[i].bar = NULL;
        allocated_bars[i].msg = NULL;
        allocated_bars[i].tape = NULL;
        allocated_bars[i].tape_menu = NULL;
        allocated_bars[i].joysticks = NULL;
        for (j = 0; j < DRIVE_NUM; ++j) {
            allocated_bars[i].drives[j] = NULL;
            allocated_bars[i].drive_popups[j] = NULL;
        }
        allocated_bars[i].hand_ptr = NULL;
    }

    sb_state.statustext_msgid = 0;
    sb_state.tape_control = 0;
    sb_state.tape_motor_status = 0;
    sb_state.tape_counter = 0;
    sb_state.drives_enabled = 0;
    sb_state.drives_tde_enabled = 0;
    for (i = 0; i < DRIVE_NUM; ++i) {
        sb_state.drive_led_types[i] = 0;
        sb_state.current_drive_leds[i][0] = 0;
        sb_state.current_drive_leds[i][1] = 0;
    }

    for (i = 0; i < JOYPORT_MAX_PORTS; ++i) {
        sb_state.current_joyports[i] = 0;
    }
    sb_state.joyports_enabled = 0;
}

void ui_statusbar_shutdown(void)
{
    /* Any universal resources we allocate get cleaned up here */
}

static int compute_drives_enabled_mask(void)
{
    int unit, mask;
    int result = 0;
    for (unit = 0, mask=1; unit < 4; ++unit, mask <<= 1) {
        int status = 0, value = 0;
        status = resources_get_int_sprintf("Drive%dType", &value, unit+8);
        if (status == 0 && value != 0) {
            result |= mask;
        }
    }
    return result;
}

static gboolean draw_tape_icon_cb(GtkWidget *widget, cairo_t *cr, gpointer data)
{
    int width, height;
    double x, y, inset;
    width = gtk_widget_get_allocated_width(widget);
    height = gtk_widget_get_allocated_height(widget);
    if (width > height) {
        x = (width - height) / 2.0;
        y = 0.0;
        inset = height / 10.0;
    } else {
        x = 0.0;
        y = (height - width) / 2.0;
        inset = width / 10.0;
    }

    if (sb_state.tape_motor_status) {
        cairo_set_source_rgb(cr, 0, 0.75, 0);
    } else {
        cairo_set_source_rgb(cr, 0.5, 0.5, 0.5);
    }
    cairo_rectangle(cr, x + inset, y + inset, inset * 8, inset * 8);
    cairo_fill(cr);

    cairo_set_source_rgb(cr, 0, 0, 0);
    switch (sb_state.tape_control) {
    case DATASETTE_CONTROL_STOP:
        cairo_rectangle(cr, x + 2.5*inset, y + 2.5*inset, inset * 5, inset * 5);
        cairo_fill(cr);
        break;
    case DATASETTE_CONTROL_START:
        cairo_move_to(cr, x + 3*inset, y + 2.5*inset);
        cairo_line_to(cr, x + 3*inset, y + 7.5*inset);
        cairo_line_to(cr, x + 7*inset, y + 5*inset);
        cairo_close_path(cr);
        cairo_fill(cr);
        break;
    case DATASETTE_CONTROL_FORWARD:
        cairo_move_to(cr, x + 2.5*inset, y + 2.5*inset);
        cairo_line_to(cr, x + 2.5*inset, y + 7.5*inset);
        cairo_line_to(cr, x + 5*inset, y + 5*inset);
        cairo_close_path(cr);
        cairo_fill(cr);
        cairo_move_to(cr, x + 5*inset, y + 2.5*inset);
        cairo_line_to(cr, x + 5*inset, y + 7.5*inset);
        cairo_line_to(cr, x + 7.5*inset, y + 5*inset);
        cairo_close_path(cr);
        cairo_fill(cr);
        break;
    case DATASETTE_CONTROL_REWIND:
        cairo_move_to(cr, x + 5*inset, y + 2.5*inset);
        cairo_line_to(cr, x + 5*inset, y + 7.5*inset);
        cairo_line_to(cr, x + 2.5*inset, y + 5*inset);
        cairo_close_path(cr);
        cairo_fill(cr);
        cairo_move_to(cr, x + 7.5*inset, y + 2.5*inset);
        cairo_line_to(cr, x + 7.5*inset, y + 7.5*inset);
        cairo_line_to(cr, x + 5*inset, y + 5*inset);
        cairo_close_path(cr);
        cairo_fill(cr);
        break;
    case DATASETTE_CONTROL_RECORD:
        cairo_new_sub_path(cr);
        cairo_arc(cr, x + 5*inset, y + 5*inset, 2.5*inset, 0, 2 * G_PI);
        cairo_close_path(cr);
        cairo_fill(cr);
        cairo_set_source_rgb(cr, 1, 0, 0);
        cairo_new_sub_path(cr);
        cairo_arc(cr, x + 5*inset, y + 5*inset, 2*inset, 0, 2 * G_PI);
        cairo_close_path(cr);
        cairo_fill(cr);
        break;
    case DATASETTE_CONTROL_RESET:
    case DATASETTE_CONTROL_RESET_COUNTER:
    default:
        /* Things that aren't really controls look like we stop it. */
        /* TODO: Should RESET_COUNTER be wiped out by the time it gets here? */
        cairo_rectangle(cr, x + 2.5*inset, y + 2.5*inset, inset * 5, inset * 5);
        cairo_fill(cr);
    }

    return FALSE;
}

static gboolean draw_drive_led_cb(GtkWidget *widget, cairo_t *cr, gpointer data)
{
    int width, height, drive, i;
    double red = 0.0, green = 0.0, x, y, w, h;

    width = gtk_widget_get_allocated_width(widget);
    height = gtk_widget_get_allocated_height(widget);
    drive = GPOINTER_TO_INT(data);
    for (i = 0; i < 2; ++i) {
        int led_color = sb_state.drive_led_types[drive] & (1 << i);
        if (led_color) {
            green += sb_state.current_drive_leds[drive][i] / 1000.0;
        } else {
            red += sb_state.current_drive_leds[drive][i] / 1000.0;
        }
    }
    /* Cairo clamps these for us */
    cairo_set_source_rgb(cr, red, green, 0);
    /* LED is half text height and aims for a 2x1 aspect ratio */
    h = height / 2.0;
    w = 2.0 * h;
    x = (width / 2.0) - h;
    y = height / 4.0;
    cairo_rectangle(cr, x, y, w, h);
    cairo_fill(cr);
    return FALSE;
}

static gboolean draw_joyport_cb(GtkWidget *widget, cairo_t *cr, gpointer data)
{
    int width, height, val;
    double e, s, x, y;

    width = gtk_widget_get_allocated_width(widget);
    height = gtk_widget_get_allocated_height(widget);
    val = sb_state.current_joyports[GPOINTER_TO_INT(data)];

    /* This widget "wants" to draw 6x6 squares inside a 20x20
     * space. We compute x and y offsets for a scaled square within
     * the real widget space, and then the actual widths for a square
     * edge (e) and the spaces between them (s). */

    if (width > height) {
        s = height / 20.0;
        x = (width - height) / 2.0;
        y = 0.0;
    } else {
        s = width / 20.0;
        y = (height - width) / 2.0;
        x = 0.0;
    }
    e = s * 5.0;

    /* Then we render the five squares. This seems like it could be
     * done more programatically, but enough changes each iteration
     * that we might as well unroll it. */

    /* Up: Bit 0 */
    cairo_set_source_rgb(cr, 0, (val&0x01) ? 1 : 0, 0);
    cairo_rectangle(cr, x + e + 2*s, y+s, e, e);
    cairo_fill(cr);
    /* Down: Bit 1 */
    cairo_set_source_rgb(cr, 0, (val&0x02) ? 1 : 0, 0);
    cairo_rectangle(cr, x + e + 2*s, y + 2*e + 3*s, e, e);
    cairo_fill(cr);
    /* Left: Bit 2 */
    cairo_set_source_rgb(cr, 0, (val&0x04) ? 1 : 0, 0);
    cairo_rectangle(cr, x + s, y + e + 2*s, e, e);
    cairo_fill(cr);
    /* Right: Bit 3 */
    cairo_set_source_rgb(cr, 0, (val&0x08) ? 1 : 0, 0);
    cairo_rectangle(cr, x + 2*e + 3*s, y + e + 2*s, e, e);
    cairo_fill(cr);
    /* Fire buttons: Bits 4-6. Each of the three notional fire buttons
     * controls the red, green, or blue color of the fire button
     * area. By default, we are using one-button joysticks and so this
     * region will be either black or red. */
    cairo_set_source_rgb(cr, (val&0x10) ? 1 : 0,
                             (val&0x20) ? 1 : 0,
                             (val&0x40) ? 1 : 0);
    cairo_rectangle(cr, x + e + 2*s, y + e + 2*s, e, e);
    cairo_fill(cr);
    return FALSE;
}

static GtkWidget *ui_drive_widget_create(int unit)
{
    GtkWidget *grid, *number, *track, *led;
    char drive_id[4];

    grid = gtk_grid_new();
    gtk_orientable_set_orientation(GTK_ORIENTABLE(grid), GTK_ORIENTATION_HORIZONTAL);
    gtk_widget_set_hexpand(grid, FALSE);

    snprintf(drive_id, 4, "%d:", unit+8);
    drive_id[3]=0;
    number = gtk_label_new(drive_id);
    gtk_widget_set_halign(number, GTK_ALIGN_START);

    track = gtk_label_new("18.5");
    gtk_widget_set_hexpand(track, TRUE);
    gtk_widget_set_halign(track, GTK_ALIGN_END);

    led = gtk_drawing_area_new();
    gtk_widget_set_size_request(led, 30, 15);
    gtk_widget_set_no_show_all(led, TRUE);
    gtk_container_add(GTK_CONTAINER(grid), number);
    gtk_container_add(GTK_CONTAINER(grid), track);
    gtk_container_add(GTK_CONTAINER(grid), led);
    /* Labels will notice clicks by default, but drawing areas need to
     * be told to. */
    gtk_widget_add_events(led, GDK_BUTTON_PRESS_MASK);
    g_signal_connect(led, "draw", G_CALLBACK(draw_drive_led_cb), GINT_TO_POINTER(unit));
    return grid;
}

static gboolean ui_do_datasette_popup(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    int i = GPOINTER_TO_INT(data);
    if (allocated_bars[i].tape && allocated_bars[i].tape_menu && event->type == GDK_BUTTON_PRESS) {
        /* 3.22 isn't available on the latest stable version of all
         * distros yet. This is expected to change when Ubuntu 18.04
         * is released. Since popup handling is the only thing 3.22
         * requires be done differently than its predecessors, this is
         * the only place we should rely on version checks. */
#if GTK_CHECK_VERSION(3,22,0)
        gtk_menu_popup_at_widget(GTK_MENU(allocated_bars[i].tape_menu),
                                 allocated_bars[i].tape,
                                 GDK_GRAVITY_NORTH_EAST,
                                 GDK_GRAVITY_SOUTH_EAST,
                                 event);
#else
        GdkEventButton *buttonEvent = (GdkEventButton *)event;
        gtk_menu_popup(GTK_MENU(allocated_bars[i].tape_menu),
                       NULL, NULL, NULL, NULL,
                       buttonEvent->button, buttonEvent->time);
#endif
    }
    return TRUE;
}

static gboolean ui_do_drive_popup(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    int i = GPOINTER_TO_INT(data);
    GtkWidget *drive_menu = allocated_bars[0].drive_popups[i];

    ui_populate_fliplist_menu(drive_menu, i+8, 0);
    gtk_widget_show_all(drive_menu);
    /* 3.22 isn't available on the latest stable version of all
     * distros yet. This is expected to change when Ubuntu 18.04 is
     * released. Since popup handling is the only thing 3.22 requires
     * be done differently than its predecessors, this is the only
     * place we should rely on version checks. */
#if GTK_CHECK_VERSION(3,22,0)
    gtk_menu_popup_at_widget(GTK_MENU(drive_menu),
                             widget,
                             GDK_GRAVITY_NORTH_EAST,
                             GDK_GRAVITY_SOUTH_EAST,
                             event);
#else
    {
        GdkEventButton *buttonEvent = (GdkEventButton *)event;

        gtk_menu_popup(GTK_MENU(drive_menu),
                       NULL, NULL, NULL, NULL,
                       buttonEvent->button, buttonEvent->time);
    }
#endif
    return TRUE;
}

static GtkWidget *ui_tape_widget_create(void)
{
    GtkWidget *grid, *header, *counter, *state;

    grid = gtk_grid_new();
    gtk_orientable_set_orientation(GTK_ORIENTABLE(grid), GTK_ORIENTATION_HORIZONTAL);
    gtk_widget_set_hexpand(grid, FALSE);
    header = gtk_label_new(_("Tape:"));
    gtk_widget_set_hexpand(header, TRUE);
    gtk_widget_set_halign(header, GTK_ALIGN_START);

    counter = gtk_label_new("000");
    state = gtk_drawing_area_new();
    gtk_widget_set_size_request(state, 20, 20);
    /* Labels will notice clicks by default, but drawing areas need to
     * be told to. */
    gtk_widget_add_events(state, GDK_BUTTON_PRESS_MASK);
    gtk_container_add(GTK_CONTAINER(grid), header);
    gtk_container_add(GTK_CONTAINER(grid), counter);
    gtk_container_add(GTK_CONTAINER(grid), state);
    g_signal_connect(state, "draw", G_CALLBACK(draw_tape_icon_cb), GINT_TO_POINTER(0));
    return grid;
}

static void vice_gtk3_update_joyport_layout(void)
{
    int i, ok[JOYPORT_MAX_PORTS];
    int userport_joysticks = 0;
    int new_joyport_mask = 0;
    /* Start with all ports enabled */
    for (i = 0; i < JOYPORT_MAX_PORTS; ++i) {
        ok[i] = 1;
    }
    /* Check for userport joystick counts */
    if ((machine_class != VICE_MACHINE_CBM5x0) &&
            (machine_class != VICE_MACHINE_VSID)) {
        int upjoy = 0;
        resources_get_int("UserportJoy", &upjoy);
        if (upjoy) {
            ++userport_joysticks;
        }
        if (machine_class != VICE_MACHINE_C64DTV) {
            int uptype = USERPORT_JOYSTICK_HUMMER;
            resources_get_int("UserportJoyType", &uptype);
            if ((uptype != USERPORT_JOYSTICK_HUMMER) &&
                (uptype != USERPORT_JOYSTICK_OEM) &&
                (upjoy != 0)) {
                ++userport_joysticks;
            }
        }

    }
    /* Port 1 disabled for machines that have no internal joystick
     * ports */
    if ((machine_class == VICE_MACHINE_CBM6x0) ||
        (machine_class == VICE_MACHINE_PET) ||
        (machine_class == VICE_MACHINE_VSID)) {
        ok[0] = 0;
    }
    /* Port 2 disabled for machines that have at most one internal
     * joystick ports */
    if ((machine_class == VICE_MACHINE_VIC20) ||
        (machine_class == VICE_MACHINE_CBM6x0) ||
        (machine_class == VICE_MACHINE_PET) ||
        (machine_class == VICE_MACHINE_VSID)) {
        ok[1] = 0;
    }
    /* Port 3 disabled for machines with no user port and no other
     * joystick adapter type, or where no userport joystick is
     * configured */
    if ((machine_class == VICE_MACHINE_CBM5x0) || (userport_joysticks < 1)) {
        ok[2] = 0;
    }
    /* Port 4 disabled for machines with no user port, or not enough
     * userport lines for 2 port userport adapters, or where at most
     * one userport joystick is configured */
    if ((machine_class == VICE_MACHINE_CBM5x0) ||
        (machine_class == VICE_MACHINE_C64DTV) ||
        (userport_joysticks < 2)) {
        ok[3] = 0;
    }
    /* Port 5 disabled for machines with no 5th control port,  */
    if (machine_class != VICE_MACHINE_PLUS4) {
        ok[4] = 0;
    } else {
        /* Port 5 also disabled if there's no SID joystick configured */
        int sidjoy = 0;
        resources_get_int("SIDCartJoy", &sidjoy);
        if (!sidjoy) {
            ok[4] = 0;
        }
    }
    /* Now that we have a list of disabled/enabled ports, let's check
     * to see if anything has changed */
    for (i = 0; i < JOYPORT_MAX_PORTS; ++i) {
        new_joyport_mask <<= 1;
        if (ok[i]) {
            new_joyport_mask |= 1;
        }
    }
    if (new_joyport_mask != sb_state.joyports_enabled) {
        int j;
        sb_state.joyports_enabled = new_joyport_mask;
        for (j = 0; j < MAX_STATUS_BARS; ++j) {
            GtkWidget *joyports_grid = allocated_bars[j].joysticks;
            if (!joyports_grid) {
                continue;
            }
            /* Hide and show the joystick ports as required */
            for (i = 0; i < JOYPORT_MAX_PORTS; ++i) {
                GtkWidget *child = gtk_grid_get_child_at(GTK_GRID(joyports_grid), 1+i, 0);
                if (child) {
                    if (ok[i]) {
                        gtk_widget_set_no_show_all(child, FALSE);
                        gtk_widget_show_all(child);
                    } else {
                        gtk_widget_set_no_show_all(child, TRUE);
                        gtk_widget_hide(child);
                    }
                }
            }
        }
    }
}

static GtkWidget *ui_joystick_widget_create(void)
{
    GtkWidget *grid, *label;
    int i;
    grid = gtk_grid_new();
    gtk_orientable_set_orientation(GTK_ORIENTABLE(grid), GTK_ORIENTATION_HORIZONTAL);
    gtk_widget_set_hexpand(grid, FALSE);
    label = gtk_label_new(_("Joysticks:"));
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_widget_set_hexpand(label, TRUE);
    gtk_container_add(GTK_CONTAINER(grid), label);
    /* Create all possible joystick displays */
    for (i = 0; i < JOYPORT_MAX_PORTS; ++i) {
        GtkWidget *joyport = gtk_drawing_area_new();
        gtk_widget_set_size_request(joyport,20,20);
        gtk_container_add(GTK_CONTAINER(grid), joyport);
        g_signal_connect(joyport, "draw", G_CALLBACK(draw_joyport_cb), GINT_TO_POINTER(i));
        gtk_widget_set_no_show_all(joyport, TRUE);
        gtk_widget_hide(joyport);
    }
    return grid;
}

static gboolean ui_statusbar_cross_cb(GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
    ui_statusbar_t *sb = (ui_statusbar_t *)user_data;
    if (event && event->type == GDK_ENTER_NOTIFY) {
        GdkDisplay *display;
        /* Sanity check arguments */
        if (sb == NULL) {
            /* Should be impossible */
            return FALSE;
        }
        /* If the "hand" pointer hasn't been created yet, create it */
        display = gtk_widget_get_display(widget);
        if (display != NULL && sb->hand_ptr == NULL) {
            sb->hand_ptr = gdk_cursor_new_from_name(display, "pointer");
            if (sb->hand_ptr != NULL) {
                g_object_ref_sink(G_OBJECT(sb->hand_ptr));
            } else {
                fprintf(stderr, "GTK3 CURSOR: Could not allocate custom pointer for status bar\n");
            }
        }
        /* If the "hand" pointer is OK, use it */
        if (sb->hand_ptr != NULL) {
            GdkWindow *window = gtk_widget_get_window(widget);
            if (window) {
                gdk_window_set_cursor(window, sb->hand_ptr);
            }
        }
    } else {
        /* We're leaving the target widget, so change the pointer back
         * to default */
        GdkWindow *window = gtk_widget_get_window(widget);
        if (window) {
            gdk_window_set_cursor(window, NULL);
        }
    }
    return FALSE;
}

static void layout_statusbar_drives(int bar_index)
{
    int i, j, state, tde = 0;
    int enabled_drive_index = 0;
    GtkWidget *bar = allocated_bars[bar_index].bar;
    if (!bar) {
        return;
    }
    /* Delete all the drives and dividers that may exist. WARNING:
     * This code assumes that the drive widgets are the rightmost
     * elements of the status bar. */
    for (i = 0; i < ((DRIVE_NUM + 1) / 2) * 2; ++i) {
        for (j = 0; j < 2; ++j) {
            GtkWidget *child = gtk_grid_get_child_at(GTK_GRID(bar), 3+i, j);
            if (child) {
                /* Fun GTK3 fact! If you destroy an event box, then
                 * even if the thing it contains still has references
                 * left, that child is destroyed _anyway_. To avoid
                 * this tragic eventuality, we detach the child files
                 * before removing the box. */
                /* TODO: This implies that we really should not be
                 * relying on g_object_ref to preserve objects, and
                 * instead keep track of widget indices the hard
                 * way. */
                if (GTK_IS_EVENT_BOX(child)) {
                    GtkWidget *grandchild = gtk_bin_get_child(GTK_BIN(child));
                    gtk_container_remove(GTK_CONTAINER(child), grandchild);
                }
                gtk_container_remove(GTK_CONTAINER(bar), child);
            }
        }
    }
    state = sb_state.drives_enabled;
    tde = sb_state.drives_tde_enabled;
    for (i = 0; i < DRIVE_NUM; ++i) {
        if (state & 1) {
            GtkWidget *drive = allocated_bars[bar_index].drives[i];
            GtkWidget *event_box = gtk_event_box_new();
            int row = enabled_drive_index % 2;
            int column = (enabled_drive_index / 2) * 2 + 4;
            if (row == 0) {
                gtk_grid_attach(GTK_GRID(bar), gtk_separator_new(GTK_ORIENTATION_VERTICAL), column-1, 0, 1, 2);
            }
            gtk_container_add(GTK_CONTAINER(event_box), drive);
            gtk_event_box_set_visible_window(GTK_EVENT_BOX(event_box), FALSE);
            g_signal_connect(event_box, "button-press-event", G_CALLBACK(ui_do_drive_popup), GINT_TO_POINTER(i));
            g_signal_connect(event_box, "enter-notify-event", G_CALLBACK(ui_statusbar_cross_cb), &allocated_bars[i]);
            g_signal_connect(event_box, "leave-notify-event", G_CALLBACK(ui_statusbar_cross_cb), &allocated_bars[i]);
            gtk_widget_show_all(event_box);
            if (tde & 1) {
                gtk_widget_show(gtk_grid_get_child_at(GTK_GRID(drive), 2, 0));
            } else {
                gtk_widget_hide(gtk_grid_get_child_at(GTK_GRID(drive), 2, 0));
            }
            gtk_grid_attach(GTK_GRID(bar), event_box, column, row, 1, 1);
            ++enabled_drive_index;
        }
        state >>= 1;
        tde >>= 1;
    }
    gtk_widget_show_all(bar);
}

static void destroy_statusbar_cb(GtkWidget *sb, gpointer ignored)
{
    int i, j;

    for (i = 0; i < MAX_STATUS_BARS; ++i) {
        if (allocated_bars[i].bar == sb) {
            allocated_bars[i].bar = NULL;
            if (allocated_bars[i].msg) {
                g_object_unref(G_OBJECT(allocated_bars[i].msg));
                allocated_bars[i].msg = NULL;
            }
            if (allocated_bars[i].tape) {
                g_object_unref(G_OBJECT(allocated_bars[i].tape));
                allocated_bars[i].tape = NULL;
            }
            if (allocated_bars[i].tape_menu) {
                g_object_unref(G_OBJECT(allocated_bars[i].tape_menu));
                allocated_bars[i].tape_menu = NULL;
            }
            if (allocated_bars[i].joysticks) {
                g_object_unref(G_OBJECT(allocated_bars[i].joysticks));
                allocated_bars[i].joysticks = NULL;
            }
            for (j = 0; j < DRIVE_NUM; ++j) {
                if (allocated_bars[i].drives[j]) {
                    g_object_unref(G_OBJECT(allocated_bars[i].drives[j]));
                    g_object_unref(G_OBJECT(allocated_bars[i].drive_popups[j]));
                    allocated_bars[i].drives[j] = NULL;
                    allocated_bars[i].drive_popups[j] = NULL;
                }
            }
            if (allocated_bars[i].hand_ptr) {
                g_object_unref(G_OBJECT(allocated_bars[i].hand_ptr));
                allocated_bars[i].hand_ptr = NULL;
            }
        }
    }
}

static GtkWidget *ui_drive_menu_create(int unit)
{
    char buf[128];
    GtkWidget *drive_menu = gtk_menu_new();
    GtkWidget *drive_menu_item;
    snprintf(buf, 128, _("Attach to drive #%d..."), unit + 8);
    buf[127] = 0;
    drive_menu_item = gtk_menu_item_new_with_label(buf);
    g_signal_connect(drive_menu_item, "activate", G_CALLBACK(ui_disk_attach_callback), GINT_TO_POINTER(unit+8));
    gtk_container_add(GTK_CONTAINER(drive_menu), drive_menu_item);
    snprintf(buf, 128, _("Detach disk from drive #%d"), unit + 8);
    buf[127] = 0;
    drive_menu_item = gtk_menu_item_new_with_label(buf);
    g_signal_connect(drive_menu_item, "activate", G_CALLBACK(ui_disk_detach_callback), GINT_TO_POINTER(unit+8));
    gtk_container_add(GTK_CONTAINER(drive_menu), drive_menu_item);
    /* GTK2/GNOME UI put TDE and Read-only checkboxes here, but that
     * seems excessive or possibly too fine-grained, so skip that for
     * now */
    ui_populate_fliplist_menu(drive_menu, unit+8, 0);
    gtk_widget_show_all(drive_menu);
    return drive_menu;
}

GtkWidget *ui_statusbar_create(void)
{
    GtkWidget *sb, *msg, *tape, *tape_events, *joysticks;
    int i, j;

    for (i = 0; i < MAX_STATUS_BARS; ++i) {
        if (allocated_bars[i].bar == NULL) {
            break;
        }
    }

    if (i >= MAX_STATUS_BARS) {
        /* Fatal error? */
        return NULL;
    }

    /* While the status bar itself is returned floating, we sink all
     * of its information-bearing subwidgets. This is so that we can
     * remove or add them to the status bar as the configuration
     * demands, while ensuring they remain alive. They receive an
     * extra dereference in ui_statusbar_destroy() so nothing should
     * leak. */
    sb = vice_gtk3_grid_new_spaced(8, 0);
    /* First column: messages */
    msg = gtk_label_new(NULL);
    g_object_ref_sink(G_OBJECT(msg));
    gtk_widget_set_halign(msg, GTK_ALIGN_START);
    gtk_widget_set_hexpand(msg, TRUE);
    gtk_label_set_ellipsize(GTK_LABEL(msg), PANGO_ELLIPSIZE_END);

    g_signal_connect(sb, "destroy", G_CALLBACK(destroy_statusbar_cb), NULL);
    allocated_bars[i].bar = sb;
    allocated_bars[i].msg = GTK_LABEL(msg);
    gtk_grid_attach(GTK_GRID(sb), msg, 0, 0, 1, 2);
    /* Second column: Tape and joysticks */
    gtk_grid_attach(GTK_GRID(sb), gtk_separator_new(GTK_ORIENTATION_VERTICAL), 1, 0, 1, 2);

    if ((machine_class != VICE_MACHINE_C64DTV)
            && (machine_class != VICE_MACHINE_VSID)) {
        tape = ui_tape_widget_create();
        g_object_ref_sink(G_OBJECT(tape));
        /* Clicking the tape status is supposed to pop up a window. This
         * requires a way to make sure events are captured by random
         * internal widgets; the GtkEventBox manages that task for us. */
        tape_events = gtk_event_box_new();
        gtk_event_box_set_visible_window(GTK_EVENT_BOX(tape_events), FALSE);
        gtk_container_add(GTK_CONTAINER(tape_events), tape);
        gtk_grid_attach(GTK_GRID(sb), tape_events, 2, 0, 1, 1);
        allocated_bars[i].tape = tape;
        allocated_bars[i].tape_menu = ui_create_datasette_control_menu();
        g_object_ref_sink(G_OBJECT(allocated_bars[i].tape_menu));
        g_signal_connect(tape_events, "button-press-event",
                G_CALLBACK(ui_do_datasette_popup), GINT_TO_POINTER(i));
        g_signal_connect(tape_events, "enter-notify-event",
                G_CALLBACK(ui_statusbar_cross_cb), &allocated_bars[i]);
        g_signal_connect(tape_events, "leave-notify-event",
                G_CALLBACK(ui_statusbar_cross_cb), &allocated_bars[i]);
    }

    if (machine_class != VICE_MACHINE_VSID) {
        joysticks = ui_joystick_widget_create();
        g_object_ref(joysticks);
        gtk_widget_set_halign(joysticks, GTK_ALIGN_END);
        gtk_grid_attach(GTK_GRID(sb), joysticks, 2, 1, 1, 1);
        allocated_bars[i].joysticks = joysticks;
    }

    /* Third column on: Drives. */
    for (j = 0; j < DRIVE_NUM; ++j) {
        GtkWidget *drive = ui_drive_widget_create(j);
        GtkWidget *drive_menu = ui_drive_menu_create(j);
        g_object_ref_sink(G_OBJECT(drive));
        g_object_ref_sink(G_OBJECT(drive_menu));
        allocated_bars[i].drives[j] = drive;
        allocated_bars[i].drive_popups[j] = drive_menu;
    }
    /* WARNING: The current implementation of ui_enable_drive_status()
     * relies on the fact that the drives are the last elements of the
     * statusbar display. If more widgets are added past this point,
     * that function will need to change as well. */
    layout_statusbar_drives(i);

    /* Set an impossible number of joyports to enabled so that the status
     * is guarenteed to be updated. */
    sb_state.joyports_enabled = ~0;
    vice_gtk3_update_joyport_layout();
    return sb;
}

void ui_display_event_time(unsigned int current, unsigned int total)
{
    NOT_IMPLEMENTED_WARN_ONLY();
}

void ui_display_playback(int playback_status, char *version)
{
    NOT_IMPLEMENTED_WARN_ONLY();
}

void ui_display_recording(int recording_status)
{
    NOT_IMPLEMENTED_WARN_ONLY();
}

static void
display_statustext_internal(const char *text)
{
    int i;
    for (i = 0; i < MAX_STATUS_BARS; ++i) {
        if (allocated_bars[i].msg) {
            gtk_label_set_text(allocated_bars[i].msg, text);
        }
    }
}

static gboolean ui_statustext_fadeout(gpointer data)
{
    intptr_t my_id = GPOINTER_TO_INT(data);
    if (my_id == sb_state.statustext_msgid) {
        display_statustext_internal("");
    }
    return FALSE;
}

void ui_display_statustext(const char *text, int fade_out)
{
    ++sb_state.statustext_msgid;
    display_statustext_internal(text);
    if (fade_out) {
        g_timeout_add(5000, ui_statustext_fadeout, GINT_TO_POINTER(sb_state.statustext_msgid));
    }
}

void ui_display_volume(int vol)
{
    NOT_IMPLEMENTED_WARN_ONLY();
}

void ui_display_joyport(uint8_t *joyport)
{
    int i;
    for (i = 0; i < JOYPORT_MAX_PORTS; ++i) {
        /* Compare the new value to the current one, set the new
         * value, and queue a redraw if and only if there was a
         * change. And yes, the input joystick ports are 1-indexed. I
         * don't know either. */
        if (sb_state.current_joyports[i] != joyport[i+1]) {
            int j;
            sb_state.current_joyports[i] = joyport[i+1];
            for (j = 0; j < MAX_STATUS_BARS; ++j) {
                if (allocated_bars[j].joysticks) {
                    GtkWidget *widget = gtk_grid_get_child_at(GTK_GRID(allocated_bars[j].joysticks), i+1, 0);
                    if (widget) {
                        gtk_widget_queue_draw(widget);
                    }
                }
            }
        }
    }
    /* Restrict visible joystick display to just the ones the
     * configuration supports */
    vice_gtk3_update_joyport_layout();
}

/* TODO: status display for TAPE emulation
 *
 * NOTE: newly written GUI code should be able to handle TWO independent tape
 *       drives. the PET emulation may make use of it some day.
 */

void ui_display_tape_control_status(int control)
{
    if (control != sb_state.tape_control) {
        int i;
        sb_state.tape_control = control;
        for (i = 0; i < MAX_STATUS_BARS; ++i) {
            if (allocated_bars[i].tape) {
                GtkWidget *widget = gtk_grid_get_child_at(GTK_GRID(allocated_bars[i].tape), 2, 0);
                if (widget) {
                    gtk_widget_queue_draw(widget);
                }
            }
        }
    }
}

void ui_display_tape_counter(int counter)
{
    if (counter != sb_state.tape_counter) {
        int i;
        char buf[8];
        snprintf(buf, 8, "%03d", counter%1000);
        buf[7] = 0;
        sb_state.tape_counter = counter;
        for (i = 0; i < MAX_STATUS_BARS; ++i) {
            if (allocated_bars[i].tape) {
                GtkWidget *widget = gtk_grid_get_child_at(GTK_GRID(allocated_bars[i].tape), 1, 0);
                if (widget) {
                    gtk_label_set_text(GTK_LABEL(widget), buf);
                }
            }
        }
    }
}

void ui_display_tape_motor_status(int motor)
{
    if (motor != sb_state.tape_motor_status) {
        int i;
        sb_state.tape_motor_status = motor;
        for (i = 0; i < MAX_STATUS_BARS; ++i) {
            if (allocated_bars[i].tape) {
                GtkWidget *widget = gtk_grid_get_child_at(GTK_GRID(allocated_bars[i].tape), 2, 0);
                if (widget) {
                    gtk_widget_queue_draw(widget);
                }
            }
        }
    }
}

void ui_set_tape_status(int tape_status)
{
    /* TODO: What does this even mean? The other GUIs don't represent it */
    /* printf("TAPE DRIVE STATUS: %d\n", tape_status); */
}

void ui_display_tape_current_image(const char *image)
{
    char buf[256];
    if (image && *image) {
        snprintf(buf, 256, _("Attached %s to tape unit"), image);
    } else {
        snprintf(buf, 256, _("Tape unit is empty"));
    }

    buf[255]=0;
    ui_display_statustext(buf, 1);
}

/* TODO: status display for DRIVE emulation
 *
 * NOTE: newly written GUI code should be able to use 4 drives, of which each
 *       can be a dual disk drive. (so it must handle 8 images total). currently
 *       the code does not make use of it yet, but it will in the future.
 */
void ui_display_drive_led(int drive_number, unsigned int pwm1, unsigned int led_pwm2)
{
    int i;
    if (drive_number < 0 || drive_number > DRIVE_NUM-1) {
        /* TODO: Fatal error? */
        return;
    }
    sb_state.current_drive_leds[drive_number][0] = pwm1;
    sb_state.current_drive_leds[drive_number][1] = led_pwm2;
    for (i = 0; i < MAX_STATUS_BARS; ++i) {
        if (allocated_bars[i].bar) {
            GtkWidget *drive, *led;
            drive = allocated_bars[i].drives[drive_number];
            led = gtk_grid_get_child_at(GTK_GRID(drive), 2, 0);
            if (led) {
                gtk_widget_queue_draw(led);
            }
        }
    }
}

void ui_display_drive_track(unsigned int drive_number,
                            unsigned int drive_base,
                            unsigned int half_track_number)
{
    int i;
    if (drive_number > DRIVE_NUM-1) {
        /* TODO: Fatal error? */
        return;
    }
    for (i = 0; i < MAX_STATUS_BARS; ++i) {
        if (allocated_bars[i].bar) {
            GtkWidget *drive, *track;
            drive = allocated_bars[i].drives[drive_number];
            track = gtk_grid_get_child_at(GTK_GRID(drive), 1, 0);
            if (track) {
                char track_str[16];
                snprintf(track_str, 16, "%.1lf", half_track_number / 2.0);
                track_str[15] = 0;
                gtk_label_set_text(GTK_LABEL(track), track_str);
            }
        }
    }
}

void ui_enable_drive_status(ui_drive_enable_t state, int *drive_led_color)
{
    int i, enabled;

    /* Update the drive LEDs first, unconditionally. If the drive is
     * *actually on* while this is called, the UI will not reflect the
     * LED type change until the next time the led's values are
     * updated. This should not happen under normal circumstances. */
    enabled = state;
    for (i = 0; i < DRIVE_NUM; ++i) {
        if (enabled & 1) {
            sb_state.drive_led_types[i] = drive_led_color[i];
            sb_state.current_drive_leds[i][0] = 0;
            sb_state.current_drive_leds[i][1] = 0;
        }
        enabled >>= 1;
    }

    /* Now give enabled its "real" value based on the drive
     * definitions. */
    enabled = compute_drives_enabled_mask();

    /* Now, if necessary, update the status bar layouts. We won't need
     * to do this if the only change was the kind of drives hooked up,
     * instead of the number */
    if ((state != sb_state.drives_tde_enabled) || (enabled != sb_state.drives_enabled)) {
        sb_state.drives_enabled = enabled;
        sb_state.drives_tde_enabled = state;
        for (i = 0; i < MAX_STATUS_BARS; ++i) {
            layout_statusbar_drives(i);
        }
    }
}

void ui_display_drive_current_image(unsigned int drive_number, const char *image)
{
    char buf[256];
    if (image && *image) {
        snprintf(buf, 256, _("Attached %s to unit %d"), image, drive_number+8);
    } else {
        snprintf(buf, 256, _("Unit %d is empty"), drive_number+8);
    }
    buf[255]=0;
    ui_display_statustext(buf, 1);
}
