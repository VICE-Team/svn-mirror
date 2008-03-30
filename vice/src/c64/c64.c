/*
 * c64.c
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Teemu Rantanen <tvr@cs.hut.fi>
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

#include "asm.h"
#include "attach.h"
#include "autostart.h"
#include "c64-cmdline-options.h"
#include "c64-resources.h"
#include "c64-snapshot.h"
#include "c64.h"
#include "c64cart.h"
#include "c64cia.h"
#include "c64mem.h"
#include "c64tpi.h"
#include "c64ui.h"
#include "cartridge.h"
#include "ciatimer.h"
#include "clkguard.h"
#include "console.h"
#include "datasette.h"
#include "drive-cmdline-options.h"
#include "drive-resources.h"
#include "drive-snapshot.h"
#include "drive.h"
#include "drivecpu.h"
#include "iecdrive.h"
#include "interrupt.h"
#include "kbdbuf.h"
#include "keyboard.h"
#include "log.h"
#include "machine.h"
#include "maincpu.h"
#include "mem.h"
#include "mon.h"
#include "patchrom.h"
#include "printer.h"
#include "psid.h"
#include "resources.h"
#include "reu.h"
#include "screenshot.h"
#include "serial.h"
#include "sid-cmdline-options.h"
#include "sid-resources.h"
#include "sid-snapshot.h"
#include "sid.h"
#include "snapshot.h"
#include "sound.h"
#include "tape.h"
#include "traps.h"
#include "types.h"
#include "utils.h"
#include "vicii.h"
#include "video.h"
#include "vsidui.h"
#include "vsync.h"

#ifdef HAVE_RS232
#include "c64acia.h"
#include "c64rsuser.h"
#include "rs232.h"
#include "rsuser.h"
#endif

#ifdef HAVE_MOUSE
#include "mouse.h"
#endif


#define NUM_KEYBOARD_MAPPINGS 2

const char *machine_keymap_res_name_list[NUM_KEYBOARD_MAPPINGS] = {
    "KeymapSymFile", "KeymapPosFile"
};

char *machine_keymap_file_list[NUM_KEYBOARD_MAPPINGS] = {
    NULL, NULL
};

const char machine_name[] = "C64";
int machine_class = VICE_MACHINE_C64;

static void machine_vsync_hook(void);

/* ------------------------------------------------------------------------- */

