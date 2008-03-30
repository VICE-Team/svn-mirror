/*
 * cbmfile.c - CBM file handling.
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
#include "charset.h"
#include "fileio.h"
#include "lib.h"
#include "rawfile.h"
#include "types.h"


fileio_info_t *cbmfile_info(const char *file_name, const char *path,
                            unsigned int command)
{
    BYTE *cbm_name;
    fileio_info_t *info;
    struct rawfile_info_s *rawfile;

    rawfile = rawfile_open(file_name, path, command & FILEIO_COMMAND_MASK);

    if (rawfile == NULL)
        return NULL;

    cbm_name = (BYTE *)lib_stralloc(rawfile->name);
    charset_petconvstring(cbm_name, 0);

    info = (fileio_info_t *)lib_malloc(sizeof(fileio_info_t));
    info->name = cbm_name;
    info->type = FILEIO_TYPE_PRG;
    info->format = FILEIO_FORMAT_RAW;
    info->rawfile = rawfile;

    return info;
}

