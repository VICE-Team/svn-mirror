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

int psid_ui_set_tune(resource_value_t tune, void *param);

char szAppName[]="WinVice VSID GUI";
char vsidstrings[10][80]={0};

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
            TextOut(hDC, 3+(txout_x*size.cx),
                         3+(txout_y*size.cy),
                         dummy, strlen(dummy));
        }
        else
        {
            GetClientRect(hwnd, &r);
            FillRect(hDC, &r, (HBRUSH) (COLOR_WINDOW+1));
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
    wndclass.hbrBackground = GetStockObject( WHITE_BRUSH );
    wndclass.lpszMenuName  = szAppName ;
    wndclass.lpszClassName = szAppName ;

    RegisterClass (&wndclass) ;
    hwnd = CreateWindow (szAppName,
                         szAppName,
                         /*WS_OVERLAPPED|*/WS_SYSMENU,
                         0, 0, 500,300,
                         NULL,
                         NULL,
                         winmain_instance,
                         NULL) ;
    ShowWindow (hwnd, SW_SHOW) ;
    //UpdateWindow (hwnd) ; //ferkoff! took me some to find out. With UpdateWindow after ShowWindow does not paint, unless an alt-tab is pressed ???
    return 0;
}

void vsid_ui_display_name(const char *name)
{
    log_message(LOG_DEFAULT, "Name: %s", name);
    sprintf(vsidstrings[1], "Name: %s", name);
}

void vsid_ui_display_author(const char *author)
{
    log_message(LOG_DEFAULT, "Author: %s", author);
    sprintf(vsidstrings[2], "Author: %s", author);
}

void vsid_ui_display_copyright(const char *copyright)
{
    log_message(LOG_DEFAULT, "Copyright by: %s", copyright);
    sprintf(vsidstrings[3], "Copyright by: %s", copyright);
}

void vsid_ui_display_sync(int sync)
{
    log_message(LOG_DEFAULT, "Using %s sync",
                sync == MACHINE_SYNC_PAL ? "PAL" : "NTSC");
    sprintf(vsidstrings[4], "Using %s sync",
                sync == MACHINE_SYNC_PAL ? "PAL" : "NTSC");
}

void vsid_ui_display_sid_model(int model)
{
    log_message(LOG_DEFAULT, "Using %s emulation",
		model == 0 ? "MOS6581" : "MOS8580");
    sprintf(vsidstrings[5], "Using %s emulation",
        model == 0 ? "MOS6581" : "MOS8580");
}

void vsid_ui_display_tune_nr(int nr)
{
/*
Playing Tune: 00 / 00  -  Default Tune: 00
*/
    sprintf(vsidstrings[7],"Playing Tune: %2d /  0  -  Default Tune: 00", nr);
    log_message(LOG_DEFAULT, "Playing Tune: %i", nr);
}

void vsid_ui_set_default_tune(int nr)
{
    char dummy[4];
    sprintf(dummy,"%2d", nr);

    log_message(LOG_DEFAULT, "Default Tune: %i", nr);
    vsidstrings[7][40]=dummy[0];
    vsidstrings[7][41]=dummy[1];
}


void vsid_ui_display_nr_of_tunes(int count)
{
    char dummy[4];
    sprintf(dummy,"%2d", count);

    log_message(LOG_DEFAULT, "Number of Tunes: %i", count);
    vsidstrings[7][19]=dummy[0];
    vsidstrings[7][20]=dummy[1];
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
    sprintf(vsidstrings[8], dummy,h,m,sec);
    vsid_disp( 0, 8, "%s", vsidstrings[8]);
    UpdateWindow (hwnd) ;
}

void vsid_ui_display_irqtype(const char *irq)
{
    sprintf(vsidstrings[6],"Using %s interrupt", irq);
}

void vsid_ui_close(void)
{
    if(hwnd)
        while(DestroyWindow(hwnd)==0);
    hwnd=NULL;
}

void vsid_setdrv(char* dummy)
{
    strcpy(vsidstrings[0], dummy);
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
        return 0;

    case WM_PAINT:
        {
            hdc = BeginPaint(window, &ps);
            if(*vsidstrings[8])
                for(i=0;i<9;i++)
                    vsid_disp( 0, i, "%s", vsidstrings[i]);

            EndPaint(window, &ps);
            return 0;
        }
    }

    return DefWindowProc(window, msg, wparam, lparam);
}
