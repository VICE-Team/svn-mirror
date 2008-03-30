/*
 * archdep.c - Miscellaneous system-specific stuff.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Thomas Bretz <tbretz@gsi.de>
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

#define INCL_DOSQUEUES     /* Queue commands     */
#define INCL_DOSSESMGR     /* DosStartSession    */
#define INCL_DOSMEMMGR     /* DosFreeMem         */
#define INCL_DOSPROFILE     // DosTmrQueryTime
#define INCL_DOSPROCESS    /* DosGetInfoBlock    */
#define INCL_DOSMODULEMGR  /* DosQueryModuleName */
#define INCL_DOSSEMAPHORES /* Dos-*-MutexSem     */

#define INCL_DOSMISC
#define INCL_DOSFILEMGR

#include <os2.h>

#include <stdarg.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <process.h>
#ifdef __IBMC__
#include <io.h>
#include <fcntl.h>
#include <direct.h>
#include <sys/stat.h>
#endif

#include "findpath.h"
#include "archdep.h"
#include "utils.h"

#include "log.h"

#include "ui.h"
#include "signals.h"
#include "vsyncarch.h"

/* ---------------------- OS/2 specific ------------------ */

static char *orig_workdir;
static char argv0[CCHMAXPATH];

static void restore_workdir(void)
{
    if (orig_workdir) chdir(orig_workdir);
}

HAB habMain;
HMQ hmqMain;

void PM_close(void)
{
    APIRET rc;

    log_message(LOG_DEFAULT, "archdep.c: PM_close");

    rc=WinDestroyMsgQueue(hmqMain);  // Destroy Msg Queue
    if (!rc)
        log_message(LOG_DEFAULT, "archdep.c: Error! Msg Queue not destroyed (rc=%li)",rc);

    rc=WinTerminate(habMain);  // Release Anchor to PM
    if (!rc)
        log_message(LOG_DEFAULT, "archdep.c: Error! PM anchor release.");
}

void PM_open(void)
{
    habMain = WinInitialize(0);              // Initialize PM
    hmqMain = WinCreateMsgQueue(habMain, 0); // Create Msg Queue

    atexit(PM_close);
}

/* ------------------------------------------------------------------------ */
void archdep_create_mutex_sem(HMTX *hmtx, const char *pszName, int fState)
{
    APIRET rc;

    char *sem = xmalloc(strlen(pszName)+15);

    sprintf(sem, "\\SEM32\\VICE2\\%s%i", pszName, vsyncarch_gettime());

    if (rc=DosCreateMutexSem(sem, hmtx, 0, fState))
        log_message(LOG_DEFAULT, "archdep.c: DosCreateMutexSem '%s' (rc=%i)", pszName, rc);
}


HMTX hmtxSpawn;

int archdep_startup(int *argc, char **argv)
{
    /* This is right way to do this in OS/2 (not via argv[0]) */
    TIB *pTib;
    PIB *pPib;
    DosGetInfoBlocks(&pTib, &pPib);
    DosQueryModuleName(pPib->pib_hmte, CCHMAXPATH, argv0);

    orig_workdir = (char*) getcwd(NULL, GET_PATH_MAX);
    atexit(restore_workdir);

    PM_open();

    archdep_create_mutex_sem(&hmtxSpawn, "Spawn", FALSE);

    return 0;
}

const char *archdep_program_name(void)
{
    static char *name=NULL;
    if (!name) {
        char drive[_MAX_DRIVE];
        char dir  [_MAX_DIR];
        char fname[_MAX_FNAME+_MAX_EXT];
        char ext  [_MAX_EXT];
        _splitpath(argv0, drive, dir, fname, ext);
        name = concat(fname, ext, NULL);
    }
    return name;
}

const char *archdep_boot_path(void)
{
    static char *boot_path=NULL;
    if (!boot_path) {
        char drive[_MAX_DRIVE+_MAX_DIR];
        char dir  [_MAX_DIR];
        char fname[_MAX_FNAME+_MAX_EXT];
        char ext  [_MAX_EXT];
        _splitpath(argv0, drive, dir, fname, ext);
        if (strlen(dir)) *(dir+strlen(dir)-1) = '\0'; // cut last backslash
        boot_path = concat(drive, dir, NULL);
     }
    return boot_path;
}

const char *archdep_default_sysfile_pathlist(const char *emu_id)
{
    static char *pathlist=NULL;

    if (!pathlist)
        pathlist=concat(emu_id, FINDPATH_SEPARATOR_STRING, "DRIVES", NULL);

    return pathlist;
}

