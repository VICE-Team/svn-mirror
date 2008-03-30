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

#include "1541cpu.h"
#include "attach.h"
#include "autostart.h"
#include "c128mem.h"
#include "c128ui.h"
#include "c64cia.h"
#include "interrupt.h"
#include "kbd.h"
#include "kbdbuf.h"
#include "machine.h"
#include "maincpu.h"
#include "mon.h"
#include "patchrom.h"
#include "serial.h"
#include "sid.h"
#include "tapeunit.h"
#include "c64tpi.h"
#include "traps.h"
#include "true1541.h"
#include "utils.h"
#include "vicii.h"
#include "vmachine.h"
#include "vsync.h"

#ifdef HAVE_RS232
#include "rs232.h"
#include "c64acia.h"
#include "rsuser.h"
#endif

#ifdef HAVE_PRINTER
#include "print.h"
#include "prdevice.h"
#include "pruser.h"
#endif

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

/* Tape traps.  */
static trap_t c128_tape_traps[] = {
    {
        "FindHeader",
        0xE8D3,
        0xE8D6,
        {0x20, 0xF2, 0xE9},
        findheader
    },
    {
        "WriteHeader",
        0xE96E,
        0xE971,
        {0x20, 0x1C, 0xEA},
        writeheader
    },
    {
        "TapeReceive",
        0xEA60,
        0xEE57,
        {0x20, 0x9B, 0xEE},
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
#ifdef HAVE_RS232
        || acia1_init_resources() < 0
        || rs232_init_resources() < 0
        || rsuser_init_resources() < 0
#endif
#ifdef HAVE_PRINTER
        || print_init_resources() < 0
        || prdevice_init_resources() < 0
        || pruser_init_resources() < 0
#endif
        || kbd_init_resources() < 0
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
#ifdef HAVE_RS232
        || acia1_init_cmdline_options() < 0
        || rs232_init_cmdline_options() < 0
        || rsuser_init_cmdline_options() < 0
#endif
#ifdef HAVE_PRINTER
        || print_init_cmdline_options() < 0
        || prdevice_init_cmdline_options() < 0
        || pruser_init_cmdline_options() < 0
#endif
        || kbd_init_cmdline_options() < 0
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

#ifdef HAVE_RS232
    /* initialize RS232 handler */
    rs232_init();
    rsuser_init();
#endif

#ifdef HAVE_PRINTER
    /* initialize print devices */
    print_init();
#endif

    /* Initialize the tape emulation.  */
    tape_init(0xb2, 0x90, 0x93, 0xA09, 0, 0xc1, 0xae, c128_tape_traps,
        842, 208);

    /* Fire up the hardware-level 1541 emulation.  */
    true1541_init(C128_PAL_CYCLES_PER_SEC, C128_NTSC_CYCLES_PER_SEC);

    /* Initialize autostart. FIXME: at least 0xa26 is only for 40 cols */
    autostart_init(3 * C128_PAL_RFSH_PER_SEC * C128_PAL_CYCLES_PER_RFSH, 1,
	0xa27, 0xe0, 0xec, 0xee);

    /* Initialize the VIC-II emulation.  */
    if (vic_ii_init() == NULL)
        return -1;

    /* Initialize the keyboard.  */
#ifndef __MSDOS__
    if (kbd_init(/* "default.vkm" */) < 0)
        return -1;
#else
    if (c64_kbd_init() < 0)
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
    kbd_buf_init(842, 208, 10,
                 C128_PAL_CYCLES_PER_RFSH * C128_PAL_RFSH_PER_SEC);

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

#ifdef HAVE_RS232
    maincpu_int_status.alarm_handler[A_ACIA1] = int_acia1;
    maincpu_int_status.alarm_handler[A_RSUSER] = int_rsuser;
#endif

    initialize_memory();

    reset_cia1();
    reset_cia2();
    reset_vic_ii();
    sid_reset();
    reset_tpi();

#ifdef HAVE_RS232
    reset_acia1();

    rs232_reset();
    rsuser_reset();
#endif

#ifdef HAVE_PRINTER
    print_reset();
#endif

    autostart_reset();

    true1541_reset();
}

void machine_shutdown(void)
{
    /* Detach all devices.  */
    serial_remove(-1);
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
	sound_prevent_clk_overflow(sub);
        vsync_prevent_clk_overflow(sub);
    }

    /* The 1541 has to deal both with our overflowing and its own one, so it
       is called even when there is no overflowing in the main CPU.  */
    true1541_prevent_clk_overflow(sub);

#ifdef HAS_JOYSTICK
    joystick();
#endif
}

int machine_set_restore_key(int v)
{
    maincpu_set_nmi(I_RESTORE, v?1:0);
    return 1;
}

