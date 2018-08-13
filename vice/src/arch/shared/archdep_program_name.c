/** \file   archdep_program_name.c
 * \brief   Retrieve name of currently running binary
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 *
 * Get name of running executable, stripping path and extension (if present).
 *
 * OS support:
 *  - Linux
 *  - Windows
 *  - MacOS
 *  - BeOS/Haiku (untested)
 *  - AmigaOS (untested)
 *  - OS/2 (untested)
 *  - MS-DOS (untested)
 *
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

#ifdef MACOSX_SUPPORT
# include <libproc.h>
#endif

/* for GetModuleFileName() */
#ifdef WIN32_COMPILE
# include "windows.h"
#endif

#include "archdep_join_paths.h"

#include "archdep_program_name.h"


/** \brief  Program name
 *
 * Heap allocated on the first call, must be free when exiting the program
 * with lib_free().
 */
static char *program_name = NULL;


/** \brief  Reference to argv[0], set during init
 *
 * Not all systems have a way to get the binary path, or require C++ code,
 * so this will have to do, for now.
 */
static char *argv0_ref = NULL;


/** \brief  Buffer used to get the (absolute) path to the binary
 */
static char buffer[4096];


/** \brief  Set argv[0] reference
 *
 * No need to copy argv[0], the argv array is guaranteed to exist during a
 * program's lifetime in the C standard.
 *
 * \param[in]   argv0   value of argv[0]
 */
void archdep_program_name_set_argv0(char *argv0)
{
    argv0_ref = argv0;
}


#if defined(WIN32_COMPILE) || defined(OS2_COMPILE) || \
    defined(MSDOS) || defined(_MSDOS) || defined(__MSDOS__) || defined(__DOS__)

/** \brief  Helper function for Windows and OS/2
 *
 * \param[in]   buf string to parse binary name from
 *
 * \return  heap-allocated binary name, free with lib_free()
 */
static char *prg_name_win32_os2(char *buf)
{
    char *s;
    char *e;
    size_t len;
    char *tmp;

    s = strrchr(buf, '\\');
    if (s == NULL) {
        s = buf;
    } else {
        s++;
    }
    e = strchr(s, '.');
    if (e == NULL) {
        e = buf + strlen(buf);
    }
    len = (int)(e - s + 1);
    tmp = lib_malloc(len);
    memcpy(tmp, s, len - 1);
    tmp[len - 1] = 0;

    return tmp;
}
#endif


#ifdef UNIX_COMPILE
/** \brief  Helper function for Unix-ish systems
 *
 * \param[in]   buf string to parse binary name from
 *
 * \return  heap-allocated binary name, free with lib_free()
 */
static char *prg_name_unix(char *buf)
{
    char *p;
    char *tmp;

    p = strrchr(buffer, '/');
    if (p == NULL) {
        tmp = lib_stralloc(buffer);
    } else {
        tmp = lib_stralloc(p + 1);
    }
    return tmp;
}
#endif

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
    program_name = prg_name_unix(buffer);
#endif

#ifdef WIN32_COMPILE
    if (GetModuleFileName(NULL, buffer, 4096) == 4096) {
        log_error(LOG_ERR, "failed to retrieve program name.");
        exit(1);
    }
    program_name = prg_name_win32_os2(buffer);
#endif

#ifdef OS2_COMPILE
    program_name = prg_name_win32_os2(argv0_ref);
#endif

#if defined(MSDOS) || defined(_MSDOS) || defined(__MSDOS__) || defined(__DOS__)
    program_name = prg_name_win32_os2(argv0_ref);
#endif

#ifdef BEOS_COMPILE
    /* this requires the argv0 hack */
    program_name = prg_name_unix(argv0);
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
