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

#include <stdio.h>
#ifndef __riscos
#include <unistd.h>
#endif

#include "attach.h"
#include "autostart.h"
#include "cartridge.h"
#include "clkguard.h"
#include "cmdline.h"
#include "drive.h"
#include "interrupt.h"
#include "kbd.h"
#include "kbdbuf.h"
#include "log.h"
#include "machine.h"
#include "maincpu.h"
#include "resources.h"
#include "tape.h"
#include "traps.h"
#include "vic.h"
#include "vic20.h"
#include "vic20mem.h"
#include "vic20sound.h"
#include "vic20ui.h"
#include "vic20via.h"
#include "vmachine.h"
#include "vsync.h"
#include "ciatimer.h"

#ifdef HAVE_PRINTER
#include "print.h"
#include "prdevice.h"
#include "pruser.h"
#endif

#ifdef HAVE_RS232
#include "rs232.h"
#include "rsuser.h"
#include "vic20rsuser.h"
#endif

static void vsync_hook(void);

const char machine_name[] = "VIC20";

int machine_class = VICE_MACHINE_VIC20;

/* ------------------------------------------------------------------------- */

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

/* Tape traps.  */
static trap_t vic20_tape_traps[] = {
    {
        "TapeFindHeader",
        0xF7B2,
        0xF7B5,
        {0x20, 0xC0, 0xF8},
        tape_find_header_trap
    },
    {
        "TapeReceive",
        0xF90B,
        0xFCCF,
        {0x20, 0xFB, 0xFC},
        tape_receive_trap
    },
    {
        NULL,
        0,
        0,
        {0, 0, 0},
        NULL
    }
};

static log_t vic20_log = LOG_ERR;

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
        || sound_init_resources() < 0
#ifdef HAVE_RS232
        || rs232_init_resources() < 0
        || rsuser_init_resources() < 0
#endif
#ifdef HAVE_PRINTER
        || print_init_resources() < 0
        || prdevice_init_resources() < 0
        || pruser_init_resources() < 0
#endif
        || kbd_init_resources() < 0
        || drive_init_resources() < 0
	|| cartridge_init_resources() <0)
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
        || sound_init_cmdline_options() < 0
#ifdef HAVE_RS232
        || rs232_init_cmdline_options() < 0
        || rsuser_init_cmdline_options() < 0
#endif
#ifdef HAVE_PRINTER
        || print_init_cmdline_options() < 0
        || prdevice_init_cmdline_options() < 0
        || pruser_init_cmdline_options() < 0
#endif
        || kbd_init_cmdline_options() < 0
        || drive_init_cmdline_options() < 0
	|| cartridge_init_cmdline_options() < 0)
        return -1;

    return 0;
}

/* VIC20-specific initialization.  */
int machine_init(void)
{
    if (vic20_log == LOG_ERR)
        vic20_log = log_open("VIC20");

    maincpu_init();

    if (mem_load() < 0)
        return -1;

    /* Setup trap handling.  */
    traps_init();

    /* Initialize serial traps.  If user does not want them, or if the
       ``drive'' emulation is used, do not install them.  */
    serial_init(vic20_serial_traps);

    /* Initialize drives, and attach true 1541 emulation hooks to
       drive 8 (which is the only true 1541-capable device).  */
    file_system_set_hooks(8, drive_attach_floppy, drive_detach_floppy);
    file_system_set_hooks(9, drive_attach_floppy, drive_detach_floppy);
    file_system_init();

#ifdef HAVE_RS232
    /* Initialize RS232 handler.  */
    rs232_init();
    vic20_rsuser_init();
#endif

#ifdef HAVE_PRINTER
    /* initialize print devices */
    print_init();
#endif

    /* Initialize the tape emulation.  */
    tape_init(0xb2, 0x90, 0x93, 0x29f, 0, 0xc1, 0xae, 0x277, 0xc6,
              vic20_tape_traps);

    /* Fire up the hardware-level 1541 emulation. */
    drive_init(VIC20_PAL_CYCLES_PER_SEC, VIC20_NTSC_CYCLES_PER_SEC);

    /* Initialize autostart.  */
    autostart_init(3 * VIC20_PAL_RFSH_PER_SEC * VIC20_PAL_CYCLES_PER_RFSH, 1,
                   0xcc, 0xd1, 0xd3, 0xd5);

    /* Initialize the VIC-I emulation.  */
    if (vic_init() == NULL)
        return -1;
 
    /* needed for VC1571/1581 emulation */
    ciat_init_table();

    via1_init();
    via2_init();
 
    /* Load the default keymap file.  */
    if (vic20_kbd_init() < 0)
        return -1;

    /* Initialize the monitor.  */
    monitor_init(&maincpu_monitor_interface, &drive0_monitor_interface,
                 &drive1_monitor_interface);

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
    reset_via1();
    reset_via2();

    reset_vic();

    drive_reset();

    vic_sound_reset();

#ifdef HAVE_RS232
    rs232_reset();
    rsuser_reset();
#endif

#ifdef HAVE_PRINTER
    print_reset();
#endif
}

