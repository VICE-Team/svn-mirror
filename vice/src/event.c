/*
 * event.c - Event handling.
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
 *  Andreas Matthies <aDOTmatthiesATgmxDOTnet>
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
#include "network.h"
#include "resources.h"
#include "snapshot.h"
#include "tape.h"
#ifdef HAS_TRANSLATION
#include "translate.h"
#endif
#include "types.h"
#include "ui.h"
#include "util.h"
#include "version.h"


#define EVENT_START_SNAPSHOT "start" FSDEV_EXT_SEP_STR "vsf"
#define EVENT_END_SNAPSHOT "end" FSDEV_EXT_SEP_STR "vsf"
#define EVENT_MILESTONE_SNAPSHOT "milestone" FSDEV_EXT_SEP_STR "vsf"


struct event_image_list_s {
    char *orig_filename;
    char *mapped_filename;
    struct event_image_list_s *next;
};
typedef struct event_image_list_s event_image_list_t;

static event_list_state_t *event_list = NULL;
static event_image_list_t *event_image_list_base = NULL;
static int image_number;

static alarm_t *event_alarm = NULL;

static log_t event_log = LOG_DEFAULT;

static unsigned int playback_active = 0, record_active = 0;

static unsigned int current_timestamp, milestone_timestamp, playback_time;
static CLOCK next_timestamp_clk;
static CLOCK milestone_timestamp_alarm;

/* the VICE version an event history was made with */
static char event_version[16];

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


/* searches for a filename in the image list    */
/* returns 0 if found                           */
/* returns 1 and appends it if not found        */
static int event_image_append(const char *filename, 
                              char **mapped_name, int append)
{
    event_image_list_t *event_image_list_ptr = event_image_list_base;

    while (event_image_list_ptr->next != NULL) {
        if (strcmp(filename, event_image_list_ptr->next->orig_filename) == 0) {
            if (mapped_name != NULL) {
                if (append == 0)
                    *mapped_name = lib_stralloc(event_image_list_ptr->next->mapped_filename);
                else
                    event_image_list_ptr->next->mapped_filename = lib_stralloc(*mapped_name);
            }
            return 0;
        }

        event_image_list_ptr = event_image_list_ptr->next;
    }

    event_image_list_ptr->next = (
        event_image_list_t *) lib_calloc(1, sizeof(event_image_list_t));

    event_image_list_ptr = event_image_list_ptr->next;
    event_image_list_ptr->next = NULL;
    event_image_list_ptr->orig_filename = lib_stralloc(filename);
    event_image_list_ptr->mapped_filename = NULL;
    if (mapped_name != NULL)
        event_image_list_ptr->mapped_filename = lib_stralloc(*mapped_name);

    return 1;
}


void event_record_attach_in_list(event_list_state_t *list, unsigned int unit,
                                 const char *filename, unsigned int read_only)
{
    char *event_data;
    unsigned int size;

    list->current->type = EVENT_ATTACHIMAGE;
    list->current->clk = maincpu_clk;
    list->current->next
        = (event_list_t *)lib_calloc(1, sizeof(event_list_t));

    size = strlen(filename) + 3;

    event_data = lib_malloc(size);
    event_data[0] = unit;
    event_data[1] = read_only;
    strcpy(&event_data[2], filename);

    if (event_image_append(filename, NULL, 0) == 1) {
        FILE *fd;
        size_t file_len = 0;
        
        fd = fopen(filename, MODE_READ);

        if (fd != NULL) {
            file_len = util_file_length(fd);
            event_data = lib_realloc(event_data, size + file_len);

            if (fread(&event_data[size], file_len, 1, fd) != 1)
                log_error(event_log, "Cannot load image file %s", filename);

            fclose(fd);
        } else {
            log_error(event_log, "Cannot open image file %s", filename);
        }
        size += file_len;
    }

    list->current->size = size;
    list->current->data = event_data;
    list->current = list->current->next;
}

void event_record_attach_image(unsigned int unit, const char *filename,
                               unsigned int read_only)
{
    if (record_active == 0)
        return;

    event_record_attach_in_list(event_list, unit, filename, read_only);
}


