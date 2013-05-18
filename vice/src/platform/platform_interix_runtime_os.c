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
   - Interix 3.5 (Windows XP Pro)
*/

#include "vice.h"

#ifdef __INTERIX

#include <stdio.h>
#include <interix/interix.h>

static char interix_platform_version[100] = "";

char *platform_get_interix_runtime_os(void)
{
    char interix_version[10] = "";
    char windows_name[100] = "";

    getreg_strvalue((PCWSTR)INTERIX_KEY, (PCWSTR)REGVAL_CURRENTRELEASE, interix_version, 10);
    if (strlen(interix_version) == 0) {
        sprintf(interix_platform_version, "Unknown interix version");
    } else {
        sprintf(interix_platform_version, "Interix %s", interix_version);
    }

    getreg_strvalue((PCWSTR)NT_VERSION_KEY, (PCWSTR)NT_PRODUCT_NAME_VALUE, windows_name, 100);
    if (strlen(windows_name) == 0) {
        sprintf(interix_platform_version, "%s (Unknown windows)", interix_platform_version);
    } else {
        sprintf(interix_platform_version,"%s (%s)", interix_platform_version, windows_name);
    }
    return interix_platform_version;
}
#endif
