/*
 * vic20.c
 *
 * Written by
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

#include <stdio.h>

#include "asm.h"
#include "attach.h"
#include "autostart.h"
#include "cartridge.h"
#include "ciatimer.h"
#include "clkguard.h"
#include "cmdline.h"
#include "datasette.h"
#include "debug.h"
#include "drive-cmdline-options.h"
#include "drive-resources.h"
#include "drive-snapshot.h"
#include "drive.h"
#include "drivecpu.h"
#include "event.h"
#include "iecdrive.h"
#include "interrupt.h"
#include "ioutil.h"
#include "kbdbuf.h"
#include "keyboard.h"
#include "log.h"
#include "machine-drive.h"
#include "machine.h"
#include "maincpu.h"
#include "mon.h"
#include "printer.h"
#include "resources.h"
#include "rs232drv.h"
#include "rsuser.h"
#include "screenshot.h"
#include "serial.h"
#include "snapshot.h"
#include "sound.h"
#include "tape.h"
#include "traps.h"
#include "types.h"
#include "via.h"
#include "vic.h"
#include "vic20-cmdline-options.h"
#include "vic20-resources.h"
#include "vic20-snapshot.h"
#include "vic20.h"
#include "vic20iec.h"
#include "vic20ieeevia.h"
#include "vic20mem.h"
#include "vic20sound.h"
#include "vic20rsuser.h"
#include "vic20ui.h"
#include "vic20via.h"
#include "video.h"
#include "vsync.h"


#define NUM_KEYBOARD_MAPPINGS 2

const char *machine_keymap_res_name_list[NUM_KEYBOARD_MAPPINGS] = {
    "KeymapSymFile", "KeymapPosFile"
};

char *machine_keymap_file_list[NUM_KEYBOARD_MAPPINGS] = {
    NULL, NULL
};

const char machine_name[] = "VIC20";
int machine_class = VICE_MACHINE_VIC20;

static void machine_vsync_hook(void);

/* ------------------------------------------------------------------------- */

static trap_t vic20_serial_traps[] = {
    {
        "SerialListen",
        0xEE2E,
        0xEEB2,
        { 0x20, 0xA0, 0xE4 },
        serial_trap_attention,
        rom_read,
        rom_store
    },
    {
        "SerialSaListen",
        0xEE40,
        0xEEB2,
        { 0x20, 0x8D, 0xEF },
        serial_trap_attention,
        rom_read,
        rom_store
    },
    {
        "SerialSendByte",
        0xEE49,
        0xEEB2,
        { 0x78, 0x20, 0xA0 },
        serial_trap_send,
        rom_read,
        rom_store
    },
    {
        "SerialReceiveByte",
        0xEF19,
        0xEEB2,
        { 0x78, 0xA9, 0x00 },
        serial_trap_receive,
        rom_read,
        rom_store
    },
    {
        "SerialReady",
        0xE4B2,
        0xEEB2,
        { 0xAD, 0x1F, 0x91 },
        serial_trap_ready,
        rom_read,
        rom_store
    },
    {
        NULL,
        0,
        0,
        {0, 0, 0},
        NULL,
        NULL,
        NULL
    }
};

