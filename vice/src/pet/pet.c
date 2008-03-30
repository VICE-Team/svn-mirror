/*
 * pet.c
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  André Fachat <fachat@physik.tu-chemnitz.de>
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

#include <math.h>   /* modf */
#include <stdio.h>
#include <stdlib.h>

#include "attach.h"
#include "autostart.h"
#include "clkguard.h"
#include "cmdline.h"
#include "crtc-mem.h"
#include "crtc.h"
#include "datasette.h"
#include "debug.h"
#include "drive-cmdline-options.h"
#include "drive-resources.h"
#include "drive-snapshot.h"
#include "drive.h"
#include "drivecpu.h"
#include "iecdrive.h"
#include "interrupt.h"
#include "ioutil.h"
#include "kbdbuf.h"
#include "keyboard.h"
#include "log.h"
#include "machine-drive.h"
#include "machine.h"
#include "maincpu.h"
#include "mem.h"
#include "monitor.h"
#include "pet-cmdline-options.h"
#include "pet-resources.h"
#include "pet-snapshot.h"
#include "pet.h"
#include "petiec.h"
#include "petmem.h"
#include "pets.h"
#include "petsound.h"
#include "petui.h"
#include "petvia.h"
#include "petacia.h"
#include "petpia.h"
#include "printer.h"
#include "resources.h"
#include "rs232drv.h"
#include "screenshot.h"
#include "serial.h"
#include "sound.h"
#include "tape.h"
#include "traps.h"
#include "types.h"
#include "utils.h"
#include "via.h"
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

const char machine_name[] = "PET";
int machine_class = VICE_MACHINE_PET;

static void machine_vsync_hook(void);

/*
static long     pet_cycles_per_rfsh     = PET_PAL_CYCLES_PER_RFSH;
static double   pet_rfsh_per_sec        = PET_PAL_RFSH_PER_SEC;
*/

static log_t pet_log = LOG_ERR;
static machine_timing_t machine_timing; 

/* ------------------------------------------------------------------------- */

#if 0

/* PET resources.  */

/* PET model name.  */
static char *model_name = NULL;

static int set_model_name(resource_value_t v, void *param)
{
    const char *name = (const char *)v;

    if (pet_set_model(name, NULL) < 0) {
        log_error(pet_log, "Invalid PET model `%s'.", name);
        return -1;
    }

    util_string_set(&model_name, name);
    return 0;
}

/* ------------------------------------------------------------------------- */

static const resource_t resources[] = {
    { "Model", RES_STRING, (resource_value_t)"8032",
      (void *)&model_name, set_model_name, NULL },
    { NULL }
};

static const cmdline_option_t cmdline_options[] = {
    { "-model", SET_RESOURCE, 1, NULL, NULL, "Model", NULL,
      "<name>", "Specify PET model name" },
    { NULL }
};

#endif

/* ------------------------------------------------------------------------ */

/* PET-specific resource initialization.  This is called before initializing
   the machine itself with `machine_init()'.  */
