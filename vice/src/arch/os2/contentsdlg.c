/*
 * contentsdlg.h - The dialog to show the disk-dir.
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
#define INCL_WINSYS
#define INCL_GPILCIDS // vac++
#define INCL_GPIPRIMITIVES
#define INCL_WINSTDFILE
#define INCL_WINBUTTONS
#define INCL_WINFRAMEMGR
#define INCL_WINWINDOWMGR
#define INCL_WINSCROLLBARS
#define INCL_DOSSEMAPHORES

#include "vice.h"

#include <stdlib.h>
#include <string.h>

#include "pm/winaddon.h"
#include "pm/scrollbars.h"

#include "log.h"
#include "types.h"
#include "utils.h"
#include "charsets.h"
#include "imagecontents.h"

static CHAR  szImgClientClass [] = "Image Contents";
static CHAR  szImgTitleBarText[] = "Image Contents";
static CHAR  achFont          [] = "11.System VIO";
static ULONG flImgFrameFlags     =
    FCF_TITLEBAR   | FCF_SYSMENU    | FCF_SHELLPOSITION | FCF_TASKLIST |
    FCF_VERTSCROLL | FCF_HORZSCROLL | FCF_SIZEBORDER    | FCF_MAXBUTTON;

MRESULT EXPENTRY pm_imagecontents (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    const ULONG flCmd =
        DT_TEXTATTRS|DT_VCENTER|DT_LEFT|DT_ERASERECT/*|DT_WORDBREAK*/;

    typedef struct _statics {
        HPS hps;
        int cWidth, cHeight;     // size of one char in pixels
        int lHorzPos, lVertPos;  // actual horz and vert pos of SBM in chars
        int w, h;                // size of visible region in chars
        int MAXX;
        int MAXY;
        image_contents_t *image;
        image_contents_file_list_t *first;
    } *statics_t;

    image_contents_file_list_t *entry;

    statics_t s=WinQueryWindowPtr(hwnd,QWL_USER); // Ptr to usr resources;

    switch (msg)
    {
    case WM_USER:
        s = xcalloc(1,sizeof(struct _statics));
        WinSetWindowPtr(hwnd, QWL_USER, s);
        s->MAXX=33;
        s->MAXY=0;
        s->lHorzPos=0;
        s->lVertPos=0;
        s->image=mp1;
        s->first=s->image->file_list;
        log_message(LOG_DEFAULT,"CONTENTS:  0 \"%s\" %s", p2a(s->image->name), p2a(s->image->id));
        entry=s->first;
        while (entry)
        {
            log_message(LOG_DEFAULT,"CONTENTS:  %-5i\"%s\"%6s",
                        entry->size, p2a(entry->name), p2a(entry->type));
            entry = entry->next;
            s->MAXY++;
        }
        s->MAXY += 4;
        log_message(LOG_DEFAULT,"CONTENTS:  %i blocks free.", s->image->blocks_free);
        WinSetPresParam(hwnd, PP_FONTNAMESIZE, strlen(achFont)+1,achFont);
        s->hps=WinGetPS(hwnd);
        {
            FONTMETRICS fmFont;
            GpiQueryFontMetrics(s->hps, sizeof(fmFont), &fmFont);
            s->cWidth  = fmFont.lAveCharWidth;    // width of one char
            s->cHeight = fmFont.lMaxBaselineExt;  // height of one char
        }
        WinSetWindowPos(WinQueryWindow(hwnd,QW_PARENT), HWND_TOP, 0, 0,
                        s->cWidth*s->MAXX+
                        WinQuerySysValue(HWND_DESKTOP,SV_CXVSCROLL)+
                        WinQuerySysValue(HWND_DESKTOP,SV_CXBORDER)+
                        2*WinQuerySysValue(HWND_DESKTOP,SV_CXSIZEBORDER),
                        s->cHeight*(s->MAXY>35?35:s->MAXY+0.2)+
                        WinQuerySysValue(HWND_DESKTOP,SV_CYHSCROLL)+
                        WinQuerySysValue(HWND_DESKTOP,SV_CYTITLEBAR)+
                        2*WinQuerySysValue(HWND_DESKTOP,SV_CYSIZEBORDER)+
                        2*WinQuerySysValue(HWND_DESKTOP,SV_CYBORDER),
                        SWP_SIZE|SWP_SHOW|SWP_ZORDER); // Make visible, resize, top window

        return FALSE;
    case WM_DESTROY:
        WinReleasePS(s->hps);
        free(s);
        return FALSE;
    case WM_SIZE:
        s->w = SHORT1FROMMP(mp2)/s->cWidth;  // Width  in chars
        s->h = SHORT2FROMMP(mp2)/s->cHeight; // Height in chars
        if (s->lHorzPos>s->MAXX-s->w-1) s->lHorzPos=s->MAXX-s->w;
        if (s->lVertPos>s->MAXY-s->h-1) s->lVertPos=s->MAXY-s->h;
        if (s->w>s->MAXX) s->lHorzPos=0;
        if (s->h>s->MAXY) s->lVertPos=0;
        SBMsetThumb(hwnd, FID_HORZSCROLL, s->lHorzPos, s->w, s->MAXX);
        SBMsetThumb(hwnd, FID_VERTSCROLL, s->lVertPos, s->h, s->MAXY);
        return FALSE;
    case WM_HSCROLL:
        SBMcalcPos(mp2, &(s->lHorzPos), s->MAXX, s->w);
        SBMsetPos (hwnd, FID_HORZSCROLL, s->lHorzPos);
        WinPostMsg(hwnd, WM_PAINT, 0, 0);
        return FALSE;
    case WM_VSCROLL:
        SBMcalcPos(mp2, &(s->lVertPos), s->MAXY, s->h);
        SBMsetPos (hwnd, FID_VERTSCROLL, s->lVertPos);
        WinPostMsg(hwnd, WM_PAINT, 0, 0);
        return FALSE;
    case WM_PAINT:
        {
            RECTL rectl;
            int i, stop;
            WinQueryWindowRect(hwnd,&rectl);
            rectl.yBottom=rectl.yTop-1;
            WinFillRect(s->hps, &rectl, SYSCLR_FIELDBACKGROUND);
            i = rectl.yTop;
            rectl.yTop=s->cHeight;
            rectl.yBottom=0;
            WinFillRect(s->hps, &rectl, SYSCLR_FIELDBACKGROUND);
            rectl.yBottom=i-s->cHeight-1;
            stop=(s->lVertPos+s->h>s->MAXY)?s->MAXY:s->lVertPos+s->h;
            i=2;
            entry=s->first;
            while (i++<s->lVertPos && entry) entry=entry->next;
            for (i=s->lVertPos; i<stop; i++)
            {
                char str[34];
                if (i==0 || i==s->MAXY-1) sprintf(str,"");
                else
                    if (i==1) sprintf(str,"  0 \"%s\" %s", s->image->name, s->image->id);
                    else
                        if (i==s->MAXY-2) sprintf(str,"  %i blocks free.", s->image->blocks_free);
                        else
                            if (entry) sprintf(str,"  %-5i\"%s\"%-6s", entry->size, entry->name, entry->type);
                            else sprintf(str,"  Aetsch!");
                //            WinFillRect(s->hps, &pointl, SYSCLR_FIELDBACKGROUND);
                //            GpiCharStringAt(s->hps, (POINTL*)&pointl, strlen(str)-s->lHorzPos, str+s->lHorzPos);
                rectl.yTop=rectl.yBottom+s->cHeight;
                WinDrawText(s->hps, strlen(str)-s->lHorzPos, str+s->lHorzPos, &rectl, 0, 0, flCmd);
                if (i==1) {
                    int l=rectl.xLeft;
                    int r=rectl.xRight;
                    rectl.xLeft = 4*s->cWidth;
                    rectl.xRight=28*s->cWidth;
                    WinInvertRect(s->hps, &rectl);
                    rectl.xLeft=l;
                    rectl.xRight=r;
                }
                rectl.yBottom-=s->cHeight;
                if (i>1 && i<s->MAXY-2) entry=entry->next;
            }
        }
        break;
    }
    return WinDefWindowProc (hwnd, msg, mp1, mp2);
}

