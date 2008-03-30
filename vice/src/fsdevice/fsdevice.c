/*
 * fsdevice.c - File system device.
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

#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "archdep.h"
#include "attach.h"
#include "charset.h"
#include "cmdline.h"
#include "fsdevice-close.h"
#include "fsdevice-flush.h"
#include "fsdevice-open.h"
#include "fsdevice-read.h"
#include "fsdevice-resources.h"
#include "fsdevice-write.h"
#include "fsdevice.h"
#include "fsdevicetypes.h"
#include "log.h"
#include "p00.h"
#include "resources.h"
#include "serial.h"
#include "util.h"
#include "vdrive-command.h"
#include "vdrive.h"


fs_buffer_info_t fs_info[16];

/* this should somehow go into the fs_info struct... */

static char fs_errorl[4][MAXPATHLEN];
static unsigned int fs_eptr[4];
static size_t fs_elen[4];

char fs_dirmask[MAXPATHLEN];

static int fsdevice_compare_wildcards(char *name, char *p00name);
static int fsdevice_reduce_filename_p00(char *filename, int len);
static size_t fsdevice_eliminate_char_p00(char *filename, int pos);

/* ------------------------------------------------------------------------- */

void fsdevice_set_directory(char *filename, unsigned int unit)
{
    switch (unit) {
      case 8:
        resources_set_value("FSDevice8Dir", (resource_value_t)filename);
        break;
      case 9:
        resources_set_value("FSDevice9Dir", (resource_value_t)filename);
        break;
      case 10:
        resources_set_value("FSDevice10Dir", (resource_value_t)filename);
        break;
      case 11:
        resources_set_value("FSDevice11Dir", (resource_value_t)filename);
        break;
      default:
        log_message(LOG_DEFAULT, "Invalid unit number %d.", unit);
    }
    return;
}

char *fsdevice_get_path(unsigned int unit)
{
    switch (unit) {
      case 8:
      case 9:
      case 10:
      case 11:
        return fsdevice_dir[unit - 8];
      default:
        log_error(LOG_DEFAULT,
                  "fsdevice_get_pathi() called with invalid device %d.", unit);
        break;
    }
    return NULL;
}

void fsdevice_error(vdrive_t *vdrive, int code)
{
    unsigned int dnr;
    static int last_code[4];
    const char *message;

    dnr = vdrive->unit - 8;

    /* Only set an error once per command */
    if (code != IPE_OK && last_code[dnr] != IPE_OK
        && last_code[dnr] != IPE_DOS_VERSION)
        return;

    last_code[dnr] = code;

    if (code != IPE_MEMORY_READ) {
        if (code == IPE_DOS_VERSION)
            message = "VICE FS DRIVER V2.0";
        else
            message = vdrive_command_errortext(code);

        sprintf(fs_errorl[dnr], "%02d,%s,00,00\015", code, message);

        fs_elen[dnr] = strlen(fs_errorl[dnr]);

        if (code && code != IPE_DOS_VERSION)
            log_message(LOG_DEFAULT, "Fsdevice: ERR = %02d, %s", code, message);
    } else {
        memcpy(fs_errorl[dnr], vdrive->mem_buf, vdrive->mem_length);
        fs_elen[dnr]  = vdrive->mem_length;

    }

    fs_eptr[dnr] = 0;
}

int fsdevice_error_get_byte(vdrive_t *vdrive, BYTE *data)
{
    unsigned int dnr;
    int rc;

    dnr = vdrive->unit - 8;
    rc = SERIAL_OK;

    if (!fs_elen[dnr])
        fsdevice_error(vdrive, IPE_OK);

    if (fs_eptr[dnr] < fs_elen[dnr]) {
        *data = (BYTE)fs_errorl[dnr][fs_eptr[dnr]++];
        rc = SERIAL_OK;
    } else {
        fsdevice_error(vdrive, IPE_OK);
        *data = 0xc7;
        rc = SERIAL_EOF;
    }

    return rc;
}

