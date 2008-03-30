/*
 * keyboard.c - Common keyboard emulation.
 *
 * Written by
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
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

#include <stdlib.h>
#include <string.h>

#include "alarm.h"
#include "keyboard.h"
#include "log.h"
#include "machine.h"
#include "maincpu.h"
#include "resources.h"
#include "types.h"
#include "utils.h"


#define KEYBOARD_RAND() (rand() & 0x3fff)

/* Keyboard array.  */
int keyarr[KBD_ROWS];
int rev_keyarr[KBD_COLS];

/* Keyboard status to be latched into the keyboard array.  */
static int latch_keyarr[KBD_ROWS];
static int latch_rev_keyarr[KBD_COLS];

/* Latched joystick status.  */
static BYTE latch_joystick_value[3] = { 0, 0, 0 };

static alarm_t keyboard_alarm;
static alarm_t joystick_alarm;


static void keyboard_latch_matrix(CLOCK offset)
{
    alarm_unset(&keyboard_alarm);
    alarm_context_update_next_pending(keyboard_alarm.context);

    memcpy(keyarr, latch_keyarr, sizeof(keyarr));
    memcpy(rev_keyarr, latch_rev_keyarr, sizeof(rev_keyarr));
}

static void joystick_latch_matrix(CLOCK offset)
{
    alarm_unset(&joystick_alarm);
    alarm_context_update_next_pending(joystick_alarm.context);

    memcpy(joystick_value, latch_joystick_value, sizeof(joystick_value));
}

/*-----------------------------------------------------------------------*/

void keyboard_set_keyarr(int row, int col, int value)
{
    if (row < 0 || col < 0)
        return;
    if (value) {
        latch_keyarr[row] |= 1 << col;
        latch_rev_keyarr[col] |= 1 << row;
    } else {
        latch_keyarr[row] &= ~(1 << col);
        latch_rev_keyarr[col] &= ~(1 << row);
    }

    alarm_set(&keyboard_alarm, maincpu_clk + KEYBOARD_RAND());
}

void keyboard_set_keyarr_and_latch(int row, int col, int value)
{
    if (row < 0 || col < 0)
        return;
    if (value) {
        latch_keyarr[row] |= 1 << col;
        latch_rev_keyarr[col] |= 1 << row;
    } else {
        latch_keyarr[row] &= ~(1 << col);
        latch_rev_keyarr[col] &= ~(1 << row);
    }
    memcpy(keyarr, latch_keyarr, sizeof(keyarr));
    memcpy(rev_keyarr, latch_rev_keyarr, sizeof(rev_keyarr));
}

void keyboard_clear_keymatrix(void)
{
    memset(keyarr, 0, sizeof(keyarr));
    memset(rev_keyarr, 0, sizeof(rev_keyarr));
    memset(latch_keyarr, 0, sizeof(latch_keyarr));
    memset(latch_rev_keyarr, 0, sizeof(latch_rev_keyarr));
}

void joystick_set_value_absolute(unsigned int joyport, BYTE value)
{
    latch_joystick_value[joyport] = value;
    alarm_set(&joystick_alarm, maincpu_clk + KEYBOARD_RAND());
}

void joystick_set_value_or(unsigned int joyport, BYTE value)
{
    latch_joystick_value[joyport] |= value;
    alarm_set(&joystick_alarm, maincpu_clk + KEYBOARD_RAND());
}

void joystick_set_value_and(unsigned int joyport, BYTE value)
{
    latch_joystick_value[joyport] &= value;
    alarm_set(&joystick_alarm, maincpu_clk + KEYBOARD_RAND());
}

void joystick_clear(unsigned int joyport)
{
    latch_joystick_value[joyport] = 0;
}

/*-----------------------------------------------------------------------*/

#ifdef COMMON_KBD
extern int load_keymap_ok;
extern int kbd_load_keymap(const char *filename);

int keyboard_set_keymap_index(resource_value_t v, void *param)
{
    const char *name, *resname;

    resname = machine_keymap_res_name_list[(unsigned int)v];

    if (resources_get_value(resname, (resource_value_t *)&name) < 0)
        return -1;

    if (load_keymap_ok) {
        if (kbd_load_keymap(name) >= 0) {
            machine_keymap_index = (unsigned int)v;
            return 0;
        } else {
            log_error(LOG_DEFAULT, _("Cannot load keymap `%s'."),
                      name ? name : _("(null)"));
        }
        return -1;
    }

    machine_keymap_index = (unsigned int)v;
    return 0;
}

int keyboard_set_keymap_file(resource_value_t v, void *param)
{
    int oldindex, newindex;

    newindex = (int)param;

    if (newindex >= machine_num_keyboard_mappings())
        return -1;

    if (resources_get_value("KeymapIndex", (resource_value_t *)&oldindex) < 0)
        return -1;

    if (util_string_set(&machine_keymap_file_list[newindex], (const char *)v))
        return 0;

    /* reset oldindex -> reload keymap file if this keymap is active */
    if (oldindex == newindex)
        resources_set_value("KeymapIndex", (resource_value_t)oldindex);

    return 0;
}
#endif

void keyboard_init(void)
{
    alarm_init(&keyboard_alarm, maincpu_alarm_context,
               "Keyboard", keyboard_latch_matrix);
    alarm_init(&joystick_alarm, maincpu_alarm_context,
               "Joystick", joystick_latch_matrix);
}

