/*
 * grc.h - GCR handling.
 *
 * Written by
 *  Andreas Boose <boose@unixserv.rz.fh-hannover.de>
 *  Daniel Sladic <sladic@eecg.toronto.edu>
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

#ifndef _GCR_H
#define _GCR_H

#include "types.h"

/* Number of bytes in one raw track.  */
#define NUM_MAX_BYTES_TRACK 7928

/* Number of tracks we emulate.  */
#define MAX_GCR_TRACKS 70

typedef struct {
    /* Raw GCR image of the disk.  */
    BYTE data[MAX_GCR_TRACKS * NUM_MAX_BYTES_TRACK];

    /* Speed zone image of the disk.  */
    BYTE speed_zone[MAX_GCR_TRACKS * NUM_MAX_BYTES_TRACK];

    /* Size of the GCR data of each track.  */
    int track_size[MAX_GCR_TRACKS];

} gcr_t;

extern void convert_4bytes_to_GCR(BYTE *buffer, BYTE *ptr);
extern void convert_GCR_to_4bytes(BYTE *buffer, BYTE *ptr);
extern void convert_sector_to_GCR(BYTE *buffer, BYTE *ptr,
				  int track, int sector,
				  BYTE diskID1, BYTE diskID2);
extern void convert_GCR_to_sector(BYTE *buffer, BYTE *ptr,
				  BYTE *GCR_track_start_ptr,
				  int GCR_current_track_size);

extern BYTE *gcr_find_sector_header(int track, int sector,
                                    BYTE *gcr_track_start_ptr,
                                    int gcr_current_track_size);
extern BYTE *gcr_find_sector_data(BYTE *offset,
                                  BYTE *gcr_track_start_ptr,
                                  int gcr_current_track_size);
extern int gcr_read_sector(gcr_t *gcr, BYTE *readdata, int track, int sector);
extern int gcr_write_sector(gcr_t *gcr, BYTE *writedata, int track, int sector);

extern gcr_t *gcr_create_image(void);
extern void gcr_destroy_image(gcr_t *gcr);
#endif

