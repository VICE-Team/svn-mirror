/*
 * vdrive-dir.h - Virtual disk-drive implementation.
 *                Directory specific functions.
 *
 * Written by
 *  Andreas Boose       <boose@linux.rz.fh-hannover.de>
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

#include "vdrive.h"

#define SLOT_TYPE_OFFSET      2
#define SLOT_FIRST_TRACK      3
#define SLOT_FIRST_SECTOR     4
#define SLOT_NAME_OFFSET      5
#define SLOT_SIDE_TRACK       21
#define SLOT_SIDE_SECTOR      22
#define SLOT_NR_BLOCKS        30

extern void vdrive_dir_find_first_slot(DRIVE *floppy, const char *name,
                                       int length, int type);
extern BYTE *vdrive_dir_find_next_slot(DRIVE *floppy);
extern void vdrive_dir_no_a0_pads(BYTE *ptr, int l);
extern void vdrive_dir_remove_slot(DRIVE *floppy, BYTE *slot);

