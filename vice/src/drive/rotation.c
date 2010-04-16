/*
 * rotation.c
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
#include "rotation.h"
#include "types.h"


/*#define NEW_SYNC*/

#define ACCUM_MAX 0x10000

#define ROTATION_TABLE_SIZE 0x1000


struct rotation_s {
    unsigned long accum;
    CLOCK rotation_last_clk;

    unsigned int last_read_data;
    BYTE last_write_data;
    int bit_counter;

    int frequency; /* 1x/2x speed toggle, index to rot_speed_bps */
    int speed_zone; /* speed zone within rot_speed_bps */

    unsigned long seed;
};
typedef struct rotation_s rotation_t;


static rotation_t rotation[DRIVE_NUM];

/* Speed (in bps) of the disk in the 4 disk areas.  */
static const int rot_speed_bps[2][4] = { { 250000, 266667, 285714, 307692 },
                                         { 125000, 133333, 142857, 153846 } };


void rotation_init(int freq, unsigned int dnr)
{
    rotation[dnr].frequency = freq;
    rotation[dnr].accum = 0;
}

void rotation_reset(drive_t *drive)
{
    unsigned int dnr;

    dnr = drive->mynumber;

    rotation[dnr].last_read_data = 0;
    rotation[dnr].last_write_data = 0;
    rotation[dnr].bit_counter = 0;
    rotation[dnr].accum = 0;
    rotation[dnr].seed = 0;
    rotation[dnr].rotation_last_clk = *(drive->clk);
}

void rotation_speed_zone_set(unsigned int zone, unsigned int dnr)
{
    rotation[dnr].speed_zone = zone;
}

void rotation_table_get(DWORD *rotation_table_ptr)
{
    unsigned int dnr;
    drive_t *drive;

    for (dnr = 0; dnr < DRIVE_NUM; dnr++) {
        drive = drive_context[dnr]->drive;

        rotation_table_ptr[dnr] = rotation[dnr].speed_zone;

        drive->snap_accum = rotation[dnr].accum;
        drive->snap_rotation_last_clk = rotation[dnr].rotation_last_clk;
        drive->snap_last_read_data = rotation[dnr].last_read_data;
        drive->snap_last_write_data = rotation[dnr].last_write_data;
        drive->snap_bit_counter = rotation[dnr].bit_counter;
        drive->snap_seed = rotation[dnr].seed;
    }
}

void rotation_table_set(DWORD *rotation_table_ptr)
{
    unsigned int dnr;
    drive_t *drive;

    for (dnr = 0; dnr < DRIVE_NUM; dnr++) {
        drive = drive_context[dnr]->drive;

        rotation[dnr].speed_zone = rotation_table_ptr[dnr];

        rotation[dnr].accum = drive->snap_accum;
        rotation[dnr].rotation_last_clk = drive->snap_rotation_last_clk;
        rotation[dnr].last_read_data = drive->snap_last_read_data;
        rotation[dnr].last_write_data = drive->snap_last_write_data;
        rotation[dnr].bit_counter = drive->snap_bit_counter;
        rotation[dnr].seed = drive->snap_seed;
    }
}

void rotation_overflow_callback(CLOCK sub, unsigned int dnr)
{
    rotation[dnr].rotation_last_clk -= sub;
}

inline static void write_next_bit(drive_t *dptr, int value)
{
    int off = dptr->GCR_head_offset;
    int byte_offset = off >> 3;
    int bit = (~off) & 7;
    dptr->GCR_head_offset = (off + 1) % (dptr->GCR_current_track_size << 3);

    if (value) {
        dptr->GCR_track_start_ptr[byte_offset] |= 1 << bit;
    } else {
        dptr->GCR_track_start_ptr[byte_offset] &= ~(1 << bit);
    }
}

inline static int read_next_bit(drive_t *dptr)
{
    int off = dptr->GCR_head_offset;
    int byte_offset = off >> 3;
    int bit = (~off) & 7;
    dptr->GCR_head_offset = (off + 1) % (dptr->GCR_current_track_size << 3);

    return (dptr->GCR_track_start_ptr[byte_offset] >> bit) & 1;
}

inline static long RANDOM_nextInt(rotation_t *rptr) {
    int bits = rptr->seed >> 15;
    rptr->seed ^= rptr->accum;
    rptr->seed = rptr->seed << 17 | bits;
    return (long) rptr->seed;
}

void rotation_begins(drive_t *dptr) {
    unsigned int dnr = dptr->mynumber;
    rotation[dnr].rotation_last_clk = *(dptr->clk);
}

/* Rotate the disk according to the current value of `drive_clk[]'.  If
   `mode_change' is non-zero, there has been a Read -> Write mode switch.  */
