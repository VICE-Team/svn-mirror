/*
 * event.c - Event handling.
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
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
#include <stdlib.h>
#include <string.h>

#include "alarm.h"
#include "archdep.h"
#include "attach.h"
#include "clkguard.h"
#include "cmdline.h"
#include "datasette.h"
#include "debug.h"
#include "event.h"
#include "interrupt.h"
#include "joystick.h"
#include "keyboard.h"
#include "lib.h"
#include "log.h"
#include "machine.h"
#include "maincpu.h"
#include "resources.h"
#include "snapshot.h"
#include "tape.h"
#include "types.h"
#include "ui.h"
#include "util.h"


#define EVENT_START_SNAPSHOT "start" FSDEV_EXT_SEP_STR "vsf"
#define EVENT_END_SNAPSHOT "end" FSDEV_EXT_SEP_STR "vsf"
#define EVENT_MILESTONE_SNAPSHOT "milestone" FSDEV_EXT_SEP_STR "vsf"


struct event_list_s {
    unsigned int type;
    CLOCK clk;
    unsigned int size;
    void *data;
    struct event_list_s *next;
};
typedef struct event_list_s event_list_t;


static event_list_t *event_list_base = NULL, *event_list_current;

static alarm_t *event_alarm = NULL;

static log_t event_log = LOG_DEFAULT;

static unsigned int playback_active = 0, record_active = 0;

static unsigned int current_timestamp, milestone_timestamp, playback_time;
static CLOCK last_timestamp_alarm;
static CLOCK milestone_timestamp_alarm;

static char *event_snapshot_dir = NULL;
static char *event_start_snapshot = NULL;
static char *event_end_snapshot = NULL;
static char *event_snapshot_path_str = NULL;
static unsigned int event_start_mode;

static char *event_snapshot_path(const char *snapshot_file)
{
    if (event_snapshot_path_str != NULL)
        lib_free(event_snapshot_path_str);

    event_snapshot_path_str = 
        util_concat(event_snapshot_dir, snapshot_file, NULL);

    return event_snapshot_path_str;
}

void event_record(unsigned int type, void *data, unsigned int size)
{
    void *event_data = NULL;

    if (record_active == 0)
        return;

    /*log_debug("EVENT RECORD %i CLK %i", type, maincpu_clk);*/

    switch (type) {
      case EVENT_RESETCPU:
        last_timestamp_alarm -= maincpu_clk;
      case EVENT_KEYBOARD_MATRIX:
      case EVENT_KEYBOARD_RESTORE:
      case EVENT_JOYSTICK_VALUE:
      case EVENT_DATASETTE:
      case EVENT_ATTACHDISK:
      case EVENT_ATTACHTAPE:
      case EVENT_INITIAL:
        event_data = lib_malloc(size);
        memcpy(event_data, data, size);
        break;
      case EVENT_TIMESTAMP:
        ui_display_event_time(current_timestamp++, 0);
        break;
      case EVENT_LIST_END:
        break;
      default:
        /*log_error(event_log, "Unknow event type %i.", type);*/
        return;
    }

    event_list_current->type = type;
    event_list_current->clk = maincpu_clk;
    event_list_current->size = size;
    event_list_current->data = event_data;
    event_list_current->next
        = (event_list_t *)lib_calloc(1, sizeof(event_list_t));
    event_list_current = event_list_current->next;
}

static void next_alarm_set(void)
{
    CLOCK new_value;

    new_value = event_list_current->clk;

    if (maincpu_clk > CLKGUARD_SUB_MIN
        && new_value < maincpu_clk - CLKGUARD_SUB_MIN)
        new_value += clk_guard_clock_sub(maincpu_clk_guard);

    alarm_set(event_alarm, new_value);
}

static void next_current_list(void)
{
    event_list_current = event_list_current->next;
}

