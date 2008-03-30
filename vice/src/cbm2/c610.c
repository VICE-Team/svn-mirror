/*
 * c610.c
 *
 * Written by
 *  André Fachat (fachat@physik.tu-chemnitz.de)
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
#include <unistd.h>

#include "c610.h"

#include "attach.h"
#include "autostart.h"
#include "snapshot.h"
#include "c610acia.h"
#include "c610cia.h"
#include "c610mem.h"
#include "c610tpi.h"
#include "c610ui.h"
#include "cmdline.h"
#include "crtc.h"
#include "interrupt.h"
#include "kbd.h"
#include "kbdbuf.h"
#include "machine.h"
#include "maincpu.h"
#include "resources.h"
#include "sid.h"
#include "sound.h"
#include "traps.h"
#include "utils.h"
#include "via.h"
#include "vmachine.h"
#include "vsync.h"
#include "drive.h"

#ifdef HAVE_PRINTER
#include "print.h"
#include "prdevice.h"
#include "pruser.h"
#endif

#ifdef HAVE_RS232
#include "rs232.h"
#endif

#if defined __MSDOS__ || defined WIN32
#include "c610kbd.h"
#endif

static void vsync_hook(void);

static double 	cbm2_rfsh_per_sec	= C610_PAL_RFSH_PER_SEC;
static long   	cbm2_cycles_per_rfsh 	= C610_PAL_CYCLES_PER_RFSH;

const char machine_name[] = "CBM-II";

/* ------------------------------------------------------------------------- */

/* CBM-II resources.  */

#if 0

/* CBM-II model name.  */
static char *model_name;

static int set_model_name(resource_value_t v)
{
    char *name = (char *)v;

    if (c610_set_model(name, NULL) < 0) {
        fprintf(errfile, "Invalid CBM-II model `%s'.\n", name);
        return -1;
    }

    string_set(&model_name, name);
    return 0;
}

#endif

/* ------------------------------------------------------------------------ */

/* CBM-II-specific resource initialization.  This is called before initializing
   the machine itself with `machine_init()'.  */
int machine_init_resources(void)
{
    if (traps_init_resources() < 0
	|| vsync_init_resources() < 0
        || video_init_resources() < 0
        || c610_mem_init_resources() < 0
        || crtc_init_resources() < 0
        || sound_init_resources() < 0
        || sid_init_resources() < 0
        || drive_init_resources() < 0
        || acia1_init_resources() < 0	/* ACIA is always there */
#ifdef HAVE_RS232
        || rs232_init_resources() < 0
#endif
#ifdef HAVE_PRINTER
        || print_init_resources() < 0
        || prdevice_init_resources() < 0
        || pruser_init_resources() < 0
#endif
#if defined __MSDOS__ || defined WIN32
        || kbd_init_resources() < 0
#else
        || pet_kbd_init_resources() < 0
#endif
	)
        return -1;
    return 0;
}

/* CBM-II-specific command-line option initialization.  */
int machine_init_cmdline_options(void)
{
    if (traps_init_cmdline_options() < 0
	|| vsync_init_cmdline_options() < 0
        || video_init_cmdline_options() < 0
        || c610_mem_init_cmdline_options() < 0
        || crtc_init_cmdline_options() < 0
        || sound_init_cmdline_options() < 0
        || sid_init_cmdline_options() < 0
        || drive_init_cmdline_options() < 0
        || acia1_init_cmdline_options() < 0
#ifdef HAVE_RS232
        || rs232_init_cmdline_options() < 0
#endif
#ifdef HAVE_PRINTER
        || print_init_cmdline_options() < 0
        || prdevice_init_cmdline_options() < 0
        || pruser_init_cmdline_options() < 0
#endif
#if defined __MSDOS__ || defined WIN32
        || kbd_init_cmdline_options() < 0
#else
        || pet_kbd_init_cmdline_options() < 0
#endif
	)
        return -1;

    return 0;
}

/* ------------------------------------------------------------------------- */

/* CBM-II-specific initialization.  */
int machine_init(void)
{
    /* Setup trap handling - must be before mem_load() */
    traps_init();

    if (mem_load() < 0)
        return -1;

    fprintf(logfile, "\nInitializing IEEE488 bus...\n");

    /* No traps installed on the CBM-II.  */
    serial_init(NULL);

    /* Initialize drives, and attach true 1541 emulation hooks to
       drive 8 (which is the only true 1541-capable device).  */
    file_system_set_hooks(8, drive_attach_floppy, drive_detach_floppy);
    file_system_set_hooks(9, drive_attach_floppy, drive_detach_floppy);
    file_system_init();

#ifdef HAVE_PRINTER
    /* initialize print devices */
    print_init();
#endif

    /* Initialize the CRTC emulation.  */
    crtc_init();

    /* Initialize the keyboard.  */
#if defined __MSDOS__ || defined WIN32
    if (c610_kbd_init() < 0)
        return -1;
#else
    if (kbd_init() < 0)
        return -1;
#endif

    /* Fire up the hardware-level 1541 emulation.  */
    drive_init(C610_PAL_CYCLES_PER_SEC, C610_NTSC_CYCLES_PER_SEC);

    /* Initialize the monitor.  */
    monitor_init(&maincpu_monitor_interface, &drive0_monitor_interface,
                 &drive1_monitor_interface);

    /* Initialize vsync and register our hook function.  */
    vsync_init(cbm2_rfsh_per_sec, C610_PAL_CYCLES_PER_SEC, vsync_hook);

    /* Initialize sound.  Notice that this does not really open the audio
       device yet.  */
    sound_init(C610_PAL_CYCLES_PER_SEC, cbm2_cycles_per_rfsh);

    /* Initialize the CBM-II-specific part of the UI.  */
    c610_ui_init();

    return 0;
}

