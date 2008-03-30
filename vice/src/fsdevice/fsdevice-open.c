/*
 * fsdevice-open.c - File system device.
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

#include "archdep.h"
#include "charset.h"
#include "fileio.h"
#include "fsdevice-open.h"
#include "fsdevice-resources.h"
#include "fsdevice-write.h"
#include "fsdevicetypes.h"
#include "ioutil.h"
#include "lib.h"
#include "rawfile.h"
#include "vdrive-command.h"
#include "vdrive.h"


static int fsdevice_open_directory(vdrive_t *vdrive, unsigned int secondary,
                                   cmd_parse_t *cmd_parse)
{
    struct ioutil_dir_s *ioutil_dir;
    char *mask;
    BYTE *p;
    int i;

    if ((secondary != 0) || (fs_info[secondary].mode != Read)) {
        fsdevice_error(vdrive, IPE_NOT_WRITE);
        return FLOPPY_ERROR;
    }
    /* Test on wildcards.  */
    if (!(mask = strrchr(cmd_parse->parsecmd, '/')))
        mask = cmd_parse->parsecmd;
    if (strchr(mask, '*') || strchr(mask, '?')) {
        if (*mask == '/') {
            strcpy(fs_dirmask, mask + 1);
            *mask++ = 0;
        } else {
            strcpy(fs_dirmask, mask);
            strcpy(cmd_parse->parsecmd, fsdevice_get_path(vdrive->unit));
        }
    } else {
        *fs_dirmask = 0;
        if (!*(cmd_parse->parsecmd))
            strcpy(cmd_parse->parsecmd, fsdevice_get_path(vdrive->unit));
    }
    /* trying to open */
    ioutil_dir = ioutil_opendir((char *)(cmd_parse->parsecmd));
    if (ioutil_dir == NULL) {
        for (p = (BYTE *)(cmd_parse->parsecmd); *p; p++) {
            if (isupper((int)*p))
                *p = tolower((int)*p);
        }
        ioutil_dir = ioutil_opendir((char *)(cmd_parse->parsecmd));
        if (ioutil_dir == NULL) {
            fsdevice_error(vdrive, IPE_NOT_FOUND);
            return FLOPPY_ERROR;
        }
    }
    strcpy(fs_info[secondary].dir, cmd_parse->parsecmd);
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

    *p++ = (BYTE)0x12;     /* Reverse on */

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
    fs_info[secondary].ioutil_dir = ioutil_dir;
    fs_info[secondary].eof = 0;

    return FLOPPY_COMMAND_OK;
}

