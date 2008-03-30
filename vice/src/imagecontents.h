/*
 * imagecontents.h - Extract the directory from disk/tape images.
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

#ifndef _IMAGE_CONTENTS_H
#define _IMAGE_CONTENTS_H

#include "types.h"

#define IMAGE_CONTENTS_NAME_LEN       16
#define IMAGE_CONTENTS_ID_LEN         5
#define IMAGE_CONTENTS_FILE_NAME_LEN  16
#define IMAGE_CONTENTS_TYPE_LEN       5

struct _image_contents_file_list {
    BYTE name[IMAGE_CONTENTS_FILE_NAME_LEN + 1];
    BYTE type[IMAGE_CONTENTS_TYPE_LEN + 1];

    unsigned int size;

    struct _image_contents_file_list *prev, *next;
};
typedef struct _image_contents_file_list image_contents_file_list_t;

struct _contents {
    BYTE name[IMAGE_CONTENTS_NAME_LEN + 1];
    BYTE id[IMAGE_CONTENTS_ID_LEN + 1];
    int blocks_free;   /* -1: no free space.  */
    image_contents_file_list_t *file_list;
};
typedef struct _contents image_contents_t;

/* ------------------------------------------------------------------------- */

image_contents_t *image_contents_read_disk(const char *file_name);
image_contents_t *image_contents_read_tape(const char *file_name);
void image_contents_free(image_contents_t *contents);

#endif
