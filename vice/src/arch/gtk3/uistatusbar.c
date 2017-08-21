/*
 * uistatusbar.c - Native GTK3 UI statusbar stuff.
 *
 * Written by
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
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

#include "lib.h"
#include "types.h"
#include "uiapi.h"

#include "uistatusbar.h"

/*
 * Counters for warnings issued, the below counters keep track of warnings
 * that get issued many times due to vsync and other events.
 *
 * These vars are used in NOT_IMPLEMENTED_WARN_X_TIMES() macro calls.
 */
static int tape_status_msgs = 0;
static int tape_counter_msgs = 0;
static int tape_motor_msgs = 0;
static int joyport_msgs = 0;

#define MAX_STATUS_BARS 3

/* Global data that custom status bar widgets base their rendering
 * on. */
static struct ui_sb_state_s {
    /* TODO: does not cover two-unit drives */
    int drive_led_types[4];
    unsigned int current_drive_leds[4][2];
} sb_state;

typedef struct ui_statusbar_s {
    GtkWidget *bar;
    GtkLabel *msg;
    /* TODO: Tape, Joystick */
    GtkWidget *drives[4];
} ui_statusbar_t;

static ui_statusbar_t allocated_bars[MAX_STATUS_BARS];

void ui_statusbar_init(void)
{
    int i, j;

    for (i = 0; i < MAX_STATUS_BARS; ++i) {
        allocated_bars[i].bar = NULL;
        allocated_bars[i].msg = NULL;
        for (j = 0; j < 4; ++j) {
            allocated_bars[i].drives[j] = NULL;
        }
    }

    for (i = 0; i < 4; ++i) {
        sb_state.drive_led_types[i] = 0;
        sb_state.current_drive_leds[i][0] = 0;
        sb_state.current_drive_leds[i][1] = 0;
    }
}

void ui_statusbar_shutdown(void)
{
    /* Any universal resources we allocate get cleaned up here */
}

