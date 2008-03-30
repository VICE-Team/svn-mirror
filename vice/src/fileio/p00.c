/*
 * p00.c - Utility functions for P00 file support.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Andreas Boose <viceteam@t-online.de>
 *
 * Based on older code by
 *  Dan Fandrich <dan@fch.wimsey.bc.ca>
 *  Jouko Valta <jopi@stekt.oulu.fi>
 *  Martin Pottendorfer <Martin.Pottendorfer@autalcatel.at>
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

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "archdep.h"
#include "fileio.h"
#include "ioutil.h"
#include "lib.h"
#include "log.h"
#include "p00.h"
#include "rawfile.h"
#include "types.h"


/* FIXME: Remove this. */
/* File Types */
#define FT_DEL          0
#define FT_SEQ          1
#define FT_PRG          2
#define FT_USR          3
#define FT_REL          4
#define FT_CBM          5       /* 1581 partition */
#define FT_DJJ          6       /* 1581 */
#define FT_FAB          7       /* 1581 - Fred's format */
#define FT_REPLACEMENT  0x20
#define FT_LOCKED       0x40
#define FT_CLOSED       0x80

/* P00 Header structure:

   typedef struct {
       BYTE Magic[8];
       char CbmName[17];
       BYTE RecordSize;
   } X00HDR; */

#define P00_HDR_MAGIC_OFFSET      0
#define P00_HDR_MAGIC_LEN         8
#define P00_HDR_CBMNAME_OFFSET    8
#define P00_HDR_CBMNAME_LEN       17
#define P00_HDR_RECORDSIZE_OFFSET 25
#define P00_HDR_RECORDSIZE_LEN    1

#define P00_HDR_LEN               26

static const BYTE p00_hdr_magic_string[8] = "C64File";

/* FIXME: There should be an enum for file types.  */
int p00_check_name(const char *name)
{
    int t = -1;
    char *p;

    if (name == NULL || (p = strrchr(name, '.')) == NULL || strlen(++p) != 3)
        return -1;

    if (!isdigit((int)p[1]) || !isdigit((int)p[2]))
        return -1;

    switch (toupper(*p)) {
      case 'D':
        t = FT_DEL;
        break;
      case 'S':
        t = FT_SEQ;
        break;
      case 'P':
        t = FT_PRG;
        break;
      case 'U':
        t = FT_USR;
        break;
      case 'R':
        t = FT_REL;
        break;
    }

    return t;
}

int p00_read_header(FILE *fd, BYTE *cbmname_return,
                    unsigned int *recsize_return)
{
    BYTE hdr[P00_HDR_LEN];

    if (fread((char *)&hdr, P00_HDR_LEN, 1, fd) != 1)
        return -1;

    if (memcmp(hdr + P00_HDR_MAGIC_OFFSET, p00_hdr_magic_string,
               P00_HDR_MAGIC_LEN) != 0)
        return -1;

    memcpy(cbmname_return, hdr + P00_HDR_CBMNAME_OFFSET, P00_HDR_CBMNAME_LEN);

    if (recsize_return != NULL)
        *recsize_return = (unsigned int)hdr[P00_HDR_RECORDSIZE_OFFSET];

    return 0;
}

int p00_write_header(FILE *fd, BYTE *cbmname, BYTE recsize)
{
    BYTE hdr[P00_HDR_LEN];

    memset(hdr, 0, sizeof(hdr));

    memcpy(hdr + P00_HDR_MAGIC_OFFSET, p00_hdr_magic_string,
           P00_HDR_MAGIC_LEN);
    memcpy(hdr + P00_HDR_CBMNAME_OFFSET, cbmname, P00_HDR_CBMNAME_LEN);
    hdr[P00_HDR_RECORDSIZE_OFFSET] = (BYTE) recsize;

    if (fseek(fd, 0, SEEK_SET) != 0)
        return -1;

    return fwrite(hdr, sizeof(hdr), 1, fd);
}

static char *p00_find_file(const char *file_name, const char *path)
{
    struct ioutil_dir_s *ioutil_dir;
    struct rawfile_info_s *rawfile;
    char p00_header_file_name[20]; /* FIXME */
    char *name, *alloc_name = NULL;
    int rc;

    ioutil_dir = ioutil_opendir(path);

    if (ioutil_dir == NULL)
        return NULL;

    while (1) {
        name = ioutil_readdir(ioutil_dir);

        if (name == NULL)
            break;

        if (p00_check_name(name) < 0)
            continue;

        rawfile = rawfile_open(name, path, FILEIO_COMMAND_READ);
        if (rawfile == NULL)
            continue;

        rc = p00_read_header((FILE *)(rawfile->fd),
                             (BYTE *)p00_header_file_name, NULL);

        if (rc >= 0)
            alloc_name = lib_stralloc(name);

        rawfile_destroy(rawfile);

        if (rc >= 0)
            break;
    }

    ioutil_closedir(ioutil_dir);

    return alloc_name;
}

fileio_info_t *p00_info(const char *file_name, const char *path,
                        unsigned int command)
{
    int p00_type;
    char p00_header_file_name[20]; /* FIXME */
    fileio_info_t *info;
    struct rawfile_info_s *rawfile;
    char *fname = NULL;

    if (/*command & FILEIO_COMMAND_FSNAME*/ 1)
        fname = lib_stralloc(file_name);
    else
        fname = p00_find_file(file_name, path);

    if (fname == NULL)
        return NULL;

    rawfile = rawfile_open(fname, path, command & FILEIO_COMMAND_MASK);

    if (rawfile == NULL) {
        lib_free(fname);
        return NULL;
    }

    p00_type = p00_check_name(fname);

    if (p00_type < 0
        || p00_read_header((FILE *)(rawfile->fd),
                           (BYTE *)p00_header_file_name, NULL) < 0) {
        rawfile_destroy(rawfile);
        lib_free(fname);
        return NULL;
    }

    info = (fileio_info_t *)lib_malloc(sizeof(fileio_info_t));
    info->name = (BYTE *)lib_stralloc(p00_header_file_name);;
    info->type = (unsigned int)p00_type;
    info->format = FILEIO_FORMAT_P00;
    info->rawfile = rawfile;

    lib_free(fname);

    return info;
}

