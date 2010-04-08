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
    unsigned long bits_moved;
    unsigned long accum;
    int finish_byte;
    int last_mode;
	int frequency; /* 1x/2x speed toggle, index to rot_speed_bps */
	int speed_zone; /* speed zone within rot_speed_bps */
    CLOCK rotation_last_clk;
};
typedef struct rotation_s rotation_t;


static rotation_t rotation[DRIVE_NUM];

/* Speed (in bps) of the disk in the 4 disk areas.  */
static const int rot_speed_bps[2][4] = { { 250000, 266667, 285714, 307692 },
                                         { 125000, 133333, 142857, 153846 } };


void rotation_init(int freq, unsigned int dnr)
{
	rotation[dnr].frequency = freq;
    rotation[dnr].bits_moved = rotation[dnr].accum = 0;
}

void rotation_reset(drive_t *drive)
{
    unsigned int dnr;

    dnr = drive->mynumber;

    rotation[dnr].accum = 0;
    rotation[dnr].bits_moved = 0;
    rotation[dnr].finish_byte = 0;
    rotation[dnr].last_mode = 1;
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
        drive->snap_bits_moved = rotation[dnr].bits_moved;
        drive->snap_finish_byte = rotation[dnr].finish_byte;
        drive->snap_last_mode = rotation[dnr].last_mode;
        drive->snap_rotation_last_clk = rotation[dnr].rotation_last_clk;
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
        rotation[dnr].bits_moved = drive->snap_bits_moved;
        rotation[dnr].finish_byte = drive->snap_finish_byte;
        rotation[dnr].last_mode = drive->snap_last_mode;
        rotation[dnr].rotation_last_clk = drive->snap_rotation_last_clk;
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

#if 0
inline static unsigned int head_next2_offset(drive_t *dptr)
{
    return (dptr->GCR_head_offset + 2) % dptr->GCR_current_track_size;
}
#endif

inline static unsigned int head_next_offset(drive_t *dptr)
{
    return (dptr->GCR_head_offset + 1) % dptr->GCR_current_track_size;
}

inline static unsigned int head_current_offset(drive_t *dptr)
{
    return dptr->GCR_head_offset;
}

inline static unsigned int head_previous_offset(drive_t *dptr)
{
    return (dptr->GCR_head_offset > 0)
           ? dptr->GCR_head_offset - 1 : dptr->GCR_current_track_size - 1;
}

#if 0
inline static unsigned int head_previous2_offset(drive_t *dptr)
{
    return (dptr->GCR_head_offset + dptr->GCR_current_track_size - 2)
           % dptr->GCR_current_track_size;
}

inline static BYTE peek_next2(drive_t *dptr)
{
    return dptr->GCR_track_start_ptr[head_next2_offset(dptr)];
}
#endif

inline static BYTE peek_next(drive_t *dptr)
{
    return dptr->GCR_track_start_ptr[head_next_offset(dptr)];
}

inline static BYTE peek_current(drive_t *dptr)
{
    return dptr->GCR_track_start_ptr[head_current_offset(dptr)];
}

inline static BYTE peek_previous(drive_t *dptr)
{
    return dptr->GCR_track_start_ptr[head_previous_offset(dptr)];
}

#if 0
inline static BYTE peek_previous2(drive_t *dptr)
{
    return dptr->GCR_track_start_ptr[head_previous2_offset(dptr)];
}
#endif

inline static void data_write(drive_t *dptr)
{
    dptr->GCR_track_start_ptr[dptr->GCR_head_offset] = dptr->GCR_write_value;
    dptr->GCR_head_offset = head_next_offset(dptr);
}

inline static void data_read(drive_t *dptr)
{
    dptr->GCR_read = peek_current(dptr);

    if (dptr->GCR_head_bitoff > 0)
        dptr->GCR_read = (dptr->GCR_read << dptr->GCR_head_bitoff)
                         | (peek_next(dptr) >> (8 - dptr->GCR_head_bitoff));
}

/* Rotate the disk according to the current value of `drive_clk[]'.  If
   `mode_change' is non-zero, there has been a Read -> Write mode switch.  */
void rotation_rotate_disk(drive_t *dptr)
{
    rotation_t *rptr;
    CLOCK delta;
	int tdelta;

    rptr = &rotation[dptr->mynumber];

    /* Calculate the number of bits that have passed under the R/W head since
       the last time.  */
    delta = *(dptr->clk) - rptr->rotation_last_clk;
    rptr->rotation_last_clk = *(dptr->clk);

    while (delta > 0) {
		tdelta = delta > 1000 ? 1000 : delta;
		delta -= tdelta;

		rptr->accum += rot_speed_bps[rptr->frequency][rptr->speed_zone] * tdelta;
		rptr->bits_moved += rptr->accum / 1000000;
		rptr->accum %= 1000000;
	}

    if (rptr->bits_moved >= 8) {
        if (rptr->finish_byte) {
            if (rptr->bits_moved >= 8) {
                if (rptr->last_mode == 0) { /* write */
                    dptr->GCR_dirty_track = 1;
                    data_write(dptr);
                } else { /* read */
                    dptr->GCR_head_offset = head_next_offset(dptr);
                    data_read(dptr);
                }
                rptr->bits_moved -= 8;
            }

            rptr->finish_byte = 0;
            rptr->last_mode = dptr->read_write_mode;
        }

        if (rptr->last_mode == 0) {     /* write */
            dptr->GCR_dirty_track = 1;
            while (rptr->bits_moved >= 8) {
                data_write(dptr);
                rptr->bits_moved -= 8;
            }
        } else {                /* read */
            dptr->GCR_head_offset = ((dptr->GCR_head_offset
                                    + rptr->bits_moved / 8)
                                    % dptr->GCR_current_track_size);
            data_read(dptr);
            rptr->bits_moved %= 8;
            /*log_debug("HEAD %04i READ %02x", dptr->GCR_head_offset,
                      dptr->GCR_read);*/
            /* We could use this kludge
            if (dptr->GCR_read == 0)
                dptr->GCR_read = *(dptr->clk);
            */
        }

        /* The byte ready line is only set when no sync is found.  */
        if (rotation_sync_found(dptr)) {
            dptr->byte_ready_level = 1;
            dptr->byte_ready_edge = 1;
        }
    } /* if (rptr->shifter >= 8) */
}

#ifdef NEW_SYNC
inline static unsigned int count_sync_from_left(BYTE gcr_byte)
{
    unsigned int num;

    for (num = 0; (gcr_byte & 0x80) != 0; gcr_byte <<= 1)
        num++;

    return num;
}

inline static unsigned int count_sync_from_right(BYTE gcr_byte)
{
    unsigned int num;

    for (num = 0; (gcr_byte & 0x01) != 0; gcr_byte >>= 1)
        num++;

    return num;
}
#else
#if 0
inline static unsigned int is_one(DWORD gcr_data, unsigned int off)
{
    return ((gcr_data >> off) & 1);
}

inline static unsigned int count_sync_from_right(DWORD gcr_data,
                                                 unsigned int off)
{
    unsigned int i, num;

    for (i = off, num = 0; i < 32; i++) {
        if (is_one(gcr_data, i))
            num++;
        else
            break;
    }

    return num;
}

inline static void add_head_bitoff(drive_t *dptr, unsigned int inc)
{
    dptr->GCR_head_bitoff += inc;

    if (dptr->GCR_head_bitoff >= 8) {
        dptr->GCR_head_bitoff -= 8;
        dptr->GCR_head_offset = head_next_offset(dptr);
    }
}
#else
inline static unsigned int count_sync_from_left(BYTE gcr_byte)
{
    unsigned int num;

    for (num = 0; (gcr_byte & 0x80) != 0; gcr_byte <<= 1)
        num++;

    return num;
}

inline static unsigned int count_sync_from_right(BYTE gcr_byte)
{
    unsigned int num;

    for (num = 0; (gcr_byte & 0x01) != 0; gcr_byte >>= 1)
        num++;

    return num;
}
#endif
#endif

/* Return non-zero if the Sync mark is found.  It is required to
   call rotation_rotate_disk() to update drive[].GCR_head_offset first.
   The return value corresponds to bit#7 of VIA2 PRB. This means 0x0
   is returned when sync is found and 0x80 is returned when no sync
   is found.  */
BYTE rotation_sync_found(drive_t *dptr)
{
#ifdef NEW_SYNC
    unsigned int dnr;
    BYTE val, preval, nextval;
    unsigned int sync_bits;
    unsigned int num;

    dnr = dptr->mynumber;

    if (rotation[dnr].last_mode == 0 || dptr->attach_clk != (CLOCK)0)
        return 0x80;

    val = peek_current(dptr);
    preval = peek_previous(dptr);

    sync_bits = count_sync_from_right(preval) + count_sync_from_left(val);

    if (sync_bits >= 10)
        return 0; /* found! */

    if (val != 0xff) /* need to count sync bits from the right */
        sync_bits = count_sync_from_right(val);        

    nextval = peek_next(dptr);

    num = count_sync_from_left(nextval);

    sync_bits += (num < rotation[dnr].shifter) ? num : rotation[dnr].shifter;

    return (sync_bits >= 10) ? 0 : 0x80;

    /* As the current rotation code cannot cope with non byte aligned
       writes, do not change `drive[].bits_moved'!  */
    /* dptr->bits_moved = 0; */
#else
#if 0
    unsigned int dnr;
    DWORD val;
    unsigned int offset;

    dnr = dptr->mynumber;

    if (rotation[dnr].last_mode == 0 || dptr->attach_clk != (CLOCK)0)
        return 0x80;

    val = (peek_previous(dptr) << 24) | (peek_current(dptr) << 16)
          | (peek_next(dptr) << 8) | peek_next2(dptr);

    offset = 16 - dptr->GCR_head_bitoff - rotation[dnr].shifter;

    if (count_sync_from_right(val, offset) >= 10) {
        /*add_head_bitoff(dptr->bits_moved);*/
        /*dptr->bits_moved = 0;*/
        return 0; /* found! */
    }

    return 0x80;
#else
    unsigned int dnr;
    BYTE val;
    unsigned int sync_bits;
    unsigned int num;

    dnr = dptr->mynumber;

    if (rotation[dnr].last_mode == 0 || dptr->attach_clk != (CLOCK)0)
        return 0x80;

    val = peek_current(dptr);

    sync_bits = count_sync_from_right(peek_previous(dptr))
                + count_sync_from_left(val);

    if (sync_bits >= 10)
        return 0; /* found! */

    if (val != 0xff) /* need to count sync bits from the right */
        sync_bits = count_sync_from_right(val);

    num = count_sync_from_left(peek_next(dptr));

    sync_bits += (num < rotation[dnr].bits_moved) ? num : rotation[dnr].bits_moved;

    return (sync_bits >= 10) ? 0 : 0x80;

    /* As the current rotation code cannot cope with non byte aligned
       writes, do not change `drive[].bits_moved'!  */
    /* dptr->bits_moved = 0; */
#endif
#endif
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

