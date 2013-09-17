/*
 * platform_solaris_runtime_os.c - Solaris runtime version discovery.
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
*/

#include "vice.h"

#if (defined(sun) || defined(__sun)) && (defined(__SVR4) || defined(__svr4__))

#include <sys/utsname.h>
#include <string.h>

char *platform_get_solaris_runtime_os(void)
{
    struct utsname name;

    uname(&name);
    if (!strcasecmp(name.release, "5.3")) {
        return "Solaris 3";
    }
    if (!strcasecmp(name.release, "5.4")) {
        return "Solaris 4";
    }
    if (!strcasecmp(name.release, "5.5.1")) {
        return "Solaris 5";
    }
    if (!strcasecmp(name.release, "5.6")) {
        return "Solaris 6";
    }
    if (!strcasecmp(name.release, "5.7")) {
        return "Solaris 7";
    }
    if (!strcasecmp(name.release, "5.8")) {
        return "Solaris 8";
    }
    if (!strcasecmp(name.release, "5.9")) {
        return "Solaris 9";
    }
    if (!strcasecmp(name.release, "5.10")) {
        return "Solaris 10";
    }
    if (!strcasecmp(name.release, "5.11")) {
        if (!strcasecmp(name.version, "11.0")) {
            return "Solaris 11";
        } else {
            return "OpenSolaris";
        }
    }
    return "Unknown Solaris version";
}
#endif