static void event_playback_attach_image(void *data, unsigned int size)
{
    unsigned int unit, read_only;
    char *orig_filename, *filename;
    size_t file_len;
    char *str;

    unit = (unsigned int)((char*)data)[0];
    read_only = (unsigned int)((char*)data)[1];
    orig_filename = &((char*)data)[2];
    file_len  = size - strlen(orig_filename) - 3;

    if (file_len > 0) {
        FILE *fd;

        str = archdep_tmpnam();
        filename = util_concat(str, FSDEV_EXT_SEP_STR,
                                util_get_extension(orig_filename), NULL);
        lib_free(str);

        fd = fopen(filename, MODE_WRITE);
        if (fd == NULL) {
#ifdef HAS_TRANSLATION
            ui_error(translate_text(IDGS_CANNOT_CREATE_IMAGE_S), filename);
#else
            ui_error(_("Cannot create image file %s"), filename);
#endif
            goto error;
        }

        if (fwrite((char*)data + strlen(orig_filename) + 3, file_len, 1, fd) != 1) {
#ifdef HAS_TRANSLATION
            ui_error(translate_text(IDGS_CANNOT_WRITE_IMAGE_FILE_S), filename);
#else
            ui_error(_("Cannot write image file %s"), filename);
#endif
            goto error;
        }

        fclose(fd);
        event_image_append(orig_filename, &filename, 1);
    } else {
        if (event_image_append(orig_filename, &filename, 0) != 0) {
#ifdef HAS_TRANSLATION
            ui_error(translate_text(IDGS_CANNOT_FIND_MAPPED_NAME_S), orig_filename);
#else
            ui_error(_("Cannot find mapped name for %s"), orig_filename);
#endif
            return;
        }
    }

    /* now filename holds the name to attach    */
    /* FIXME: read_only isn't handled for tape  */
    if (unit == 1) {
        tape_image_event_playback(unit, filename);
    } else {
        resources_set_sprintf("AttachDevice%dReadonly", 
                                (resource_value_t) read_only, unit);
        file_system_event_playback(unit, filename);
    }
    
error:
    lib_free(filename);
}


void event_record_in_list(event_list_state_t *list, unsigned int type,
                          void *data, unsigned int size)
{
    void *event_data = NULL;

    if (record_active == 0 && !network_connected())
        return;

    /*log_debug("EVENT RECORD %i CLK %i", type, maincpu_clk);*/

    switch (type) {
      case EVENT_RESETCPU:
        next_timestamp_clk -= maincpu_clk;
      case EVENT_KEYBOARD_MATRIX:
      case EVENT_KEYBOARD_RESTORE:
      case EVENT_KEYBOARD_DELAY:
      case EVENT_JOYSTICK_VALUE:
      case EVENT_DATASETTE:
      case EVENT_ATTACHDISK:
      case EVENT_ATTACHTAPE:
      case EVENT_ATTACHIMAGE:
      case EVENT_INITIAL:
      case EVENT_SYNC_TEST:
        event_data = lib_malloc(size);
        memcpy(event_data, data, size);
        break;
      case EVENT_LIST_END:
      case EVENT_OVERFLOW:
        break;
      default:
        /*log_error(event_log, "Unknow event type %i.", type);*/
        return;
    }

    list->current->type = type;
    list->current->clk = maincpu_clk;
    list->current->size = size;
    list->current->data = event_data;
    list->current->next
        = (event_list_t *)lib_calloc(1, sizeof(event_list_t));
    list->current = list->current->next;
}

void event_record(unsigned int type, void *data, unsigned int size)
{
    if (!network_connected())
        event_record_in_list(event_list, type, data, size);
}


static void next_alarm_set(void)
{
    CLOCK new_value;

    new_value = event_list->current->clk;

    if (maincpu_clk > CLKGUARD_SUB_MIN
        && new_value < maincpu_clk - CLKGUARD_SUB_MIN)
        new_value += clk_guard_clock_sub(maincpu_clk_guard);

    alarm_set(event_alarm, new_value);
}

static void next_current_list(void)
{
    event_list->current = event_list->current->next;
}

