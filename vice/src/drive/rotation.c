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
#include "rotation.h"
#include "types.h"


#define ACCUM_MAX           0x10000

#define ROTATION_TABLE_SIZE 0x1000


struct rotation_table_s {
    unsigned long bits;
    unsigned long accum;
};
typedef struct rotation_table_s rotation_table_t;

struct rotation_s {
    rotation_table_t *rotation_table_ptr;
    rotation_table_t rotation_table[4][ROTATION_TABLE_SIZE];
    unsigned long bits_moved;
    unsigned long accum;
    unsigned long shifter;
    int finish_byte;
    int last_mode;
    CLOCK rotation_last_clk;
};
typedef struct rotation_s rotation_t;


static rotation_t rotation[2];

/* Speed (in bps) of the disk in the 4 disk areas.  */
static int rot_speed_bps[2][4] = { { 250000, 266667, 285714, 307692 },
                                   { 125000, 133333, 142857, 153846 } };


void rotation_init(int freq, unsigned int dnr)
{
    rotation_init_table(freq, dnr);
    rotation[dnr].bits_moved = rotation[dnr].accum = 0;
}

void rotation_init_table(int freq, unsigned int dnr)
{
    int i, j;

    for (i = 0; i < 4; i++) {
        int speed = rot_speed_bps[freq][i];

        for (j = 0; j < ROTATION_TABLE_SIZE; j++) {
            double bits = (double)j * (double)speed / 1000000.0;

            rotation[dnr].rotation_table[i][j].bits = (unsigned long)bits;
            rotation[dnr].rotation_table[i][j].accum
                = (unsigned long)(((bits - (unsigned long)bits) * ACCUM_MAX));
        }
    }
}

void rotation_reset(unsigned int dnr)
{
    rotation[dnr].rotation_table_ptr = rotation[dnr].rotation_table[0];
    rotation[dnr].accum = 0;
    rotation[dnr].bits_moved = 0;
    rotation[dnr].finish_byte = 0;
    rotation[dnr].last_mode = 1;
    rotation[dnr].rotation_last_clk = *(drive[dnr].clk);
}

void rotation_speed_zone_set(unsigned int zone, unsigned int dnr)
{
    rotation[dnr].rotation_table_ptr = rotation[dnr].rotation_table[zone];
}

void rotation_table_get(DWORD *rotation_table_ptr)
{
    unsigned int i;

    for (i = 0; i < 2; i++) {
        rotation_table_ptr[i] = (DWORD)(rotation[i].rotation_table_ptr
                                - rotation[i].rotation_table[0]);

        drive[i].snap_accum = rotation[i].accum;
        drive[i].snap_bits_moved = rotation[i].bits_moved;
        drive[i].snap_finish_byte = rotation[i].finish_byte;
        drive[i].snap_last_mode = rotation[i].last_mode;
        drive[i].snap_rotation_last_clk = rotation[i].rotation_last_clk;
    }
}

void rotation_table_set(DWORD *rotation_table_ptr)
{
    unsigned int i;

    for (i = 0; i < 2; i++) {
        rotation[i].rotation_table_ptr = rotation[i].rotation_table[0]
            + rotation_table_ptr[i];

        rotation[i].accum = drive[i].snap_accum;
        rotation[i].bits_moved = drive[i].snap_bits_moved;
        rotation[i].finish_byte = drive[i].snap_finish_byte;
        rotation[i].last_mode = drive[i].snap_last_mode;
        rotation[i].rotation_last_clk = drive[i].snap_rotation_last_clk;

        rotation[i].shifter = rotation[i].bits_moved;
    }
}

void rotation_overflow_callback(CLOCK sub, unsigned int dnr)
{
    rotation[dnr].rotation_last_clk -= sub;
}

void rotation_change_mode(unsigned int dnr)
{
    rotation[dnr].finish_byte = 1;
}

/* Rotate the disk according to the current value of `drive_clk[]'.  If
   `mode_change' is non-zero, there has been a Read -> Write mode switch.  */