/* Tape traps.  */
static trap_t vic20_tape_traps[] = {
    {
        "TapeFindHeader",
        0xF7B2,
        0xF7B5,
        { 0x20, 0xC0, 0xF8 },
        tape_find_header_trap,
        rom_read,
        rom_store
    },
    {
        "TapeReceive",
        0xF90B,
        0xFCCF,
        { 0x20, 0xFB, 0xFC },
        tape_receive_trap,
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

static tape_init_t tapeinit = {
    0xb2,
    0x90,
    0x93,
    0x29f,
    0,
    0xc1,
    0xae,
    0x277,
    0xc6,
    vic20_tape_traps,
    36 * 8,
    54 * 8,
    55 * 8,
    73 * 8,
    74 * 8,
    100 * 8
};

static log_t vic20_log = LOG_ERR;
static machine_timing_t machine_timing;

/* ------------------------------------------------------------------------ */

/* VIC20-specific resource initialization.  This is called before
   initializing the machine itself with `machine_init()'.  */
int machine_resources_init(void)
{
    if (traps_resources_init() < 0
        || vsync_resources_init() < 0
        || video_resources_init(VIDEO_RESOURCES_PAL) < 0
        || vic20_resources_init() < 0
        || vic_resources_init() < 0
        || sound_resources_init() < 0
        || rs232drv_resources_init() < 0
        || rsuser_resources_init() < 0
        || printer_resources_init() < 0
#ifndef COMMON_KBD
        || kbd_resources_init() < 0
#endif
        || drive_resources_init() < 0
        || datasette_resources_init() < 0
        || cartridge_resources_init() <0)
        return -1;

    return 0;
}

/* VIC20-specific command-line option initialization.  */
int machine_cmdline_options_init(void)
{
    if (traps_cmdline_options_init() < 0
        || vsync_cmdline_options_init() < 0
        || video_init_cmdline_options() < 0
        || vic20_cmdline_options_init() < 0
        || vic_cmdline_options_init() < 0
        || sound_cmdline_options_init() < 0
        || rs232drv_cmdline_options_init() < 0
        || rsuser_cmdline_options_init() < 0
        || printer_cmdline_options_init() < 0
#ifndef COMMON_KBD
        || kbd_cmdline_options_init() < 0
#endif
        || drive_cmdline_options_init() < 0
        || datasette_cmdline_options_init() < 0
        || cartridge_cmdline_options_init() < 0)
        return -1;

    return 0;
}

void vic20_monitor_init(void)
{
    monitor_cpu_type_t asm6502;
    monitor_cpu_type_t *asmarray[2];

    asm6502_init(&asm6502);

    asmarray[0] = &asm6502;
    asmarray[1] = NULL;

    /* Initialize the monitor.  */
    monitor_init(&maincpu_monitor_interface, drive0_get_monitor_interface_ptr(),
                 drive1_get_monitor_interface_ptr(), asmarray);
}

/* VIC20-specific initialization.  */
int machine_init(void)
{
    vic20_log = log_open("VIC20");

    if (mem_load() < 0)
        return -1;

    /* Setup trap handling.  */
    traps_init();

    /* Initialize serial traps.  If user does not want them, or if the
       ``drive'' emulation is used, do not install them.  */
    if (serial_init(vic20_serial_traps, 0xa4) < 0)
        return -1;

    /* Initialize drives. */
    file_system_init();

    /* Initialize RS232 handler.  */
    rs232drv_init();
    vic20_rsuser_init();

    /* initialize print devices.  */
    printer_init();

    /* Initialize the tape emulation.  */
    tape_init(&tapeinit);

    /* Initialize the datasette emulation.  */
    datasette_init();

    /* Fire up the hardware-level drive emulation. */
    drive_init();

    /* Initialize autostart.  */
    autostart_init((CLOCK)
                   (3 * VIC20_PAL_RFSH_PER_SEC * VIC20_PAL_CYCLES_PER_RFSH),
                   1, 0xcc, 0xd1, 0xd3, 0xd5);

    /* Initialize the VIC-I emulation.  */
    if (vic_init() == NULL)
        return -1;

    /* needed for VC1571/1581 emulation */
    ciat_init_table();

    via1_init();
    via2_init();

    ieeevia1_init();
    ieeevia2_init();

#ifndef COMMON_KBD
    /* Load the default keymap file.  */
    if (vic20_kbd_init() < 0)
        return -1;
#endif

    vic20_monitor_init();

    /* Initialize vsync and register our hook function.  */
    vsync_init(machine_vsync_hook);
    vsync_set_machine_parameter(machine_timing.rfsh_per_sec,
                                machine_timing.cycles_per_sec);

    /* Initialize sound.  Notice that this does not really open the audio
       device yet.  */
    sound_init(machine_timing.cycles_per_sec, machine_timing.cycles_per_rfsh);

    /* Initialize keyboard buffer.  */
    kbd_buf_init(631, 198, 10, (CLOCK)(machine_timing.cycles_per_rfsh
                 * machine_timing.rfsh_per_sec));

    /* Initialize the VIC20-specific part of the UI.  */
    vic20_ui_init();

    vic20iec_init();

    machine_drive_stub();

    return 0;
}

/* VIC20-specific reset sequence.  */
void machine_specific_reset(void)
{
    serial_reset();

    via1_reset();
    via2_reset();
    vic_reset();
    vic_sound_reset();

    ieeevia1_reset();
    ieeevia2_reset();

    rs232drv_reset();
    rsuser_reset();

    printer_reset();
    drive_reset();
    datasette_reset();
}

void machine_powerup(void)
{
    mem_powerup();
    maincpu_trigger_reset();
}

void machine_specific_shutdown(void)
{
    /* Detach all disks.  */
    file_system_detach_disk_shutdown();

    /* and the tape */
    tape_image_detach(1);

    /* close the video chip(s) */
    vic_shutdown();
}

/* Return nonzero if `addr' is in the trappable address space.  */
int rom_trap_allowed(WORD addr)
{
    return 1; /* FIXME */
}

/* ------------------------------------------------------------------------- */

/* This hook is called at the end of every frame.  */
static void machine_vsync_hook(void)
{
    CLOCK sub;

    drive_vsync_hook();

    autostart_advance();

    sub = clk_guard_prevent_overflow(&maincpu_clk_guard);

    /* The drive has to deal both with our overflowing and its own one, so
       it is called even when there is no overflowing in the main CPU.  */
    /* FIXME: Do we have to check drive_enabled here?  */
    drive_prevent_clk_overflow(sub, 0);
    drive_prevent_clk_overflow(sub, 1);
}

int machine_set_restore_key(int v)
{
    via2_signal(VIA_SIG_CA1, v ? VIA_SIG_FALL : VIA_SIG_RISE);
    return 1;
}

/* ------------------------------------------------------------------------- */

long machine_get_cycles_per_second(void)
{
    return machine_timing.cycles_per_sec;
}

void machine_change_timing(int timeval)
{
    switch (timeval) {
      case MACHINE_SYNC_PAL:
        machine_timing.cycles_per_sec = VIC20_PAL_CYCLES_PER_SEC;
        machine_timing.cycles_per_rfsh = VIC20_PAL_CYCLES_PER_RFSH;
        machine_timing.rfsh_per_sec = VIC20_PAL_RFSH_PER_SEC;
        machine_timing.cycles_per_line = VIC20_PAL_CYCLES_PER_LINE;
        machine_timing.screen_lines = VIC20_PAL_SCREEN_LINES;
        break;
      case MACHINE_SYNC_NTSC:
        machine_timing.cycles_per_sec = VIC20_NTSC_CYCLES_PER_SEC;
        machine_timing.cycles_per_rfsh = VIC20_NTSC_CYCLES_PER_RFSH;
        machine_timing.rfsh_per_sec = VIC20_NTSC_RFSH_PER_SEC;
        machine_timing.cycles_per_line = VIC20_NTSC_CYCLES_PER_LINE;
        machine_timing.screen_lines = VIC20_NTSC_SCREEN_LINES;
        break;
      default:
        log_error(vic20_log, "Unknown machine timing.");
    }

    vsync_set_machine_parameter(machine_timing.rfsh_per_sec,
                                machine_timing.cycles_per_sec);
    sound_set_machine_parameter(machine_timing.cycles_per_sec,
                                machine_timing.cycles_per_rfsh);
    debug_set_machine_parameter(machine_timing.cycles_per_line,
                                machine_timing.screen_lines);
    drive_set_machine_parameter(machine_timing.cycles_per_sec);
    clk_guard_set_clk_base(&maincpu_clk_guard, machine_timing.cycles_per_rfsh);

    vic_change_timing();

    mem_patch_kernal();

    machine_powerup();
}

/* ------------------------------------------------------------------------- */

/* #define SNAP_MACHINE_NAME   "VIC20" */
#define SNAP_MAJOR          0
#define SNAP_MINOR          0

int machine_write_snapshot(const char *name, int save_roms, int save_disks,
                           int event_mode)
{
    snapshot_t *s;
    int ieee488;

    s = snapshot_create(name, ((BYTE)(SNAP_MAJOR)), ((BYTE)(SNAP_MINOR)),
                        machine_name);
    if (s == NULL) {
        perror(name);
        return -1;
    }

    /* FIXME: Missing sound.  */
    if (maincpu_snapshot_write_module(s) < 0
        || vic20_snapshot_write_module(s, save_roms) < 0
        || vic_snapshot_write_module(s) < 0
        || via1_snapshot_write_module(s) < 0
        || via2_snapshot_write_module(s) < 0
        || drive_snapshot_write_module(s, save_disks, save_roms) < 0
        || event_snapshot_write_module(s, event_mode) < 0) {
        snapshot_close(s);
        ioutil_remove(name);
        return -1;
    }

    resources_get_value("IEEE488", (resource_value_t*) &ieee488);
    if (ieee488) {
        if (ieeevia1_snapshot_write_module(s) < 0
            || ieeevia2_snapshot_write_module(s) < 0) {
            snapshot_close(s);
            ioutil_remove(name);
            return 1;
        }
    }

    snapshot_close(s);
    return 0;
}

int machine_read_snapshot(const char *name, int event_mode)
{
    snapshot_t *s;
    BYTE minor, major;

    s = snapshot_open(name, &major, &minor, machine_name);
    if (s == NULL)
        return -1;

    if (major != SNAP_MAJOR || minor != SNAP_MINOR) {
        log_error(vic20_log,
                  "Snapshot version (%d.%d) not valid: expecting %d.%d.",
                  major, minor, SNAP_MAJOR, SNAP_MINOR);
        goto fail;
    }

    /* FIXME: Missing sound.  */
    if (maincpu_snapshot_read_module(s) < 0
        || vic20_snapshot_read_module(s) < 0
        || vic_snapshot_read_module(s) < 0
        || via1_snapshot_read_module(s) < 0
        || via2_snapshot_read_module(s) < 0
        || drive_snapshot_read_module(s) < 0
        || event_snapshot_read_module(s, event_mode) < 0)
        goto fail;

    if (ieeevia1_snapshot_read_module(s) < 0
        || ieeevia2_snapshot_read_module(s) < 0) {
        /* IEEE488 module not undumped */
        resources_set_value("IEEE488", (resource_value_t)0);
    } else {
        resources_set_value("IEEE488", (resource_value_t)1);
    }

    snapshot_close(s);
    return 0;

fail:
    if (s != NULL)
        snapshot_close(s);
    maincpu_trigger_reset();
    return -1;
}


/* ------------------------------------------------------------------------- */
int machine_autodetect_psid(const char *name)
{
    return -1;
}

void machine_play_psid(int tune)
{
}

struct video_canvas_s *machine_canvas_get(unsigned int window)
{
    if (window == 0)
        return vic_get_canvas();

    return NULL;
}

int machine_screenshot(screenshot_t *screenshot, struct video_canvas_s *canvas)
{
    if (canvas != vic_get_canvas())
        return -1;

    vic_screenshot(screenshot);
    return 0;
}

int machine_canvas_async_refresh(struct canvas_refresh_s *refresh,
                                 struct video_canvas_s *canvas)
{
    if (canvas != vic_get_canvas())
        return -1;

    vic_async_refresh(refresh);
    return 0;
}

unsigned int machine_num_keyboard_mappings(void)
{
    return NUM_KEYBOARD_MAPPINGS;
}