static void event_alarm_handler(CLOCK offset, void *data)
{
    alarm_unset(event_alarm);

    /* when recording set a timestamp */
    if (record_active) {
        ui_display_event_time(current_timestamp++, 0);
        next_timestamp_clk = next_timestamp_clk 
                                + machine_get_cycles_per_second();
        alarm_set(event_alarm, next_timestamp_clk);
        return;
    }

    /*log_debug("EVENT PLAYBACK %i CLK %i", event_list_current->type,
              event_list_current->clk);*/

    switch (event_list->current->type) {
      case EVENT_KEYBOARD_MATRIX:
        keyboard_event_playback(offset, event_list->current->data);
        break;
      case EVENT_KEYBOARD_RESTORE:
        keyboard_restore_event_playback(offset, event_list->current->data);
        break;
      case EVENT_JOYSTICK_VALUE:
        joystick_event_playback(offset, event_list->current->data);
        break;
      case EVENT_DATASETTE:
        datasette_event_playback(offset, event_list->current->data);
        break;
      case EVENT_ATTACHIMAGE:
        event_playback_attach_image(event_list->current->data,
                                    event_list->current->size);
        break;
      case EVENT_ATTACHDISK:
      case EVENT_ATTACHTAPE:
        {
            /* old style attach via absolute filename and detach*/
            unsigned int unit;
            const char *filename;

            unit = (unsigned int)((char*)event_list->current->data)[0];
            filename = &((char*)event_list->current->data)[1];
            
            if (unit == 1)
                tape_image_event_playback(unit, filename);
            else
                file_system_event_playback(unit, filename);
        }
        break;
      case EVENT_RESETCPU:
        machine_reset_event_playback(offset, event_list->current->data);
        break;
      case EVENT_TIMESTAMP:
        ui_display_event_time(current_timestamp++, playback_time);
        break;
      case EVENT_LIST_END:
        event_playback_stop();
        break;
      case EVENT_OVERFLOW:
        break;
      default:
        log_error(event_log, "Unknow event type %i.", event_list->current->type);
    }

    if (event_list->current->type != EVENT_LIST_END
        && event_list->current->type != EVENT_RESETCPU) {
        next_current_list();
        next_alarm_set();
    }
}

/*-----------------------------------------------------------------------*/
void event_playback_event_list(event_list_state_t *list)
{
    event_list_t *current = list->base;

    while (current->type != EVENT_LIST_END) {
        switch (current->type) {
            case EVENT_SYNC_TEST:
                break;
            case EVENT_KEYBOARD_DELAY:
                keyboard_register_delay(*(unsigned int*)current->data);
                break;
            case EVENT_KEYBOARD_MATRIX:
                keyboard_event_delayed_playback(current->data);
                break;
            case EVENT_KEYBOARD_RESTORE:
                keyboard_restore_event_playback(0, current->data);
                break;
            case EVENT_JOYSTICK_DELAY:
                joystick_register_delay(*(unsigned int*)current->data);
                break;
            case EVENT_JOYSTICK_VALUE:
                joystick_event_delayed_playback(current->data);
                break;
            case EVENT_DATASETTE:
                datasette_event_playback(0, current->data);
                break;
            case EVENT_RESETCPU:
                machine_reset_event_playback(0, current->data);
                break;
            case EVENT_ATTACHDISK:
            case EVENT_ATTACHTAPE:
              {
                /* in fact this is only for detaching */
                unsigned int unit;

                unit = (unsigned int)((char*)current->data)[0];
            
                if (unit == 1)
                    tape_image_event_playback(1, NULL);
                else
                    file_system_event_playback(unit, NULL);
                break;
              }
            case EVENT_ATTACHIMAGE:
                event_playback_attach_image(current->data, current->size);
                break;
            default:
                log_error(event_log, "Unknow event type %i.", current->type);
        }
        current = current->next;
    }
}

void event_register_event_list(event_list_state_t *list)
{
    list->base = (event_list_t *)lib_calloc(1, sizeof(event_list_t));
    list->current = list->base;
}

void event_init_image_list(void)
{
    event_image_list_base = 
        (event_image_list_t *)lib_calloc(1, sizeof(event_image_list_t));
    image_number = 0;
}

static void create_list(void)
{
    event_list = (event_list_state_t *)lib_malloc(sizeof(event_list_state_t));
    event_register_event_list(event_list);
    event_init_image_list();
}


static void cut_list(event_list_t *cut_base)
{
    event_list_t *c1, *c2;

    c1 = cut_base;

    while (c1 != NULL) {
        c2 = c1->next;
        lib_free(c1->data);
        lib_free(c1);
        c1 = c2;
    }
}

void event_destroy_image_list(void)
{
    event_image_list_t *d1, *d2;
 
    d1 = event_image_list_base;

    while (d1 != NULL) {
        d2 = d1->next;
        lib_free(d1->orig_filename);
        if (d1->mapped_filename != NULL)
            lib_free(d1->mapped_filename);
        lib_free(d1);
        d1 = d2;
    }

    event_image_list_base = NULL;
}

void event_clear_list(event_list_state_t *list)
{
    if (list != NULL && list->base != NULL)
        cut_list(list->base);
}

static void destroy_list(void)
{
    event_clear_list(event_list);
    lib_free(event_list);
    event_destroy_image_list();
}

