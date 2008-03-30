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
#include "rawfile.h"
#include "types.h"


fileio_info_t *fileio_info(const char *file_name)
{
    fileio_info_t *info;

    info = p00_info(file_name);

    if (info == NULL)
        info = cbmfile_info(file_name);

    return info;
}

void fileio_destroy(fileio_info_t *info)
{
    if (info != NULL) {
        rawfile_destroy(info->rawfile);
        lib_free(info->name);
        lib_free(info);
    }
}

