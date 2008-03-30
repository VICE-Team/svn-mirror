/*
 * gcr.c - GCR handling.
 *
 * Written by
 *  Daniel Sladic (sladic@eecg.toronto.edu)
 *  Andreas Boose (boose@unixserv.rz.fh-hannover.de)
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

#ifdef STDC_HEADERS
#include <stdio.h>
#ifndef __riscos
#include <fcntl.h>
#endif
#include <ctype.h>
#include <string.h>
#endif

#include "gcr.h"

/* GCR handling.  */

static BYTE GCR_conv_data[16] = { 0x0a, 0x0b, 0x12, 0x13,
				  0x0e, 0x0f, 0x16, 0x17,
				  0x09, 0x19, 0x1a, 0x1b,
				  0x0d, 0x1d, 0x1e, 0x15 };

static BYTE From_GCR_conv_data[32] = { 0, 0, 0, 0, 0, 0, 0, 0,
				       0, 8, 0, 1, 0,12, 4, 5,
				       0, 0, 2, 3, 0,15, 6, 7,
				       0, 9,10,11, 0,13,14, 0 };

void convert_4bytes_to_GCR(BYTE *buffer, BYTE *ptr)
{
    *ptr = GCR_conv_data[(*buffer) >> 4] << 3;
    *ptr |= GCR_conv_data[(*buffer) & 0x0f] >> 2;
    ptr++;

    *ptr = GCR_conv_data[(*buffer) & 0x0f] << 6;
    buffer++;
    *ptr |= GCR_conv_data[(*buffer) >> 4] << 1;
    *ptr |= GCR_conv_data[(*buffer) & 0x0f] >> 4;
    ptr++;

    *ptr = GCR_conv_data[(*buffer) & 0x0f] << 4;
    buffer++;
    *ptr |= GCR_conv_data[(*buffer) >> 4] >> 1;
    ptr++;

    *ptr = GCR_conv_data[(*buffer) >> 4] << 7;
    *ptr |= GCR_conv_data[(*buffer) & 0x0f] << 2;
    buffer++;
    *ptr |= GCR_conv_data[(*buffer) >> 4] >> 3;
    ptr++;

    *ptr = GCR_conv_data[(*buffer) >> 4] << 5;
    *ptr |= GCR_conv_data[(*buffer) & 0x0f];
}

void convert_GCR_to_4bytes(BYTE *buffer, BYTE *ptr)
{
    BYTE gcr_bytes[8];
    int i;

    gcr_bytes[0] =  (*buffer) >> 3;
    gcr_bytes[1] =  ((*buffer) & 0x07) << 2;
    buffer++;
    gcr_bytes[1] |= (*buffer) >> 6;
    gcr_bytes[2] =  ((*buffer) & 0x3e) >> 1;
    gcr_bytes[3] =  ((*buffer) & 0x01) << 4;
    buffer++;
    gcr_bytes[3] |= ((*buffer) & 0xf0) >> 4;
    gcr_bytes[4] =  ((*buffer) & 0x0f) << 1;
    buffer++;
    gcr_bytes[4] |= (*buffer) >> 7;
    gcr_bytes[5] =  ((*buffer) & 0x7c) >> 2;
    gcr_bytes[6] =  ((*buffer) & 0x03) << 3;
    buffer++;
    gcr_bytes[6] |= ((*buffer) & 0xe0) >> 5;
    gcr_bytes[7] = (*buffer) & 0x1f;

    for (i = 0; i < 4; i++, ptr++) {
        *ptr = From_GCR_conv_data[gcr_bytes[2 * i]] << 4;
        *ptr |= From_GCR_conv_data[gcr_bytes[2 * i + 1]];
    }
}

void convert_sector_to_GCR(BYTE *buffer, BYTE *ptr, int track, int sector,
                           BYTE diskID1, BYTE diskID2)
{
    int i;
    BYTE buf[4];

    memset(ptr, 0xff, 5);	/* Sync */
    ptr += 5;

    buf[0] = 0x08;		/* Header identifier */
    buf[1] = sector ^ track ^ diskID2 ^ diskID1;
    buf[2] = sector;
    buf[3] = track;
    convert_4bytes_to_GCR(buf, ptr);
    ptr += 5;

    buf[0] = diskID2;
    buf[1] = diskID1;
    buf[2] = buf[3] = 0x0f;
    convert_4bytes_to_GCR(buf, ptr);
    ptr += 5;

    memset(ptr, 0x55, 9);	/* Header Gap */
    ptr += 9;

    memset(ptr, 0xff, 5);	/* Sync */
    ptr += 5;

    for (i = 0; i < 65; i++) {
	convert_4bytes_to_GCR(buffer, ptr);
	buffer += 4;
	ptr += 5;
    }

    /* FIXME: This is approximated.  */
    memset(ptr, 0x55, 6);	/* Gap before next sector.  */
    ptr += 6;

}

void convert_GCR_to_sector(BYTE *buffer, BYTE *ptr,
                           BYTE *GCR_track_start_ptr,
                           int GCR_current_track_size)
{
    BYTE *offset = ptr;
    BYTE *GCR_track_end = GCR_track_start_ptr + GCR_current_track_size;
    BYTE GCR_header[5];
    int i, j;

    for (i = 0; i < 65; i++) {
	for (j = 0; j < 5; j++) {
	    GCR_header[j] = *(offset++);
	    if (offset >= GCR_track_end)
		offset = GCR_track_start_ptr;
	}
	convert_GCR_to_4bytes(GCR_header, buffer);
	buffer += 4;
    }
}

