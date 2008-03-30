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
#include "autostart.h"

static void vsync_hook(void);

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

/* Struct to access the kernal buffer.  */
kernal_kbd_buf_t kernal_kbd_buf = {

    /* First location of the buffer.  */
    631,

    /* Location that stores the number of characters pending in the
       buffer.  */
    198,

    /* Maximum number of characters that fit in the buffer.  */
    10

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
	"Serial ready",
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

/* C64-specific initialization.  */
int machine_init(void)
{
    if (mem_load() < 0)
	return -1;

    if (app_resources.kernalRev)
	patch_rom(app_resources.kernalRev);

    printf("\nInitializing Serial Bus...\n");

    /* Setup trap handling.  */
    initialize_traps();

    /* Initialize serial traps.  If user does not want them, or if the
       ``true1541'' emulation is used, do not install them. */
    initialize_serial(c64_serial_traps);
    if (app_resources.noTraps || app_resources.true1541)
        remove_serial_traps();

#if 0
    /* This is disabled because currently broken. */
    initialize_printer(4, app_resources.PrinterLang, app_resources.Locale);
#endif

    /* Initialize drives.  Only drive #8 allows true 1541 emulation.  */
    initialize_1541(8, DT_DISK | DT_1541,
                    true1541_attach_floppy, true1541_detach_floppy);
    initialize_1541(9, DT_DISK | DT_1541, NULL, NULL);
    initialize_1541(10, DT_DISK | DT_1541, NULL, NULL);

    /* Initialize FS-based emulation for drive #11.  */
    initialize_1541(11, DT_FS | DT_1541, NULL, NULL);

    if (!app_resources.noTraps)
	initialize_tape(c64_tape_traps);

    /* Fire up the hardware-level 1541 emulation.  */
    initialize_true1541();

    /* Initialize autostart.  FIXME: This should be common to all the
       machines someday.  */
    {
        FILE *autostartfd;
        char *autostartprg;
        char *autostartfile;
        char *tmp;

        autostart_init();
        /* Check for image:prg -format.  */
        if (app_resources.autostartName != NULL) {
            tmp = strrchr(app_resources.autostartName, ':');
            if (tmp) {
                autostartfile = stralloc(app_resources.autostartName);
                autostartprg = strrchr(autostartfile, ':');
                *autostartprg++ = '\0';
                autostartfd = fopen(autostartfile, "r");
                /* image exists? */
                if (autostartfd) {
                    fclose(autostartfd);
                    autostart_autodetect(autostartfile, autostartprg);
                }
                else
                    autostart_autodetect(app_resources.autostartName, NULL);
                free(autostartfile);
            } else {
                autostart_autodetect(app_resources.autostartName, NULL);
            }
        }
    }

    /* Initialize the VIC-II emulation.  */
    vic_ii_init();

    /* Initialize the keyboard.  */
    if (kbd_init("vice.vkm") < 0)
        return -1;

    /* Initialize the monitor.  */
    monitor_init(&maincpu_monitor_interface, &true1541_monitor_interface);

    /* Initialize vsync and register our hook function.  */
    vsync_init(RFSH_PER_SEC, CYCLES_PER_SEC, vsync_hook);

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

    if (app_resources.reu)
	reset_reu(NULL, 0);

    /* FIXME: This should be common to all the systems someday.  */
    autostart_reset();

    true1541_reset();
}

void machine_shutdown(void)
{
    /* Detach REU.  */
    if (app_resources.reu)
	close_reu(app_resources.reuName);

    /* Detach all devices.  */
    remove_serial(-1);
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
    if (app_resources.true1541
	&& app_resources.true1541IdleMethod == TRUE1541_IDLE_TRAP_IDLE) {
	true1541_cpu_execute();
    }

    true1541_update_ui_status();

    /* FIXME: This will be common to all the machines someday.  */
    autostart_advance();

    if (maincpu_prevent_clk_overflow()) {
	vic_ii_prevent_clk_overflow();
	cia1_prevent_clk_overflow();
	cia2_prevent_clk_overflow();
        sid_prevent_clk_overflow();
        vsync_prevent_clk_overflow();
    }
    true1541_prevent_clk_overflow();
}
