/*
 * platform_netbsd_runtime_os.c - NetBSD runtime version discovery.
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
   cpu | netbsd version
   --------------------
 */

#include "vice.h"

#ifdef __NetBSD__

#include <stdio.h>
#include <sys/utsname.h>

#include "archdep.h"
#include "lib.h"
#include "platform.h"
#include "util.h"
#include "log.h"

static char netbsd_version[100];
static char netbsd_cpu[100];
static int got_netbsd_version = 0;
static int got_netbsd_cpu = 0;

char *platform_get_netbsd_runtime_cpu(void)
{
    FILE *cpuinfo = NULL;
    char *buffer = NULL;
    char *loc1 = NULL;
    char *loc2 = NULL;
    char *loc3 = NULL;
    char *tempfile = NULL;
    char tempsystem[512];
    size_t size1 = 0;
    size_t size2 = 0;
    struct utsname name;

    if (!got_netbsd_cpu) {
        sprintf(netbsd_cpu, "Unknown CPU");
        cpuinfo = fopen("/proc/cpuinfo", "rb");
        if (cpuinfo) {
            fclose(cpuinfo);
            cpuinfo = NULL;
            tempfile = archdep_tmpnam();
            sprintf(tempsystem, "cat /proc/cpuinfo >%s", tempfile);
            if (system(tempsystem) < 0) {
                log_warning(LOG_ERR, "`%s' failed.", tempsystem);
            }
            cpuinfo = fopen(tempfile, "rb");
        }
        if (cpuinfo) {
            fseek(cpuinfo, 0L, SEEK_END);
            size1 = ftell(cpuinfo);
            fseek(cpuinfo, 0L, SEEK_SET);
            buffer = (char *)malloc(size1);
            size2 = fread(buffer, 1, size1, cpuinfo);
            if (size1 == size2) {
                loc1 = strstr(buffer, "model name");
                if (loc1) {
                    loc2 = strstr(loc1, ":");
                    if (loc2) {
                        loc2 += 2;
                        while (isspace(*loc2)) {
                            loc2++;
                        }
                        loc3 = strstr(loc2, "\n");
                        if (loc3) {
                            *loc3 = 0;
                            sprintf(netbsd_cpu, "%s", loc2);
                            got_netbsd_cpu = 1;
                        }
                    }
                }
            }
            fclose(cpuinfo);
            unlink(tempfile);
            lib_free(tempfile);
            if (buffer) {
                free(buffer);
            }
        }
#ifndef PLATFORM_NO_X86_ASM
        if (!got_netbsd_cpu) {
            sprintf(netbsd_cpu, "%s", platform_get_x86_runtime_cpu());
            got_netbsd_cpu = 1;
        }
#endif
        if (!got_netbsd_cpu) {
            uname(&name);
            sprintf(netbsd_cpu, "%s", name.machine);
            got_netbsd_cpu = 1;
        }
    }
    return netbsd_cpu;
}

char *platform_get_netbsd_runtime_os(void)
{
    struct utsname name;

    if (!got_netbsd_version) {
        uname(&name);

        sprintf(netbsd_version, "%s %s", name.sysname, name.release);

        got_netbsd_version = 1;
    }

    return netbsd_version;
}
#endif
