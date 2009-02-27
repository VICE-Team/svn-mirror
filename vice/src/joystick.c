/*
 * joystick.c - Common joystick emulation.
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
 *
 * Based on old code by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Jouko Valta <jopi@stekt.oulu.fi>
 *  André Fachat <fachat@physik.tu-chemnitz.de>
 *  Bernhard Kuhn <kuhn@eikon.e-technik.tu-muenchen.de>
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
#include "event.h"
#include "keyboard.h"
#include "joy.h"
#include "joystick.h"
#include "kbd.h"
#include "maincpu.h"
#include "network.h"
#include "snapshot.h"
#include "uiapi.h"
#include "types.h"
#include "log.h"
#include "resources.h"

#define JOYSTICK_RAND() (rand() & 0x3fff)

#define JOYSTICK_NUM 3

/* Global joystick value.  */
BYTE joystick_value[JOYSTICK_NUM] = { 0, 0, 0 };

/* Latched joystick status.  */
static BYTE latch_joystick_value[JOYSTICK_NUM] = { 0, 0, 0 };
static BYTE network_joystick_value[JOYSTICK_NUM] = { 0, 0, 0 };

/* to prevent illegal direction combinations */
static const BYTE joystick_opposite_direction[] = 
    { 0, 2, 1, 3, 8, 10, 9, 11, 4, 6, 5, 7, 12, 14, 13, 15 };

static alarm_t *joystick_alarm = NULL;

static CLOCK joystick_delay;

#ifdef COMMON_KBD
static int joykeys[3][9];
#endif

static void joystick_latch_matrix(CLOCK offset)
{
    BYTE idx;

    if (network_connected()) {
        idx = network_joystick_value[0];
        if (idx > 0)
            joystick_value[idx] = network_joystick_value[idx];
        else
            memcpy(joystick_value, network_joystick_value, sizeof(joystick_value));
    } else {
        memcpy(joystick_value, latch_joystick_value, sizeof(joystick_value));
    }
    ui_display_joyport(joystick_value);
}

/*-----------------------------------------------------------------------*/

static void joystick_event_record(void)
{
    event_record(EVENT_JOYSTICK_VALUE, (void *)joystick_value,
                 sizeof(joystick_value));
}

void joystick_event_playback(CLOCK offset, void *data)
{
    memcpy(latch_joystick_value, data, sizeof(joystick_value));

    joystick_latch_matrix(offset);
}

static void joystick_latch_handler(CLOCK offset, void *data)
{
    alarm_unset(joystick_alarm);
    alarm_context_update_next_pending(joystick_alarm->context);

    joystick_latch_matrix(offset);

    joystick_event_record(); 
}

void joystick_event_delayed_playback(void *data)
{
    memcpy(network_joystick_value, data, sizeof(latch_joystick_value));
    alarm_set(joystick_alarm, maincpu_clk + joystick_delay);
}

void joystick_register_delay(unsigned int delay)
{
    joystick_delay = delay;
}
/*-----------------------------------------------------------------------*/
static void joystick_process_latch(void)
{
    if (network_connected()) {
        CLOCK joystick_delay = JOYSTICK_RAND();
        network_event_record(EVENT_JOYSTICK_DELAY,
                (void *)&joystick_delay, sizeof(joystick_delay));
        network_event_record(EVENT_JOYSTICK_VALUE, 
                (void *)latch_joystick_value, sizeof(latch_joystick_value));
    } 
    else
    {
        alarm_set(joystick_alarm, maincpu_clk + JOYSTICK_RAND());
    }
}

void joystick_set_value_absolute(unsigned int joyport, BYTE value)
{
    if (event_playback_active())
        return;

    if (latch_joystick_value[joyport] != value) {
        latch_joystick_value[joyport] = value;
        latch_joystick_value[0] = (BYTE)joyport;
        joystick_process_latch();
    }
}

void joystick_set_value_or(unsigned int joyport, BYTE value)
{
    if (event_playback_active())
        return;

    latch_joystick_value[joyport] |= value;
    latch_joystick_value[joyport] &= ~joystick_opposite_direction[value & 0xf];
    latch_joystick_value[0] = (BYTE)joyport;
    joystick_process_latch();
}

void joystick_set_value_and(unsigned int joyport, BYTE value)
{
    if (event_playback_active())
        return;

    latch_joystick_value[joyport] &= value;
    latch_joystick_value[0] = (BYTE)joyport;
    joystick_process_latch();
}