static gboolean draw_drive_led_cb(GtkWidget *widget, cairo_t *cr, gpointer data)
{
    int width, height, drive, i;
    double red = 0.0, green = 0.0, x, y, w, h;

    width = gtk_widget_get_allocated_width(widget);
    height = gtk_widget_get_allocated_height(widget);
    drive = *((int *)data);
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

static void destroy_drive_led_cb(GtkWidget *drive, gpointer data)
{
    lib_free(data);
}

static GtkWidget *ui_drive_widget_create(int unit)
{
    GtkWidget *grid, *number, *track, *led;
    char drive_id[4];
    int *drive_state;
    
    grid = gtk_grid_new();
    gtk_orientable_set_orientation(GTK_ORIENTABLE(grid), GTK_ORIENTATION_HORIZONTAL);
    snprintf(drive_id, 4, "%d:", unit+8);
    drive_id[3]=0;
    number = gtk_label_new(drive_id);
    track = gtk_label_new("18.5");
    led = gtk_drawing_area_new();
    gtk_widget_set_size_request(led, 30, 15);
    gtk_container_add(GTK_CONTAINER(grid), number);
    gtk_container_add(GTK_CONTAINER(grid), track);
    gtk_container_add(GTK_CONTAINER(grid), led);
    drive_state = lib_malloc(sizeof(int));
    *drive_state = unit;
    g_signal_connect(led, "destroy", G_CALLBACK(destroy_drive_led_cb), drive_state);
    g_signal_connect(led, "draw", G_CALLBACK(draw_drive_led_cb), drive_state);
    return grid;
}

static void destroy_statusbar_cb(GtkWidget *sb, gpointer ignored)
{
    int i, j;

    /* TODO: Unreference all widgets to allow collection. */
    for (i = 0; i < MAX_STATUS_BARS; ++i) {
        if (allocated_bars[i].bar == sb) {
            allocated_bars[i].bar = NULL;
            allocated_bars[i].msg = NULL;
            for (j = 0; j < 4; ++j) {
                allocated_bars[i].drives[j] = NULL;
            }
        }
    }
}

GtkWidget *ui_statusbar_create(void)
{
    GtkWidget *sb, *msg;
    int i, j;

    TEMPORARY_IMPLEMENTATION();

    for (i = 0; i < MAX_STATUS_BARS; ++i) {
        if (allocated_bars[i].bar == NULL) {
            break;
        }
    }

    if (i >= MAX_STATUS_BARS) {
        /* Fatal error? */
        return NULL;
    }

    /* TODO: Create all elements independently and keep them
     *       referenced by this code, even when not part of any
     *       immediate display. Procedurally lay out all active
     *       devices based on system status. */
    sb = gtk_grid_new();
    msg = gtk_label_new(NULL);
    gtk_label_set_xalign(GTK_LABEL(msg), 0.0);
    gtk_widget_set_hexpand(msg, TRUE);
    g_signal_connect(sb, "destroy", G_CALLBACK(destroy_statusbar_cb), NULL);
    allocated_bars[i].bar = sb;
    allocated_bars[i].msg = GTK_LABEL(msg);
    gtk_grid_attach(GTK_GRID(sb), msg, 0, 0, 1, 2);
    for (j = 0; j < 4; ++j) {
        GtkWidget *drive = ui_drive_widget_create(j);
        int row = j % 2;
        int column = (j / 2) * 2 + 2;
        if (row == 0) {
            gtk_grid_attach(GTK_GRID(sb), gtk_separator_new(GTK_ORIENTATION_VERTICAL), column-1, 0, 1, 2);
        }
        gtk_grid_attach(GTK_GRID(sb), drive, column, row, 1, 1);
        allocated_bars[i].drives[j] = drive;
    }

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

void ui_display_statustext(const char *text, int fade_out)
{
    /* TODO: handle fade out properly. This may work better with a
     *       GtkStatusBar and its per-message revocation methods, but
     *       based on other implementations, we're much better off
     *       pushing graphical updates to another function and using
     *       this to set up a timer fed by the speed display */
    int i;
    for (i = 0; i < MAX_STATUS_BARS; ++i) {
        if (allocated_bars[i].msg) {
            gtk_label_set_text(allocated_bars[i].msg, text);
        }
    }

    TEMPORARY_IMPLEMENTATION();
}

void ui_display_volume(int vol)
{
    NOT_IMPLEMENTED_WARN_ONLY();
}

/* TODO: status display for JOYSTICK emulation
 * 
 * NOTE: newly written GUI code should be able to handle EIGHT independent
 *       joysticks, with 3 buttons each.
 */

void ui_display_joyport(BYTE *joyport)
{
    /* Yes, joystick ports are 1-indexed. I don't know either. */
    /* printf("Joysticks: %02X %02X\n", joyport[1], joyport[2]); */
    NOT_IMPLEMENTED_WARN_X_TIMES(joyport_msgs, 3);
}

/* TODO: status display for TAPE emulation
 * 
 * NOTE: newly written GUI code should be able to handle TWO independent tape
 *       drives. the PET emulation may make use of it some day.
 */

void ui_display_tape_control_status(int control)
{
    NOT_IMPLEMENTED_WARN_ONLY();
}

void ui_display_tape_counter(int counter)
{
    NOT_IMPLEMENTED_WARN_X_TIMES(tape_counter_msgs, 3);
}

void ui_display_tape_motor_status(int motor)
{
    NOT_IMPLEMENTED_WARN_X_TIMES(tape_motor_msgs, 3);
}

void ui_set_tape_status(int tape_status)
{
    NOT_IMPLEMENTED_WARN_X_TIMES(tape_status_msgs, 3);
}

void ui_display_tape_current_image(const char *image)
{
    NOT_IMPLEMENTED_WARN_ONLY();
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
    if (drive_number < 0 || drive_number > 3) {
        /* TODO: Fatal error? */
        return;
    }
    sb_state.current_drive_leds[drive_number][0] = pwm1;
    sb_state.current_drive_leds[drive_number][1] = led_pwm2;
    for (i = 0; i < MAX_STATUS_BARS; ++i) {
        if (allocated_bars[i].bar) {
            GtkWidget *drive, *led;
            drive = allocated_bars[i].drives[i];
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
    int i, unit = drive_base-8;
    if (unit < 0 || unit > 3) {
        /* TODO: Fatal error? */
        return;
    }
    for (i = 0; i < MAX_STATUS_BARS; ++i) {
        if (allocated_bars[i].bar) {
            GtkWidget *drive, *track;
            drive = allocated_bars[i].drives[i];
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
    int i;
    /* 4 is based on drive/drive.h's DRIVE_NUM, but drive.h can't be
     * included here for some reason */
    /* TODO: Fix that */
    for (i = 0; i < 4; ++i) {
        if (state & 1) {
            sb_state.drive_led_types[i] = drive_led_color[i];
            sb_state.current_drive_leds[i][0] = 0;
            sb_state.current_drive_leds[i][1] = 0;
        }
        state >>= 1;
    }
    TEMPORARY_IMPLEMENTATION();
}

void ui_display_drive_current_image(unsigned int drive_number, const char *image)
{
    char buf[256];
    snprintf(buf, 256, _("Attached %s to unit %d"), image, drive_number+8);
    buf[255]=0;    
    ui_display_statustext(buf, 1);    
}

