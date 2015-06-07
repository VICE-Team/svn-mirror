/*
 * platform_freebsd_runtime_os.c - FreeBSD runtime version discovery.
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
   cpu   | Operating System
   ------------------------
   i386  | FreeBSD 4.11
   i386  | FreeBSD 5.0
   i386  | FreeBSD 5.1
   i386  | FreeBSD 5.2
   i386  | FreeBSD 5.2.1
   i386  | FreeBSD 5.3
   i386  | FreeBSD 5.4
   i386  | FreeBSD 5.5
   i386  | FreeBSD 6.0
   i386  | FreeBSD 6.1
   i386  | FreeBSD 6.2
   i386  | FreeBSD 6.3
   i386  | FreeBSD 6.4
   i386  | FreeBSD 7.0
   i386  | FreeBSD 7.1
   i386  | FreeBSD 7.2
   i386  | FreeBSD 7.3
   i386  | FreeBSD 7.4
   i386  | FreeBSD 8.0
   i386  | FreeBSD 8.1
   i386  | FreeBSD 8.2
   i386  | FreeBSD 8.3
   i386  | FreeBSD 8.4
   i386  | FreeBSD 9.0
   i386  | FreeBSD 9.1
   i386  | FreeBSD 9.2
   i386  | FreeBSD 9.3
   i386  | FreeBSD 10.0
   i386  | FreeBSD 10.1
   amd64 | FreeBSD 10.1
   i386  | NetBSD (emulation layer)
 */

#include "vice.h"

#ifdef __FreeBSD__

#include <stdio.h>
#include <sys/utsname.h>
#include <sys/param.h>
#include <sys/sysctl.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

#include "archdep.h"
#include "lib.h"
#include "platform.h"
#include "util.h"
#include "log.h"

static char freebsd_version[100];
static char freebsd_cpu[100];
static int got_freebsd_version = 0;
static int got_freebsd_cpu = 0;

char *platform_get_freebsd_runtime_cpu(void)
{
    char *model = NULL;
    size_t len = 0;

    if (!got_freebsd_cpu) {
        sprintf(freebsd_cpu, "Unknown CPU");

        sysctlbyname("hw.model", NULL, &len, NULL, 0);
        model = lib_malloc(len);
        sysctlbyname("hw.model", model, &len, NULL, 0);

        sprintf(freebsd_cpu, "%s", model);

        if (model) {
            lib_free(model);
        }
        got_freebsd_cpu = 1;
    }
    return freebsd_cpu;
}

char *platform_get_freebsd_runtime_os(void)
{
    struct utsname name;

    if (!got_freebsd_version) {
        uname(&name);

        sprintf(freebsd_version, "%s %s", name.sysname, name.release);

        got_freebsd_version = 1;
    }

    return freebsd_version;
}
#endif
