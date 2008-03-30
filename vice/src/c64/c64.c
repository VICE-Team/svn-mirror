/*
 * c64.c
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
 *  Teemu Rantanen (tvr@cs.hut.fi)
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

#include "c64.h"
#include "c64ui.h"
#include "interrupt.h"
#include "vicii.h"
#include "cia.h"
#include "vmachine.h"
#include "machspec.h"
#include "maincpu.h"
#include "kbdbuf.h"
#include "sid.h"
#include "reu.h"
#include "tpi.h"
#include "true1541.h"
#include "1541cpu.h"
#include "traps.h"
#include "tapeunit.h"
#include "patchrom.h"
#include "utils.h"
#include "serial.h"
#include "mon.h"
#include "kbd.h"
#include "vsync.h"
#include "c64mem.h"
#include "attach.h"
#include "autostart.h"

static void vsync_hook(void);

/* ------------------------------------------------------------------------- */

/* Machine description.  */
machdesc_t machdesc = {
    /* Machine name.  */
    "C64",

    /* Flag: does this machine have joysticks?  */
    1,

    /* Flag: how many colors does this machine have?  */
    16,

    /* Flag: does this machine allow 1541 emulation?  */
    1,

    /* Flag: does this machine have a tape recorder?  */
    1,

    /* Flag: does this machine have a IEEE488 emulation?  */
    1,

    /* Flag: does this machine have sound capabilities?  */
    1,

    /* Flag: does this machine have a RAM Expansion unit?  */
    1,

    /* Flag: does this machine have hardware sprites?  */
    1

};

/* Serial traps.  */
static trap_t c64_serial_traps[] = {
    {
	"SerialListen",
	0xED24,
        0xEDAB,
	{0x20, 0x97, 0xEE},
	serialattention
    },
    {
	"SerialSaListen",
	0xED36,
        0xEDAB,
	{0x78, 0x20, 0x8E},
	serialattention
    },
    {
	"SerialSendByte",
	0xED40,
        0xEDAB,
	{0x78, 0x20, 0x97},
	serialsendbyte
    },
    {
	"SerialReceiveByte",
	0xEE13,
        0xEDAB,
	{0x78, 0xA9, 0x00},
	serialreceivebyte
    },
    {
	"SerialReady",
	0xEEA9,
        0xEDAB,
	{0xAD, 0x00, 0xDD},
	trap_serial_ready
    },

    {
        NULL,
        0,
        0,
        {0, 0, 0},
        NULL
    }
};

/* Tape traps.  */
static trap_t c64_tape_traps[] = {
    {
	"FindHeader",
	0xF72F,
        0xF732,
	{0x20, 0x41, 0xF8},
	findheader
    },
    {
	"WriteHeader",
	0xF7BE,
        0xF7C1,
	{0x20, 0x6B, 0xF8},
	writeheader
    },
    {
	"TapeReceive",
	0xF8A1,
        0xFC93,
	{0x20, 0xBD, 0xFC},
	tapereceive
    },
    {
        NULL,
        0,
        0,
        {0, 0, 0},
        NULL
    }
};

/* ------------------------------------------------------------------------ */

/* C64-specific resource initialization.  This is called before initializing
   the machine itself with `machine_init()'.  */
int machine_init_resources(void)
{
    if (traps_init_resources() < 0
        || vsync_init_resources() < 0
        || video_init_resources() < 0
        || c64_mem_init_resources() < 0
        || vic_ii_init_resources() < 0
        || sound_init_resources() < 0
        || sid_init_resources() < 0
        || true1541_init_resources() < 0)
        return -1;

    return 0;
}

/* C64-specific command-line option initialization.  */
int machine_init_cmdline_options(void)
{
    if (traps_init_cmdline_options() < 0
        || vsync_init_cmdline_options() < 0
        || video_init_cmdline_options() < 0
        || c64_mem_init_cmdline_options() < 0
        || vic_ii_init_cmdline_options() < 0
        || sound_init_cmdline_options() < 0
        || sid_init_cmdline_options() < 0
        || true1541_init_cmdline_options() < 0)
        return -1;

    return 0;
}

