/*
 * plus4.c
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

#include "attach.h"
#include "autostart.h"
#include "clkguard.h"
#include "datasette.h"
#include "debug.h"
#include "drive-cmdline-options.h"
#include "drive-resources.h"
#include "drive.h"
#include "drivecpu.h"
#include "iecdrive.h"
#include "imagecontents.h"
#include "kbdbuf.h"
#include "keyboard.h"
#include "log.h"
#include "machine-drive.h"
#include "machine.h"
#include "maincpu.h"
#include "monitor.h"
#include "plus4-cmdline-options.h"
#include "plus4-resources.h"
#include "plus4-snapshot.h"
#include "plus4.h"
#include "plus4acia.h"
#include "plus4iec.h"
#include "plus4mem.h"
#include "plus4tcbm.h"
#include "plus4ui.h"
#include "printer.h"
#include "rs232drv.h"
#include "screenshot.h"
#include "serial.h"
#include "sound.h"
#include "tape.h"
#include "ted-cmdline-options.h"
#include "ted-resources.h"
#include "ted.h"
#include "traps.h"
#include "types.h"
#include "video.h"
#include "vsync.h"


#define NUM_KEYBOARD_MAPPINGS 2

const char *machine_keymap_res_name_list[NUM_KEYBOARD_MAPPINGS] = {
    "KeymapSymFile", "KeymapPosFile"
};

char *machine_keymap_file_list[NUM_KEYBOARD_MAPPINGS] = {
    NULL, NULL
};

const char machine_name[] = "PLUS4";
int machine_class = VICE_MACHINE_PLUS4;

static void machine_vsync_hook(void);

/* ------------------------------------------------------------------------- */

static const trap_t plus4_serial_traps[] = {
    {
        "SerialListen",
        0xE16B,
        0xE1E7,
        { 0x20, 0xC6, 0xE2 },
        serial_trap_attention,
        kernal_read,
        kernal_store
    },
    {
        "SerialSaListen",
        0xE177,
        0xE1E7,
        { 0x78, 0x20, 0xBF },
        serial_trap_attention,
        kernal_read,
        kernal_store
    },
    {
        "SerialSendByte",
        0xE181,
        0xE1E7,
        { 0x78, 0x20, 0xC6 },
        serial_trap_send,
        kernal_read,
        kernal_store
    },
/*
    {
        "SerialSendByte2",
        0xE158,
        0xE1E7,
        { 0x48, 0x24, 0x94 },
        serial_trap_send,
        kernal_read,
        kernal_store
    },
*/
    {
        "SerialReceiveByte",
        0xE252,
        0xE1E7,
        { 0x78, 0xA9, 0x00 },
        serial_trap_receive,
        kernal_read,
        kernal_store
    },
    {
        "SerialReady",
        0xE216,
        0xE1E7,
        { 0x24, 0x01, 0x70 },
        serial_trap_ready,
        kernal_read,
        kernal_store
    },
    {
        "SerialReady2",
        0xE2D4,
        0xE1E7,
        { 0xA5, 0x01, 0xC5 },
        serial_trap_ready,
        kernal_read,
        kernal_store
    },
    {
        NULL,
        0,
        0,
        { 0, 0, 0 },
        NULL,
        NULL,
        NULL
    }
};

/* Tape traps.  */
static const trap_t plus4_tape_traps[] = {
    {
        "TapeFindHeader",
        0xE9CC,
        0xE9CF,
        { 0x20, 0xD3, 0xE8 },
        tape_find_header_trap_plus4,
        rom_read,
        rom_store
    },
    {
        "TapeReceive",
        0xE74B,
        0xE8C7,
        { 0xBA, 0x8E, 0xBE },
        tape_receive_trap_plus4,
        rom_read,
        rom_store
    },
    {
        NULL,
        0,
        0,
        { 0, 0, 0 },
        NULL,
        NULL,
        NULL
    }
};

static const tape_init_t tapeinit = {
    0x0333,
    0x90,
    0x93,
    0x0000,
    0,
    0xb4,
    0x9d,
    0x527,
    0xef,
    plus4_tape_traps,
    36 * 8,
    54 * 8,
    55 * 8,
    73 * 8,
    74 * 8,
    100 * 8
};

static log_t plus4_log = LOG_ERR;
static machine_timing_t machine_timing;

/*
static long cycles_per_sec = PLUS4_PAL_CYCLES_PER_SEC;
static long cycles_per_rfsh = PLUS4_PAL_CYCLES_PER_RFSH;
static double rfsh_per_sec = PLUS4_PAL_RFSH_PER_SEC;
*/

/* Plus4-specific resource initialization.  This is called before initializing
   the machine itself with `machine_init()'.  */
