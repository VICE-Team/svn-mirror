/*
 * p00.h - Utility functions for P00 file support.
 *
 * Written by
 *  Ettore Perazzoli     (ettore@comm2000.it)
 *
 * Based on older code by
 *  Dan Fandrich         (dan@fch.wimsey.bc.ca)
 *  Jouko Valta          (jopi@stekt.oulu.fi)
 *  Martin Pottendorfer  (Martin.Pottendorfer@autalcatel.at)
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

#ifndef P00_H
#define P00_H

#include "types.h"

int p00_check_name(const char *name);
int p00_read_header(FILE * fd, BYTE *cbmname_return,
                    unsigned int *recsize_return);
int p00_write_header(FILE * fd, BYTE *cbmname, BYTE recsize);

#endif
