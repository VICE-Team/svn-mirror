/*
 * debug.c - Various debugging options.
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
#include <string.h>

#include "archdep.h"
#include "cmdline.h"
#include "debug.h"
#include "drive.h"
#include "event.h"
#include "interrupt.h"
#include "lib.h"
#include "log.h"
#include "resources.h"
#include "types.h"
#include "ui.h"
#include "util.h"


debug_t debug;


inline static debug_history_step(const char *st);

static int set_do_core_dumps(resource_value_t v, void *param)
{
    debug.do_core_dumps = (int)v;
    return 0;
}

#ifdef DEBUG
static int set_maincpu_traceflg(resource_value_t v, void *param)
{
    debug.maincpu_traceflg = (int)v;
    return 0;
}

static int set_drive_traceflg(resource_value_t v, void *param)
{
    debug.drivecpu_traceflg[(int)param] = (int)v;
    return 0;
}

static int set_trace_mode(resource_value_t v, void *param)
{
    debug.trace_mode = (int)v;
    return 0;
}

#endif

/* Debug-related resources. */
static const resource_t resources[] = {
    { "DoCoreDump", RES_INTEGER, (resource_value_t)0,
      (void *)&debug.do_core_dumps, set_do_core_dumps, NULL },
#ifdef DEBUG
    { "MainCPU_TRACE", RES_INTEGER, (resource_value_t)0,
      (void *)&debug.maincpu_traceflg, set_maincpu_traceflg, NULL },
    { "Drive0CPU_TRACE", RES_INTEGER, (resource_value_t)0,
      (void *)&debug.drivecpu_traceflg[0], set_drive_traceflg, (void *)0 },
    { "Drive1CPU_TRACE", RES_INTEGER, (resource_value_t)0,
      (void *)&debug.drivecpu_traceflg[1], set_drive_traceflg, (void *)1 },
#if DRIVE_NUM > 2
    { "Drive2CPU_TRACE", RES_INTEGER, (resource_value_t)0,
      (void *)&debug.drivecpu_traceflg[2], set_drive_traceflg, (void *)2 },
#endif
#if DRIVE_NUM > 3
    { "Drive3CPU_TRACE", RES_INTEGER, (resource_value_t)0,
      (void *)&debug.drivecpu_traceflg[3], set_drive_traceflg, (void *)3 },
#endif
    { "TraceMode", RES_INTEGER, (resource_value_t)0,
      (void *)&debug.trace_mode, set_trace_mode, NULL },
#endif
    { NULL }
};

int debug_resources_init(void)
{
    return resources_register(resources);
}

static const cmdline_option_t cmdline_options[] = {
#ifdef DEBUG
    { "-trace_maincpu", SET_RESOURCE, 0, NULL, NULL,
      "MainCPU_TRACE", (resource_value_t)1,
      NULL, "Trace the main CPU" },
    { "+trace_maincpu", SET_RESOURCE, 0, NULL, NULL,
      "MainCPU_TRACE", (resource_value_t)0,
      NULL, "Do not trace the main CPU" },
    { "-trace_drive0", SET_RESOURCE, 0, NULL, NULL,
      "Drive0CPU_TRACE", (resource_value_t)1,
      NULL, "Trace the drive0 CPU" },
    { "+trace_drive0", SET_RESOURCE, 0, NULL, NULL,
      "Drive0CPU_TRACE", (resource_value_t)0,
      NULL, "Do not trace the drive0 CPU" },
    { "-trace_drive1", SET_RESOURCE, 0, NULL, NULL,
      "Drive1CPU_TRACE", (resource_value_t)1,
      NULL, "Trace the drive1 CPU" },
    { "+trace_drive1", SET_RESOURCE, 0, NULL, NULL,
      "Drive1CPU_TRACE", (resource_value_t)0,
      NULL, "Do not trace the drive1 CPU" },
#if DRIVE_NUM > 2
    { "-trace_drive2", SET_RESOURCE, 0, NULL, NULL,
      "Drive2CPU_TRACE", (resource_value_t)1,
      NULL, "Trace the drive2 CPU" },
    { "+trace_drive2", SET_RESOURCE, 0, NULL, NULL,
      "Drive2CPU_TRACE", (resource_value_t)0,
      NULL, "Do not trace the drive2 CPU" },
#endif
#if DRIVE_NUM > 3
    { "-trace_drive3", SET_RESOURCE, 0, NULL, NULL,
      "Drive3CPU_TRACE", (resource_value_t)1,
      NULL, "Trace the drive3 CPU" },
    { "+trace_drive3", SET_RESOURCE, 0, NULL, NULL,
      "Drive3CPU_TRACE", (resource_value_t)0,
      NULL, "Do not trace the drive3 CPU" },
#endif
    { "-trace_mode", SET_RESOURCE, 1, NULL, NULL,
      "TraceMode", NULL,
      "<value>", "0=normal 1=small 2=history" },
#endif
    { NULL }
};

