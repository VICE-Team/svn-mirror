/*
 * archdep.c - Miscellaneous system-specific stuff.
 *
 * Written by
 *  Tibor Biczo <crown@mail.matav.hu>
 *  Andreas Boose <viceteam@t-online.de>
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
#include <tlhelp32.h>
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
#ifdef HAVE_IO_H
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
#include "lib.h"
#include "log.h"
#include "machine.h"
#include "system.h"
#include "util.h"


#define STDIN_FILENO        0
#define STDOUT_FILENO       1
#define STDERR_FILENO       2


static char *orig_workdir;
static char *argv0;


int archdep_init(int *argc, char **argv)
{
    _fmode = O_BINARY;

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
        if (s == NULL)
            s = argv0;
        else
            s++;
        e = strchr(s, '.');
        if (e == NULL)
            e = argv0 + strlen(argv0);

        len = e - s + 1;
        program_name = lib_malloc(len);
        memcpy(program_name, s, len - 1);
        program_name[len - 1] = 0;
    }

    return program_name;
}

static char *boot_path = NULL;

static HANDLE   hkernel = NULL;
static HANDLE   hpsapi = NULL;

typedef HANDLE (WINAPI * _CreateToolhelp32Snapshot)(
    DWORD dwFlags,
    DWORD th32ProcessID
);

typedef BOOL (WINAPI * _Module32First)(
    HANDLE hSnapshot,
    LPMODULEENTRY32 lpme
);

typedef BOOL (WINAPI * _Module32Next)(
    HANDLE hSnapshot,
    LPMODULEENTRY32 lpme
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

static BOOL verify_exe(char *file_name)
{
DWORD	version_info_size;
BOOL	bResult = FALSE;
char	*company_name = NULL;
int 	company_name_length = 0;

    if (version_info_size = GetFileVersionInfoSize(file_name, NULL)) {
        BYTE *version_info_buffer = lib_malloc(version_info_size);

        if (GetFileVersionInfo(file_name, (DWORD)NULL, version_info_size, (VOID*)version_info_buffer)) {
            if (VerQueryValue(version_info_buffer, "\\StringFileInfo\\04090000\\CompanyName", &company_name, &company_name_length)) {
                if (company_name) {
                    if (strncmp("Vice Team", company_name, company_name_length) == 0) {
                        bResult = TRUE;
                    }
                }
            }
        }

        lib_free(version_info_buffer);
    }

    return bResult;
}

const char *archdep_boot_path(void)
{
HANDLE          snap;
MODULEENTRY32   ment;
HANDLE          hproc;
int             cbneed;
int             i;
_CreateToolhelp32Snapshot   func_CreateToolhelp32Snapshot = NULL;
_Module32First              func_Module32First  = NULL;
_Module32Next               func_Module32Next = NULL;
_EnumProcessModules         func_EnumProcessModules = NULL;
_GetModuleFileNameEx        func_GetModuleFileNameEx = NULL;
char            *possible_trojan_path;

    possible_trojan_path = NULL;
    if (boot_path == NULL) {
        hkernel = LoadLibrary(TEXT("kernel32.dll"));
        if (hkernel) {
            OutputDebugString(TEXT("DLL: kernel32.dll loaded"));

            OutputDebugString(TEXT("DLL: getting address for CreateToolhelp32Snapshot"));
            func_CreateToolhelp32Snapshot
                = (_CreateToolhelp32Snapshot)GetProcAddress(hkernel,
                  TEXT("CreateToolhelp32Snapshot"));
            if (func_CreateToolhelp32Snapshot)
                OutputDebugString(TEXT("CreateToolhelp32Snaphshot success"));

            OutputDebugString(TEXT("DLL: getting address for Module32First"));
            func_Module32First = (_Module32First)GetProcAddress(hkernel,
                                 TEXT("Module32First"));
            if (func_Module32First)
                OutputDebugString(TEXT("Module32First success"));

            OutputDebugString(TEXT("DLL: getting address for Module32Next"));
            func_Module32Next = (_Module32Next)GetProcAddress(hkernel,
                                 TEXT("Module32Next"));
            if (func_Module32Next)
                OutputDebugString(TEXT("Module32Next success"));
        }

        hpsapi = LoadLibrary(TEXT("psapi.dll"));
        if (hpsapi) {
            OutputDebugString(TEXT("DLL: psapi.dll loaded"));

            OutputDebugString(TEXT("DLL: getting address for EnumProcessModules"));
            func_EnumProcessModules
                = (_EnumProcessModules)GetProcAddress(hpsapi,
                  TEXT("EnumProcessModules"));
            if (func_EnumProcessModules)
                OutputDebugString(TEXT("EnumProcessModules success"));

            OutputDebugString(TEXT("DLL: getting address for GetModuleFileNameEx"));
            func_GetModuleFileNameEx
                = (_GetModuleFileNameEx)GetProcAddress(hpsapi,
                  TEXT("GetModuleFileNameExA"));
            if (func_GetModuleFileNameEx)
                OutputDebugString(TEXT("GetModuleFileNameEx success"));
        }

        if (func_EnumProcessModules) {
            OutputDebugString(TEXT("BOOT path NT method"));
            hproc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
                                FALSE, GetCurrentProcessId());
            cbneed = 0;
            func_EnumProcessModules(hproc, NULL, 0, (LPDWORD)&cbneed);
            if (cbneed) {
                HMODULE *modules = lib_malloc(cbneed);
                int	n_modules = cbneed / sizeof(HMODULE);
                if (func_EnumProcessModules(hproc, modules, cbneed,
                    (LPDWORD)&cbneed)) {
                    for (i = 0; i < n_modules; i++) {
                        TCHAR st_temp[MAX_PATH];
                        char temp[MAX_PATH];
                        if (func_GetModuleFileNameEx(hproc, modules[i], st_temp,
                            MAX_PATH)) {
                            system_wcstombs(temp, st_temp, MAX_PATH);
                            OutputDebugString(st_temp);
                            if (verify_exe(temp)) {
                                util_fname_split(temp, &boot_path, NULL);
                                break;
                            } else if (i == 0) {
                                possible_trojan_path = lib_stralloc(temp);
                            }
                        }
                    }
                }
                lib_free(modules);
            }
            CloseHandle(hproc);
        } else if (func_CreateToolhelp32Snapshot && func_Module32First && func_Module32Next) {
            OutputDebugString(TEXT("BOOT path Win9x method"));
            snap = func_CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,
                                                 GetCurrentProcessId());
            memset(&ment, 0, sizeof(MODULEENTRY32));
            ment.dwSize = sizeof(MODULEENTRY32);
            func_Module32First(snap, &ment);
            OutputDebugString(ment.szExePath);
            if (verify_exe(ment.szExePath)) {
                util_fname_split(ment.szExePath, &boot_path, NULL);
            } else {
                possible_trojan_path = lib_stralloc(ment.szExePath);
                while (func_Module32Next(snap, &ment)) {
                    OutputDebugString(ment.szExePath);
                    if (verify_exe(ment.szExePath)) {
                        util_fname_split(ment.szExePath, &boot_path, NULL);
                        break;
                    }
                }
            }
            CloseHandle(snap);
        } else {
            TCHAR st_temp[MAX_PATH];
            char temp[MAX_PATH];
            OutputDebugString(TEXT("BOOT path NT4 without PSAPI"));
            if (GetModuleFileName(NULL, st_temp, MAX_PATH)) {
                system_wcstombs(temp, st_temp, MAX_PATH);
                if (!verify_exe(temp)) {
                    possible_trojan_path = lib_stralloc(temp);
                }
                util_fname_split(temp, &boot_path, NULL);
            } else {
                OutputDebugString(TEXT("Module file name could not be obtained"));
            }
        }
        OutputDebugString(TEXT("boot path:"));
        OutputDebugString(boot_path);

        /* This should not happen, but you never know...  */
        if (boot_path == NULL)
            boot_path = lib_stralloc(".\\");
    }

    if (possible_trojan_path) {
        archdep_startup_log_error("Vice detected a possible trojan running in its process space\n\n%s\n\nPlease run your favourite Antivirus software!", possible_trojan_path);

        lib_free(possible_trojan_path);
    }

    return boot_path;
}

