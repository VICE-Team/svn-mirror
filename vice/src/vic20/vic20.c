/*
 * vic20.c
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
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

#include "autostart.h"
#include "cartridge.h"
#include "clkguard.h"
#include "cmdline.h"
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
#include "machine-printer.h"
#include "machine-video.h"
#include "machine.h"
#include "maincpu.h"
#include "mem.h"
#include "monitor.h"
#include "parallel.h"
#include "printer.h"
#include "rs232drv.h"
#include "rsuser.h"
#include "screenshot.h"
#include "serial.h"
#include "sid.h"
#include "sid-cmdline-options.h"
#include "sid-resources.h"
#include "sound.h"
#include "tape.h"
#include "traps.h"
#include "types.h"
#include "via.h"
#include "vic.h"
#include "vic20-cmdline-options.h"
#include "vic20-midi.h"
#include "vic20-resources.h"
#include "vic20-snapshot.h"
#include "vic20.h"
#include "vic20iec.h"
#include "vic20ieeevia.h"
#include "vic20mem.h"
#include "vic20memrom.h"
#include "vic20sound.h"
#include "vic20rsuser.h"
#include "vic20ui.h"
#include "vic20via.h"
#include "video.h"
#include "vsync.h"

int io_source;

machine_context_t machine_context;

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

static const trap_t vic20_serial_traps[] = {
    {
        "SerialListen",
        0xEE2E,
        0xEEB2,
        { 0x20, 0xA0, 0xE4 },
        serial_trap_attention,
        vic20memrom_trap_read,
        vic20memrom_trap_store
    },
    {
        "SerialSaListen",
        0xEE40,
        0xEEB2,
        { 0x20, 0x8D, 0xEF },
        serial_trap_attention,
        vic20memrom_trap_read,
        vic20memrom_trap_store
    },
    {
        "SerialSendByte",
        0xEE49,
        0xEEB2,
        { 0x78, 0x20, 0xA0 },
        serial_trap_send,
        vic20memrom_trap_read,
        vic20memrom_trap_store
    },
    {
        "SerialReceiveByte",
        0xEF19,
        0xEEB2,
        { 0x78, 0xA9, 0x00 },
        serial_trap_receive,
        vic20memrom_trap_read,
        vic20memrom_trap_store
    },
    {
        "SerialReady",
        0xE4B2,
        0xEEB2,
        { 0xAD, 0x1F, 0x91 },
        serial_trap_ready,
        vic20memrom_trap_read,
        vic20memrom_trap_store
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
static const trap_t vic20_tape_traps[] = {
    {
        "TapeFindHeader",
        0xF7B2,
        0xF7B5,
        { 0x20, 0xC0, 0xF8 },
        tape_find_header_trap,
        vic20memrom_trap_read,
        vic20memrom_trap_store
    },
    {
        "TapeReceive",
        0xF90B,
        0xFCCF,
        { 0x20, 0xFB, 0xFC },
        tape_receive_trap,
        vic20memrom_trap_read,
        vic20memrom_trap_store
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
        || machine_video_resources_init() < 0
        || vic20_resources_init() < 0
        || vic_resources_init() < 0
        || sound_resources_init() < 0
        || sidcart_resources_init() < 0
        || rs232drv_resources_init() < 0
        || rsuser_resources_init() < 0
        || serial_resources_init() < 0
        || printer_resources_init() < 0
#ifndef COMMON_KBD
        || kbd_resources_init() < 0
#endif
        || drive_resources_init() < 0
        || datasette_resources_init() < 0
        || cartridge_resources_init() <0
#ifdef HAVE_MIDI
        || vic20_midi_resources_init() <0
#endif
)
        return -1;

    return 0;
}

void machine_resources_shutdown(void)
{
    serial_shutdown();
    video_resources_shutdown();
    vic20_resources_shutdown();
    sound_resources_shutdown();
    rs232drv_resources_shutdown();
    printer_resources_shutdown();
    drive_resources_shutdown();
    cartridge_resources_shutdown();
#ifdef HAVE_MIDI
    midi_resources_shutdown();
#endif
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
        || sidcart_cmdline_options_init() < 0
        || rs232drv_cmdline_options_init() < 0
        || rsuser_cmdline_options_init() < 0
        || serial_cmdline_options_init() < 0
        || printer_cmdline_options_init() < 0
#ifndef COMMON_KBD
        || kbd_cmdline_options_init() < 0
#endif
        || drive_cmdline_options_init() < 0
        || datasette_cmdline_options_init() < 0
        || cartridge_cmdline_options_init() < 0
#ifdef HAVE_MIDI
        || vic20_midi_cmdline_options_init() < 0
#endif
)
        return -1;

    return 0;
}

static void vic20_monitor_init(void)
{
    unsigned int dnr;
    monitor_cpu_type_t asm6502;
    monitor_interface_t *drive_interface_init[DRIVE_NUM];
    monitor_cpu_type_t *asmarray[2];

    asmarray[0]=&asm6502;
    asmarray[1]=NULL;

    asm6502_init(&asm6502);

    for (dnr = 0; dnr < DRIVE_NUM; dnr++)
        drive_interface_init[dnr] = drivecpu_monitor_interface_get(dnr);

    /* Initialize the monitor.  */
    monitor_init(maincpu_monitor_interface_get(), drive_interface_init,
                 asmarray);
}

