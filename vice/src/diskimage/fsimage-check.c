/*
 * fsimage-check.c
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

#include "diskconstants.h"
#include "diskimage.h"
#include "fsimage-check.h"


int fsimage_check_sector(unsigned int format, unsigned int track,
                         unsigned int sector)
{
    unsigned int sectors = 0, i;

    if (track < 1)
        return -1;

    switch (format) {
      case DISK_IMAGE_TYPE_D64:
      case DISK_IMAGE_TYPE_X64:
        if (track > MAX_TRACKS_1541 || sector
            >= disk_image_sector_per_track(DISK_IMAGE_TYPE_D64, track))
            return -1;
        for (i = 1; i < track; i++)
            sectors += disk_image_sector_per_track(DISK_IMAGE_TYPE_D64, i);
        sectors += sector;
        break;
      case DISK_IMAGE_TYPE_D67:
        if (track > MAX_TRACKS_2040 || sector
            >= disk_image_sector_per_track(DISK_IMAGE_TYPE_D67, track))
            return -1;
        for (i = 1; i < track; i++)
            sectors += disk_image_sector_per_track(DISK_IMAGE_TYPE_D67, i);
        sectors += sector;
        break;
      case DISK_IMAGE_TYPE_D71:
        if (track > MAX_TRACKS_1571)
            return -1;
        if (track > NUM_TRACKS_1541) {          /* The second side */
            track -= NUM_TRACKS_1541;
            sectors = NUM_BLOCKS_1541;
        }
        if (sector >= disk_image_sector_per_track(DISK_IMAGE_TYPE_D64, track))
            return -1;
        for (i = 1; i < track; i++)
            sectors += disk_image_sector_per_track(DISK_IMAGE_TYPE_D64, i);
        sectors += sector;
        break;
      case DISK_IMAGE_TYPE_D81:
        if (track > MAX_TRACKS_1581 || sector >=  NUM_SECTORS_1581)
            return -1;
        sectors = (track - 1) * NUM_SECTORS_1581 + sector;
        break;
      case DISK_IMAGE_TYPE_D80:
        if (track > MAX_TRACKS_8050 || sector
            >= disk_image_sector_per_track(DISK_IMAGE_TYPE_D80, track))
            return -1;
        for (i = 1; i < track; i++)
            sectors += disk_image_sector_per_track(DISK_IMAGE_TYPE_D80, i);
        sectors += sector;
        break;
      case DISK_IMAGE_TYPE_D82:
        if (track > MAX_TRACKS_8250)
            return -1;
        if (track > NUM_TRACKS_8050) {          /* The second side */
            track -= NUM_TRACKS_8050;
            sectors = NUM_BLOCKS_8050;
        }
        if (sector >= disk_image_sector_per_track(DISK_IMAGE_TYPE_D80, track))
            return -1;
        for (i = 1; i < track; i++)
            sectors += disk_image_sector_per_track(DISK_IMAGE_TYPE_D80, i);
        sectors += sector;
        break;
      default:
        return -1;
    }
    return (int)(sectors);
}

