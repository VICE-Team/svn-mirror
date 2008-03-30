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
#include "interrupt.h"
#include "via.h"
#include "vmachine.h"
#include "machspec.h"
#include "maincpu.h"
#include "kbd.h"
#include "kbdbuf.h"
#include "drive.h"
#include "pia.h"
#include "crtc.h"
#include "vsync.h"
#include "soundpet.h"
#include "attach.h"
#include "autostart.h"

static void vsync_hook(void);

/* Machine description.  */
machdesc_t machdesc = {
    /* Machine name.  */
    "PET",

    /* Flag: does this machine have joysticks?  */
    1,

    /* Flag: how many colors does this machine have?  */
    2,

    /* Flag: does this machine allow 1541 emulation?  */
    0,

    /* Flag: does this machine have a tape recorder?  */
    0,

    /* Flag: does this machine have a IEEE488 emulation?  */
    0,

    /* Flag: does this machine have sound capabilities?  */
    1,

    /* Flag: does this machine have a RAM Expansion unit?  */
    0,

    /* Flag: does this machine have hardware sprites?  */
    0,
};

/* ------------------------------------------------------------------------ */

/* PET-specific initialization.  */
int machine_init(void)
{
    if (mem_load() < 0)
        return -1;

    printf("\nInitializing IEEE488 bus...\n");

    /* No traps installed on the PET.  */
    serial_init(NULL);

    /* Initialize drives.  */
    initialize_drives();

    /* Initialize the CRTC emulation.  */
    crtc_init();

    /* Initialize the keyboard.  FIXME!  */
    if (1)
        kbd_init("busi_uk.vkm");
    else
        kbd_init("graphics.vkm");

    /* Initialize the monitor.  */
    monitor_init(&maincpu_monitor_interface, NULL);

    /* Initialize vsync and register our hook function.  */
    vsync_init(RFSH_PER_SEC, CYCLES_PER_SEC, vsync_hook);

    /* Initialize sound.  Notice that this does not really open the audio
       device yet.  */
    sound_init(PET_CYCLES_PER_SEC, PET_CYCLES_PER_RFSH);

    /* Initialize keyboard buffer.  FIXME: Is this correct?  */
    kbd_buf_init(631, 198, 10, PET_CYCLES_PER_RFSH * PET_RFSH_PER_SEC);

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

    autostart_advance();

    sub = maincpu_prevent_clk_overflow();

    if (sub > 0) {
	crtc_prevent_clk_overflow(sub);
	via_prevent_clk_overflow(sub);
        sid_prevent_clk_overflow(sub);
        vsync_prevent_clk_overflow(sub);
    }
}
