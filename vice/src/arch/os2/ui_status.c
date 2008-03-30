/*
 * ui_status.c - The user interface status window of Vice/2
 *
 * Written by
 *  Thomas Bretz (tbretz@gsi.de)
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
/*#define INCL_WINSYS
#define INCL_GPILCIDS // vac++
#define INCL_WINSTDFILE
#define INCL_WINFRAMEMGR
#define INCL_WINWINDOWMGR
#define INCL_WINSCROLLBARS

#include "vice.h"*/

#include "ui_status.h"

#include <stdlib.h>

#include "pm/pbar.h"
#include "log.h"

/* ------------------------ VICE/2 Status Window ------------------------ */

static CHAR  szStatusClntClass[] = "VICE/2";
static CHAR  szStatusBarTitle [] = "VICE/2 Status";
static ULONG flStatusFrameFlags = FCF_TITLEBAR|FCF_SHELLPOSITION;

#define DB_RAISED 0x400

void ui_close_status_window(void) {
    if (ui_status.init) {
        WinPostMsg(ui_status.init, WM_QUIT, 0,0);
        while (ui_status.hps) DosSleep(1);
    }
}

void ui_open_status_window(void) {
    _beginthread(PM_status,NULL,0x4000,NULL);
    atexit(ui_close_status_window);
}

void ui_display_speed(float spd, float fps, int sec)
{
    char str[80];
    RECTL rectl=ui_status.rectl;

    if (!ui_status.init) return;

    //    if (spd>ui_status.maxSpeed) ui_status.maxSpeed=spd;
    //    if (fps>ui_status.maxFps)   ui_status.maxFps  =fps;

    rectl.xLeft  +=2;                rectl.xRight =rectl.xLeft+50;
    rectl.yBottom =rectl.yTop-ui_status.step+2; rectl.yTop  -=2;
    sprintf(str,"%3.0f%%", spd);
    drawBar(ui_status.hps, rectl, spd, 100, str); // 100%

    rectl.xLeft  += 55;
    rectl.xRight += 55;
    sprintf(str,"%2.0ffps", fps);
    drawBar(ui_status.hps, rectl, fps, 50, str); // 50fps

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
            ui_enable_drive_status(ui_status.lastDriveState, 0);
            ui_display_drive_current_image(0, ui_status.lastImage[0]);
            ui_display_drive_current_image(1, ui_status.lastImage[1]);
            ui_display_drive_current_image(2, ui_status.lastImage[2]);
            ui_display_drive_current_image(3, ui_status.lastImage[3]);
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

void PM_status(void *unused)
{ // Status nach archdep???
    HAB  hab;
    HMQ  hmq;   // Handle to Msg Queue
    QMSG qmsg;  // Msg Queue Event
    HWND hwndFrame, hwndClient;

    //    archdep_setup_signals(0);
    //    ui_status.maxSpeed =0.01;
    //    ui_status.maxFps   =0.01;
    ui_status.lastSpeed=0;
    ui_status.lastFps  =0;
    ui_status.lastSec  =0;
    ui_status.lastDriveState=UI_DRIVE_ENABLE_NONE;

    hab = WinInitialize(0);            // Initialize PM
    hmq = WinCreateMsgQueue(hab, 0);   // Create Msg Queue

    // 2048 Byte Memory (Used eg for the Anchor Blocks
    WinRegisterClass(hab, szStatusClntClass, PM_statProc,
                     CS_SIZEREDRAW, 0);

    // maybe HWND_DESKTOP should be canvas->hwndFrame?
    hwndFrame = WinCreateStdWindow(HWND_DESKTOP, 0, &flStatusFrameFlags,
                                   szStatusClntClass, szStatusBarTitle,
                                   0L, 0, 0, &hwndClient);
    ui_status.step = WinQuerySysValue(HWND_DESKTOP, SV_CYTITLEBAR);
    WinSetWindowPos(hwndFrame, HWND_TOP, 0, 0,
                    20*ui_status.step+2, 6*ui_status.step+2,
                    SWP_SIZE|SWP_SHOW|SWP_ZORDER); // Make visible, resize, top window

    while (WinGetMsg (hab, &qmsg, NULLHANDLE, 0, 0))
        WinDispatchMsg (hab, &qmsg) ;

    log_message(LOG_DEFAULT, "ui_status.c: closing");
    WinDestroyWindow (hwndFrame);
    log_message(LOG_DEFAULT, "ui_status.c: window destroyed");
    WinDestroyMsgQueue(hmq);      // Destroy Msg Queue
    log_message(LOG_DEFAULT, "ui_status.c: msg queue sestroyed");
    WinTerminate (hab);           // Release Anchor to PM
    log_message(LOG_DEFAULT, "ui_status.c: PM anchor released");
    ui_status.hps=FALSE;  // used to indicate end of destroy
}

