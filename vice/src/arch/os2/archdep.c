
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
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

//#include "pm.h"

#include "fcntl.h"
#include "findpath.h"
#include "archdep.h"

#include "log.h"
#include "utils.h"
#include "kbd.h"

#include "pm/pbar.h"

static char *orig_workdir;
static char *argv0;

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

static CHAR  szStatusClntClass[] = "VICE/2";
static CHAR  szStatusBarTitle [] = "VICE/2 Status";
static ULONG flFrameFlags = FCF_TITLEBAR|FCF_SHELLPOSITION;

#define DB_RAISED 0x400

typedef struct _ui_status
{
    HPS   hps;
    RECTL rectl;
    BOOL  init;
    float maxSpeed;
    float maxFps;
    float lastSpeed;
    float lastFps;
    int   lastSec;
    float lastTrack[4];
    ui_drive_enable_t lastDriveState;
    
} ui_status_t;

ui_status_t ui_status;

void ui_display_speed(float spd, float fps, int sec)
{
    char str[80];
    RECTL rectl=ui_status.rectl;
    
    if (!ui_status.init) return;

    if (spd>ui_status.maxSpeed) ui_status.maxSpeed=spd;
    if (fps>ui_status.maxFps)   ui_status.maxFps  =fps;

    rectl.xLeft   +=2; rectl.xRight =rectl.xLeft+50;
    rectl.yBottom +=2; rectl.yTop  -=2;
    sprintf(str,"%3.0f%%", ui_status.maxSpeed);
    drawBar(ui_status.hps, rectl, spd, ui_status.maxSpeed, str, 100);

    rectl.xLeft  += 55;
    rectl.xRight += 55;
    sprintf(str,"%2.0ffps", ui_status.maxFps);
    drawBar(ui_status.hps, rectl, fps, ui_status.maxFps, str, 0);

    /*    rectl.xLeft  +=55; //rectl.yBottom -=1;
     rectl.xRight +=75; //rectl.yTop    -=1;
     sprintf(str,"%02i:%02i:%02i", (sec/(24*60))%24, (sec/60)%60, sec%60);
     WinDrawText(ui_status.hps, strlen(str), str, &rectl, 0, 0,
     DT_TEXTATTRS|DT_VCENTER|DT_CENTER|DT_ERASERECT);
     */
    ui_status.lastSpeed=spd;
    ui_status.lastFps  =fps;
    ui_status.lastSec  =sec;
}

extern int PM_winActive;
//extern int use_leds;

void ui_display_drive_led(int drive_number, int status)
{
    BYTE keyState[256];
    RECTL rectl=ui_status.rectl;
    int  height=rectl.yTop-rectl.yBottom;
    if (PM_winActive) {
        WinSetKeyboardStateTable(HWND_DESKTOP, keyState, FALSE);
        if (status) keyState[VK_CAPSLOCK] |=  1;
        else        keyState[VK_CAPSLOCK] &= ~1;
        WinSetKeyboardStateTable(HWND_DESKTOP, keyState, TRUE);
    }
    if (!ui_status.init || drive_number>1) return;
    rectl.xLeft   =rectl.xRight-(2-drive_number)*20-2;
    rectl.xRight  =rectl.xLeft+10+3;
    rectl.yTop    =height/2+10;
    rectl.yBottom =height/2+5;
    WinFillRect(ui_status.hps, &rectl, SYSCLR_BUTTONDARK);
    rectl.xLeft   +=1;
    rectl.xRight  -=1;
    rectl.yBottom +=1;
    rectl.yTop    -=1;
    WinFillRect(ui_status.hps, &rectl, status?4:SYSCLR_FIELDBACKGROUND);
}

void ui_display_drive_track(int drive_number, double track_number)
{
    char str[40];
    RECTL rectl=ui_status.rectl;
    int height=rectl.yTop-rectl.yBottom;
    if (!ui_status.init || drive_number>1) return;
    rectl.xLeft   =rectl.xRight-(2-drive_number)*20-4;
    rectl.xRight  =rectl.xLeft+10+6;
    rectl.yBottom =height/2-10;
    rectl.yTop    =height/2+5;
    sprintf(str,"%.0f",track_number);
    WinDrawText(ui_status.hps, strlen(str), str, &rectl, 0, 0,
                DT_TEXTATTRS|DT_VCENTER|DT_CENTER|DT_ERASERECT|
                (track_number-(int)track_number?DT_UNDERSCORE:0));
    ui_status.lastTrack[drive_number]=track_number;
}

void ui_enable_drive_status(ui_drive_enable_t state, int *drive_led_color)
{
    int i;
    RECTL rectl;
    int height=ui_status.rectl.yTop-ui_status.rectl.yBottom;
    if (!ui_status.init) return;
    for (i=0; i<2; i++) {
        rectl=ui_status.rectl;
        rectl.xLeft   = rectl.xRight-(2-i)*20-2;
        rectl.xRight  = rectl.xLeft+10+3;
        rectl.yTop    = height/2+10;
        rectl.yBottom = height/2+5;
        if ((state&(1<<i))) {
            ui_display_drive_track(i,ui_status.lastTrack[i]);
            WinFillRect(ui_status.hps, &rectl, SYSCLR_BUTTONDARK);
            rectl.xLeft   +=1;
            rectl.xRight  -=1;
            rectl.yBottom +=1;
            rectl.yTop    -=1;
        }
        WinFillRect(ui_status.hps, &rectl, SYSCLR_FIELDBACKGROUND);
    }
    ui_status.lastDriveState=state;
}

