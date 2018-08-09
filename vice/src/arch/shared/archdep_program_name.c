/** \file   archdep_program_name.c
 * \brief   Retrieve name of currently running binary
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 */

/*
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

#include "lib.h"
#include "log.h"

#ifdef AMIGA_SUPPORT
/* some includes */
#endif

/* for readlink(2) */
#ifdef UNIX_COMPILE
# include <unistd.h>
#endif

#ifdef MACOS_SUPPORT
# include <libproc.h>
#endif

/* for GetModuleFileName() */
#ifdef WIN32_COMPILE
# include "windows.h"
#endif

#include "archdep_program_name.h"


/** \brief  Program name
 *
 * Heap allocated on the first call, must be free when exiting the program
 * with lib_free().
 */
static char *program_name = NULL;

#if 0
static char *argv0 = "x64sc.exe";
#endif

/** \brief  Buffer used to get the (absolute) path to the binary
 */
static char buffer[4096];


/** \brief  Get name of the currently running binary
 *
 * Allocates the name on the first call, this must be free with lib_free()
 * when exiting the program.
 *
 * \return  program name
 */
char *archdep_program_name(void)
{
    /* if we already have found the program name, just return it */
    if (program_name != NULL) {
        return program_name;
    }

#ifdef AMIGA_SUPPORT
    char *p;

    GetProgramName(buffer, 4096);
    p = FilePart(buffer);
    if (p != NULL) {
        program_name = lib_stralloc(p);
    } else {
        log_error(LOG_ERR, "failed to retrieve program name.");
        exit(1);
    }
#endif

#ifdef UNIX_COMPILE
    /* XXX: Only works on Linux, support for *BSD, Solaris and MacOS to be
     *      added later:
     *
     *      MacOS:      _NSGetExecutablePath()
     *      Solaris:    getexecname()
     *      FreeBSD:    sysctl CTL_KERN_PROC KERN_PROC_PATHNAME - 1 (???)
     *      NetBSD:     readlink /proc/curproc/exe
     *      DFlyBSD:    readlink /proc/curproc/file
     *      OpenBSD:    ???
     */

    char *p;

# ifdef MACOSX_SUPPORT
    /* get path via libproc */
    pid_t pid;

    pid = getpid();
    if (proc_pidpath(pid, buffer, 4096) <= 0) {
        log_error(LOG_ERR, "proc_pidpath() failed for PID %d.", (int)pid);
        exit(1);
    }

# else
    /* Linux-specific */
    if (readlink("/proc/self/exe", buffer, 4096) < 0) {
        log_error(LOG_ERR, "failed to retrieve program name.");
        exit(1);
    }
# endif
    p = strrchr(buffer, '/');
    if (p == NULL) {
        program_name = lib_stralloc(buffer);
    } else {
        program_name = lib_stralloc(p + 1);
    }
#endif

#ifdef WIN32_COMPILE
    char *s;
    char *e;
    int len;

    if (GetModuleFileName(NULL, buffer, 4096) == 4096) {
        log_error(LOG_ERR, "failed to retrieve program name.");
        exit(1);
    }

    s = strrchr(buffer, '\\');
    if (s == NULL) {
        s = buffer;
    } else {
        s++;
    }
    e = strchr(s, '.');
    if (e == NULL) {
        e = buffer + strlen(buffer);
    }
    len = (int)(e - s + 1);
    program_name = lib_malloc(len);
    memcpy(program_name, s, len - 1);
    program_name[len - 1] = 0;
#endif

    printf("%s: got program name '%s'\n", __func__, program_name);
    return program_name;
}


/** \brief  Free program name
 */
void archdep_program_name_free(void)
{
    if (program_name != NULL) {
        lib_free(program_name);
        program_name = NULL;
    }
}
