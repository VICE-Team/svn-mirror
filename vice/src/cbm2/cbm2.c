/*
 * cbm2.c
 *
 * Written by
 *  André Fachat <fachat@physik.tu-chemnitz.de>
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

#include "alarm.h"
#include "attach.h"
#include "autostart.h"
#include "cbm2-cmdline-options.h"
#include "cbm2-resources.h"
#include "cbm2-snapshot.h"
#include "cbm2.h"
#include "cbm2acia.h"
#include "cbm2cia.h"
#include "cbm2iec.h"
#include "cbm2mem.h"
#include "cbm2tpi.h"
#include "cbm2ui.h"
#include "cia.h"
#include "clkguard.h"
#include "cmdline.h"
#include "crtc.h"
#include "datasette.h"
#include "debug.h"
#include "drive-cmdline-options.h"
#include "drive-resources.h"
#include "drive.h"
#include "drivecpu.h"
#include "iecdrive.h"
#include "kbdbuf.h"
#include "keyboard.h"
#include "log.h"
#include "machine-drive.h"
#include "machine.h"
#include "maincpu.h"
#include "mem.h"
#include "monitor.h"
#include "parallel.h"
#include "printer.h"
#include "resources.h"
#include "rs232drv.h"
#include "screenshot.h"
#include "serial.h"
#include "sid-cmdline-options.h"
#include "sid-resources.h"
#include "sid.h"
#include "snapshot.h"
#include "sound.h"
#include "tape.h"
#include "tpi.h"
#include "traps.h"
#include "types.h"
#include "via.h"
#include "vicii.h"
#include "video.h"
#include "vsync.h"


machine_context_t machine_context;

#define NUM_KEYBOARD_MAPPINGS 6

const char *machine_keymap_res_name_list[NUM_KEYBOARD_MAPPINGS] = {
    "KeymapBusinessUKSymFile", "KeymapBusinessUKPosFile",
    "KeymapGraphicsSymFile", "KeymapGraphicsPosFile",
    "KeymapBusinessDESymFile", "KeymapBusinessDEPosFile"
};

char *machine_keymap_file_list[NUM_KEYBOARD_MAPPINGS] = {
    NULL, NULL, NULL, NULL, NULL, NULL
};

const char machine_name[] = "CBM-II";
int machine_class = VICE_MACHINE_CBM2;

static void machine_vsync_hook(void);

#define C500_POWERLINE_CYCLES_PER_IRQ (C500_PAL_CYCLES_PER_RFSH)

/*
static long cbm2_cycles_per_sec = C610_PAL_CYCLES_PER_SEC;
static double cbm2_rfsh_per_sec = C610_PAL_RFSH_PER_SEC;
static long cbm2_cycles_per_rfsh = C610_PAL_CYCLES_PER_RFSH;
*/

static log_t cbm2_log = LOG_ERR;
static machine_timing_t machine_timing;

int cbm2_isC500 = 0;

/* ------------------------------------------------------------------------- */

int cbm2_is_c500(void)
{
    return cbm2_isC500;
}

/* ------------------------------------------------------------------------- */

/* CBM-II-specific resource initialization.  This is called before initializing
   the machine itself with `machine_init()'.  */
int machine_resources_init(void)
{
    if (traps_resources_init() < 0
        || vsync_resources_init() < 0
        || video_resources_pal_init() < 0
        || video_resources_init() < 0
        || cbm2_resources_init() < 0
        || crtc_resources_init() < 0
        || vicii_resources_init() < 0
        || sound_resources_init() < 0
        || sid_resources_init() < 0
        || drive_resources_init() < 0
        || datasette_resources_init() < 0
        || acia1_resources_init() < 0
        || rs232drv_resources_init() < 0
        || printer_resources_init() < 0
#ifndef COMMON_KBD
        || pet_kbd_resources_init() < 0
#endif
        )
        return -1;
    return 0;
}

void machine_resources_shutdown(void)
{
    video_resources_shutdown();
    cbm2_resources_shutdown();
    sound_resources_shutdown();
    rs232drv_resources_shutdown();
    printer_resources_shutdown();
    drive_resources_shutdown();
}

/* CBM-II-specific command-line option initialization.  */
int machine_cmdline_options_init(void)
{
    if (traps_cmdline_options_init() < 0
        || vsync_cmdline_options_init() < 0
        || video_init_cmdline_options() < 0
        || cbm2_cmdline_options_init() < 0
        || crtc_cmdline_options_init() < 0
        || vicii_cmdline_options_init() < 0
        || sound_cmdline_options_init() < 0
        || sid_cmdline_options_init() < 0
        || drive_cmdline_options_init() < 0
        || datasette_cmdline_options_init() < 0
        || acia1_cmdline_options_init() < 0
        || rs232drv_cmdline_options_init() < 0
        || printer_cmdline_options_init() < 0
#ifndef COMMON_KBD
        || pet_kbd_cmdline_options_init() < 0
#endif
        )
        return -1;

    return 0;
}

/* ------------------------------------------------------------------------- */
/* provide the 50(?)Hz IRQ signal for the standard IRQ */

