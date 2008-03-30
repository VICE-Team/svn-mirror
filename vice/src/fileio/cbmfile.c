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
#include <string.h>

#include "cbmfile.h"
#include "charset.h"
#include "fileio.h"
#include "lib.h"
#include "rawfile.h"
#include "types.h"


fileio_info_t *cbmfile_open(const char *file_name, const char *path,
                            unsigned int command, unsigned int type)
{
    BYTE *cbm_name;
    fileio_info_t *info;
    struct rawfile_info_s *rawfile;
    char *fsname;

    fsname = lib_stralloc(file_name);

    if (!(command & FILEIO_COMMAND_FSNAME))
        charset_petconvstring(fsname, 1);

    rawfile = rawfile_open(fsname, path, command & FILEIO_COMMAND_MASK);

    lib_free(fsname);

    if (rawfile == NULL)
        return NULL;

    cbm_name = lib_stralloc(file_name);

    if (command & FILEIO_COMMAND_FSNAME)
        charset_petconvstring(cbm_name, 0);

    info = (fileio_info_t *)lib_malloc(sizeof(fileio_info_t));
    info->name = cbm_name;
    info->length = strlen(cbm_name);
    info->type = type;
    info->format = FILEIO_FORMAT_RAW;
    info->rawfile = rawfile;

    return info;
}

void cbmfile_close(fileio_info_t *info)
{
    rawfile_destroy(info->rawfile);
}

unsigned int cbmfile_read(fileio_info_t *info, BYTE *buf, unsigned int len)
{
    return rawfile_read(info->rawfile, buf, len);
}

unsigned int cbmfile_write(fileio_info_t *info, BYTE *buf, unsigned int len)
{
    return rawfile_write(info->rawfile, buf, len);
}

unsigned int cbmfile_ferror(fileio_info_t *info)
{
    return rawfile_ferror(info->rawfile);
}

unsigned int cbmfile_rename(const char *src_name, const char *dst_name,
                            const char *path)
{
    char *src_cbm, *dst_cbm;
    unsigned int rc;
     
    src_cbm = lib_stralloc(src_name);
    dst_cbm = lib_stralloc(dst_name);

    charset_petconvstring(src_cbm, 1);
    charset_petconvstring(dst_cbm, 1);

    rc = rawfile_rename(src_cbm, dst_cbm, path);

    lib_free(src_cbm);
    lib_free(dst_cbm);

    return rc;
}

unsigned int cbmfile_scratch(const char *file_name, const char *path)
{
    char *src_cbm;
    unsigned int rc;

    src_cbm = lib_stralloc(file_name);
    charset_petconvstring(src_cbm, 1);

    rc = rawfile_remove(src_cbm, path);

    lib_free(src_cbm);

    return rc;
}

