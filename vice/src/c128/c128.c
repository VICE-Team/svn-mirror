/*
 * c128.c
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
 *  Ettore Perazzoli <ettore@comm2000.it>
 *
 * Based on the original work in VICE 0.11.0 by
 *  Jouko Valta <jopi@stekt.oulu.fi>
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
#include "c128-cmdline-options.h"
#include "c128-resources.h"
#include "c128-snapshot.h"
#include "c128.h"
#include "c128iec.h"
#include "c128mem.h"
#include "c128mmu.h"
#include "c128ui.h"
#include "c64acia.h"
#include "c64cia.h"
#include "c64keyboard.h"
#include "c64rsuser.h"
#include "c64tpi.h"
#include "cartridge.h"
#include "ciatimer.h"
#include "clkguard.h"
#include "datasette.h"
#include "debug.h"
#include "drive-cmdline-options.h"
#include "drive-resources.h"
#include "drive-snapshot.h"
#include "drive.h"
#include "drivecpu.h"
#include "event.h"
#include "functionrom.h"
#include "iecdrive.h"
#include "interrupt.h"
#include "ioutil.h"
#include "kbdbuf.h"
#include "keyboard.h"
#include "log.h"
#include "machine-drive.h"
#include "machine.h"
#include "maincpu.h"
#include "monitor.h"
#include "patchrom.h"
#include "printer.h"
#include "reu.h"
#include "rs232drv.h"
#include "rsuser.h"
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
#include "vicii.h"
#include "video.h"
#include "vdc.h"
#include "vsync.h"
#include "z80.h"
#include "z80mem.h"

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

const char machine_name[] = "C128";
int machine_class = VICE_MACHINE_C128;

static void machine_vsync_hook(void);

/* ------------------------------------------------------------------------- */

