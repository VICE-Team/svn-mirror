/*
 * machine.c  - Interface to machine-specific implementations.
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

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "alarm.h"
#include "autostart.h"
#include "clkguard.h"
#include "cmdline.h"
#include "console.h"
#include "interrupt.h"
#include "log.h"
#include "machine.h"
#include "maincpu.h"
#include "mem.h"
#include "printer.h"
#include "resources.h"
#include "sound.h"
#include "types.h"
#include "ui.h"
#include "utils.h"
#include "vsync.h"

#ifdef HAS_JOYSTICK
#include "joy.h"
#endif


unsigned int machine_keymap_index;


unsigned int machine_jam(const char *format, ...)
{
    char *str;
    va_list ap;
    ui_jam_action_t ret;

    va_start(ap, format);

    str = xmvsprintf(format, ap);
    ret = ui_jam_dialog(str);
    free(str);

    switch (ret) {
      case UI_JAM_RESET:
        return JAM_RESET;
      case UI_JAM_HARD_RESET:
        return JAM_HARD_RESET;
      case UI_JAM_MONITOR:
        return JAM_MONITOR;
    }

    return JAM_NONE;
}

void machine_reset(void)
{
    log_message(LOG_DEFAULT, "Main CPU: RESET.");

    /* Do machine-specific initialization.  */
    machine_specific_reset();

    autostart_reset();

    mem_initialize_memory();

    vsync_suspend_speed_eval();
}

static void machine_maincpu_clk_overflow_callback(CLOCK sub, void *data)
{
    alarm_context_time_warp(maincpu_alarm_context, sub, -1);
    interrupt_cpu_status_time_warp(&maincpu_int_status, sub, -1);
}

void machine_maincpu_init(void)
{
    maincpu_alarm_context = (alarm_context_t *)xmalloc(sizeof(alarm_context_t));
    alarm_context_init(maincpu_alarm_context, "MainCPU");

    clk_guard_init(&maincpu_clk_guard, &maincpu_clk, CLOCK_MAX
                   - CLKGUARD_SUB_MIN);
    clk_guard_add_callback(&maincpu_clk_guard,
                           machine_maincpu_clk_overflow_callback, NULL);
}

void machine_shutdown(void)
{
    machine_specific_shutdown();

#ifdef HAS_JOYSTICK
    joystick_close();
#endif

    sound_close();

    printer_shutdown();

    console_close_all();

    cmdline_shutdown();

    resources_shutdown();

    log_close_all();
}

