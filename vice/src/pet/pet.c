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
#include "pia.h"
#include "resources.h"
#include "snapshot.h"
#include "sound.h"
#include "traps.h"
#include "utils.h"
#include "vdrive.h"
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

#ifdef __MSDOS__
#include "petkbd.h"
#endif

static void vsync_hook(void);

const char machine_name[] = "PET";

/* ------------------------------------------------------------------------- */

/* PET resources.  */

/* PET model name.  */
static char *model_name;

static int set_model_name(resource_value_t v)
{
    char *name = (char *)v;

    if (pet_set_model(name, NULL) < 0) {
        fprintf(stderr, "Invalid PET model `%s'.\n", name);
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
        || pia_init_resources() < 0
        || sound_init_resources() < 0
        || acia1_init_resources() < 0
#ifdef HAVE_RS232
	|| rs232_init_resources() < 0
#endif
#ifdef HAVE_PRINTER
        || print_init_resources() < 0
        || prdevice_init_resources() < 0
        || pruser_init_resources() < 0
#endif
#ifdef __MSDOS__
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
        || pia_init_cmdline_options() < 0
        || sound_init_cmdline_options() < 0
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

    printf("\nInitializing IEEE488 bus...\n");

    /* No traps installed on the PET.  */
    serial_init(NULL);

    /* Initialize drives.  */
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
    vsync_init(PET_PAL_RFSH_PER_SEC, PET_PAL_CYCLES_PER_SEC, vsync_hook);

    /* Initialize sound.  Notice that this does not really open the audio
       device yet.  */
    sound_init(PET_PAL_CYCLES_PER_SEC, PET_PAL_CYCLES_PER_RFSH);

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

    sub = maincpu_prevent_clk_overflow(PET_PAL_CYCLES_PER_RFSH);

    if (sub > 0) {
	crtc_prevent_clk_overflow(sub);
	via_prevent_clk_overflow(sub);
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

/* PET machine config dump
 *
 *
 *
 */
#define	PET_DUMP_VER_MAJOR	0
#define	PET_DUMP_VER_MINOR	0

/* FIXME: Error check.  */
int pet_dump(FILE *p)
{
    snapshot_module_t *m;

    m = snapshot_module_create(p, "PET",
                               PET_DUMP_VER_MAJOR, PET_DUMP_VER_MINOR);
    if (m == NULL)
        return -1;

    snapshot_module_write_word(m, pet.ramSize);	/* in k */
    snapshot_module_write_word(m, pet.IOSize);		/* in byte */
    snapshot_module_write_byte(m, pet.video);		/* screen width */
    snapshot_module_write_byte(m, (pet.mem9 ? 1 : 0) | (pet.memA ? 2 : 0)
                               | (pet.pet2k ? 4 : 0) | (pet.superpet ? 8 : 0)
                               | (pet.crtc ? 16 : 0) );
    snapshot_module_write_byte(m, pet.kbd_type);	/* 1 = graph, 0 = business */

    return snapshot_module_close(m);
}

/* FIXME: Error check.  */
int pet_undump(FILE *p)
{
    char name[SNAPSHOT_MODULE_NAME_LEN];
    BYTE vmajor, vminor;
    BYTE byte;
    WORD word;
    int ival;
    snapshot_module_t *m;

    m = snapshot_module_open(p, name, &vmajor, &vminor);
    if (m == NULL)
        return -1;

    if (strcmp(name, "PET") || vmajor != PET_DUMP_VER_MAJOR) {
        snapshot_module_close(m);
        return -1;
    }

    snapshot_module_read_word(m, &word); ival = word;
    resources_set_value("RamSize", (resource_value_t) ival);
    snapshot_module_read_word(m, &word); ival = word;
    resources_set_value("IOSize", (resource_value_t) ival);
    snapshot_module_read_byte(m, &byte); ival = byte;
    resources_set_value("VideoSize", (resource_value_t) ival);

    snapshot_module_read_byte(m, &byte);
    ival = byte & 1;
    resources_set_value("Ram9", (resource_value_t) ival);
    ival = (byte & 2) ? 1 : 0;
    resources_set_value("RamA", (resource_value_t) ival);
    ival = (byte & 4) ? 1 : 0;
    pet.pet2k = ival;	/* hm... */
    ival = (byte & 8) ? 1 : 0;
    resources_set_value("SuperPET", (resource_value_t) ival);
    ival = (byte & 16) ? 1 : 0;
    resources_set_value("Crtc", (resource_value_t) ival);

    snapshot_module_read_byte(m, &byte);
    resources_get_value("KeymapIndex", (resource_value_t *) &ival);
    resources_set_value("KeymapIndex", (resource_value_t) ((ival & 1)
                                       + 2 * (byte ? 1 : 0)));

    /* what about the ROM names? */
    mem_load();

    return snapshot_module_close(m);
}

/* ------------------------------------------------------------------------- */

int machine_write_snapshot(const char *name)
{
    return 0;
}

int machine_read_snapshot(const char *name)
{
    return 0;
}
