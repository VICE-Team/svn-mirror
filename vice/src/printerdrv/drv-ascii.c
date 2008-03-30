/*
 * drv-ascii.c - ASCII printer driver.
 *
 * Written by
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
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

#include "drv-ascii.h"
#include "output-file.h"
#include "types.h"

int drv_ascii_open(int device)
{
    return output_file_open(device);
}

void drv_ascii_close(int fi)
{
    output_file_close(fi);
}

int drv_ascii_putc(int fi, BYTE b)
{
    return output_file_putc(fi, b);
}

int drv_ascii_getc(int fi, BYTE *b)
{
    return output_file_getc(fi, b);
}

int drv_ascii_flush(int fi)
{
    return output_file_flush(fi);
}

