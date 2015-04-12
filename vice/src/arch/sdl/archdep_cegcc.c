/*
 * archdep_cegcc.c - Miscellaneous system-specific stuff.
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

#include "vice.h"

#include <windows.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h>

#include "archdep.h"
#include "lib.h"
#include "log.h"
#include "machine.h"
#include "ui.h"
#include "util.h"

#define STDIN_FILENO  0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

#define WSADESCRIPTION_LEN 256
#define WSASYS_STATUS_LEN  128

typedef struct WSAData {
    WORD wVersion;
    WORD wHighVersion;
    char szDescription[WSADESCRIPTION_LEN + 1];
    char szSystemStatus[WSASYS_STATUS_LEN + 1];
    unsigned short iMaxSockets;
    unsigned short iMaxUdpDg;
    char *lpVendorInfo;
} WSADATA;
typedef WSADATA *LPWSADATA;

extern int PASCAL WSAStartup(WORD, LPWSADATA);
extern int PASCAL WSACleanup(void);

static char *orig_workdir;
static char *argv0;

static size_t system_mbstowcs(char *wcs, const char *mbs, size_t len)
{
    strncpy(wcs, mbs, len);
    return strlen(wcs);
}

static char *system_mbstowcs_alloc(const char *mbs)
{
    char *wcs;

    if (mbs == NULL) {
        return NULL;
    }

    wcs = lib_malloc((strlen(mbs) + 1) * sizeof(char));
    system_mbstowcs(wcs, mbs, strlen(mbs) + 1);

    return wcs;
}

static void system_mbstowcs_free(char *wcs)
{
    lib_free(wcs);
}

int archdep_network_init(void)
{
    WORD wVersionRequested = MAKEWORD(1, 1);
    WSADATA wsaData;

    WSAStartup(wVersionRequested, &wsaData);

    return 0;
}

void archdep_network_shutdown(void)
{
    WSACleanup();
}

static int archdep_init_extra(int *argc, char **argv)
{
    _setmode(_fileno(stdin), O_BINARY);
    _setmode(_fileno(stdout), O_BINARY);

    argv0 = lib_stralloc(argv[0]);

    orig_workdir = getcwd(NULL, MAX_PATH);

    return 0;
}

static char *program_name = NULL;

char *archdep_program_name(void)
{
    if (program_name == NULL) {
        char *s, *e;
        int len;

        s = strrchr(argv0, '\\');
        if (s == NULL) {
            s = argv0;
        } else {
            s++;
        }
        e = strchr(s, '.');
        if (e == NULL) {
            e = argv0 + strlen(argv0);
        }

        len = e - s + 1;
        program_name = lib_malloc(len);
        memcpy(program_name, s, len - 1);
        program_name[len - 1] = 0;
    }

    return program_name;
}

typedef HANDLE (WINAPI * _CreateToolhelp32Snapshot)(
    DWORD dwFlags,
    DWORD th32ProcessID
);

typedef BOOL (WINAPI * _EnumProcessModules)(
    HANDLE hProcess,      // handle to process
    HMODULE *lphModule,   // array of module handles
    DWORD cb,             // size of array
    LPDWORD lpcbNeeded    // number of bytes required
);

typedef DWORD (WINAPI * _GetModuleFileNameEx)(
    HANDLE hProcess,    // handle to process
    HMODULE hModule,    // handle to module
    LPTSTR lpFilename,  // path buffer
    DWORD nSize         // maximum characters to retrieve
);

char boot_path[MAX_PATH];

const char *archdep_boot_path(void)
{
    char *checkpath;

    GetModuleFileName(NULL, boot_path, MAX_PATH);

    checkpath = boot_path + strlen(boot_path);

    while (*checkpath != '\\') {
        checkpath--;
    }
    *checkpath = 0;

    return boot_path;
}

char *archdep_default_sysfile_pathlist(const char *emu_id)
{
    static char *default_path;

    if (default_path == NULL) {
        const char *boot_path = archdep_boot_path();

        default_path = util_concat(boot_path, "\\", emu_id, ARCHDEP_FINDPATH_SEPARATOR_STRING,
                                   boot_path, "\\DRIVES", ARCHDEP_FINDPATH_SEPARATOR_STRING,
                                   boot_path, "\\PRINTER", NULL);
    }

    return default_path;
}

/* Return a malloc'ed backup file name for file `fname'.  */
char *archdep_make_backup_filename(const char *fname)
{
    char *tmp;

    tmp = util_concat(fname, NULL);
    tmp[strlen(tmp) - 1] = '~';
    return tmp;
}

char *archdep_default_save_resource_file_name(void)
{
    return archdep_default_resource_file_name();
}