void fsdevice_test_pc64_name(vdrive_t *vdrive, char *rname, int secondary)
{
    char p00id[8];
    char p00name[17];
    char pathname[MAXPATHLEN];
    FILE *fd;
    int tmptype;

    tmptype = p00_check_name(rname);
    if (tmptype >= 0) {
        strcpy(pathname, fsdevice_get_path(vdrive->unit));
        strcat(pathname, FSDEV_DIR_SEP_STR);
        strcat(pathname, rname);
        fd = fopen(pathname, MODE_READ);
        if (!fd)
            return;

        fread((char *) p00id, 8, 1, fd);
        if (ferror(fd)) {
            fclose(fd);
            return;
        }
        p00id[7] = '\0';
        if (!strncmp(p00id, "C64File", 7)) {
            fread((char *) p00name, 16, 1, fd);
            if (ferror(fd)) {
                fclose(fd);
                return;
            }
            fs_info[secondary].type = tmptype;
            p00name[16] = '\0';
            strcpy(rname, p00name);
            fclose(fd);
            return;
        }
        fclose(fd);
    }
}

FILE *fsdevice_find_pc64_name(vdrive_t *vdrive, char *name, int length,
                              char *pname)
{
    struct dirent *dirp;
    char *p;
    DIR *dp;
    char p00id[8], p00name[17], p00dummy[2];
    FILE *fd;

    name[length] = '\0';

    dp = opendir(fsdevice_get_path(vdrive->unit));
    if (dp == NULL)
        return NULL;

    do {
        dirp = readdir(dp);
        if (dirp != NULL) {
            strcpy(pname, fsdevice_get_path(vdrive->unit));
            strcat(pname, FSDEV_DIR_SEP_STR);
            strcat(pname, dirp->d_name);
            p = pname;
            if (p00_check_name(p) >= 0) {
                fd = fopen(p, MODE_READ);
                if (!fd)
                    continue;
                fread((char *) p00id, 8, 1, fd);
                if (ferror(fd)) {
                    fclose(fd);
                    continue;
                }
                p00id[7] = '\0';
                if (!strncmp(p00id, "C64File", 7)) {
                    fread((char *) p00name, 16, 1, fd);
                    if (ferror(fd)) {
                        fclose(fd);
                        continue;
                    }
                    p00name[16] = '\0';
                    if (fsdevice_compare_wildcards(name, p00name) > 0) {
                        fread((char *) p00dummy, 2, 1, fd);
                        if (ferror(fd)) {
                            fclose(fd);
                            continue;
                        }
                        closedir(dp);
                        return fd;
                    }
                }
                fclose(fd);
            }
        }
    }
    while (dirp != NULL);
    closedir(dp);
    return NULL;
}

static int fsdevice_compare_wildcards(char *name, char *p00name)
{
    size_t i, len;

    len = strlen(name);
    if (len == 0)
        return 0;

    for (i = 0; i < len; i++) {
        if (name[i] == '*')
            return 1;
        if (name[i] != '?' && name[i] != p00name[i])
            return 0;
    }
    if (strlen(p00name) > len)
        return 0;
    return 1;
}

void fsdevice_compare_file_name(vdrive_t *vdrive, char *fsname2,
                                char *fsname, int secondary)
{
    struct dirent *dirp;
    DIR *dp;
    char rname[MAXPATHLEN];

    dp = opendir(fsdevice_get_path(vdrive->unit));
    do {
        dirp = readdir(dp);
        if (dirp != NULL) {
            if (fsdevice_compare_wildcards(fsname2, dirp->d_name) > 0) {
                strcpy(rname, dirp->d_name);
                fsdevice_test_pc64_name(vdrive, rname, secondary);
                if (strcmp(rname, dirp->d_name) == 0) {
                    strcpy(fsname, fsdevice_get_path(vdrive->unit));
                    strcat(fsname, FSDEV_DIR_SEP_STR);
                    strcat(fsname, dirp->d_name);
                    closedir(dp);
                    return;
                }
            }
        }
    }
    while (dirp != NULL);
    closedir(dp);
    return;
}

