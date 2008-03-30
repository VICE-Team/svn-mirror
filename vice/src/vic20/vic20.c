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

#include "interrupt.h"
#include "vic.h"
#include "via.h"
#include "vmachine.h"
#include "machspec.h"
#include "maincpu.h"
#include "kbdbuf.h"


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

/* Struct to access the kernal buffer.  */
kernal_kbd_buf_t kernal_kbd_buf = {
    
    /* First location of the buffer. FIXME? */
    631,

    /* Location that stores the number of characters pending in the
       buffer.  FIXME? */
    198,

    /* Maximum number of characters that fit in the buffer. FIXME? */
    10
    
};

/* ------------------------------------------------------------------------ */

/* VIC20-specific initialization.  */
void machine_init(void)
{
    vic_init();
}

/* VIC20-specific initialization.  */
void machine_reset(void)
{
    maincpu_int_status.alarm_handler[A_RASTERDRAW] = int_rasterdraw;
    maincpu_int_status.alarm_handler[A_VIA1T1] = int_via1t1;
    maincpu_int_status.alarm_handler[A_VIA1T2] = int_via1t2;
    maincpu_int_status.alarm_handler[A_VIA2T1] = int_via2t1;
    maincpu_int_status.alarm_handler[A_VIA2T2] = int_via2t2;
    maincpu_set_alarm_clk(A_RASTERDRAW, CYCLES_PER_LINE);
    reset_via1();
    reset_via2();

    true1541_reset();
}

/* Return nonzero if `addr' is in the trappable address space.  */
int rom_trap_allowed(ADDRESS addr)
{
    return 1; /* FIXME */
}
  
