/*
 * output-file.h - Output file interface.
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

#ifndef _OUTPUT_FILE_H
#define _OUTPUT_FILE_H

#include "types.h"

extern int output_file_init_resources(void);
extern int output_file_init_cmdline_options(void);
extern void output_file_init(void);

extern void output_file_reset(void);

extern int output_file_open(unsigned int prnr);
extern void output_file_close(unsigned int prnr);
extern int output_file_putc(unsigned int prnr, BYTE b);
extern int output_file_getc(unsigned int prnr, BYTE *b);
extern int output_file_flush(unsigned int prnr);

#endif

