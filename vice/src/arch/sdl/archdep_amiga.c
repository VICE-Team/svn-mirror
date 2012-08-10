/*
 * archdep_amiga.c
 *
 * Written by
 *  Mathias Roslund <vice.emu@amidog.se>
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

#ifndef __VBCC__
#define __USE_INLINE__
#endif

#include <proto/dos.h>
#include <proto/exec.h>

#ifndef AMIGA_OS4
#include <proto/socket.h>
#endif

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <errno.h>
#include <signal.h>
#include <sys/stat.h>
#include <unistd.h>

#include "archdep.h"
#include "findpath.h"
#include "ioutil.h"
#include "lib.h"
#include "log.h"
#include "machine.h"
#include "platform.h"
#include "ui.h"
#include "util.h"

#if defined(AMIGA_M68K) || defined(AMIGA_OS4)
#include <exec/execbase.h>
extern struct ExecBase *SysBase;
#endif

#if defined(AMIGA_AROS) && (defined(__i386__) || defined(__i486__) || defined(__i586__) || defined(__i686__)) && !defined(__amd64__) && !defined(__x86_64__)
#include "platform_x86_runtime_cpu.h"
#endif

static char *boot_path = NULL;
static int run_from_wb = 0;

#ifndef AMIGA_OS4
struct Library *SocketBase;
#endif

#ifdef SDL_AMIGA_INLINE
struct Library *SDLBase = NULL;
#define SDLLIBBASE SDLBase
#define SDLLIBNAME "SDL.library"
#endif

#ifdef POWERSDL_AMIGA_INLINE
struct Library *PowerSDLBase;
#define SDLLIBBASE PowerSDLBase
#define SDLLIBNAME "powersdl.library"
#endif

#if defined(SDL_AMIGA_INLINE) || defined(POWERSDL_AMIGA_INLINE)
void SDL_Quit(void)
{
    SDL_RealQuit();
    CloseLibrary(SDLLIBBASE);
}

int SDL_Init(Uint32 flags)
{
    SDLLIBBASE = OpenLibrary(SDLLIBNAME, 0L);

    if (!SDLLIBBASE) {
        printf("Unable to open %s\n", SDLLIBNAME);
        exit(0);
    }

    return SDL_RealInit(flags);
}
#endif

int archdep_network_init(void)
{
#ifndef AMIGA_OS4
    if (SocketBase == NULL) {
        SocketBase = OpenLibrary("bsdsocket.library", 3);
        if (SocketBase == NULL) {
            return -1;
        }
    }
#endif

    return 0;
}

void archdep_network_shutdown(void)
{
#ifndef AMIGA_OS4
    if (SocketBase != NULL) {
        CloseLibrary(SocketBase);
        SocketBase = NULL;
    }
#endif
}

int archdep_init_extra(int *argc, char **argv)
{
    if (*argc == 0) { /* run from WB */
        run_from_wb = 1;
    } else { /* run from CLI */
        run_from_wb = 0;
    }

    return 0;
}

char *archdep_program_name(void)
{
    static char *program_name = NULL;

    if (program_name == NULL) {
        char *p, name[1024];

        GetProgramName(name, 1024);
        p = FilePart(name);

        if (p != NULL) {
            program_name = lib_stralloc(p);
        }
    }

    return program_name;
}

const char *archdep_boot_path(void)
{
    if (boot_path == NULL) {
        char cwd[1024];
        BPTR lock;

        lock = GetProgramDir();
        if (NameFromLock(lock, cwd, 1024)) {
            if (cwd[strlen(cwd) - 1] != ':') {
                strcat(cwd, "/");
            }
            boot_path = lib_stralloc(cwd);
        }
    }

    return boot_path;
}

char *archdep_default_sysfile_pathlist(const char *emu_id)
{
    static char *default_path;

    if (default_path == NULL) {
        const char *boot_path;

        boot_path = archdep_boot_path();

        default_path = util_concat(emu_id, ARCHDEP_FINDPATH_SEPARATOR_STRING,
                                   boot_path, emu_id, ARCHDEP_FINDPATH_SEPARATOR_STRING,
                                   "DRIVES", ARCHDEP_FINDPATH_SEPARATOR_STRING,
                                   boot_path, "DRIVES", ARCHDEP_FINDPATH_SEPARATOR_STRING,
                                   "PRINTER", ARCHDEP_FINDPATH_SEPARATOR_STRING,
                                   boot_path, "PRINTER", NULL);
    }

    return default_path;
}

/* Return a malloc'ed backup file name for file `fname'.  */
char *archdep_make_backup_filename(const char *fname)
{
    return util_concat(fname, "~", NULL);
}

char *archdep_default_resource_file_name(void)
{
    const char *home;

    home = archdep_boot_path();
    return util_concat(home, "vice-sdl.ini", NULL);
}

char *archdep_default_fliplist_file_name(void)
{
    const char *home;

    home = archdep_boot_path();
    return util_concat(home, "fliplist-", machine_get_name(), ".vfl", NULL);
}