void rotation_rotate_disk(drive_t *dptr)
{
    rotation_t *rptr;
    unsigned long new_bits;
    CLOCK delta;

    rptr = &rotation[dptr->mynumber];

    /* Calculate the number of bits that have passed under the R/W head since
       the last time.  */
    delta = *(dptr->clk) - rptr->rotation_last_clk;
    new_bits = 0;

    while (delta > 0) {
        rotation_table_t *p;

        if (delta >= ROTATION_TABLE_SIZE) {
            p = (rptr->rotation_table_ptr + ROTATION_TABLE_SIZE - 1);
            delta -= ROTATION_TABLE_SIZE - 1;
        } else {
            p = rptr->rotation_table_ptr + delta;
            delta = 0;
        }

        new_bits += p->bits;
        rptr->accum += p->accum;

        if (rptr->accum >= ACCUM_MAX) {
            rptr->accum -= ACCUM_MAX;
            new_bits++;
        }
    }

    rptr->shifter = rptr->bits_moved + new_bits;

    if (rptr->shifter >= 8) {
        rptr->bits_moved += new_bits;
        rptr->rotation_last_clk = *(dptr->clk);

        if (rptr->finish_byte) {
            if (rptr->last_mode == 0) { /* write */
                dptr->GCR_dirty_track = 1;
                if (rptr->bits_moved >= 8) {
                    dptr->GCR_track_start_ptr[dptr->GCR_head_offset]
                        = dptr->GCR_write_value;
                    dptr->GCR_head_offset = ((dptr->GCR_head_offset + 1) %
                                            dptr->GCR_current_track_size);
                    rptr->bits_moved -= 8;
                }
            } else {            /* read */
                if (rptr->bits_moved >= 8) {
                    dptr->GCR_head_offset = ((dptr->GCR_head_offset + 1) %
                                            dptr->GCR_current_track_size);
                    rptr->bits_moved -= 8;
                    dptr->GCR_read
                        = dptr->GCR_track_start_ptr[dptr->GCR_head_offset];
                }
            }

            rptr->finish_byte = 0;
            rptr->last_mode = dptr->read_write_mode;
        }

        if (rptr->last_mode == 0) {     /* write */
            dptr->GCR_dirty_track = 1;
            while (rptr->bits_moved >= 8) {
                dptr->GCR_track_start_ptr[dptr->GCR_head_offset]
                    = dptr->GCR_write_value;
                dptr->GCR_head_offset = ((dptr->GCR_head_offset + 1)
                                         % dptr->GCR_current_track_size);
                rptr->bits_moved -= 8;
            }
        } else {                /* read */
            dptr->GCR_head_offset = ((dptr->GCR_head_offset
                                     + rptr->bits_moved / 8)
                                     % dptr->GCR_current_track_size);
            rptr->bits_moved %= 8;
            dptr->GCR_read = dptr->GCR_track_start_ptr[dptr->GCR_head_offset];
        }

        rptr->shifter = rptr->bits_moved;

        /* The byte ready line is only set when no sync is found.  */
        if (rotation_sync_found(dptr)) {
            dptr->byte_ready_level = 1;
            dptr->byte_ready_edge = 1;
        }
    } /* if (rptr->shifter >= 8) */
}

/* Return non-zero if the Sync mark is found.  It is required to
   call rotation_rotate_disk() to update drive[].GCR_head_offset first.
   The return value corresponds to bit#7 of VIA2 PRB. This means 0x0
   is returned when sync is found and 0x80 is returned when no sync
   is found.  */
BYTE rotation_sync_found(drive_t *dptr)
{
    unsigned int dnr;
    BYTE val;

    dnr = dptr->mynumber;
    val = dptr->GCR_track_start_ptr[dptr->GCR_head_offset];

    if (val != 0xff || rotation[dnr].last_mode == 0
        || dptr->attach_clk != (CLOCK)0) {
        return 0x80;
    } else {
        unsigned int previous_head_offset;

        previous_head_offset = (dptr->GCR_head_offset > 0
                               ? dptr->GCR_head_offset - 1
                               : dptr->GCR_current_track_size - 1);

        if ((dptr->GCR_track_start_ptr[previous_head_offset] & 3) != 3) {
            if (rotation[dnr].shifter >= 2) {
                unsigned int next_head_offset;

                next_head_offset = ((dptr->GCR_head_offset
                                   < (dptr->GCR_current_track_size - 1))
                                   ? dptr->GCR_head_offset + 1 : 0);

                if ((dptr->GCR_track_start_ptr[next_head_offset] & 0xc0)
                    == 0xc0)
                    return 0;
            }
            return 0x80;
        }
        /* As the current rotation code cannot cope with non byte aligned
           writes, do not change `drive[].bits_moved'!  */
        /* dptr->bits_moved = 0; */
        return 0;
    }
}

