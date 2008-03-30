
/*
 * archdep.c - Miscellaneous system-specific stuff.
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
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
//#define INCL_WINSYS
//#define INCL_WININPUT
//#define INCL_WINRECTANGLES
//#define INCL_WINWINDOWMGR

#include "vice.h"

#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#ifdef __IBMC__
#include <direct.h>
#endif

#include "findpath.h"
#include "archdep.h"
#include "utils.h"

#include "log.h"

static char *orig_workdir;
static char *argv0;
static char *program_name;
static char *boot_path;
static char *default_path;

static void restore_workdir(void)
{
    if (orig_workdir) chdir(orig_workdir);
}

static void pmlog(char *s, int i)
{
    FILE *fl=fopen("output","a");
    fprintf(fl,"%s %i\n",s,i);
    fclose(fl);
}

HAB habMain;
HMQ hmqMain;

void PM_close(void)
{
    WinDestroyMsgQueue(hmqMain);  // Destroy Msg Queue
    WinTerminate      (habMain);  // Release Anchor to PM
}

void PM_open(void)
{
    habMain = WinInitialize(0);              // Initialize PM
    hmqMain = WinCreateMsgQueue(habMain, 0); // Create Msg Queue
    atexit(PM_close);
}

/* ------------------------------------------------------------------------ */

int archdep_startup(int *argc, char **argv)
{
    FILE *fl;
    fl=fopen("output","w");
    fclose(fl);

    argv0 = (char*)strdup(argv[0]);
    orig_workdir = (char*) getcwd(NULL, GET_PATH_MAX);
    atexit(restore_workdir);

    PM_open();     // Open PM for usage

    return 0;
}

const char *archdep_program_name(void)
{
//#ifdef __IBMC__
    char drive[_MAX_DRIVE];
    char dir  [_MAX_DIR];
    char fname[_MAX_FNAME+_MAX_EXT];
    char ext  [_MAX_EXT];
    _splitpath(argv0, drive, dir, fname, ext);
    program_name = strdup((const char*)strcat(fname, ext));
//#else
//    program_name = strdup(_getname(argv0));
//#endif
    return program_name;
}

char *archdep_boot_path(void)
{
//#ifdef __IBMC__
    char drive[_MAX_DRIVE+_MAX_DIR];
    char dir  [_MAX_DIR];
    char fname[_MAX_FNAME+_MAX_EXT];
    char ext  [_MAX_EXT];
    _splitpath(argv0, drive, dir, fname, ext);
    if (strlen(dir)) *(dir+strlen(dir)-1) = 0; // cut last backslash

    boot_path = strdup((const char*)strcat(drive, dir));
/*#else
    char *start = (char*)  strdup(argv0);
    char *end   = (char*)_getname(start);
    if (end!=start) *(end-1) = 0; // cut last backslash
    else *end=0;
    boot_path = (char*)strdup(start);
    free(start);
#endif*/

    printf("boot_path: %s\n", boot_path);
    return boot_path;
}

const char *archdep_default_sysfile_pathlist(const char *emu_id)
{
    boot_path    = archdep_boot_path();
    default_path = concat(emu_id, FINDPATH_SEPARATOR_STRING,
                          "DRIVES", NULL);
    printf("default_path: %s\n",default_path);

    return default_path;
 }

const char *archdep_default_save_resource_file_name(void) {
    return archdep_default_resource_file_name();
}

const char *archdep_default_resource_file_name(void)
{
    static char *fname;

    if (fname) free(fname);

    fname = (char*)malloc(strlen(archdep_boot_path())+8);
    strcat(strcpy(fname,archdep_boot_path()), "\\vice.cfg");
    printf("fname resource: %s\n",fname);
    return fname;
}

FILE *archdep_open_default_log_file(void)
{
    char *fname;
    FILE *f;

    fname = (char*)malloc(strlen(archdep_boot_path())+10);
    strcat(strcpy(fname,archdep_boot_path()), "\\vice.log");
    printf("fname log: %s\n",fname);
    f = fopen(fname, "w");
    free(fname);

    return f;
}

int archdep_num_text_lines(void)
{
#ifdef __IBMC__
   return 25;
#else
   int dst[2];
   _scrsize(dst);
   return dst[1];
#endif
}

int archdep_num_text_columns(void)
{
#ifdef __IBMC__
   return 80;
#else
   int dst[2];
   _scrsize(dst);
   return dst[0];
#endif
}

int archdep_default_logger(const char *level_string, const char *format,
                                                                va_list ap)
{
    return 0;
}

static RETSIGTYPE break64(int sig)
{

#ifdef SYS_SIGLIST_DECLARED
    log_message(LOG_DEFAULT, "Received signal %d (%s).",
                sig, sys_siglist[sig]);
#else
    log_message(LOG_DEFAULT, "Received signal %d.", sig);
#endif

    exit (-1);
}

void archdep_setup_signals(int do_core_dumps)
{
    signal(SIGINT, SIG_IGN);

    if (!do_core_dumps) {
        signal(SIGSEGV,  break64);
        signal(SIGILL,   break64);
#ifdef __IBMC__
        signal(SIGABRT,  break64);
#else
        signal(SIGPIPE,  break64);
        signal(SIGHUP,   break64);
        signal(SIGQUIT,  break64);
#endif
    }
}

int archdep_path_is_relative(const char *path)
{
    return (isalpha(path[0]) && path[1] == ':'
            && (path[2] == '/' || path[2] == '\\'));
}

char *archdep_make_backup_filename(const char *fname)
{
    return NULL;
}

int archdep_expand_path(char **return_path, const char *filename)
{
    return 0;
}

/* Launch program `name' (searched via the PATH environment variable)
   passing `argv' as the parameters, wait for it to exit and return its
   exit status. If `stdout_redir' or `stderr_redir' are != NULL,
   redirect stdout or stderr to the corresponding file.  */
int archdep_spawn(const char *name, char **argv,
                  const char *stdout_redir, const char *stderr_redir)
{
    return 0;
}

/* Monitor console functions.  */
void archdep_open_monitor_console(FILE **mon_input, FILE **mon_output)
{
}

void archdep_close_monitor_console(FILE *mon_input, FILE *mon_output)
{
}


