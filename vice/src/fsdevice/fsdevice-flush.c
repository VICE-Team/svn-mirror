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
#include "fileio.h"
#include "fsdevice-flush.h"
#include "fsdevice-resources.h"
#include "fsdevice.h"
#include "fsdevicetypes.h"
#include "ioutil.h"
#include "lib.h"
#include "types.h"
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
        lib_free(arg);
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
        for (i = 0; i < 4 && *comma++; i++);
        if (i == 4 && *comma++ == ',' && *comma++ == 'c' && !*comma)
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

static int fsdevice_flush_scratch(vdrive_t *vdrive, char *realarg)
{
    unsigned int format = 0, rc;

    if (realarg[0] == '\0')
        return IPE_SYNTAX;

    if (fsdevice_convert_p00_enabled[(vdrive->unit) - 8])
        format |= FILEIO_FORMAT_P00;
    if (!fsdevice_hide_cbm_files_enabled[vdrive->unit - 8])
        format |= FILEIO_FORMAT_RAW;

    rc = fileio_scratch(realarg, fsdevice_get_path(vdrive->unit), format);

    switch (rc) {
      case FILEIO_FILE_NOT_FOUND:
        return IPE_NOT_FOUND;
      case FILEIO_FILE_PERMISSION:
        return IPE_PERMISSION;
      case FILEIO_FILE_SCRATCHED:
        return IPE_DELETED;
    }

    return IPE_OK;
}

static int fsdevice_flush_rename(vdrive_t *vdrive, char *realarg)
{
    char *src, *dest, *tmp;
    unsigned int format = 0, rc;

    tmp = strchr(realarg, '=');

    if (tmp == NULL)
        return IPE_SYNTAX;

    if (tmp == realarg)
        return IPE_SYNTAX;

    if (tmp[1] == '\0')
        return IPE_SYNTAX;

    tmp[0] = '\0';

    src = &tmp[1];
    dest = realarg;

    if (fsdevice_convert_p00_enabled[(vdrive->unit) - 8])
        format |= FILEIO_FORMAT_P00;
    if (!fsdevice_hide_cbm_files_enabled[vdrive->unit - 8])
        format |= FILEIO_FORMAT_RAW;

    rc = fileio_rename(src, dest, fsdevice_get_path(vdrive->unit), format);

    switch (rc) {
      case FILEIO_FILE_NOT_FOUND:
        return IPE_NOT_FOUND;
      case FILEIO_FILE_EXISTS:
        return IPE_FILE_EXISTS;
      case FILEIO_FILE_PERMISSION:
        return IPE_PERMISSION;
    }

    return IPE_OK;
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

    strcpy(cbmcmd, (char *)fs_cmdbuf[dnr]);
    charset_petconvstring((BYTE *)cbmcmd, 1);   /* CBM name to FSname */
    cmd = cbmcmd;

    while (*cmd == ' ')
        cmd++;

    arg = strchr(cbmcmd, ':');

    if (arg != NULL) {
        *arg++ = '\0';
    }

    realarg = strchr((char *)fs_cmdbuf[dnr], ':');

    if (realarg != NULL) {
        *realarg++ = '\0';
    }

    if (!strncmp((char *)fs_cmdbuf[dnr], "M-R", 3)) {
        er = fsdevice_flush_mr(vdrive);
    } else if (!strcmp(cmd, "cd")) {
        er = fsdevice_flush_cd(vdrive, arg);
    } else if (!strcmp((char *)fs_cmdbuf[dnr], "CD_")) {
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
        er = fsdevice_flush_scratch(vdrive, realarg);
    } else if (*cmd == 'r' && arg != NULL) {
        er = fsdevice_flush_rename(vdrive, realarg);
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

