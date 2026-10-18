/*
 * mon_input.c - The VICE built-in monitor, keyboard matrix and joystick
 *               input injection.
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

/* Keys and joystick bits set here bypass the host input path entirely: they
   are kept in separate overlays in keyboard.c and joystick.c, so they work
   for software that scans the matrix itself (unlike "keybuf") and survive
   the UI clearing the host input state on focus changes.

   Timed releases are counted in frames from the vsync hook instead of using
   alarms, because maincpu_clk is reset to a small value on machine reset and
   restored from snapshots, which would leave alarm based releases stuck. */

#include "vice.h"

#include <stdio.h>

#include "joyport.h"
#include "joystick.h"
#include "keyboard.h"
#include "machine.h"
#include "mon_input.h"
#include "monitor.h"
#include "montypes.h"
#include "network.h"
#include "types.h"


/* frames until an injected key/joystick value is released again, 0 means it
   stays until it is released explicitly */
static int key_frames[KBD_ROWS][KBD_COLS];
static int restore_frames = 0;
static int joy_frames[JOYPORT_MAX_PORTS];

/* number of running timers, so the vsync hook doesn't have to scan the
   arrays above every frame */
static int timers_running = 0;

static const char * const joy_bit_names[] = {
    "up", "down", "left", "right",
    "fire", "fire2", "fire3", "fire4",
    "fire5", "fire6", "fire7", "fire8"
};

/* all bits used by joystick_value[], see joystick.h */
#define JOY_VALUE_MASK  0x0fff


static void set_timer(int *timer, int frames)
{
    if (*timer > 0) {
        timers_running--;
    }
    *timer = frames;
    if (frames > 0) {
        timers_running++;
    }
}

/* returns 1 when the timer ran out in this frame */
static int run_timer(int *timer)
{
    if (*timer > 0) {
        (*timer)--;
        if (*timer == 0) {
            timers_running--;
            return 1;
        }
    }
    return 0;
}

static int injection_possible(void)
{
    if (machine_class == VICE_MACHINE_VSID) {
        mon_out("Not available in VSID.\n");
        return 0;
    }
    if (network_connected()) {
        mon_out("Not available during network play.\n");
        return 0;
    }
    return 1;
}

static int check_frames(int frames)
{
    if (frames < 0) {
        mon_out("Illegal frame count.\n");
        return 0;
    }
    return 1;
}

/* size of the keyboard matrix as used in keyarr[] */
static void keymatrix_size(int *rows, int *cols)
{
    *rows = 8;
    *cols = 8;

    switch (machine_class) {
        case VICE_MACHINE_C128:
            *rows = 11;
            break;
        case VICE_MACHINE_PET:
            *rows = 10;
            break;
        case VICE_MACHINE_CBM5x0:
        case VICE_MACHINE_CBM6x0:
            *rows = 16;
            *cols = 6;
            break;
        default:
            break;
    }
}

static int new_state(ACTION action, int old_state)
{
    switch (action) {
        case e_OFF:
            return 0;
        case e_TOGGLE:
            return old_state ? 0 : 1;
        default:
            return 1;
    }
}

/** \brief  Print the keyboard matrix
 *
 * Keys pressed from the monitor are shown as '#', keys pressed by the host
 * keyboard as '*'.
 */
void mon_keymatrix_show(void)
{
    int rows;
    int cols;
    int row;
    int col;
    int timed = 0;

    if (machine_class == VICE_MACHINE_VSID) {
        mon_out("Not available in VSID.\n");
        return;
    }

    keymatrix_size(&rows, &cols);

    mon_out("      col");
    for (col = 0; col < cols; col++) {
        mon_out(" %d", col);
    }
    mon_out("\n");

    for (row = 0; row < rows; row++) {
        mon_out("row %2d:  ", row);
        for (col = 0; col < cols; col++) {
            if (keyboard_inject_key_get(row, col)) {
                mon_out(" #");
            } else if (keyarr[row] & (1 << col)) {
                mon_out(" *");
            } else {
                mon_out(" .");
            }
        }
        mon_out("\n");
    }

    for (row = 0; row < rows; row++) {
        for (col = 0; col < cols; col++) {
            if (key_frames[row][col] > 0) {
                if (!timed) {
                    mon_out("Timed releases:\n");
                    timed = 1;
                }
                mon_out("  row %d col %d: %d frames left\n",
                        row, col, key_frames[row][col]);
            }
        }
    }

    if (machine_has_restore_key()) {
        mon_out("RESTORE: %s", keyboard_inject_restore_get() ? "pressed" : "released");
        if (restore_frames > 0) {
            mon_out(", %d frames left", restore_frames);
        }
        mon_out("\n");
    }
}

/** \brief  Press or release a key in the keyboard matrix
 *
 * \param[in]   row     matrix row
 * \param[in]   col     matrix column
 * \param[in]   action  e_ON to press, e_OFF to release, e_TOGGLE to toggle
 * \param[in]   frames  release the key again after this many frames when
 *                      pressing it, 0 keeps it pressed
 *
 * \return  0 on success, -1 on error (a message has been printed)
 */
