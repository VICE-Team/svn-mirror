/*
 *
 * cia1581drive1.c - Definitions for the MOS6526 (CIA) chip in the 1581
 * disk drive ($4000).  Notice that the real 1581 uses a 8520 CIA.
 *
 * Written by
 *   Andreas Boose (boose@linux.rz.fh-hannover.de)
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
 * */

#include "ciacore.h"

/* set mycia_debugFlag to 1 to get output */
#undef CIA_TIMER_DEBUG

/*************************************************************************
 * Renaming exported functions
 */

#define mycia_init cia1581d1_init
#define reset_mycia reset_cia1581d1
#define store_mycia store_cia1581d1
#define read_mycia read_cia1581d1
#define peek_mycia peek_cia1581d1
#define mycia_prevent_clk_overflow cia1581d1_prevent_clk_overflow
#define mycia_set_flag cia1581d1_set_flag
#define mycia_set_sdr cia1581d1_set_sdr
#define mycia_write_snapshot_module cia1581d1_write_snapshot_module
#define mycia_read_snapshot_module cia1581d1_read_snapshot_module
#define mycia_debugFlag cia1581d1_debugFlag

#define MYCIA_NAME "CIA1581D1"

/*************************************************************************
 * CPU binding
 */

#include "drive.h"
#include "ciad.h"
#include "iecdrive.h"

#include "interrupt.h"

#define MYCIA_INT       IK_IRQ

#define myclk drive_clk[1]

#define cia_set_int_clk(value,clk) \
                set_int(&drive1_int_status,(I_CIA1FL),(value),(clk))

#define cia_restore_int(value) \
                set_int_noclk(&drive1_int_status,(I_CIA1FL),(value))

#define mycpu_alarm_context drive1_alarm_context

static iec_info_t *iec_info;

static inline void do_reset_cia(void)
{
    iec_info = iec_get_drive_port();
}

#define PRE_STORE_CIA
#define PRE_READ_CIA
#define PRE_PEEK_CIA

static inline void store_sdr(BYTE byte)
{
    iec_fast_drive_write(cia[CIA_SDR]);
}

static inline void undump_ciapa(CLOCK rclk, BYTE b)
{
    drive[1].led_status = b & 0x40;
}

static inline void undump_ciapb(CLOCK rclk, BYTE b)
{

}

static inline void store_ciapa(ADDRESS addr, CLOCK rclk, BYTE byte)
{
    drive[1].led_status = byte & 0x40;
}

static inline void store_ciapb(ADDRESS addr, CLOCK rclk, BYTE byte)
{
    if (byte != oldpb) {
        if (iec_info != NULL) {
            iec_info->drive2_data = ~byte;
            iec_info->drive2_bus = (((iec_info->drive2_data << 3) & 0x40)
                | ((iec_info->drive2_data << 6)
                & ((iec_info->drive2_data | iec_info->cpu_bus) << 3) & 0x80));
            iec_info->cpu_port = iec_info->cpu_bus & iec_info->drive2_bus
                & iec_info->drive_bus;
            iec_info->drive2_port = iec_info->drive_port = (((iec_info->cpu_port >> 4) & 0x4)
                | (iec_info->cpu_port >> 7)
                | ((iec_info->cpu_bus << 3) & 0x80));
        } else {
            iec_drive1_write(~byte);
        }
    }
}

static inline BYTE read_ciapa(void)
{
    return (8 & ~cia[CIA_DDRA]) | (cia[CIA_PRA] & cia[CIA_DDRA]);
}

static inline BYTE read_ciapb(void)
{
    if (iec_info != NULL)
        return ((cia[CIA_PRB] & 0x1a) | iec_info->drive2_port) ^ 0x85;
    else
        return ((cia[CIA_PRB] & 0x1a) | iec_drive1_read()) ^ 0x85;
}

static inline void read_ciaicr(void)
{

}

#include "ciacore.c"

/* POST_CIA_FUNCS */

