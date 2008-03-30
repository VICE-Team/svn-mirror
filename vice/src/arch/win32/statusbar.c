/*
 * statusbar.c - Status bar code.
 *
 * Written by
 *  Tibor Biczo <crown@mtavnet.hu>
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
#include <windows.h>
#ifdef HAVE_COMMCTRL_H
#include <commctrl.h>
#endif
#include "datasette.h"
#include "res.h"
#include "ui.h"
#include "statusbar.h"


static HWND status_hwnd[2];
static int  number_of_status_windows=0;
static int  status_height;

static ui_drive_enable_t    status_enabled;
static int                  status_led[2];
static int                  status_map[2];          //  Translate from window index -> drive index
static int                  status_partindex[2];    //  Translate from drive index -> window index
static double               status_track[2];
static int                 *drive_active_led;

static int                  tape_enabled = 0;
static int                  tape_motor;
static int                  tape_counter;
static int                  tape_control;

static char                 emu_status_text[1024];

static HBRUSH   led_red;
static HBRUSH   led_green;
static HBRUSH   led_black;
static HBRUSH   tape_motor_on_brush;
static HBRUSH   tape_motor_off_brush;


static void SetStatusWindowParts(HWND hwnd)
{
int     number_of_parts;
int     enabled_drives;
RECT    rect;
int     posx[4];
int     width;
int     i;

    number_of_parts=0;
    enabled_drives=0;

    if (tape_enabled)
        number_of_parts++;

    if (status_enabled&UI_DRIVE_ENABLE_0) {
        status_map[enabled_drives++]=0;
        status_partindex[0]=number_of_parts++;
    }
    if (status_enabled&UI_DRIVE_ENABLE_1) {
        status_map[enabled_drives++]=1;
        status_partindex[1]=number_of_parts++;
    }
    GetWindowRect(hwnd,&rect);
    width=rect.right-rect.left;
    for (i=number_of_parts; i>=0; i--) {
        posx[i]=width;
        width-=110;
    }
    SendMessage(hwnd,SB_SETPARTS,number_of_parts+1,(LPARAM)posx);
    if (number_of_parts==3) {
        SendMessage(hwnd,SB_SETTEXT,3|SBT_OWNERDRAW,0);
    }
    if (number_of_parts==2) {
        SendMessage(hwnd,SB_SETTEXT,2|SBT_OWNERDRAW,0);
    }
    if (number_of_parts==1) {
        SendMessage(hwnd,SB_SETTEXT,1|SBT_OWNERDRAW,0);
    }
}


void statusbar_create(HWND hwnd)
{
RECT    rect;

//return;

    status_hwnd[number_of_status_windows]=CreateStatusWindow(WS_CHILD|WS_VISIBLE,"",hwnd,IDM_STATUS_WINDOW);
    GetClientRect(status_hwnd[number_of_status_windows],&rect);
    status_height=rect.bottom-rect.top;
    SetStatusWindowParts(status_hwnd[number_of_status_windows]);
    number_of_status_windows++;
}

void statusbar_destroy(void)
{
int i;

//return;

    for (i=0; i<number_of_status_windows; i++) {
        DestroyWindow(status_hwnd[i]);
    }
    status_height=0;
    number_of_status_windows=0;
}

void statusbar_create_brushes(void)
{
    led_green=CreateSolidBrush(0xff00);
    led_red=CreateSolidBrush(0xff);
    led_black=CreateSolidBrush(0x00);
    tape_motor_on_brush=CreateSolidBrush(0xffff);
    tape_motor_off_brush=CreateSolidBrush(0x808080);
}

int statusbar_get_status_height(void)
{
    return status_height;
}

void statusbar_setstatustext(const char *text)
{
int i;

    strcpy(emu_status_text,text);
    for (i=0; i<number_of_status_windows; i++) {
        SendMessage(status_hwnd[i],SB_SETTEXT,0|SBT_OWNERDRAW,0);
    }
}

void statusbar_enable_drive_status(ui_drive_enable_t enable, int *drive_led_color)
{
int i;

    status_enabled = enable;
    drive_active_led = drive_led_color;
    for (i=0; i<number_of_status_windows; i++) {
        SetStatusWindowParts(status_hwnd[i]);
    }
}

void statusbar_display_drive_track(int drivenum, int drive_base, double track_number)
{
int i;

    status_track[drivenum]=track_number;
    for (i=0; i<number_of_status_windows; i++) {
        SendMessage(status_hwnd[i],SB_SETTEXT,(status_partindex[drivenum]+1)|SBT_OWNERDRAW,0);
    }
}


void statusbar_display_drive_led(int drivenum, int status)
{
int i;

    status_led[drivenum]=status;
    for (i=0; i<number_of_status_windows; i++) {
        SendMessage(status_hwnd[i],SB_SETTEXT,(status_partindex[drivenum]+1)|SBT_OWNERDRAW,0);
    }
}

void statusbar_set_tape_status(int tape_status)
{
int i;

    tape_enabled = tape_status;
    for (i=0; i<number_of_status_windows; i++) {
        SetStatusWindowParts(status_hwnd[i]);
    }
}

void statusbar_display_tape_motor_status(int motor)
{   
int i;

    tape_motor = motor;
    for (i=0; i<number_of_status_windows; i++) {
        SendMessage(status_hwnd[i],SB_SETTEXT,1|SBT_OWNERDRAW,0);
    }
}

void statusbar_display_tape_control_status(int control)
{
int i;

    tape_control = control;
    for (i=0; i<number_of_status_windows; i++) {
        SendMessage(status_hwnd[i],SB_SETTEXT,1|SBT_OWNERDRAW,0);
    }
}

void statusbar_display_tape_counter(int counter)
{
int i;

    if (counter!=tape_counter) {
        tape_counter = counter;
        for (i=0; i<number_of_status_windows; i++) {
            SendMessage(status_hwnd[i],SB_SETTEXT,1|SBT_OWNERDRAW,0);
        }
    }
}

void statusbar_handle_WMSIZE(UINT msg, WPARAM wparam, LPARAM lparam, int window_index)
{
    SendMessage(status_hwnd[window_index],msg,wparam,lparam);
    SetStatusWindowParts(status_hwnd[window_index]);
}

void statusbar_handle_WMDRAWITEM(WPARAM wparam, LPARAM lparam)
{
RECT    led;
char    text[256];

    if (wparam==IDM_STATUS_WINDOW) {
        if (((DRAWITEMSTRUCT*)lparam)->itemID==0) {
            /* it's the status info */
            led=((DRAWITEMSTRUCT*)lparam)->rcItem;
            led.left += 2;
            led.right += 2;
            led.top += 2;
            led.bottom += 2;
            SetBkColor(((DRAWITEMSTRUCT*)lparam)->hDC,(COLORREF)GetSysColor(COLOR_3DFACE));
            SetTextColor(((DRAWITEMSTRUCT*)lparam)->hDC,(COLORREF)GetSysColor(COLOR_MENUTEXT));
            DrawText(((DRAWITEMSTRUCT*)lparam)->hDC,emu_status_text,-1,&led,0);
        }
        if ((((DRAWITEMSTRUCT*)lparam)->itemID==1) && tape_enabled) {
            /* it's the tape status */
            POINT tape_control_sign[3];

            /* the leading "Tape:" */
            led.top=((DRAWITEMSTRUCT*)lparam)->rcItem.top+2;
            led.bottom=((DRAWITEMSTRUCT*)lparam)->rcItem.top+18;
            led.left=((DRAWITEMSTRUCT*)lparam)->rcItem.left+2;
            led.right=((DRAWITEMSTRUCT*)lparam)->rcItem.left+34;
            SetBkColor(((DRAWITEMSTRUCT*)lparam)->hDC,(COLORREF)GetSysColor(COLOR_3DFACE));
            SetTextColor(((DRAWITEMSTRUCT*)lparam)->hDC,(COLORREF)GetSysColor(COLOR_MENUTEXT));
            DrawText(((DRAWITEMSTRUCT*)lparam)->hDC,"Tape:",-1,&led,0);

            /* the tape-motor */
            led.top=((DRAWITEMSTRUCT*)lparam)->rcItem.top+1;
            led.bottom=((DRAWITEMSTRUCT*)lparam)->rcItem.top+15;
            led.left=((DRAWITEMSTRUCT*)lparam)->rcItem.left+36;
            led.right=((DRAWITEMSTRUCT*)lparam)->rcItem.left+50;
            FillRect(((DRAWITEMSTRUCT*)lparam)->hDC,&led,tape_motor?tape_motor_on_brush:tape_motor_off_brush);

            /* the tape-control */
            led.top+=3;
            led.bottom-=3;
            led.left+=3;
            led.right-=3;
            tape_control_sign[0].x = led.left;
            tape_control_sign[1].x = led.left+4;
            tape_control_sign[2].x = led.left;
            tape_control_sign[0].y = led.top;
            tape_control_sign[1].y = led.top+4;
            tape_control_sign[2].y = led.top+8;
            switch (tape_control) {
                case DATASETTE_CONTROL_STOP:
                    FillRect(((DRAWITEMSTRUCT*)lparam)->hDC,&led,led_black);
                    break;
                case DATASETTE_CONTROL_START:
                case DATASETTE_CONTROL_RECORD:
                    SelectObject(((DRAWITEMSTRUCT*)lparam)->hDC,led_black);
                    Polygon(((DRAWITEMSTRUCT*)lparam)->hDC,tape_control_sign,3);
                    if (tape_control==DATASETTE_CONTROL_RECORD) {
                        SelectObject(((DRAWITEMSTRUCT*)lparam)->hDC,led_red);
                        Ellipse(((DRAWITEMSTRUCT*)lparam)->hDC,
                            led.left+17,
                            led.top+1,
                            led.left+24,
                            led.top+8);
                    }
                    break;
                case DATASETTE_CONTROL_REWIND:
                    tape_control_sign[0].x += 4;
                    tape_control_sign[1].x -= 4;
                    tape_control_sign[2].x += 4;
                case DATASETTE_CONTROL_FORWARD:
                    Polyline(((DRAWITEMSTRUCT*)lparam)->hDC,tape_control_sign,3);
                    tape_control_sign[0].x += 4;
                    tape_control_sign[1].x += 4;
                    tape_control_sign[2].x += 4;
                    Polyline(((DRAWITEMSTRUCT*)lparam)->hDC,tape_control_sign,3);
            }

            /* the tape-counter */
            led.top=((DRAWITEMSTRUCT*)lparam)->rcItem.top+2;
            led.bottom=((DRAWITEMSTRUCT*)lparam)->rcItem.top+18;
            led.left=((DRAWITEMSTRUCT*)lparam)->rcItem.left+75;
            led.right=((DRAWITEMSTRUCT*)lparam)->rcItem.left+110;
            sprintf(text,"%03i",tape_counter);
            DrawText(((DRAWITEMSTRUCT*)lparam)->hDC,text,-1,&led,0);

        }
        if ((int)((DRAWITEMSTRUCT*)lparam)->itemID>(tape_enabled?1:0)) {
            int index=((DRAWITEMSTRUCT*)lparam)->itemID-(tape_enabled?2:1);
            /* it's a disk */
            led.top=((DRAWITEMSTRUCT*)lparam)->rcItem.top+2;
            led.bottom=((DRAWITEMSTRUCT*)lparam)->rcItem.top+18;
            led.left=((DRAWITEMSTRUCT*)lparam)->rcItem.left+2;
            led.right=((DRAWITEMSTRUCT*)lparam)->rcItem.left+84;
            sprintf(text,"%d: Track: %.1f",status_map[index]+8,status_track[status_map[index]]);
            SetBkColor(((DRAWITEMSTRUCT*)lparam)->hDC,(COLORREF)GetSysColor(COLOR_3DFACE));
            SetTextColor(((DRAWITEMSTRUCT*)lparam)->hDC,(COLORREF)GetSysColor(COLOR_MENUTEXT));
            DrawText(((DRAWITEMSTRUCT*)lparam)->hDC,text,-1,&led,0);

            led.top=((DRAWITEMSTRUCT*)lparam)->rcItem.top+2;
            led.bottom=((DRAWITEMSTRUCT*)lparam)->rcItem.top+2+12;
            led.left=((DRAWITEMSTRUCT*)lparam)->rcItem.left+86;
            led.right=((DRAWITEMSTRUCT*)lparam)->rcItem.left+86+16;
            FillRect(((DRAWITEMSTRUCT*)lparam)->hDC,&led,status_led[status_map[index]] ? (drive_active_led[status_map[index]] ? led_green : led_red ) : led_black);
        }
    }
}