int machine_resources_init(void)
{
    if (traps_resources_init() < 0
        || vsync_resources_init() < 0
        || video_resources_pal_init() < 0
        || video_resources_init() < 0
        || plus4_resources_init() < 0
        || ted_resources_init() < 0
        || sound_resources_init() < 0
        || acia_resources_init() < 0
        || rs232drv_resources_init() < 0
        || printer_resources_init() < 0
#ifndef COMMON_KBD
        || kbd_resources_init() < 0
#endif
        || drive_resources_init() < 0
        || datasette_resources_init() < 0
        )
        return -1;

    return 0;
}

void machine_resources_shutdown(void)
{
    video_resources_shutdown();
    plus4_resources_shutdown();
    sound_resources_shutdown();
    rs232drv_resources_shutdown();
    printer_resources_shutdown();
    drive_resources_shutdown();
}

/* Plus4-specific command-line option initialization.  */
int machine_cmdline_options_init(void)
{
    if (traps_cmdline_options_init() < 0
        || vsync_cmdline_options_init() < 0
        || video_init_cmdline_options() < 0
        || plus4_cmdline_options_init() < 0
        || ted_cmdline_options_init() < 0
        || sound_cmdline_options_init() < 0
        || acia_cmdline_options_init() < 0
        || rs232drv_cmdline_options_init() < 0
        || printer_cmdline_options_init() < 0
#ifndef COMMON_KBD
        || kbd_cmdline_options_init() < 0
#endif
        || drive_cmdline_options_init() < 0
        || datasette_cmdline_options_init() < 0
        )
        return -1;

    return 0;
}

static void plus4_monitor_init(void)
{
    monitor_cpu_type_t asm6502;
    monitor_cpu_type_t *asmarray[2] = { &asm6502, NULL };

    asm6502_init(&asm6502);

    /* Initialize the monitor.  */
    monitor_init(maincpu_monitor_interface_get(),
                 drive0_monitor_interface_get(),
                 drive1_monitor_interface_get(), asmarray);
}

void machine_setup_context(void)
{
}

/* Plus4-specific initialization.  */
int machine_init(void)
{
    plus4_log = log_open("Plus4");

    if (mem_load() < 0)
        return -1;

    /* Setup trap handling.  */
    traps_init();

    /* Initialize serial traps.  */
    if (serial_init(plus4_serial_traps) < 0)
        return -1;

    serial_trap_init(0xa8);

    /* Initialize drives. */
    file_system_init();

    rs232drv_init();

    /* Initialize print devices.  */
    printer_init();

    /* Initialize the tape emulation.  */
    tape_init(&tapeinit);

    /* Initialize the datasette emulation.  */
    datasette_init();

    /* Fire up the hardware-level drive emulation.  */
    drive_init();

    /* Initialize autostart.  */
    autostart_init((CLOCK)(2 * PLUS4_PAL_RFSH_PER_SEC
                   * PLUS4_PAL_CYCLES_PER_RFSH), 0, 0, 0xc8, 0xca, -40);

    if (!ted_init())
        return -1;

    acia_init();

#ifndef COMMON_KBD
    if (plus4_kbd_init() < 0)
        return -1;
#endif

    plus4_monitor_init();

    /* Initialize vsync and register our hook function.  */
    vsync_init(machine_vsync_hook);
    vsync_set_machine_parameter(machine_timing.rfsh_per_sec,
                                machine_timing.cycles_per_sec);

    /* Initialize sound.  Notice that this does not really open the audio
       device yet.  */
    sound_init(machine_timing.cycles_per_sec, machine_timing.cycles_per_rfsh);

    /* Initialize keyboard buffer.  */
    kbd_buf_init(1319, 239, 8, (CLOCK)(machine_timing.rfsh_per_sec
                 * machine_timing.cycles_per_rfsh));

    plus4ui_init();

    plus4iec_init();

    machine_drive_stub();

    return 0;
}

/* PLUS4-specific reset sequence.  */
void machine_specific_reset(void)
{
    serial_traps_reset();

    acia_reset();
    rs232drv_reset();

    printer_reset();

    plus4tcbm1_reset();
    plus4tcbm2_reset();

    ted_reset();

    drive_reset();
    datasette_reset();
}

void machine_specific_powerup(void)
{
    ted_reset_registers();
}

void machine_specific_shutdown(void)
{
    file_system_detach_disk_shutdown();

    tape_image_detach(1);

    ted_shutdown();

    plus4ui_shutdown();
}

void machine_handle_pending_alarms(int num_write_cycles)
{
     ted_handle_pending_alarms(num_write_cycles);
}

/* ------------------------------------------------------------------------- */