/* CBM-II-specific initialization.  */
void machine_reset(void)
{
    maincpu_int_status.alarm_handler[A_RASTERDRAW] = int_rasterdraw;
    maincpu_int_status.alarm_handler[A_ACIA1] = int_acia1;
    maincpu_int_status.alarm_handler[A_CIA1TA] = int_cia1ta;
    maincpu_int_status.alarm_handler[A_CIA1TB] = int_cia1tb;
    maincpu_int_status.alarm_handler[A_CIA1TOD] = int_cia1tod;
    reset_acia1();
    reset_cia1();
    reset_tpi1();
    reset_tpi2();

    reset_crtc();
    sid_reset();

#ifdef HAVE_PRINTER
    print_reset();
#endif

#ifdef HAVE_RS232
    rs232_reset();
#endif

    drive_reset();

    set_bank_exec(15);
    set_bank_ind(15);
}

void machine_powerup(void)
{
    mem_powerup();
    maincpu_trigger_reset();
}

void machine_shutdown(void)
{
    /* Detach all devices.  */
    serial_remove(-1);
}

void machine_handle_pending_alarms(int num_write_cycles)
{
}


/* ------------------------------------------------------------------------- */

/* This hook is called at the end of every frame.  */
static void vsync_hook(void)
{
    CLOCK sub;

    drive_vsync_hook();

    autostart_advance();

    sub = maincpu_prevent_clk_overflow(C610_PAL_CYCLES_PER_RFSH);

    if (sub > 0) {
	crtc_prevent_clk_overflow(sub);
	cia1_prevent_clk_overflow(sub);
        sound_prevent_clk_overflow(sub);
        vsync_prevent_clk_overflow(sub);
    }

    /* The 1541 has to deal both with our overflowing and its own one, so it
       is called even when there is no overflowing in the main CPU.  */
    /* FIXME: Do we have to check drive_enabled here?  */
    drive_prevent_clk_overflow(sub, 0);
    drive_prevent_clk_overflow(sub, 1);

#ifdef HAS_JOYSTICK
    joystick();
#endif
}

/* Dummy - no restore key.  */
int machine_set_restore_key(int v)
{
    return 0;	/* key not used -> lookup in keymap */
}

/* ------------------------------------------------------------------------- */

long machine_get_cycles_per_second(void)
{
    return C610_PAL_CYCLES_PER_SEC;
}

/* Set the screen refresh rate, as this is variable in the CRTC */
void machine_set_cycles_per_frame(long cpf) {

    cbm2_cycles_per_rfsh = cpf;
    cbm2_rfsh_per_sec = ((double) C610_PAL_CYCLES_PER_SEC) / ((double) cpf);

    fprintf(logfile, "machine_set_cycles: cycl/frame=%ld, freq=%e\n", cpf,
						cbm2_rfsh_per_sec);

    vsync_init(cbm2_rfsh_per_sec, C610_PAL_CYCLES_PER_SEC, vsync_hook);

    /* sound_set_cycles_per_rfsh(cbm2_cycles_per_rfsh); */
}

/* ------------------------------------------------------------------------- */

/* #define SNAP_MACHINE_NAME   "C610" */
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
    if (maincpu_write_snapshot_module(s) < 0
        || mem_write_snapshot_module(s, save_roms) < 0
        || crtc_write_snapshot_module(s) < 0
        || cia1_write_snapshot_module(s) < 0
        || tpi1_write_snapshot_module(s) < 0
        || tpi2_write_snapshot_module(s) < 0
        || acia1_write_snapshot_module(s) < 0
        || sid_write_snapshot_module(s) < 0
        || drive_write_snapshot_module(s, save_disks, save_roms) < 0
	) {
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
    if (s == NULL) {
        return -1;
    }

    if (major != SNAP_MAJOR || minor != SNAP_MINOR) {
        fprintf(logfile, "Snapshot version (%d.%d) not valid: expecting %d.%d.\n",
               major, minor, SNAP_MAJOR, SNAP_MINOR);
        goto fail;
    }

    if (maincpu_read_snapshot_module(s) < 0
        || mem_read_snapshot_module(s) < 0
        || crtc_read_snapshot_module(s) < 0
        || cia1_read_snapshot_module(s) < 0
        || tpi1_read_snapshot_module(s) < 0
        || tpi2_read_snapshot_module(s) < 0
        || acia1_read_snapshot_module(s) < 0
	|| sid_read_snapshot_module(s) < 0
        || drive_read_snapshot_module(s) < 0
	)
        goto fail;

    return 0;

fail:
    if (s != NULL)
        snapshot_close(s);
    maincpu_trigger_reset();
    return -1;
}