static void event_alarm_handler(CLOCK offset, void *data)
{
    alarm_unset(event_alarm);

    /* when recording write a timestamp */
    if (record_active) {
        event_record(EVENT_TIMESTAMP, NULL, 0);
        last_timestamp_alarm = last_timestamp_alarm 
                                + machine_get_cycles_per_second();
        alarm_set(event_alarm, last_timestamp_alarm);
        return;
    }

    /*log_debug("EVENT PLAYBACK %i CLK %i", event_list_current->type,
              event_list_current->clk);*/

    switch (event_list_current->type) {
      case EVENT_KEYBOARD_MATRIX:
        keyboard_event_playback(offset, event_list_current->data);
        break;
      case EVENT_KEYBOARD_RESTORE:
        keyboard_restore_event_playback(offset, event_list_current->data);
        break;
      case EVENT_JOYSTICK_VALUE:
        joystick_event_playback(offset, event_list_current->data);
        break;
      case EVENT_DATASETTE:
        datasette_event_playback(offset, event_list_current->data);
        break;
      case EVENT_ATTACHDISK:
        file_system_event_playback(offset, event_list_current->data);
        break;
      case EVENT_ATTACHTAPE:
        tape_image_event_playback(offset, event_list_current->data);
        break;
      case EVENT_RESETCPU:
        machine_reset_event_playback(offset, event_list_current->data);
        break;
      case EVENT_TIMESTAMP:
        ui_display_event_time(current_timestamp++, playback_time);
        break;
      case EVENT_LIST_END:
        event_playback_stop();
        break;
      default:
        log_error(event_log, "Unknow event type %i.", event_list_current->type);
    }

    if (event_list_current->type != EVENT_LIST_END
        && event_list_current->type != EVENT_RESETCPU) {
        next_current_list();
        next_alarm_set();
    }
}

/*-----------------------------------------------------------------------*/

static void create_list(void)
{
    event_list_base = (event_list_t *)lib_calloc(1, sizeof(event_list_t));
    event_list_current = event_list_base;
}

static void destroy_list(void)
{
    event_list_t *c1, *c2;

    c1 = event_list_base;

    while (c1 != NULL) {
        c2 = c1->next;
        lib_free(c1->data);
        lib_free(c1);
        c1 = c2;
    }

    event_list_base = NULL;
    event_list_current = NULL;
}

static void warp_end_list(void)
{
    event_list_t *curr;

    curr = event_list_base;

    while (curr->type != EVENT_LIST_END)
        curr = curr->next;

    memset(curr, 0, sizeof(event_list_t));
    event_list_current = curr;
}

/*-----------------------------------------------------------------------*/

static void event_initial_write(void)
{
    BYTE *data = NULL;
    size_t len = 0;

    switch (event_start_mode) {
      case EVENT_START_MODE_FILE_SAVE:
        len = 1 + strlen(event_start_snapshot) + 1;
        data = lib_malloc(len);
        data[0] = EVENT_START_MODE_FILE_SAVE;
        strcpy((char *)&data[1], event_start_snapshot);
        break;
      case EVENT_START_MODE_RESET:
        len = 1;
        data = lib_malloc(len);
        data[0] = EVENT_START_MODE_RESET;
        break;
    }

    event_record(EVENT_INITIAL, (void *)data, (unsigned int)len);

    lib_free(data);
}

/*-----------------------------------------------------------------------*/

