/*
 * platform_beos_runtime_os.c - BeOS/Zeta/Haiku runtime version discovery.
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
   - BeOS R5 PE x86
   - BeOS Max V4b1 x86
   - Zeta 1.21
   - Haiku R1 Alpha3 x86
   - Haiku R1 Alpha4 x86
   - Haiku R1 Alpha4 amd64/x86_64
*/

/* Binary compatibility table:
   compiled for | |running on ->
                v | BeoS 5.x | Zeta | Haiku r1 | haiku 64bit | PPC BeOS
   --------------------------------------------------------------------
   BeOS gcc 2.9   | yes      | yes  | yes      | NO          | NO
   BeOS gcc 2.95* | yes      | yes  | yes      | NO          | NO
   Zeta           | NO       | yes  | NO       | NO          | NO
   Haiku r1       | NO       | NO   | yes      | NO          | NO
   Haiku 64bit    | NO       | NO   | NO       | yes         | NO
   PPC BeOS       | NO       | NO   | NO       | NO          | yes
 */

#include "vice.h"

#ifdef BEOS_COMPILE

#include <sys/utsname.h>
#include <string.h>
#include <OS.h>

#include "platform.h"
#include "util.h"

/* this check is needed for haiku, since it always returns 1 on
   SupportsWindowMode() */
int CheckForHaiku(void)
{
    struct utsname name;

    uname(&name);
    if (!strncasecmp(name.sysname, "Haiku", 5)) {
        return -1;
    }
    return 0;
}

/* Ugly check for Zeta */
int CheckForZeta(void)
{
    return util_file_exists("/boot/beos/system/lib/libzeta.so");
}

char platform_name[128];

char *platform_get_haiku_runtime_os(void)
{
    struct utsname name;

    uname(&name);
    sprintf(platform_name, "Haiku (rev %s)", name.version);

    return platform_name;
}

char *platform_get_zeta_runtime_os(void)
{
    struct utsname name;

    uname(&name);
    if (name.release[0] == '6') {
        name.release[0] = '1';
    }
    sprintf(platform_name, "Zeta %s", name.release);

    return platform_name;
}

char *platform_get_beos_runtime_os(void)
{
    struct utsname name;

    uname(&name);
    sprintf(platform_name, "BeOS %s", name.release);

    return platform_name;
}

/* TODO: cpu_types enum in the Haiku version of OS.h
   lists many more PPC CPU types. */
char *platform_get_beosppc_runtime_cpu(void)
{
    system_info si;

    get_system_info(&si);

    switch (si.cpu_type) {
        case B_CPU_PPC_601:
            return "PPC601";
        case B_CPU_PPC_603:
            return "PPC603";
        case B_CPU_PPC_603e:
            return "PPC603e";
        case B_CPU_PPC_604:
            return "PPC604";
        case B_CPU_PPC_604e:
            return "PPC604e";
        case B_CPU_PPC_750:
            return "PPC750";
        case B_CPU_PPC_686:
            return "PPC686";
        default:
            return "Unknown PPC";
    }
}
#endif