#define SIGNAL_VERT_BLANK_OFF tpicore_set_int(machine_context.tpi1, 0, 1);

#define SIGNAL_VERT_BLANK_ON  tpicore_set_int(machine_context.tpi1, 0, 0);

/* ------------------------------------------------------------------------- */
/* for the C500 there is a powerline IRQ... */

static alarm_t *c500_powerline_clk_alarm = NULL;
static CLOCK c500_powerline_clk = 0;

void c500_powerline_clk_alarm_handler(CLOCK offset) {

    c500_powerline_clk += C500_POWERLINE_CYCLES_PER_IRQ;

    SIGNAL_VERT_BLANK_OFF

    alarm_set(c500_powerline_clk_alarm, c500_powerline_clk);

    SIGNAL_VERT_BLANK_ON

}

static void c500_powerline_clk_overflow_callback(CLOCK sub, void *data)
{
    c500_powerline_clk -= sub;
}

/* ------------------------------------------------------------------------- */
/* ... while the other CBM-II use the CRTC retrace signal. */

static void cbm2_crtc_signal(unsigned int signal) {
    if (signal) {
        SIGNAL_VERT_BLANK_ON
    } else {
        SIGNAL_VERT_BLANK_OFF
    }
}

/* ------------------------------------------------------------------------- */

static void cbm2_monitor_init(void)
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
    tpi1_setup_context(&machine_context);
    tpi2_setup_context(&machine_context);
}

/* CBM-II-specific initialization.  */
int machine_init(void)
{
    cbm2_log = log_open("CBM2");

    cbm2_init_ok = 1;

    /* Setup trap handling - must be before mem_load() */
    traps_init();

    if (mem_load() < 0)
        return -1;

    /* No traps installed on the CBM-II.  */
    if (serial_init(NULL) < 0)
        return -1;

    /* Initialize drives. */
    file_system_init();

    rs232drv_init();

    /* initialize print devices */
    printer_init();

    if (!cbm2_isC500) {
        /* Initialize the CRTC emulation.  */
        if (crtc_init() == NULL)
            return -1;
        crtc_set_retrace_callback(cbm2_crtc_signal);
        crtc_set_retrace_type(0);
        crtc_set_hw_options(1, 0x7ff, 0x1000, 512, -0x2000);
    } else {
        /* Initialize the VIC-II emulation.  */
        if (vicii_init(VICII_STANDARD) == NULL)
            return -1;

        /*
        c500_set_phi1_bank(15);
        c500_set_phi2_bank(15);
        */

        c500_powerline_clk_alarm = alarm_new(maincpu_alarm_context,
                                             "C500PowerlineClk",
                                             c500_powerline_clk_alarm_handler);
        clk_guard_add_callback(maincpu_clk_guard,
                               c500_powerline_clk_overflow_callback, NULL);
        machine_timing.cycles_per_sec = C500_PAL_CYCLES_PER_SEC;
        machine_timing.rfsh_per_sec = C500_PAL_RFSH_PER_SEC;
        machine_timing.cycles_per_rfsh = C500_PAL_CYCLES_PER_RFSH;
    }

    cia1_init(machine_context.cia1);
    acia1_init();
    tpi1_init(machine_context.tpi1);
    tpi2_init(machine_context.tpi2);

#ifndef COMMON_KBD
    /* Initialize the keyboard.  */
    if (cbm2_kbd_init() < 0)
        return -1;
#endif

    /* Initialize the datasette emulation.  */
    datasette_init();

    /* Fire up the hardware-level 1541 emulation.  */
    drive_init();

    cbm2_monitor_init();

    /* Initialize vsync and register our hook function.  */
    vsync_init(machine_vsync_hook);
    vsync_set_machine_parameter(machine_timing.rfsh_per_sec,
                                machine_timing.cycles_per_sec);

    /* Initialize sound.  Notice that this does not really open the audio
       device yet.  */
    sound_init(machine_timing.cycles_per_sec, machine_timing.cycles_per_rfsh);

    /* Initialize the CBM-II-specific part of the UI.  */
    cbm2_ui_init();

    cbm2iec_init();

    machine_drive_stub();

    return 0;
}

/* CBM-II-specific initialization.  */
void machine_specific_reset(void)
{
    acia1_reset();
    ciacore_reset(machine_context.cia1);
    tpicore_reset(machine_context.tpi1);
    tpicore_reset(machine_context.tpi2);

    sid_reset();

    if (!cbm2_isC500) {
        crtc_reset();
    } else {
        c500_powerline_clk = maincpu_clk + C500_POWERLINE_CYCLES_PER_IRQ;
        alarm_set(c500_powerline_clk_alarm, c500_powerline_clk);
        vicii_reset();
    }
    printer_reset();

    rs232drv_reset();

    drive_reset();
    datasette_reset();

    mem_reset();
}

void machine_specific_powerup(void)
{
}