char *archdep_default_sysfile_pathlist(const char *emu_id)
{
    static char *default_path;

    if (default_path == NULL) {
        const char *boot_path = archdep_boot_path();

        default_path = util_concat(boot_path, "\\", emu_id,
                                   ARCHDEP_FINDPATH_SEPARATOR_STRING,
                                   boot_path, "\\DRIVES",
                                   ARCHDEP_FINDPATH_SEPARATOR_STRING,
                                   boot_path, "\\PRINTER",
                                   NULL);
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
    return util_concat(archdep_boot_path(), "\\vice.ini", NULL);
}

char *archdep_default_fliplist_file_name(void)
{
    return util_concat(archdep_boot_path(), "\\fliplist-", 
		       machine_name, ".vfl", NULL);
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
    if (path == NULL)
        return 0;

    /* `c:\foo', `c:/foo', `c:foo', `\foo' and `/foo' are absolute.  */

    return !((isalpha(path[0]) && path[1] == ':')
            || path[0] == '/' || path[0] == '\\');
}

int archdep_spawn(const char *name, char **argv,
                  const char *stdout_redir, const char *stderr_redir)
{
#ifndef _S_IREAD
#define _S_IREAD S_IREAD
#endif
#ifndef _S_IWRITE
#define _S_IWRITE S_IWRITE
#endif

    int new_stdout, new_stderr;
    int old_stdout_mode, old_stderr_mode;
    int old_stdout, old_stderr;
    int retval;

    new_stdout = new_stderr = old_stdout = old_stderr = -1;

    /* Make sure we are in binary mode.  */
    old_stdout_mode = _setmode(STDOUT_FILENO, _O_BINARY);
    old_stderr_mode = _setmode(STDERR_FILENO, _O_BINARY);

    /* Redirect stdout and stderr as requested, saving the old
       descriptors.  */
    if (stdout_redir != NULL) {
        old_stdout = _dup(STDOUT_FILENO);
        new_stdout = _open(stdout_redir, _O_WRONLY | _O_TRUNC | _O_CREAT,
                           _S_IWRITE | _S_IREAD);
        if (new_stdout == -1) {
            log_error(LOG_DEFAULT, "open(\"%s\") failed: %s.",
                      stdout_redir, strerror(errno));
            retval = -1;
            goto cleanup;
        }
        _dup2(new_stdout, STDOUT_FILENO);
    }
    if (stderr_redir != NULL) {
        old_stderr = _dup(STDERR_FILENO);
        new_stderr = _open(stderr_redir, _O_WRONLY | _O_TRUNC | _O_CREAT,
                           _S_IWRITE | _S_IREAD);
        if (new_stderr == -1) {
            log_error(LOG_DEFAULT, "open(\"%s\") failed: %s.",
                      stderr_redir, strerror(errno));
            retval = -1;
            goto cleanup;
        }
        _dup2(new_stderr, STDERR_FILENO);
    }

    /* Spawn the child process.  */
    retval = _spawnvp(_P_WAIT, name, (const char **)argv);

cleanup:
    if (old_stdout >= 0) {
        _dup2(old_stdout, STDOUT_FILENO);
        _close(old_stdout);
    }
    if (old_stderr >= 0) {
        _dup2(old_stderr, STDERR_FILENO);
        _close(old_stderr);
    }
    if (old_stdout_mode >= 0)
        _setmode(STDOUT_FILENO, old_stdout_mode);
    if (old_stderr_mode >= 0)
        _setmode(STDERR_FILENO, old_stderr_mode);
    if (new_stdout >= 0)
        _close(new_stdout);
    if (new_stderr >= 0)
        _close(new_stderr);

    return retval;
}

/* return malloc´d version of full pathname of orig_name */
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

    ui_error_string(tmp);
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
    if (getenv("temp"))
        return util_concat(getenv("temp"), tmpnam(NULL), NULL);
    else if (getenv("tmp"))
        return util_concat(getenv("tmp"), tmpnam(NULL), NULL);
    else
        return lib_stralloc(tmpnam(NULL));
}

int archdep_file_is_gzip(const char *name)
{
    size_t l = strlen(name);

    if ((l < 4 || strcasecmp(name + l - 3, ".gz"))
        && (l < 3 || strcasecmp(name + l - 2, ".z"))
        && (l < 4 || toupper(name[l - 1]) != 'Z' || name[l - 4] != '.'))
        return 0;
    return 1;
}

int archdep_file_set_gzip(const char *name)
{
    return 0;
}

int archdep_mkdir(const char *pathname, int mode)
{
    return _mkdir(pathname);
}

int archdep_stat(const char *file_name, unsigned int *len, unsigned int *isdir)
{
    struct stat statbuf;

    if (stat(file_name, &statbuf) < 0)
        return -1;

    *len = statbuf.st_size;
    *isdir = S_ISDIR(statbuf.st_mode);

    return 0;
}

int archdep_file_is_blockdev(const char *name)
{
    return 0;
}

int archdep_file_is_chardev(const char *name)
{
    if (strcmp(name, "/dev/cbm") == 0)
        return 1;

    return 0;
}

void archdep_shutdown(void)
{
    lib_free(boot_path);
    lib_free(argv0);
    lib_free(orig_workdir);
}

void archdep_workaround_nop(const char *otto)
{
}

