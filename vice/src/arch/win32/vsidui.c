/*
 * vsidui.c - Implementation of the VSID UI.
 *
 * Written by
 *  Dag Lem <resid@nimrod.no>
 *
 * Win32 implementation
 *  Emiliano 'iAN CooG' Peruch <iancoog@email.it>
 *  Adapted to common API by Andreas Matthies
 *
 * based on c64ui.c written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  André Fachat <fachat@physik.tu-chemnitz.de>
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
#include <windows.h>
#include "vice.h"
#include "winmain.h"
#include "res.h"
#include "resources.h"

#include "log.h"
#include "machine.h"
#include "psid.h"

enum {VSID_S_TITLE=0
     ,VSID_S_AUTHOR
     ,VSID_S_RELEASED
     ,VSID_S_SYNC
     ,VSID_S_MODEL
     ,VSID_S_IRQ
     ,VSID_S_PLAYING
     ,VSID_S_TIMER
     ,VSID_S_LASTLINE
     };

int psid_ui_set_tune(resource_value_t tune, void *param);

char szAppName[]="WinVice VSID GUI";
char vsidstrings[VSID_S_LASTLINE+1][80]={{0}};

typedef struct psid_s {
    /* PSID data */
    WORD version;
    WORD data_offset;
    WORD load_addr;
    WORD init_addr;
    WORD play_addr;
    WORD songs;
    WORD start_song;
    DWORD speed;
    BYTE name[32];
    BYTE author[32];
    BYTE copyright[32];
    WORD flags;
    BYTE start_page;
    BYTE max_pages;
    WORD reserved;
    WORD data_size;
    BYTE data[65536];

    /* Non-PSID data */
    DWORD frames_played;
} psid_t;


static int current_song;
static int songs;
static int default_song;

MSG  msg ;
HWND hwnd;
WNDCLASS wndclass;

static long CALLBACK window_proc(HWND window, UINT msg,
                                 WPARAM wparam, LPARAM lparam);

/*****************************************************************************/
void vsid_disp(int txout_x, int txout_y, const char *str1, const char* str2)
{
    HDC hDC;
    RECT r;
    SIZE size;
    char dummy[100];

    if (NULL != hwnd)
    {
        hDC = GetDC(hwnd);

        if(NULL != str2)
        {
            SelectObject (hDC, GetStockObject (SYSTEM_FIXED_FONT)) ;
            GetTextExtentPoint32( hDC, " ", 1, &size );
            sprintf(dummy,str1,str2);
            SetBkColor(hDC, GetSysColor(COLOR_BTNFACE));
            TextOut(hDC, 3+(txout_x*size.cx),
                         3+(txout_y*(size.cy+3)),
                         dummy, strlen(dummy));
        }
        else
        {
            GetClientRect(hwnd, &r);
            FillRect(hDC, &r, GetSysColorBrush(COLOR_BTNFACE) );
        }
        ReleaseDC(hwnd, hDC);
    }
}
/*****************************************************************************/
int vsid_ui_init(void)
{
    wndclass.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wndclass.lpfnWndProc   = window_proc;
    wndclass.cbClsExtra    = 0 ;
    wndclass.cbWndExtra    = 0 ;
    wndclass.hInstance     = winmain_instance ;
    wndclass.hIcon         = LoadIcon(winmain_instance,
                               MAKEINTRESOURCE(IDI_ICON1));
    wndclass.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = GetSysColorBrush(COLOR_BTNFACE);
    wndclass.lpszMenuName  = szAppName ;
    wndclass.lpszClassName = szAppName ;

    RegisterClass (&wndclass) ;
    if(!hwnd) /* do not recreate on drag&drop */
    {
        hwnd = CreateWindow (szAppName,
                             szAppName,
                             /*WS_OVERLAPPED|*/WS_SYSMENU,
                             0, 0, 480,200,
                             NULL,
                             NULL,
                             winmain_instance,
                             NULL) ;
        SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
    }
    else
    {
        SetForegroundWindow(hwnd);
    }
    ShowWindow (hwnd, SW_SHOW) ;
    DragAcceptFiles(hwnd, TRUE);
    return 0;
}

void vsid_ui_display_name(const char *name)
{
    sprintf(vsidstrings[VSID_S_TITLE],  "   Title: %s", name);
    log_message(LOG_DEFAULT, "%s", vsidstrings[VSID_S_TITLE]);

}

void vsid_ui_display_author(const char *author)
{
    sprintf(vsidstrings[VSID_S_AUTHOR],  "  Author: %s", author);
    log_message(LOG_DEFAULT, "%s", vsidstrings[VSID_S_AUTHOR]);

}

void vsid_ui_display_copyright(const char *copyright)
{
    sprintf(vsidstrings[VSID_S_RELEASED],  "Released: %s", copyright);
    log_message(LOG_DEFAULT, "%s", vsidstrings[VSID_S_RELEASED]);

}

void vsid_ui_display_sync(int sync)
{
    sprintf(vsidstrings[VSID_S_SYNC], "Using %s sync",
                sync == MACHINE_SYNC_PAL ? "PAL" : "NTSC");
    log_message(LOG_DEFAULT, "%s",vsidstrings[VSID_S_SYNC]);
}

void vsid_ui_display_sid_model(int model)
{
    sprintf(vsidstrings[VSID_S_MODEL], "Using %s emulation",
                    csidmodel[ model>19 ? 7 : model ]);

    log_message(LOG_DEFAULT, "%s", vsidstrings[VSID_S_MODEL]);

}