void machine_specific_shutdown(void)
{
    /* Detach all disks.  */
    file_system_detach_disk_shutdown();

    /* and the tape */
    tape_image_detach(1);

    ciacore_shutdown(machine_context.cia1);
    tpicore_shutdown(machine_context.tpi1);
    tpicore_shutdown(machine_context.tpi2);

    /* close the video chip(s) */
    if (cbm2_isC500) {
        vicii_shutdown();
    } else {
        crtc_shutdown();
    }
}

void machine_handle_pending_alarms(int num_write_cycles)
{
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

/* Dummy - no restore key.  */
int machine_set_restore_key(int v)
{
    return 0;   /* key not used -> lookup in keymap */
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
    if (cbm2_isC500) {
        machine_timing.cycles_per_sec = C500_PAL_CYCLES_PER_SEC;
        machine_timing.cycles_per_rfsh = C500_PAL_CYCLES_PER_RFSH;
        machine_timing.rfsh_per_sec = C500_PAL_RFSH_PER_SEC;
        machine_timing.cycles_per_line = C500_PAL_CYCLES_PER_LINE;
        machine_timing.screen_lines = C500_PAL_SCREEN_LINES;
    } else {
        machine_timing.cycles_per_sec = C610_PAL_CYCLES_PER_SEC;
        machine_timing.cycles_per_rfsh = C610_PAL_CYCLES_PER_RFSH;
        machine_timing.rfsh_per_sec = C610_PAL_RFSH_PER_SEC;
        machine_timing.cycles_per_line = C610_PAL_CYCLES_PER_LINE;
        machine_timing.screen_lines = C610_PAL_SCREEN_LINES;
    }

    debug_set_machine_parameter(machine_timing.cycles_per_line,
                                machine_timing.screen_lines);
    drive_set_machine_parameter(machine_timing.cycles_per_sec);
    clk_guard_set_clk_base(maincpu_clk_guard, machine_timing.cycles_per_rfsh);

    vicii_change_timing(&machine_timing);
}

/* Set the screen refresh rate, as this is variable in the CRTC */
void machine_set_cycles_per_frame(long cpf) {

    machine_timing.cycles_per_rfsh = cpf;
    machine_timing.rfsh_per_sec = ((double)machine_timing.cycles_per_sec)
                                  / ((double)cpf);

    vsync_set_machine_parameter(machine_timing.rfsh_per_sec,
                                machine_timing.cycles_per_sec);
}

/* ------------------------------------------------------------------------- */

int machine_write_snapshot(const char *name, int save_roms, int save_disks,
                           int event_mode)
{
    return cbm2_snapshot_write(name, save_roms, save_disks, event_mode);
}

int machine_read_snapshot(const char *name, int event_mode)
{
    return cbm2_snapshot_read(name, event_mode);
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
        return crtc_get_canvas();
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
    if (canvas == crtc_get_canvas()) {
        crtc_screenshot(screenshot);
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
    if (canvas == crtc_get_canvas()) {
        crtc_async_refresh(refresh);
        return 0;
    }

    return -1;
}

/*-----------------------------------------------------------------------*/

/*
 * C500 extra data (state of 50Hz clk)
 */
#define C500DATA_DUMP_VER_MAJOR   0
#define C500DATA_DUMP_VER_MINOR   0

/*
 * DWORD        IRQCLK          CPU clock ticks until next 50 Hz IRQ
 *
 */

static const char module_name[] = "C500DATA";

int cbm2_c500_snapshot_write_module(snapshot_t *p)
{
    snapshot_module_t *m;

    m = snapshot_module_create(p, module_name, C500DATA_DUMP_VER_MAJOR,
                               C500DATA_DUMP_VER_MINOR);
    if (m == NULL)
        return -1;

    SMW_DW(m, c500_powerline_clk - maincpu_clk);

    snapshot_module_close(m);

    return 0;
}

int cbm2_c500_snapshot_read_module(snapshot_t *p)
{
    BYTE vmajor, vminor;
    snapshot_module_t *m;
    DWORD dword;

    m = snapshot_module_open(p, module_name, &vmajor, &vminor);
    if (m == NULL)
        return -1;

    if (vmajor != C500DATA_DUMP_VER_MAJOR) {
        snapshot_module_close(m);
        return -1;
    }

    SMR_DW(m, &dword);
    c500_powerline_clk = maincpu_clk + dword;
    alarm_set(c500_powerline_clk_alarm, c500_powerline_clk);

    snapshot_module_close(m);

    return 0;
}

int machine_sid2_check_range(unsigned int sid2_adr)
{
    if (sid2_adr >= 0xda20 && sid2_adr <= 0xdae0)
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

struct image_contents_s *machine_diskcontents_bus_read(unsigned int unit)
{
    return NULL;
}

int machine_bus_lib_directory(unsigned int unit, const char *pattern,
                              BYTE **buf)
{
    return -1;
}

int machine_bus_lib_read_sector(unsigned int unit, unsigned int track,
                                unsigned int sector, BYTE *buf)
{
    return -1;
}

int machine_bus_lib_write_sector(unsigned int unit, unsigned int track,
                                 unsigned int sector, BYTE *buf)
{
    return -1;
}

