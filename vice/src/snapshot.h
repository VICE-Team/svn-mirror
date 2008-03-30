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

typedef struct snapshot_module snapshot_module_t;
typedef struct snapshot snapshot_t;

extern int snapshot_module_write_byte(snapshot_module_t *m, BYTE b);
extern int snapshot_module_write_word(snapshot_module_t *m, WORD w);
extern int snapshot_module_write_dword(snapshot_module_t *m, DWORD dw);
extern int snapshot_module_write_padded_string(snapshot_module_t *m,
                                               const char *s, BYTE pad_char,
                                               int len);
extern int snapshot_module_write_byte_array(snapshot_module_t *m, BYTE *b,
                                            int len);

extern int snapshot_module_read_byte(snapshot_module_t *m, BYTE *b_return);
extern int snapshot_module_read_word(snapshot_module_t *m, WORD *w_return);
extern int snapshot_module_read_dword(snapshot_module_t *m, DWORD *dw_return);
extern int snapshot_module_read_byte_array(snapshot_module_t *m,
                                           BYTE *b_return, int size);

extern snapshot_module_t *snapshot_module_create(snapshot_t *s,
                                                 const char *name,
                                                 BYTE major_version,
                                                 BYTE minor_version);
extern snapshot_module_t *snapshot_module_open(snapshot_t *s,
                                               const char *name,
                                               BYTE *major_version_return,
                                               BYTE *minor_version_return);
extern int snapshot_module_close(snapshot_module_t *m);

extern snapshot_t *snapshot_create(const char *filename,
                                   BYTE major_version, BYTE minor_version,
                                   const char *machine_name);
extern snapshot_t *snapshot_open(const char *filename,
                                 BYTE *major_version_return,
                                 BYTE *minor_version_return,
                                 char *machine_name_return);
extern int snapshot_close(snapshot_t *s);

#endif
