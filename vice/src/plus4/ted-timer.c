/*
 * ted-timer.c - Timer implementation for the TED emulation.
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

#include "alarm.h"
#include "maincpu.h"
#include "ted-timer.h"
#include "tedtypes.h"
#include "types.h"


static alarm_t ted_t1_alarm;
static alarm_t ted_t2_alarm;
static alarm_t ted_t3_alarm;

static unsigned int t1_start;
static unsigned int t2_start;
static unsigned int t3_start;

/*
static unsigned int t1_running;
static unsigned int t2_running;
static unsigned int t3_running;
*/

static unsigned int t1_value;
static unsigned int t2_value;
static unsigned int t3_value;

/*-----------------------------------------------------------------------*/

static void ted_t1(CLOCK offset)
{
    alarm_set(&ted_t1_alarm, clk + (t1_start == 0 ? 65536 : t1_start) - offset);
/*printf("TI1 ALARM %x\n", clk);*/
    ted.irq_status |= 0x08;
}

static void ted_t2(CLOCK offset)
{
    alarm_set(&ted_t2_alarm, clk + 65536 - offset);
    t2_start = 0;
/*printf("TI2 ALARM %x\n", clk);*/
    ted.irq_status |= 0x10;
}

static void ted_t3(CLOCK offset)
{
    alarm_set(&ted_t3_alarm, clk + 65536 - offset);
    t3_start = 0;
/*printf("TI3 ALARM\n");*/
    ted.irq_status |= 0x40;
}

/*-----------------------------------------------------------------------*/

static void ted_timer_t1_store_low(BYTE value)
{
    alarm_unset(&ted_t1_alarm);
    t1_value = t1_start = (t1_start & 0xff00) | value;
}

static void ted_timer_t1_store_high(BYTE value)
{
    alarm_unset(&ted_t1_alarm);
    t1_value = t1_start = (t1_start & 0x00ff) | (value << 8);
    alarm_set(&ted_t1_alarm, clk + (t1_start == 0 ? 65536 : t1_start));
}

static void ted_timer_t2_store_low(BYTE value)
{
    alarm_unset(&ted_t2_alarm);
    t2_value = t2_start = (t2_start & 0xff00) | value;
}

static void ted_timer_t2_store_high(BYTE value)
{
    alarm_unset(&ted_t2_alarm);
    t2_value = t2_start = (t2_start & 0x00ff) | (value << 8);
    alarm_set(&ted_t2_alarm, clk + (t2_start == 0 ? 65536 : t2_start));
}

static void ted_timer_t3_store_low(BYTE value)
{
    alarm_unset(&ted_t3_alarm);
    t3_value = t3_start = (t3_start & 0xff00) | value;
}

static void ted_timer_t3_store_high(BYTE value)
{
    alarm_unset(&ted_t3_alarm);
    t3_value = t3_start = (t3_start & 0x00ff) | (value << 8);
    alarm_set(&ted_t3_alarm, clk + (t3_start == 0 ? 65536 : t3_start));
}

static BYTE ted_timer_t1_read_low(void)
{
/*printf("TI1 READL %02x\n", t1_value & 0xff);*/
    return (BYTE)(t1_value & 0xff);
}

static BYTE ted_timer_t1_read_high(void)
{
/*printf("TI1 READH %02x\n", t1_value >> 8);*/
    return (BYTE)(t1_value >> 8);
}

static BYTE ted_timer_t2_read_low(void)
{
/*printf("TI2 READL %02x\n", t2_value & 0xff);*/
    return (BYTE)(t2_value & 0xff);
}

static BYTE ted_timer_t2_read_high(void)
{
/*printf("TI2 READH %02x\n", t2_value >> 8);*/
    return (BYTE)(t2_value >> 8);
}

static BYTE ted_timer_t3_read_low(void)
{
/*printf("TI3 READL %02x\n", t3_value & 0xff);*/
    return (BYTE)(t3_value & 0xff);
}

static BYTE ted_timer_t3_read_high(void)
{
/*printf("TI3 READH %02x\n", t3_value >> 8);*/
    return (BYTE)(t3_value >> 8);
}

/*-----------------------------------------------------------------------*/

void REGPARM2 ted_timer_store(ADDRESS addr, BYTE value)
{
/*printf("TI STORE %02x %02x CLK %x\n", addr, value, clk);*/
    switch (addr) {
      case 0:
        ted_timer_t1_store_low(value);
        break;
      case 1:
        ted_timer_t1_store_high(value);
        break;
      case 2:
        ted_timer_t2_store_low(value);
        break;
      case 3:
        ted_timer_t2_store_high(value);
        break;
      case 4:
        ted_timer_t3_store_low(value);
        break;
      case 5:
        ted_timer_t3_store_high(value);
        break;
    }
}

BYTE REGPARM1 ted_timer_read(ADDRESS addr)
{
    switch (addr) {
      case 0:
        return ted_timer_t1_read_low();
      case 1:
        return ted_timer_t1_read_high();
      case 2:
        return ted_timer_t2_read_low();
      case 3:
        return ted_timer_t2_read_high();
      case 4:
        return ted_timer_t3_read_low();
      case 5:
        return ted_timer_t3_read_high();
    }
    return 0;
}

void ted_timer_init(void)
{
    alarm_init(&ted_t1_alarm, maincpu_alarm_context, "TED T1", ted_t1);
    alarm_init(&ted_t2_alarm, maincpu_alarm_context, "TED T2", ted_t2);
    alarm_init(&ted_t3_alarm, maincpu_alarm_context, "TED T3", ted_t3);
}

void ted_timer_reset(void)
{
    alarm_unset(&ted_t1_alarm);
    alarm_unset(&ted_t2_alarm);
    alarm_unset(&ted_t3_alarm);
}

