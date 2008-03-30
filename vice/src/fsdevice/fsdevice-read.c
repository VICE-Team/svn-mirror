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

#include "archdep.h"
#include "charset.h"
#include "fsdevice-read.h"
#include "fsdevice-resources.h"
#include "fsdevicetypes.h"
#include "serial.h"
#include "types.h"
#include "vdrive-command.h"
#include "vdrive.h"


int fsdevice_read(vdrive_t *vdrive, BYTE *data, unsigned int secondary)
{
    int i, l, f;
    unsigned short blocks;
    struct dirent *dirp;        /* defined in /usr/include/sys/dirent.h */
#ifdef __riscos
    int objType;
    int catInfo[4];
#else
    struct stat statbuf;
#endif
    fs_buffer_info_t *info = &fs_info[secondary];
    char rname[256];

    if (secondary == 15) {
        if (!fs_elen)
            fsdevice_error(vdrive, IPE_OK);
        if (fs_eptr < fs_elen) {
            *data = (BYTE)fs_errorl[fs_eptr++];
            return SERIAL_OK;
        } else {
            fsdevice_error(vdrive, IPE_OK);
            *data = 0xc7;
            return SERIAL_EOF;
        }
    }
    switch (info->mode) {
      case Write:
      case Append:
        return FLOPPY_ERROR;

      case Read:
        if (info->fd) {
            i = fgetc(info->fd);
            if (ferror(info->fd))
                return FLOPPY_ERROR;
            if (feof(info->fd)) {
                *data = 0xc7;
                return SERIAL_EOF;
            }
            *data = i;
            return SERIAL_OK;
        }
        break;
      case Directory:
        if (info->dp) {
            if (info->buflen <= 0) {
                char buf[MAXPATHLEN];

                info->bufp = info->name;

                if (info->eof) {
                    *data = 0xc7;
                    return SERIAL_EOF;
                }
                /*
                 * Find the next directory entry and return it as a CBM
                 * directory line.
                 */

                /* first test if dirmask is needed - maybe this should be
                   replaced by some regex functions... */
                f = 1;
                do {
                    char *p;
                    dirp = readdir(info->dp);
                    if (!dirp)
                        break;
                    fs_info[secondary].type = FT_PRG;
                    strcpy(rname, dirp->d_name);
                    if (fsdevice_convert_p00_enabled[(vdrive->unit) - 8])
                        fsdevice_test_pc64_name(vdrive, rname, secondary);
                        if (strcmp(rname, dirp->d_name) == 0
                        && fsdevice_hide_cbm_files_enabled[vdrive->unit - 8])
                            continue;
                    if (!*fs_dirmask)
                        break;
                    l = strlen(fs_dirmask);
                    for (p = rname, i = 0; *p && fs_dirmask[i] && i < l;
                        i++) {
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
                } while (f);
                if (dirp != NULL) {
                    BYTE *p = info->name;
                    char *tp;

                    strcpy(buf, info->dir);
                    strcat(buf, FSDEV_DIR_SEP_STR);
                    tp = buf + strlen(buf);
                    strcat(buf, dirp->d_name);

                    /* Line link, Length and spaces */

                    *p++ = 1;
                    *p++ = 1;
#ifdef __riscos
                    if ((objType = ReadCatalogueInfo(buf, catInfo)) != 0)
                        blocks = (unsigned short) ((catInfo[2] + 253) / 254);
#else
                    if (stat(buf, &statbuf) >= 0)
                        blocks = (unsigned short) ((statbuf.st_size + 253)
                                 / 254);
#endif
                    else
                        blocks = 0;   /* this file can't be opened */

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

                    if (strcmp(rname, dirp->d_name)) {
                        for (i = 0; rname[i] && (*p = rname[i]); ++i, ++p);
                    } else {
                        for (i = 0; tp[i] /*i < dirp->d_namlen */ &&
                             (*p = charset_p_topetcii(tp[i])); ++i, ++p);
                    }

                    *p++ = '"';
                    for (; i < 17; i++)
                        *p++ = ' ';

#ifdef __riscos
                    if ((objType & 2) != 0)
#else
                    if (S_ISDIR(statbuf.st_mode))
#endif
                    {
                        *p++ = 'D';
                        *p++ = 'I';
                        *p++ = 'R';
                    } else {
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

                    *p = '\0';        /* to allow strlen */

                    /* some (really very) old programs rely on the directory
                       entry to be 32 Bytes in total (incl. nullbyte) */
                    l = strlen((char *) (info->name + 4)) + 4;
                    while (l < 31) {
                        *p++ = ' ';
                        l++;
                    }

                    *p++ = '\0';

                    info->buflen = (int) (p - info->name);

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
            }                 /* info->buflen */
            *data = *info->bufp++;
            info->buflen--;
            return SERIAL_OK;
        }                     /* info->dp */
        break;
    }
    return FLOPPY_ERROR;
}

