/*
 * platform_sunos_runtime_os.c - SunOS runtime version discovery.
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
 * SunOS 4.1.1 (sun2)
 * SunOS 4.1.4 (sun4m)
*/

#include "vice.h"

#if (defined(sun) || defined(__sun)) && !(defined(__SVR4) || defined(__svr4__))

#include <sys/utsname.h>
#include <string.h>

char *platform_get_sunos_runtime_cpu(void)
{
    struct utsname name;

    uname(&name);
    if (!strcmp(name.machine, "sun") || !strcmp(name.machine, "sun1")) {
        return "68000";
    }
    if (!strcmp(name.machine, "sun2")) {
        return "68010";
    }
    if (!strcmp(name.machine, "sun3")) {
        return "68020";
    }
    if (!strcmp(name.machine, "sun3x")) {
        return "68030";
    }
    if (!strcmp(name.machine, "sun386") || !strcmp(name.machine, "sun386i")) {
        return "80386";
    }
    if (!strcmp(name.machine, "sun4") || !strcmp(name.machine, "sun4c") || !strcmp(name.machine, "sun4m")) {
        return "Sparc";
    }
    if (!strcmp(name.machine, "sun4u")) {
        return "Sparc64";
    }
    return "Unknown CPU";
}

static char osname[100];

char *platform_get_sunos_runtime_os(void)
{
    struct utsname name;

    uname(&name);

    sprintf(osname, "%s %s", name.sysname, name.release);

    return osname;    
}
#endif
