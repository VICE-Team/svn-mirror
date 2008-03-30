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

#include "c610.h"

#include "attach.h"
#include "autostart.h"
#include "cmdline.h"
#include "snapshot.h"
#include "crtc.h"
#include "interrupt.h"
#include "kbd.h"
#include "kbdbuf.h"
#include "machine.h"
#include "maincpu.h"
#include "c610mem.h"
#include "c610ui.h"
#include "c610cia.h"
#include "c610acia.h"
#include "c610tpi.h"
#include "sid.h"
#include "resources.h"
#include "sound.h"
#include "traps.h"
#include "utils.h"
#include "via.h"
#include "vmachine.h"
#include "vsync.h"

#ifdef HAVE_PRINTER
#include "print.h"
#include "prdevice.h"
#include "pruser.h"
#endif

#ifdef HAVE_RS232
#include "rs232.h"
#endif

static void vsync_hook(void);

const char machine_name[] = "C610";

/* ------------------------------------------------------------------------- */

/* CBM-II resources.  */

#if 0

/* CBM-II model name.  */
static char *model_name;

static int set_model_name(resource_value_t v)
{
    char *name = (char *)v;

    if (c610_set_model(name, NULL) < 0) {
        fprintf(stderr, "Invalid CBM-II model `%s'.\n", name);
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
        || acia1_init_resources() < 0	/* ACIA is always there */
#ifdef HAVE_RS232
        || rs232_init_resources() < 0
#endif
#ifdef HAVE_PRINTER
        || print_init_resources() < 0
        || prdevice_init_resources() < 0
        || pruser_init_resources() < 0
#endif
#ifdef __MSDOS__
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
        || acia1_init_cmdline_options() < 0
#ifdef HAVE_RS232
        || rs232_init_cmdline_options() < 0
#endif
#ifdef HAVE_PRINTER
        || print_init_cmdline_options() < 0
        || prdevice_init_cmdline_options() < 0
        || pruser_init_cmdline_options() < 0
#endif
#ifdef __MSDOS__
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

    printf("\nInitializing IEEE488 bus...\n");

    /* No traps installed on the CBM-II.  */
    serial_init(NULL);

    /* Initialize drives.  */
    file_system_init();

#ifdef HAVE_PRINTER
    /* initialize print devices */
    print_init();
#endif

    /* Initialize the CRTC emulation.  */
    crtc_init();

    /* Initialize the keyboard.  */
#ifdef __MSDOS__
    if (pet_kbd_init() < 0)
        return -1;
#else
    if (kbd_init() < 0)
        return -1;
#endif

    /* Initialize the monitor.  */
    monitor_init(&maincpu_monitor_interface, NULL);

    /* Initialize vsync and register our hook function.  */
    vsync_init(C610_PAL_RFSH_PER_SEC, C610_PAL_CYCLES_PER_SEC, vsync_hook);

    /* Initialize sound.  Notice that this does not really open the audio
       device yet.  */
    sound_init(C610_PAL_CYCLES_PER_SEC, C610_PAL_CYCLES_PER_RFSH);

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

    set_bank_exec(15);
    set_bank_ind(15);
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

    autostart_advance();

    sub = maincpu_prevent_clk_overflow(C610_PAL_CYCLES_PER_RFSH);

    if (sub > 0) {
	crtc_prevent_clk_overflow(sub);
	cia1_prevent_clk_overflow(sub);
        sound_prevent_clk_overflow(sub);
        vsync_prevent_clk_overflow(sub);
    }
}

/* Dummy - no restore key.  */
int machine_set_restore_key(int v)
{
    return 0;	/* key not used -> lookup in keymap */
}

/* ------------------------------------------------------------------------- */

#define SNAP_MACHINE_NAME   "C64"
#define SNAP_MAJOR          0
#define SNAP_MINOR          0

int machine_write_snapshot(const char *name)
{
    FILE *f;

    f = snapshot_create(name, SNAP_MAJOR, SNAP_MINOR, SNAP_MACHINE_NAME);
    if (f == NULL) {
        perror(name);
        return -1;
    }
    if (maincpu_write_snapshot_module(f) < 0
/*
        || mem_write_snapshot_module(f) < 0
        || vic_ii_write_snapshot_module(f) < 0
        || cia1_write_snapshot_module(f) < 0
        || cia2_write_snapshot_module(f) < 0 */ ) {
        fclose(f);
        unlink(name);
        return -1;
    }

    fclose(f);
    return 0;
}

int machine_read_snapshot(const char *name)
{
    FILE *f;
    BYTE minor, major;
    char machine_name[SNAPSHOT_MACHINE_NAME_LEN];

    f = snapshot_open(name, &major, &minor, machine_name);
    if (f == NULL) {
        perror(name);
        return -1;
    }

    if (major != SNAP_MAJOR || minor != SNAP_MINOR) {
        printf("Snapshot version (%d.%d) not valid: expecting %d.%d.\n",
               major, minor, SNAP_MAJOR, SNAP_MINOR);
        goto fail;
    }

    if (maincpu_read_snapshot_module(f) < 0
/*
        || mem_read_snapshot_module(f) < 0
        || vic_ii_read_snapshot_module(f) < 0
        || cia1_read_snapshot_module(f) < 0
        || cia2_read_snapshot_module(f) < 0 */ )
        goto fail;

    return 0;

fail:
    if (f != NULL)
        fclose(f);
    return -1;
}

