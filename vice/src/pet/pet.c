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

#include "machine.h"
#include "attach.h"
#include "autostart.h"
#include "cmdline.h"
#include "crtc.h"
#include "drive.h"
#include "interrupt.h"
#include "kbd.h"
#include "kbdbuf.h"
#include "maincpu.h"
#include "petmem.h"
#include "pets.h"
#include "petsound.h"
#include "petui.h"
#include "petvia.h"
#include "pia.h"
#include "resources.h"
#include "sound.h"
#include "utils.h"
#include "via.h"
#include "vmachine.h"
#include "vsync.h"

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

/*
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
*/

/* ------------------------------------------------------------------------ */

/* PET-specific resource initialization.  This is called before initializing
   the machine itself with `machine_init()'.  */
int machine_init_resources(void)
{
#if 0
    if (resources_register(resources) < 0)
        return -1;
#endif

    if (vsync_init_resources() < 0
        || video_init_resources() < 0
        || pet_mem_init_resources() < 0
        || crtc_init_resources() < 0
        || pia_init_resources() < 0
        || sound_init_resources() < 0)
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

    if (vsync_init_cmdline_options() < 0
        || video_init_cmdline_options() < 0
        || pet_mem_init_cmdline_options() < 0
        || crtc_init_cmdline_options() < 0
        || pia_init_cmdline_options() < 0
        || sound_init_cmdline_options() < 0)
        return -1;

    return 0;
}

/* ------------------------------------------------------------------------- */

/* PET-specific initialization.  */
int machine_init(void)
{
    if (mem_load() < 0)
        return -1;

    printf("\nInitializing IEEE488 bus...\n");

    /* No traps installed on the PET.  */
    serial_init(NULL);

    /* Initialize drives.  */
    file_system_init();

    /* Initialize the CRTC emulation.  */
    crtc_init();

    /* Initialize the keyboard.  FIXME!  */
#ifdef __MSDOS__
    if (pet_kbd_init() < 0)
        return -1;
#else
    if (1)
        kbd_init("busi_uk.vkm");
    else
        kbd_init("graphics.vkm");
#endif

    /* Initialize the monitor.  */
    monitor_init(&maincpu_monitor_interface, NULL);

    /* Initialize vsync and register our hook function.  */
    vsync_init(PET_PAL_RFSH_PER_SEC, PET_PAL_CYCLES_PER_SEC, vsync_hook);

    /* Initialize sound.  Notice that this does not really open the audio
       device yet.  */
    sound_init(PET_PAL_CYCLES_PER_SEC, PET_PAL_CYCLES_PER_RFSH);

    /* Initialize keyboard buffer.  FIXME: Is this correct?  */
    kbd_buf_init(631, 198, 10, PET_PAL_CYCLES_PER_RFSH * PET_PAL_RFSH_PER_SEC);

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
