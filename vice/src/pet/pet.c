/*
 * pet.c
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
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

#include "pet.h"

#include "attach.h"
#include "autostart.h"
#include "cmdline.h"
#include "crtc.h"
#include "interrupt.h"
#include "kbd.h"
#include "kbdbuf.h"
#include "machine.h"
#include "maincpu.h"
#include "petmem.h"
#include "pets.h"
#include "petsound.h"
#include "petui.h"
#include "petvia.h"
#include "petacia.h"
#include "petpia.h"
#include "resources.h"
#include "snapshot.h"
#include "sound.h"
#include "traps.h"
#include "utils.h"
#include "vdrive.h"
#include "drive.h"
#include "via.h"
#include "vmachine.h"
#include "vsync.h"

#ifdef HAVE_RS232
#include "rs232.h"
#endif

#ifdef HAVE_PRINTER
#include "print.h"
#include "prdevice.h"
#include "pruser.h"
#endif

#if defined __MSDOS__ || defined WIN32
#include "petkbd.h"
#endif

static void vsync_hook(void);

static long 	pet_cycles_per_rfsh 	= PET_PAL_CYCLES_PER_RFSH;
static double	pet_rfsh_per_sec 	= PET_PAL_RFSH_PER_SEC;

const char machine_name[] = "PET";

int machine_class = VICE_MACHINE_PET;

/* ------------------------------------------------------------------------- */

/* PET resources.  */

/* PET model name.  */
static char *model_name;

static int set_model_name(resource_value_t v)
{
    char *name = (char *)v;

    if (pet_set_model(name, NULL) < 0) {
        fprintf(errfile, "Invalid PET model `%s'.\n", name);
        return -1;
    }

    string_set(&model_name, name);
    return 0;
}

/* ------------------------------------------------------------------------- */

#if 0

static resource_t resources[] = {
    { "Model", RES_STRING, (resource_value_t) "8032",
      (resource_value_t *) &model_name, set_model_name },
    { NULL }
};

static cmdline_option_t cmdline_options[] = {
    { "-model", SET_RESOURCE, 1, NULL, NULL, "Model", NULL,
      "<name>", "Specify PET model name" },
    { NULL }
};

#endif

/* ------------------------------------------------------------------------ */

/* PET-specific resource initialization.  This is called before initializing
   the machine itself with `machine_init()'.  */
int machine_init_resources(void)
{
#if 0
    if (resources_register(resources) < 0)
        return -1;
#endif

    if (traps_init_resources() < 0
	|| vsync_init_resources() < 0
        || video_init_resources() < 0
        || pet_mem_init_resources() < 0
        || pet_init_resources() < 0
        || crtc_init_resources() < 0
        || pia1_init_resources() < 0
        || sound_init_resources() < 0
        || drive_init_resources() < 0
        || acia1_init_resources() < 0
#ifdef HAVE_RS232
	|| rs232_init_resources() < 0
#endif
#ifdef HAVE_PRINTER
        || print_init_resources() < 0
        || prdevice_init_resources() < 0
        || pruser_init_resources() < 0
#endif
#if defined __MSDOS__ || defined WIN32 || defined(__riscos)
        || kbd_init_resources() < 0)
#else
        || pet_kbd_init_resources() < 0)
#endif
        return -1;

    return 0;
}

/* PET-specific command-line option initialization.  */
int machine_init_cmdline_options(void)
{
#if 0
    if (cmdline_register_options(cmdline_options) < 0)
        return -1;
#endif

    if (traps_init_cmdline_options() < 0
	|| vsync_init_cmdline_options() < 0
        || video_init_cmdline_options() < 0
        || pet_mem_init_cmdline_options() < 0
        || pet_init_cmdline_options() < 0
        || crtc_init_cmdline_options() < 0
        || pia1_init_cmdline_options() < 0
        || sound_init_cmdline_options() < 0
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
        || kbd_init_cmdline_options() < 0)
#else
        || pet_kbd_init_cmdline_options() < 0)
#endif
        return -1;

    return 0;
}

/* ------------------------------------------------------------------------- */

/* PET-specific initialization.  */
int machine_init(void)
{
    /* Setup trap handling - must be before mem_load() */
    traps_init();

    if (mem_load() < 0)
        return -1;

    fprintf(logfile, "\nInitializing IEEE488 bus...\n");

    /* No traps installed on the PET.  */
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

    /* Initialize autostart.  FIXME: We could probably use smaller values.  */
    /* moved to mem_load() as it is kernal-dependant AF 30jun1998
    autostart_init(1 * PET_PAL_RFSH_PER_SEC * PET_PAL_CYCLES_PER_RFSH, 0);
    */

    /* Initialize the CRTC emulation.  */
    crtc_init();

    /* Initialize the keyboard.  */
#if defined __MSDOS__ || defined WIN32
    if (pet_kbd_init() < 0)
        return -1;
#else
    if (kbd_init() < 0)
        return -1;
#endif

    /* Fire up the hardware-level 1541 emulation.  */
    drive_init(PET_PAL_CYCLES_PER_SEC, PET_NTSC_CYCLES_PER_SEC);

    /* Initialize the monitor.  */
    monitor_init(&maincpu_monitor_interface, &drive0_monitor_interface,
                 &drive1_monitor_interface);

    /* Initialize vsync and register our hook function.  */
    vsync_init(pet_rfsh_per_sec, PET_PAL_CYCLES_PER_SEC, vsync_hook);

    /* Initialize sound.  Notice that this does not really open the audio
       device yet.  */
    sound_init(PET_PAL_CYCLES_PER_SEC, pet_cycles_per_rfsh);

    /* Initialize keyboard buffer.  FIXME: Is this correct?  */
    /* moved to mem_load() because it's model specific... AF 30jun1998
    kbd_buf_init(631, 198, 10, PET_PAL_CYCLES_PER_RFSH * PET_PAL_RFSH_PER_SEC);
    */

    /* Initialize the PET-specific part of the UI.  */
    pet_ui_init();

    return 0;
}

