/*
 * grc.h - GCR handling.
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

#ifndef _GCR_H
#define _GCR_H

#include "types.h"

extern void convert_4bytes_to_GCR(BYTE *buffer, BYTE *ptr);
extern void convert_GCR_to_4bytes(BYTE *buffer, BYTE *ptr);
extern void convert_sector_to_GCR(BYTE *buffer, BYTE *ptr,
				  int track, int sector,
				  BYTE diskID1, BYTE diskID2);
extern void convert_GCR_to_sector(BYTE *buffer, BYTE *ptr,
				  BYTE *GCR_track_start_ptr,
				  int GCR_current_track_size);

#endif
