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

#include "cmdline.h"
#include "debug.h"
#include "interrupt.h"
#include "lib.h"
#include "log.h"
#include "resources.h"
#include "types.h"
#include "utils.h"


debug_t debug;


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

static int set_trace_small(resource_value_t v, void *param)
{
    debug.trace_small = (int)v;
    return 0;
}

#endif

/* Debug-related resources. */
static const resource_t resources[] = {
    { "DoCoreDump", RES_INTEGER, (resource_value_t)0,
      (resource_value_t *)&debug.do_core_dumps,
      set_do_core_dumps, NULL },
#ifdef DEBUG
    { "MainCPU_TRACE", RES_INTEGER, (resource_value_t)0,
      (resource_value_t *)&debug.maincpu_traceflg,
      set_maincpu_traceflg, NULL },
    { "Drive0CPU_TRACE", RES_INTEGER, (resource_value_t)0,
      (resource_value_t *)&debug.drivecpu_traceflg[0],
      set_drive_traceflg, (void *)0 },
    { "Drive1CPU_TRACE", RES_INTEGER, (resource_value_t)0,
      (resource_value_t *)&debug.drivecpu_traceflg[1],
      set_drive_traceflg, (void *)1 },
    { "TraceSmall", RES_INTEGER, (resource_value_t)0,
      (resource_value_t *)&debug.trace_small,
      set_trace_small, NULL },
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
    { "-trace_small", SET_RESOURCE, 0, NULL, NULL,
      "TraceSmall", (resource_value_t)1,
      NULL, "Make debug output small and compact" },
    { "+trace_small", SET_RESOURCE, 0, NULL, NULL,
      "TraceSmall", (resource_value_t)0,
      NULL, "Do not make debug output small and compact" },
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
    if (debug.trace_small) {
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
    } else {
        log_debug(".%04X %03i %03i %10ld  %-20s "
                  "A=$%02X X=$%02X Y=$%02X SP=$%02X", (unsigned int)reg_pc,
                  RLINE(mclk), RCYCLE(mclk), (long)mclk, dis,
                  reg_a, reg_x, reg_y, reg_sp);
    }
}

void debug_drive(DWORD reg_pc, CLOCK mclk, const char *dis, BYTE reg_a)
{
    log_debug("Drive: .%04X %10ld %-20s A=$%02x.", (unsigned int)reg_pc,
              (long)mclk, dis, reg_a);
}

void debug_text(const char *text)
{
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
#endif

