/*
 * tapecontents.c - Extract the directory from tape images.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
 *  Tibor Biczo <crown@mail.matav.hu>
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

#include <stdio.h>
#include <string.h>

#include "imagecontents.h"
#include "t64.h"
#include "utils.h"


image_contents_t *image_contents_read_tape(const char *file_name)
{
    t64_t *t64;
    image_contents_t *new;
    image_contents_file_list_t *lp;

    t64 = t64_open(file_name);
    if (t64 == NULL)
        return NULL;

    new = image_contents_new();

    memcpy(new->name, t64->header.description, T64_REC_CBMNAME_LEN);
    *new->id = 0;
    new->blocks_free = -1;

    lp = NULL;
    new->file_list = NULL;

    while (t64_seek_to_next_file(t64, 0) >= 0) {
        t64_file_record_t *rec;

        rec = t64_get_current_file_record(t64);
        if (rec->entry_type != T64_FILE_RECORD_FREE) {
            image_contents_file_list_t *new_list;

            new_list = (image_contents_file_list_t*)xmalloc(sizeof(image_contents_file_list_t));
            memcpy(new_list->name, rec->cbm_name, T64_REC_CBMNAME_LEN);
            new_list->name[IMAGE_CONTENTS_FILE_NAME_LEN] = 0;

            /* XXX: Not quite true, but this is what the tape emulation
               will do anyway.  */
            strcpy((char *)new_list->type, " PRG ");

            new_list->size = (rec->end_addr - rec->start_addr) / 254;
            new_list->next = NULL;

            if (lp == NULL) {
                new_list->prev = NULL;
                new->file_list = new_list;
                lp = new->file_list;
            } else {
                new_list->prev = lp;
                lp->next = new_list;
                lp = new_list;
            }
        }
    }

    t64_close(t64);
    return new;
}

char *image_contents_tape_filename_by_number(const char *filename,
                                             unsigned int file_index)
{
    image_contents_t *contents;
    image_contents_file_list_t *current;
    char *s;

    contents = image_contents_read_tape(filename);
    if (contents == NULL) {
        return NULL;
    }

    s = NULL;

    if (file_index != 0) {
        current = contents->file_list;
        file_index--;
        while ((file_index != 0) && (current != NULL)) {
            current = current->next;
            file_index--;
        }
        if (current != NULL) {
            s = stralloc((char *)(current->name));
        }
    }

    image_contents_destroy(contents);

    return s;
}

