/*
 * pm_cmdline.c - Commandline PM Classes
 *
 * Written by
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

#define INCL_WINSYS        // PP_FONTNAMESIZE
#define INCL_GPILCIDS      // vac++
#define INCL_WINFRAMEMGR
#define INCL_WINWINDOWMGR
#define INCL_WINSCROLLBARS
#include <os2.h>

#include <fcntl.h>
#include <io.h>
#include <math.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "ui.h"
#include "lib.h"
#include "cmdline.h"
#include "pm_cmdline.h"
#include "pm/scrollbars.h"

#define DB_RAISED 0x400
#define INITX 40  // init width  of window in chars (INITX<CHARS)
#define INITY 30  // init height of window in chars (INITY<LINES)

extern HAB habMain;
extern HMQ hmqMain;

static CHAR  szCmdClientClass [] = "VICE/2 Cmdline";
static CHAR  szCmdTitleBarText[] = "VICE/2 Commandline Options";
static CHAR  achFont          [] = "11.System VIO";
static ULONG flCmdFrameFlags     =
    FCF_TITLEBAR   | FCF_SYSMENU    | FCF_SHELLPOSITION | FCF_TASKLIST |
    FCF_VERTSCROLL | FCF_HORZSCROLL | FCF_SIZEBORDER    | FCF_MAXBUTTON;

static char *ui_cmdline_text;
static char *ui_cmdline_textopt;

MRESULT EXPENTRY PM_scrollProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    const ULONG flCmd =
        DT_TEXTATTRS|DT_VCENTER|DT_LEFT|DT_ERASERECT/*|DT_WORDBREAK*/;

    static HPS hps;
    static int cWidth, cHeight;    // size of one char in pixels
    static int lHorzPos, lVertPos; // actual horz and vert pos of SBM in chars
    static int w,h;                // size of visible region in chars

    switch (msg)
    {
    case WM_CREATE:
        WinSetPresParam(hwnd, PP_FONTNAMESIZE, strlen(achFont)+1,achFont);
        hps=WinGetPS(hwnd);
        {
            FONTMETRICS fmFont;
            GpiQueryFontMetrics(hps, sizeof(fmFont), &fmFont);
            cWidth  = fmFont.lAveCharWidth;    // width of one char
            cHeight = fmFont.lMaxBaselineExt;  // height of one char
        }
        WinSetWindowPos(WinQueryWindow(hwnd,QW_PARENT), HWND_TOP, 0, 0,
                        cWidth*INITX+
                        WinQuerySysValue(HWND_DESKTOP,SV_CXVSCROLL)+
                        WinQuerySysValue(HWND_DESKTOP,SV_CXBORDER)+
                        2*WinQuerySysValue(HWND_DESKTOP,SV_CXSIZEBORDER),
                        cHeight*(INITY+0.2)+
                        WinQuerySysValue(HWND_DESKTOP,SV_CYHSCROLL)+
                        WinQuerySysValue(HWND_DESKTOP,SV_CYTITLEBAR)+
                        2*WinQuerySysValue(HWND_DESKTOP,SV_CYSIZEBORDER)+
                        2*WinQuerySysValue(HWND_DESKTOP,SV_CYBORDER),
                        SWP_SIZE|SWP_SHOW|SWP_ZORDER); // Make visible, resize, top window
        lHorzPos=0;
        lVertPos=0;
        return FALSE;
    case WM_DESTROY:
        WinReleasePS(hps);
        return FALSE;
    case WM_SIZE:
        w = SHORT1FROMMP(mp2)/cWidth;  // Width  in chars
        h = SHORT2FROMMP(mp2)/cHeight; // Height in chars
        if (lHorzPos>ui_cmdline_chars-w-1) lHorzPos=ui_cmdline_chars-w;
        if (lVertPos>ui_cmdline_lines-h-1) lVertPos=ui_cmdline_lines-h;
        if (w>ui_cmdline_chars) lHorzPos=0;
        if (h>ui_cmdline_lines) lVertPos=0;
        SBMsetThumb(hwnd, FID_HORZSCROLL, lHorzPos, w, ui_cmdline_chars);
        SBMsetThumb(hwnd, FID_VERTSCROLL, lVertPos, h, ui_cmdline_lines);
        return FALSE;
    case WM_HSCROLL:
        SBMcalcPos(mp2, &lHorzPos, ui_cmdline_chars, w);
        SBMsetPos (hwnd, FID_HORZSCROLL, lHorzPos);
        WinPostMsg(hwnd, WM_PAINT, 0, 0);
        return FALSE;
    case WM_VSCROLL:
        SBMcalcPos(mp2, &lVertPos, ui_cmdline_lines, h);
        SBMsetPos (hwnd, FID_VERTSCROLL, lVertPos);
        WinPostMsg(hwnd, WM_PAINT, 0, 0);
        return FALSE;
    case WM_PAINT:
        {
            RECTL  rectl;
            int i, stop;
            //        WinQueryWindowPtr(hwnd,QWL_USER);
            WinQueryWindowRect(hwnd,&rectl);
            rectl.yBottom=rectl.yTop-1;
            WinFillRect(hps, &rectl, SYSCLR_FIELDBACKGROUND);
            i = rectl.yTop;
            rectl.yTop=cHeight;
            rectl.yBottom=0;
            WinFillRect(hps, &rectl, SYSCLR_FIELDBACKGROUND);
            rectl.yBottom=i-cHeight-1;
            //        WinFillRect(hps, &rectl, SYSCLR_FIELDBACKGROUND);
            //        pointl.x=0;
            //        pointl.y=rectl.yTop-cHeight-1;
            stop=(lVertPos+h>ui_cmdline_lines)?ui_cmdline_lines:lVertPos+h;
            for (i=lVertPos; i<stop; i++) {
                sprintf(ui_cmdline_textopt,"%s %s",ui_cmdline_options[i].name,
                        (ui_cmdline_options[i].need_arg && ui_cmdline_options[i].param_name)?
                        ui_cmdline_options[i].param_name:"");
                sprintf(ui_cmdline_text,optFormat,ui_cmdline_textopt,ui_cmdline_options[i].description);
                //            GpiCharStringAt(hps, &pointl,
                //                            strlen(ui_cmdline_text)-lHorzPos, ui_cmdline_text+lHorzPos);
                //            pointl.y-=cHeight;
                rectl.yTop=rectl.yBottom+cHeight;
                WinDrawText(hps, strlen(ui_cmdline_text)-lHorzPos, ui_cmdline_text+lHorzPos, &rectl, 0, 0, flCmd);
                rectl.yBottom-=cHeight;
            }
        }
        break;
    }
    return WinDefWindowProc (hwnd, msg, mp1, mp2);
}

void ui_cmdline_window(int optlen, int txtlen)
{
    HWND hwndFrame, hwndClient;
    QMSG qmsg;

    ui_cmdline_textopt=lib_calloc(1,optlen+1);
    ui_cmdline_text   =lib_calloc(1,txtlen+1);

    WinRegisterClass(habMain, szCmdClientClass, PM_scrollProc,
                     CS_SIZEREDRAW, 0);

    hwndFrame = WinCreateStdWindow(HWND_DESKTOP, 0, &flCmdFrameFlags,
                                   szCmdClientClass, szCmdTitleBarText, 0L, 0, 0,
                                   &hwndClient);
    //    WinSetWindowPtr(hwndClient, QWL_USER, (VOID*)(options));

    while (WinGetMsg (habMain, &qmsg, NULLHANDLE, 0, 0))
        WinDispatchMsg (habMain, &qmsg) ;

    WinDestroyWindow (hwndFrame);

    lib_free(ui_cmdline_text);
    lib_free(ui_cmdline_textopt);
}