static trap_t c64_serial_traps[] = {
    {
        "SerialListen",
        0xED24,
        0xEDAB,
        { 0x20, 0x97, 0xEE },
        serialattention,
        rom_read,
        rom_store
    },
    {
        "SerialSaListen",
        0xED36,
        0xEDAB,
        { 0x78, 0x20, 0x8E },
        serialattention,
        rom_read,
        rom_store
    },
    {
        "SerialSendByte",
        0xED40,
        0xEDAB,
        { 0x78, 0x20, 0x97 },
        serialsendbyte,
        rom_read,
        rom_store
    },
    {
        "SerialReceiveByte",
        0xEE13,
        0xEDAB,
        { 0x78, 0xA9, 0x00 },
        serialreceivebyte,
        rom_read,
        rom_store
    },
    {
        "SerialReady",
        0xEEA9,
        0xEDAB,
        { 0xAD, 0x00, 0xDD },
        trap_serial_ready,
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

/* Tape traps.  */
static trap_t c64_tape_traps[] = {
    {
        "TapeFindHeader",
        0xF72F,
        0xF732,
        { 0x20, 0x41, 0xF8 },
        tape_find_header_trap,
        rom_read,
        rom_store
    },
    {
        "TapeReceive",
        0xF8A1,
        0xFC93,
        { 0x20, 0xBD, 0xFC },
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
    c64_tape_traps,
    36 * 8,
    54 * 8,
    55 * 8,
    73 * 8,
    74 * 8,
    100 * 8
};

static log_t c64_log = LOG_ERR;

static long cycles_per_sec = C64_PAL_CYCLES_PER_SEC;
static long cycles_per_rfsh = C64_PAL_CYCLES_PER_RFSH;
static double rfsh_per_sec = C64_PAL_RFSH_PER_SEC;

/* ------------------------------------------------------------------------ */

/* C64-specific resource initialization.  This is called before initializing
   the machine itself with `machine_init()'.  */
int machine_resources_init(void)
{
    if (traps_resources_init() < 0
        || vsync_resources_init() < 0
        || video_resources_init(VIDEO_RESOURCES_PAL) < 0
        || c64_resources_init() < 0
        || reu_resources_init() < 0
        || vic_ii_resources_init() < 0
        || sound_resources_init() < 0
        || sid_resources_init() < 0
#ifdef HAVE_RS232
        || acia1_resources_init() < 0
        || rs232_resources_init() < 0
        || rsuser_resources_init() < 0
#endif
        || printer_resources_init() < 0
#ifdef HAVE_MOUSE
        || mouse_resources_init() < 0
#endif
#ifndef COMMON_KBD
        || kbd_resources_init() < 0
#endif
        || drive_resources_init() < 0
        || datasette_resources_init() < 0
        || cartridge_resources_init() < 0
        )
        return -1;

    if (vsid_mode && psid_init_resources() < 0)
        return -1;

    return 0;
}

/* C64-specific command-line option initialization.  */
int machine_cmdline_options_init(void)
{
    if (vsid_mode) {
        if (sound_cmdline_options_init() < 0
            || sid_cmdline_options_init() < 0
            || psid_init_cmdline_options() < 0
            )
            return -1;

        return 0;
    }

    if (traps_cmdline_options_init() < 0
        || vsync_cmdline_options_init() < 0
        || video_init_cmdline_options() < 0
        || c64_cmdline_options_init() < 0
        || reu_cmdline_options_init() < 0
        || vic_ii_cmdline_options_init() < 0
        || sound_cmdline_options_init() < 0
        || sid_cmdline_options_init() < 0
#ifdef HAVE_RS232
        || acia1_cmdline_options_init() < 0
        || rs232_cmdline_options_init() < 0
        || rsuser_cmdline_options_init() < 0
#endif
        || printer_cmdline_options_init() < 0
#ifdef HAVE_MOUSE
        || mouse_cmdline_options_init() < 0
#endif
#ifndef COMMON_KBD
        || kbd_cmdline_options_init() < 0
#endif
        || drive_cmdline_options_init() < 0
        || datasette_cmdline_options_init() < 0
        || cartridge_cmdline_options_init() < 0
        )
        return -1;

    return 0;
}

static void c64_monitor_init(void)
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

/* C64-specific initialization.  */
int machine_init(void)
{
    c64_log = log_open("C64");

    if (mem_load() < 0)
        return -1;

    if (vsid_mode) {
        psid_init_driver();
    }

    if (!vsid_mode) {
        /* Setup trap handling.  */
        traps_init();

        /* Initialize serial traps.  */
        if (serial_init(c64_serial_traps, 0xa4) < 0)
            return -1;

        /* Initialize drives. */
        file_system_init();

#ifdef HAVE_RS232
        /* Initialize RS232 handler.  */
        rs232_init();
        c64_rsuser_init();
#endif

        /* Initialize print devices.  */
        printer_init();

        /* Initialize the tape emulation.  */
        tape_init(&tapeinit);

        /* Initialize the datasette emulation.  */
        datasette_init();

        /* Fire up the hardware-level drive emulation.  */
        drive_init(C64_PAL_CYCLES_PER_SEC, C64_NTSC_CYCLES_PER_SEC);

        /* Initialize autostart.  */
        autostart_init((CLOCK)(3 * rfsh_per_sec * cycles_per_rfsh),
                       1, 0xcc, 0xd1, 0xd3, 0xd5);
    }

    /* Initialize the VIC-II emulation. */
    if (!vic_ii_init() && !console_mode && !vsid_mode)
        return -1;

    vic_ii_enable_extended_vicii(0);

    cia1_enable_extended_keyboard_rows(0);

    ciat_init_table();
    cia1_init();
    cia2_init();

    if (!vsid_mode) {
        tpi_init();

#ifdef HAVE_RS232
        acia1_init();
#endif

#ifndef COMMON_KBD
        /* Initialize the keyboard.  */
        if (c64_kbd_init() < 0)
            return -1;
#endif
    }

    c64_monitor_init();

    /* Initialize vsync and register our hook function.  */
    vsync_init(machine_vsync_hook);
    vsync_set_machine_parameter(rfsh_per_sec, cycles_per_sec);

    /* Initialize sound.  Notice that this does not really open the audio
       device yet.  */
    sound_init(cycles_per_sec, cycles_per_rfsh);

    /* Initialize keyboard buffer.  */
    kbd_buf_init(631, 198, 10,
                 (CLOCK)(rfsh_per_sec * cycles_per_rfsh));

    /* Initialize the C64-specific part of the UI.  */
    if (!console_mode) {

        if (vsid_mode)
            vsid_ui_init();
        else
            c64_ui_init();
    }

    if (!vsid_mode)
    {
        /* Initialize the REU.  */
        reu_init();

#ifdef HAVE_MOUSE
        /* Initialize mouse support (if present).  */
        mouse_init();
#endif

        iec_init();

        cartridge_init();
    }

    return 0;
}

/* C64-specific reset sequence.  */
void machine_specific_reset(void)
{
    serial_reset();

    cia1_reset();
    cia2_reset();
    sid_reset();

    if (!vsid_mode) {
        tpi_reset();

#ifdef HAVE_RS232
        acia1_reset();
        rs232_reset();
        rsuser_reset();
#endif

        printer_reset();

        /* FIXME */
        /* reset_reu(); */
    }

    /* The VIC-II must be the *last* to be reset.  */
    vic_ii_reset();

    if (vsid_mode) {
        psid_init_tune();
        return;
    }

    drive_reset();
    datasette_reset();
    reu_reset();
}

void machine_powerup(void)
{
    /* Hard reset unloads PSID. */
    if (vsid_mode) {
        machine_play_psid(-1);
    }

    mem_powerup();
    vic_ii_reset_registers();
    maincpu_trigger_reset();
}

void machine_shutdown(void)
{
    /* Detach all disks.  */
    if (!vsid_mode) {
        file_system_detach_disk_shutdown();
    }

    /* and the tape */
    tape_image_detach(1);

    /* and cartridge */
    cartridge_detach_image();

    console_close_all();

    /* close the video chip(s) */
    vic_ii_free();

    reu_shutdown();

    if (vsid_mode) {
        vsid_ui_close();
    }
}

void machine_handle_pending_alarms(int num_write_cycles)
{
    vic_ii_handle_pending_alarms_external(num_write_cycles);
}

/* ------------------------------------------------------------------------- */

/* This hook is called at the end of every frame.  */
static void machine_vsync_hook(void)
{
    CLOCK sub;

    if (vsid_mode) {
        unsigned int playtime;
        static unsigned int time=0;

        playtime = (psid_increment_frames() * cycles_per_rfsh) / cycles_per_sec;        if (playtime!=time)
        {
            vsid_ui_display_time(playtime);
            time=playtime;
        }
        clk_guard_prevent_overflow(&maincpu_clk_guard);
        return;
    }

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
    maincpu_set_nmi(I_RESTORE, v ? 1 : 0);
    return 1;
}

/* ------------------------------------------------------------------------- */

long machine_get_cycles_per_second(void)
{
    return cycles_per_sec;
}

void machine_change_timing(int timeval)
{
    maincpu_trigger_reset();

    switch (timeval) {
      case MACHINE_SYNC_PAL:
        cycles_per_sec = C64_PAL_CYCLES_PER_SEC;
        cycles_per_rfsh = C64_PAL_CYCLES_PER_RFSH;
        rfsh_per_sec = C64_PAL_RFSH_PER_SEC;
        break;
      case MACHINE_SYNC_NTSC:
        cycles_per_sec = C64_NTSC_CYCLES_PER_SEC;
        cycles_per_rfsh = C64_NTSC_CYCLES_PER_RFSH;
        rfsh_per_sec = C64_NTSC_RFSH_PER_SEC;
        break;
      case MACHINE_SYNC_NTSCOLD:
        cycles_per_sec = C64_NTSCOLD_CYCLES_PER_SEC;
        cycles_per_rfsh = C64_NTSCOLD_CYCLES_PER_RFSH;
        rfsh_per_sec = C64_NTSCOLD_RFSH_PER_SEC;
        break;
      default:
        log_error(c64_log, "Unknown machine timing.");
    }

    vsync_set_machine_parameter(rfsh_per_sec, cycles_per_sec);
    sound_set_machine_parameter(cycles_per_sec, cycles_per_rfsh);
}

/* ------------------------------------------------------------------------- */

#define SNAP_MAJOR 1
#define SNAP_MINOR 0

int machine_write_snapshot(const char *name, int save_roms, int save_disks)
{
    snapshot_t *s;

    s = snapshot_create(name, ((BYTE)(SNAP_MAJOR)), ((BYTE)(SNAP_MINOR)),
                        machine_name);
    if (s == NULL)
        return -1;

    /* Execute drive CPUs to get in sync with the main CPU.  */
    if (drive[0].enable)
        drive0_cpu_execute(maincpu_clk);
    if (drive[1].enable)
        drive1_cpu_execute(maincpu_clk);

    if (maincpu_snapshot_write_module(s) < 0
        || c64_snapshot_write_module(s, save_roms) < 0
        || cia1_snapshot_write_module(s) < 0
        || cia2_snapshot_write_module(s) < 0
        || sid_snapshot_write_module(s) < 0
        || drive_snapshot_write_module(s, save_disks, save_roms) < 0
        || vic_ii_snapshot_write_module(s) < 0) {
        snapshot_close(s);
        util_file_remove(name);
        return -1;
    }

    snapshot_close(s);
    return 0;
}

int machine_read_snapshot(const char *name)
{
    snapshot_t *s;
    BYTE minor, major;

    s = snapshot_open(name, &major, &minor, machine_name);
    if (s == NULL)
        return -1;

    if (major != SNAP_MAJOR || minor != SNAP_MINOR) {
        log_error(c64_log,
                  "Snapshot version (%d.%d) not valid: expecting %d.%d.",
                  major, minor, SNAP_MAJOR, SNAP_MINOR);
        goto fail;
    }

    vic_ii_prepare_for_snapshot();

    if (maincpu_snapshot_read_module(s) < 0
        || c64_snapshot_read_module(s) < 0
        || cia1_snapshot_read_module(s) < 0
        || cia2_snapshot_read_module(s) < 0
        || sid_snapshot_read_module(s) < 0
        || drive_snapshot_read_module(s) < 0
        || vic_ii_snapshot_read_module(s) < 0)
        goto fail;

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
    if (name == NULL) {
        return -1;
    }

    return psid_load_file(name);
}

void machine_play_psid(int tune)
{
    psid_set_tune(tune);
}

int machine_screenshot(screenshot_t *screenshot, unsigned int wn)
{
    if (wn != 0)
        return -1;

    vic_ii_screenshot(screenshot);
    return 0;
}

int machine_canvas_screenshot(screenshot_t *screenshot,
                              struct video_canvas_s *canvas)
{
    if (canvas != vic_ii_get_canvas())
        return -1;

    vic_ii_screenshot(screenshot);
    return 0;
}

int machine_canvas_async_refresh(struct canvas_refresh_s *refresh,
                                 struct video_canvas_s *canvas)
{
    if (canvas != vic_ii_get_canvas())
        return -1;

    vic_ii_async_refresh(refresh);
    return 0;
}

void machine_update_memory_ptrs(void)
{
    vic_ii_update_memory_ptrs_external();
}

int machine_sid2_check_range(unsigned int sid2_adr)
{
    if (sid2_adr >= 0xd420 && sid2_adr <= 0xd7e0)
        return 0;

    if (sid2_adr >= 0xde00 && sid2_adr <= 0xdfe0)
        return 0;

    return -1;
}

unsigned int machine_num_keyboard_mappings(void)
{
    return NUM_KEYBOARD_MAPPINGS;
}