/* C64-specific initialization.  */
int machine_init(void)
{
    if (mem_load() < 0)
	return -1;

    printf("\nInitializing Serial Bus...\n");

    /* Setup trap handling.  */
    traps_init();

    /* Initialize serial traps.  If user does not want them, or if the
       ``true1541'' emulation is used, do not install them.  */
    serial_init(c64_serial_traps);

    /* Initialize drives.  Only drive #8 allows true 1541 emulation.  */
    initialize_drives();

    /* Initialize the tape emulation.  */
    tape_init(c64_tape_traps);

    /* Fire up the hardware-level 1541 emulation.  */
    true1541_init(C64_PAL_CYCLES_PER_SEC, C64_NTSC_CYCLES_PER_SEC);

    /* Initialize autostart.  */
    autostart_init(3 * C64_PAL_RFSH_PER_SEC * C64_PAL_CYCLES_PER_RFSH);

    /* Initialize the VIC-II emulation.  */
    if (vic_ii_init() == NULL)
        return -1;

    /* Initialize the keyboard.  */
    if (kbd_init("vice.vkm") < 0)
        return -1;

    /* Initialize the monitor.  */
    monitor_init(&maincpu_monitor_interface, &true1541_monitor_interface);

    /* Initialize vsync and register our hook function.  */
    vsync_init(C64_PAL_RFSH_PER_SEC, C64_PAL_CYCLES_PER_SEC, vsync_hook);

    /* Initialize sound.  Notice that this does not really open the audio
       device yet.  */
    sound_init(C64_PAL_CYCLES_PER_SEC, C64_PAL_CYCLES_PER_RFSH);

    /* Initialize keyboard buffer.  */
    kbd_buf_init(631, 198, 10, C64_PAL_CYCLES_PER_RFSH * C64_PAL_RFSH_PER_SEC);

    /* Initialize the C64-specific part of the UI.  */
    c64_ui_init();
    return 0;
}

/* C64-specific reset sequence.  */
void machine_reset(void)
{
    maincpu_int_status.alarm_handler[A_RASTERDRAW] = int_rasterdraw;
    maincpu_int_status.alarm_handler[A_RASTERFETCH] = int_rasterfetch;
    maincpu_int_status.alarm_handler[A_RASTER] = int_raster;
    maincpu_int_status.alarm_handler[A_CIA1TOD] = int_cia1tod;
    maincpu_int_status.alarm_handler[A_CIA1TA] = int_cia1ta;
    maincpu_int_status.alarm_handler[A_CIA1TB] = int_cia1tb;
    maincpu_int_status.alarm_handler[A_CIA2TOD] = int_cia2tod;
    maincpu_int_status.alarm_handler[A_CIA2TA] = int_cia2ta;
    maincpu_int_status.alarm_handler[A_CIA2TB] = int_cia2tb;

    reset_cia1();
    reset_cia2();
    reset_vic_ii();
    reset_sid();
    reset_tpi();

    /* reset_reu(); */                /* FIXME */

    autostart_reset();

    true1541_reset();
}

void machine_shutdown(void)
{
#if 0                           /* FIXME */
    /* Detach REU.  */
    if (app_resources.reu)
	close_reu(app_resources.reuName);
#endif

    /* Detach all devices.  */
    serial_remove(-1);
}

/* Return nonzero if `addr' is in the trappable address space.  */
int rom_trap_allowed(ADDRESS addr)
{
    return 1; /* FIXME */
}

/* ------------------------------------------------------------------------- */

/* This hook is called at the end of every frame.  */
static void vsync_hook(void)
{
    CLOCK sub;

    true1541_vsync_hook();

    autostart_advance();

    /* We have to make sure the number of cycles subtracted is multiple of
       `C64_PAL_CYCLES_PER_RFSH' here, or the VIC-II emulation could go
       nuts.  */
    sub = maincpu_prevent_clk_overflow(C64_PAL_CYCLES_PER_RFSH);
    if (sub > 0) {
	vic_ii_prevent_clk_overflow(sub);
	cia1_prevent_clk_overflow(sub);
	cia2_prevent_clk_overflow(sub);
        sound_prevent_clk_overflow(sub);
        sid_prevent_clk_overflow(sub);
        vsync_prevent_clk_overflow(sub);
    }

    /* The 1541 has to deal both with our overflowing and its own one, so it
       is called even when there is no overflowing in the main CPU.  */
    true1541_prevent_clk_overflow(sub);

#ifdef HAS_JOYSTICK
    joystick()
#endif
}
