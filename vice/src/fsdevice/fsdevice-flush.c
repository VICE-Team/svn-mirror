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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif

#include "archdep.h"
#include "charset.h"
#include "fsdevice-flush.h"
#include "fsdevice-resources.h"
#include "fsdevice.h"
#include "fsdevicetypes.h"
#include "ioutil.h"
#include "serial.h"
#include "types.h"
#include "utils.h"
#include "vdrive-command.h"
#include "vdrive.h"


static BYTE fs_cmdbuf[4][MAXPATHLEN];
static unsigned int fs_cptr[4] = { 0, 0, 0, 0 };


static int fsdevice_flush_mr(vdrive_t *vdrive)
{
    unsigned int dnr;
    WORD addr;

    dnr = vdrive->unit - 8;
    addr = fs_cmdbuf[dnr][3] | (fs_cmdbuf[dnr][4] << 8);

    return vdrive_command_memory_read(vdrive, addr, fs_cmdbuf[dnr][5]);
}

static int fsdevice_flush_cd(vdrive_t* vdrive, char *arg)
{
    int er;

    er = IPE_OK;
    if (ioutil_chdir(fsdevice_get_path(vdrive->unit)) || ioutil_chdir(arg)) {
        er = IPE_NOT_FOUND;
        if (errno == EPERM)
            er = IPE_PERMISSION;
    } else { /* get full path and save */
        arg = ioutil_current_dir();
        fsdevice_set_directory(arg, vdrive->unit);
        free(arg);
    }

    return er;
}

static int fsdevice_flush_cdup(vdrive_t* vdrive)
{
    return fsdevice_flush_cd(vdrive, "..");
}

static int fsdevice_flush_reset(void)
{
    return IPE_DOS_VERSION;
}

static int fsdevice_flush_mkdir(char *arg)
{
    int er;

    er = IPE_OK;
    if (ioutil_mkdir(arg, 0770)) {
        er = IPE_INVAL;
        if (errno == EEXIST)
            er = IPE_FILE_EXISTS;
        if (errno == EACCES)
            er = IPE_PERMISSION;
        if (errno == ENOENT)
            er = IPE_NOT_FOUND;
    }

    return er;
}

static int fsdevice_flush_partition(vdrive_t* vdrive, char* arg)
{
    char* comma;
    int er;

    if (arg == NULL || *arg == '\0')
        er = IPE_SYNTAX; /* change to root partition not implemented */
    else if ((comma = strchr(arg, ',')) == NULL)
        er = fsdevice_flush_cd(vdrive, arg);
    else { /* create partition: check syntax */
        int i = 0;
        *comma++ = '\0';
        for (i=0; i<4 && *comma++; i++);
        if (i==4 && *comma++ == ',' && *comma++ == 'c' && !*comma)
            er = fsdevice_flush_mkdir(arg);
        else
            er = IPE_SYNTAX;
    }
    return er;
}

static int fsdevice_flush_remove(char *arg)
{
    int er;

    er = IPE_OK;
    if (ioutil_remove(arg)) {
        er = IPE_NOT_EMPTY;
        if (errno == EPERM)
            er = IPE_PERMISSION;
    }

    return er;
}

static int fsdevice_flush_scratch(vdrive_t *vdrive, char *arg, char *realarg)
{
    int er;
    char name[MAXPATHLEN];
    FILE *fd;

    er = IPE_DELETED;
    fd = fsdevice_find_pc64_name(vdrive, realarg, strlen(realarg), name);

    if (fd != NULL) {
        fclose(fd);
    } else {
        if (fsdevice_hide_cbm_files_enabled[vdrive->unit - 8])
            return IPE_NOT_FOUND;
        strcpy(name, fsdevice_get_path(vdrive->unit));
        strcat(name, FSDEV_DIR_SEP_STR);
        strcat(name, arg);
    }

    if (ioutil_remove(name)) {
        er = IPE_NOT_FOUND;
        if (errno == EPERM)
            er = IPE_PERMISSION;
    }

    return er;
}

