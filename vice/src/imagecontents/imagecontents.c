/*
 * imagecontents.c - Extract the directory from disk/tape images.
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
#include <stdlib.h>
#include <string.h>

#include "charset.h"
#include "imagecontents.h"
#include "types.h"
#include "utils.h"


/* ------------------------------------------------------------------------- */

image_contents_t *image_contents_new(void)
{
    image_contents_t *newimg;

    newimg = xcalloc(1, sizeof(image_contents_t));

    newimg->blocks_free = -1;
    newimg->file_list = NULL;

    return newimg;
}

void image_contents_destroy(image_contents_t *contents)
{
    image_contents_file_list_t *p, *h;

    for (p = contents->file_list; p != NULL; h = p, p = p->next, free(h));

    free(contents);
}

void image_contents_screencode_destroy(image_contents_screencode_t *c)
{
    image_contents_screencode_t *h;

    while (c != NULL) {
        h = c->next;
        free(c->line);
        free(c);
        c = h;
    }
}

image_contents_screencode_t *image_contents_to_screencode(image_contents_t
                                                          *contents)
{
    BYTE *buf, rawline[50];
    unsigned int len, i;
    image_contents_screencode_t *image_contents_screencode, *screencode_ptr;
    image_contents_file_list_t *p;

    image_contents_screencode = (image_contents_screencode_t *)xmalloc
                                (sizeof(image_contents_screencode_t));

    screencode_ptr = image_contents_screencode;

    sprintf(rawline, "0 \"%s\" %s", contents->name, contents->id);
    charset_petcii_to_screencode_line(rawline, &buf, &len);
    screencode_ptr->line = buf;
    screencode_ptr->length = len;
    screencode_ptr->next = NULL;

    /*
     I removed this for OS/2 because I want to have an output
     which looks like a directory listing which you can load in
     the emulator.
     */
#ifndef __OS2__
    if (contents->file_list == NULL) {
        charset_petcii_to_screencode_line("(eMPTY IMAGE.)", &buf, &len);
        screencode_ptr->next = (image_contents_screencode_t *)xmalloc
                               (sizeof(image_contents_screencode_t));
        screencode_ptr = screencode_ptr->next;

        screencode_ptr->line = buf;
        screencode_ptr->length = len;
        screencode_ptr->next = NULL;
    }
#endif

    for (p = contents->file_list; p != NULL; p = p->next) {

        sprintf((char *)rawline, "%-5d \"                  ", p->size);
        memcpy(&rawline[7], p->name, IMAGE_CONTENTS_FILE_NAME_LEN);

        for (i = 0; i < IMAGE_CONTENTS_FILE_NAME_LEN; i++) {
            if (rawline[7 + i] == 0xa0) {
                rawline[7 + i] = '"';
                break;
            }
        }

        if (i == IMAGE_CONTENTS_FILE_NAME_LEN)
            rawline[7 + IMAGE_CONTENTS_FILE_NAME_LEN] = '"';

        memcpy(&rawline[7 + IMAGE_CONTENTS_FILE_NAME_LEN + 2], p->type, 5);
        charset_petcii_to_screencode_line(rawline, &buf, &len);

        screencode_ptr->next = (image_contents_screencode_t *)xmalloc
                               (sizeof(image_contents_screencode_t));
        screencode_ptr = screencode_ptr->next;

        screencode_ptr->line = buf;
        screencode_ptr->length = len;
        screencode_ptr->next = NULL;
    }

    if (contents->blocks_free >= 0) {
        sprintf(rawline, "%d BLOCKS FREE.", contents->blocks_free);
        charset_petcii_to_screencode_line(rawline, &buf, &len);

        screencode_ptr->next = (image_contents_screencode_t *)xmalloc
                               (sizeof(image_contents_screencode_t));
        screencode_ptr = screencode_ptr->next;

        screencode_ptr->line = buf;
        screencode_ptr->length = len;
        screencode_ptr->next = NULL;
    }

    return image_contents_screencode;
}

char *image_contents_to_string(image_contents_t *contents,
                               unsigned int conversion_rule)
{
    /* 16 spaces are a 17byte string. is this ok with '+1' ? */
    static char filler[IMAGE_CONTENTS_FILE_NAME_LEN+1] = "                ";
    image_contents_file_list_t *p;
    char line_buf[256];
    char *buf;
    int buf_size;
    size_t max_buf_size;
    int len;

#define BUFCAT(s, n) util_bufcat(buf, &buf_size, &max_buf_size, (s), (n))

    max_buf_size = 4096;
    buf = (char*)xmalloc(max_buf_size);
    buf_size = 0;

    buf = BUFCAT("0 \"", 3);
    buf = BUFCAT((char *)contents->name, strlen((char *)contents->name));
    buf = BUFCAT("\" ", 2);
    buf = BUFCAT((char *)contents->id, strlen((char *)contents->id));

    if (contents->file_list == NULL) {
        const char *s;

        if (conversion_rule == IMAGE_CONTENTS_STRING_PETSCII)
            s = "\n(EMPTY IMAGE.)";
        else
            s = "\n(eMPTY IMAGE.)";

        buf = BUFCAT(s, strlen(s));
    }

    for (p = contents->file_list; p != NULL; p = p->next) {
        size_t name_len;
        int i;
        char print_name[IMAGE_CONTENTS_FILE_NAME_LEN + 1];

        memset(print_name, 0, IMAGE_CONTENTS_FILE_NAME_LEN + 1);
        for (i = 0; i < IMAGE_CONTENTS_FILE_NAME_LEN; i++) {
            if (p->name[i] == 0xa0)
                break;           
            print_name[i] = (char)p->name[i];
        }

        len = sprintf(line_buf, "\n%-5d \"%s\" ", p->size, print_name);
        buf = BUFCAT(line_buf, len);

        name_len = strlen((char *)print_name);
        if (name_len < IMAGE_CONTENTS_FILE_NAME_LEN)
            buf = BUFCAT(filler, IMAGE_CONTENTS_FILE_NAME_LEN - name_len);

        buf = BUFCAT((char *)p->type, strlen((char *)p->type));
    }

    if (contents->blocks_free >= 0) {
        if (conversion_rule == IMAGE_CONTENTS_STRING_PETSCII)
            len = sprintf(line_buf, "\n%d BLOCKS FREE.", contents->blocks_free);
        else
            len = sprintf(line_buf, "\n%d blocks free.", contents->blocks_free);

        buf = BUFCAT(line_buf, len);
    }

    buf = BUFCAT("\n", 2); /* With a closing zero.  */

    if (conversion_rule == IMAGE_CONTENTS_STRING_ASCII)
        charset_petconvstring(buf, 1);

    return buf;
}

