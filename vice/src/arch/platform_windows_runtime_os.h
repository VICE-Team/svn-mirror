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
    int realos;
} winver_t;

static winver_t windows_versions[] = {
    { NULL, 0,
      0, 0, 0 },	 /* place holder for what has been detected */
    { "Windows 95", VER_PLATFORM_WIN32_WINDOWS,
      4, 0, 3 },
    { "Windows 98", VER_PLATFORM_WIN32_WINDOWS,
      4, 1, 4 },
    { "Windows ME", VER_PLATFORM_WIN32_WINDOWS,
      4, 90, 5 },
    { "Windows NT 3.10", VER_PLATFORM_WIN32_NT,
      3, 10, 0 },
    { "Windows NT 3.50", VER_PLATFORM_WIN32_NT,
      3, 5, 0 },
    { "Windows NT 3.51", VER_PLATFORM_WIN32_NT,
      3, 51, 0 },
    { "Windows NT 4.0", VER_PLATFORM_WIN32_NT,
      4, 0, 1 },
    { "Windows 2000", VER_PLATFORM_WIN32_NT,
      5, 0, 6 },
    { "Windows XP", VER_PLATFORM_WIN32_NT,
      5, 1, 8 },
    { "Windows XP64 / Windows 2003 Server / Windows Home Server", VER_PLATFORM_WIN32_NT,
      5, 2, 8 },
    { "Windows Vista / Windows 2008 Server", VER_PLATFORM_WIN32_NT,
      6, 0, 10 },
    { "Windows 7 / Windows 2008 R2 Server", VER_PLATFORM_WIN32_NT,
      6, 1, 10 },
    { "Windows 8 / Windows 2012 Server", VER_PLATFORM_WIN32_NT,
      6, 2, 10 },
    { NULL, 0, 0, 0, 0 }
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

/* 
0: NT3, 1: NT4, 2: 95, 3: 95OSR2, 4: 98, 5: ME, 6: 2000, 7: XP, 8: XPSP1, 9: 2003, 10: VISTA+
*/

static int GetRealOS(void)
{
    HMODULE k = GetModuleHandle(TEXT("kernel32.dll"));

    if (GetProcAddress(k, "GetLocaleInfoEx") != NULL) {
        return 10;
    }
    if (GetProcAddress(k, "GetLargePageMinimum") != NULL) {
        return 9;
    }
    if (GetProcAddress(k, "GetDLLDirectory") != NULL) {
        return 8;
    }
    if (GetProcAddress(k, "GetNativeSystemInfo") != NULL) {
        return 7;
    }
    if (GetProcAddress(k, "ReplaceFile") != NULL) {
        return 6;
    }
    if (GetProcAddress(k, "OpenThread") != NULL) {
        return 5;
    }
    if (GetProcAddress(k, "GetThreadPriorityBoost") != NULL) {
        return 1;
    }
    if (GetProcAddress(k, "ConnectNamedPipe") != NULL) {
        return 0;
    }
    if (GetProcAddress(k, "IsDebuggerPresent") != NULL) {
        return 4;
    }
    if (GetProcAddress(k, "GetDiskFreeSpaceEx") != NULL) {
        return 3;
    }
    return 2;
}

static int IsWow64(void)
{
    int bIsWow64 = 0;
    typedef BOOL (APIENTRY *LPFN_ISWOW64PROCESS)(HANDLE, PBOOL);
    LPFN_ISWOW64PROCESS fnIsWow64Process;
    HMODULE module = GetModuleHandle(TEXT("kernel32"));
    const char funcName[] = "IsWow64Process";

    fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(module, funcName);

    if (NULL != fnIsWow64Process) {
        fnIsWow64Process(GetCurrentProcess(), &bIsWow64);
    }

    return bIsWow64;
}

static int IsReactOS(void)
{
    char *s = os_version_info.szCSDVersion;
    int i = 0;

    if (s[0] == 0) {
        return 0;
    }

    while (s[i++] != 0);

    if (!strncmp(s, "ReactOS", 7)) {
        return 1;
    }
    return 0;
}

static int IsWine(void)
{
    if (GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "wine_get_unix_file_name") != NULL) {
        return 1;
    }
    return 0;
}

static int IsOdin32(void)
{
    if (GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "RegisterLXExe") != NULL) {
        return 1;
    }
    return 0;
}

static int IsHxDos(void)
{
    if (GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "GetDKrnl32Version") != NULL) {
        return 1;
    }
    return 0;
}

static char *get_win95_version(void)
{
    if (!strncmp(os_version_info.szCSDVersion, " A ", 3)) {
        return "A";
    }
    if (!strncmp(os_version_info.szCSDVersion, " B ", 3)) {
        return "B";
    }
    if (!strncmp(os_version_info.szCSDVersion, " C ", 3)) {
        return "C";
    }
    return "";
}

static char *get_win98_version(void)
{
    if (!strncmp(os_version_info.szCSDVersion, " A ", 3)) {
        return "SE";
    }
    return "";
}

static char windows_version[256];

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
    windows_versions[0].realos = GetRealOS();

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
        sprintf(windows_version, "%s", windows_versions[i - 1].name);
        if (windows_versions[0].platformid == VER_PLATFORM_WIN32_WINDOWS) {
            if (windows_versions[0].minorver == 0) {
                sprintf(windows_version, "%s%s", windows_version, get_win95_version());
            }
            if (windows_versions[0].minorver == 10) {
                sprintf(windows_version, "%s%s", windows_version, get_win98_version());
            }
        }
        if (windows_versions[0].realos > windows_versions[i - 1].realos) {
            sprintf(windows_version, "%s (compatibility mode)", windows_version);
        }
        if (IsWow64()) {
            sprintf(windows_version, "%s (WOW64)", windows_version);
        }
        if (IsReactOS()) {
            sprintf(windows_version, "%s (ReactOS)", windows_version);
        }
        if (IsWine()) {
            sprintf(windows_version, "%s (Wine)", windows_version);
        }
        if (IsOdin32()) {
            sprintf(windows_version, "%s (Odin32)", windows_version);
        }
        if (IsHxDos()) {
            sprintf(windows_version, "%s (HXDOS)", windows_version);
        }
    } else {
        sprintf(windows_version, "%s", "Unknown Windows version");
    }
    return windows_version;
}
#endif
