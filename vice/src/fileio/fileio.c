/*
 * fileio.c - File IO handling.
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

#include "vice.h"

#include <stdio.h>

#include "cbmfile.h"
#include "fileio.h"
#include "lib.h"
#include "p00.h"
#include "types.h"
#include "util.h"


fileio_info_t *fileio_open(const char *file_name, const char *path,
                           unsigned int format, unsigned int command,
                           unsigned int type)
{
    fileio_info_t *info = NULL;
    char *new_file, *new_path;

    if ((command & FILEIO_COMMAND_FSNAME) && path == NULL) {
        util_fname_split(file_name, &new_path, &new_file);
    } else {
        new_file = lib_stralloc(file_name);
        new_path = lib_stralloc(path);
    }

    do {
        if (format & FILEIO_FORMAT_P00)
            info = p00_open(new_file, new_path, command, type);

        if (info != NULL)
            break;

        if (format & FILEIO_FORMAT_RAW)
           info = cbmfile_open(new_file, new_path, command, type);

        if (info != NULL)
            break;
    } while (0);

    lib_free(new_file);
    lib_free(new_path);

    return info;
}

void fileio_close(fileio_info_t *info)
{
    if (info != NULL) {
        switch (info->format) {
          case FILEIO_FORMAT_RAW:
            cbmfile_close(info);
            break;
          case FILEIO_FORMAT_P00:
            p00_close(info);
            break;
        }
        lib_free(info->name);
        lib_free(info);
    }
}