int debug_cmdline_options_init(void)
{
    return cmdline_register_options(cmdline_options);
}

static unsigned int cycles_per_line;
static unsigned int screen_lines;

void debug_set_machine_parameter(unsigned int cycles, unsigned int lines)
{
    cycles_per_line = cycles;
    screen_lines = lines;
}

#ifdef DEBUG
#define RLINE(clk)  ((unsigned int)((clk) / cycles_per_line % screen_lines))
#define RCYCLE(clk) ((unsigned int)((clk) % cycles_per_line))

void debug_maincpu(DWORD reg_pc, CLOCK mclk, const char *dis, BYTE reg_a,
                   BYTE reg_x, BYTE reg_y, BYTE reg_sp)
{
    switch (debug.trace_mode) {
      case DEBUG_SMALL:
      {
        char small_dis[7];

        small_dis[0] = dis[0];
        small_dis[1] = dis[1];

        if (dis[3] == ' ') {
            small_dis[2] = '\0';
        } else {
            small_dis[2] = dis[3];
            small_dis[3] = dis[4];
            if (dis[6] == ' ') {
                small_dis[4] = '\0';
            } else {
                small_dis[4] = dis[6];
                small_dis[5] = dis[7];
                small_dis[6] = '\0';
            }  
        }
        
        log_debug("%04X %ld %02X%02X%02X %s", (unsigned int)reg_pc,
                  (long)mclk, reg_a, reg_x, reg_y, small_dis);
        break;
      }
      case DEBUG_HISTORY:
      {
        char st[DEBUG_MAXLINELEN];

        sprintf(st, ".%04X %02X %02X %8lX %-20s "
                  "%02X%02X%02X%02X", (unsigned int)reg_pc,
                  RLINE(mclk), RCYCLE(mclk), (long)mclk, dis,
                  reg_a, reg_x, reg_y, reg_sp);
        debug_history_step(st);
        break;
      }
      case DEBUG_NORMAL:
        log_debug(".%04X %03i %03i %10ld  %-20s "
                  "A=$%02X X=$%02X Y=$%02X SP=$%02X", (unsigned int)reg_pc,
                  RLINE(mclk), RCYCLE(mclk), (long)mclk, dis,
                  reg_a, reg_x, reg_y, reg_sp);
        break;
      default:
        log_debug("Unknown debug format.");

    }
}

void debug_drive(DWORD reg_pc, CLOCK mclk, const char *dis,
                 BYTE reg_a,BYTE reg_x, BYTE reg_y, BYTE reg_sp)
{
    char st[DEBUG_MAXLINELEN];

    sprintf(st, "Drive: .%04X %10ld %-20s %02x%02x%02x%02x", (unsigned int)reg_pc,
              (long)mclk, dis, reg_a, reg_x, reg_y, reg_sp);
    if (debug.trace_mode == DEBUG_HISTORY)
        debug_history_step(st);
    else
        log_debug(st);
}

void debug_text(const char *text)
{    if (debug.trace_mode == DEBUG_HISTORY)
        debug_history_step(text);
    else
        log_debug(text);
}

