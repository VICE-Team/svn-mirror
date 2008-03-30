/*
 * imagecontents.h - Extract the directory from disk/tape images.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
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

#ifndef _IMAGE_CONTENTS_H
#define _IMAGE_CONTENTS_H

#include "types.h"

#define IMAGE_CONTENTS_NAME_LEN       16
#define IMAGE_CONTENTS_ID_LEN         5
#define IMAGE_CONTENTS_FILE_NAME_LEN  16
#define IMAGE_CONTENTS_TYPE_LEN       5

struct image_contents_file_list_s {
    BYTE name[IMAGE_CONTENTS_FILE_NAME_LEN + 1];
    BYTE type[IMAGE_CONTENTS_TYPE_LEN + 1];

    unsigned int size;

    struct image_contents_file_list_s *prev, *next;
};
typedef struct image_contents_file_list_s image_contents_file_list_t;

struct image_contents_s {
    BYTE name[IMAGE_CONTENTS_NAME_LEN + 1];
    BYTE id[IMAGE_CONTENTS_ID_LEN + 1];
    int blocks_free;   /* -1: No free space information.  */
    image_contents_file_list_t *file_list;
};
typedef struct image_contents_s image_contents_t;

struct image_contents_screencode_s {
    BYTE *line;
    unsigned int length;
    struct image_contents_screencode_s *next;
};
typedef struct image_contents_screencode_s image_contents_screencode_t;

/* ------------------------------------------------------------------------- */

extern void image_contents_destroy(image_contents_t *contents);
extern image_contents_t *image_contents_new(void);

#define IMAGE_CONTENTS_STRING_PETSCII 0
#define IMAGE_CONTENTS_STRING_ASCII   1

extern char *image_contents_to_string(image_contents_t *contents,
                                      unsigned int conversion_rule);
extern image_contents_screencode_t *image_contents_to_screencode
                                   (image_contents_t *contents);
extern void image_contents_screencode_destroy(image_contents_screencode_t *c);

#define IMAGE_CONTENTS_AUTO 0
#define IMAGE_CONTENTS_DISK 1
#define IMAGE_CONTENTS_TAPE 2

extern image_contents_t *image_contents_read(unsigned int type,
                                             const char *filename,
                                             unsigned int unit);
extern char *image_contents_read_string(unsigned int type,
                                        const char *filename,
                                        unsigned int unit,
                                        unsigned int conversion);
extern char *image_contents_filename_by_number(unsigned int type,
                                               const char *filename,
                                               unsigned int unit,
                                               unsigned int file_index);

extern image_contents_t *diskcontents_iec_read(unsigned int unit);

#endif

