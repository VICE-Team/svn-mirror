/*
 * c1541.c - Stand-alone disk image maintenance program.
 *
 * Written by
 *  Teemu Rantanen   (tvr@cs.hut.fi)
 *  Jouko Valta      (jopi@zombie.oulu.fi)
 *  Gerhard Wesp     (gwesp@cosy.sbg.ac.at)
 *
 * Patches by
 *  Olaf Seibert     (rhialto@mbfys.kun.nl)
 *  Ettore Perazzoli (ettore@comm2000.it)
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

#ifndef _C1541_H
#define _C1541_H

#include "drive.h"

extern int  ip_execute ( DRIVE *floppy, BYTE *buf, int length );
extern int  do_validate ( DRIVE *floppy );
extern int  check_header ( int fd, hdrinfo *hdr );
extern int  check_track_sector( int format, int track, int sector );
extern int  get_diskformat ( int devtype );
extern int  num_blocks ( int format, int tracks );
extern void no_a0_pads ( BYTE *ptr, int l );
extern int  compare_filename ( char *name, char *pattern );

extern DRIVE *DriveData[4];

#endif /* ndef _C1541_H */