void machine_powerup(void)
{
    mem_powerup();
    maincpu_trigger_reset();
}

void machine_shutdown(void)
{
    /* Detach all devices.  */
    serial_remove_file(-1);
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

    drive_vsync_hook();

    autostart_advance();

    sub = clk_guard_prevent_overflow(&maincpu_clk_guard);

#if 0
    /* We have to make sure the number of cycles subtracted is multiple of
       `VIC20_PAL_CYCLES_PER_RFSH' here, or the VIC emulation could go
       nuts.  */
    sub = maincpu_prevent_clk_overflow(VIC20_PAL_CYCLES_PER_RFSH);
    if (sub > 0) {
	vic_prevent_clk_overflow(sub);
#ifdef HAVE_RS232
        rsuser_prevent_clk_overflow(sub);
#endif
	via1_prevent_clk_overflow(sub);
	via2_prevent_clk_overflow(sub);
	sound_prevent_clk_overflow(sub);
        vsync_prevent_clk_overflow(sub);
    }
#endif

    /* The 1541 has to deal both with our overflowing and its own one, so it
       is called even when there is no overflowing in the main CPU.  */
    /* FIXME: Do we have to check drive_enabled here?  */
    drive_prevent_clk_overflow(sub, 0);
    drive_prevent_clk_overflow(sub, 1);
}

int machine_set_restore_key(int v)
{
    via2_signal(VIA_SIG_CA1, v? VIA_SIG_FALL: VIA_SIG_RISE);
    return 1;
}


/* ------------------------------------------------------------------------- */

long machine_get_cycles_per_second(void)
{
    return VIC20_PAL_CYCLES_PER_SEC;
}

/* ------------------------------------------------------------------------- */

/* #define SNAP_MACHINE_NAME   "VIC20" */
#define SNAP_MAJOR          0
#define SNAP_MINOR          0

int machine_write_snapshot(const char *name, int save_roms, int save_disks)
{
    snapshot_t *s;

    s = snapshot_create(name, SNAP_MAJOR, SNAP_MINOR, machine_name);
    if (s == NULL) {
        perror(name);
        return -1;
    }

    /* FIXME: Missing sound.  */
    if (maincpu_write_snapshot_module(s) < 0
        || mem_write_snapshot_module(s, save_roms) < 0
        || vic_write_snapshot_module(s) < 0
        || via1_write_snapshot_module(s) < 0
        || via2_write_snapshot_module(s) < 0
        || drive_write_snapshot_module(s, save_disks, save_roms) < 0) {
        snapshot_close(s);
        unlink(name);
        return -1;
    }

    snapshot_close(s);
    return 0;
}

int machine_read_snapshot(const char *name)
{
    snapshot_t *s;
    BYTE minor, major;

    s = snapshot_open(name, &major, &minor, machine_name);
    if (s == NULL)
        return -1;

    if (major != SNAP_MAJOR || minor != SNAP_MINOR) {
        log_error(vic20_log,
                  "Snapshot version (%d.%d) not valid: expecting %d.%d.",
                  major, minor, SNAP_MAJOR, SNAP_MINOR);
        goto fail;
    }

    /* FIXME: Missing sound.  */
    if (maincpu_read_snapshot_module(s) < 0
        || mem_read_snapshot_module(s) < 0
        || vic_read_snapshot_module(s) < 0
        || via1_read_snapshot_module(s) < 0
        || via2_read_snapshot_module(s) < 0
        || drive_read_snapshot_module(s) < 0)
        goto fail;

    snapshot_close(s);
    return 0;

fail:
    if (s != NULL)
        snapshot_close(s);
    maincpu_trigger_reset();
    return -1;
}