int machine_resources_init(void)
{
#if 0
    if (resources_register(resources) < 0)
        return -1;
#endif

    if (traps_resources_init() < 0
        || vsync_resources_init() < 0
        || video_resources_init() < 0
        || pet_resources_init() < 0
        || crtc_resources_init() < 0
        || pia1_init_resources() < 0
        || sound_resources_init() < 0
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
    pet_resources_shutdown();
    sound_resources_shutdown();
    rs232drv_resources_shutdown();
    printer_resources_shutdown();
    drive_resources_shutdown();
}

/* PET-specific command-line option initialization.  */
int machine_cmdline_options_init(void)
{
#if 0
    if (cmdline_register_options(cmdline_options) < 0)
        return -1;
#endif

    if (traps_cmdline_options_init() < 0
        || vsync_cmdline_options_init() < 0
        || video_init_cmdline_options() < 0
        || pet_cmdline_options_init() < 0
        || crtc_cmdline_options_init() < 0
        || pia1_init_cmdline_options() < 0
        || sound_cmdline_options_init() < 0
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

#define SIGNAL_VERT_BLANK_OFF   pia1_signal(PIA_SIG_CB1, PIA_SIG_RISE);

#define SIGNAL_VERT_BLANK_ON    pia1_signal(PIA_SIG_CB1, PIA_SIG_FALL);

static void pet_crtc_signal(unsigned int signal) {
    if (signal) {
        SIGNAL_VERT_BLANK_ON
    } else {
        SIGNAL_VERT_BLANK_OFF
    }
}

/* ------------------------------------------------------------------------- */

static void pet_monitor_init(void)
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
    petvia_setup_context(&machine_context);
}

/* PET-specific initialization.  */
int machine_init(void)
{
    pet_log = log_open("PET");

    pet_init_ok = 1;    /* used in pet_set_model() */

    /* Setup trap handling - must be before mem_load() */
    traps_init();

    if (mem_load() < 0)
        return -1;

    log_message(pet_log, "Initializing IEEE488 bus...");

    /* No traps installed on the PET.  */
    if (serial_init(NULL, 0xa4) < 0)
        return -1;

    /* Initialize drives. */
    file_system_init();

    rs232drv_init();

    /* initialize print devices */
    printer_init();

    /* Initialize autostart.  FIXME: We could probably use smaller values.  */
    /* moved to mem_load() as it is kernal-dependant AF 30jun1998
    autostart_init(1 * PET_PAL_RFSH_PER_SEC * PET_PAL_CYCLES_PER_RFSH, 0);
    */

    /* Initialize the CRTC emulation.  */
    if (crtc_init() == NULL)
        return -1;
    crtc_set_retrace_type(petres.crtc);
    crtc_set_retrace_callback(pet_crtc_signal);
    pet_crtc_set_screen();

    via_init(&(machine_context.via));
    pia1_init();
    pia2_init();
    acia1_init();

#ifndef COMMON_KBD
    /* Initialize the keyboard.  */
    if (pet_kbd_init() < 0)
        return -1;
#endif

    /* Initialize the datasette emulation.  */
    datasette_init();

    /* Fire up the hardware-level 1541 emulation.  */
    drive_init();

    pet_monitor_init();

    /* Initialize vsync and register our hook function.  */
    vsync_init(machine_vsync_hook);
    vsync_set_machine_parameter(machine_timing.rfsh_per_sec,
                                PET_PAL_CYCLES_PER_SEC);

    /* Initialize sound.  Notice that this does not really open the audio
       device yet.  */
    sound_init(PET_PAL_CYCLES_PER_SEC, machine_timing.cycles_per_rfsh);

    /* Initialize keyboard buffer.  FIXME: Is this correct?  */
    /* moved to mem_load() because it's model specific... AF 30jun1998
    kbd_buf_init(631, 198, 10, PET_PAL_CYCLES_PER_RFSH * PET_PAL_RFSH_PER_SEC);
    */

    /* Initialize the PET-specific part of the UI.  */
    pet_ui_init();

    petiec_init();

    machine_drive_stub();

    return 0;
}

/* PET-specific initialization.  */
void machine_specific_reset(void)
{
    serial_reset();

    pia1_reset();
    pia2_reset();
    viacore_reset(&(machine_context.via));
    acia1_reset();
    crtc_reset();
    petsnd_reset();
    petmem_reset();
    rs232drv_reset();
    printer_reset();
    drive_reset();
    datasette_reset();
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

    /* close the video chip(s) */
    crtc_shutdown();
}

/* ------------------------------------------------------------------------- */

/* This hook is called at the end of every frame.  */
static void machine_vsync_hook(void)
{
    CLOCK sub;

    autostart_advance();

    drive_vsync_hook();

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
    return PET_PAL_CYCLES_PER_SEC;
}

void machine_get_line_cycle(unsigned int *line, unsigned int *cycle)
{
    *line = (unsigned int)((maincpu_clk) / machine_timing.cycles_per_line
            % machine_timing.screen_lines);

    *cycle = (unsigned int)((maincpu_clk) % machine_timing.cycles_per_line);
}

void machine_change_timing(int timeval)
{
    machine_timing.cycles_per_sec = PET_PAL_CYCLES_PER_SEC;
    machine_timing.cycles_per_rfsh = PET_PAL_CYCLES_PER_RFSH;
    machine_timing.rfsh_per_sec = PET_PAL_RFSH_PER_SEC;
    machine_timing.cycles_per_line = PET_PAL_CYCLES_PER_LINE;
    machine_timing.screen_lines = PET_PAL_SCREEN_LINES;

    debug_set_machine_parameter(PET_PAL_CYCLES_PER_LINE, PET_PAL_SCREEN_LINES);
    drive_set_machine_parameter(machine_timing.cycles_per_sec);
}

/* Set the screen refresh rate, as this is variable in the CRTC */
void machine_set_cycles_per_frame(long cpf)
{
    double i, f;

    machine_timing.cycles_per_rfsh = cpf;
    machine_timing.rfsh_per_sec = ((double)PET_PAL_CYCLES_PER_SEC)
                                  / ((double)cpf);

    f = modf(machine_timing.rfsh_per_sec, &i) * 1000;

    log_message(pet_log, "cycles per frame set to %ld, refresh to %d.%03dHz",
                cpf, (int)i, (int)f);

    vsync_set_machine_parameter(machine_timing.rfsh_per_sec,
                                PET_PAL_CYCLES_PER_SEC);

    /* sound_set_cycles_per_rfsh(machine_timing.cycles_per_rfsh); */
}

/* ------------------------------------------------------------------------- */

int machine_write_snapshot(const char *name, int save_roms, int save_disks,
                           int event_mode)
{
    return pet_snapshot_write(name, save_roms, save_disks, event_mode);
}

int machine_read_snapshot(const char *name, int event_mode)
{
    return pet_snapshot_read(name, event_mode);
}


/* ------------------------------------------------------------------------- */

int machine_autodetect_psid(const char *name)
{
    return -1;
}

void machine_play_psid(int tune)
{
}


/* ------------------------------------------------------------------------- */

void pet_crtc_set_screen(void)
{
    int cols, vmask;

    cols = petres.video;
    vmask = petres.vmask;

    /* mem_initialize_memory(); */

    if (!cols) {
        cols = petres.rom_video;
        vmask = (cols == 40) ? 0x3ff : 0x7ff;
    }
    if (!cols) {
        cols = PET_COLS;
        vmask = (cols == 40) ? 0x3ff : 0x7ff;
    }

    /* when switching 8296 to 40 columns, CRTC ends up at $9000 otherwise...*/
    if (cols == 40)
        vmask = 0x3ff;
/*
    log_message(pet_mem_log, "set_screen(vmask=%04x, cols=%d, crtc=%d)",
                vmask, cols, petres.crtc);
*/
/*
    crtc_set_screen_mode(mem_ram + 0x8000, vmask, cols, (cols==80) ? 2 : 0);
*/
    crtc_set_screen_options(cols, 25 * 10);
    crtc_set_screen_addr(mem_ram + 0x8000);
    crtc_set_hw_options((cols == 80) ? 2 : 0, vmask, 0x800, 512, 0x1000);
    crtc_set_retrace_type(petres.crtc ? 1 : 0);

    /* No CRTC -> assume 40 columns */
    if (!petres.crtc) {
        crtc_store(0, 13);
        crtc_store(1, 0);
        crtc_store(0, 12);
        crtc_store(1, 0x10);
        crtc_store(0, 9);
        crtc_store(1, 7);
        crtc_store(0, 8);
        crtc_store(1, 0);
        crtc_store(0, 7);
        crtc_store(1, 29);
        crtc_store(0, 6);
        crtc_store(1, 25);
        crtc_store(0, 5);
        crtc_store(1, 16);
        crtc_store(0, 4);
        crtc_store(1, 32);
        crtc_store(0, 3);
        crtc_store(1, 8);
        crtc_store(0, 2);
        crtc_store(1, 50);
        crtc_store(0, 1);
        crtc_store(1, 40);
        crtc_store(0, 0);
        crtc_store(1, 63);
    }
}

struct video_canvas_s *machine_canvas_get(unsigned int window)
{
    if (window == 0)
        return crtc_get_canvas();

    return NULL;
}

int machine_screenshot(screenshot_t *screenshot, struct video_canvas_s *canvas)
{
    if (canvas != crtc_get_canvas())
        return -1;

    crtc_screenshot(screenshot);
    return 0;
}

int machine_canvas_async_refresh(struct canvas_refresh_s *refresh,
                                 struct video_canvas_s *canvas)
{
    if (canvas != crtc_get_canvas())
        return -1;

    crtc_async_refresh(refresh);
    return 0;
}

unsigned int machine_num_keyboard_mappings(void)
{
    return NUM_KEYBOARD_MAPPINGS;
}

