/*
 * c128.c
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
 *
 * Based on the original work in VICE 0.11.0 by
 *  Jouko Valta (jopi@stekt.oulu.fi)
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

#include "c128.h"
#include "machine.h"
#include "c64ui.h"
#include "interrupt.h"
#include "vicii.h"
#include "c64cia.h"
#include "vmachine.h"
#include "maincpu.h"
#include "kbdbuf.h"
#include "sid.h"
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
#include "c128mem.h"
#include "attach.h"
#include "autostart.h"

static void vsync_hook(void);

/* ------------------------------------------------------------------------- */

const char machine_name[] = "C128";

static trap_t c128_serial_traps[] = {
    {
	"SerialListen",
	0xE355,
	0xE5BA,
	{0x20, 0x73, 0xE5},
	serialattention
    },
    {
	"SerialSaListen",
	0xE37C,
	0xE5BA,
	{0x20, 0x73, 0xE5},
	serialattention
    },
    {
	"SerialSendByte",
	0xE38C,
	0xE5BA,
	{0x20, 0x73, 0xE5},
	serialsendbyte
    },
    {
	"SerialReceiveByte",
	0xE43E,
	0xE5BA,
	{0x20, 0x73, 0xE5},
	serialreceivebyte
    },
    {
	"Serial ready",
	0xE569,
	0xE572,
	{0xAD, 0x00, 0xDD},
	trap_serial_ready
    },
    {
	"Serial ready",
	0xE4F5,
	0xE572,
	{0xAD, 0x00, 0xDD},
	trap_serial_ready
    },

    { NULL }
};

/* ------------------------------------------------------------------------ */

/* C128-specific resource initialization.  This is called before initializing
   the machine itself with `machine_init()'.  */
int machine_init_resources(void)
{
    if (traps_init_resources() < 0
        || vsync_init_resources() < 0
        || video_init_resources() < 0
        || c128_mem_init_resources() < 0
        || vic_ii_init_resources() < 0
        || sound_init_resources() < 0
        || sid_init_resources() < 0
        || true1541_init_resources() < 0)
        return -1;

    return 0;
}

/* C128-specific command-line option initialization.  */
int machine_init_cmdline_options(void)
{
    if (traps_init_cmdline_options() < 0
        || vsync_init_cmdline_options() < 0
        || video_init_cmdline_options() < 0
        || c128_mem_init_cmdline_options() < 0
        || vic_ii_init_cmdline_options() < 0
        || sound_init_cmdline_options() < 0
        || sid_init_cmdline_options() < 0
        || true1541_init_cmdline_options() < 0)
        return -1;

    return 0;
}

/* C128-specific initialization.  */
int machine_init(void)
{
    if (mem_load() < 0)
	return -1;

    printf("\nInitializing Serial Bus...\n");

    /* Setup trap handling.  */
    traps_init();

    /* Initialize serial traps.  If user does not want them, or if the
       ``true1541'' emulation is used, do not install them.  */
    serial_init(c128_serial_traps);

    /* Initialize drives, and attach true 1541 emulation hooks to
       drive 8 (which is the only true 1541-capable device).  */
    file_system_set_hooks(8, true1541_attach_floppy, true1541_detach_floppy);
    file_system_init();

    /* Initialize the tape emulation.  */
    /* tape_init(c64_tape_traps); */

    /* Fire up the hardware-level 1541 emulation.  */
    true1541_init(C128_PAL_CYCLES_PER_SEC, C128_NTSC_CYCLES_PER_SEC);

    /* Initialize autostart.  */
    autostart_init(3 * C128_PAL_RFSH_PER_SEC * C128_PAL_CYCLES_PER_RFSH);

    /* Initialize the VIC-II emulation.  */
    if (vic_ii_init() == NULL)
        return -1;

    /* Initialize the keyboard.  */
#ifndef __MSDOS__
    if (kbd_init("vice.vkm") < 0)
        return -1;
#else
    if (c128_kbd_init() < 0)
        return -1;
#endif

    /* Initialize the monitor.  */
    monitor_init(&maincpu_monitor_interface, &true1541_monitor_interface);

    /* Initialize vsync and register our hook function.  */
    vsync_init(C128_PAL_RFSH_PER_SEC, C128_PAL_CYCLES_PER_SEC, vsync_hook);

    /* Initialize sound.  Notice that this does not really open the audio
       device yet.  */
    sound_init(C128_PAL_CYCLES_PER_SEC, C128_PAL_CYCLES_PER_RFSH);

    /* Initialize keyboard buffer.  */
    kbd_buf_init(631, 198, 10, C128_PAL_CYCLES_PER_RFSH * C128_PAL_RFSH_PER_SEC);

    /* Initialize the C128-specific part of the UI.  */
    c128_ui_init();
    return 0;
}

/* C128-specific reset sequence.  */
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

    initialize_memory();

    reset_cia1();
    reset_cia2();
    reset_vic_ii();
    sid_reset();
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
       `C128_PAL_CYCLES_PER_RFSH' here, or the VIC-II emulation could go
       nuts.  */
    sub = maincpu_prevent_clk_overflow(C128_PAL_CYCLES_PER_RFSH);
    if (sub > 0) {
	vic_ii_prevent_clk_overflow(sub);
	cia1_prevent_clk_overflow(sub);
	cia2_prevent_clk_overflow(sub);
        sid_prevent_clk_overflow(sub);
        vsync_prevent_clk_overflow(sub);
    }

    /* The 1541 has to deal both with our overflowing and its own one, so it
       is called even when there is no overflowing in the main CPU.  */
    true1541_prevent_clk_overflow(sub);

#ifdef HAS_JOYSTICK
    joystick();
#endif
}
