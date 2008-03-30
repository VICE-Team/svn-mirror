/*
 * dialogs.c - The dialogs.
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

#define INCL_WINBUTTONS
#define INCL_WINDIALOGS
#define INCL_WINSTDSPIN
#define INCL_WINSTDSLIDER
#define INCL_WINCIRCULARSLIDER

#include "vice.h"
#include "dialogs.h"

#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "types.h"
#include "utils.h"
#include "joystick.h"
#include "resources.h"

/* Is-this-dialog-open handling                                     */
/*----------------------------------------------------------------- */

#define DLGO_SOUND    0x1
#define DLGO_JOYSTICK 0x2
#define DLGO_DRIVE    0x4
#define DLGO_ABOUT    0x8

static int dlg_open = FALSE;

int dlgOpen(int dlg)
{
    return dlg_open & dlg;
}

void setDlgOpen(int dlg)
{
    dlg_open |= dlg;
}

void delDlgOpen(int dlg)
{
    dlg_open &= ~dlg;
}

/* Needed prototype functions                                       */
/*----------------------------------------------------------------- */

extern void set_volume(int vol);
extern int  get_volume(void);


/* Dialog procedures                                                */
/*----------------------------------------------------------------- */

MRESULT EXPENTRY pm_sound(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    static int first = TRUE;
    const int ID_ON  = 1;
    const int ID_OFF = 2;
    switch (msg)
    {
    case WM_INITDLG:
        setDlgOpen(DLGO_SOUND);
        break;
    case WM_PAINT:
        {
            if (first)
            {
                int sound;
                resources_get_value("Sound", (resource_value_t*) &sound);
                WinCheckButton   (hwnd, CB_SOUND, sound?1:0);
                WinSendDlgItemMsg(hwnd, CS_VOLUME, SLM_SETSLIDERINFO,
                                  MPFROM2SHORT(SMA_SLIDERARMPOSITION,SMA_RANGEVALUE),
                                  MPFROMSHORT(get_volume()));
                resources_get_value("SoundSampleRate", (resource_value_t*) &sound);
                if (sound==8000 || sound==11025 || sound==22050 || sound==44100)
                    WinCheckButton(hwnd, sound, 1);
                resources_get_value("SoundOversample", (resource_value_t *) &sound);
                if (sound >=0 && sound <= 3)
                    WinCheckButton(hwnd, sound|RB_OFF, 1);
                resources_get_value("SoundBufferSize", (resource_value_t *) &sound);
                WinSendDlgItemMsg(hwnd, SPB_BUFFER, SPBM_SETCURRENTVALUE,
                                  (MPARAM)sound,(MPARAM)0);
                resources_get_value("SidModel", (resource_value_t *) &sound);
                WinCheckButton(hwnd, sound?RB_8580:RB_6581 ,1);
                resources_get_value("SidFilters", (resource_value_t *) &sound);
                WinCheckButton(hwnd, CB_SIDFILTER, sound?1:0);
#ifdef HAVE_RESID
                resources_get_value("SidUseResid", (resource_value_t *) &sound);
                WinCheckButton(hwnd, CB_RESID, sound?1:0);
#endif
                first = FALSE;
            }
        }
        break;
    case WM_CLOSE:
        first = TRUE;
        delDlgOpen(DLGO_SOUND);
        break;
    case WM_COMMAND:
        if ((int)mp1==DID_CLOSE)
        {
            first = TRUE;
            delDlgOpen(DLGO_SOUND);
        }
        break;
    case WM_CONTROL:
        {
            switch (SHORT1FROMMP(mp1))
            {
            case CB_SOUND:
                {
                    int sound;
                    resources_get_value("Sound", (resource_value_t *) &sound);
                    resources_set_value("Sound", (resource_value_t) !sound);
                }
                break;
            case CS_VOLUME:
                {
                    if (SHORT2FROMMP(mp1)==SLN_SLIDERTRACK || SHORT2FROMMP(mp1)==SLN_CHANGE)
                        set_volume(LONGFROMMP(mp2));
                }
                break;
            case RB_8000HZ:
            case RB_11025HZ:
            case RB_22050HZ:
            case RB_44100HZ:
                {
                    int sound;
                    resources_get_value("SoundSampleRate", (resource_value_t *) &sound);
                    if (sound!=SHORT1FROMMP(mp1))
                        resources_set_value("SoundSampleRate", (resource_value_t) SHORT1FROMMP(mp1));
                }
                break;
            case RB_OFF:
            case RB_2X:
            case RB_4X:
            case RB_8X:
                {
                    int sound;
                    resources_get_value("SoundOversample", (resource_value_t *) &sound);
                    if (sound!=(SHORT1FROMMP(mp1)&0x3))
                        resources_set_value("SoundOversample", (resource_value_t)(SHORT1FROMMP(mp1)&0x3));
                }
                break;
            case RB_6581:
            case RB_8580:
                resources_set_value("SidModel", (resource_value_t)(SHORT1FROMMP(mp1)&0x1));
                break;
            case CB_SIDFILTER:
                {
                    int sound;
                    resources_get_value("SidFilters", (resource_value_t *) &sound);
                    resources_set_value("SidFilters", (resource_value_t) !sound);
                }
                break;
#ifdef HAVE_RESID
            case CB_RESID:
                {
                    int sound;
                    resources_get_value("SidUseResid", (resource_value_t *) &sound);
                    resources_set_value("SidUseResid", (resource_value_t) !sound);
                }
                break;
#endif
            case SPB_BUFFER:
                if (SHORT2FROMMP(mp1)==SPBN_ENDSPIN) {
                    ULONG sound;
                    WinSendDlgItemMsg(hwnd, SPB_BUFFER, SPBM_QUERYVALUE, &sound, 0);
                    resources_set_value("SoundBufferSize", (resource_value_t)sound);
                }
                break;
            }
            break;
        }
    }
    return WinDefDlgProc (hwnd, msg, mp1, mp2);
}

