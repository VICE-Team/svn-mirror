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

#ifdef __riscos
#include "ui.h"
#else
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
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
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
#include "fsdevice-resources.h"
#include "fsdevice.h"
#include "log.h"
#include "p00.h"
#include "resources.h"
#include "serial.h"
#include "utils.h"
#include "vdrive-command.h"
#include "vdrive.h"

enum fsmode {
    Write, Read, Append, Directory
};

struct fs_buffer_info {
    FILE *fd;
    DIR  *dp;
    enum fsmode mode;
    char dir[MAXPATHLEN];
    BYTE name[MAXPATHLEN + 5];
    int buflen;
    BYTE *bufp;
    int eof;
    int reclen;
    int type;
} fs_info[16];

/* this should somehow go into the fs_info struct... */

static char fs_errorl[MAXPATHLEN];
static unsigned int fs_eptr;
static size_t fs_elen;
static BYTE fs_cmdbuf[MAXPATHLEN];
static unsigned int fs_cptr = 0;

static char fs_dirmask[MAXPATHLEN];

static FILE *fs_find_pc64_name(vdrive_t *vdrive, char *name, int length,
                               char *pname);
static void fs_test_pc64_name(vdrive_t *vdrive, char *rname, int secondary);
static int fsdevice_compare_wildcards(char *name, char *p00name);
static void fsdevice_compare_file_name(vdrive_t *vdrive, char *fsname2,
                                       char *fsname, int secondary);
static int fsdevice_create_file_p00(vdrive_t *vdrive, char *name, int length,
                                    char *fsname, int secondary);
static int fsdevice_reduce_filename_p00(char *filename, int len);
static size_t fsdevice_eliminate_char_p00(char *filename, int pos);
static int fsdevice_evaluate_name_p00(char *name, int length, char *filename);

/* FIXME: ugly.  */
extern errortext_t floppy_error_messages;

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

static char *fsdevice_get_path(unsigned int unit)
{
    switch (unit) {
      case 8:
      case 9:
      case 10:
      case 11:
        return fsdevice_dir[unit - 8];
      default:
        log_error(LOG_DEFAULT, "Boom! fsdevice_get_path called with "
                                                "invalid device %d",unit);
        break;
    }
    return NULL;
}

static void fs_error(vdrive_t *vdrive, int code)
{
    static int last_code;
    const char *message;

    /* Only set an error once per command */
    if (code != IPE_OK && last_code != IPE_OK && last_code != IPE_DOS_VERSION)
        return;

    last_code = code;

    if (code != IPE_MEMORY_READ) {
        if (code == IPE_DOS_VERSION) {
            message = "VICE FS DRIVER V2.0";
        } else {
            errortext_t *e;
            e = &floppy_error_messages;
            while (e->nr >= 0 && e->nr != code)
                e++;
            if (e->nr >= 0)
                message = e->text;
            else
                message = "UNKNOWN ERROR NUMBER";
        }

        sprintf(fs_errorl, "%02d,%s,00,00\015", code, message);

        fs_elen = strlen(fs_errorl);

        if (code && code != IPE_DOS_VERSION)
            log_message(LOG_DEFAULT, "Fsdevice: ERR = %02d, %s", code, message);
    } else {
        memcpy(fs_errorl, vdrive->mem_buf, vdrive->mem_length);
        fs_elen  = vdrive->mem_length;

    }
    fs_eptr = 0;
}

