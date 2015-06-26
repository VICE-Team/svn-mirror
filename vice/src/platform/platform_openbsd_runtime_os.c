/*
 * platform_openbsd_runtime_os.c - OpenBSD runtime version discovery.
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
   amd64 | OpenBSD 5.7
   i386  | OpenBSD 5.7

 */

#include "vice.h"

#ifdef __OpenBSD__

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

static char openbsd_version[100];
static char openbsd_cpu[100];
static int got_openbsd_version = 0;
static int got_openbsd_cpu = 0;

static char *GetModelSysCtl(void)
{
    int Query[2];
    static char Model[128];
    size_t	Length = sizeof(Model);

    Query[0] = CTL_HW;
    Query[1] = HW_MODEL;
    if (sysctl(Query, 2, Model, &Length, NULL, 0) == -1) {
        return NULL;
    }

    return Model;
}

char *platform_get_openbsd_runtime_cpu(void)
{
    char *model = NULL;
    size_t len = 0;

    if (!got_openbsd_cpu) {
        sprintf(openbsd_cpu, "Unknown CPU");

        model = GetModelSysCtl();

        if (model) {
            sprintf(openbsd_cpu, "%s", model);
        }

        got_openbsd_cpu = 1;
    }
    return openbsd_cpu;
}

char *platform_get_openbsd_runtime_os(void)
{
    struct utsname name;

    if (!got_openbsd_version) {
        uname(&name);

        sprintf(openbsd_version, "%s %s", name.sysname, name.release);

        got_openbsd_version = 1;
    }

    return openbsd_version;
}
#endif
