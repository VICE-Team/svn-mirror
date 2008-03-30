/*
 * machine.c  - Interface to machine-specific implementations.
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

#include <stdarg.h>
#include <stdlib.h>

#include "autostart.h"
#include "machine.h"
#include "mem.h"
#include "ui.h"
#include "utils.h"
#include "vsync.h"


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
    /* Do machine-specific initialization.  */
    machine_specific_reset();

    autostart_reset();

    mem_initialize_memory();

    vsync_suspend_speed_eval();
}

