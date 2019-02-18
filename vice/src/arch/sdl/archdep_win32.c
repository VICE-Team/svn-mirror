/** \file   archdep_win32.c
 * \brief   Miscellaneous system-specific stuff
 *
 * \author  Tibor Biczo <crown@mail.matav.hu>
 * \author  Andreas Boose <viceteam@t-online.de>
 * \author  Marco van den Heuvel <blackystardust68@yahoo.com>
 * \author  Bas Wassink <b.wassink@ziggo.nl>
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

#include "vice.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <windows.h>
#include <winsock.h>
#include <tchar.h>

#include "ui.h"

#ifdef HAVE_DIR_H
#include <dir.h>
#endif

#ifdef HAVE_DIRECT_H
#include <direct.h>
#endif

#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif

#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif

#if defined(HAVE_IO_H) && !defined(WINMIPS)
#include <io.h>
#endif

#ifdef HAVE_PROCESS_H
#include <process.h>
#endif

#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "archdep.h"
#include "ioutil.h"
#include "kbd.h"
#include "keyboard.h"
#include "lib.h"
#include "log.h"
#include "machine.h"
#include "util.h"

#define STDIN_FILENO  0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

#ifndef O_BINARY
#define O_BINARY 0x8000
#endif

#ifndef _O_BINARY
#define _O_BINARY O_BINARY
#endif

#ifndef _O_WRONLY
#define _O_WRONLY O_WRONLY
#endif

#ifndef _O_TRUNC
#define _O_TRUNC O_TRUNC
#endif

#ifndef _O_CREAT
#define _O_CREAT O_CREAT
#endif

#ifndef S_IWRITE
#define S_IWRITE 0x200
#endif

#ifndef S_IREAD
#define S_IREAD 0x400
#endif

#ifndef LANG_ENGLISH
#define LANG_ENGLISH 0x09
#endif

#ifndef SUBLANG_ENGLISH_US
#define SUBLANG_ENGLISH_US 0x01
#endif

#ifndef SUBLANG_ENGLISH_UK
#define SUBLANG_ENGLISH_UK 0x02
#endif

#ifndef LANG_GERMAN
#define LANG_GERMAN 0x07
#endif

#ifndef SUBLANG_GERMAN
#define SUBLANG_GERMAN 0x01
#endif

#ifndef LANG_DANISH
#define LANG_DANISH 0x06
#endif

#ifndef SUBLANG_DANISH_DENMARK
#define SUBLANG_DANISH_DENMARK 0x01
#endif

#ifndef LANG_NORWEGIAN
#define LANG_NORWEGIAN  0x14
#endif

#ifndef SUBLANG_NORWEGIAN_BOKMAL
#define SUBLANG_NORWEGIAN_BOKMAL 0x01
#endif

#ifndef LANG_FINNISH
#define LANG_FINNISH 0x0b
#endif

#ifndef SUBLANG_FINNISH_FINLAND
#define SUBLANG_FINNISH_FINLAND 0x01
#endif

#ifndef LANG_ITALIAN
#define LANG_ITALIAN 0x10
#endif

#ifndef SUBLANG_ITALIAN
#define SUBLANG_ITALIAN 0x01
#endif


static char *argv0;

#if 0
static char *system_mbstowcs_alloc(const char *mbs)
{
    char *wcs;
    size_t len;

    if (mbs == NULL) {
        return NULL;
    }

    len = strlen(mbs);

    wcs = lib_malloc(len + 1);
    return memcpy(wcs, mbs, len + 1);
}

static void system_mbstowcs_free(char *wcs)
{
    lib_free(wcs);
}
#endif


static int archdep_init_extra(int *argc, char **argv)
{
    _fmode = O_BINARY;

    _setmode(_fileno(stdin), O_BINARY);
    _setmode(_fileno(stdout), O_BINARY);

    argv0 = lib_stralloc(argv[0]);

    return 0;
}


char *archdep_default_hotkey_file_name(void)
{
    return util_concat(archdep_boot_path(), "\\sdl-hotkey-", machine_get_name(), ".vkm", NULL);
}

char *archdep_default_joymap_file_name(void)
{
    return util_concat(archdep_boot_path(), "\\sdl-joymap-", machine_get_name(), ".vjm", NULL);
}


#ifndef _S_IREAD
#define _S_IREAD S_IREAD
#endif
#ifndef _S_IWRITE
#define _S_IWRITE S_IWRITE
#endif


/* set permissions of given file to rw, respecting current umask */
int archdep_fix_permissions(const char *file_name)
{
    return _chmod(file_name, _S_IREAD | _S_IWRITE);
}


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
            *p++ = lib_stralloc(buf);
        }
        mask <<= 1;
        ++i;
    }
    *p = NULL;

    return result;
}