MRESULT EXPENTRY pm_joystick(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
/*    JOYDEV_NONE   =0x00,
    JOYDEV_NUMPAD =0x04,  4
    JOYDEV_KEYSET1=0x08,  8
    JOYDEV_KEYSET2=0x10, 16
    JOYDEV_HW1    =0x01,  1
    JOYDEV_HW2    =0x02   2*/

    static int first = TRUE;

    const int ID_ON  = 1;
    const int ID_OFF = 2;
#define JOY_ALL (CB_JOY11|CB_JOY12|CB_JOY21|CB_JOY22)
#define JOY_PORT1 0x100
#define JOY_PORT2 0x200
    switch (msg)
    {
    case WM_INITDLG:
        setDlgOpen(DLGO_JOYSTICK);
        break;
    case WM_PAINT:
        {
            if (first) {
                int joy1, joy2;
                resources_get_value("JoyDevice1", (resource_value_t*) &joy1);
                resources_get_value("JoyDevice2", (resource_value_t*) &joy2);
                WinCheckButton(hwnd,CB_JOY11,(JOYDEV_HW1 & joy1) ? 1 : 0);
                WinCheckButton(hwnd,CB_JOY12,(JOYDEV_HW1 & joy2) ? 1 : 0);
                WinCheckButton(hwnd,CB_JOY21,(JOYDEV_HW2 & joy1) ? 1 : 0);
                WinCheckButton(hwnd,CB_JOY22,(JOYDEV_HW2 & joy2) ? 1 : 0);
                first=FALSE;
            }
        }
        break;
    case WM_CLOSE:
        first = TRUE;
        delDlgOpen(DLGO_JOYSTICK);
        break;
    case WM_COMMAND:
        switch(LONGFROMMP(mp1))
        {
        case DID_CLOSE:
            first = TRUE;
            delDlgOpen(DLGO_JOYSTICK);
            break;
        case ID_SWAP:
            {
                int joy1, joy2, swp;

                resources_get_value("JoyDevice1", (resource_value_t*) &joy1);
                resources_get_value("JoyDevice2", (resource_value_t*) &joy2);

                swp  = (joy1 & ~(JOYDEV_HW1|JOYDEV_HW2)) | (joy2 & (JOYDEV_HW1|JOYDEV_HW2));
                joy2 = (joy2 & ~(JOYDEV_HW1|JOYDEV_HW2)) | (joy1 & (JOYDEV_HW1|JOYDEV_HW2));
                joy1 = swp;

                resources_set_value("JoyDevice1", (resource_value_t) joy1);
                resources_set_value("JoyDevice2", (resource_value_t) joy2);

                WinCheckButton(hwnd,CB_JOY11,(JOYDEV_HW1 & joy1) ? 1 : 0);
                WinCheckButton(hwnd,CB_JOY12,(JOYDEV_HW1 & joy2) ? 1 : 0);
                WinCheckButton(hwnd,CB_JOY21,(JOYDEV_HW2 & joy1) ? 1 : 0);
                WinCheckButton(hwnd,CB_JOY22,(JOYDEV_HW2 & joy2) ? 1 : 0);
                return FALSE;
            }
            break;
        }
        break;
    case WM_CONTROL:
        {
            int button = SHORT1FROMMP(mp1);
            int joy_hw = (button & JOYDEV_HW1)? JOYDEV_HW1:JOYDEV_HW2;
            if (button & JOY_ALL)
            {
                int state = SHORT2FROMMP(mp1);
                int joy;
                resources_get_value((button & JOY_PORT1)?"JoyDevice1":"JoyDevice2",
                                    (resource_value_t*) &joy);

                if (joy & joy_hw) joy &= ~joy_hw;
                else              joy |=  joy_hw;

                resources_set_value((button & JOY_PORT1)?"JoyDevice1":"JoyDevice2",
                                    (resource_value_t) joy);
            }
        }
        break;
    }
    return WinDefDlgProc (hwnd, msg, mp1, mp2);
}