static int fsdevice_open_file(vdrive_t *vdrive, unsigned int secondary,
                              cmd_parse_t *cmd_parse, char *rname)
{
    char *comma;
    FILE *fd;
    char fsname2[MAXPATHLEN];
    tape_image_t *tape;

    unsigned int format = 0;
    fileio_info_t *finfo;

    if (fsdevice_convert_p00_enabled[(vdrive->unit) - 8])
        format |= FILEIO_FORMAT_P00;
    if (!fsdevice_hide_cbm_files_enabled[vdrive->unit - 8])
        format |= FILEIO_FORMAT_RAW;

    /* Override access mode if secondary address is 0 or 1.  */
    if (secondary == 0)
        fs_info[secondary].mode = Read;
    if (secondary == 1)
        fs_info[secondary].mode = Write;

    /* Remove comma.  */
    if ((cmd_parse->parsecmd)[0] == ',') {
        (cmd_parse->parsecmd)[1] = '\0';
    } else {
        comma = strchr(cmd_parse->parsecmd, ',');
        if (comma != NULL)
            *comma = '\0';
    }
    strcpy(fsname2, cmd_parse->parsecmd);
    strcpy(cmd_parse->parsecmd, fsdevice_get_path(vdrive->unit));
    strcat(cmd_parse->parsecmd, FSDEV_DIR_SEP_STR);
    strcat(cmd_parse->parsecmd, fsname2);

    /* Test on wildcards.  */
    if (strchr(fsname2, '*') || strchr(fsname2, '?')) {
        if (fs_info[secondary].mode == Write
            || fs_info[secondary].mode == Append) {
            fsdevice_error(vdrive, IPE_BAD_NAME);
            return FLOPPY_ERROR;
        } else {
            fsdevice_compare_file_name(vdrive, fsname2, cmd_parse->parsecmd,
                                       secondary);
        }
    }

    /* Open file for write mode access.  */
    if (fs_info[secondary].mode == Write) {
        if (fsdevice_save_p00_enabled[vdrive->unit - 8])
            format = FILEIO_FORMAT_P00;
        else
            format = FILEIO_FORMAT_RAW;

        finfo = fileio_open(rname, fsdevice_get_path(vdrive->unit), format,
                            FILEIO_COMMAND_WRITE, fs_info[secondary].type);

        if (finfo != NULL) {
            fs_info[secondary].fd = (FILE *)(finfo->rawfile->fd);
            fs_info[secondary].info = finfo;
            fsdevice_error(vdrive, IPE_OK);
            return FLOPPY_COMMAND_OK;
        } else {
            fsdevice_error(vdrive, IPE_FILE_EXISTS);
            return FLOPPY_ERROR;
        }
    }

    /* Open file for append mode access.  */
    finfo = fileio_open(rname, fsdevice_get_path(vdrive->unit), format,
                        FILEIO_COMMAND_APPEND_READ, fs_info[secondary].type);

    if (finfo != NULL) {
        fs_info[secondary].fd = (FILE *)(finfo->rawfile->fd);
        fs_info[secondary].info = finfo;
        fsdevice_error(vdrive, IPE_OK);
        return FLOPPY_COMMAND_OK;
    } else {
        fsdevice_error(vdrive, IPE_NOT_FOUND);
        return FLOPPY_ERROR;
    }

    /* Open file for read mode access.  */
    tape = &(fs_info[secondary].tape);
    tape->name = lib_stralloc(cmd_parse->parsecmd);
    tape->read_only = 1;
    if (tape_image_open(tape) < 0) {
        lib_free(tape->name);
        tape->name = NULL;
    } else {
        tape_file_record_t *r;
        static BYTE startaddr[2];
        tape_seek_start(tape);
        r = tape_get_current_file_record(tape);
        startaddr[0] = r->start_addr & 255;
        startaddr[1] = r->start_addr >> 8;
        fs_info[secondary].bufp = startaddr;
        fs_info[secondary].buflen = 2;
        return FLOPPY_COMMAND_OK;
    }

    finfo = fileio_open(rname, fsdevice_get_path(vdrive->unit), format,
                        FILEIO_COMMAND_READ, fs_info[secondary].type);

    if (finfo != NULL) {
        fs_info[secondary].fd = (FILE *)(finfo->rawfile->fd);
        fs_info[secondary].info = finfo;
        fsdevice_error(vdrive, IPE_OK);
        return FLOPPY_COMMAND_OK;
    } else {
        fsdevice_error(vdrive, IPE_NOT_FOUND);
        return FLOPPY_ERROR;
    }

    return FLOPPY_COMMAND_OK;
}

int fsdevice_open(vdrive_t *vdrive, const char *name, int length,
                  unsigned int secondary)
{
    char rname[MAXPATHLEN];
    int status = 0, rc, i;
    cmd_parse_t cmd_parse;

    if (fs_info[secondary].fd)
        return FLOPPY_ERROR;

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
    strncpy(rname, cmd_parse.parsecmd, cmd_parse.parselength + 1);

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

    if (*name == '$') {
        status = fsdevice_open_directory(vdrive, secondary, &cmd_parse);
    } else {
        status = fsdevice_open_file(vdrive, secondary, &cmd_parse, rname);
    }

    if (status != FLOPPY_COMMAND_OK)
        goto out;


#ifdef __riscos
    archdep_set_drive_leds(vdrive->unit - 8, 1);
#endif
    fsdevice_error(vdrive, IPE_OK);

out:
    lib_free(cmd_parse.parsecmd);

    return status;
}


