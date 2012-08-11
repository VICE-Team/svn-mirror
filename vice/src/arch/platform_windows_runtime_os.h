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
    int majorver;
    int minorver;
} winver_t;

static winver_t windows_versions[] = {
    { NULL, 0,
      0, 0 },	 /* place holder for what has been detected */
    { "Windows 95", VER_PLATFORM_WIN32_WINDOWS,
      4, 0 },
    { "Windows 98", VER_PLATFORM_WIN32_WINDOWS,
      4, 1 },
    { "Windows ME", VER_PLATFORM_WIN32_WINDOWS,
      4, 90 },
    { "Windows NT 3.10", VER_PLATFORM_WIN32_NT,
      3, 10 },
    { "Windows NT 3.50", VER_PLATFORM_WIN32_NT,
      3, 5 },
    { "Windows NT 3.51", VER_PLATFORM_WIN32_NT,
      3, 51 },
    { "Windows NT 4.0", VER_PLATFORM_WIN32_NT,
      4, 0 },
    { "Windows 2000", VER_PLATFORM_WIN32_NT,
      5, 0 },
    { "Windows XP", VER_PLATFORM_WIN32_NT,
      5, 1 },
    { "Windows XP64 / Windows 2003 Server / Windows Home Server", VER_PLATFORM_WIN32_NT,
      5, 2 },
    { "Windows Vista / Windows 2008 Server", VER_PLATFORM_WIN32_NT,
      6, 0 },
    { "Windows 7 / Windows 2008 R2 Server", VER_PLATFORM_WIN32_NT,
      6, 1 },
    { "Windows 8 / Windows 2012 Server", VER_PLATFORM_WIN32_NT,
      6, 2 },
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
    windows_versions[0].majorver = os_version_info.dwMajorVersion;
    windows_versions[0].minorver = os_version_info.dwMinorVersion;

    for (i = 1; found == 0 && windows_versions[i].name != NULL; i++) {
        if (windows_versions[0].platformid == windows_versions[i].platformid) {
            if (windows_versions[0].majorver == windows_versions[i].majorver) {
                if (windows_versions[0].minorver == windows_versions[i].minorver) {
                    found = 1;
                }
            }
        }
    }

    if (found) {
        return windows_versions[i - 1].name;
    }
    return "Unknown Windows version";
}
#endif
