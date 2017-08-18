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

#include "types.h"
#include "uiapi.h"

#include "uistatusbar.h"

/*
 * Counters for warnings issued, the below counters keep track of warnings
 * that get issued many times due to vsync and other events.
 *
 * These vars are used in NOT_IMPLEMENTED_WARN_X_TIMES() macro calls.
 */
static int led_msgs = 0;
static int track_msgs = 0;
static int tape_status_msgs = 0;
static int tape_counter_msgs = 0;
static int tape_motor_msgs = 0;
static int joyport_msgs = 0;

#define MAX_STATUS_BARS 3

/* TODO: Multiple LEDs, or possibly no LEDs for the non-TDE case */
typedef struct ui_statusbar_drive_s {
    GtkLabel *number, *track;
    GtkDrawingArea *led;
} ui_statusbar_drive_t;

typedef struct ui_statusbar_s {
    GtkWidget *bar;
    GtkLabel *msg;
    /* TODO: Tape, Joystick */
    ui_statusbar_drive_t drives[4];
} ui_statusbar_t;

static ui_statusbar_t allocated_bars[MAX_STATUS_BARS];

void ui_statusbar_init(void)
{
    int i, j;

    for (i = 0; i < MAX_STATUS_BARS; ++i) {
        allocated_bars[i].bar = NULL;
        allocated_bars[i].msg = NULL;
        for (j = 0; j < 4; ++j) {
            allocated_bars[i].drives[j].number = NULL;
            allocated_bars[i].drives[j].track = NULL;
            allocated_bars[i].drives[j].led = NULL;
        }
    }
}

static GtkWidget *ui_drive_widget_create(void)
{
    GtkWidget *grid, *number, *track, *led;
    grid = gtk_grid_new();
    gtk_orientable_set_orientation(GTK_ORIENTABLE(grid), GTK_ORIENTATION_HORIZONTAL);
    /* TODO: Fit this into the general status bar creation code, and
     *       keep them all hidden at first. */
    number = gtk_label_new(NULL);
    track = gtk_label_new(NULL);
    led = gtk_drawing_area_new();
    gtk_container_add(GTK_CONTAINER(grid), number);
    gtk_container_add(GTK_CONTAINER(grid), track);
    gtk_container_add(GTK_CONTAINER(grid), led);
    /* TODO: LED drawing code. Color is imported from setup, and the
     * LED itself was 45*16 on Win32, and much shorter on GTK2. */
    return grid;
}
    

static void destroy_statusbar_cb(GtkWidget *sb, gpointer ignored)
{
    int i, j;

    for (i = 0; i < MAX_STATUS_BARS; ++i) {
        if (allocated_bars[i].bar == sb) {
            allocated_bars[i].bar = NULL;
            allocated_bars[i].msg = NULL;
            for (j = 0; j < 4; ++j) {
                allocated_bars[i].drives[j].number = NULL;
                allocated_bars[i].drives[j].track = NULL;
                allocated_bars[i].drives[j].led = NULL;
            }
        }
    }
}

GtkWidget *ui_statusbar_create(void)
{
    GtkWidget *sb;
    int i;

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

    /* TODO: Create a real bar */
    sb = gtk_label_new(NULL);
    g_signal_connect(sb, "destroy", G_CALLBACK(destroy_statusbar_cb), NULL);
    allocated_bars[i].bar = sb;
    allocated_bars[i].msg = GTK_LABEL(sb);

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
    printf ("%d: [%u/%u]\n", drive_number, pwm1, led_pwm2);
    NOT_IMPLEMENTED_WARN_X_TIMES(led_msgs, 3);
}

void ui_display_drive_track(unsigned int drive_number,
                            unsigned int drive_base,
                            unsigned int half_track_number)
{
    printf("%u/%u: %.1lf\n", drive_base, drive_number, half_track_number / 2.0);
    NOT_IMPLEMENTED_WARN_X_TIMES(track_msgs, 3);
}

void ui_enable_drive_status(ui_drive_enable_t state, int *drive_led_color)
{
    NOT_IMPLEMENTED_WARN_ONLY();
}

void ui_display_drive_current_image(unsigned int drive_number, const char *image)
{
    NOT_IMPLEMENTED_WARN_ONLY();
}

