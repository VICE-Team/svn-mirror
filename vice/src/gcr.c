/*
 * gcr.c - GCR handling.
 *
 * Written by
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
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
#include "utils.h"

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

BYTE *gcr_find_sector_header(int track, int sector,
                             BYTE *gcr_track_start_ptr,
                             int gcr_current_track_size)
{
    BYTE *offset = gcr_track_start_ptr;
    BYTE *GCR_track_end = gcr_track_start_ptr + gcr_current_track_size;
    BYTE GCR_header[5], header_data[4];
    int i, sync_count = 0, wrap_over = 0;

    while ((offset < GCR_track_end) && !wrap_over) {
        while (*offset != 0xff) {
            offset++;
            if (offset >= GCR_track_end)
                return NULL;
        }

        while (*offset == 0xff) {
            offset++;
            if (offset == GCR_track_end) {
                offset = gcr_track_start_ptr;
                wrap_over = 1;
            }
            /* Check for killer tracks.  */
            if((++sync_count) >= gcr_current_track_size)
                return NULL;
        }

        for (i = 0; i < 5; i++) {
            GCR_header[i] = *(offset++);
            if (offset >= GCR_track_end) {
                offset = gcr_track_start_ptr;
                wrap_over = 1;
            }
        }

        convert_GCR_to_4bytes(GCR_header, header_data);

        if (header_data[0] == 0x08) {
            /* FIXME: Add some sanity checks here.  */
            if (header_data[2] == sector && header_data[3] == track)
                return offset;
        }
    }
    return NULL;
}

BYTE *gcr_find_sector_data(BYTE *offset,
                           BYTE *gcr_track_start_ptr,
                           int gcr_current_track_size)
{
    BYTE *GCR_track_end = gcr_track_start_ptr + gcr_current_track_size;
    int header = 0;

    while (*offset != 0xff) {
        offset++;
        if (offset >= GCR_track_end)
            offset = gcr_track_start_ptr;
        header++;
        if (header >= 500)
            return NULL;
    }

    while (*offset == 0xff) {
        offset++;
        if (offset == GCR_track_end)
            offset = gcr_track_start_ptr;
    }
    return offset;
}

int gcr_read_sector(gcr_t *gcr, BYTE *readdata, int track, int sector)
{
    BYTE buffer[260], *offset;
    BYTE *GCR_track_start_ptr;
    int GCR_current_track_size;

    GCR_track_start_ptr = gcr->data
                           + ((track - 1) * NUM_MAX_BYTES_TRACK);
    GCR_current_track_size = gcr->track_size[track - 1];

    offset = gcr_find_sector_header(track, sector,
                                    GCR_track_start_ptr,
                                    GCR_current_track_size);
    if (offset == NULL)
        return -1;
    offset = gcr_find_sector_data(offset, GCR_track_start_ptr,
                                  GCR_current_track_size);
    if (offset == NULL)
        return -1;

    convert_GCR_to_sector(buffer, offset, GCR_track_start_ptr,
                          GCR_current_track_size);
    if (buffer[0] != 0x7)
        return -1;

    memcpy(readdata, &buffer[1], 256);
    return 0;
}

int gcr_write_sector(gcr_t *gcr, BYTE *writedata, int track, int sector)
{
    BYTE buffer[260], gcr_buffer[325], *offset, *buf, *gcr_data;
    BYTE chksum;
    BYTE *GCR_track_start_ptr;
    int GCR_current_track_size;
    int i;

    GCR_track_start_ptr = gcr->data
                          + ((track - 1) * NUM_MAX_BYTES_TRACK);
    GCR_current_track_size = gcr->track_size[track - 1];

    offset = gcr_find_sector_header(track, sector,
                                    GCR_track_start_ptr,
                                    GCR_current_track_size);
    if (offset == NULL)
        return -1;
    offset = gcr_find_sector_data(offset, GCR_track_start_ptr,
                                  GCR_current_track_size);
    if (offset == NULL)
        return -1;
    buffer[0] = 0x7;
    memcpy(&buffer[1], writedata, 256);
    chksum = buffer[1];
    for (i = 2; i < 257; i++)
        chksum ^= buffer[i];
    buffer[257] = chksum;
    buffer[258] = buffer[259] = 0;

    buf = buffer;
    gcr_data = gcr_buffer;

    for (i = 0; i < 65; i++) {
        convert_4bytes_to_GCR(buf, gcr_data);
        buf += 4;
        gcr_data += 5;
    }

    for (i = 0; i < 325; i++) {
        *offset = gcr_buffer[i];
        offset++;
        if (offset == GCR_track_start_ptr + GCR_current_track_size)
            offset = GCR_track_start_ptr;
    }
    return 0;
}

gcr_t *gcr_create_image(void)
{
    return (gcr_t *)xmalloc(sizeof(gcr_t));
}

void gcr_destroy_image(gcr_t *gcr)
{
    free(gcr);
    return;
}