char *archdep_get_current_drive(void)
{
    char *p = ioutil_current_dir();
    char *p2 = strchr(p, '\\');
    p2[0] = '/';
    p2[1] = '\0';
    return p;
}

void archdep_set_current_drive(const char *drive)
{
    if (_chdir(drive)) {
        ui_error("Failed to change drive to %s", drive);
    }
}
#endif

int archdep_require_vkbd(void)
{
    return 0;
}


static void archdep_shutdown_extra(void)
{
    lib_free(argv0);
}

void archdep_workaround_nop(const char *otto)
{
}

int archdep_rtc_get_centisecond(void)
{
    SYSTEMTIME t;

    GetSystemTime(&t);
    return (int)(t.wMilliseconds / 10);
}

#if defined(_MSC_VER)
#include "dirent.h"

struct _vice_dir {
    WIN32_FIND_DATA find_data;
    HANDLE handle;
    int first_passed;
    char *filter;
};

DIR *opendir(const char *path)
{
    DIR *dir;
    TCHAR *st_filter;

    dir = lib_malloc(sizeof(DIR));
    dir->filter = util_concat(path, "\\*", NULL);

    st_filter = system_mbstowcs_alloc(dir->filter);
    dir->handle = FindFirstFile(st_filter, &dir->find_data);
    system_mbstowcs_free(st_filter);
    if (dir->handle == INVALID_HANDLE_VALUE) {
        return NULL;
    }

    dir->first_passed = 0;
    return dir;
}

struct dirent *readdir(DIR *dir)
{
    static struct dirent ret;

    if (dir->first_passed) {
        if (!FindNextFile(dir->handle, &dir->find_data)) {
            return NULL;
        }
    }

    dir->first_passed = 1;
    ret.d_name = dir->find_data.cFileName;
    ret.d_namlen = (int)strlen(ret.d_name);

    return &ret;
}

void closedir(DIR *dir)
{
    FindClose(dir->handle);
    lib_free(dir->filter);
    lib_free(dir);
}
#endif


/* returns host keyboard mapping. used to initialize the keyboard map when
   starting with a black (default) config, so an educated guess works good
   enough most of the time :)

   FIXME: add more languages, constants are defined in winnt.h

   https://msdn.microsoft.com/en-us/library/windows/desktop/dd318693%28v=vs.85%29.aspx
*/
int kbd_arch_get_host_mapping(void)
{
    uintptr_t lang;
    int n;
    int maps[KBD_MAPPING_NUM] = {
        KBD_MAPPING_US, KBD_MAPPING_UK, KBD_MAPPING_DE, KBD_MAPPING_DA,
        KBD_MAPPING_NO, KBD_MAPPING_FI, KBD_MAPPING_IT, KBD_MAPPING_NL };
    int langids[KBD_MAPPING_NUM] = {
        MAKELANGID(LANG_ENGLISH,    SUBLANG_ENGLISH_US),
        MAKELANGID(LANG_ENGLISH,    SUBLANG_ENGLISH_UK),
        MAKELANGID(LANG_GERMAN,     SUBLANG_GERMAN),
        MAKELANGID(LANG_DANISH,     SUBLANG_DANISH_DENMARK),
        MAKELANGID(LANG_NORWEGIAN,  SUBLANG_NORWEGIAN_BOKMAL),
        MAKELANGID(LANG_FINNISH,    SUBLANG_FINNISH_FINLAND),
        MAKELANGID(LANG_ITALIAN,    SUBLANG_ITALIAN),
        MAKELANGID(LANG_DUTCH,      SUBLANG_DUTCH)
    };

    /* GetKeyboardLayout returns a pointer, but the first 16 bits of it return
     * a 'language identfier', whatever that is. This is seriously fucked */
    lang = (uintptr_t)(void *)GetKeyboardLayout(0);

    /* try full match first */
    lang &= 0xffff; /* lower 16 bit contain the language id */
    for (n = 0; n < KBD_MAPPING_NUM; n++) {
        if (lang == langids[n]) {
            return maps[n];
        }
    }
    /* try only primary language */
    lang &= 0x3ff; /* lower 10 bit contain the primary language id */
    for (n = 0; n < KBD_MAPPING_NUM; n++) {
        if (lang == (langids[n] & 0x3ff)) {
            return maps[n];
        }
    }
    return KBD_MAPPING_US;
}
