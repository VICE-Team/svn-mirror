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

#include "attach.h"
#include "autostart.h"
#include "c64-cmdline-options.h"
#include "c64-resources.h"
#include "c64-snapshot.h"
#include "c64.h"
#include "c64acia.h"
#include "c64cart.h"
#include "c64cia.h"
#include "c64iec.h"
#include "c64keyboard.h"
#include "c64memrom.h"
#include "c64rsuser.h"
#include "c64tpi.h"
#include "c64ui.h"
#include "cartridge.h"
#include "ciatimer.h"
#include "clkguard.h"
#include "datasette.h"
#include "debug.h"
#include "drive-cmdline-options.h"
#include "drive-resources.h"
#include "drive.h"
#include "drivecpu.h"
#include "iecdrive.h"
#include "interrupt.h"
#include "kbdbuf.h"
#include "keyboard.h"
#include "log.h"
#include "machine-drive.h"
#include "machine.h"
#include "maincpu.h"
#include "mem.h"
#include "monitor.h"
#include "parallel.h"
#include "patchrom.h"
#include "printer.h"
#include "psid.h"
#include "resources.h"
#include "reu.h"
#include "rs232drv.h"
#include "rsuser.h"
#include "screenshot.h"
#include "serial.h"
#include "sid-cmdline-options.h"
#include "sid-resources.h"
#include "sid.h"
#include "sound.h"
#include "tape.h"
#include "traps.h"
#include "types.h"
#include "vicii.h"
#include "video.h"
#include "vsidui.h"
#include "vsync.h"

#ifdef HAVE_MOUSE
#include "mouse.h"
#endif

#ifdef HAVE_TFE
#include "tfe.h"
#endif /* #ifdef HAVE_TFE */


machine_context_t machine_context;

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

static const trap_t c64_serial_traps[] = {
    {
        "SerialListen",
        0xED24,
        0xEDAB,
        { 0x20, 0x97, 0xEE },
        serial_trap_attention,
        c64memrom_trap_read,
        c64memrom_trap_store
    },
    {
        "SerialSaListen",
        0xED36,
        0xEDAB,
        { 0x78, 0x20, 0x8E },
        serial_trap_attention,
        c64memrom_trap_read,
        c64memrom_trap_store
    },
    {
        "SerialSendByte",
        0xED40,
        0xEDAB,
        { 0x78, 0x20, 0x97 },
        serial_trap_send,
        c64memrom_trap_read,
        c64memrom_trap_store
    },
    {
        "SerialReceiveByte",
        0xEE13,
        0xEDAB,
        { 0x78, 0xA9, 0x00 },
        serial_trap_receive,
        c64memrom_trap_read,
        c64memrom_trap_store
    },
    {
        "SerialReady",
        0xEEA9,
        0xEDAB,
        { 0xAD, 0x00, 0xDD },
        serial_trap_ready,
        c64memrom_trap_read,
        c64memrom_trap_store
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
static const trap_t c64_tape_traps[] = {
    {
        "TapeFindHeader",
        0xF72F,
        0xF732,
        { 0x20, 0x41, 0xF8 },
        tape_find_header_trap,
        c64memrom_trap_read,
        c64memrom_trap_store
    },
    {
        "TapeReceive",
        0xF8A1,
        0xFC93,
        { 0x20, 0xBD, 0xFC },
        tape_receive_trap,
        c64memrom_trap_read,
        c64memrom_trap_store
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
static machine_timing_t machine_timing;

/* ------------------------------------------------------------------------ */

/* C64-specific resource initialization.  This is called before initializing
   the machine itself with `machine_init()'.  */
int machine_resources_init(void)
{
    if (traps_resources_init() < 0
        || vsync_resources_init() < 0
        || video_resources_pal_init() < 0
        || video_resources_init() < 0
        || c64_resources_init() < 0
        || reu_resources_init() < 0
#ifdef HAVE_TFE
        || tfe_resources_init() < 0
#endif /* #ifdef HAVE_TFE */
        || vicii_resources_init() < 0
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
        )
        return -1;

    if (vsid_mode && psid_init_resources() < 0)
        return -1;

    return 0;
}

void machine_resources_shutdown(void)
{
    c64_resources_shutdown();
    reu_resources_shutdown();
    sound_resources_shutdown();
    rs232drv_resources_shutdown();
    printer_resources_shutdown();
    drive_resources_shutdown();
    cartridge_resources_shutdown();
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
#ifdef HAVE_TFE
        || tfe_cmdline_options_init() < 0
#endif /* #ifdef HAVE_TFE */
        || vicii_cmdline_options_init() < 0
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
        )
        return -1;

    return 0;
}

static void c64_monitor_init(void)
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
    cia1_setup_context(&machine_context);
    cia2_setup_context(&machine_context);
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

        /* Initialize RS232 handler.  */
        rs232drv_init();
        c64_rsuser_init();

        /* Initialize print devices.  */
        printer_init();

        /* Initialize the tape emulation.  */
        tape_init(&tapeinit);

        /* Initialize the datasette emulation.  */
        datasette_init();

        /* Fire up the hardware-level drive emulation.  */
        drive_init();

        /* Initialize autostart.  */
        autostart_init((CLOCK)(3 * C64_PAL_RFSH_PER_SEC
                       * C64_PAL_CYCLES_PER_RFSH),
                       1, 0xcc, 0xd1, 0xd3, 0xd5);
    }

    /* Initialize the VIC-II emulation. */
    if (!vicii_init(VICII_STANDARD) && !console_mode && !vsid_mode)
        return -1;

    ciat_init_table();
    cia1_init(&(machine_context.cia1));
    cia2_init(&(machine_context.cia2));

    if (!vsid_mode) {
        tpi_init();

        acia1_init();

#ifndef COMMON_KBD
        /* Initialize the keyboard.  */
        if (c64_kbd_init() < 0)
            return -1;
#endif

        c64keyboard_init();
    }

    c64_monitor_init();

    /* Initialize vsync and register our hook function.  */
    vsync_init(machine_vsync_hook);
    vsync_set_machine_parameter(machine_timing.rfsh_per_sec,
                                machine_timing.cycles_per_sec);

    /* Initialize sound.  Notice that this does not really open the audio
       device yet.  */
    sound_init(machine_timing.cycles_per_sec, machine_timing.cycles_per_rfsh);

    /* Initialize keyboard buffer.  */
    kbd_buf_init(631, 198, 10, (CLOCK)(machine_timing.rfsh_per_sec
                 * machine_timing.cycles_per_rfsh));

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

#ifdef HAVE_TFE
        /* Initialize the TFE.  */
        tfe_init();
#endif /* #ifdef HAVE_TFE */

#ifdef HAVE_MOUSE
        /* Initialize mouse support (if present).  */
        mouse_init();
#endif

        c64iec_init();

        cartridge_init();
    }

    machine_drive_stub();

    return 0;
}

