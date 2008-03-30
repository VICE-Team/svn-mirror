/*
 * mc6821.c - MC6821 emulation for the 1571 disk drives with DD3.
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

/*#define MC_DEBUG*/

#include "vice.h"

#include <stdio.h>
#include <string.h>

#include "drive.h"
#include "drivemem.h"
#include "drivetypes.h"
#include "iecdrive.h"
#include "log.h"
#include "mc6821.h"


/* mc6821 structure.  */
mc6821_t mc6821[DRIVE_NUM];


/*-----------------------------------------------------------------------*/
/* MC6821 register functions.  */

static void mc6821_write_pra(BYTE byte, unsigned int dnr)
{
    if (mc6821[dnr].drive->parallel_cable == DRIVE_PC_DD3)
        parallel_cable_drive_write(byte, PARALLEL_WRITE, dnr);

    mc6821[dnr].pra = byte;
}

static void mc6821_write_ddra(BYTE byte, unsigned int dnr)
{
    parallel_cable_drive_write((BYTE)((~byte) | mc6821[dnr].pra), PARALLEL_WRITE, dnr);

    mc6821[dnr].ddra = byte;
}

static BYTE mc6821_read_pra(unsigned int dnr)
{
    BYTE byte = 0xff;
    int hs = 0;

    if ((mc6821[dnr].cra & 0x28) == 0x28)
        hs = 1;

    if (mc6821[dnr].drive->parallel_cable == DRIVE_PC_DD3)
        byte = parallel_cable_drive_read(hs);

    mc6821[dnr].cra = (BYTE)(mc6821[dnr].cra & 0x7f);

    return (mc6821[dnr].pra & mc6821[dnr].ddra)
        | (byte & ~(mc6821[dnr].ddra));
}

static void mc6821_write_prb(BYTE byte, unsigned int dnr)
{
    mc6821[dnr].prb = byte;
}

static void mc6821_write_ddrb(BYTE byte, unsigned int dnr)
{
    mc6821[dnr].ddrb = byte;
}

static BYTE mc6821_read_prb(unsigned int dnr)
{
    mc6821[dnr].crb = (BYTE)(mc6821[dnr].crb & 0x7f);

    return (mc6821[dnr].prb & mc6821[dnr].ddrb)
        | (0xff & ~(mc6821[dnr].ddrb));
}

static void mc6821_write_cra(BYTE byte, unsigned int dnr)
{
    static BYTE old_cra = 0;

    if ((old_cra & 0x38) != (byte & 0x38)) {
#ifdef MC_DEBUG
        log_debug("MODE %02x", byte & 0x38);
#endif
        old_cra = byte;
    }

    mc6821[dnr].cra = byte;
}

static void mc6821_write_crb(BYTE byte, unsigned int dnr)
{
    mc6821[dnr].crb = byte;
}

/*-----------------------------------------------------------------------*/
/* MC6821 signals.  */

void mc6821_set_signal(drive_context_t *drive_context, int line)
{
    unsigned int dnr;

    dnr = drive_context->mynumber;

    switch (line) {
      case MC6821_SIG_CA1:
        mc6821[dnr].cra = (BYTE)(mc6821[dnr].cra | 0x80);
        break;
      case MC6821_SIG_CA2:
        mc6821[dnr].cra = (BYTE)(mc6821[dnr].cra | 0x40);
        break;
      case MC6821_SIG_CB1:
        mc6821[dnr].crb = (BYTE)(mc6821[dnr].crb | 0x80);
        break;
      case MC6821_SIG_CB2:
        mc6821[dnr].crb = (BYTE)(mc6821[dnr].crb | 0x40);
        break;
    }
}

/*-----------------------------------------------------------------------*/
/* MC6821 register read/write access.  */

static void mc6821_store_internal(WORD addr, BYTE byte, unsigned int dnr)
{
#ifdef MC_DEBUG
    log_debug("MC WRITE ADDR: %i DATA:%02x CLK:%i",
              addr, byte, drive_clk[dnr]);
#endif

    switch (addr) {
      case 0:
        if (mc6821[dnr].cra & 0x04)
            mc6821_write_pra(byte, dnr);
        else
            mc6821_write_ddra(byte, dnr);
        break;
      case 1:
        mc6821_write_cra(byte, dnr);
        break;
      case 2:
        if (mc6821[dnr].crb & 0x04)
            mc6821_write_prb(byte, dnr);
        else
            mc6821_write_ddrb(byte, dnr);
        break;
      case 3:
        mc6821_write_crb(byte, dnr);
        break;
    }
}

static BYTE mc6821_read_internal(WORD addr, unsigned int dnr)
{
    BYTE tmp = 0;

    switch (addr) {
      case 0:
        if (mc6821[dnr].cra & 0x04)
            tmp = mc6821_read_pra(dnr);
        else
            tmp = mc6821[dnr].ddra;
        break;
      case 1:
        tmp = mc6821[dnr].cra;
        break;
      case 2:
        if (mc6821[dnr].crb & 0x04)
            tmp = mc6821_read_prb(dnr);
        else
            tmp = mc6821[dnr].ddrb;
        break;
      case 3:
        tmp = mc6821[dnr].crb;
        break;
    }

#ifdef MC_DEBUG
    log_debug("MC READ ADDR: %i DATA:%02x CLK:%i",
              addr, tmp, drive_clk[dnr]);
#endif

    return tmp;
}

static void mc6821_reset_internal(unsigned int dnr)
{
    mc6821[dnr].pra = 0;
    mc6821[dnr].ddra = 0;
    mc6821[dnr].cra = 0;
    mc6821[dnr].prb = 0;
    mc6821[dnr].ddrb = 0;
    mc6821[dnr].crb = 0;
}

/*-----------------------------------------------------------------------*/

static void REGPARM3 mc6821_store(drive_context_t *drv, WORD addr, BYTE byte)
{
    mc6821_store_internal((WORD)(addr & 3), byte, drv->mynumber);
}

static BYTE REGPARM2 mc6821_read(drive_context_t *drv, WORD addr)
{
    return mc6821_read_internal((WORD)(addr & 3), drv->mynumber);
}

/*-----------------------------------------------------------------------*/

/* MC6821 external interface.  */

/* Functions using drive context.  */
void mc6821_init(drive_context_t *drv)
{
    mc6821[drv->mynumber].drive = drv->drive;
}

void mc6821_reset(drive_context_t *drv)
{
    mc6821_reset_internal(drv->mynumber);
}

void mc6821_mem_init(struct drive_context_s *drv, unsigned int type)
{
    drivecpud_context_t *cpud;

    cpud = drv->cpud;

    /* Setup parallel cable */
    if (type == DRIVE_TYPE_1541 || type == DRIVE_TYPE_1541II
        || type == DRIVE_TYPE_1570 || type == DRIVE_TYPE_1571
        || type == DRIVE_TYPE_1571CR) {
        if (drv->drive->parallel_cable == DRIVE_PC_DD3) {
            drivemem_set_func(cpud, 0x50, 0x60, mc6821_read, mc6821_store);
        }
    }
}