/* PET-specific initialization.  */
void machine_reset(void)
{
    maincpu_int_status.alarm_handler[A_RASTERDRAW] = int_rasterdraw;
    maincpu_int_status.alarm_handler[A_VIAT1] = int_viat1;
    maincpu_int_status.alarm_handler[A_VIAT2] = int_viat2;
    reset_pia1();
    reset_pia2();
    reset_via();
    reset_crtc();
    petsnd_reset();
    superpet_reset();
#ifdef HAVE_PRINTER
    print_reset();
#endif
    drive_reset();
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

/* ------------------------------------------------------------------------- */

/* This hook is called at the end of every frame.  */
static void vsync_hook(void)
{
    CLOCK sub;

    autostart_advance();

    drive_vsync_hook();

    sub = maincpu_prevent_clk_overflow(PET_PAL_CYCLES_PER_RFSH);

    if (sub > 0) {
	crtc_prevent_clk_overflow(sub);
	via_prevent_clk_overflow(sub);
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
    return PET_PAL_CYCLES_PER_SEC;
}

/* Set the screen refresh rate, as this is variable in the CRTC */
void machine_set_cycles_per_frame(long cpf) {

    pet_cycles_per_rfsh = cpf;
    pet_rfsh_per_sec = ((double) PET_PAL_CYCLES_PER_SEC) / ((double) cpf);

    fprintf(logfile, "machine_set_cycles: cycl/frame=%ld, freq=%e\n", cpf,
							pet_rfsh_per_sec);

    vsync_init(pet_rfsh_per_sec, PET_PAL_CYCLES_PER_SEC, vsync_hook);

    /* sound_set_cycles_per_rfsh(pet_cycles_per_rfsh); */
}

/* ------------------------------------------------------------------------- */

/*#define SNAP_MACHINE_NAME   "PET"*/
#define SNAP_MAJOR          0
#define SNAP_MINOR          0

/* now machine_name[] is used */
/* const char machine_snapshot_name[] = SNAP_MACHINE_NAME; */

int machine_write_snapshot(const char *name, int save_roms, int save_disks)
{
    snapshot_t *s;
    int ef = 0;

    s = snapshot_create(name, SNAP_MAJOR, SNAP_MINOR, machine_name);
    if (s == NULL) {
        perror(name);
        return -1;
    }
    if (maincpu_write_snapshot_module(s) < 0
        || mem_write_snapshot_module(s, save_roms) < 0
        || crtc_write_snapshot_module(s) < 0
        || pia1_write_snapshot_module(s) < 0
        || pia2_write_snapshot_module(s) < 0
        || via_write_snapshot_module(s) < 0
        || drive_write_snapshot_module(s, save_disks, save_roms) < 0
	) {
	ef = -1;
    }

    if ((!ef) && pet.superpet) {
	ef = acia1_write_snapshot_module(s);
    }

    snapshot_close(s);

    if (ef) {
	unlink(name);
    }
    return ef;
}

int machine_read_snapshot(const char *name)
{
    snapshot_t *s;
    BYTE minor, major;
    int ef = 0;

    s = snapshot_open(name, &major, &minor, machine_name);
    if (s == NULL) {
        return -1;
    }

    if (major != SNAP_MAJOR || minor != SNAP_MINOR) {
        fprintf(logfile, "Snapshot version (%d.%d) not valid: expecting %d.%d.\n",
               major, minor, SNAP_MAJOR, SNAP_MINOR);
        ef = -1;
    }

    if (ef
	|| maincpu_read_snapshot_module(s) < 0
        || mem_read_snapshot_module(s) < 0
        || crtc_read_snapshot_module(s) < 0
        || pia1_read_snapshot_module(s) < 0
        || pia2_read_snapshot_module(s) < 0
        || via_read_snapshot_module(s) < 0
        || drive_read_snapshot_module(s) < 0
	) {
	ef = -1;
    }

    if (!ef) {
	if (acia1_read_snapshot_module(s) < 0) {
	    reset_acia1();	/* clear interrupts */
	}
    }

    snapshot_close(s);

    if (ef) {
        maincpu_trigger_reset();
    }
    return ef;
}

#ifdef __riscos
/* Dummies needed for RISC OS version (accessed by ui.c) */
void cartridge_detach_image(void)
{
}

CLOCK vic_ii_fetch_clk, vic_ii_draw_clk;

int int_rasterfetch(long offset)
{
  return 0;
}
#endif