static void flush_fs(vdrive_t *vdrive, unsigned int secondary)
{
    char *cmd, *realarg, *arg, *realarg2 = NULL, *arg2 = NULL;
    char cbmcmd[MAXPATHLEN], name1[MAXPATHLEN], name2[MAXPATHLEN];
    int er = IPE_SYNTAX;
    FILE *fd;

    if (secondary != 15 || !fs_cptr)
        return;

    /* remove trailing cr */
    while (fs_cptr && (fs_cmdbuf[fs_cptr - 1] == 13))
        fs_cptr--;
    fs_cmdbuf[fs_cptr] = 0;

    strcpy(cbmcmd, fs_cmdbuf);
    charset_petconvstring(cbmcmd, 1);   /* CBM name to FSname */
    cmd = cbmcmd;
    while (*cmd == ' ')
        cmd++;

    arg = strchr(cbmcmd, ':');
    if (arg) {
        *arg++ = '\0';
    }
    realarg = strchr(fs_cmdbuf, ':');
    if (realarg) {
        *realarg++ = '\0';
    }

    if (!strncmp(fs_cmdbuf, "M-R", 3)) {
        ADDRESS addr = 0;
        addr = fs_cmdbuf[3] | (fs_cmdbuf[4] << 8);
        er = vdrive_command_memory_read(vdrive, addr, fs_cmdbuf[5]);
    } else if (!strcmp(cmd, "cd")) {
        er = IPE_OK;
        if (chdir(arg)) {
            er = IPE_NOT_FOUND;
            if (errno == EPERM)
                er = IPE_PERMISSION;
        }
    } else if (!strcmp(cmd, "md")) {
        er = IPE_OK;
        if (mkdir(arg, 0770)) {
            er = IPE_INVAL;
            if (errno == EEXIST)
                er = IPE_FILE_EXISTS;
            if (errno == EACCES)
                er = IPE_PERMISSION;
            if (errno == ENOENT)
                er = IPE_NOT_FOUND;
        }
    } else if (!strcmp(cmd, "rd")) {
        er = IPE_OK;
        if (rmdir(arg)) {
            er = IPE_NOT_EMPTY;
            if (errno == EPERM)
                er = IPE_PERMISSION;
        }
    } else if (*cmd == 's' && arg != NULL) {
        er = IPE_DELETED;
        fd = fs_find_pc64_name(vdrive, realarg, strlen(realarg), name1);
            if (fd != NULL) {
                fclose(fd);
            } else {
                if (fsdevice_hide_cbm_files_enabled[vdrive->unit - 8]) {
                    fs_error(vdrive, IPE_NOT_FOUND);
                    fs_cptr = 0;
                    return;
                }
                strcpy(name1, fsdevice_get_path(vdrive->unit));
                strcat(name1, FSDEV_DIR_SEP_STR);
                strcat(name1, arg);
            }
        if (util_file_remove(name1)) {
            er = IPE_NOT_FOUND;
            if (errno == EPERM)
                er = IPE_PERMISSION;
        }
    } else if (*cmd == 'r' && arg != NULL) {
        if ((arg2 = strchr(arg, '='))) {
            char name2long[MAXPATHLEN];
            er = IPE_OK;
            *arg2++ = 0;
            realarg2 = strchr(realarg, '=');
            *realarg2++ = 0;
            fd = fs_find_pc64_name(vdrive, realarg2, strlen(realarg2),
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

                        util_file_remove(name1p00);
                        if (rename(name2p00, name1p00) == 0)
                            break;
                    }
                }
            } else {
                /* Rename CBM file.  */
                if (fsdevice_hide_cbm_files_enabled[vdrive->unit - 8]) {
                    fs_error(vdrive, IPE_NOT_FOUND);
                    fs_cptr = 0;
                    return;
                }
                strcpy(name1, fsdevice_get_path(vdrive->unit));
                strcat(name1, FSDEV_DIR_SEP_STR);
                strcat(name1, arg);
                strcpy(name2, fsdevice_get_path(vdrive->unit));
                strcat(name2, FSDEV_DIR_SEP_STR);
                strcat(name2, arg2);

                util_file_remove(name1);
                if (rename(name2, name1)) {
                    er = IPE_NOT_FOUND;
                    if (errno == EPERM)
                        er = IPE_PERMISSION;
                }
            }
        }
    }
    fs_error(vdrive, er);
    fs_cptr = 0;
}

static int write_fs(vdrive_t *vdrive, BYTE data, unsigned int secondary)
{
    if (secondary == 15) {
        if (fs_cptr < MAXPATHLEN - 1) {         /* keep place for nullbyte */
            fs_cmdbuf[fs_cptr++] = data;
            return SERIAL_OK;
        } else {
            fs_error(vdrive, IPE_LONG_LINE);
            return SERIAL_ERROR;
        }
    }
    if (fs_info[secondary].mode != Write && fs_info[secondary].mode != Append)
        return FLOPPY_ERROR;

    if (fs_info[secondary].fd) {
        fputc(data, fs_info[secondary].fd);
        return FLOPPY_COMMAND_OK;
    };

    return FLOPPY_ERROR;
}

