/*
 * diskcontents.c - Extract the directory from disk images.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Andreas Boose <viceteam@t-online.de>
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

#include "diskcontents-block.h"
#include "diskcontents-iec.h"
#include "diskcontents.h"
#include "imagecontents.h"
#include "serial.h"
#include "utils.h"


image_contents_t *diskcontents_read(const char *file_name, unsigned int unit)
{
    image_contents_t *contents = NULL;

    switch (unit) {
      case 0:
        contents = diskcontents_block_read(file_name, unit);
        break;
      case 8:
      case 9:
      case 10:
      case 11:
        if (serial_device_get_realdevice_state(unit))
            contents = diskcontents_iec_read(unit);
        else
            contents = diskcontents_block_read(file_name, unit);
        break;
    }

    return contents;
}

char *diskcontents_filename_by_number(const char *filename, unsigned int unit,
                                      unsigned int file_index)
{
    image_contents_t *contents;
    image_contents_file_list_t *current;
    char *s;

    contents = diskcontents_read(filename, unit);

    if (contents == NULL)
        return NULL;

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