static int fsdevice_flush_rename(vdrive_t *vdrive, char *arg, char *realarg)
{
    int er = IPE_SYNTAX;
    char name1[MAXPATHLEN], name2[MAXPATHLEN];
    char *realarg2 = NULL, *arg2 = NULL;
    FILE *fd;

    if ((arg2 = strchr(arg, '='))) {
        char name2long[MAXPATHLEN];
        er = IPE_OK;
        *arg2++ = 0;
        realarg2 = strchr(realarg, '=');
        *realarg2++ = 0;
        fd = fsdevice_find_pc64_name(vdrive, realarg2, strlen(realarg2),
                                     name2long);
        if (fd != NULL) {
            /* Rename P00 file.  */
            int name1len;
            char *p, p00name[17], p00type, p00count[2];
            char name1p00[MAXPATHLEN], name2p00[MAXPATHLEN];
            fclose(fd);
            strcpy(name2p00, name2long);
            p = strrchr(name2long, FSDEV_EXT_SEP_CHR);
            p00type = p[1];
            *p = '\0';
            p = strrchr(name2long, FSDEV_DIR_SEP_CHR);
            strcpy(name2, ++p);
            name1len = fsdevice_evaluate_name_p00(realarg, strlen(realarg),
                                                  name1);
            name1[name1len] = '\0';
            memset(p00name, 0, 17);
            strncpy(p00name, realarg, 16);
            fd = fopen(name2p00, MODE_READ_WRITE);
            if (fd) {
                if ((fseek(fd, 8, SEEK_SET) != 0)
                    || (fwrite(p00name, 16, 1, fd) < 1))
                    er = IPE_NOT_FOUND;
                fclose(fd);
            } else {
                er = IPE_NOT_FOUND;
            }
            if (er == IPE_OK && strcmp(name1, name2) != 0) {
                int i;
                for (i = 0; i < 100; i++) {
                    memset(name1p00, 0, MAXPATHLEN);
                    strcpy(name1p00, fsdevice_get_path(vdrive->unit));
                    strcat(name1p00, FSDEV_DIR_SEP_STR);
                    strcat(name1p00, name1);
                    strcat(name1p00, FSDEV_EXT_SEP_STR);
                    strncat(name1p00, &p00type, 1);
                    sprintf(p00count, "%02i", i);
                    strncat(name1p00, p00count, 2);
                    fd = fopen(name1p00, MODE_READ);
                    if (fd) {
                        fclose(fd);
                        continue;
                    }

                    ioutil_remove(name1p00);
                    if (ioutil_rename(name2p00, name1p00) == 0)
                        break;
                }
            }
        } else {
            /* Rename CBM file.  */
            if (fsdevice_hide_cbm_files_enabled[vdrive->unit - 8])
                return IPE_NOT_FOUND;

            strcpy(name1, fsdevice_get_path(vdrive->unit));
            strcat(name1, FSDEV_DIR_SEP_STR);
            strcat(name1, arg);
            strcpy(name2, fsdevice_get_path(vdrive->unit));
            strcat(name2, FSDEV_DIR_SEP_STR);
            strcat(name2, arg2);

            ioutil_remove(name1);
            if (ioutil_rename(name2, name1)) {
                er = IPE_NOT_FOUND;
                if (errno == EPERM)
                    er = IPE_PERMISSION;
            }
        }
    }

    return er;
}

void fsdevice_flush(vdrive_t *vdrive, unsigned int secondary)
{
    unsigned int dnr;
    char *cmd, *realarg, *arg;
    char cbmcmd[MAXPATHLEN];
    int er = IPE_SYNTAX;

    dnr = vdrive->unit - 8;

    if (secondary != 15 || !fs_cptr[dnr])
        return;

    /* FIXME: Use `vdrive_command_parse()'! */
    /* remove trailing cr */
    while (fs_cptr[dnr] && (fs_cmdbuf[dnr][fs_cptr[dnr] - 1] == 13))
        fs_cptr[dnr]--;

    fs_cmdbuf[dnr][fs_cptr[dnr]] = 0;

    strcpy(cbmcmd, fs_cmdbuf[dnr]);
    charset_petconvstring((BYTE *)cbmcmd, 1);   /* CBM name to FSname */
    cmd = cbmcmd;

    while (*cmd == ' ')
        cmd++;

    arg = strchr(cbmcmd, ':');

    if (arg) {
        *arg++ = '\0';
    }

    realarg = strchr(fs_cmdbuf[dnr], ':');

    if (realarg) {
        *realarg++ = '\0';
    }

    if (!strncmp(fs_cmdbuf[dnr], "M-R", 3)) {
        er = fsdevice_flush_mr(vdrive);
    } else if (!strcmp(cmd, "cd")) {
        er = fsdevice_flush_cd(vdrive, arg);
    } else if (!strcmp(fs_cmdbuf[dnr], "CD_")) {
        er = fsdevice_flush_cdup(vdrive);
    } else if (*cmd == '/') {
        er = fsdevice_flush_partition(vdrive, arg);
    } else if (!strcmp(cmd, "md")) {
        er = fsdevice_flush_mkdir(arg);
    } else if (!strcmp(cmd, "rd")) {
        er = fsdevice_flush_remove(arg);
    } else if (!strcmp(cmd, "ui")) {
        er = fsdevice_flush_reset();
    } else if (*cmd == 's' && arg != NULL) {
        er = fsdevice_flush_scratch(vdrive, arg, realarg);
    } else if (*cmd == 'r' && arg != NULL) {
        er = fsdevice_flush_rename(vdrive, arg, realarg);
    }

    fsdevice_error(vdrive, er);

    fs_cptr[dnr] = 0;
}

int fsdevice_flush_write_byte(vdrive_t *vdrive, BYTE data)
{
    unsigned int dnr;
    int rc;

    dnr = vdrive->unit - 8;
    rc = SERIAL_OK;

    /* FIXME: Consider the real size of the input buffer. */
    if (fs_cptr[dnr] < MAXPATHLEN - 1) {
        fs_cmdbuf[dnr][fs_cptr[dnr]++] = data;
        rc = SERIAL_OK;
    } else {
        fsdevice_error(vdrive, IPE_LONG_LINE);
        rc = SERIAL_ERROR;
    }

    return rc;
}