static void event_record_start_trap(WORD addr, void *data)
{
    switch (event_start_mode) {
      case EVENT_START_MODE_FILE_SAVE:
        if (machine_write_snapshot(event_snapshot_path(event_start_snapshot),
                                    1, 1, 0) < 0) {
            ui_error(_("Could not create start snapshot file."));
            return;
        }
        destroy_list();
        create_list();
        record_active = 1;
        event_initial_write();
        last_timestamp_alarm = maincpu_clk;
        current_timestamp = 0;
        break;
      case EVENT_START_MODE_FILE_LOAD:
        if (machine_read_snapshot(
                event_snapshot_path(event_end_snapshot), 1) < 0) {
            ui_error(_("Error reading end snapshot file."));
            return;
        }
        warp_end_list();
        record_active = 1;
        last_timestamp_alarm = maincpu_clk;
        current_timestamp = playback_time;
        break;
      case EVENT_START_MODE_RESET:
        machine_trigger_reset(MACHINE_RESET_MODE_HARD);
        destroy_list();
        create_list();
        record_active = 1;
        event_initial_write();
        last_timestamp_alarm = 0;
        current_timestamp = 0;
        break;
      default:
        log_error(event_log, "Unknown event start mode %i", event_start_mode); 
        return;
    }

#ifdef  DEBUG
    debug_start_recording();
#endif

    /* use alarm for timestamps */
    milestone_timestamp_alarm = 0;
    alarm_set(event_alarm, last_timestamp_alarm);
}

int event_record_start(void)
{
    if (playback_active != 0)
        return -1;

    if (record_active != 0)
        return -1;

    interrupt_maincpu_trigger_trap(event_record_start_trap, (void *)0);

    ui_display_recording(1);

    return 0;
}

static void event_record_stop_trap(WORD addr, void *data)
{
    if (machine_write_snapshot(
            event_snapshot_path(event_end_snapshot), 1, 1, 1) < 0) {
        ui_error(_("Could not create end snapshot file."));
        return;
    }
    record_active = 0;

#ifdef  DEBUG
    debug_stop_recording();
#endif
}

int event_record_stop(void)
{
    if (record_active == 0)
        return -1;

    event_record(EVENT_LIST_END, NULL, 0);

    interrupt_maincpu_trigger_trap(event_record_stop_trap, (void *)0);

    ui_display_recording(0);

    alarm_unset(event_alarm);

    return 0;
}

/*-----------------------------------------------------------------------*/

static unsigned int playback_reset_ack = 0;

void event_reset_ack(void)
{
    if (playback_reset_ack) {
        playback_reset_ack = 0;
        next_alarm_set();
    }

    if (event_list_current 
        && event_list_current->type == EVENT_RESETCPU)
    {
        next_current_list();
        next_alarm_set();
    }

    /* timestamp alarm needs to be set */
    if (record_active)
        alarm_set(event_alarm, last_timestamp_alarm);
}

static void event_playback_start_trap(WORD addr, void *data)
{
    snapshot_t *s;
    BYTE minor, major;

    s = snapshot_open(
        event_snapshot_path(event_end_snapshot), &major, &minor, machine_name);

    if (s == NULL) {
        ui_error(_("Could not open end snapshot file."));
        return;
    }

    destroy_list();
    create_list();

    if (event_snapshot_read_module(s, 1) < 0) {
        snapshot_close(s);
        ui_error(_("Could not find event section in end snapshot file."));
        return;
    }

    snapshot_close(s);

    event_list_current = event_list_base;

    if (event_list_current->type == EVENT_INITIAL) {
        BYTE *data = (BYTE *)(event_list_current->data);
        switch (data[0]) {
          case EVENT_START_MODE_FILE_SAVE:
            /*log_debug("READING %s", (char *)(&data[1]));*/
            if (machine_read_snapshot(
                    event_snapshot_path((char *)(&data[1])), 0) < 0) {
                ui_error(_("Error reading start snapshot file."));
                return;
            }
            next_current_list();
            next_alarm_set();
            break;
          case EVENT_START_MODE_RESET:
            /*log_debug("RESET MODE!");*/
            machine_trigger_reset(MACHINE_RESET_MODE_HARD);
            next_current_list();
            /* Alarm will be set if reset is ack'ed.  */
            playback_reset_ack = 1;
            break;
        }
    } else {
        if (machine_read_snapshot(
                event_snapshot_path(event_start_snapshot), 0) < 0) {
            ui_error(_("Error reading start snapshot file."));
            return;
        }
        next_alarm_set();
    }

    playback_active = 1;
    current_timestamp = 0;

#ifdef  DEBUG
    debug_start_playback();
#endif
}