/* C64-specific reset sequence.  */
void machine_specific_reset(void)
{
    serial_reset();

    ciacore_reset(&(machine_context.cia1));
    ciacore_reset(&(machine_context.cia2));
    sid_reset();

    if (!vsid_mode) {
        tpi_reset();

        acia1_reset();
        rs232drv_reset();
        rsuser_reset();

        printer_reset();

        /* FIXME */
        /* reset_reu(); */
    }

    /* The VIC-II must be the *last* to be reset.  */
    vicii_reset();

    if (vsid_mode) {
        psid_init_tune();
        return;
    }

    drive_reset();
    datasette_reset();
    reu_reset();
}

void machine_specific_powerup(void)
{
    /* Hard reset unloads PSID. */
    if (vsid_mode) {
        machine_play_psid(-1);
    }

    vicii_reset_registers();
}

void machine_specific_shutdown(void)
{
    /* Detach all disks.  */
    if (!vsid_mode) {
        file_system_detach_disk_shutdown();
    }

    /* and the tape */
    tape_image_detach(1);

    /* and cartridge */
    cartridge_detach_image();

    /* close the video chip(s) */
    vicii_shutdown();

    reu_shutdown();

    if (vsid_mode) {
        vsid_ui_close();
    }
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

    if (vsid_mode) {
        unsigned int playtime;
        static unsigned int time=0;

        playtime = (psid_increment_frames() * machine_timing.cycles_per_rfsh)
                   / machine_timing.cycles_per_sec;
        if (playtime != time) {
            vsid_ui_display_time(playtime);
            time = playtime;
        }
        clk_guard_prevent_overflow(maincpu_clk_guard);
        return;
    }

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
    c64keyboard_restore_key(v);
    return 1;
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
        machine_timing.cycles_per_sec = C64_PAL_CYCLES_PER_SEC;
        machine_timing.cycles_per_rfsh = C64_PAL_CYCLES_PER_RFSH;
        machine_timing.rfsh_per_sec = C64_PAL_RFSH_PER_SEC;
        machine_timing.cycles_per_line = C64_PAL_CYCLES_PER_LINE;
        machine_timing.screen_lines = C64_PAL_SCREEN_LINES;
        break;
      case MACHINE_SYNC_NTSC:
        machine_timing.cycles_per_sec = C64_NTSC_CYCLES_PER_SEC;
        machine_timing.cycles_per_rfsh = C64_NTSC_CYCLES_PER_RFSH;
        machine_timing.rfsh_per_sec = C64_NTSC_RFSH_PER_SEC;
        machine_timing.cycles_per_line = C64_NTSC_CYCLES_PER_LINE;
        machine_timing.screen_lines = C64_NTSC_SCREEN_LINES;
        break;
      case MACHINE_SYNC_NTSCOLD:
        machine_timing.cycles_per_sec = C64_NTSCOLD_CYCLES_PER_SEC;
        machine_timing.cycles_per_rfsh = C64_NTSCOLD_CYCLES_PER_RFSH;
        machine_timing.rfsh_per_sec = C64_NTSCOLD_RFSH_PER_SEC;
        machine_timing.cycles_per_line = C64_NTSCOLD_CYCLES_PER_LINE;
        machine_timing.screen_lines = C64_NTSCOLD_SCREEN_LINES;
        break;
      default:
        log_error(c64_log, "Unknown machine timing.");
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

    machine_trigger_reset(MACHINE_RESET_MODE_HARD);
}

/* ------------------------------------------------------------------------- */

int machine_write_snapshot(const char *name, int save_roms, int save_disks,
                           int event_mode)
{
    return c64_snapshot_write(name, save_roms, save_disks, event_mode);
}

int machine_read_snapshot(const char *name, int event_mode)
{
    return c64_snapshot_read(name, event_mode);
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

struct video_canvas_s *machine_canvas_get(unsigned int window)
{
    if (window == 0)
        return vicii_get_canvas();

    return NULL;
}

int machine_screenshot(screenshot_t *screenshot, struct video_canvas_s *canvas)
{
    if (canvas != vicii_get_canvas())
        return -1;

    vicii_screenshot(screenshot);
    return 0;
}

int machine_canvas_async_refresh(struct canvas_refresh_s *refresh,
                                 struct video_canvas_s *canvas)
{
    if (canvas != vicii_get_canvas())
        return -1;

    vicii_async_refresh(refresh);
    return 0;
}

void machine_update_memory_ptrs(void)
{
    vicii_update_memory_ptrs_external();
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

void machine_traps_enable(int enable)
{
    parallel_bus_enable(enable);
}

