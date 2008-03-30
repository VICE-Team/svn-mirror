/*
 * vic20.c
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
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

#include "vic20.h"
#include "vic20ui.h"
#include "interrupt.h"
#include "vic.h"
#include "via.h"
#include "vmachine.h"
#include "machspec.h"
#include "maincpu.h"
#include "kbdbuf.h"
#include "true1541.h"
#include "1541cpu.h"
#include "traps.h"
#include "kbd.h"
#include "vsync.h"
#include "sid.h"
#include "vic20mem.h"
#include "attach.h"
#include "resources.h"
#include "cmdline.h"
#include "autostart.h"

static void vsync_hook(void);

/* Machine description.  */
machdesc_t machdesc = {
    /* Machine name.  */
    "VIC20",

    /* Flag: does this machine have joysticks?  */
    1,

    /* Flag: how many colors does this machine have?  */
    8,

    /* Flag: does this machine allow 1541 emulation?  */
    1,

    /* Flag: does this machine have a tape recorder?  */
    0,

    /* Flag: does this machine have a IEEE488 emulation?  */
    0,

    /* Flag: does this machine have sound capabilities?  */
    1,

    /* Flag: does this machine have a RAM Expansion unit?  */
    0,

    /* Flag: does this machine have hardware sprites?  */
    0

};

/* VIC20 Traps */
static trap_t vic20_serial_traps[] = {
    {
	"SerialListen",
	0xEE2E,
        0xEEB2,
	{0x20, 0xA0, 0xE4},
	serialattention
    },
    {
	"SerialSaListen",
	0xEE40,
        0xEEB2,
	{0x20, 0x8D, 0xEF},
	serialattention
    },
    {
	"SerialSendByte",
	0xEE49,
        0xEEB2,
	{0x78, 0x20, 0xA0},
	serialsendbyte
    },
    {
	"SerialReceiveByte",
	0xEF19,
        0xEEB2,
	{0x78, 0xA9, 0x00},
	serialreceivebyte
    },
    {
	"SerialReady",
	0xE4B2,
        0xEEB2,
	{0xAD, 0x1F, 0x91},
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

/* ------------------------------------------------------------------------ */

/* VIC20-specific resource initialization.  This is called before
   initializing the machine itself with `machine_init()'.  */
int machine_init_resources(void)
{
    if (traps_init_resources()
        || vsync_init_resources() < 0
        || video_init_resources() < 0
        || vic20_mem_init_resources() < 0
        || vic_init_resources() < 0
        || sid_init_resources() < 0
        || true1541_init_resources() < 0)
        return -1;

    return 0;
}

/* VIC20-specific command-line option initialization.  */
int machine_init_cmdline_options(void)
{
    if (traps_init_cmdline_options()
        || vsync_init_cmdline_options() < 0
        || video_init_cmdline_options() < 0
        || vic20_mem_init_cmdline_options() < 0
        || vic_init_cmdline_options() < 0
        || sid_init_cmdline_options() < 0
        || true1541_init_cmdline_options() < 0)
        return -1;

    return 0;
}

/* VIC20-specific initialization.  */
int machine_init(void)
{
    if (mem_load() < 0)
        return -1;

    printf("\nInitializing Serial Bus...\n");

    /* Setup trap handling.  */
    traps_init();

    /* Initialize serial traps.  If user does not want them, or if the
       ``true1541'' emulation is used, do not install them.  */
    serial_init(vic20_serial_traps);

    /* Initialize drives.  Only drive #8 allows true 1541 emulation.  */
    initialize_drives();

    /* Fire up the hardware-level 1541 emulation. */
    true1541_init(VIC20_PAL_CYCLES_PER_SEC, VIC20_NTSC_CYCLES_PER_SEC);

    /* Initialize autostart.  */
    autostart_init(3 * VIC20_PAL_RFSH_PER_SEC * VIC20_PAL_CYCLES_PER_RFSH);

    /* Initialize the VIC-I emulation.  */
    vic_init();

    /* Load the default keymap file.  */
    if (kbd_init("vice.vkm") < 0)
        return -1;

    /* Initialize the monitor.  */
    monitor_init(&maincpu_monitor_interface, &true1541_monitor_interface);

    /* Initialize vsync and register our hook function.  */
    vsync_init(VIC20_PAL_RFSH_PER_SEC, VIC20_PAL_CYCLES_PER_SEC, vsync_hook);

    /* Initialize sound.  Notice that this does not really open the audio
       device yet.  */
    sound_init(VIC20_PAL_CYCLES_PER_SEC, VIC20_PAL_CYCLES_PER_RFSH);

    /* Initialize keyboard buffer.  */
    kbd_buf_init(631, 198, 10,
                 VIC20_PAL_CYCLES_PER_RFSH * VIC20_PAL_RFSH_PER_SEC);

    /* Initialize the VIC20-specific part of the UI.  */
    vic20_ui_init();

    return 0;
}

/* Reset.  */
void machine_reset(void)
{
    maincpu_int_status.alarm_handler[A_RASTERDRAW] = int_rasterdraw;
    maincpu_int_status.alarm_handler[A_VIA1T1] = int_via1t1;
    maincpu_int_status.alarm_handler[A_VIA1T2] = int_via1t2;
    maincpu_int_status.alarm_handler[A_VIA2T1] = int_via2t1;
    maincpu_int_status.alarm_handler[A_VIA2T2] = int_via2t2;

    maincpu_set_alarm_clk(A_RASTERDRAW, VIC20_PAL_CYCLES_PER_LINE);

    reset_via1();
    reset_via2();

    true1541_reset();
}

void machine_shutdown(void)
{
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
       `VIC20_PAL_CYCLES_PER_RFSH' here, or the VIC emulation could go
       nuts.  */
    sub = maincpu_prevent_clk_overflow(VIC20_PAL_CYCLES_PER_RFSH);
    if (sub > 0) {
	vic_prevent_clk_overflow(sub);
	via1_prevent_clk_overflow(sub);
	via2_prevent_clk_overflow(sub);
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
