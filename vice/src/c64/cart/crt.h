/*
 * crt.h - CRT image handling.
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

#ifndef VICE_CRT_H
#define VICE_CRT_H

#include "types.h"

typedef struct crt_chip_header_s {
    unsigned int skip;       /* bytes to skip after ROM */
    int type;                /* chip type */
    int bank;                /* bank number */
    int start;               /* start address of ROM */
    int size;                /* size of ROM in bytes */
} crt_chip_header_t;

extern int crt_attach(const char *filename, BYTE *rawcart);
extern int crt_getid(const char *filename);
extern int crt_read_chip_header(FILE *fd, crt_chip_header_t *header);
extern int crt_read_chip(BYTE *rawcart, int offset, crt_chip_header_t *chip, FILE *fd);

extern const char CRT_HEADER[];
extern const char CHIP_HEADER[];

#endif
