/*
 * c64.c
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

#include "interrupt.h"
#include "vicii.h"
#include "cia.h"
#include "vmachine.h"
#include "machspec.h"
#include "maincpu.h"
#include "kbdbuf.h"


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

/* ------------------------------------------------------------------------ */

/* C64-specific initialization.  */
void machine_init(void)
{
    vic_ii_init();
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

/* Return nonzero if `addr' is in the trappable address space.  */
int rom_trap_allowed(ADDRESS addr)
{
    return 1; /* FIXME */
}
  
