/*
 * serial-trap.c
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
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

#include "iec.h"
#include "maincpu.h"
#include "mem.h"
#include "mos6510.h"
#include "serial-trap.h"
#include "serial.h"
#include "types.h"


/* Warning: these are only valid for the VIC20, C64 and C128, but *not* for
   the PET.  (FIXME?)  */
#define BSOUR 0x95 /* Buffered Character for IEEE Bus */


/* Address of serial TMP register.  */
static ADDRESS tmp_in;

/* On which channel did listen happen to?  */
BYTE TrapDevice;
BYTE TrapSecondary;


/* Command Serial Bus to TALK, LISTEN, UNTALK, or UNLISTEN, and send the
   Secondary Address to Serial Bus under Attention.  */
void serial_trap_attention(void)
{
    BYTE b;
    serial_t *p;

    /*
     * Which Secondary Address ?
     */
    b = mem_read(((BYTE)(BSOUR))); /* BSOUR - character for serial bus */

    /* do a flush if unlisten for close and command channel */
    if (b == 0x3f) {
        iec_unlisten(TrapDevice, TrapSecondary);
    } else if (b == 0x5f) {
        iec_untalk(TrapDevice, TrapSecondary);
    } else {
        switch (b & 0xf0) {
          case 0x20:
          case 0x40:
            TrapDevice = b;
            break;
          case 0x60:
            TrapSecondary = b;
            iec_listentalk(TrapDevice, TrapSecondary);
            break;
          case 0xe0:
            TrapSecondary = b;
            iec_close(TrapDevice, TrapSecondary);
            break;
          case 0xf0:
            TrapSecondary = b;
            iec_open(TrapDevice, TrapSecondary);
            break;
        }
    }

    p = serial_get_device(TrapDevice & 0x0f);
    if (!(p->inuse))
        serial_set_st(0x80);

    MOS6510_REGS_SET_CARRY(&maincpu_regs, 0);
    MOS6510_REGS_SET_INTERRUPT(&maincpu_regs, 0);

    if (attention_callback_func)
        attention_callback_func();
}

/* Send one byte on the serial bus.  */
void serial_trap_send(void)
{
    BYTE data;

    data = mem_read(BSOUR); /* BSOUR - character for serial bus */

    iec_write(TrapDevice, TrapSecondary, data, serial_set_st);

    MOS6510_REGS_SET_CARRY(&maincpu_regs, 0);
    MOS6510_REGS_SET_INTERRUPT(&maincpu_regs, 0);
}

/* Receive one byte from the serial bus.  */
void serial_trap_receive(void)
{
    BYTE data;

    data = iec_read(TrapDevice, TrapSecondary, serial_set_st);

    mem_store(tmp_in, data);

    /* If at EOF, call specified callback function.  */
    if ((serial_get_st() & 0x40) && eof_callback_func != NULL)
        eof_callback_func();

    /* Set registers like the Kernal routine does.  */
    MOS6510_REGS_SET_A(&maincpu_regs, data);
    MOS6510_REGS_SET_SIGN(&maincpu_regs, (data & 0x80) ? 1 : 0);
    MOS6510_REGS_SET_ZERO(&maincpu_regs, data ? 0 : 1);
    MOS6510_REGS_SET_CARRY(&maincpu_regs, 0);
    MOS6510_REGS_SET_INTERRUPT(&maincpu_regs, 0);
}


/* Kernal loops serial-port (0xdd00) to see when serial is ready: fake it.
   EEA9 Get serial data and clk in (TKSA subroutine).  */

void serial_trap_ready(void)
{
    MOS6510_REGS_SET_A(&maincpu_regs, 1);
    MOS6510_REGS_SET_SIGN(&maincpu_regs, 0);
    MOS6510_REGS_SET_ZERO(&maincpu_regs, 0);
    MOS6510_REGS_SET_INTERRUPT(&maincpu_regs, 0);
}

void serial_trap_init(ADDRESS tmpin)
{
    tmp_in = tmpin;
}

