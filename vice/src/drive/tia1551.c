/*
 * tia1551.c - 1551 tripple interface adaptor emulation.
 *
 * Written by
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
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
#include <string.h>

#include "drive.h"
#include "drivetypes.h"
#include "iecdrive.h"
#include "log.h"
#include "types.h"

typedef struct tia1551_s {
    BYTE ddra;
    BYTE dataa;
    BYTE ddrb;
    BYTE datab;
    BYTE ddrc;
    BYTE datac;
} tia1551_t;

BYTE tia1551_outputa[2], tia1551_outputb[2], tia1551_outputc[2];

static tia1551_t tia1551[2];
static log_t tia1551_log = LOG_ERR;

static void tia1551d_store(ADDRESS addr, BYTE byte, unsigned int dnr);
static BYTE tia1551d_read(ADDRESS addr, unsigned int dnr);
static void tia1551d_reset(unsigned int dnr);

static void tia1551d0_init(void)
{
    if (tia1551_log == LOG_ERR)
        tia1551_log = log_open("TIA1551");

    memset(&tia1551[0], 0, sizeof(tia1551_t));
}

static void tia1551d1_init(void)
{
    memset(&tia1551[1], 0, sizeof(tia1551_t));
}

void tia1551_init(drive_context_t *drv)
{
    if (drv->mynumber == 0)
        tia1551d0_init();
    else
        tia1551d1_init();
}

void REGPARM3 tia1551_store(drive_context_t *drv, ADDRESS addr, BYTE byte)
{
    tia1551d_store((ADDRESS)(addr & 3), byte, drv->mynumber);
}

BYTE REGPARM2 tia1551_read(drive_context_t *drv, ADDRESS addr)
{
    return tia1551d_read((ADDRESS)(addr & 3), drv->mynumber);
}

void tia1551_reset(drive_context_t *drv)
{
    tia1551d_reset(drv->mynumber);
}

/*-----------------------------------------------------------------------*/

inline void tia1551_porta_update(unsigned int dnr)
{
    static BYTE old_output[2];
    BYTE output, input;

    output = (tia1551[dnr].dataa & tia1551[dnr].ddra)
             | ~tia1551[dnr].ddra;
    tia1551_outputa[dnr] = output;
    tcbm_update_bus();
    input = tcbm_busa[dnr];

    tia1551[dnr].dataa = (tia1551[dnr].dataa & tia1551[dnr].ddra)
                         | (input & ~tia1551[dnr].ddra);
    old_output[dnr] = output;
}

inline void tia1551_portb_update(unsigned int dnr)
{
    static BYTE old_output[2];
    BYTE output, input;

    output = (tia1551[dnr].datab & tia1551[dnr].ddrb)
             | ~tia1551[dnr].ddrb;

    input = 0;

    tia1551[dnr].datab = (tia1551[dnr].datab & tia1551[dnr].ddrb)
                         | (input & ~tia1551[dnr].ddrb);
    old_output[dnr] = output;
}

inline void tia1551_portc_update(unsigned int dnr)
{
    static BYTE old_output[2];
    BYTE output, input;

    output = (tia1551[dnr].datac & tia1551[dnr].ddrc)
             | ~tia1551[dnr].ddrc;

    tia1551_outputb[dnr] = output & 3;
    tia1551_outputc[dnr] = ((output >> 1) & 0x40)| ((output << 4) & 0x80);
    tcbm_update_bus();
    input = (dnr << 5) | (tcbm_busb[dnr] & 3) | ((tcbm_busc[dnr] & 0x40) << 1)
            | ((tcbm_busc[dnr] & 0x80) >> 4);

    if (dnr == 0)
        input |= drive_sync_found(drive0_context.drive_ptr) ? 0x40 : 0;
    else
        input |= drive_sync_found(drive1_context.drive_ptr) ? 0x40 : 0;

    tia1551[dnr].datac = (tia1551[dnr].datac & tia1551[dnr].ddrc)
                         | (input & ~tia1551[dnr].ddrc);
    old_output[dnr] = output;
}

inline static BYTE dataa_read(unsigned int dnr)
{
    tia1551_porta_update(dnr);
    return tia1551[dnr].dataa;
}

inline static BYTE datab_read(unsigned int dnr)
{
    tia1551_portb_update(dnr);
    return tia1551[dnr].datab;
}

inline static BYTE datac_read(unsigned int dnr)
{
    tia1551_portc_update(dnr);
    return tia1551[dnr].datac;
}

inline static BYTE ddra_read(unsigned int dnr)
{
    tia1551_porta_update(dnr);
    return tia1551[dnr].ddra;
}

inline static BYTE ddrb_read(unsigned int dnr)
{
    tia1551_portb_update(dnr);
    return tia1551[dnr].ddrb;
}

inline static BYTE ddrc_read(unsigned int dnr)
{
    tia1551_portc_update(dnr);
    return tia1551[dnr].ddrc;
}

inline static void dataa_store(BYTE byte, unsigned int dnr)
{
    tia1551[dnr].dataa = byte;
    tia1551_porta_update(dnr);
}

inline static void datab_store(BYTE byte, unsigned int dnr)
{
    tia1551[dnr].datab = byte;
    tia1551_portb_update(dnr);
}

inline static void datac_store(BYTE byte, unsigned int dnr)
{
    tia1551[dnr].datac = byte;
    tia1551_portc_update(dnr);
}

inline static void ddra_store(BYTE byte, unsigned int dnr)
{
    tia1551[dnr].ddra = byte;
    tia1551_porta_update(dnr);
}

inline static void ddrb_store(BYTE byte, unsigned int dnr)
{
    tia1551[dnr].ddrb = byte;
    tia1551_portb_update(dnr);
}

inline static void ddrc_store(BYTE byte, unsigned int dnr)
{
    tia1551[dnr].ddrc = byte;
    tia1551_portc_update(dnr);
}

/*-----------------------------------------------------------------------*/

static void tia1551d_reset(unsigned int dnr)
{
}

static void tia1551d_store(ADDRESS addr, BYTE byte, unsigned int dnr)
{
    switch (addr & 7) {
      case 0:
        dataa_store(byte, dnr);
        break;
      case 1:
        datab_store(byte, dnr);
        break;
      case 2:
        datac_store(byte, dnr);
        break;
      case 3:
        ddra_store(byte, dnr);
        break;
      case 4:
        ddrb_store(byte, dnr);
        break;
      case 5:
        ddrc_store(byte, dnr);
        break;
    }
}

static BYTE tia1551d_read(ADDRESS addr, unsigned int dnr)
{
    switch (addr & 7) {
      case 0:
        return dataa_read(dnr);
      case 1:
        return datab_read(dnr);
      case 2:
        return datac_read(dnr);
      case 3:
        return ddra_read(dnr);
      case 4:
        return ddrb_read(dnr);
      case 5:
        return ddrc_read(dnr);
    }

    return 0;
}

