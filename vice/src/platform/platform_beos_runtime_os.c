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
*/

#include "vice.h"

#ifdef __BEOS__

#include <sys/utsname.h>
#include <string.h>

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
#endif