void ui_draw_status_window(HWND hwnd)
{
    WinQueryWindowRect(hwnd, &(ui_status.rectl));
    WinDrawBorder(ui_status.hps, &(ui_status.rectl), 1, 1,
                  SYSCLR_BUTTONDARK, SYSCLR_BUTTONMIDDLE, DB_RAISED);
    WinInflateRect(WinQueryAnchorBlock(hwnd), &(ui_status.rectl), -1, -1);
    WinFillRect(ui_status.hps, &(ui_status.rectl), SYSCLR_FIELDBACKGROUND);
}

MRESULT EXPENTRY PM_statProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
        switch (msg)
        {
        case WM_CREATE:
            if (ui_status.init) break;
            ui_status.hps = WinGetPS(hwnd);
            ui_status.init=hwnd;
            break;
        case WM_PAINT:
            if (!ui_status.init) break;
            ui_draw_status_window(hwnd);
            ui_display_speed(ui_status.lastSpeed, ui_status.lastFps, ui_status.lastSec);
            ui_enable_drive_status(ui_status.lastDriveState,0);
            break;
        case WM_QUIT:
            if (!ui_status.init) break;
            ui_status.init=FALSE;
            WinReleasePS(ui_status.hps);
            break;
        case WM_DESTROY:
            if (!ui_status.init) break;
            ui_status.init=FALSE;
            WinReleasePS(ui_status.hps);
            break;
        }
    return WinDefWindowProc (hwnd, msg, mp1, mp2);
}

void PM_status()
{ // Status nach archdep???
    HAB  hab;
    HMQ  hmq;   // Handle to Msg Queue
    QMSG qmsg;  // Msg Queue Event
    HWND hwndFrame, hwndClient;

    ui_status.maxSpeed =0.01;
    ui_status.maxFps   =0.01;
    ui_status.lastSpeed=0;
    ui_status.lastFps  =0;
    ui_status.lastSec  =0;
    ui_status.lastDriveState=0;

    hab = WinInitialize(0);            // Initialize PM
    hmq = WinCreateMsgQueue(hab, 0);   // Create Msg Queue

    // 2048 Byte Memory (Used eg for the Anchor Blocks
    WinRegisterClass(hab, szStatusClntClass, PM_statProc,
                     CS_SIZEREDRAW, 0);

    hwndFrame = WinCreateStdWindow(HWND_DESKTOP, 0, &flFrameFlags,
                                   szStatusClntClass, szStatusBarTitle,
                                   0L, 0, 0, &hwndClient);
    pmlog("StatusWindowOpened",0);
    WinSetWindowPos(hwndFrame, HWND_TOP, 0, 0, 160,
                    2*WinQuerySysValue(HWND_DESKTOP, SV_CYTITLEBAR)+2,
                    SWP_SIZE|SWP_SHOW|SWP_ZORDER); // Make visible, resize, top window

    while (WinGetMsg (hab, &qmsg, NULLHANDLE, 0, 0))
        WinDispatchMsg (hab, &qmsg) ;

    WinDestroyWindow (hwndFrame);
    WinDestroyMsgQueue(hmq);      // Destroy Msg Queue
    WinTerminate (hab);           // Release Anchor to PM
    ui_status.hps=0;  // used to indicate end of destroy
    pmlog("StatusWindowClosed",0);
}

HAB habMain;
HMQ hmqMain;

void closeStatus() {
    if (ui_status.init) {
        WinPostMsg(ui_status.init, WM_QUIT, 0,0);
        while (ui_status.hps);
    }
}

void openStatus() {
    _beginthread(PM_status,NULL,0x4000,NULL);
    atexit(closeStatus);
}
    

void PM_close()
{
    //------------------
    WinDestroyMsgQueue(hmqMain);  // Destroy Msg Queue
    WinTerminate      (habMain);  // Release Anchor to PM
}

void PM_open()
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
    openStatus();  // open Statuswindow

    return 0;
}

const char *archdep_program_name(void)
{
    static char *program_name;
    if (!program_name) program_name = (char*)strdup((const char*)_getname(argv0));
    return program_name;
}

const char *archdep_boot_path(void)
{
    static char *boot_path;

    if (!boot_path) {
        char *start = (char*)  strdup(argv0);
        char *end   = (char*)_getname(start);
        if (end!=start) *(end-1) = 0;
        else *end=0;
        boot_path = (char*)strdup(start);
        free(start);
    }

    printf("boot_path: %s\n",boot_path);
    return boot_path;
}

const char *archdep_default_sysfile_pathlist(const char *emu_id)
{
    static char *default_path;

    if (!default_path) {
        const char *boot_path = archdep_boot_path();

        default_path = concat(emu_id, FINDPATH_SEPARATOR_STRING,
                              "DRIVES", NULL);
    }
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
    f = fopen(fname, "wt");
    free(fname);

    return f;
}

int archdep_num_text_lines(void)
{
    int dst[2];
    _scrsize(dst);
    return dst[1];
}

int archdep_num_text_columns(void)
{
    int dst[2];
    _scrsize(dst);
    return dst[0];
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
        signal(SIGPIPE,  break64);
        signal(SIGHUP,   break64);
        signal(SIGQUIT,  break64);
    }
}

int archdep_path_is_relative(const char *path)
{
    return (isalpha(path[0]) && path[1] == ':'
            && (path[2] == '/' || path[2] == '\\'));
}
