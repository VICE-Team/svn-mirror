/*
 * archdep_os2.h - Architecture dependant defines.
 *
 * Written by
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
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

#ifndef VICE_ARCHDEP_OS2_H
#define VICE_ARCHDEP_OS2_H

#define VICE_ARCHAPI_PRIVATE_API
#include "archapi.h"
#undef VICE_ARCHAPI_PRIVATE_API

/* Filesystem dependant operators.  */
#define FSDEVICE_DEFAULT_DIR "."
#define FSDEV_DIR_SEP_STR    "\\"
#define FSDEV_DIR_SEP_CHR    '\\'
#define FSDEV_EXT_SEP_STR    "."
#define FSDEV_EXT_SEP_CHR    '.'

/* Path separator.  */
#define ARCHDEP_FINDPATH_SEPARATOR_CHAR   ';'
#define ARCHDEP_FINDPATH_SEPARATOR_STRING ";"

/* Modes for fopen().  */
#define MODE_READ              "rb"
#define MODE_READ_TEXT         "r"
#define MODE_READ_WRITE        "rb+"
#define MODE_WRITE             "wb"
#define MODE_WRITE_TEXT        "w"
#define MODE_APPEND            "ab"
#define MODE_APPEND_READ_WRITE "ab+"

/* Printer default devices.  */
#define ARCHDEP_PRINTER_DEFAULT_DEV1 "viceprnt.out"
#define ARCHDEP_PRINTER_DEFAULT_DEV2 "LPT1:"
#define ARCHDEP_PRINTER_DEFAULT_DEV3 "LPT2:"

/* Default RS232 devices.  */
#define ARCHDEP_RS232_DEV1 "10.0.0.1:25232"
#define ARCHDEP_RS232_DEV2 "10.0.0.1:25232"
#define ARCHDEP_RS232_DEV3 "10.0.0.1:25232"
#define ARCHDEP_RS232_DEV4 "10.0.0.1:25232"

/* Default location of raw disk images.  */
#define ARCHDEP_RAWDRIVE_DEFAULT "A:"

/* Access types */
#define ARCHDEP_R_OK 4
#define ARCHDEP_W_OK 2
#define ARCHDEP_X_OK 1
#define ARCHDEP_F_OK 0

/* Standard line delimiter.  */
#define ARCHDEP_LINE_DELIMITER "\r\n"

/* Ethernet default device */
#define ARCHDEP_ETHERNET_DEFAULT_DEVICE ""

/* Default sound fragment size */
#define ARCHDEP_SOUND_FRAGMENT_SIZE SOUND_FRAGMENT_MEDIUM

/*
    FIXME: confirm wether SIGPIPE must be handled or not. if the emulator quits
           or crashes when the connection is closed, you might have to install
           a signal handler which calls monitor_abort().

           see archdep_unix.c and bug #3201796
*/
#if 0
#define archdep_signals_init(x)
#define archdep_signals_pipe_set()
#define archdep_signals_pipe_unset()
#endif

#define MAKE_SO_NAME_VERSION_PROTO(name, version)  #name "-" #version ".dll"

/* add second level macro to allow expansion and stringification */
#define ARCHDEP_MAKE_SO_NAME_VERSION(n, v) MAKE_SO_NAME_VERSION_PROTO(n, v)

#define ARCHDEP_OPENCBM_SO_NAME  "opencbm.dll"
#define ARCHDEP_LAME_SO_NAME     "lame.dll"

/* ffmpeg headers for windows don't seem to have some of the av_ prefixes */
#define ARCHDEP_AV_PREFIX_NEEDED

/* Needs extra call to log_archdep() even when logfile is already opened */
#define ARCHDEP_EXTRA_LOG_CALL

/* When using the ascii printer driver we need a return before the newline */
#define ARCHDEP_PRINTER_RETURN_BEFORE_NEWLINE

/* what to use to return an error when a socket error happens */
#define ARCHDEP_SOCKET_ERROR errno

#endif

/* FIXME: the following where dangling around in old archdep code, remove them
          when os/2 port was checked working */

/* FIXME: OS/2 only? / referenced in archdep_spawn */
#if 0
static int archdep_search_path(const char *name, char *pBuf, int lBuf)
{
    const int flags = SEARCH_CUR_DIRECTORY|SEARCH_IGNORENETERRS;
    char *path = "";        /* PATH environment variable */
    char *pgmName = util_concat(name, ".exe", NULL);

    // Search the program in the path
    DosScanEnv("PATH", &path);


    if (DosSearchPath(flags, path, pgmName, pBuf, lBuf)) {
        return -1;
    }
    lib_free(pgmName);

    return 0;
}
#endif

/* FIXME: OS/2 only? / referenced in archdep_spawn */
#if 0
static char *archdep_cmdline(const char *name, char **argv, const char *sout, const char *serr)
{
    char *res;
    int length = 0;
    int i = 0;

    while (argv[++i]) {
        length += strlen(argv[i]);
    }

    length += i + 1 + (name ? strlen(name) : 0) + 3 + (sout ? strlen(sout) : 0) + 5 +(serr ? strlen(serr) : 0) + 6; // need space for the spaces
    res = lib_calloc(1, length);

    strcat(strcpy(res,"/c "), name);

    i = 0;
    while (argv[++i]) {
        strcat(strcat(res," "), argv[i]);
    }

    if (sout) {
        strcat(strcat(strcat(res,  " > \""), sout), "\"");
    }

    if (serr) {
        strcat(strcat(strcat(res, " 2> \""), serr), "\"");
    }

    return res;
}
#endif

/* FIXME: OS/2 only? */
#if 0
static void archdep_create_mutex_sem(HMTX *hmtx, const char *pszName, int fState)
{
    APIRET rc;

    char *sem = lib_malloc(13+strlen(pszName) + 5 + 1);

    sprintf(sem, "\\SEM32\\VICE2\\%s_%04x", pszName, vsyncarch_gettime()&0xffff);

    rc = DosCreateMutexSem(sem, hmtx, 0, fState);
}
#endif