char *archdep_default_autostart_disk_image_file_name(void)
{
    const char *home;

    home = archdep_boot_path();
    return util_concat(home, "autostart-", machine_get_name(), ".d64", NULL);
}

char *archdep_default_hotkey_file_name(void)
{
    const char *home;

    home = archdep_boot_path();
    return util_concat(home, "sdl-hotkey-", machine_get_name(), ".vkm", NULL);
}

char *archdep_default_joymap_file_name(void)
{
    const char *home;

    home = archdep_boot_path();
    return util_concat(home, "sdl-joymap-", machine_get_name(), ".vjm", NULL);
}

char *archdep_default_save_resource_file_name(void)
{
    return archdep_default_resource_file_name();
}

FILE *archdep_open_default_log_file(void)
{
    if (run_from_wb) {
        char *fname;
        FILE *f;

        fname = util_concat(archdep_boot_path(), "vice.log", NULL);
        f = fopen(fname, MODE_WRITE_TEXT);

        lib_free(fname);

        if (f == NULL) {
            return stdout;
        }

        return f;
    } else {
        return stdout;
    }
}

int archdep_num_text_lines(void)
{
    return 25;
}

int archdep_num_text_columns(void)
{
    return 80;
}

int archdep_default_logger(const char *level_string, const char *txt)
{
    if (run_from_wb) {
        return 0;
    }

    if (fputs(level_string, stdout) == EOF || fprintf(stdout, txt) < 0 || fputc ('\n', stdout) == EOF) {
        return -1;
    }

    return 0;
}

int archdep_path_is_relative(const char *path)
{
    if (path == NULL) {
        return 0;
    }

    return (strchr(path, ':') == NULL);
}

int archdep_spawn(const char *name, char **argv, char **stdout_redir, const char *stderr_redir)
{
    return -1;
}

/* return malloc'd version of full pathname of orig_name */
int archdep_expand_path(char **return_path, const char *orig_name)
{
    BPTR lock;

    lock = Lock(orig_name, ACCESS_READ);
    if (lock) {
        char name[1024];
        LONG rc;
        rc = NameFromLock(lock, name, 1024);
        UnLock(lock);
        if (rc) {
            *return_path = lib_stralloc(name);
            return 0;
        }
    }
    *return_path = lib_stralloc(orig_name);
    return 0;
}

void archdep_startup_log_error(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    vfprintf(stderr, format, ap);
    va_end(ap);
}

char *archdep_filename_parameter(const char *name)
{
    return lib_stralloc(name);
}

char *archdep_quote_parameter(const char *name)
{
    return lib_stralloc(name);
}

char *archdep_tmpnam(void)
{
    return lib_stralloc(tmpnam(NULL));
}

FILE *archdep_mkstemp_fd(char **filename, const char *mode)
{
    char *tmp;
    FILE *fd;

    tmp = lib_stralloc(tmpnam(NULL));

    fd = fopen(tmp, mode);

    if (fd == NULL) {
        return NULL;
    }

    *filename = tmp;

    return fd;
}

int archdep_file_is_gzip(const char *name)
{
    size_t l = strlen(name);

    if ((l < 4 || strcasecmp(name + l - 3, ".gz")) && (l < 3 || strcasecmp(name + l - 2, ".z")) && (l < 4 || toupper(name[l - 1]) != 'Z' || name[l - 4] != '.')) {
        return 0;
    }

    return 1;
}

int archdep_file_set_gzip(const char *name)
{
    return 0;
}

int archdep_mkdir(const char *pathname, int mode)
{
    return mkdir(pathname, (mode_t)mode);
}

int archdep_stat(const char *file_name, unsigned int *len, unsigned int *isdir)
{
    struct stat statbuf;

    if (stat(file_name, &statbuf) < 0) {
        return -1;
    }

    *len = statbuf.st_size;
    *isdir = S_ISDIR(statbuf.st_mode);

    return 0;
}

/* set permissions of given file to rw, respecting current umask */
int archdep_fix_permissions(const char *file_name)
{
    SetProtection(file_name, 0);
    return 0;
}

int archdep_file_is_blockdev(const char *name)
{
    return 0;
}

int archdep_file_is_chardev(const char *name)
{
    return 0;
}

int archdep_require_vkbd(void)
{
    return 0;
}

void archdep_shutdown_extra(void)
{
    lib_free(boot_path);
}

#define LF (LDF_DEVICES | LDF_VOLUMES | LDF_ASSIGNS | LDF_READ)