void vsid_ui_display_tune_nr(int nr)
{
    sprintf(vsidstrings[VSID_S_PLAYING],"Playing Tune: %2d /  0  -  Default Tune: 00", nr);
    log_message(LOG_DEFAULT, "%s", vsidstrings[VSID_S_PLAYING]);
}

void vsid_ui_set_default_tune(int nr)
{
    char dummy[4];
    sprintf(dummy,"%2d", nr);

    log_message(LOG_DEFAULT, "Default Tune: %i", nr);
    vsidstrings[VSID_S_PLAYING][40]=dummy[0];
    vsidstrings[VSID_S_PLAYING][41]=dummy[1];
}


void vsid_ui_display_nr_of_tunes(int count)
{
    char dummy[4];
    sprintf(dummy,"%2d", count);

    log_message(LOG_DEFAULT, "Number of Tunes: %i", count);
    vsidstrings[VSID_S_PLAYING][19]=dummy[0];
    vsidstrings[VSID_S_PLAYING][20]=dummy[1];
}

void vsid_ui_display_time(unsigned int sec)
{
    char dummy[]="%02d:%02d:%02d";
    unsigned int h,m,s;
    s=sec;
    h=sec/3600;
    sec= sec-(h*3600);
    m=sec/60;
    sec= sec-(m*60);
    sprintf(vsidstrings[VSID_S_TIMER], dummy,h,m,sec);
    vsid_disp( 0, VSID_S_TIMER, "%s", vsidstrings[VSID_S_TIMER]);
    UpdateWindow (hwnd) ;
}

void vsid_ui_display_irqtype(const char *irq)
{
    sprintf(vsidstrings[VSID_S_IRQ],"Using %s interrupt", irq);
}

void vsid_ui_close(void)
{
    if(hwnd)
        while(DestroyWindow(hwnd)==0);
    hwnd=NULL;
}

void vsid_ui_setdrv(char* driver_info_text)
{
    strcpy(vsidstrings[VSID_S_LASTLINE], driver_info_text);
}

/* Window procedure.  All messages are handled here.  */
static long CALLBACK window_proc(HWND window, UINT msg, WPARAM wparam, LPARAM lparam)
{
    HDC hdc;
    PAINTSTRUCT ps;
    int i;
    switch (msg)
    {

    case WM_CREATE:
        songs = psid_tunes(&default_song);
        current_song = default_song;

        if(songs == 0)
        {
            log_message(LOG_DEFAULT, "Vsid: no file specified, quitting");
            return -1;
        }

        DragAcceptFiles(window, TRUE);
        return 0;

    case WM_KEYDOWN:
        switch(wparam)
        {
        case 'I': /* infoline on request, just press I */
            vsid_disp( 0, VSID_S_LASTLINE, "%s", vsidstrings[VSID_S_LASTLINE]);
            break;
        case VK_LEFT:
        case VK_DOWN:
            if(current_song > 1)
            {
                current_song--;
                psid_ui_set_tune((resource_value_t)current_song,NULL);
                vsid_ui_display_tune_nr(current_song);
                vsid_ui_set_default_tune(default_song);
                vsid_ui_display_nr_of_tunes(songs);
                InvalidateRect(window, NULL, 0);
            }

            break;
        case VK_RIGHT:
        case VK_UP:

            if(current_song < songs)
            {
                current_song++;
                psid_ui_set_tune((resource_value_t)current_song,NULL);
                vsid_ui_display_tune_nr(current_song);
                vsid_ui_set_default_tune(default_song);
                vsid_ui_display_nr_of_tunes(songs);
                InvalidateRect(window, NULL, 0);
            }
            break;
        }
        return 0;

    case WM_KEYUP:
        switch(wparam)
        {
        case 'I': /* infoline on request, just press I */
            vsid_disp( 0, VSID_S_LASTLINE, "%79s", " ");
            break;
        }
        return 0;

    case WM_SIZE:
        return 0;
    case WM_COMMAND:
        return 0;
    case WM_MOVE:
        break;
    case WM_CLOSE:
        vsid_ui_close();
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_DROPFILES:
        {
            char dummy[MAX_PATH];
            DragQueryFile( (HDROP)wparam, 0, dummy, sizeof(dummy) );
            if (machine_autodetect_psid(dummy)>=0)
            {
                vsid_disp( 0, 0,  NULL, NULL);
                psid_init_driver();
                psid_init_tune();

                vsid_ui_init();
                machine_play_psid(0);
                machine_trigger_reset(MACHINE_RESET_MODE_SOFT);
                songs = psid_tunes(&default_song);
                current_song = default_song;
                psid_ui_set_tune((resource_value_t)current_song,NULL);
                vsid_ui_display_tune_nr(current_song);
                vsid_ui_set_default_tune(default_song);
                vsid_ui_display_nr_of_tunes(songs);
                InvalidateRect(window, NULL, 0);
            }
        }
        return 0;


    case WM_PAINT:
        {
            hdc = BeginPaint(window, &ps);
            if(*vsidstrings[VSID_S_TIMER]) /* start only when timer string has been filled */
                for(i=0;i < VSID_S_LASTLINE;i++)
                    vsid_disp( 0, i, "%s", vsidstrings[i]);

            EndPaint(window, &ps);
            return 0;
        }
    }

    return DefWindowProc(window, msg, wparam, lparam);
}