static void warp_end_list(void)
{
    event_list_t *curr;

    curr = event_list->base;

    while (curr->type != EVENT_LIST_END) {

        if (curr->type == EVENT_ATTACHIMAGE)
            event_image_append(&((char*)curr->data)[2], NULL, 0);

        curr = curr->next;
    }

    memset(curr, 0, sizeof(event_list_t));
    event_list->current = curr;
}
/*-----------------------------------------------------------------------*/
/* writes or replaces version string in the initial event                */
static void event_write_version(void)
{
    BYTE *new_data;
    BYTE *data;
    unsigned int ver_idx;

    if (event_list->base->type != EVENT_INITIAL) {
        /* EVENT_INITIAL is missing (bug in 1.14.xx); fix it */
        event_list_t *new_event;

        new_event = (event_list_t *)lib_calloc(1, sizeof(event_list_t));
        new_event->clk = event_list->base->clk;
        new_event->size = strlen(event_start_snapshot) + 2;
        new_event->type = EVENT_INITIAL;
        data = lib_malloc(new_event->size);
        data[0] = EVENT_START_MODE_FILE_SAVE;
        strcpy((char *)&data[1], event_start_snapshot);
        new_event->data = data;
        new_event->next = event_list->base;
        event_list->base = new_event;
    }

    data = event_list->base->data;

    ver_idx = 1;
    if (data[0] == EVENT_START_MODE_FILE_SAVE)
        ver_idx += strlen((char *)&data[1]) + 1;

    event_list->base->size = ver_idx + strlen(VERSION) + 1;
    new_data = lib_malloc(event_list->base->size);

    memcpy(new_data, data, ver_idx);

    strcpy((char *)&new_data[ver_idx], VERSION);

    event_list->base->data = new_data;
    lib_free(data);
}

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

    event_write_version();

    lib_free(data);
}

/*-----------------------------------------------------------------------*/