const char *archdep_default_save_resource_file_name(void)
{
    return archdep_default_resource_file_name();
}

const char *archdep_default_resource_file_name(void)
{
    static char *filename=NULL;

    if (!filename)
        filename = concat(archdep_boot_path(), "\\vice2.cfg", NULL);

    return filename;
}

int archdep_default_logger(const char *level_string,
                           const char *format, va_list ap)
{
    return 0;
}

FILE *archdep_open_default_log_file(void)
{
    char *fname = concat (archdep_boot_path(), "\\vice2.log", NULL);

    FILE *f = fopen(fname, "w");
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

static RETSIGTYPE break64(int sig)
{
    char sigtxt[255];
    sprintf(sigtxt,
            "Received signal %d (%s). Vice will be closed.",
            sig, sys_siglist[sig]);
    log_message(LOG_DEFAULT, sigtxt);
#if !defined __C1541__ && !defined __PETCAT__
    WinMessageBox(HWND_DESKTOP, HWND_DESKTOP,
                  sigtxt, "VICE/2 Exception", 0, MB_OK);
#endif
    exit (-1);
}

void archdep_setup_signals(int do_core_dumps)
{
    // at the place where it's called at the moment it's only valid
    // for the vice (simulation) thread

    signal(SIGINT, SIG_IGN);

    if (!do_core_dumps) {
        signal(SIGSEGV,  break64);
        signal(SIGILL,   break64);
        signal(SIGFPE,   break64);
        signal(SIGABRT,  break64);
        //        signal(SIGINT,   break64);
        signal(SIGTERM,  break64);
        signal(SIGUSR1,  break64);
        signal(SIGUSR2,  break64);
        signal(SIGBREAK, break64);
#ifdef __IBMC__
        signal(SIGUSR3,  break64);
#else
        signal(SIGPIPE,  break64);
        signal(SIGHUP,   break64);
        signal(SIGQUIT,  break64);
#endif
    }
}

int archdep_path_is_relative(const char *path)
{
    return !(isalpha(path[0]) && path[1] == ':' &&
            (path[2] == '/' || path[2] == '\\') ||
            (path[0] == '/' || path[0] == '\\'));
}

/* Return a malloc'ed backup file name for file `fname'.  */
char *archdep_make_backup_filename(const char *fname)
{
    return concat(fname, "~", NULL);
}

/* return malloced version of full pathname of filename */
int archdep_expand_path(char **return_path, const char *filename)
{
    if (filename[0] == '\\' || filename[1] == ':')
        *return_path = stralloc(filename);
    else
    {
        char *cwd = get_current_dir();
        *return_path = concat(cwd, "\\", filename, NULL);
        free(cwd);
    }
    return 0;
}

int archdep_search_path(const char *name, char *pBuf, int lBuf)
{
    const int flags = SEARCH_CUR_DIRECTORY|SEARCH_IGNORENETERRS;
    char *path      = "";        /* PATH environment variable */
    char *pgmName   = concat(name, ".exe", NULL);

    // Search the program in the path
    if (DosScanEnv("PATH",&path))
        log_message(LOG_DEFAULT, "archdep.c: Environment variable PATH not found.");
    if (DosSearchPath(flags, path, pgmName, pBuf, lBuf))
    {
        log_message(LOG_DEFAULT, "archdep.c: File \"%s\" not found.", pgmName);
        return -1;
    }
    free(pgmName);
    return 0;
}

char *archdep_cmdline(const char *name, char **argv, const char *sout, const char *serr)
{
    char *res;
    int length = 0;
    int i      = 0;
    while (argv[++i]) length += strlen(argv[i]);
    length += i+strlen(name)+3+
        (sout?strlen(sout)+5:0)+
        (serr?strlen(serr)+6:0); // need space for the spaces
    res = xcalloc(1,length);
    i = 0;
    strcat(strcpy(res,"/c "),name);
    while (argv[++i]) strcat(strcat(res," "), argv[i]);
    if (sout) strcat(strcat(strcat(res,  " > \""), sout), "\"");
    if (serr) strcat(strcat(strcat(res, " 2> \""), serr), "\"");
    return res;
}

/* Launch program `name' (searched via the PATH environment variable)
   passing `argv' as the parameters, wait for it to exit and return its
   exit status. If `stdout_redir' or `stderr_redir' are != NULL,
   redirect stdout or stderr to the corresponding file.  */
int archdep_spawn(const char *name, char **argv,
                  const char *stdout_redir, const char *stderr_redir)
{  // how to redirect stdout & stderr??
    typedef struct _CHILDINFO {  /* Define a structure for the queue data */
        USHORT usSessionID;
        USHORT usReturn;
    } CHILDINFO;

    UCHAR       fqName[256] = ""; /* Result of PATH search             */
    HQUEUE      hqQueue;          /* Queue handle                      */
    REQUESTDATA rdRequest;        /* Request data for the queue        */
    ULONG       ulSzData;         /* Size of the queue data            */
    BYTE        bPriority;        /* For the queue                     */
    PVOID       pvData;           /* Pointer to the queue data         */
    STARTDATA   sd;               /* Start Data for DosStartSession    */
    PID         pid;              /* PID for the started child session */
    ULONG       ulSession;        /* Session ID for the child session  */
    APIRET      rc;               /* Return code from API's            */
    char       *cmdline;

    if (archdep_search_path(name, fqName, sizeof(fqName)))
        return -1;

    // Make the needed command string
    cmdline = archdep_cmdline(fqName, argv, stdout_redir, stderr_redir);
#ifndef __C1541__
    log_message(LOG_DEFAULT, "archdep.c: Spawning \"cmd.exe %s\"", cmdline);
#endif

    memset(&sd,0,sizeof(STARTDATA));
    sd.Length     = sizeof(STARTDATA);
    sd.FgBg       = SSF_FGBG_BACK;      /* Start session in background */
    sd.Related    = SSF_RELATED_CHILD;  /* Start a child session       */
    sd.PgmName    = "cmd.exe";
    sd.PgmInputs  = cmdline;
    sd.PgmControl = SSF_CONTROL_INVISIBLE;  // | SSF_CONTROL_NOAUTOCLOSE;
    sd.TermQ      = "\\QUEUES\\VICE2\\CHILD.QUE";
    sd.InheritOpt = SSF_INHERTOPT_SHELL;

    /* Start a child process and return it's session ID.
     Wait for the session to end and get it's session ID
     from the termination queue */

    // this prevents you from closing Vice while a child is running
    if (DosRequestMutexSem(hmtxSpawn, SEM_INDEFINITE_WAIT))
        return 0;

    if(rc=DosCreateQueue(&hqQueue, QUE_FIFO|QUE_CONVERT_ADDRESS, sd.TermQ))
        log_message(LOG_DEFAULT,"archdep.c: Error in DosCreateQueue (rc=%li).",rc);
    else
    {
        if(rc=DosStartSession(&sd, &ulSession, &pid))  /* Start the child session */
            log_message(LOG_DEFAULT,"archdep.c: Error in DosStartSession (rc=%li).",rc);
        else
        {
            if(rc=DosReadQueue(hqQueue, &rdRequest, &ulSzData,        /* Wait for the child session to end (you'll have to end it */
                               &pvData, 0, DCWW_WAIT, &bPriority, 0)) /* in some other way) */
                log_message(LOG_DEFAULT,"archdep.c: Error in DosReadQueue (rc=%li).",rc);
            else
            {
                rc = ((CHILDINFO*)pvData)->usReturn;
                DosFreeMem(pvData); /* Free the memory of the queue data element read */
            }
        }
        DosCloseQueue(hqQueue);
    }
    DosReleaseMutexSem(hmtxSpawn);
    free(cmdline);
    //    DosSleep(1000);
    if (rc)
        log_message(LOG_DEFAULT, "archdep.c: Return Code: rc = %li", rc);

    return rc;
}

void archdep_startup_log_error(const char *format, ...)
{
    char txt[1024];
    va_list ap;
    va_start(ap, format);
    vsprintf(txt, format, ap);
#if !defined __C1541__ && !defined __PETCAT__
    WinMessageBox(HWND_DESKTOP, HWND_DESKTOP,
                  txt, "VICE/2 Startup Error", 0, MB_OK);
#else
    printf(txt);
#endif
}


char *archdep_quote_parameter(const char *name)
{
    char *a = concat("\"", name, "\"", NULL);
    return a;
}


char *archdep_filename_parameter(const char *name)
{
    char *exp;
    char *a;
    archdep_expand_path(&exp, name);
    a = archdep_quote_parameter(exp);
    free(exp);
    return a;
}


char *archdep_tmpnam(void)
{
    return stralloc(tmpnam(NULL));
}