int event_playback_start(void)
{
    if (record_active != 0)
        return -1;

    if (playback_active != 0)
        return -1;

    interrupt_maincpu_trigger_trap(event_playback_start_trap, (void *)0);

    ui_display_playback(1);

    return 0;
}

int event_playback_stop(void)
{
    if (playback_active == 0)
        return -1;

    playback_active = 0;

    alarm_unset(event_alarm);

    ui_display_playback(0);

#ifdef  DEBUG
    debug_stop_playback();
#endif

    return 0;
}

static void event_record_set_milestone_trap(WORD addr, void *data)
{
    if (machine_write_snapshot(
        event_snapshot_path(event_end_snapshot), 1, 1, 1) < 0) {
            ui_error(_("Could not create end snapshot file."));
    } else {
        milestone_timestamp_alarm = last_timestamp_alarm;
        milestone_timestamp = current_timestamp;
    }
}

int event_record_set_milestone(void)
{
    if (record_active == 0)
        return -1;

    interrupt_maincpu_trigger_trap(event_record_set_milestone_trap, (void *)0);

    return 0;
}

static void event_record_reset_milestone_trap(WORD addr, void *data)
{
    /* We need to disable recording to avoid events being recorded while
       snapshot reading. */
    record_active = 0;

    if (machine_read_snapshot(
            event_snapshot_path(event_end_snapshot), 1) < 0) {
        ui_error(_("Error reading end snapshot file."));
        return;
    }
    warp_end_list();
    record_active = 1;
    if (milestone_timestamp_alarm > 0) {
        alarm_set(event_alarm, milestone_timestamp_alarm);
        current_timestamp = milestone_timestamp;
    }
}

int event_record_reset_milestone(void)
{
    if (playback_active != 0)
        return -1;

    if (record_active == 0)
        return -1;

    interrupt_maincpu_trigger_trap(event_record_reset_milestone_trap, (void *)0);

    return 0;
}

/*-----------------------------------------------------------------------*/

int event_record_active(void)
{
    return record_active;
}

int event_playback_active(void)
{
    return playback_active;
}

/*-----------------------------------------------------------------------*/

int event_snapshot_read_module(struct snapshot_s *s, int event_mode)
{
    snapshot_module_t *m;
    BYTE major_version, minor_version;
    event_list_t *curr;
    unsigned int num_of_timestamps;

    if (event_mode == 0)
        return 0;

    m = snapshot_module_open(s, "EVENT", &major_version, &minor_version);

    /* This module is not mandatory.  */
    if (m == NULL)
        return 0;

    destroy_list();
    create_list();

    curr = event_list_base;
    num_of_timestamps = 0;
    playback_time = 0;

    while (1) {
        if (SMR_DW_INT(m, (int*)&(curr->type)) < 0) {
            snapshot_module_close(m);
            return -1;
        }

        if (SMR_DW(m, &(curr->clk)) < 0) {
            snapshot_module_close(m);
            return -1;
        }

        if (SMR_DW_INT(m, (int*)&(curr->size)) < 0) {
            snapshot_module_close(m);
            return -1;
        }

        if (curr->size > 0) {
            curr->data = lib_malloc(curr->size);
            if (SMR_BA(m, curr->data, curr->size) < 0) {
                snapshot_module_close(m);
                return -1;
            }
        }

        if (curr->type == EVENT_LIST_END)
            break;

        if (curr->type == EVENT_TIMESTAMP)
            num_of_timestamps++;

        curr->next = (event_list_t *)lib_calloc(1, sizeof(event_list_t));
        curr = curr->next;
    }

    if (num_of_timestamps > 0)
        playback_time = num_of_timestamps - 1;

    snapshot_module_close(m);

    return 0;
}