static void event_record_start_trap(WORD addr, void *data)
{
    switch (event_start_mode) {
      case EVENT_START_MODE_FILE_SAVE:
        if (machine_write_snapshot(event_snapshot_path(event_start_snapshot),
                                    1, 1, 0) < 0) {
#ifdef HAS_TRANSLATION
            ui_error(translate_text(IDGS_CANT_CREATE_START_SNAP_S), 
#else
            ui_error(_("Could not create start snapshot file %s."), 
#endif
                        event_snapshot_path(event_start_snapshot));
            ui_display_recording(0);
            return;
        }
        destroy_list();
        create_list();
        record_active = 1;
        event_initial_write();
        next_timestamp_clk = maincpu_clk;
        current_timestamp = 0;
        break;
      case EVENT_START_MODE_FILE_LOAD:
        if (machine_read_snapshot(
                event_snapshot_path(event_end_snapshot), 1) < 0) {
#ifdef HAS_TRANSLATION
            ui_error(translate_text(IDGS_ERROR_READING_END_SNAP_S),
#else
            ui_error(_("Error reading end snapshot file %s."),
#endif
                        event_snapshot_path(event_end_snapshot));
            return;
        }
        warp_end_list();
        record_active = 1;
        next_timestamp_clk = maincpu_clk;
        current_timestamp = playback_time;
        break;
      case EVENT_START_MODE_RESET:
        machine_trigger_reset(MACHINE_RESET_MODE_HARD);
        destroy_list();
        create_list();
        record_active = 1;
        event_initial_write();
        next_timestamp_clk = 0;
        current_timestamp = 0;
        break;
      case EVENT_START_MODE_PLAYBACK:
        cut_list(event_list->current->next);
        event_destroy_image_list();
        event_write_version();
        record_active = 1;
        next_timestamp_clk = maincpu_clk;
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
    alarm_set(event_alarm, next_timestamp_clk);
}

int event_record_start(void)
{
    if (event_start_mode == EVENT_START_MODE_PLAYBACK) {
        if (playback_active != 0)
            event_playback_stop();
        else
            return -1;
    }

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
#ifdef HAS_TRANSLATION
        ui_error(translate_text(IDGS_CANT_CREATE_END_SNAP_S),
#else
        ui_error(_("Could not create end snapshot file %s."),
#endif
                    event_snapshot_path(event_end_snapshot));
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
    if (event_list == NULL)
        return;

    if (playback_reset_ack) {
        playback_reset_ack = 0;
        next_alarm_set();
    }

    if (event_list->current 
        && event_list->current->type == EVENT_RESETCPU)
    {
        next_current_list();
        next_alarm_set();
    }

    /* timestamp alarm needs to be set */
    if (record_active)
        alarm_set(event_alarm, next_timestamp_clk);
}

static void event_playback_start_trap(WORD addr, void *data)
{
    snapshot_t *s;
    BYTE minor, major;

    event_version[0] = 0;

    s = snapshot_open(
        event_snapshot_path(event_end_snapshot), &major, &minor, machine_name);

    if (s == NULL) {
#ifdef HAS_TRANSLATION
        ui_error(translate_text(IDGS_CANT_OPEN_END_SNAP_S), 
#else
        ui_error(_("Could not open end snapshot file %s."), 
#endif
                    event_snapshot_path(event_end_snapshot));
        ui_display_playback(0, NULL);
        return;
    }

    destroy_list();
    create_list();

    if (event_snapshot_read_module(s, 1) < 0) {
        snapshot_close(s);
#ifdef HAS_TRANSLATION
        ui_error(translate_text(IDGS_CANT_FIND_SECTION_END_SNAP));
#else
        ui_error(_("Could not find event section in end snapshot file."));
#endif
        ui_display_playback(0, NULL);
        return;
    }

    snapshot_close(s);

    event_list->current = event_list->base;

    if (event_list->current->type == EVENT_INITIAL) {
        BYTE *data = (BYTE *)(event_list->current->data);
        switch (data[0]) {
          case EVENT_START_MODE_FILE_SAVE:
            /*log_debug("READING %s", (char *)(&data[1]));*/
            if (machine_read_snapshot(
                    event_snapshot_path((char *)(&data[1])), 0) < 0
                && machine_read_snapshot(
                    event_snapshot_path(event_start_snapshot), 0) < 0)
            {
                char *st = lib_stralloc(event_snapshot_path((char *)(&data[1])));
#ifdef HAS_TRANSLATION
                ui_error(translate_text(IDGS_ERROR_READING_START_SNAP_TRIED),
#else
                ui_error(_("Error reading start snapshot file. Tried %s and %s"),
#endif
                            st, event_snapshot_path(event_start_snapshot));
                lib_free(st);
                ui_display_playback(0, NULL);
                return;
            }

            if (event_list->current->size > strlen((char *)&data[1]) + 2)
                strncpy(event_version, (char *)(&data[strlen((char *)&data[1]) + 2]), 15);

            next_current_list();
            next_alarm_set();
            break;
          case EVENT_START_MODE_RESET:
            /*log_debug("RESET MODE!");*/
            machine_trigger_reset(MACHINE_RESET_MODE_HARD);
            if (event_list->current->size > 1)
                strncpy(event_version, (char *)(&data[1]), 15);
            next_current_list();
            /* Alarm will be set if reset is ack'ed.  */
            playback_reset_ack = 1;
            break;
        }
    } else {
        if (machine_read_snapshot(
                event_snapshot_path(event_start_snapshot), 0) < 0) {
#ifdef HAS_TRANSLATION
            ui_error(translate_text(IDGS_ERROR_READING_START_SNAP));
#else
            ui_error(_("Error reading start snapshot file."));
#endif
            ui_display_playback(0, NULL);
            return;
        }
        next_alarm_set();
    }

    playback_active = 1;
    current_timestamp = 0;

    ui_display_playback(1, event_version);

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

    return 0;
}

int event_playback_stop(void)
{
    if (playback_active == 0)
        return -1;

    playback_active = 0;

    alarm_unset(event_alarm);

    ui_display_playback(0, NULL);

#ifdef  DEBUG
    debug_stop_playback();
#endif

    return 0;
}

static void event_record_set_milestone_trap(WORD addr, void *data)
{
    if (machine_write_snapshot(
        event_snapshot_path(event_end_snapshot), 1, 1, 1) < 0) {
#ifdef HAS_TRANSLATION
            ui_error(translate_text(IDGS_CANT_CREATE_END_SNAP_S),
#else
            ui_error(_("Could not create end snapshot file %s."),
#endif
                        event_snapshot_path(event_end_snapshot));
    } else {
        milestone_timestamp_alarm = next_timestamp_clk;
        milestone_timestamp = current_timestamp;
#ifdef  DEBUG
        debug_set_milestone();
#endif
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
#ifdef HAS_TRANSLATION
        ui_error(translate_text(IDGS_ERROR_READING_END_SNAP_S),
#else
        ui_error(_("Error reading end snapshot file %s."),
#endif
                    event_snapshot_path(event_end_snapshot));
        return;
    }
    warp_end_list();
    record_active = 1;
    if (milestone_timestamp_alarm > 0) {
        alarm_set(event_alarm, milestone_timestamp_alarm);
        next_timestamp_clk = milestone_timestamp_alarm;
        current_timestamp = milestone_timestamp;
    }
#ifdef  DEBUG
    debug_reset_milestone();
#endif
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

int event_network_start(void)
{
    return 0;
}

int event_network_stop(void)
{
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

    curr = event_list->base;
    num_of_timestamps = 0;
    playback_time = 0;
    next_timestamp_clk = CLOCK_MAX;

    while (1) {
        unsigned int type, size;
        CLOCK clk;
        BYTE *data = NULL;

        /* 
            throw away recorded timestamp (recording them  was introduced in
            1.14.x so there might exist history files with TIMESTAMP events)
        */
        do {
            if (SMR_DW_INT(m, (int*)&(type)) < 0) {
                snapshot_module_close(m);
                return -1;
            }

            if (SMR_DW(m, &(clk)) < 0) {
                snapshot_module_close(m);
                return -1;
            }

            if (SMR_DW_INT(m, (int*)&(size)) < 0) {
                snapshot_module_close(m);
                return -1;
            }

        } while (type == EVENT_TIMESTAMP);

        if (size > 0) {
            data = lib_malloc(size);
            if (SMR_BA(m, data, size) < 0) {
                snapshot_module_close(m);
                return -1;
            }
        }

        if (next_timestamp_clk == CLOCK_MAX) /* if EVENT_INITIAL is missing */
            next_timestamp_clk = clk;

        if (type == EVENT_INITIAL) {
            if (data[0] == EVENT_START_MODE_RESET)
                next_timestamp_clk = 0;
            else
                next_timestamp_clk = clk;
        } else {
            /* insert timestamps each second */
            while (next_timestamp_clk < clk || (type == EVENT_OVERFLOW 
                && next_timestamp_clk < maincpu_clk_guard->clk_max_value))
            {
                curr->type = EVENT_TIMESTAMP;
                curr->clk = next_timestamp_clk;
                curr->size = 0;
                curr->next = (event_list_t *)lib_calloc(1, sizeof(event_list_t));
                curr = curr->next;
                next_timestamp_clk += machine_get_cycles_per_second();
                num_of_timestamps++;
            }

            if (type == EVENT_OVERFLOW)
                    next_timestamp_clk -= clk_guard_clock_sub(maincpu_clk_guard);
        }

        curr->type = type;
        curr->clk = clk;
        curr->size = size;
        curr->data = (size > 0 ? data : NULL);

        if (type == EVENT_LIST_END)
            break;

        if (type == EVENT_RESETCPU)
            next_timestamp_clk -= clk;

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

    curr = event_list->base;

    while (curr != NULL) {
        if (curr->type != EVENT_TIMESTAMP
            && (0
            || SMW_DW(m, (DWORD)curr->type) < 0
            || SMW_DW(m, (DWORD)curr->clk) < 0
            || SMW_DW(m, (DWORD)curr->size) < 0
            || SMW_BA(m, curr->data, curr->size) < 0)) {
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
        && mode != EVENT_START_MODE_RESET
        && mode != EVENT_START_MODE_PLAYBACK)
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

#ifdef HAS_TRANSLATION
static const cmdline_option_t cmdline_options[] = {
    { "-playback", CALL_FUNCTION, 0, cmdline_help, NULL, NULL, NULL,
      0, IDCLS_PLAYBACK_RECORDED_EVENTS },
    { NULL }
};
#else
static const cmdline_option_t cmdline_options[] = {
    { "-playback", CALL_FUNCTION, 0, cmdline_help, NULL, NULL, NULL,
      NULL, N_("Playback recorded events") },
    { NULL }
};
#endif

int event_cmdline_options_init(void)
{
    return cmdline_register_options(cmdline_options);
}

/*-----------------------------------------------------------------------*/

static void clk_overflow_callback(CLOCK sub, void *data)
{
    if (event_record_active())
        event_record(EVENT_OVERFLOW, NULL, 0);

    if (next_timestamp_clk)
        next_timestamp_clk -= sub;
}


void event_init(void)
{
    event_log = log_open("Event");

    event_alarm = alarm_new(maincpu_alarm_context, "Event",
                            event_alarm_handler, NULL);

    clk_guard_add_callback(maincpu_clk_guard, clk_overflow_callback, NULL);
}