char *archdep_default_resource_file_name(void)
{
    return util_concat(archdep_boot_path(), "\\sdl-vice.ini", NULL);
}

char *archdep_default_fliplist_file_name(void)
{
    return util_concat(archdep_boot_path(), "\\fliplist-", machine_get_name(), ".vfl", NULL);
}

char *archdep_default_rtc_file_name(void)
{
    return util_concat(archdep_boot_path(), "\\sdl-vice.rtc", NULL);
}

char *archdep_default_autostart_disk_image_file_name(void)
{
    return util_concat(archdep_boot_path(), "\\autostart-", machine_get_name(), ".d64", NULL);
}

char *archdep_default_hotkey_file_name(void)
{
    return util_concat(archdep_boot_path(), "\\sdl-hotkey-", machine_get_name(), ".vkm", NULL);
}

char *archdep_default_joymap_file_name(void)
{
    return util_concat(archdep_boot_path(), "\\sdl-joymap-", machine_get_name(), ".vjm", NULL);
}

FILE *archdep_open_default_log_file(void)
{
    char *fname;
    FILE *f;

    fname = util_concat(archdep_boot_path(), "\\vice.log", NULL);
    f = fopen(fname, "wt");
    lib_free(fname);

    return f;
}

int archdep_default_logger(const char *level_string, const char *txt)
{
    TCHAR *st_out;
    char *out = lib_msprintf("*** %s %s\n", level_string, txt);

    st_out = system_mbstowcs_alloc(out);
    OutputDebugString(st_out);
    system_mbstowcs_free(st_out);
    lib_free(out);
    return 0;
}

int archdep_path_is_relative(const char *path)
{
    if (path == NULL) {
        return 0;
    }

    /* `c:\foo', `c:/foo', `c:foo', `\foo' and `/foo' are absolute.  */

    return !((isalpha(path[0]) && path[1] == ':') || path[0] == '/' || path[0] == '\\');
}

int archdep_spawn(const char *name, char **argv, char **pstdout_redir, const char *stderr_redir)
{
    return -1;
}

/* return malloc'd version of full pathname of orig_name */
int archdep_expand_path(char **return_path, const char *orig_name)
{
    /*  Win32 version   */
    *return_path = lib_stralloc(orig_name);
    return 0;
}

void archdep_startup_log_error(const char *format, ...)
{
    char *tmp;
    va_list args;

    va_start(args, format);
    tmp = lib_mvsprintf(format, args);
    va_end(args);

    ui_error(tmp);
    lib_free(tmp);
}

char *archdep_quote_parameter(const char *name)
{
    char *a;

    a = util_concat("\"", name, "\"", NULL);
    return a;
}

char *archdep_filename_parameter(const char *name)
{
    char *exp;
    char *a;

    archdep_expand_path(&exp, name);
    a = archdep_quote_parameter(exp);
    lib_free(exp);
    return a;
}

char *archdep_tmpnam(void)
{
    if (getenv("temp")) {
        return util_concat(getenv("temp"), tmpnam(NULL), NULL);
    } else if (getenv("tmp")) {
        return util_concat(getenv("tmp"), tmpnam(NULL), NULL);
    } else {
        return lib_stralloc(tmpnam(NULL));
    }
}

FILE *archdep_mkstemp_fd(char **filename, const char *mode)
{
    char *tmp;
    FILE *fd;

    if (getenv("temp")) {
        tmp = util_concat(getenv("temp"), tmpnam(NULL), NULL);
    } else if (getenv("tmp")) {
        tmp = util_concat(getenv("tmp"), tmpnam(NULL), NULL);
    } else {
        tmp = lib_stralloc(tmpnam(NULL));
    }

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
    return mkdir(pathname, mode);
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
    return 0;
}

int archdep_file_is_blockdev(const char *name)
{
    return 0;
}

int archdep_file_is_chardev(const char *name)
{
    if (strcmp(name, "/dev/cbm") == 0) {
        return 1;
    }

    return 0;
}

int archdep_require_vkbd(void)
{
    return 0;
}

int archdep_rename(const char *oldpath, const char *newpath)
{
    return rename(oldpath, newpath);
}

static void archdep_shutdown_extra(void)
{
    lib_free(argv0);
    lib_free(orig_workdir);
}

char *archdep_get_runtime_os(void)
{
    /* TODO: add runtime os detection code */
    return "Unknown OS";
}

char *archdep_get_runtime_cpu(void)
{
    /* TODO: add runtime cpu detection code */
    return "Unknown CPU";
}

/* returns host keyboard mapping. used to initialize the keyboard map when
   starting with a black (default) config, so an educated guess works good
   enough most of the time :)

   FIXME: add more languages/actual detection
*/
int kbd_arch_get_host_mapping(void)
{
    return KBD_MAPPING_US;
}
