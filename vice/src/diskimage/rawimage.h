/*
 * rawimage.h
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

#ifndef _RAWIMAGE_H
#define _RAWIMAGE_H

#include "types.h"

struct disk_image_s;

extern void rawimage_init(void);

extern int rawimage_open(struct disk_image_s *image);
extern int rawimage_close(struct disk_image_s *image);
extern int rawimage_read_sector(struct disk_image_s *image, BYTE *buf,
                                unsigned int track, unsigned int sector);
extern int rawimage_write_sector(struct disk_image_s *image, BYTE *buf,
                                 unsigned int track, unsigned int sector);
#endif

