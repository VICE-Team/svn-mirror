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

#include "lib.h"
#include "log.h"
#include "rawfile.h"
#include "util.h"


rawfile_info_t *rawfile_info(const char *file_name)
{
    rawfile_info_t *info;

    if (util_file_exists(file_name) == 0) {
        log_error(LOG_DEFAULT, "Cannot open `%s'.", file_name);
        return NULL;
    }

    info = (rawfile_info_t *)lib_malloc(sizeof(rawfile_info_t));

    util_fname_split(file_name, &(info->path), &(info->name));
    info->readonly = 0;

    return info;
}

void rawfile_destroy(rawfile_info_t *info)
{
    if (info != NULL) {
        lib_free(info->name);
        lib_free(info->path);
        lib_free(info);
    }
}