/*void contents_dialog_close(void) {
    if (ui_status.init) {
        WinPostMsg(ui_status.init, WM_QUIT, 0,0);
        while (ui_status.hps) DosSleep(1);
    }
}*/

void contents_dialog(void *szFullFile)
{
//    atexit(ui_close_status_window);
    QMSG qmsg;
    HWND hwndFrame, hwndClient;

    image_contents_t *image=image_contents_read_disk((char*)szFullFile);

    HAB  hab = WinInitialize(0);            // Initialize PM
    HMQ  hmq = WinCreateMsgQueue(hab, 0);   // Create Msg Queue

    WinRegisterClass(hab, szImgClientClass, pm_imagecontents,
                     CS_SIZEREDRAW, 0x80);

    hwndFrame = WinCreateStdWindow(HWND_DESKTOP, 0, &flImgFrameFlags,
                                   szImgClientClass, szImgTitleBarText, 0L, 0, 0,
                                   &hwndClient);
    WinPostMsg(hwndClient, WM_USER, image, 0);

    while (WinGetMsg (hab, &qmsg, NULLHANDLE, 0, 0))
        WinDispatchMsg (hab, &qmsg);

    WinDestroyWindow (hwndFrame);
    WinDestroyMsgQueue(hmq);      // Destroy Msg Queue
    WinTerminate (hab);           // Release Anchor to PM
}