int fsdevice_create_file_p00(vdrive_t *vdrive, char *name, int length,
                             char *fsname, int secondary)
{
    char filename[17], realname[16];
    int i;
    size_t len;
    FILE *fd;

    if (length > 16)
        length = 16;
    memset(realname, 0, 16);
    strncpy(realname, name, length);

    len = fsdevice_evaluate_name_p00(name, length, filename);

    strcpy(fsname, fsdevice_get_path(vdrive->unit));
    strcat(fsname, FSDEV_DIR_SEP_STR);
    strncat(fsname, filename, len);
    switch (fs_info[secondary].type) {
      case FT_DEL:
        strcat(fsname, FSDEV_EXT_SEP_STR "D");
        break;
      case FT_SEQ:
        strcat(fsname, FSDEV_EXT_SEP_STR "S");
        break;
      case FT_PRG:
        strcat(fsname, FSDEV_EXT_SEP_STR "P");
        break;
      case FT_USR:
        strcat(fsname, FSDEV_EXT_SEP_STR "U");
        break;
      case FT_REL:
        strcat(fsname, FSDEV_EXT_SEP_STR "R");
        break;
    }
    strcat(fsname, "00");

    for (i = 1; i < 100; i++) {
        fd = fopen(fsname, MODE_READ);
        if (!fd)
            break;
        fclose(fd);
        sprintf(&fsname[strlen(fsname) - 2], "%02i", i);
    }

    if (i >= 100)
        return 1;

    fd = fopen(fsname, MODE_WRITE);
    if (!fd)
        return 1;

    if (fwrite("C64File", 8, 1, fd) < 1) {
        fclose(fd);
        return 1;
    }
    if (fwrite(realname, 16, 1, fd) < 1) {
        fclose(fd);
        return 1;
    }
    if (fwrite("\0\0", 2, 1, fd) < 1) {
        fclose(fd);
        return 1;
    }
    fclose(fd);
    return 0;
}

static int fsdevice_reduce_filename_p00(char *filename, int len)
{
    int i, j;

    for (i = len - 1; i >= 0; i--) {
        if (filename[i] == '_')
            if (fsdevice_eliminate_char_p00(filename, i) <= 8)
                return 8;
        }

    for (i = 0; i < len; i++) {
        if (strchr("AEIOU", filename[i]) != NULL)
            break;
    }

    for (j = len - 1; j >= i; j--) {
        if (strchr("AEIOU", filename[j]) != NULL)
            if (fsdevice_eliminate_char_p00(filename, j) <= 8)
                return 8;
    }

    for (i = len - 1; i >= 0; i--) {
        if (isalpha((int) filename[i]))
            if (fsdevice_eliminate_char_p00(filename, i) <= 8)
                return 8;
    }

    for (i = len - 1; i >= 0; i--)
        if (fsdevice_eliminate_char_p00(filename, i) <= 8)
            return 8;

    return 1;
}

static size_t fsdevice_eliminate_char_p00(char *filename, int pos)
{
    memcpy(&filename[pos], &filename[pos+1], 16 - pos);
    return strlen(filename);
}

int fsdevice_evaluate_name_p00(char *name, int length, char *filename)
{
    int i, j;

    memset(filename, 0, 17);

    for (i = 0, j = 0; i < length; i++) {
        switch (name[i]) {
          case ' ':
          case '-':
            filename[j++] = '_';
            break;
          default:
            if (islower((int) name[i])) {
                filename[j++] = toupper(name[i]);
                break;
            }
            if (isalnum((int) name[i])) {
                filename[j++] = name[i];
                break;
            }
        }
    }
    if (j == 0) {
        strcpy(filename, "_");
        j++;
    }
    return ((j > 8) ? fsdevice_reduce_filename_p00(filename, j) : j);
}

int fsdevice_attach(unsigned int device, const char *name)
{
    vdrive_t *vdrive;

    vdrive = file_system_get_vdrive(device);

    if (serial_attach_device(device, name, fsdevice_read, fsdevice_write,
                             fsdevice_open, fsdevice_close, fsdevice_flush))
        return 1;

    vdrive->image_format = VDRIVE_IMAGE_FORMAT_1541;
    fsdevice_error(vdrive, IPE_DOS_VERSION);
    return 0;
}