void joystick_clear(unsigned int joyport)
{
    latch_joystick_value[joyport] = 0;
    latch_joystick_value[0] = (BYTE)joyport;
    joystick_latch_matrix(0);
}

void joystick_clear_all(void)
{
    memset(latch_joystick_value, 0, JOYSTICK_NUM);
    joystick_latch_matrix(0);
}

/*-----------------------------------------------------------------------*/

#ifdef COMMON_KBD
/* the order of values in joypad_bits is the same as in joystick_direction_t */
static int joypad_bits[9] = {
    0x10, 0x06, 0x02, 0x0a, 0x04, 0x08, 0x05, 0x01, 0x09
};

static int joypad_status[3][9];

typedef enum {
    KEYSET_FIRE,
    KEYSET_SW,
    KEYSET_S,
    KEYSET_SE,
    KEYSET_W,
    KEYSET_E,
    KEYSET_NW,
    KEYSET_N,
    KEYSET_NE
} joystick_direction_t;

static int joyreleaseval(int column, int *status)
{
    int val = 0;

    switch (column) {
      case KEYSET_SW:
        val = (status[KEYSET_S] ? 0 : joypad_bits[KEYSET_S]) | 
              (status[KEYSET_W] ? 0 : joypad_bits[KEYSET_W]);
        break;
      case KEYSET_SE:
        val = (status[KEYSET_S] ? 0 : joypad_bits[KEYSET_S]) | 
              (status[KEYSET_E] ? 0 : joypad_bits[KEYSET_E]);
        break;
      case KEYSET_NW:
        val = (status[KEYSET_N] ? 0 : joypad_bits[KEYSET_N]) | 
              (status[KEYSET_W] ? 0 : joypad_bits[KEYSET_W]);
        break;
      case KEYSET_NE:
        val = (status[KEYSET_N] ? 0 : joypad_bits[KEYSET_N]) | 
              (status[KEYSET_E] ? 0 : joypad_bits[KEYSET_E]);
        break;
      default:
        val = joypad_bits[column];
        break;
    }
    return ~val;
}

/* toggle keyset joystick. 
   this disables any active key-based joystick and is useful for typing. */
static int joykeys_enable = 0;

static int set_joykeys_enable(int val, void *param)
{
  joykeys_enable = val;
  return 0;
}

#define DEFINE_SET_KEYSET(num)                       \
    static int set_keyset##num(int val, void *param) \
    {                                                \
        joykeys[num][(int)param] = val;              \
                                                     \
        return 0;                                    \
    }

DEFINE_SET_KEYSET(1)
DEFINE_SET_KEYSET(2)

static const resource_int_t resources_int[] = {
    { "KeySet1NorthWest", 0, RES_EVENT_NO, NULL,
      &joykeys[1][KEYSET_NW], set_keyset1, (void *)KEYSET_NW },
    { "KeySet1North", 0, RES_EVENT_NO, NULL,
      &joykeys[1][KEYSET_N], set_keyset1, (void *)KEYSET_N },
    { "KeySet1NorthEast", 0, RES_EVENT_NO, NULL,
      &joykeys[1][KEYSET_NE], set_keyset1, (void *)KEYSET_NE },
    { "KeySet1East", 0, RES_EVENT_NO, NULL,
      &joykeys[1][KEYSET_E], set_keyset1, (void *)KEYSET_E },
    { "KeySet1SouthEast", 0, RES_EVENT_NO, NULL,
      &joykeys[1][KEYSET_SE], set_keyset1, (void *)KEYSET_SE },
    { "KeySet1South", 0, RES_EVENT_NO, NULL,
      &joykeys[1][KEYSET_S], set_keyset1, (void *)KEYSET_S },
    { "KeySet1SouthWest", 0, RES_EVENT_NO, NULL,
      &joykeys[1][KEYSET_SW], set_keyset1, (void *)KEYSET_SW },
    { "KeySet1West", 0, RES_EVENT_NO, NULL,
      &joykeys[1][KEYSET_W], set_keyset1, (void *)KEYSET_W },
    { "KeySet1Fire", 0, RES_EVENT_NO, NULL,
      &joykeys[1][KEYSET_FIRE], set_keyset1, (void *)KEYSET_FIRE },
    { "KeySet2NorthWest", 0, RES_EVENT_NO, NULL,
      &joykeys[2][KEYSET_NW], set_keyset2, (void *)KEYSET_NW },
    { "KeySet2North", 0, RES_EVENT_NO, NULL,
      &joykeys[2][KEYSET_N], set_keyset2, (void *)KEYSET_N },
    { "KeySet2NorthEast", 0, RES_EVENT_NO, NULL,
      &joykeys[2][KEYSET_NE], set_keyset2, (void *)KEYSET_NE },
    { "KeySet2East", 0, RES_EVENT_NO, NULL,
      &joykeys[2][KEYSET_E], set_keyset2, (void *)KEYSET_E },
    { "KeySet2SouthEast", 0, RES_EVENT_NO, NULL,
      &joykeys[2][KEYSET_SE], set_keyset2, (void *)KEYSET_SE },
    { "KeySet2South", 0, RES_EVENT_NO, NULL,
      &joykeys[2][KEYSET_S], set_keyset2, (void *)KEYSET_S },
    { "KeySet2SouthWest", 0, RES_EVENT_NO, NULL,
      &joykeys[2][KEYSET_SW], set_keyset2, (void *)KEYSET_SW },
    { "KeySet2West", 0, RES_EVENT_NO, NULL,
      &joykeys[2][KEYSET_W], set_keyset2, (void *)KEYSET_W },
    { "KeySet2Fire", 0, RES_EVENT_NO, NULL,
      &joykeys[2][KEYSET_FIRE], set_keyset2, (void *)KEYSET_FIRE },
    { "KeySetEnable", 1, RES_EVENT_NO, NULL,
      &joykeys_enable, set_joykeys_enable, NULL },
    { NULL }
};

