/*
 * fsdevice-read.c - File system device.
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
 *
 * Based on old code by
 *  Teemu Rantanen <tvr@cs.hut.fi>
 *  Jarkko Sonninen <sonninen@lut.fi>
 *  Jouko Valta <jopi@stekt.oulu.fi>
 *  Olaf Seibert <rhialto@mbfys.kun.nl>
 *  André Fachat <a.fachat@physik.tu-chemnitz.de>
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Martin Pottendorfer <Martin.Pottendorfer@aut.alcatel.at>
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

#ifdef HAVE_IO_H
#include <io.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "archdep.h"
#include "fileio.h"
#include "fsdevice-read.h"
#include "fsdevice-resources.h"
#include "fsdevicetypes.h"
#include "ioutil.h"
#include "types.h"
#include "vdrive.h"


static int command_read(fs_buffer_info_t *info, BYTE *data)
{
    int i;

    if (info->tape.name) {
        if (info->buflen > 0) {
            *data = *info->bufp++;
            info->buflen--;
        }
        else if (tape_read(&(info->tape), data, 1) != 1) {
            *data = 0xc7;
            return SERIAL_EOF;
        }

        return SERIAL_OK;
    }
    else if (info->fd) {
        i = fgetc(info->fd);
        if (ferror(info->fd))
            return FLOPPY_ERROR;
        if (feof(info->fd)) {
            *data = 0xc7;
            return SERIAL_EOF;
        }
        *data = (BYTE)i;
        return SERIAL_OK;
    }

    return FLOPPY_ERROR;
}

static void command_directory_get(vdrive_t *vdrive, fs_buffer_info_t *info,
                                  BYTE *data, unsigned int secondary)
{
    int i, l, f;
    unsigned int blocks;
    char *direntry;
#ifdef __riscos
    int objType;
    int catInfo[4];
#else
    struct stat statbuf;
#endif
    fileio_info_t *finfo = NULL;
    unsigned int format = 0;
    char buf[MAXPATHLEN];

    info->bufp = info->name;

    if (fsdevice_convert_p00_enabled[(vdrive->unit) - 8])
        format |= FILEIO_FORMAT_P00;
    if (!fsdevice_hide_cbm_files_enabled[vdrive->unit - 8])
        format |= FILEIO_FORMAT_RAW;

    /*
     * Find the next directory entry and return it as a CBM
     * directory line.
     */

    /* first test if dirmask is needed - maybe this should be
       replaced by some regex functions... */
    f = 1;
    do {
        char *p;

        direntry = ioutil_readdir(info->ioutil_dir);

        if (direntry == NULL)
            break;

        finfo = fileio_open(direntry, fs_info[secondary].dir, format,
                            FILEIO_COMMAND_READ | FILEIO_COMMAND_FSNAME,
                            FILEIO_TYPE_PRG);

        if (finfo == NULL)
            continue;

        fs_info[secondary].type = finfo->type;

        if (!*fs_dirmask)
            break;
        l = strlen(fs_dirmask);

        for (p = finfo->name, i = 0; *p && fs_dirmask[i] && i < l; i++) {
            if (fs_dirmask[i] == '?') {
                p++;
            } else if (fs_dirmask[i] == '*') {
                if (!fs_dirmask[i + 1]) {
                    f = 0;
                    break;
                } /* end mask */
                while (*p && (*p != fs_dirmask[i + 1]))
                    p++;
            } else {
                if (*p != fs_dirmask[i])
                    break;
                p++;
            }
            if ((!*p) && (!fs_dirmask[i + 1])) {
                f = 0;
                break;
            }
        }
        if (f > 0)
            fileio_close(finfo);
    } while (f);
    if (direntry != NULL) {
        BYTE *p = info->name;
        char *tp;

        strcpy(buf, info->dir);
        strcat(buf, FSDEV_DIR_SEP_STR);
        tp = buf + strlen(buf);
        strcat(buf, direntry);

        /* Line link, Length and spaces */

        *p++ = 1;
        *p++ = 1;
#ifdef __riscos
        if ((objType = ReadCatalogueInfo(buf, catInfo)) != 0)
            blocks = (unsigned short)((catInfo[2] + 253) / 254);
#else
        if (stat(buf, &statbuf) >= 0)
            blocks = (unsigned short)((statbuf.st_size + 253) / 254);
#endif
        else
            blocks = 0;   /* this file can't be opened */

        if (blocks > 0xffff)
            blocks = 0xffff; /* limit file size to 16 bits */

        SET_LO_HI(p, blocks);

        if (blocks < 10)
            *p++ = ' ';
        if (blocks < 100)
            *p++ = ' ';
        if (blocks < 1000)
            *p++ = ' ';

        /*
         * Filename
         */

        *p++ = '"';

        for (i = 0; finfo->name[i] && (*p = finfo->name[i]); ++i, ++p);

        *p++ = '"';
        for (; i < 16; i++)
            *p++ = ' ';

#ifdef __riscos
        if ((objType & 2) != 0)
#else
        if (S_ISDIR(statbuf.st_mode))
#endif
        {
            *p++ = ' '; /* normal file */
            *p++ = 'D';
            *p++ = 'I';
            *p++ = 'R';
        } else {
            if (blocks)
                *p++ = ' '; /* normal file */
            else
                *p++ = '*'; /* splat file */
            switch(fs_info[secondary].type) {
              case FT_DEL:
                *p++ = 'D';
                *p++ = 'E';
                *p++ = 'L';
                break;
              case FT_SEQ:
                *p++ = 'S';
                *p++ = 'E';
                *p++ = 'Q';
                break;
              case FT_PRG:
                *p++ = 'P';
                *p++ = 'R';
                *p++ = 'G';
                break;
              case FT_USR:
                *p++ = 'U';
                *p++ = 'S';
                *p++ = 'R';
                break;
              case FT_REL:
                *p++ = 'R';
                *p++ = 'E';
                *p++ = 'L';
                break;
            }
        }
        if (access(buf, W_OK))
            *p++ = '<'; /* read-only file */

        *p = '\0';        /* to allow strlen */

        /* some (really very) old programs rely on the directory
           entry to be 32 Bytes in total (incl. nullbyte) */
        l = strlen((char *)(info->name + 4)) + 4;
        while (l < 31) {
            *p++ = ' ';
            l++;
        }

        *p++ = '\0';

        info->buflen = (int)(p - info->name);

    } else {
        BYTE *p = info->name;

        /* EOF => End file */

        *p++ = 1;
        *p++ = 1;
        *p++ = 0;
        *p++ = 0;
        memcpy(p, "BLOCKS FREE.", 12);
        p += 12;
        memset(p, ' ', 13);
        p += 13;

        memset(p, 0, 3);
        info->buflen = 32;
        info->eof++;
    }
    fileio_close(finfo);
}


static int command_directory(vdrive_t *vdrive, fs_buffer_info_t *info,
                             BYTE *data, unsigned int secondary)
{
    if (info->ioutil_dir == NULL)
        return FLOPPY_ERROR;

    if (info->buflen <= 0) {
        if (info->eof) {
            *data = 0xc7;
            return SERIAL_EOF;
        }
        command_directory_get(vdrive, info, data, secondary);
    }

    *data = *info->bufp++;
    info->buflen--;

    return SERIAL_OK;
}

int fsdevice_read(vdrive_t *vdrive, BYTE *data, unsigned int secondary)
{
    fs_buffer_info_t *info = &fs_info[secondary];

    if (secondary == 15)
        return fsdevice_error_get_byte(vdrive, data);

    switch (info->mode) {
      case Write:
      case Append:
        return FLOPPY_ERROR;
      case Read:
        return command_read(info, data);
      case Directory:
        return command_directory(vdrive, info, data, secondary);
    }
    return FLOPPY_ERROR;
}

