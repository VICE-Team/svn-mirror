/*
 * snapshot.h - Implementation of machine snapshot files.
 *
 * Written by
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

#ifndef SNAPSHOT_H
#define SNAPSHOT_H

#include "types.h"

#define SNAPSHOT_MACHINE_NAME_LEN       16
#define SNAPSHOT_MODULE_NAME_LEN        16

extern int snapshot_write_byte(FILE *f, BYTE b);
extern int snapshot_write_word(FILE *f, WORD w);
extern int snapshot_write_dword(FILE *f, DWORD w);
extern int snapshot_write_padded_string(FILE *f, const char *s, BYTE pad_char,
                                        int len);
extern int snapshot_write_byte_array(FILE *f, BYTE *b, int len);
extern int snapshot_read_byte(FILE *f, BYTE *b_return);
extern int snapshot_read_word(FILE *f, WORD *w_return);
extern int snapshot_read_dword(FILE *f, DWORD *dw_return);
extern int snapshot_read_byte_array(FILE *f, BYTE *b_return, int size);
extern FILE *snapshot_create(const char *filename,
                             BYTE major_version, BYTE minor_version,
                             const char *machine_name);
extern FILE *snapshot_open(const char *filename,
                           BYTE *major_version_return,
                           BYTE *minor_version_return,
                           char *machine_name_return);
extern int snapshot_write_module_header(FILE *f,
                                        const char *name,
                                        BYTE major_version,
                                        BYTE minor_version);
extern int snapshot_read_module_header(FILE *f,
                                       char *name_return,
                                       BYTE *major_version_return,
                                       BYTE *minor_version_return);
extern int snapshot_close(FILE *f);

#endif