int joystick_check_set(signed long key, int keysetnum, unsigned int joyport)
{
    int column;

    /* if joykeys are disabled then ignore key sets */
    if(!joykeys_enable)
      return 0;

    for (column = 0; column < 9; column++) {
        if (key == joykeys[keysetnum][column]) {
            if (joypad_bits[column]) {
                /*joystick_value[joyport] |= joypad_bits[column];*/
                joystick_set_value_or(joyport, (BYTE)joypad_bits[column]);
                joypad_status[keysetnum][column] = 1;
            } else {
                /*joystick_value[joyport] = 0;*/
                joystick_set_value_absolute(joyport, 0);
                memset(joypad_status[keysetnum], 0, sizeof(joypad_status[keysetnum]));
            }
            return 1;
        }
    }
    return 0;
}

int joystick_check_clr(signed long key, int keysetnum, unsigned int joyport)
{
    int column;

    /* if joykeys are disabled then ignore key sets */
    if(!joykeys_enable)
      return 0;

    for (column = 0; column < 9; column++) {
        if (key == joykeys[keysetnum][column]) {
            /*joystick_value[joyport] &= joyreleaseval(column,
                                                     joypad_status[joynum]);*/
            joystick_set_value_and(joyport, (BYTE)joyreleaseval(column,
                                   joypad_status[keysetnum]));
            joypad_status[keysetnum][column] = 0;
            return 1;
        }
    }
    return 0;
}

void joystick_joypad_clear(void)
{
    memset(joypad_status, 0, sizeof(joypad_status));
}

/*-----------------------------------------------------------------------*/

int joystick_init_resources(void)
{
    resources_register_int(resources_int);

    return joystick_arch_init_resources();
}
#endif

int joystick_init(void)
{
    joystick_alarm = alarm_new(maincpu_alarm_context, "Joystick",
                               joystick_latch_handler, NULL);

#ifdef COMMON_KBD
    kbd_initialize_numpad_joykeys(joykeys[0]);
#endif

    return joy_arch_init();
}

/*--------------------------------------------------------------------------*/
int joystick_snapshot_write_module(snapshot_t *s)
{
    snapshot_module_t *m;

    m = snapshot_module_create(s, "JOYSTICK", 1, 0);
    if (m == NULL)
       return -1;

    if (0
        || SMW_BA(m, joystick_value, JOYSTICK_NUM) < 0)
    {
        snapshot_module_close(m);
        return -1;
    }

    if (snapshot_module_close(m) < 0)
        return -1;

    return 0;
}

int joystick_snapshot_read_module(snapshot_t *s)
{
    BYTE major_version, minor_version;
    snapshot_module_t *m;

    m = snapshot_module_open(s, "JOYSTICK",
                             &major_version, &minor_version);
    if (m == NULL) {
        return 0;
    }

    if (0
        || SMR_BA(m, joystick_value, JOYSTICK_NUM) < 0)
    {
        snapshot_module_close(m);
        return -1;
    }

    snapshot_module_close(m);
    return 0;
}
