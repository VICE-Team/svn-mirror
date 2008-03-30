/*
 * tcbm.c
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

#include "drive.h"
#include "drivecpu.h"
#include "maincpu.h"
#include "tcbm.h"
#include "tia1551.h"
#include "types.h"

/* #define TCBM_DEBUG */

typedef struct tiatcbm_s {
    BYTE ddra;
    BYTE dataa;
    BYTE ddrb;
    BYTE datab;
    BYTE ddrc;
    BYTE datac;
} tiatcbm_t;

static tiatcbm_t tiatcbm[2];

BYTE tcbm_busa[2], tcbm_busb[2], tcbm_busc[2];
BYTE tcbm_outputa[2], tcbm_outputb[2], tcbm_outputc[2];

/*-----------------------------------------------------------------------*/

void tcbm_update_bus(void)
{
    tcbm_busa[0] = tcbm_outputa[0] & tia1551_outputa[0];
    tcbm_busb[0] = tcbm_outputb[0] & tia1551_outputb[0] & 0x03;
    tcbm_busc[0] = tcbm_outputc[0] & tia1551_outputc[0] & 0xc0;
    tcbm_busa[1] = tcbm_outputa[1] & tia1551_outputa[1];
    tcbm_busb[1] = tcbm_outputb[1] & tia1551_outputb[1] & 0x03;
    tcbm_busc[1] = tcbm_outputc[1] & tia1551_outputc[1] & 0xc0;
#ifdef TCBM_DEBUG
    printf("BUS A:%02x B:%02x C:%02x\n",tcbm_busa[0],tcbm_busb[0],tcbm_busc[0]);
#endif
}

/*-----------------------------------------------------------------------*/

inline void tiatcbm_porta_update(unsigned int dnr)
{
    static BYTE old_output[2];
    BYTE output, input;

    output = (tiatcbm[dnr].dataa & tiatcbm[dnr].ddra)
             | ~tiatcbm[dnr].ddra;
    tcbm_outputa[dnr] = output;
    tcbm_update_bus();
    input = tcbm_busa[dnr];

    tiatcbm[dnr].dataa = (tiatcbm[dnr].dataa & tiatcbm[dnr].ddra)
                         | (input & ~tiatcbm[dnr].ddra);
    old_output[dnr] = output;
}

inline void tiatcbm_portb_update(unsigned int dnr)
{
    static BYTE old_output[2];
    BYTE output, input;

    output = (tiatcbm[dnr].datab & tiatcbm[dnr].ddrb)
             | ~tiatcbm[dnr].ddrb;
    tcbm_outputb[dnr] = output;
    tcbm_update_bus();
    input = tcbm_busb[dnr];

    tiatcbm[dnr].datab = (tiatcbm[dnr].datab & tiatcbm[dnr].ddrb)
                         | (input & ~tiatcbm[dnr].ddrb);
    old_output[dnr] = output;
}

inline void tiatcbm_portc_update(unsigned int dnr)
{
    static BYTE old_output[2];
    BYTE output, input;

    output = (tiatcbm[dnr].datac & tiatcbm[dnr].ddrc)
             | ~tiatcbm[dnr].ddrc;
    tcbm_outputc[dnr] = output;
    tcbm_update_bus();
    input = tcbm_busc[dnr];

    tiatcbm[dnr].datac = (tiatcbm[dnr].datac & tiatcbm[dnr].ddrc)
                         | (input & ~tiatcbm[dnr].ddrc);
    old_output[dnr] = output;
}

inline static BYTE dataa_read(unsigned int dnr)
{
    tiatcbm_porta_update(dnr);
#ifdef TCBM_DEBUG
    printf("TCBM PA READ %02x\n", tiatcbm[dnr].dataa);
#endif
    return tiatcbm[dnr].dataa;
}

inline static BYTE datab_read(unsigned int dnr)
{
    tiatcbm_portb_update(dnr);
#ifdef TCBM_DEBUG
    printf("TCBM PB READ %02x\n", tiatcbm[dnr].datab);
#endif
    return tiatcbm[dnr].datab;
}

inline static BYTE datac_read(unsigned int dnr)
{
    tiatcbm_portc_update(dnr);
#ifdef TCBM_DEBUG
    printf("TCBM PC READ %02x\n", tiatcbm[dnr].datac);
#endif
    return tiatcbm[dnr].datac;
}

inline static BYTE ddra_read(unsigned int dnr)
{
    tiatcbm_porta_update(dnr);
    return tiatcbm[dnr].ddra;
}

inline static BYTE ddrb_read(unsigned int dnr)
{
    tiatcbm_portb_update(dnr);
    return tiatcbm[dnr].ddrb;
}

inline static BYTE ddrc_read(unsigned int dnr)
{
    tiatcbm_portc_update(dnr);
    return tiatcbm[dnr].ddrc;
}

inline static void dataa_store(BYTE byte, unsigned int dnr)
{
#ifdef TCBM_DEBUG
    printf("TCBM PA STORE %02x\n", byte);
#endif
    tiatcbm[dnr].dataa = byte;
    tiatcbm_porta_update(dnr);
}

inline static void datab_store(BYTE byte, unsigned int dnr)
{
#ifdef TCBM_DEBUG
    printf("TCBM PB STORE %02x\n", byte);
#endif
    tiatcbm[dnr].datab = byte;
    tiatcbm_portb_update(dnr);
}

inline static void datac_store(BYTE byte, unsigned int dnr)
{
#ifdef TCBM_DEBUG
    printf("TCBM PC STORE %02x\n", byte);
#endif
    tiatcbm[dnr].datac = byte;
    tiatcbm_portc_update(dnr);
}

inline static void ddra_store(BYTE byte, unsigned int dnr)
{
    tiatcbm[dnr].ddra = byte;
    tiatcbm_porta_update(dnr);
}

inline static void ddrb_store(BYTE byte, unsigned int dnr)
{
    tiatcbm[dnr].ddrb = byte;
    tiatcbm_portb_update(dnr);
}

inline static void ddrc_store(BYTE byte, unsigned int dnr)
{
    tiatcbm[dnr].ddrc = byte;
    tiatcbm_portc_update(dnr);
}

/*-----------------------------------------------------------------------*/

static void tiatcbm_reset(unsigned int dnr)
{
}

static void tiatcbm_store(ADDRESS addr, BYTE byte, unsigned int dnr)
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

static BYTE tiatcbm_read(ADDRESS addr, unsigned int dnr)
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

/*-----------------------------------------------------------------------*/

BYTE REGPARM1 tcbm1_read(ADDRESS addr)
{
    if (drive[0].enable && drive[0].type == DRIVE_TYPE_1551) {
        drive0_cpu_execute(maincpu_clk);
        return tiatcbm_read(addr, 0);
    }
    return 0;
}

void REGPARM2 tcbm1_store(ADDRESS addr, BYTE value)
{
    if (drive[0].enable && drive[0].type == DRIVE_TYPE_1551) {
        drive0_cpu_execute(maincpu_clk);
        tiatcbm_store(addr, value, 0);
    }
}

BYTE REGPARM1 tcbm2_read(ADDRESS addr)
{
    if (drive[1].enable && drive[1].type == DRIVE_TYPE_1551) {
        drive1_cpu_execute(maincpu_clk);
        return tiatcbm_read(addr, 1);
    }
    return 0;
}

void REGPARM2 tcbm2_store(ADDRESS addr, BYTE value)
{
    if (drive[1].enable && drive[1].type == DRIVE_TYPE_1551) {
        drive1_cpu_execute(maincpu_clk);
        tiatcbm_store(addr, value, 1);
    }
}