void machine_setup_context(void)
{
    vic20via1_setup_context(&machine_context);
    vic20via2_setup_context(&machine_context);
    vic20ieeevia1_setup_context(&machine_context);
    vic20ieeevia2_setup_context(&machine_context);
    machine_printer_setup_context(&machine_context);
}

void machine_handle_pending_alarms(int num_write_cycles)
{
}

/* VIC20-specific initialization.  */
int machine_specific_init(void)
{
    vic20_log = log_open("VIC20");

    if (mem_load() < 0)
        return -1;

    /* Setup trap handling.  */
    traps_init();

    /* Initialize serial traps.  If user does not want them, or if the
       ``drive'' emulation is used, do not install them.  */
    if (serial_init(vic20_serial_traps) < 0)
        return -1;

    serial_trap_init(0xa4);
    serial_iec_bus_init();

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

    via1_init(machine_context.via1);
    via2_init(machine_context.via2);

    ieeevia1_init(machine_context.ieeevia1);
    ieeevia2_init(machine_context.ieeevia2);

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
    kbdbuf_init(631, 198, 10, (CLOCK)(machine_timing.cycles_per_rfsh
                * machine_timing.rfsh_per_sec));

    /* Initialize the VIC20-specific part of the UI.  */
    vic20ui_init();

    vic20iec_init();

#ifdef HAVE_MIDI
    midi_init();
#endif

    machine_drive_stub();

    return 0;
}

/* VIC20-specific reset sequence.  */
void machine_specific_reset(void)
{
    serial_traps_reset();

    viacore_reset(machine_context.via1);
    viacore_reset(machine_context.via2);
    vic_reset();
    vic_sound_reset();
    sid_reset();

    viacore_reset(machine_context.ieeevia1);
    viacore_reset(machine_context.ieeevia2);

    rs232drv_reset();
    rsuser_reset();
#ifdef HAVE_MIDI
    midi_reset();
#endif

    printer_reset();
    drive_reset();
    datasette_reset();
}

void machine_specific_powerup(void)
{
}

void machine_specific_shutdown(void)
{
    /* and the tape */
    tape_image_detach_internal(1);

    viacore_shutdown(machine_context.via1);
    viacore_shutdown(machine_context.via2);
    viacore_shutdown(machine_context.ieeevia1);
    viacore_shutdown(machine_context.ieeevia2);

    /* close the video chip(s) */
    vic_shutdown();

    vic20ui_shutdown();
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
    drivecpu_prevent_clk_overflow_all(sub);
}

void machine_set_restore_key(int v)
{
    viacore_signal(machine_context.via2,
                   VIA_SIG_CA1, v ? VIA_SIG_FALL : VIA_SIG_RISE);
}

int machine_has_restore_key(void)
{
    return 1;
}
/* ------------------------------------------------------------------------- */

long machine_get_cycles_per_second(void)
{
    return machine_timing.cycles_per_sec;
}

void machine_get_line_cycle(unsigned int *line, unsigned int *cycle, int *half_cycle)
{
    *line = (unsigned int)((maincpu_clk) / machine_timing.cycles_per_line
            % machine_timing.screen_lines);

    *cycle = (unsigned int)((maincpu_clk) % machine_timing.cycles_per_line);

    *half_cycle = (int)-1;
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
    sid_set_machine_parameter(machine_timing.cycles_per_sec);
    debug_set_machine_parameter(machine_timing.cycles_per_line,
                                machine_timing.screen_lines);
    drive_set_machine_parameter(machine_timing.cycles_per_sec);
    serial_iec_device_set_machine_parameter(machine_timing.cycles_per_sec);
    clk_guard_set_clk_base(maincpu_clk_guard, machine_timing.cycles_per_rfsh);

    vic_change_timing();

    mem_patch_kernal();

    machine_trigger_reset(MACHINE_RESET_MODE_HARD);
}

/* ------------------------------------------------------------------------- */

int machine_write_snapshot(const char *name, int save_roms, int save_disks,
                           int event_mode)
{
    return vic20_snapshot_write(name, save_roms, save_disks, event_mode);
}

int machine_read_snapshot(const char *name, int event_mode)
{
    return vic20_snapshot_read(name, event_mode);
}


/* ------------------------------------------------------------------------- */
int machine_autodetect_psid(const char *name)
{
    return -1;
}

void machine_play_psid(int tune)
{
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

int machine_num_keyboard_mappings(void)
{
    return NUM_KEYBOARD_MAPPINGS;
}

struct image_contents_s *machine_diskcontents_bus_read(unsigned int unit)
{
    return diskcontents_iec_read(unit);
}

BYTE machine_tape_type_default(void)
{
    return TAPE_CAS_TYPE_BAS;
}

