/*
 * ui.c - A (very) simple user interface for MS-DOS.
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
#define INCL_WINSYS
#define INCL_WINSTDFILE
#define INCL_WINFRAMEMGR
#define INCL_WINWINDOWMGR
#define INCL_WINSCROLLBARS

#include "vice.h"

#include <fcntl.h>
#include <io.h>
#include <math.h>
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/hw.h>

#include "ui.h"

#include "cmdline.h"
#include "dos.h"
#include "machine.h"
//#include "menudefs.h"
#include "mon.h"
#include "resources.h"
#include "sound.h"
//#include "tui.h"
//#include "tuicharset.h"
//#include "tuimenu.h"
#include "types.h"
#include "utils.h"
#include "vsync.h"
#include "archdep.h"

/* ------------------------ ui resources ------------------------ */

/* Flag: Use keyboard LEDs?  */
static int use_leds;

static int set_use_leds(resource_value_t v)
{
    use_leds = (int) v;
    return 0;
}

static resource_t resources[] = {
    { "UseLeds", RES_INTEGER, (resource_value_t) 1,
      (resource_value_t *) &use_leds, set_use_leds },
    { NULL }
};

int ui_init_resources(void)
{
    return resources_register(resources);
}

static cmdline_option_t cmdline_options[] = {
    { "-leds", SET_RESOURCE, 0, NULL, NULL, "UseLeds", (resource_value_t) 1,
      NULL, "Enable usage of PC keyboard LEDs" },
    { "+leds", SET_RESOURCE, 0, NULL, NULL, "UseLeds", (resource_value_t) 0,
      NULL, "Disable usage of PC keyboard LEDs" },
    { NULL },
};

int ui_init_cmdline_options(void)
{
    return cmdline_register_options(cmdline_options);
}

/* ------------------------ VICE specific stuff ------------------------ */

int ui_init(int *argc, char **argv)
{
    return 0;
}

int ui_init_finish(void)
{
//    atexit(ui_exit);
    return 0;
}

void ui_main(char hotkey)
{
//    sound_suspend();
//    speed_index = vsync_get_avg_speed_index();
//    frame_rate = vsync_get_avg_frame_rate();

//    if (speed_index > 0.0 && frame_rate > 0.0)
//	sprintf(s, "%s emulator at %d%% speed, %d fps",
//		machine_name, (int)floor(speed_index), (int)floor(frame_rate));
//    suspend_speed_eval();
}

void ui_error(const char *format,...)
{
    char txt[1024];
    va_list ap;
    va_start(ap, format);
    vsprintf(txt, format, ap);
    ui_OK_dialog("VICE/2 Error", txt);
}

ui_jam_action_t ui_jam_dialog(const char *format,...)
{
    char txt[1024];
    va_list ap;
    va_start(ap, format);
    vsprintf(txt, format, ap);
    ui_OK_dialog("VICE/2 CPU JAM happend", txt);
    return UI_JAM_HARD_RESET;  // Always hard reset.
}

void ui_show_text(const char *title, const char *text)
{
    ui_OK_dialog(title, text);
}

void ui_update_menus(void)
{
/*    if (ui_main_menu != NULL)
        tui_menu_update(ui_main_menu);*/
}

int ui_extend_image_dialog(void)
{
    return ui_yesno_dialog("VICE/2 Extend Disk Image",
                           "Extend disk image in drive 8 to 40 tracks?");
}

/* ------------------------ OS/2 specific stuff ------------------------ */

void ui_OK_dialog(const char *title, const char *msg)
{
    WinMessageBox(HWND_DESKTOP, HWND_DESKTOP, msg, title, 0, MB_OK);
}

int ui_yesno_dialog(const char *title, const char *msg)
{
    return (WinMessageBox(HWND_DESKTOP, HWND_DESKTOP,
                          msg, title, 0, MB_YESNO)==MBID_YES);
}

int ui_file_dialog(char *title, char *drive,
                   char *path, char *button, char *result)
{
    FILEDLG filedlg;                      // File dialog info structure
    memset(&filedlg, 0, sizeof(FILEDLG)); // Initially set all fields to 0 

    // Initialize used fields in the FILEDLG structure
    filedlg.cbSize      = sizeof(FILEDLG); // Size of structure
    filedlg.fl          = FDS_CENTER | FDS_OPEN_DIALOG;
    filedlg.pszTitle    = title;
    filedlg.pszOKButton = button;
    filedlg.pszIDrive   = drive;

    strcpy(filedlg.szFullFile, path); // Init Path, Filter (*.t64)

    // Display the dialog and get the file
    if (WinFileDlg(HWND_DESKTOP, HWND_DESKTOP, &filedlg))
    {
        strcpy(result, filedlg.szFullFile);
        return filedlg.lReturn;
    }
    else return FALSE;
}

/* ------------------------ OS/2 specific Commandline Output ------------------------ */

#include "cmdline.h"
#include "pm/scrollbars.h"

