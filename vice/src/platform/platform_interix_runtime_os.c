/*
 * platform_interix_runtime_os.c - Interix runtime version discovery.
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

/* Tested and confirmed working on:
   - Interix 3.5 (Windows 2000 Pro)
*/

#include "vice.h"

#ifdef __INTERIX

#include <stdio.h>
#include <interix/interix.h>
#include <interix/registry.h>

#define NT_SERVER_KEY L"\\Registry\\Machine\\System\\CurrentControlSet\\Control\\ProductOptions"
#define NT_SERVER_VALUE L"ProductType"

#define NT_PRODUCT_SUITE_PATH "\\Registry\\Machine\\System\\CurrentControlSet\\Control\\ProductOptions\\ProductSuite"

#define NT_SBS_PATH "\\Registry\\Machine\\System\\CurrentControlSet\\Services\\LicenseInfoSuites\\SmallBusiness\\ConcurrentLimit"

typedef struct winver_s {
    char *name;
    char *windows_name;

/* server:
 * 0 = not a server
 * 1 = server
 * 2 = small business server
 * 3 = enterprise server
 */
    int server;

    int flags;
} winver_t;

static winver_t windows_versions[] = {
    { "Windows NT 4 Workstation", "Microsoft Windows NT 4", 0, 0 },
    { "Windows NT 4 Server", "Microsoft Windows NT 4", 1, 0 },
    { "Windows NT 4 Small Business Server", "Microsoft Windows NT 4", 2, 0 },
    { "Windows NT 4 Enterprise Server", "Microsoft Windows NT 4", 3, 0 },
    { "Windows 2000 Pro", "Microsoft Windows 2000", 0, 0 },
    { "Windows 2000 Server", "Microsoft Windows 2000", 1, 0 },
    { NULL, NULL, 0, 0 }
};

static char *get_windows_version(void)
{
    char windows_name[100];
    char server_name[20];
    char nt_version[10];
    char temp[100];
    int rcode, i;
    int windows_server = 0;
    int windows_flags = 0;
    int type;
    size_t size = 100;

    rcode = getreg_strvalue((PCWSTR)NT_VERSION_KEY, (PCWSTR)NT_PRODUCT_NAME_VALUE, windows_name, 100);
    if (rcode) {
        rcode = getreg_strvalue((PCWSTR)NT_VERSION_KEY, (PCWSTR)NT_VERSION_VALUE, nt_version, 10);
        if (!strcmp("4.0", nt_version)) {
            sprintf(windows_name, "Microsoft Windows NT 4");
        }
    }

    rcode = getreg_strvalue((PCWSTR)NT_SERVER_KEY, (PCWSTR)NT_SERVER_VALUE, server_name, 20);
    if (!rcode) {
        if (!strcmp("ServerNT", server_name) || !strcmp("LanmanNT", server_name)) {
            rcode = getreg(NT_PRODUCT_SUITE_PATH, &type, &temp, &size);
            if (rcode) {
                windows_server = 1;
            } else {
                rcode = getreg(NT_SBS_PATH, &type, &temp, &size);
                if (rcode) {
                    windows_server = 3;
                } else {
                    windows_server = 2;
                }
            }
        }
    }


    /* Check the table for a matching entry */
    for (i = 0; windows_versions[i].name; i++) {
        if (!strcmp(windows_versions[i].windows_name, windows_name)) {
            if (windows_versions[i].server == windows_server) {
                if (windows_versions[i].flags == windows_flags) {
                    return windows_versions[i].name;
                }
            }
        }
    }
    return "Unknown Windows version";
}

static char interix_platform_version[100] = "";

char *platform_get_interix_runtime_os(void)
{
    char interix_version[10];
    char service_pack[100];
    int rcode;

    rcode = getreg_strvalue((PCWSTR)INTERIX_KEY, (PCWSTR)REGVAL_CURRENTRELEASE, interix_version, 10);
    if (rcode) {
        sprintf(interix_platform_version, "Unknown interix version");
    } else {
        sprintf(interix_platform_version, "Interix %s", interix_version);
    }

    rcode = getreg_strvalue((PCWSTR)NT_SERVICEPACK_KEY, (PCWSTR)NT_SERVICEPACK_VALUE, service_pack, 100);
    if (!rcode) {
        sprintf(interix_platform_version, "%s (%s %s)", interix_platform_version, get_windows_version(), service_pack);
    } else {
        sprintf(interix_platform_version, "%s (%s)", interix_platform_version, get_windows_version());
    }

    return interix_platform_version;
}
#endif
