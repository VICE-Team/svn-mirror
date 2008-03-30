/*
 * fsdevice-flush.c - File system device.
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

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef __riscos
#ifdef __IBMC__
#include <direct.h>
#include "snippets/dirport.h"
#else
#include <dirent.h>
#endif
#endif

#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif

#include "archdep.h"
#include "charset.h"
#include "fsdevice-open.h"
#include "fsdevice-resources.h"
#include "fsdevice-write.h"
#include "fsdevicetypes.h"
#include "lib.h"
#include "serial.h"
#include "vdrive-command.h"
#include "vdrive.h"


int fsdevice_open(vdrive_t *vdrive, const char *name, int length,
                  unsigned int secondary)
{
    FILE *fd;
    DIR *dp;
    BYTE *p;
    char fsname2[MAXPATHLEN], rname[MAXPATHLEN];
    char *mask, *comma;
    int status = 0, rc, i;
    cmd_parse_t cmd_parse;

    if (fs_info[secondary].fd)
        return FLOPPY_ERROR;

    memcpy(fsname2, name, length);
    fsname2[length] = 0;

    if (secondary == 15) {
        for (i = 0; i < length; i++)
            status = fsdevice_write(vdrive, name[i], 15);
        return status;
    }

    cmd_parse.cmd = name;
    cmd_parse.cmdlength = length;
    cmd_parse.readmode = (secondary == 1) ? FAM_WRITE : FAM_READ;

    rc = vdrive_command_parse(&cmd_parse);

    if (rc != SERIAL_OK) {
        status = SERIAL_ERROR;
        goto out;
    }

    fs_info[secondary].type = cmd_parse.filetype;

    if (fs_info[secondary].type == FT_DEL)
        fs_info[secondary].type = (secondary < 2) ? FT_PRG : FT_SEQ;

    /* Override read mode if secondary is 0 or 1.  */
    if (secondary == 0)
        cmd_parse.readmode = FAM_READ;
    if (secondary == 1)
        cmd_parse.readmode = FAM_WRITE;

    cmd_parse.parsecmd[cmd_parse.parselength] = 0;
    strncpy(rname, cmd_parse.parsecmd, cmd_parse.parselength);

    /* CBM name to FSname */
    charset_petconvstring((BYTE *)(cmd_parse.parsecmd), 1);

    switch (cmd_parse.readmode) {
      case FAM_WRITE:
        fs_info[secondary].mode = Write;
        break;
      case FAM_READ:
        fs_info[secondary].mode = Read;
        break;
      case FAM_APPEND:
        fs_info[secondary].mode = Append;
        break;
    }

    if (*name == '$') { /* Directory read */
        if ((secondary != 0) || (fs_info[secondary].mode != Read)) {
            fsdevice_error(vdrive, IPE_NOT_WRITE);
            status = FLOPPY_ERROR;
            goto out;
        }
        /* Test on wildcards.  */
        if (!(mask = strrchr(cmd_parse.parsecmd, '/')))
            mask = cmd_parse.parsecmd;
        if (strchr(mask, '*') || strchr(mask, '?')) {
            if (*mask == '/') {
                strcpy(fs_dirmask, mask + 1);
                *mask++ = 0;
            } else {
                strcpy(fs_dirmask, mask);
                strcpy(cmd_parse.parsecmd, fsdevice_get_path(vdrive->unit));
            }
        } else {
            *fs_dirmask = 0;
            if (!*(cmd_parse.parsecmd))
                strcpy(cmd_parse.parsecmd, fsdevice_get_path(vdrive->unit));
        }
        /* trying to open */
        if (!(dp = opendir((char *)(cmd_parse.parsecmd)))) {
            for (p = (BYTE *)(cmd_parse.parsecmd); *p; p++)
                if (isupper((int) *p))
                    *p = tolower((int)*p);
            if (!(dp = opendir((char *)(cmd_parse.parsecmd)))) {
                fsdevice_error(vdrive, IPE_NOT_FOUND);
                status = FLOPPY_ERROR;
                goto out;
            }
        }
        strcpy(fs_info[secondary].dir, cmd_parse.parsecmd);
        /*
         * Start Address, Line Link and Line number 0
         */

        p = fs_info[secondary].name;

        *p++ = 1;
        *p++ = 4;

        *p++ = 1;
        *p++ = 1;

        *p++ = 0;
        *p++ = 0;

        *p++ = (BYTE) 0x12;     /* Reverse on */

        *p++ = '"';
        strcpy((char *)p, fs_info[secondary].dir);     /* Dir name */
        charset_petconvstring((BYTE *)p, 0);   /* ASCII name to PETSCII */
        i = 0;
        while (*p) {
            ++p;
            i++;
        }
        while (i < 16) {
            *p++ = ' ';
            i++;
        }
        *p++ = '"';
        *p++ = ' ';
        *p++ = 'V';
        *p++ = 'I';
        *p++ = 'C';
        *p++ = 'E';
        *p++ = ' ';
        *p++ = 0;

        fs_info[secondary].buflen = p - fs_info[secondary].name;
        fs_info[secondary].bufp = fs_info[secondary].name;
        fs_info[secondary].mode = Directory;
        fs_info[secondary].dp = dp;
        fs_info[secondary].eof = 0;
    } else {                    /* Normal file, not directory ("$") */
        /* Override access mode if secondary address is 0 or 1.  */
        if (secondary == 0)
            fs_info[secondary].mode = Read;
        if (secondary == 1)
            fs_info[secondary].mode = Write;

        /* Remove comma.  */
        if ((cmd_parse.parsecmd)[0] == ',') {
            (cmd_parse.parsecmd)[1] = '\0';
        } else {
            comma = strchr(cmd_parse.parsecmd, ',');
            if (comma != NULL)
                *comma = '\0';
        }
        strcpy(fsname2, cmd_parse.parsecmd);
        strcpy(cmd_parse.parsecmd, fsdevice_get_path(vdrive->unit));
        strcat(cmd_parse.parsecmd, FSDEV_DIR_SEP_STR);
        strcat(cmd_parse.parsecmd, fsname2);

        /* Test on wildcards.  */
        if (strchr(fsname2, '*') || strchr(fsname2, '?')) {
            if (fs_info[secondary].mode == Write
                                || fs_info[secondary].mode == Append) {
                fsdevice_error(vdrive, IPE_BAD_NAME);
                status = FLOPPY_ERROR;
                goto out;
            } else {
                fsdevice_compare_file_name(vdrive, fsname2, cmd_parse.parsecmd,
                                           secondary);
            }
        }

        /* Open file for write mode access.  */
        if (fs_info[secondary].mode == Write) {
            fd = fopen(cmd_parse.parsecmd, MODE_READ);
            if (fd != NULL) {
                fclose(fd);
                fsdevice_error(vdrive, IPE_FILE_EXISTS);
                status = FLOPPY_ERROR;
                goto out;
            }
            if (fsdevice_convert_p00_enabled[(vdrive->unit) - 8]) {
                fd = fsdevice_find_pc64_name(vdrive, rname,
                                             cmd_parse.parselength, fsname2);
                if (fd != NULL) {
                    fclose(fd);
                    fsdevice_error(vdrive, IPE_FILE_EXISTS);
                    status = FLOPPY_ERROR;
                    goto out;
                }
            }
            if (fsdevice_save_p00_enabled[(vdrive->unit) - 8]) {
                if (fsdevice_create_file_p00(vdrive, rname,
                                             cmd_parse.parselength,
                                             cmd_parse.parsecmd,
                                             secondary) > 0) {
                    fsdevice_error(vdrive, IPE_FILE_EXISTS);
                    status = FLOPPY_ERROR;
                    goto out;
                } else {
                    fd = fopen(cmd_parse.parsecmd, MODE_APPEND_READ_WRITE);
                    fs_info[secondary].fd = fd;
                    fsdevice_error(vdrive, IPE_OK);
                    status = FLOPPY_COMMAND_OK;
                    goto out;
                }
            } else {
                fd = fopen(cmd_parse.parsecmd, MODE_WRITE);
                fs_info[secondary].fd = fd;
                fsdevice_error(vdrive, IPE_OK);
                status = FLOPPY_COMMAND_OK;
                goto out;
            }
        }

        /* Open file for append mode access.  */
        if (fs_info[secondary].mode == Append) {
            fd = fopen(cmd_parse.parsecmd, MODE_READ);
            if (!fd) {
                if (!fsdevice_convert_p00_enabled[(vdrive->unit) - 8]) {
                    fsdevice_error(vdrive, IPE_NOT_FOUND);
                    status = FLOPPY_ERROR;
                    goto out;
                }
                fd = fsdevice_find_pc64_name(vdrive, rname,
                                             cmd_parse.parselength, fsname2);
                if (!fd) {
                    fsdevice_error(vdrive, IPE_NOT_FOUND);
                    status = FLOPPY_ERROR;
                    goto out;
                }
                fclose(fd);
                fd = fopen(fsname2, MODE_APPEND_READ_WRITE);
                if (!fd) {
                    fsdevice_error(vdrive, IPE_NOT_FOUND);
                    status = FLOPPY_ERROR;
                    goto out;
                }
                fs_info[secondary].fd = fd;
                fsdevice_error(vdrive, IPE_OK);
                status = FLOPPY_COMMAND_OK;
                goto out;
            } else {
                fclose(fd);
                fd = fopen(cmd_parse.parsecmd, MODE_APPEND_READ_WRITE);
                if (!fd) {
                    fsdevice_error(vdrive, IPE_NOT_FOUND);
                    return FLOPPY_ERROR;
                }
                fs_info[secondary].fd = fd;
                fsdevice_error(vdrive, IPE_OK);
                status = FLOPPY_COMMAND_OK;
                goto out;
            }
        }

        /* Open file for read mode access.  */
        fd = fopen(cmd_parse.parsecmd, MODE_READ);
        if (!fd) {
            if (!fsdevice_convert_p00_enabled[(vdrive->unit) - 8]) {
                fsdevice_error(vdrive, IPE_NOT_FOUND);
                status = FLOPPY_ERROR;
                goto out;
            }
            fd = fsdevice_find_pc64_name(vdrive, rname, cmd_parse.parselength,
                                         fsname2);
            if (!fd) {
                fsdevice_error(vdrive, IPE_NOT_FOUND);
                status = FLOPPY_ERROR;
                goto out;
            }
            fs_info[secondary].fd = fd;
            fsdevice_error(vdrive, IPE_OK);
            status = FLOPPY_COMMAND_OK;
            goto out;
        } else {
            if (fsdevice_hide_cbm_files_enabled[vdrive->unit - 8]) {
                fclose(fd);
                fsdevice_error(vdrive, IPE_NOT_FOUND);
                status = FLOPPY_ERROR;
                goto out;
            }
            fs_info[secondary].fd = fd;
            fsdevice_error(vdrive, IPE_OK);
            status = FLOPPY_COMMAND_OK;
            goto out;
        }
    }
#ifdef __riscos
    archdep_set_drive_leds(vdrive->unit - 8, 1);
#endif
    fsdevice_error(vdrive, IPE_OK);

out:
    lib_free(cmd_parse.parsecmd);

    return status;
}