#define DB_RAISED 0x400
#define INITX 40  // init width  of window in chars (INITX<CHARS)
#define INITY 30  // init height of window in chars (INITY<LINES)

extern HAB habMain;
extern HMQ hmqMain;

static CHAR  szClientClass [] = "VICE/2 Cmdline";
static CHAR  szTitleBarText[] = "VICE/2 Commandline Options";
static CHAR  achFont       [] = "11.System VIO";
static ULONG flFrameFlags     =
    FCF_TITLEBAR   | FCF_SYSMENU    | FCF_SHELLPOSITION | FCF_TASKLIST |
    FCF_VERTSCROLL | FCF_HORZSCROLL | FCF_SIZEBORDER    | FCF_MAXBUTTON;

int CHARS, LINES;   // maximum area could be shown

char optFormat[13];

char *text;
char *textopt;
static cmdline_option_t *options;

MRESULT EXPENTRY PM_scrollProc (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    static HPS hps;
    static int cWidth, cHeight;    // size of one char in pixels
    static int lHorzPos, lVertPos; // actual horz and vert pos of SBM in chars
    static int w,h;                // size of visible region in chars
    FONTMETRICS fmFont;
    RECTL  rectl;
    POINTL pointl;
    int i, stop;

    switch (msg)
    {
    case WM_CREATE:
        WinSetPresParam(hwnd, PP_FONTNAMESIZE, strlen(achFont)+1,achFont);
        hps=WinGetPS(hwnd);
        GpiQueryFontMetrics(hps, sizeof(fmFont), &fmFont);
        cWidth  = fmFont.lAveCharWidth;    // width of one char
        cHeight = fmFont.lMaxBaselineExt;  // height of one char
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
        if (lHorzPos>CHARS-w-1) lHorzPos=CHARS-w;
        if (lVertPos>LINES-h-1) lVertPos=LINES-h;
        if (w>CHARS) lHorzPos=0;
        if (h>LINES) lVertPos=0;
        SBMsetThumb(hwnd, FID_HORZSCROLL, lHorzPos, w, CHARS);
        SBMsetThumb(hwnd, FID_VERTSCROLL, lVertPos, h, LINES);
        return FALSE;
    case WM_HSCROLL:
        SBMcalcPos(mp2, &lHorzPos, CHARS, w);
        SBMsetPos (hwnd, FID_HORZSCROLL, lHorzPos);
        WinPostMsg(hwnd, WM_PAINT, 0, 0);
        return FALSE;
    case WM_VSCROLL:
        SBMcalcPos(mp2, &lVertPos, LINES, h);
        SBMsetPos (hwnd, FID_VERTSCROLL, lVertPos);
        WinPostMsg(hwnd, WM_PAINT, 0, 0);
        return FALSE;
    case WM_PAINT:
        //        WinQueryWindowPtr(hwnd,QWL_USER);
        WinQueryWindowRect(hwnd,&rectl);
        WinFillRect(hps, &rectl, SYSCLR_FIELDBACKGROUND);
        pointl.x=0;
        pointl.y=rectl.yTop-cHeight-1;
        stop=(lVertPos+h>LINES)?LINES:lVertPos+h;
        for (i=lVertPos; i<stop; i++) {
            sprintf(textopt,"%s %s",options[i].name,
                    (options[i].need_arg && options[i].param_name)?
                    options[i].param_name:"");
            sprintf(text,optFormat,textopt,options[i].description);
            GpiCharStringAt(hps, &pointl,
                            strlen(text)-lHorzPos, text+lHorzPos);
            pointl.y-=cHeight;
        }
        break;
    }
    return WinDefWindowProc (hwnd, msg, mp1, mp2);
}

void ui_cmdline_show_help(int num_options, cmdline_option_t *opts)
{
    HWND hwndFrame, hwndClient;
    QMSG qmsg;

    int i, j;
    int jmax=0;

    options=opts;
    LINES=num_options;
    for (i=0; i<LINES; i++) {
        j    =strlen(options[i].name)+1;
        j   +=strlen((options[i].need_arg && options[i].param_name)?
                     options[i].param_name:"")+1;
        jmax =j>jmax?j:jmax;
        j   +=strlen(options[i].description)+1;
        CHARS=j>CHARS?j:CHARS;
    }
    sprintf(optFormat,"%%-%ds%%s",jmax);
    textopt=malloc(jmax);
    text   =malloc(CHARS);

    WinRegisterClass(habMain, szClientClass, PM_scrollProc,
                     CS_SIZEREDRAW, 0);

    hwndFrame = WinCreateStdWindow(HWND_DESKTOP, 0, &flFrameFlags,
                                   szClientClass, szTitleBarText, 0L, 0, 0,
                                   &hwndClient);
    //    WinSetWindowPtr(hwndClient, QWL_USER, (VOID*)(options));
 
    while (WinGetMsg (habMain, &qmsg, NULLHANDLE, 0, 0))
        WinDispatchMsg (habMain, &qmsg) ;

    WinDestroyWindow (hwndFrame);

    free(text);
    free(textopt);
}