void rotation_rotate_disk(drive_t *dptr)
{
    rotation_t *rptr;
    CLOCK delta;
    int tdelta;
    int bits_moved = 0;

    rptr = &rotation[dptr->mynumber];

    /* Calculate the number of bits that have passed under the R/W head since
       the last time.  */
    delta = *(dptr->clk) - rptr->rotation_last_clk;
    rptr->rotation_last_clk = *(dptr->clk);

    while (delta > 0) {
        tdelta = delta > 1000 ? 1000 : delta;
        delta -= tdelta;

        rptr->accum += rot_speed_bps[rptr->frequency][rptr->speed_zone] * tdelta;
        bits_moved += rptr->accum / 1000000;
        rptr->accum %= 1000000;
    }

    if (dptr->read_write_mode) {
        while (bits_moved -- != 0) {
            rptr->last_read_data = ((rptr->last_read_data << 1) & 0x3fe) | read_next_bit(dptr);
            /* GCR=0 support.
             * 
             * In the absence of 1-bits (magnetic flux changes), the drive
             * will use a timer counter to count how many 0s it has read. Every
             * 4 read bits, it will detect a 1-bit, because it doesn't
             * distinguish between reset occuring from magnetic flux or regular
             * wraparound.
             * 
             * Random magnetic flux events can also occur after GCR data has been
             * quiet for a long time, for at least 4 bits. So the first value
             * read will always be 1. Afterwards, the 0-bit sequence lengths
             * vary randomly, but can never exceed 3.
             * 
             * Each time a random event happens, it tends to advance the bit counter
             * by half a clock, because the random event can occur at any time
             * and thus the expectation value is that it occurs at 50 % point
             * within the bitcells.
             * 
             * Additionally, the underlying disk rotation has no way to keep in sync
             * with the electronics, so the bitstream after a GCR=0 may or may not
             * be shifted with respect to the bit counter by the time drive
             * encounters it. This situation will persist until the next sync
             * sequence. There is no specific emulation for variable disk rotation,
             * this case is thought to be covered by the random event handling.
             * 
             * Here's some genuine 1541 patterns for reference:
             * 
             * 53 12 46 22 24 AA AA AA AA AA AA AA A8 AA AA AA
             * 53 11 11 11 14 AA AA AA AA AA AA AA A8 AA AA AA
             * 53 12 46 22 24 AA AA AA AA AA AA AA A8 AA AA AA
             * 53 12 22 24 45 2A AA AA AA AA AA AA AA 2A AA AA
             * 53 11 52 22 24 AA AA AA AA AA AA AA A8 AA AA AA
             */

            /* Simulate random magnetic flux events in our lame-ass emulation. */
            if ((rptr->last_read_data & 0x3f) == 0x8 && RANDOM_nextInt(rptr) > (1 << 30)) {
                rptr->last_read_data |= 1;
                /*
                 * Simulate loss of sync against the underlying platter.
                 * Whenever 1-bits occur, there's a chance that they occured
                 * due to a random magnetic flux event, and can thus occur
                 * at any phase of the bit-cell clock.
                 * 
                 * It follows, therefore, that such events have a chance to
                 * advance the bit_counter by about 0,5 clocks each time they
                 * occur. Hence > 0 here, which filters out 50 % of events.
                 */
                if (rptr->bit_counter < 7 && RANDOM_nextInt(rptr) > 0) {
                    rptr->bit_counter ++;
                    rptr->last_read_data = (rptr->last_read_data << 1) & 0x3fe;
                }
            } else if ((rptr->last_read_data & 0xf) == 0) {
                /* Simulate clock reset */
                rptr->last_read_data |= 1;
            }
            rptr->last_write_data <<= 1;

            /* is sync? reset bit counter, don't move data, etc. */
            if (rptr->last_read_data == 0x3ff) {
                rptr->bit_counter = 0;
            } else {
                if (++ rptr->bit_counter == 8) {
                    rptr->bit_counter = 0;
                    dptr->GCR_read = (BYTE) rptr->last_read_data;
                    /* tlr claims that the write register is loaded at every
                     * byte boundary, and since the bus is shared, it's reasonable
                     * to guess that it would be loaded with whatever was last read. */
                    rptr->last_write_data = dptr->GCR_read;
                    dptr->byte_ready_edge = 1;
                    dptr->byte_ready_level = 1;
                }
            }
        }
    } else {
        /* When writing, the first byte after transition is going to echo the
         * bits from the last read value.
         */
        while (bits_moved -- != 0) {
            rptr->last_read_data = (rptr->last_read_data << 1) & 0x3fe;
            if ((rptr->last_read_data & 0xf) == 0) {
                rptr->last_read_data |= 1;
            }
                
            dptr->GCR_dirty_track = 1;
            write_next_bit(dptr, rptr->last_write_data & 0x80);
            rptr->last_write_data <<= 1;

            if (++ rptr->bit_counter == 8) {
                rptr->bit_counter = 0;
                rptr->last_write_data = dptr->GCR_write_value;
                dptr->byte_ready_edge = 1;
                dptr->byte_ready_level = 1;
            }
        }
    }
}

/* Return non-zero if the Sync mark is found.  It is required to
   call rotation_rotate_disk() to update drive[].GCR_head_offset first.
   The return value corresponds to bit#7 of VIA2 PRB. This means 0x0
   is returned when sync is found and 0x80 is returned when no sync
   is found.  */
BYTE rotation_sync_found(drive_t *dptr)
{
    unsigned int dnr = dptr->mynumber;

    if (dptr->read_write_mode == 0 || dptr->attach_clk != (CLOCK)0)
        return 0x80;

    return rotation[dnr].last_read_data == 0x3ff ? 0 : 0x80;
}

void rotation_byte_read(drive_t *dptr)
{
    if (dptr->attach_clk != (CLOCK)0) {
        if (*(dptr->clk) - dptr->attach_clk < DRIVE_ATTACH_DELAY)
            dptr->GCR_read = 0;
        else
            dptr->attach_clk = (CLOCK)0;
    } else if (dptr->attach_detach_clk != (CLOCK)0) {
        if (*(dptr->clk) - dptr->attach_detach_clk < DRIVE_ATTACH_DETACH_DELAY)
            dptr->GCR_read = 0;
        else
            dptr->attach_detach_clk = (CLOCK)0;
    } else {
        if (dptr->byte_ready_active == 0x06)
            rotation_rotate_disk(dptr);
    }
}