static int CountEntries(void)
{
    int entries = 0;
    struct DosList *dl = LockDosList(LF);

    while (dl = NextDosEntry(dl, LF) {
        entries++;
    }
    UnlockDosList(LF);

    return entries;
}

char **archdep_list_drives(void)
{
    int drive_count = CountEntries();
    int i = 0;
    char **result, **p;
    struct DosList *dl = LockDosList(LF);

    result = lib_malloc(sizeof(char*) * drive_count);
    p = result;

    while (dl = NextDosEntry(dl, LF)) {
        *p++ = lib_stralloc(BADDR(dl->dol_Name));
        ++i;
    }
    result[i] = NULL;

    UnlockDosList(LF);

    return result;
}

char *archdep_get_current_drive(void)
{
    char *p = ioutil_current_dir();
    char *p2 = strchr(p, ':');

    if (p2 == NULL) {
        return lib_stralloc("PROGDIR:");
    }

    p2[1] = '\0';

    return p;
}

void archdep_set_current_drive(const char *drive)
{
    BPTR lck = Lock(drive, ACCESS_READ);

    if (lck) {
        CurrentDir(lck);
        Unlock(lck);
    }
}

#ifdef AMIGA_M68K
struct Library *WorkbenchBase;

static archdep_get_os3_runtime_os(void)
{
    char *retval = NULL;

    if (WorkbenchBase = OpenLibrary("workbench.library", 45)) {
        retval = "AmigaOS-3.9";
    }
    if (!retval && WorkbenchBase = OpenLibrary("workbench.library", 44)) {
        retval = "AmigaOS-3.5";
    }
    if (!retval && WorkbenchBase = OpenLibrary("workbench.library", 40)) {
        retval = "AmigaOS-3.1";
    }
    if (!retval && WorkbenchBase = OpenLibrary("workbench.library", 39)) {
        retval = "AmigaOS-3.0";
    }
    if (retval) {
        CloseLibrary(WorkbenchBase);
    } else {
        retval = "Unknown AmigaOS";
    }
    return retval;
}

static archdep_get_os3_runtime_cpu(void)
{
    UWORD attnflags = SysBase->AttnFlags;

    if (attnflags & 0x80) {
        return "68060";
    }
    if (attnflags & AFF_68040) {
        return "68040";
    }
    if (attnflags & AFF_68030) {
        return "68030";
    }
    if (attnflags & AFF_68020) {
        return "68020";
    }
    if (attnflags & AFF_68010) {
        return "68010";
    }
    return "68000";
}
#endif

#ifdef AMIGA_AROS
static archdep_get_aros_runtime_os(void)
{
    /* TODO: Add AROS native/hosted detection */
    return "AROS";
}

static archdep_get_aros_runtime_cpu(void)
{
#if defined(AMIGA_AROS) && (defined(__i386__) || defined(__i486__) || defined(__i586__) || defined(__i686__)) && !defined(__amd64__) && !defined(__x86_64__)
    return platform_get_x86_runtime_cpu();
#else
    /* TODO: Add AROS cpu detection (amd64/arm/ppc) */
    return "Unknown CPU";
#endif
}
#endif

#ifdef AMIGA_MORPHOS
static archdep_get_mos_runtime_os(void)
{
    /* TODO: Add MorphOS version detection */
    return "MorphOS";
}


static archdep_get_mos_runtime_cpu(void)
{
    /* TODO: Add PPC type detection */
    return "Unknown PPC CPU";
}
#endif

#ifdef AMIGA_OS4
static archdep_get_os4_runtime_os(void)
{
    /* TODO: Add AmigaOS4 version detection */
    return "AmigaOS4.x";
}

+static archdep_get_os4_runtime_cpu(void)
{
    UWORD attnflags = SysBase->AttnFlags;

    AFF_750     = (1<<10),
    AFF_7400    = (1<<11),
    AFF_ALTIVEC = (1<<12),
    AFF_4XX     = (1<<13),

    if (attnflags & AFF_603) {
        return "PPC603";
    }
    if (attnflags & AFF_604) {
        return "PPC604";
    }
    if (attnflags & AFF_750) {
        return "PPC750";
    }
    if (attnflags & AFF_ALTIVEC) {
        return "PPC-Altivec";
    }
    if (attnflags & AFF_4XX) {
        return "PPC4xx";
    }
    return "Unknown PPC CPU";
}
#endif

char *archdep_get_runtime_os(void)
{
#ifdef AMIGA_M68K
    return archdep_get_os3_runtime_os();
#endif

#ifdef AMIGA_OS4
    return archdep_get_os4_runtime_os();
#endif

#ifdef AMIGA_MORPHOS
    return archdep_get_mos_runtime_os();
#endif

#ifdef AMIGA_AROS
    return archdep_get_aros_runtime_os();
#endif
}

char *archdep_get_runtime_cpu(void)
{
#ifdef AMIGA_M68K
    return archdep_get_os3_runtime_cpu();
#endif

#ifdef AMIGA_OS4
    return archdep_get_os4_runtime_cpu();
#endif

#ifdef AMIGA_MORPHOS
    return archdep_get_mos_runtime_cpu();
#endif

#ifdef AMIGA_AROS
    return archdep_get_aros_runtime_cpu();
#endif
}