static trap_t c128_serial_traps[] = {
    {
        "SerialListen",
        0xE355,
        0xE5BA,
        { 0x20, 0x73, 0xE5 },
        serial_trap_attention,
        rom_read,
        rom_store
    },
    {
        "SerialSaListen",
        0xE37C,
        0xE5BA,
        { 0x20, 0x73, 0xE5 },
        serial_trap_attention,
        rom_read,
        rom_store
    },
    {
        "SerialSendByte",
        0xE38C,
        0xE5BA,
        { 0x20, 0x73, 0xE5 },
        serial_trap_send,
        rom_read,
        rom_store
    },
    {
        "SerialReceiveByte",
        0xE43E,
        0xE5BA,
        { 0x20, 0x73, 0xE5 },
        serial_trap_receive,
        rom_read,
        rom_store
    },
    {
        "Serial ready",
        0xE569,
        0xE572,
        { 0xAD, 0x00, 0xDD },
        serial_trap_ready,
        rom_read,
        rom_store
    },
    {
        "Serial ready",
        0xE4F5,
        0xE572,
        { 0xAD, 0x00, 0xDD },
        serial_trap_ready,
        rom_read,
        rom_store
    },
    {
        "SerialListen",
        0xED24,
        0xEDAB,
        { 0x20, 0x97, 0xEE },
        serial_trap_attention,
        rom64_read,
        rom64_store
    },
    {
        "SerialSaListen",
        0xED36,
        0xEDAB,
        { 0x78, 0x20, 0x8E },
        serial_trap_attention,
        rom64_read,
        rom64_store
    },
    {
        "SerialSendByte",
        0xED40,
        0xEDAB,
        { 0x78, 0x20, 0x97 },
        serial_trap_send,
        rom64_read,
        rom64_store
    },
    {
        "SerialReceiveByte",
        0xEE13,
        0xEDAB,
        { 0x78, 0xA9, 0x00 },
        serial_trap_receive,
        rom64_read,
        rom64_store
    },
    {
        "SerialReady",
        0xEEA9,
        0xEDAB,
        { 0xAD, 0x00, 0xDD },
        serial_trap_ready,
        rom64_read,
        rom64_store
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
static trap_t c128_tape_traps[] = {
    {
        "TapeFindHeader",
        0xE8D3,
        0xE8D6,
        { 0x20, 0xF2, 0xE9 },
        tape_find_header_trap,
        rom_read,
        rom_store
    },
    {
        "TapeReceive",
        0xEA60,
        0xEE57,
        { 0x20, 0x9B, 0xEE },
        tape_receive_trap,
        rom_read,
        rom_store
    },
    {
        "TapeFindHeader",
        0xF72F,
        0xF732,
        { 0x20, 0x41, 0xF8 },
        tape_find_header_trap,
        rom64_read,
        rom64_store
    },
    {
        "TapeReceive",
        0xF8A1,
        0xFC93,
        { 0x20, 0xBD, 0xFC },
        tape_receive_trap,
        rom64_read,
        rom64_store
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
    0xa09,
    0,
    0xc1,
    0xae,
    0x34a,
    0xd0,
    c128_tape_traps,
    36 * 8,
    54 * 8,
    55 * 8,
    73 * 8,
    74 * 8,
    100 * 8
};

static log_t c128_log = LOG_ERR;
static machine_timing_t machine_timing;

/* ------------------------------------------------------------------------ */

/* C128-specific resource initialization.  This is called before initializing
   the machine itself with `machine_init()'.  */
int machine_resources_init(void)
{
    if (traps_resources_init() < 0
        || vsync_resources_init() < 0
        || video_resources_pal_init() < 0
        || video_resources_init() < 0
        || c128_resources_init() < 0
        || reu_resources_init() < 0
        || vicii_resources_init() < 0
        || vdc_init_resources() < 0
        || sound_resources_init() < 0
        || sid_resources_init() < 0
        || acia1_resources_init() < 0
        || rs232drv_resources_init() < 0
        || rsuser_resources_init() < 0
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
        || mmu_resources_init() < 0
        || z80mem_resources_init() < 0
        || functionrom_resources_init() < 0)
        return -1;

    return 0;
}

/* C128-specific command-line option initialization.  */
int machine_cmdline_options_init(void)
{
    if (traps_cmdline_options_init() < 0
        || vsync_cmdline_options_init() < 0
        || video_init_cmdline_options() < 0
        || c128_cmdline_options_init() < 0
        || reu_cmdline_options_init() < 0
        || vicii_cmdline_options_init() < 0
        || vdc_init_cmdline_options() < 0
        || sound_cmdline_options_init() < 0
        || sid_cmdline_options_init() < 0
        || acia1_cmdline_options_init() < 0
        || rs232drv_cmdline_options_init() < 0
        || rsuser_cmdline_options_init() < 0
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
        || mmu_cmdline_options_init() < 0
        || functionrom_cmdline_options_init() < 0
        || z80mem_cmdline_options_init() < 0)
        return -1;

    return 0;
}

void c128_monitor_init(void)
{
    monitor_cpu_type_t asm6502, asmz80;
    monitor_cpu_type_t *asmarray[3];

    asm6502_init(&asm6502);
    asmz80_init(&asmz80);

    asmarray[0] = &asm6502;
    asmarray[1] = &asmz80;
    asmarray[2] = NULL;

    /* Initialize the monitor.  */
    monitor_init(&maincpu_monitor_interface, drive0_get_monitor_interface_ptr(),
                 drive1_get_monitor_interface_ptr(), asmarray);
}

/* C128-specific initialization.  */
int machine_init(void)
{
    c128_log = log_open("C128");

    if (mem_load() < 0)
        return -1;

    if (z80mem_load() < 0)
        return -1;

    /* Setup trap handling.  */
    traps_init();

    /* Initialize serial traps.  */
    if (serial_init(c128_serial_traps, 0xa4) < 0)
        return -1;

    /* Initialize drives. */
    file_system_init();

    /* initialize RS232 handler */
    rs232drv_init();
    c64_rsuser_init();

    /* initialize print devices */
    printer_init();

    /* Initialize the tape emulation.  */
    tape_init(&tapeinit);

    /* Initialize the datasette emulation.  */
    datasette_init();

    /* Fire up the hardware-level drive emulation.  */
    drive_init();

    /* Initialize autostart. FIXME: at least 0xa26 is only for 40 cols */
    autostart_init((CLOCK)
                   (3 * C128_PAL_RFSH_PER_SEC * C128_PAL_CYCLES_PER_RFSH),
                   1, 0xa27, 0xe0, 0xec, 0xee);

    /* Initialize the VDC emulation.  */
    if (vdc_init() == NULL)
        return -1;

    /* Initialize the VIC-II emulation.  */
    if (vicii_init(VICII_EXTENDED) == NULL)
        return -1;

    ciat_init_table();
    cia1_init();
    cia2_init();

    tpi_init();

    acia1_init();

#ifndef COMMON_KBD
    /* Initialize the keyboard.  */
    if (c128_kbd_init() < 0)
        return -1;
#endif

    c64keyboard_init();

    c128_monitor_init();

    /* Initialize vsync and register our hook function.  */
    vsync_init(machine_vsync_hook);
    vsync_set_machine_parameter(machine_timing.rfsh_per_sec,
                                machine_timing.cycles_per_sec);

    /* Initialize sound.  Notice that this does not really open the audio
       device yet.  */
    sound_init(machine_timing.cycles_per_sec, machine_timing.cycles_per_rfsh);

    /* Initialize keyboard buffer.  */
    kbd_buf_init(842, 208, 10, (CLOCK)(machine_timing.rfsh_per_sec
                 * machine_timing.cycles_per_rfsh));

    /* Initialize the C128-specific part of the UI.  */
    c128_ui_init();

    /* Initialize the REU.  */
    reu_init();

#ifdef HAVE_MOUSE
    /* Initialize mouse support (if present).  */
    mouse_init();
#endif

    c128iec_init();

    cartridge_init();

    mmu_init();

    machine_drive_stub();

    return 0;
}

/* C128-specific reset sequence.  */
void machine_specific_reset(void)
{
    serial_reset();

    cia1_reset();
    cia2_reset();
    sid_reset();
    tpi_reset();

    acia1_reset();
    rs232drv_reset();
    rsuser_reset();

    printer_reset();

    vdc_reset();

    /* The VIC-II must be the *last* to be reset.  */
    vicii_reset();

    drive_reset();
    datasette_reset();
    reu_reset();

    z80mem_initialize();
    z80_reset();
}

void machine_powerup(void)
{
    mem_powerup();
    /*vicii_reset_registers();*/
    maincpu_trigger_reset();
}

void machine_specific_shutdown(void)
{
    /* Detach all disks.  */
    file_system_detach_disk_shutdown();

    /* and the tape */
    tape_image_detach(1);

    /* close the video chip(s) */
    vicii_shutdown();
    vdc_shutdown();

    reu_shutdown();
}

void machine_handle_pending_alarms(int num_write_cycles)
{
    vicii_handle_pending_alarms_external(num_write_cycles);
}

/* ------------------------------------------------------------------------- */

/* This hook is called at the end of every frame.  */
static void machine_vsync_hook(void)
{
    CLOCK sub;

    drive_vsync_hook();

    autostart_advance();

    sub = clk_guard_prevent_overflow(maincpu_clk_guard);

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
    return machine_timing.cycles_per_sec;
}

void machine_change_timing(int timeval)
{
    switch (timeval) {
      case MACHINE_SYNC_PAL:
        machine_timing.cycles_per_sec = C128_PAL_CYCLES_PER_SEC;
        machine_timing.cycles_per_rfsh = C128_PAL_CYCLES_PER_RFSH;
        machine_timing.rfsh_per_sec = C128_PAL_RFSH_PER_SEC;
        machine_timing.cycles_per_line = C128_PAL_CYCLES_PER_LINE;
        machine_timing.screen_lines = C128_PAL_SCREEN_LINES;
        break;
      case MACHINE_SYNC_NTSC:
        machine_timing.cycles_per_sec = C128_NTSC_CYCLES_PER_SEC;
        machine_timing.cycles_per_rfsh = C128_NTSC_CYCLES_PER_RFSH;
        machine_timing.rfsh_per_sec = C128_NTSC_RFSH_PER_SEC;
        machine_timing.cycles_per_line = C128_NTSC_CYCLES_PER_LINE;
        machine_timing.screen_lines = C128_NTSC_SCREEN_LINES;
        break;
      default:
        log_error(c128_log, "Unknown machine timing.");
    }

    vsync_set_machine_parameter(machine_timing.rfsh_per_sec,
                                machine_timing.cycles_per_sec);
    sound_set_machine_parameter(machine_timing.cycles_per_sec,
                                machine_timing.cycles_per_rfsh);
    debug_set_machine_parameter(machine_timing.cycles_per_line,
                                machine_timing.screen_lines);
    drive_set_machine_parameter(machine_timing.cycles_per_sec);
    clk_guard_set_clk_base(maincpu_clk_guard, machine_timing.cycles_per_rfsh);

    vicii_change_timing(&machine_timing);

    machine_powerup();
}

/* ------------------------------------------------------------------------- */

#define SNAP_MACHINE_NAME   "C128"
#define SNAP_MAJOR          0
#define SNAP_MINOR          0

int machine_write_snapshot(const char *name, int save_roms, int save_disks,
                           int event_mode)
{
    snapshot_t *s;

    s = snapshot_create(name, ((BYTE)(SNAP_MAJOR)), ((BYTE)(SNAP_MINOR)),
                        SNAP_MACHINE_NAME);
    if (s == NULL)
        return -1;

    if (maincpu_snapshot_write_module(s) < 0
        || c128_snapshot_write_module(s, save_roms) < 0
        || cia1_snapshot_write_module(s) < 0
        || cia2_snapshot_write_module(s) < 0
        || sid_snapshot_write_module(s) < 0
        || drive_snapshot_write_module(s, save_disks, save_roms) < 0
        || vicii_snapshot_write_module(s) < 0
        || event_snapshot_write_module(s, event_mode) < 0) {
        snapshot_close(s);
        ioutil_remove(name);
        return -1;
    }

    snapshot_close(s);
    return 0;
}

int machine_read_snapshot(const char *name, int event_mode)
{
    snapshot_t *s;
    BYTE minor, major;

    s = snapshot_open(name, &major, &minor, SNAP_MACHINE_NAME);
    if (s == NULL)
        return -1;

    if (major != SNAP_MAJOR || minor != SNAP_MINOR) {
        log_message(c128_log,
                    "Snapshot version (%d.%d) not valid: expecting %d.%d.",
                    major, minor, SNAP_MAJOR, SNAP_MINOR);
        goto fail;
    }

    vicii_prepare_for_snapshot();

    if (maincpu_snapshot_read_module(s) < 0
        || c128_snapshot_read_module(s) < 0
        || cia1_snapshot_read_module(s) < 0
        || cia2_snapshot_read_module(s) < 0
        || sid_snapshot_read_module(s) < 0
        || drive_snapshot_read_module(s) < 0
        || vicii_snapshot_read_module(s) < 0
        || event_snapshot_read_module(s, event_mode) < 0)
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
    return -1;
}

void machine_play_psid(int tune)
{
}

struct video_canvas_s *machine_canvas_get(unsigned int window)
{
    if (window == 0)
        return vdc_get_canvas();
    if (window == 1)
        return vicii_get_canvas();

    return NULL;
}

int machine_screenshot(screenshot_t *screenshot, struct video_canvas_s *canvas)
{
    if (canvas == vicii_get_canvas()) {
        vicii_screenshot(screenshot);
        return 0;
    }
    if (canvas == vdc_get_canvas()) {
        vdc_screenshot(screenshot);
        return 0;
    }

    return -1;
}

int machine_canvas_async_refresh(struct canvas_refresh_s *refresh,
                                 struct video_canvas_s *canvas)
{
    if (canvas == vicii_get_canvas()) {
        vicii_async_refresh(refresh);
        return 0;
    }
    if (canvas == vdc_get_canvas()) {
        vdc_async_refresh(refresh);
        return 0;
    }

    return -1;
}

void machine_update_memory_ptrs(void)
{
     vicii_update_memory_ptrs_external();
}

int machine_sid2_check_range(unsigned int sid2_adr)
{
    if (sid2_adr >= 0xd420 && sid2_adr <= 0xd4e0)
        return 0;

    if (sid2_adr >= 0xde00 && sid2_adr <= 0xdfe0)
        return 0;

    return -1;
}

unsigned int machine_num_keyboard_mappings(void)
{
    return NUM_KEYBOARD_MAPPINGS;
}

