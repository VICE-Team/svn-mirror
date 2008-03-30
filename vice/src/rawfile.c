/*
 * rawfile.c - Raw file handling.
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

#include "archdep.h"
#include "fileio.h"
#include "lib.h"
#include "rawfile.h"
#include "util.h"


rawfile_info_t *rawfile_open(const char *file_name, const char *path,
                             unsigned int command)
{
    rawfile_info_t *info;
    char *complete;
    FILE *fd;
    const char *mode = NULL;

    if (path == NULL)
        complete = lib_stralloc(file_name);
    else
        complete = util_concat(path, FSDEV_DIR_SEP_STR, file_name, NULL);

    switch (command) {
      case FILEIO_COMMAND_READ:
        mode = MODE_READ;
        break;
      case FILEIO_COMMAND_WRITE:
        mode = MODE_WRITE;
        break;
      case FILEIO_COMMAND_APPEND:
        mode = MODE_APPEND;
        break;
      case FILEIO_COMMAND_APPEND_READ:
        mode = MODE_APPEND_READ_WRITE;
        break;
      default:
        return NULL;
    }

    fd = fopen(complete, mode);

    if (fd == NULL) {
        lib_free(complete);
        return NULL;
    }

    info = (rawfile_info_t *)lib_malloc(sizeof(rawfile_info_t));

    info->fd = (void *)fd;
    util_fname_split(complete, &(info->path), &(info->name));
    info->readonly = 0;

    lib_free(complete);

    return info;
}

void rawfile_destroy(rawfile_info_t *info)
{
    if (info != NULL) {
        fclose((FILE *)(info->fd));
        lib_free(info->name);
        lib_free(info->path);
        lib_free(info);
    }
}

