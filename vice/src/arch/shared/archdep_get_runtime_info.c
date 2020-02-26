/** \file   archdep_get_runtime_info.c
 * \brief   Get runtime information
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
#include <stdbool.h>

#include "archdep_defs.h"
#include "log.h"

#ifdef ARCHDEP_OS_UNIX
# include <sys/utsname.h>
#elif defined(ARCHDEP_OS_WINDOWS)
# include <windows.h>
#endif


#include "archdep_get_runtime_info.h"


#ifdef ARCHDEP_OS_WINDOWS

/* This was copied from MSDN or so
 *
 * Seems to not work properly: returns 32-bit on msys2 on Windows 10 64-bit :)
 */

typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);


static BOOL os_is_win64(void)
{
    BOOL amd64 = FALSE;

    LPFN_ISWOW64PROCESS fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(
            GetModuleHandle("kernel32.dll"), "IsWow64Process");

    if (fnIsWow64Process != NULL) {
        if (!fnIsWow64Process(GetCurrentProcess(), &amd64))  {
            log_error(LOG_ERR, "failed to determine arch");
            return FALSE;
        }
    }
    return amd64;
}
#endif


/** \brief  Get runtime info
 *
 * Try to retrieve OS name, version, release and arch.
 *
 * \param[out]  info    OS info struct
 *
 * \return  bool
 */
bool archdep_get_runtime_info(archdep_runtime_info_t *info)
{
#ifdef ARCHDEP_OS_UNIX
    struct utsname buf;
#endif
    /* set defaults */
    memset(info->os_name, 0, ARCHDEP_RUNTIME_STRMAX);
    memset(info->os_version, 0, ARCHDEP_RUNTIME_STRMAX);
    memset(info->os_release, 0, ARCHDEP_RUNTIME_STRMAX);
    memset(info->machine, 0, ARCHDEP_RUNTIME_STRMAX);

#ifdef ARCHDEP_OS_UNIX
    if (uname(&buf) == 0) {
        /* OK */
        printf("sysname = '%s'\n", buf.sysname);
        printf("release = '%s'\n", buf.release);
        printf("version = '%s'\n", buf.version);
        printf("machine = '%s'\n", buf.machine);

        strncpy(info->os_name, buf.sysname, ARCHDEP_RUNTIME_STRMAX - 1U);
        strncpy(info->os_version, buf.version, ARCHDEP_RUNTIME_STRMAX - 1U);
        strncpy(info->os_release, buf.release, ARCHDEP_RUNTIME_STRMAX - 1U);
        strncpy(info->machine, buf.machine, ARCHDEP_RUNTIME_STRMAX - 1U);
        return true;
    }
#elif defined(ARCHDEP_OS_WINDOWS)
    strcpy(info->os_name, "Windows");
    strcpy(info->os_version, "[determining version blows]");
    strcpy(info->os_release, "[determining release is absolutely retarded]");
    if (os_is_win64()) {
        strcpy(info->machine, "x86_64 (64-bit)");
    } else {
        strcpy(info->machine, "x86 (32-bit)");
    }
    return true;
#endif
    return false;
}
