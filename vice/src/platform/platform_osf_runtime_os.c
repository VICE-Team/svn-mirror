/*
 * platform_osf_runtime_os.c - OSF/DGUX/TRU64 runtime version discovery.
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
 *
 *
 * alpha-osf4.0E (Digital Unix)
 * mipsel-osf2
 */

#include "vice.h"

#ifdef __osf__

#include <sys/utsname.h>
#include <string.h>
#include <stdio.h>

static char cpu[10];
static int got_cpu = 0;

#ifdef __alpha__
static char *get_alpha_osf_cpu(void)
{
    char buffer[160];
    FILE *infile = NULL;
    char *retval = NULL;

    infile = popen("sizer -implver", "r");
    if (infile) {
        if (fgets(buffer, 159, infile)) {
            if (buffer[0] == 'E' && buffer[1] == 'V') {
                for (i = 0; buffer[i]; ++i) { }
                if (i) {
                    if (buffer[i - 1] == '\n') {
                        buffer[i - 1] = 0;
                    }
                    retval = lib_stralloc(buffer);
                }
            }
        }
        pclose(infile);
    }
    return retval;
}
#endif

char *platform_get_osf_runtime_cpu(void)
{
    struct utsname name;

#ifdef __alpha__
    char *cpu_version = NULL;
#endif

    if (!got_cpu) {
#ifdef __alpha__
        cpu_version = get_alpha_osf_cpu();
        if (cpu_version) {
            sprintf(cpu, "%s", cpu_version);
            lib_free(cpu_version);
            got_cpu = 1;
            return cpu;
        }
#endif

        uname(&name);
        sprintf(cpu, "%s", name.machine);
        got_cpu = 1;
    }
    return cpu;
}

static char osname[100];
static int got_os = 0;

#ifdef __alpha__
static char *get_alpha_osf_string(void)
{
    char buffer[160];
    FILE *infile = NULL;
    char *retval = NULL;
    int found = 0;
    int i;

    infile = popen("sizer -v", "r");
    if (infile) {
        if (fgets(buffer, 159, infile)) {
            for (i = 0; i < 160 && !found; ++i) {
                if (buffer[i] == '(') {
                    found = 1;
                }
            }
            if (found == 1) {
                i -= 2;
                while (buffer[i] == ' ' && i) {
                    --i;
                }
                if (i) {
                    buffer[i + 1] = 0;
                }
            }
        }
        pclose(infile);
        if (found == 1) {
            retval = lib_stralloc(buffer);
        }
    }
    return retval;
}
#endif

char *platform_get_osf_runtime_os(void)
{
    struct utsname name;

#ifdef __alpha__
    char *realname = NULL;
#endif

    if (!got_os) {
#ifdef __alpha__
        realname = get_alpha_osf_string();
        if (realname) {
            sprintf(osname, "%s", realname);
            lib_free(realname);
            got_os = 1;
            return osname;
        }
#endif
        uname(&name);
        sprintf(osname, "OSF %s", name.release);
        got_os = 1;
    }
    return osname;    
}
#endif
