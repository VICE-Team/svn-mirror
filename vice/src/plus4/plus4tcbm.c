/*
 * plus4tcbm.c
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

#include "drive.h"
#include "drivetypes.h"
#include "iecdrive.h"
#include "log.h"
#include "maincpu.h"
#include "plus4tcbm.h"
#include "tcbm.h"
#include "types.h"


/*#define TCBM_DEBUG*/


typedef struct tiatcbm_s {
    uint8_t ddra;
    uint8_t dataa;
    uint8_t ddrb;
    uint8_t datab;
    uint8_t ddrc;
    uint8_t datac;
} tiatcbm_t;

static tiatcbm_t tiatcbm[2];

uint8_t plus4tcbm_outputa[2], plus4tcbm_outputb[2], plus4tcbm_outputc[2];
uint8_t tpid_outputa[2], tpid_outputb[2], tpid_outputc[2];

/*-----------------------------------------------------------------------*/

void plus4tcbm_update_pa(uint8_t byte, unsigned int dnr)
{
    tpid_outputa[dnr] = byte;
}

void plus4tcbm_update_pb(uint8_t byte, unsigned int dnr)
{
    tpid_outputb[dnr] = byte;
}

void plus4tcbm_update_pc(uint8_t byte, unsigned int dnr)
{
    tpid_outputc[dnr] = byte;
}

/*-----------------------------------------------------------------------*/

inline static uint8_t dataa_read(unsigned int dnr)
{
#ifdef TCBM_DEBUG
    log_debug(LOG_DEFAULT, "TCBM PA READ DATA %02x DDR %02x TIPD %02x",
              tiatcbm[dnr].dataa, tiatcbm[dnr].ddra, tpid_outputa[dnr]);
#endif
    return (tiatcbm[dnr].dataa | ~tiatcbm[dnr].ddra) & tpid_outputa[dnr];
}

inline static uint8_t datab_read(unsigned int dnr)
{
#ifdef TCBM_DEBUG
    log_debug(LOG_DEFAULT, "TCBM PB READ DATA %02x DDR %02x",
              tiatcbm[dnr].datab, tiatcbm[dnr].ddrb);
#endif
    return (tiatcbm[dnr].datab | ~tiatcbm[dnr].ddrb)
           & (tpid_outputc[dnr] | 0xfc);
}

inline static uint8_t datac_read(unsigned int dnr)
{
#ifdef TCBM_DEBUG
    log_debug(LOG_DEFAULT, "TCBM PC READ DATA %02x DDR %02x",
              tiatcbm[dnr].datac, tiatcbm[dnr].ddrc);
#endif
    return (tiatcbm[dnr].datac | ~tiatcbm[dnr].ddrc)
           & ((tpid_outputc[dnr] << 4) | 0x7f)
           & ((tpid_outputc[dnr] >> 1) | 0xbf);
}

inline static void store_pa(unsigned int dnr)
{
#ifdef TCBM_DEBUG
    log_debug(LOG_DEFAULT, "TCBM PA STORE DATA %02x DDR %02x",
              tiatcbm[dnr].dataa, tiatcbm[dnr].ddra);
#endif
    plus4tcbm_outputa[dnr] = tiatcbm[dnr].dataa | ~tiatcbm[dnr].ddra;
}

inline static void store_pb(unsigned int dnr)
{
#ifdef TCBM_DEBUG
    log_debug(LOG_DEFAULT, "TCBM PB STORE DATA %02x DDR %02x",
              tiatcbm[dnr].datab, tiatcbm[dnr].ddrb);
#endif
    plus4tcbm_outputb[dnr] = tiatcbm[dnr].datab | ~tiatcbm[dnr].ddrb;
}

inline static void store_pc(unsigned int dnr)
{
#ifdef TCBM_DEBUG
    log_debug(LOG_DEFAULT, "TCBM PC STORE DATA %02x DDR %02x",
              tiatcbm[dnr].datac, tiatcbm[dnr].ddrc);
#endif

    plus4tcbm_outputc[dnr] = tiatcbm[dnr].datac | ~tiatcbm[dnr].ddrc;
}

/*-----------------------------------------------------------------------*/

static void tiatcbm_reset(unsigned int dnr)
{
    tiatcbm[dnr].ddra = 0;
    tiatcbm[dnr].dataa = 0;
    tiatcbm[dnr].ddrb = 0;
    tiatcbm[dnr].datab = 0;
    tiatcbm[dnr].ddrc = 0;
    tiatcbm[dnr].datac = 0;
    plus4tcbm_outputa[dnr] = 0xff;
    plus4tcbm_outputb[dnr] = 0xff;
    plus4tcbm_outputc[dnr] = 0xff;
}

void plus4tcbm1_reset(void)
{
    tiatcbm_reset(0);
}

void plus4tcbm2_reset(void)
{
    tiatcbm_reset(1);
}

static void tiatcbm_store(uint16_t addr, uint8_t byte, unsigned int dnr)
{
    switch (addr & 7) {
        case 0:
            tiatcbm[dnr].dataa = byte;
            store_pa(dnr);
            break;
        case 1:
            tiatcbm[dnr].datab = byte;
            store_pb(dnr);
            break;
        case 2:
            tiatcbm[dnr].datac = byte;
            store_pc(dnr);
            break;
        case 3:
            tiatcbm[dnr].ddra = byte;
            store_pa(dnr);
            break;
        case 4:
            tiatcbm[dnr].ddrb = byte;
            store_pb(dnr);
            break;
        case 5:
            tiatcbm[dnr].ddrc = byte;
            store_pc(dnr);
            break;
    }
}

static uint8_t tiatcbm_read(uint16_t addr, unsigned int dnr)
{
    switch (addr & 7) {
        case 0:
            return dataa_read(dnr);
        case 1:
            return datab_read(dnr);
        case 2:
            return datac_read(dnr);
        case 3:
            return tiatcbm[dnr].ddra;
        case 4:
            return tiatcbm[dnr].ddrb;
        case 5:
            return tiatcbm[dnr].ddrc;
    }

    return 0xff;
}

/*-----------------------------------------------------------------------*/

uint8_t plus4tcbm1_read(uint16_t addr)
{
    if (diskunit_context[0]->enable
        && diskunit_context[0]->type == DRIVE_TYPE_1551) {
        drive_cpu_execute_one(diskunit_context[0], maincpu_clk);
        return tiatcbm_read(addr, 0);
    }
    return 0;
}

void plus4tcbm1_store(uint16_t addr, uint8_t value)
{
    if (diskunit_context[0]->enable
        && diskunit_context[0]->type == DRIVE_TYPE_1551) {
        drive_cpu_execute_one(diskunit_context[0], maincpu_clk);
        tiatcbm_store(addr, value, 0);
    }
}

uint8_t plus4tcbm2_read(uint16_t addr)
{
    if (diskunit_context[1]->enable
        && diskunit_context[1]->type == DRIVE_TYPE_1551) {
        drive_cpu_execute_one(diskunit_context[1], maincpu_clk);
        return tiatcbm_read(addr, 1);
    }
    return 0;
}

void plus4tcbm2_store(uint16_t addr, uint8_t value)
{
    if (diskunit_context[1]->enable
        && diskunit_context[1]->type == DRIVE_TYPE_1551) {
        drive_cpu_execute_one(diskunit_context[1], maincpu_clk);
        tiatcbm_store(addr, value, 1);
    }
}
