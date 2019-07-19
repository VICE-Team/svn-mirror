
/*
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

#include "vice.h"
#include "archdep_defs.h"

#include "archdep.h"
#include "lib.h"

/* FIXME: includes for os/2 */
/* FIXME: includes for amiga */

#if defined(ARCHDEP_OS_WINDOWS)

/* FIXME: is this needed* */
#ifdef SDL_CHOOSE_DRIVES
char **archdep_list_drives(void)
{
    DWORD bits, mask;
    int drive_count = 1, i = 0;
    char **result, **p;

    bits = GetLogicalDrives();
    mask = 1;
    while (mask != 0) {
        if (bits & mask) {
            ++drive_count;
        }
        mask <<= 1;
    }
    result = lib_malloc(sizeof(char*) * drive_count);
    p = result;
    mask = 1;
    while (mask != 0) {
        if (bits & mask) {
            char buf[16];
            sprintf(buf, "%c:/", 'a' + i);
            *p++ = lib_strdup(buf);
        }
        mask <<= 1;
        ++i;
    }
    *p = NULL;

    return result;
}
#endif

#endif

#if defined(ARCHDEP_OS_OS2)

/* FIXME: is this needed* */
#ifdef SDL_CHOOSE_DRIVES
char **archdep_list_drives(void)
{
    char **result, **p;
    ULONG dn = 0;
    FSINFO buffer = {0};
    APIRET rc = NO_ERROR;
    int drive_count = 1;
    int i;
    int drives[26];

    drives[0] = 0;
    drives[1] = 0;
    for (i = 3; i <= 26; ++i) {
        dn = (ULONG)i;
        rc = DosQueryFSInfo(dn, FSIL_VOLSER, &buffer, sizeof(FSINFO));
        if (rc == NO_ERROR) {
            drives[i - 1] = 1;
            ++drive_count;
        } else {
            drives[i - 1] = 0;
        }
    }

    result = lib_malloc(sizeof(char*) * drive_count);
    p = result;

    for (i = 2; i < 26; ++i) {
        if (drives[i]) {
            char buf[16];
            sprintf(buf, "%c:/", 'a' + i);
            *p++ = lib_strdup(buf);
        }
    }
    *p = NULL;

    return result;
}
#endif

#endif

#if defined (ARCHDEP_OS_AMIGA)

#define LF (LDF_DEVICES | LDF_VOLUMES | LDF_ASSIGNS | LDF_READ)

static int CountEntries(void)
{
    int entries = 1;
    struct DosList *dl = LockDosList(LF);

    while (dl = NextDosEntry(dl, LF)) {
        entries++;
    }
    UnLockDosList(LF);

    return entries;
}

char **archdep_list_drives(void)
{
    int drive_count = CountEntries();
    char **result, **p;
    struct DosList *dl = LockDosList(LF);

    result = lib_malloc(sizeof(char*) * drive_count);
    p = result;

    while (dl = NextDosEntry(dl, LF)) {
        *p++ = lib_strdup(BADDR(dl->dol_Name));
    }
    *p = NULL;

    UnLockDosList(LF);

    return result;
}

#endif
