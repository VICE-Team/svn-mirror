/** \file   archdep_serial_control_list.c
 * \brief   Enumerate the host's serial ports
 * \author  Felix Erckenbrecht <code@eligs.de>
 *
 * Returns a NULL-terminated array of newly-allocated device-name strings for
 * the serial ports present on the system (the SERIALCOMM registry key on
 * Windows, a glob over /dev on *nix). The caller frees every string and the
 * array itself with lib_free().
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
#include "archdep_defs.h"

#include <string.h>

#include "lib.h"

#include "archdep_serial_control_list.h"

#if defined(WINDOWS_COMPILE) || defined(UNIX_COMPILE)
/* grow a NULL-terminated string array, appending a copy of s */
static char **list_append(char **list, int *n, int *cap, const char *s)
{
    if (*n + 2 > *cap) {
        *cap *= 2;
        list = lib_realloc(list, (size_t)*cap * sizeof(char *));
    }
    list[(*n)++] = lib_strdup(s);
    list[*n] = NULL;
    return list;
}
#endif

#if defined(WINDOWS_COMPILE)

#include <windows.h>

/** \brief  Enumerate the host's serial ports
 *
 * \return  NULL-terminated array of COM-port names
 */
char **archdep_serial_control_list(void)
{
    int    cap = 8;
    int    n = 0;
    char **list = lib_malloc((size_t)cap * sizeof(char *));
    HKEY   key;

    list[0] = NULL;
    /* the SERIALCOMM registry key lists the COM ports actually present */
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "HARDWARE\\DEVICEMAP\\SERIALCOMM",
                     0, KEY_READ, &key) == ERROR_SUCCESS) {
        char  name[256];
        BYTE  value[256];
        DWORD index = 0;

        for (;;) {
            DWORD name_len = sizeof(name);
            DWORD value_len = sizeof(value) - 1;
            DWORD type;

            if (RegEnumValue(key, index++, name, &name_len, NULL, &type,
                             value, &value_len) != ERROR_SUCCESS) {
                break;
            }
            if (type == REG_SZ) {
                value[value_len] = 0;
                list = list_append(list, &n, &cap, (const char *)value);
            }
        }
        RegCloseKey(key);
    }
    return list;
}

#elif defined(UNIX_COMPILE)

#include <glob.h>

/** \brief  Enumerate the host's serial ports
 *
 * \return  NULL-terminated array of tty device paths
 */
char **archdep_serial_control_list(void)
{
    static const char * const patterns[] = {
#ifdef MACOS_COMPILE
        "/dev/cu.*",
#else
        "/dev/ttyUSB*", "/dev/ttyACM*", "/dev/ttyS*",
#endif
        NULL
    };
    int    cap = 8;
    int    n = 0;
    char **list = lib_malloc((size_t)cap * sizeof(char *));
    glob_t g;
    int    p;
    int    have = 0;

    list[0] = NULL;
    memset(&g, 0, sizeof(g));
    for (p = 0; patterns[p] != NULL; p++) {
        if (glob(patterns[p], have ? GLOB_APPEND : 0, NULL, &g) == 0) {
            have = 1;
        }
    }
    if (have) {
        size_t i;

        for (i = 0; i < g.gl_pathc; i++) {
            list = list_append(list, &n, &cap, g.gl_pathv[i]);
        }
        globfree(&g);
    }
    return list;
}

#else

/** \brief  Enumerate the host's serial ports
 *
 * \return  empty NULL-terminated array; no serial support on this platform
 */
char **archdep_serial_control_list(void)
{
    char **list = lib_malloc(sizeof(char *));

    list[0] = NULL;
    return list;
}

#endif