int mon_keymatrix_set(int row, int col, ACTION action, int frames)
{
    int rows;
    int cols;
    int pressed;

    if (!injection_possible() || !check_frames(frames)) {
        return -1;
    }

    keymatrix_size(&rows, &cols);
    if (row < 0 || row >= rows || col < 0 || col >= cols) {
        mon_out("Illegal key, rows are $00-$%02x and columns $00-$%02x.\n",
                (unsigned int)(rows - 1), (unsigned int)(cols - 1));
        return -1;
    }

    pressed = new_state(action, keyboard_inject_key_get(row, col));
    keyboard_inject_key(row, col, pressed);
    set_timer(&key_frames[row][col], pressed ? frames : 0);
    return 0;
}

/** \brief  Press or release RESTORE
 *
 * \param[in]   action  e_ON to press, e_OFF to release, e_TOGGLE to toggle
 * \param[in]   frames  release the key again after this many frames when
 *                      pressing it, 0 keeps it pressed
 *
 * \return  0 on success, -1 on error (a message has been printed)
 */
int mon_keymatrix_restore(ACTION action, int frames)
{
    int pressed;

    if (!injection_possible() || !check_frames(frames)) {
        return -1;
    }

    pressed = new_state(action, keyboard_inject_restore_get());
    if (keyboard_inject_restore(pressed) < 0) {
        mon_out("This machine has no RESTORE key.\n");
        return -1;
    }
    set_timer(&restore_frames, pressed ? frames : 0);
    return 0;
}

/** \brief  Release all keys pressed from the monitor
 */
void mon_keymatrix_clear(void)
{
    int row;
    int col;

    if (machine_class == VICE_MACHINE_VSID) {
        return;
    }

    for (row = 0; row < KBD_ROWS; row++) {
        for (col = 0; col < KBD_COLS; col++) {
            set_timer(&key_frames[row][col], 0);
        }
    }
    set_timer(&restore_frames, 0);
    keyboard_inject_clear();
}

static int joy_port_valid(int port)
{
    return port >= 0 && port < JOYPORT_MAX_PORTS
           && joyport_get_port_name(port) != NULL
           && joyport_port_is_active(port);
}

/** \brief  Print the joystick values set from the monitor for all ports
 */
void mon_joystick_show(void)
{
    int port;
    int found = 0;
    unsigned int bit;
    uint16_t value;

    for (port = 0; port < JOYPORT_MAX_PORTS; port++) {
        if (!joy_port_valid(port)) {
            continue;
        }
        found = 1;
        value = joystick_inject_get(port);
        mon_out("%2d (%s): $%02x", port + 1, joyport_get_port_name(port), (unsigned int)value);
        for (bit = 0; bit < sizeof joy_bit_names / sizeof joy_bit_names[0]; bit++) {
            if (value & (1 << bit)) {
                mon_out(" %s", joy_bit_names[bit]);
            }
        }
        if (joy_frames[port] > 0) {
            mon_out(", %d frames left", joy_frames[port]);
        }
        mon_out("\n");
    }
    if (!found) {
        mon_out("No active joystick ports.\n");
    }
}

/** \brief  Set joystick bits on a joystick port
 *
 * \param[in]   port    port number, starting at 1 for control port 1
 * \param[in]   value   joystick bits (1 = up, 2 = down, 4 = left,
 *                      8 = right, $10 = fire, $20-$800 = fire 2-8)
 * \param[in]   frames  clear the value again after this many frames,
 *                      0 keeps it
 *
 * \return  0 on success, -1 on error (a message has been printed)
 */
int mon_joystick_set(int port, int value, int frames)
{
    if (!injection_possible() || !check_frames(frames)) {
        return -1;
    }

    if (!joy_port_valid(port - 1)) {
        mon_out("Illegal port.\n");
        return -1;
    }
    if (value < 0 || value > JOY_VALUE_MASK) {
        mon_out("Illegal value.\n");
        return -1;
    }

    joystick_inject_set(port - 1, (uint16_t)value);
    set_timer(&joy_frames[port - 1], value ? frames : 0);
    return 0;
}

/** \brief  Clear the joystick values set from the monitor on all ports
 */
void mon_joystick_clear(void)
{
    int port;

    for (port = 0; port < JOYPORT_MAX_PORTS; port++) {
        set_timer(&joy_frames[port], 0);
        if (joystick_inject_get(port)) {
            joystick_inject_set(port, 0);
        }
    }
}

/** \brief  Release injected keys and joystick values whose time ran out
 *
 * Called once per frame.
 */
void mon_input_vsync_hook(void)
{
    int row;
    int col;
    int port;

    if (timers_running == 0) {
        return;
    }

    for (row = 0; row < KBD_ROWS; row++) {
        for (col = 0; col < KBD_COLS; col++) {
            if (run_timer(&key_frames[row][col])) {
                keyboard_inject_key(row, col, 0);
            }
        }
    }
    if (run_timer(&restore_frames)) {
        keyboard_inject_restore(0);
    }
    for (port = 0; port < JOYPORT_MAX_PORTS; port++) {
        if (run_timer(&joy_frames[port])) {
            joystick_inject_set(port, 0);
        }
    }
}