static void debug_int(interrupt_cpu_status_t *cs, const char *name,
                      unsigned int type)
{
    unsigned int i;
    char *textout, *texttmp;

    textout = lib_stralloc(name);

    for (i = 0; i < cs->num_ints; i++) {
        if (cs->pending_int[i] & type) {
            texttmp = util_concat(textout, " ", cs->int_name[i], NULL);
            lib_free(textout);
            textout = texttmp;
        }
    }

    if (debug.trace_mode == DEBUG_HISTORY)
        debug_history_step(textout);
    else
        log_debug(textout);

    lib_free(textout);
}

void debug_irq(interrupt_cpu_status_t *cs)
{
    debug_int(cs, "*** IRQ", IK_IRQ);
}

void debug_nmi(interrupt_cpu_status_t *cs)
{
    debug_int(cs, "*** NMI", IK_NMI);
}

/*------------------------------------------------------------------------*/

static FILE *debug_file = NULL;
static char *debug_buffer;
static int debug_buffer_ptr;
static int debug_buffer_size;
static int debug_file_current;
static int debug_file_line;
static int debug_file_milestone;

static void debug_close_file(void)
{
    if (debug_file != NULL) {
        fwrite(debug_buffer, sizeof(char), debug_buffer_ptr, debug_file);
        fclose(debug_file);
        debug_file = NULL;
        debug_buffer_ptr = 0;
        debug_file_current++;
    }
}

static void debug_create_new_file(void)
{
    char *filename, *directory;
    char st[256];

    debug_close_file();

    resources_get_value("EventSnapshotDir", (void *)&directory);
    sprintf(st, "debug%06d", debug_file_current);
    filename = util_concat(directory, st, FSDEV_EXT_SEP_STR, "log", NULL);
    debug_file = fopen(filename, MODE_WRITE_TEXT);
    lib_free(filename);
}

static debug_open_new_file(void)
{
    char *filename, *directory;
    char st[256];

    if (debug_file != NULL)
        fclose(debug_file);

    resources_get_value("EventSnapshotDir", (void *)&directory);
    sprintf(st, "debug%06d", debug_file_current);
    filename = util_concat(directory, st, FSDEV_EXT_SEP_STR, "log", NULL);
    debug_file = fopen(filename, MODE_READ_TEXT);
    if (debug_file != NULL) {
        debug_buffer_size = fread(debug_buffer, sizeof(char), 
                                    DEBUG_HISTORY_MAXFILESIZE, debug_file);
        debug_buffer_ptr = 0;
        debug_file_current++;
    } else {
        debug_buffer_size = 0;
    }

    debug_file_line = 0;

    lib_free(filename);

}

inline static debug_history_step(const char *st)
{
    if (event_record_active()) {
        
        if (debug_buffer_ptr + DEBUG_MAXLINELEN >= DEBUG_HISTORY_MAXFILESIZE) {
            debug_create_new_file();
        }
        
        debug_buffer_ptr += 
            sprintf(debug_buffer + debug_buffer_ptr, "%s\n", st);
    }

    if (event_playback_active()) {

        char tempstr[DEBUG_MAXLINELEN];
        int line_len = sprintf(tempstr, "%s\n", st);

        if (debug_buffer_ptr >= debug_buffer_size)
            debug_open_new_file();

        debug_file_line++;

        if (strncmp(st, debug_buffer + debug_buffer_ptr, strlen(st)) != 0) {
            event_playback_stop();
            ui_error("Playback error: %s different from line %d of file "
                     "debug%06d", st, debug_file_line, debug_file_current - 1);
        }

        debug_buffer_ptr += line_len;
    }
}

void debug_start_recording(void)
{
    debug_file_current = 0;
    debug_file_milestone = 0;
    debug_buffer_ptr = 0;
    debug_buffer = lib_malloc(DEBUG_HISTORY_MAXFILESIZE);
    debug_create_new_file();
}

void debug_stop_recording(void)
{
    debug_close_file();
    lib_free(debug_buffer);
}

void debug_start_playback(void)
{
    debug_file_current = 0;
    debug_file_milestone = 0;
    debug_buffer_ptr = 0;
    debug_buffer = lib_malloc(DEBUG_HISTORY_MAXFILESIZE);
    debug_open_new_file();
}

void debug_stop_playback(void)
{
    if (debug_file != NULL) {
        fclose(debug_file);
        debug_file = NULL;
    }
    lib_free(debug_buffer);
}

#endif