MRESULT EXPENTRY pm_drive(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    static int first = TRUE;

    switch (msg)
    {
    case WM_INITDLG:
        setDlgOpen(DLGO_DRIVE);
        break;
    case WM_PAINT:
        {
            if (first) {
                int emu;
                resources_get_value("DriveTrueEmulation", (resource_value_t*) &emu);
                WinCheckButton(hwnd, CB_TRUEDRIVE, emu);
                first=FALSE;
            }
        }
        break;
    case WM_CLOSE:
        first = TRUE;
        delDlgOpen(DLGO_DRIVE);
        break;
    case WM_COMMAND:
        if (LONGFROMMP(mp1)==DID_CLOSE)
        {
            first = TRUE;
            delDlgOpen(DLGO_DRIVE);
        }
        break;
    case WM_CONTROL:
        {
            if (SHORT1FROMMP(mp1) == CB_TRUEDRIVE)
            {
                int emu;
                resources_get_value("DriveTrueEmulation", (resource_value_t*) &emu);
                resources_set_value("DriveTrueEmulation", (resource_value_t)  !emu);
            }
        }
        break;
    }
    return WinDefDlgProc (hwnd, msg, mp1, mp2);
}

/* call to open dialog                                              */
/*----------------------------------------------------------------- */

void sound_dialog(HWND hwnd)
{
    if (dlgOpen(DLGO_SOUND)) return;
    /*HWND hwndDlg =*/ WinLoadDlg(HWND_DESKTOP, hwnd, pm_sound, NULLHANDLE,
                              DLG_SOUND, NULL);
}

void joystick_dialog(HWND hwnd)
{
    if (dlgOpen(DLGO_JOYSTICK)) return;
    /*HWND 6hwndDlg =*/ WinLoadDlg(HWND_DESKTOP, hwnd, pm_joystick, NULLHANDLE,
                              DLG_JOYSTICK, NULL);
}

void drive_dialog(HWND hwnd)
{
    if (dlgOpen(DLGO_DRIVE)) return;
    /*HWND hwndDlg =*/ WinLoadDlg(HWND_DESKTOP, hwnd, pm_drive, NULLHANDLE,
                              DLG_DRIVE, NULL);
}

void about_dialog(HWND hwnd)
{
    /*HWND hwndDlg =*/ WinLoadDlg(HWND_DESKTOP, hwnd, WinDefDlgProc, NULLHANDLE,
                              DLG_ABOUT, NULL);
}