/* This hook is called at the end of every frame.  */
static void machine_vsync_hook(void)
{
    CLOCK sub;

    drive_vsync_hook();

    autostart_advance();

    screenshot_record();

    sub = clk_guard_prevent_overflow(maincpu_clk_guard);

    /* The drive has to deal both with our overflowing and its own one, so
       it is called even when there is no overflowing in the main CPU.  */
    /* FIXME: Do we have to check drive_enabled here?  */
    drive_prevent_clk_overflow(sub, 0);
    drive_prevent_clk_overflow(sub, 1);
}

int machine_set_restore_key(int v)
{
    return 0;
}

/* ------------------------------------------------------------------------- */

long machine_get_cycles_per_second(void)
{
    return machine_timing.cycles_per_sec;
}

void machine_get_line_cycle(unsigned int *line, unsigned int *cycle)
{
    *line = (unsigned int)((maincpu_clk) / machine_timing.cycles_per_line
            % machine_timing.screen_lines);

    *cycle = (unsigned int)((maincpu_clk) % machine_timing.cycles_per_line);
}

void machine_change_timing(int timeval)
{
    switch (timeval) {
      case MACHINE_SYNC_PAL:
        machine_timing.cycles_per_sec = PLUS4_PAL_CYCLES_PER_SEC;
        machine_timing.cycles_per_rfsh = PLUS4_PAL_CYCLES_PER_RFSH;
        machine_timing.rfsh_per_sec = PLUS4_PAL_RFSH_PER_SEC;
        machine_timing.cycles_per_line = PLUS4_PAL_CYCLES_PER_LINE;
        machine_timing.screen_lines = PLUS4_PAL_SCREEN_LINES;
        break;
      case MACHINE_SYNC_NTSC:
        machine_timing.cycles_per_sec = PLUS4_NTSC_CYCLES_PER_SEC;
        machine_timing.cycles_per_rfsh = PLUS4_NTSC_CYCLES_PER_RFSH;
        machine_timing.rfsh_per_sec = PLUS4_NTSC_RFSH_PER_SEC;
        machine_timing.cycles_per_line = PLUS4_NTSC_CYCLES_PER_LINE;
        machine_timing.screen_lines = PLUS4_NTSC_SCREEN_LINES;
        break;
      default:
        log_error(plus4_log, "Unknown machine timing.");
    }

    vsync_set_machine_parameter(machine_timing.rfsh_per_sec,
                                machine_timing.cycles_per_sec);
    sound_set_machine_parameter(machine_timing.cycles_per_sec,
                                machine_timing.cycles_per_rfsh);
    debug_set_machine_parameter(machine_timing.cycles_per_line,
                                machine_timing.screen_lines);
    drive_set_machine_parameter(machine_timing.cycles_per_sec);
    clk_guard_set_clk_base(maincpu_clk_guard, machine_timing.cycles_per_rfsh);

    ted_change_timing(&machine_timing);

    machine_trigger_reset(MACHINE_RESET_MODE_HARD);
}

/* ------------------------------------------------------------------------- */

int machine_write_snapshot(const char *name, int save_roms, int save_disks,
                           int event_mode)
{
    return plus4_snapshot_write(name, save_roms, save_disks, event_mode);
}

int machine_read_snapshot(const char *name, int event_mode)
{
    return plus4_snapshot_read(name, event_mode);
}

/* ------------------------------------------------------------------------- */

int machine_autodetect_psid(const char *name)
{
    return -1;
}

struct video_canvas_s *machine_canvas_get(unsigned int window)
{
    if (window == 0)
        return ted_get_canvas();

    return NULL;
}

int machine_screenshot(screenshot_t *screenshot, struct video_canvas_s *canvas)
{
    if (canvas != ted_get_canvas())
        return -1;

    ted_screenshot(screenshot);

    return 0;
}

int machine_canvas_async_refresh(struct canvas_refresh_s *refresh,
                                 struct video_canvas_s *canvas)
{
    if (canvas != ted_get_canvas())
        return -1;

    ted_async_refresh(refresh);

    return 0;
}

unsigned int machine_num_keyboard_mappings(void)
{
    return NUM_KEYBOARD_MAPPINGS;
}

void machine_traps_enable(int enable)
{
}

struct image_contents_s *machine_diskcontents_bus_read(unsigned int unit)
{
    return diskcontents_iec_read(unit);
}

int machine_bus_lib_directory(unsigned int unit, const char *pattern,
                              BYTE **buf)
{
    return serial_iec_lib_directory(unit, pattern, buf);
}

int machine_bus_lib_read_sector(unsigned int unit, unsigned int track,
                                unsigned int sector, BYTE *buf)
{
    return serial_iec_lib_read_sector(unit, track, sector, buf);
}

int machine_bus_lib_write_sector(unsigned int unit, unsigned int track,
                                 unsigned int sector, BYTE *buf)
{
    return serial_iec_lib_write_sector(unit, track, sector, buf);
}

/* ------------------------------------------------------------------------- */
/* Temporary kluge: */

void printer_interface_userport_set_busy(int b)
{
}

