/*
 * platform_windows_runtime_os.h - Windows runtime version discovery.
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

#ifndef VICE_PLATFORM_WINDOWS_RUNTIME_OS_H
#define VICE_PLATFORM_WINDOWS_RUNTIME_OS_H

typedef struct winver_s {
    char *name;
    int platformid;
} winver_t;

static winver_t windows_versions[] = {
    { NULL, 0 },	 /* place holder for what has been detected */
    { "Windows 9x", VER_PLATFORM_WIN32_WINDOWS },
    { "Windows NT+", VER_PLATFORM_WIN32_NT },
    { NULL, 0 }
};

/* 
        DWORD dwOSVersionInfoSize;
        DWORD dwMajorVersion;
        DWORD dwMinorVersion;
        DWORD dwBuildNumber;
        DWORD dwPlatformId;
        CHAR szCSDVersion[128];
*/
static OSVERSIONINFO os_version_info;

static inline char *archdep_get_runtime_windows_os(void)
{
    int found = 0;
    int i;

    ZeroMemory(&os_version_info, sizeof(os_version_info));
    os_version_info.dwOSVersionInfoSize = sizeof(os_version_info);

    GetVersionEx(&os_version_info);

    windows_versions[0].platformid = os_version_info.dwPlatformId;

    for (i = 1; found == 0 && windows_versions[i].name != NULL; i++) {
        if (windows_versions[0].platformid == windows_versions[i].platformid) {
            found = 1;
        }
    }

    if (found) {
        return windows_versions[i - 1].name;
    }
    return "Unknown Windows";
}
#endif
