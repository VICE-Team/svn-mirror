/*
 * init.c - General initialization.
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
 *  Ettore Perazzoli <ettore@comm2000.it>
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

#include "archdep.h"
#include "attach.h"
#include "cmdline.h"
#include "console.h"
#include "debug.h"
#include "diskimage.h"
#include "drive.h"
#include "drivecpu.h"
#include "fsdevice.h"
#include "gfxoutput.h"
#include "initcmdline.h"
#include "joy.h"
#include "joystick.h"
#include "kbdbuf.h"
#include "keyboard.h"
#include "log.h"
#include "machine.h"
#include "maincpu.h"
#include "palette.h"
#include "resources.h"
#include "screenshot.h"
#include "sysfile.h"
#include "ui.h"
#include "vdrive.h"


int init_resources(void)
{
    if (resources_init(machine_name)) {
        archdep_startup_log_error("Cannot initialize resource handling.\n");
        return -1;
    }
    if (log_resources_init() < 0) {
        archdep_startup_log_error("Cannot initialize log resource handling.\n");
        return -1;
    }
    if (sysfile_resources_init() < 0) {
        archdep_startup_log_error("Cannot initialize resources for the system file locator.\n");
        return -1;
    }
    if (ui_resources_init() < 0) {
        archdep_startup_log_error("Cannot initialize UI-specific resources.\n");
        return -1;
    }
    if (file_system_resources_init() < 0) {
        archdep_startup_log_error("Cannot initialize file system-specific resources.\n");
        return -1;
    }
    /* Initialize file system device-specific resources.  */
    if (fsdevice_resources_init() < 0) {
        archdep_startup_log_error("Cannot initialize file system device-specific resources.\n");
        return -1;
    }
#ifdef DEBUG
    if (debug_resources_init() < 0) {
        archdep_startup_log_error("Cannot initialize debug resources.\n");
        return -1;
    }
#endif
    if (machine_resources_init() < 0) {
        archdep_startup_log_error("Cannot initialize machine-specific resources.\n");
        return -1;
    }
    if (joystick_init_resources() < 0) {
        archdep_startup_log_error("Cannot initialize joystick-specific resources.\n");
        return -1;
    }
    return 0;
}

int init_cmdline_options(void)
{
    if (cmdline_init()) {
        archdep_startup_log_error("Cannot initialize resource handling.\n");
        return -1;
    }
    if (log_cmdline_options_init() < 0) {
        archdep_startup_log_error("Cannot initialize log command-line option handling.\n");
        return -1;
    }
    if (initcmdline_init() < 0) {
        archdep_startup_log_error("Cannot initialize main command-line options.\n");
        return -1;
    }
    if (sysfile_cmdline_options_init() < 0) {
        archdep_startup_log_error("Cannot initialize command-line options for system file locator.\n");
        return -1;
    }
    if (!vsid_mode && ui_cmdline_options_init() < 0) {
        archdep_startup_log_error("Cannot initialize UI-specific command-line options.\n");
        return -1;
    }
    if (!vsid_mode && file_system_cmdline_options_init() < 0) {
        archdep_startup_log_error("Cannot initialize Attach-specific command-line options.\n");
        return -1;
    }
#ifdef DEBUG
    if (debug_cmdline_options_init() < 0) {
        archdep_startup_log_error("Cannot initialize debug-specific command-line options.\n");
        return -1;
    }
#endif
    if (machine_cmdline_options_init() < 0) {
        archdep_startup_log_error("Cannot initialize machine-specific command-line options.\n");
        return -1;
    }

    if (vsid_mode) {
        return 0;
    }

    if (fsdevice_cmdline_options_init() < 0) {
        archdep_startup_log_error("Cannot initialize file system-specific command-line options.\n");
        return -1;
    }
    if (joystick_init_cmdline_options() < 0) {
        archdep_startup_log_error("Cannot initialize joystick-specific command-line options.\n");
        return -1;
    }
    if (kbd_buf_cmdline_options_init() < 0) {
        archdep_startup_log_error("Cannot initialize keyboard buffer-specific command-line options.\n");
        return -1;
    }

    return 0;
}

int init_main(void)
{
    archdep_setup_signals(debug.do_core_dumps);

    if (!vsid_mode) {
        palette_init();
        gfxoutput_init();
        screenshot_init();

        drive0_cpu_early_init();
        drive1_cpu_early_init();
    }

    maincpu_init();

    /* Machine-specific initialization.  */
    if (machine_init() < 0) {
        log_error(LOG_DEFAULT, "Machine initialization failed.");
        return -1;
    }

    /* FIXME: what's about uimon_init??? */
    if (!vsid_mode && console_init() < 0) {
        log_error(LOG_DEFAULT, "Console initialization failed.");
        return -1;
    }

    keyboard_init();

    if (!vsid_mode) {
        joystick_init();
        disk_image_init();
        vdrive_init();
    }

    ui_init_finalize();

    return 0;
}