int event_snapshot_write_module(struct snapshot_s *s, int event_mode)
{
    snapshot_module_t *m;
    event_list_t *curr;

    if (event_mode == 0)
        return 0;

    m = snapshot_module_create(s, "EVENT", 0, 0);

    if (m == NULL)
        return -1;

    curr = event_list_base;

    while (curr != NULL) {
        if (0
            || SMW_DW(m, (DWORD)curr->type) < 0
            || SMW_DW(m, (DWORD)curr->clk) < 0
            || SMW_DW(m, (DWORD)curr->size) < 0
            || SMW_BA(m, curr->data, curr->size) < 0) {
            snapshot_module_close(m);
            return -1;
        }
        curr = curr->next;
    }

    if (snapshot_module_close(m) < 0)
        return -1;

    return 0;
}

/*-----------------------------------------------------------------------*/

static int set_event_snapshot_dir(resource_value_t v, void *param)
{
    const char *s = (const char *)v;

    /* Make sure that the string ends with FSDEV_DIR_SEP_STR */
    if (s[strlen(s) - 1] == FSDEV_DIR_SEP_CHR) {
        util_string_set(&event_snapshot_dir, s);
    } else {
        if (event_snapshot_dir != NULL)
            lib_free(event_snapshot_dir);
        event_snapshot_dir = util_concat(s, FSDEV_DIR_SEP_STR, NULL);
    }

    return 0;
}

static int set_event_start_snapshot(resource_value_t v, void *param)
{
    if (util_string_set(&event_start_snapshot, (const char *)v))
        return 0;

    return 0;
}

static int set_event_end_snapshot(resource_value_t v, void *param)
{
    if (util_string_set(&event_end_snapshot, (const char *)v))
        return 0;

    return 0;
}

static int set_event_start_mode(resource_value_t v, void *param)
{
    unsigned int mode;

    mode = (unsigned int)v;

    if (mode != EVENT_START_MODE_FILE_SAVE
        && mode != EVENT_START_MODE_FILE_LOAD
        && mode != EVENT_START_MODE_RESET)
        return -1;

    event_start_mode = mode;

    return 0;
}

static const resource_t resources[] = {
    { "EventSnapshotDir", RES_STRING, 
      (resource_value_t)FSDEVICE_DEFAULT_DIR FSDEV_DIR_SEP_STR,
      (void *)&event_snapshot_dir, set_event_snapshot_dir, NULL },
    { "EventStartSnapshot", RES_STRING, (resource_value_t)EVENT_START_SNAPSHOT,
      (void *)&event_start_snapshot, set_event_start_snapshot, NULL },
    { "EventEndSnapshot", RES_STRING, (resource_value_t)EVENT_END_SNAPSHOT,
      (void *)&event_end_snapshot, set_event_end_snapshot, NULL },
    { "EventStartMode", RES_INTEGER,
      (resource_value_t)EVENT_START_MODE_FILE_SAVE,
      (void *)&event_start_mode, set_event_start_mode, NULL },
    { NULL }
};

int event_resources_init(void)
{
    return resources_register(resources);
}

void event_shutdown(void)
{
    lib_free(event_start_snapshot);
    lib_free(event_end_snapshot);
    lib_free(event_snapshot_dir);
    if (event_snapshot_path_str != NULL)
        lib_free(event_snapshot_path_str);
    destroy_list();
}

/*-----------------------------------------------------------------------*/

static int cmdline_help(const char *param, void *extra_param)
{
    return event_playback_start();
}

static const cmdline_option_t cmdline_options[] = {
    { "-playback", CALL_FUNCTION, 0, cmdline_help, NULL, NULL, NULL,
      NULL, "Playback recorded events" },
    { NULL }
};

int event_cmdline_options_init(void)
{
    return cmdline_register_options(cmdline_options);
}

/*-----------------------------------------------------------------------*/

void event_init(void)
{
    event_log = log_open("Event");

    event_alarm = alarm_new(maincpu_alarm_context, "Event",
                            event_alarm_handler, NULL);
}