static int read_fs(vdrive_t *vdrive, BYTE * data, unsigned int secondary)
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
    struct fs_buffer_info *info = &fs_info[secondary];
    char rname[256];

    if (secondary == 15) {
        if (!fs_elen)
            fs_error(vdrive, IPE_OK);
        if (fs_eptr < fs_elen) {
            *data = (BYTE)fs_errorl[fs_eptr++];
            return SERIAL_OK;
        } else {
            fs_error(vdrive, IPE_OK);
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
                        fs_test_pc64_name(vdrive, rname, secondary);
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

static int open_fs(vdrive_t *vdrive, const char *name, int length,
                   unsigned int secondary)
{
    FILE *fd;
    DIR *dp;
    BYTE *p;
    char fsname[MAXPATHLEN], fsname2[MAXPATHLEN], rname[MAXPATHLEN];
    char *mask, *comma;
    int status = 0, i;
    unsigned int reallength, readmode, rl;

    if (fs_info[secondary].fd)
        return FLOPPY_ERROR;

    memcpy(fsname2, name, length);
    fsname2[length] = 0;

    if (secondary == 15) {
        for (i = 0; i < length; i++)
            status = write_fs(vdrive, name[i], 15);
        return status;
    }

    /* Default filemodes.  */
    readmode = (secondary == 1) ? FAM_WRITE : FAM_READ;

    rl = 0;

    if (vdrive_parse_name(fsname2, length, fsname, &reallength, &readmode,
                          (unsigned int*)&fs_info[secondary].type, &rl) != SERIAL_OK)
        return SERIAL_ERROR;

    if (fs_info[secondary].type == FT_DEL)
        fs_info[secondary].type = (secondary < 2) ? FT_PRG : FT_SEQ;

    /* Override read mode if secondary is 0 or 1.  */
    if (secondary == 0)
        readmode = FAM_READ;
    if (secondary == 1)
        readmode = FAM_WRITE;

    fsname[reallength] = 0;
    strncpy(rname, fsname, reallength);

    charset_petconvstring(fsname, 1);   /* CBM name to FSname */

    switch (readmode) {
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
            fs_error(vdrive, IPE_NOT_WRITE);
            return FLOPPY_ERROR;
        }
        /* Test on wildcards.  */
        if (!(mask = strrchr(fsname, '/')))
            mask = fsname;
        if (strchr(mask, '*') || strchr(mask, '?')) {
            if (*mask == '/') {
                strcpy(fs_dirmask, mask + 1);
                *mask++ = 0;
            } else {
                strcpy(fs_dirmask, mask);
                strcpy(fsname, fsdevice_get_path(vdrive->unit));
            }
        } else {
            *fs_dirmask = 0;
            if (!*fsname)
                strcpy(fsname, fsdevice_get_path(vdrive->unit));
        }
        /* trying to open */
        if (!(dp = opendir((char *) fsname))) {
            for (p = (BYTE *) fsname; *p; p++)
                if (isupper((int) *p))
                    *p = tolower((int) *p);
            if (!(dp = opendir((char *) fsname))) {
                fs_error(vdrive, IPE_NOT_FOUND);
                return FLOPPY_ERROR;
            }
        }
        strcpy(fs_info[secondary].dir, fsname);

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
        strcpy((char *) p, fs_info[secondary].dir);     /* Dir name */
        charset_petconvstring((char *) p, 0);   /* ASCII name to PETSCII */
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
        if (fsname[0] == ',') {
            fsname[1] = '\0';
        } else {
            comma = strchr(fsname, ',');
            if (comma != NULL)
                *comma = '\0';
        }
        strcpy(fsname2, fsname);
        strcpy(fsname, fsdevice_get_path(vdrive->unit));
        strcat(fsname, FSDEV_DIR_SEP_STR);
        strcat(fsname, fsname2);

        /* Test on wildcards.  */
        if (strchr(fsname2, '*') || strchr(fsname2, '?')) {
            if (fs_info[secondary].mode == Write
                                || fs_info[secondary].mode == Append) {
                fs_error(vdrive, IPE_BAD_NAME);
                return FLOPPY_ERROR;
            } else {
                fsdevice_compare_file_name(vdrive, fsname2, fsname, secondary);
            }
        }

        /* Open file for write mode access.  */
        if (fs_info[secondary].mode == Write) {
            fd = fopen(fsname, MODE_READ);
            if (fd != NULL) {
                fclose(fd);
                fs_error(vdrive, IPE_FILE_EXISTS);
                return FLOPPY_ERROR;
            }
            if (fsdevice_convert_p00_enabled[(vdrive->unit) - 8]) {
                fd = fs_find_pc64_name(vdrive, rname, reallength, fsname2);
                if (fd != NULL) {
                    fclose(fd);
                    fs_error(vdrive, IPE_FILE_EXISTS);
                    return FLOPPY_ERROR;
                }
            }
            if (fsdevice_save_p00_enabled[(vdrive->unit) - 8]) {
                if (fsdevice_create_file_p00(vdrive, rname, reallength, fsname,
                                             secondary) > 0) {
                    fs_error(vdrive, IPE_FILE_EXISTS);
                    return FLOPPY_ERROR;
                } else {
                    fd = fopen(fsname, MODE_APPEND_READ_WRITE);
                    fs_info[secondary].fd = fd;
                    fs_error(vdrive, IPE_OK);
                    return FLOPPY_COMMAND_OK;
                }
            } else {
                fd = fopen(fsname, MODE_WRITE);
                fs_info[secondary].fd = fd;
                fs_error(vdrive, IPE_OK);
                return FLOPPY_COMMAND_OK;
            }
        }

        /* Open file for append mode access.  */
        if (fs_info[secondary].mode == Append) {
            fd = fopen(fsname, MODE_READ);
            if (!fd) {
                if (!fsdevice_convert_p00_enabled[(vdrive->unit) - 8]) {
                    fs_error(vdrive, IPE_NOT_FOUND);
                    return FLOPPY_ERROR;
                }
                fd = fs_find_pc64_name(vdrive, rname, reallength, fsname2);
                if (!fd) {
                    fs_error(vdrive, IPE_NOT_FOUND);
                    return FLOPPY_ERROR;
                }
                fclose(fd);
                fd = fopen(fsname2, MODE_APPEND_READ_WRITE);
                if (!fd) {
                    fs_error(vdrive, IPE_NOT_FOUND);
                    return FLOPPY_ERROR;
                }
                fs_info[secondary].fd = fd;
                fs_error(vdrive, IPE_OK);
                return FLOPPY_COMMAND_OK;
            } else {
                fclose(fd);
                fd = fopen(fsname, MODE_APPEND_READ_WRITE);
                if (!fd) {
                    fs_error(vdrive, IPE_NOT_FOUND);
                    return FLOPPY_ERROR;
                }
                fs_info[secondary].fd = fd;
                fs_error(vdrive, IPE_OK);
                return FLOPPY_COMMAND_OK;
            }
        }

        /* Open file for read mode access.  */
        fd = fopen(fsname, MODE_READ);
        if (!fd) {
            if (!fsdevice_convert_p00_enabled[(vdrive->unit) - 8]) {
                fs_error(vdrive, IPE_NOT_FOUND);
                return FLOPPY_ERROR;
            }
            fd = fs_find_pc64_name(vdrive, rname, reallength, fsname2);
            if (!fd) {
                fs_error(vdrive, IPE_NOT_FOUND);
                return FLOPPY_ERROR;
            }
            fs_info[secondary].fd = fd;
            fs_error(vdrive, IPE_OK);
            return FLOPPY_COMMAND_OK;
        } else {
            if (fsdevice_hide_cbm_files_enabled[vdrive->unit - 8]) {
                fclose(fd);
                fs_error(vdrive, IPE_NOT_FOUND);
                return FLOPPY_ERROR;
            }
            fs_info[secondary].fd = fd;
            fs_error(vdrive, IPE_OK);
            return FLOPPY_COMMAND_OK;
        }
    }
#ifdef __riscos
    ui_set_drive_leds(vdrive->unit - 8, 1);
#endif
    fs_error(vdrive, IPE_OK);
    return FLOPPY_COMMAND_OK;
}

static int close_fs(vdrive_t *vdrive, unsigned int secondary)
{
#ifdef __riscos
    ui_set_drive_leds(vdrive->unit - 8, 0);
#endif

    if (secondary == 15) {
        fs_error(vdrive, IPE_OK);
        return FLOPPY_COMMAND_OK;
    }
    switch (fs_info[secondary].mode) {
      case Write:
      case Read:
      case Append:
          if (!fs_info[secondary].fd)
              return FLOPPY_ERROR;

          fclose(fs_info[secondary].fd);
          fs_info[secondary].fd = NULL;
          break;

      case Directory:
          if (!fs_info[secondary].dp)
              return FLOPPY_ERROR;

          closedir(fs_info[secondary].dp);
          fs_info[secondary].dp = NULL;
          break;
    }

    return FLOPPY_COMMAND_OK;
}

void fs_test_pc64_name(vdrive_t *vdrive, char *rname, int secondary)
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

FILE *fs_find_pc64_name(vdrive_t *vdrive, char *name, int length, char *pname)
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

static void fsdevice_compare_file_name(vdrive_t *vdrive, char *fsname2,
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
                fs_test_pc64_name(vdrive, rname, secondary);
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

static int fsdevice_create_file_p00(vdrive_t *vdrive, char *name, int length,
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

static int fsdevice_evaluate_name_p00(char *name, int length, char *filename)
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

    vdrive = (vdrive_t *)file_system_get_vdrive(device);

    if (serial_attach_device(device, name,
                             read_fs, write_fs, open_fs, close_fs, flush_fs))
        return 1;

    vdrive->image_format = VDRIVE_IMAGE_FORMAT_1541;
    fs_error(vdrive, IPE_DOS_VERSION);
    return 0;
}

