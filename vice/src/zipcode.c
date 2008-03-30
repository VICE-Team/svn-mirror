/*
 * zipcode.c - Support for zipcode files in VICE.
 *
 * Written by
 *  Daniel Sladic (sladic@eecg.toronto.edu)
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
#include <unistd.h>
#endif


int zipcode_read_sector(int zip_fd, int track, int *sector, char *buf)
{
  unsigned char trk, sec, len, rep, repnum, chra;
  int i, j, count, t1, t2;

  t1 = read(zip_fd, &trk, 1);
  t2 = read(zip_fd, &sec, 1);

  *sector = sec;

  if ((trk & 0x3f) != track || !t1 || !t2) {
    return 1;
  }

  if (trk & 0x80) {
    t1 = read(zip_fd, &len, 1);
    t2 = read(zip_fd, &rep, 1);
    if (!t1 || !t2) {
       return 1;
    }

    count = 0;

    for (i = 0; i < len; i++) {
      if ( (t1 = read(zip_fd, &chra, 1)) == 0) {
         return 1;
      }

      if (chra != rep)
	buf[count++] = chra;
      else {
        t1 = read(zip_fd, &repnum, 1);
        t2 = read(zip_fd, &chra, 1);
        if (!t1 || !t2) {
           return 1;
        }
	i += 2;
	for (j = 0; j < repnum; j++)
	  buf[count++] = chra;
      }
    }
  }

  else if (trk & 0x40) {
    if ( (t1 = read(zip_fd, &chra, 1)) == 0) {
       return 1;
    }

    for (i = 0; i < 256; i++)
      buf[i] = chra;
  }

  else if (256 != read (zip_fd, buf, 256)) {
    return 1;
  }

  return 0;
}
