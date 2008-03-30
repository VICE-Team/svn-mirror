/*
 * dialogs.c - The dialogs.
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

#define INCL_WINBUTTONS
#define INCL_WINDIALOGS
#define INCL_WINSTDSPIN
#define INCL_WINSTDSLIDER
#define INCL_WINLISTBOXES
#define INCL_WINENTRYFIELDS
#define INCL_WINCIRCULARSLIDER

#include "vice.h"
#include "dialogs.h"

#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "types.h"
#include "utils.h"
#include "vsync.h"
#include "joystick.h"
#include "resources.h"
#include "datasette.h"

/* WinPM-Macros                                                     */
/*----------------------------------------------------------------- */

#define WinIsDlgEnabled(hwnd, id) \
    WinIsWindowEnabled(WinWindowFromID(hwnd, id))
#define WinSendDlgMsg(hwnd, id, msg, mp1, mp2) \
    WinSendMsg(WinWindowFromID(hwnd, id), msg, (MPARAM)mp1, (MPARAM)mp2)
#define WinSetDlgLboxItemText(hwnd, id, index, psz) \
    WinSendDlgMsg(hwnd, id, LM_SETITEMTEXT, MPFROMLONG(index), MPFROMP(psz))
#define WinSetSpinVal(hwnd, id, val) \
    WinSendDlgItemMsg(hwnd, id, SPBM_SETCURRENTVALUE, (MPARAM)val,(MPARAM)0)
#define WinGetSpinVal(hwnd, id, val) \
    WinSendDlgItemMsg(hwnd, id, SPBM_QUERYVALUE, (MPARAM)val, (MPARAM)0)
#define WinLboxSelectItem(hwnd, id, index) \
    WinSendDlgMsg(hwnd, id, LM_SELECTITEM, index, TRUE);
#define WinLboxDeselectItem(hwnd, id, index) \
    WinSendDlgMsg(hwnd, id, LM_SELECTITEM, index, FALSE);
#define WinLboxInsertItem(hwnd, id, psz) \
    WinInsertLboxItem(WinWindowFromID(hwnd, id), LIT_END, psz)
#define WinLboxQuerySelectedItem(hwnd, id) \
    WinQueryLboxSelectedItem (WinWindowFromID(hwnd, id))

/* Is-this-dialog-open handling                                     */
/*----------------------------------------------------------------- */

#define DLGO_SOUND      0x01
#define DLGO_JOYSTICK   0x02
#define DLGO_DRIVE      0x04
#define DLGO_ABOUT      0x08
#define DLGO_DATASETTE  0x10
#define DLGO_CALIBRATE  0x20
#define DLGO_EMULATOR   0x40

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
        first = TRUE;
        break;
    case WM_COMMAND:
        if (LONGFROMMP(mp1)!=DID_CLOSE) break;
    case WM_CLOSE:
        delDlgOpen(DLGO_SOUND);
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
                WinSetSpinVal(hwnd, SPB_BUFFER, sound);
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
                    WinGetSpinVal(hwnd, SPB_BUFFER, &sound);
                    resources_set_value("SoundBufferSize", (resource_value_t)sound);
                }
                break;
            }
            break;
        }
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
        first = TRUE;
        break;
    case WM_COMMAND:
        if (LONGFROMMP(mp1)!=DID_CLOSE) break;
    case WM_CLOSE:
        delDlgOpen(DLGO_DRIVE);
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

MRESULT EXPENTRY pm_datasette(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    switch (msg)
    {
    case WM_INITDLG:
        setDlgOpen(DLGO_DATASETTE);
        break;
    case WM_CLOSE:
        delDlgOpen(DLGO_DATASETTE);
        break;
    case WM_COMMAND:
        switch (LONGFROMMP(mp1))
        {
        case DID_CLOSE:
            delDlgOpen(DLGO_DATASETTE);
            break;
        case PB_STOP:
        case PB_START:
        case PB_FORWARD:
        case PB_REWIND:
        case PB_RECORD:
        case PB_RESET:
            datasette_control(LONGFROMMP(mp1)&0xf);
            return FALSE;
        }
    }
    return WinDefDlgProc (hwnd, msg, mp1, mp2);
}

#ifdef HAS_JOYSTICK
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
        first = TRUE;
        break;
    case WM_CLOSE:
        if (dlgOpen(DLGO_CALIBRATE))
            WinSendDlgItemMsg(hwnd, DLG_CALIBRATE, WM_CLOSE, 0, 0);
        delDlgOpen(DLGO_JOYSTICK);
        delDlgOpen(DLGO_CALIBRATE);
        break;
    case WM_PAINT:
        {
            if (first)
            {
                int joy1, joy2;
                resources_get_value("JoyDevice1", (resource_value_t*) &joy1);
                resources_get_value("JoyDevice2", (resource_value_t*) &joy2);
                WinCheckButton(hwnd, CB_JOY11,(JOYDEV_HW1 & joy1) ? 1 : 0);
                WinCheckButton(hwnd, CB_JOY12,(JOYDEV_HW1 & joy2) ? 1 : 0);
                WinCheckButton(hwnd, CB_JOY21,(JOYDEV_HW2 & joy1) ? 1 : 0);
                WinCheckButton(hwnd, CB_JOY22,(JOYDEV_HW2 & joy2) ? 1 : 0);
                first=FALSE;
            }
        }
        break;
    case WM_COMMAND:
        switch(LONGFROMMP(mp1))
        {
        case DID_CLOSE:
            delDlgOpen(DLGO_JOYSTICK);
            delDlgOpen(DLGO_CALIBRATE);
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

                WinCheckButton(hwnd, CB_JOY11,(JOYDEV_HW1 & joy1) ? 1 : 0);
                WinCheckButton(hwnd, CB_JOY12,(JOYDEV_HW1 & joy2) ? 1 : 0);
                WinCheckButton(hwnd, CB_JOY21,(JOYDEV_HW2 & joy1) ? 1 : 0);
                WinCheckButton(hwnd, CB_JOY22,(JOYDEV_HW2 & joy2) ? 1 : 0);
            }
            return FALSE;
        case ID_CALIBRATE:
            calibrate_dialog(hwnd);
            return FALSE;;
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
                if (dlgOpen(DLGO_CALIBRATE))
                {
                    WinSendDlgMsg(hwnd, DLG_CALIBRATE, WM_SETJOY,
                                  ((joy_hw&JOYDEV_HW1)?0:1), (joy&joy_hw));
                }
            }
        }
        break;
    }
    return WinDefDlgProc (hwnd, msg, mp1, mp2);
}

int get_joyA_autoCal(void);
int get_joyB_autoCal(void);

MRESULT EXPENTRY pm_calibrate(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    static int first = TRUE;
    static int joy1  = TRUE;

    switch (msg)
    {
    case WM_INITDLG:
        setDlgOpen(DLGO_CALIBRATE);
        first = TRUE;
        break;
    case WM_CLOSE:
        delDlgOpen(DLGO_CALIBRATE);
        break;
    case WM_PAINT:
        {
            if (first)
            {
                int j1, j2;
                resources_get_value("JoyDevice1", (resource_value_t*) &j1);
                resources_get_value("JoyDevice2", (resource_value_t*) &j2);
                WinSendMsg(hwnd, WM_PROCESS,
                           (void*)!!((j1&JOYDEV_HW1)|(j2&JOYDEV_HW1)),
                           (void*)!!((j1&JOYDEV_HW2)|(j2&JOYDEV_HW2)));
                first = FALSE;
            }
        }
        break;
    case WM_CONTROL:
        {
            int ctrl = SHORT1FROMMP(mp1);
            switch (ctrl)
            {
            case RB_JOY1:
            case RB_JOY2:
                if (!(ctrl==RB_JOY1 && joy1) && !(ctrl==RB_JOY2 && !joy1))
                {
                    joy1 = !joy1;
                    WinSendMsg(hwnd, WM_ENABLECTRL, 0,
                               (void*)(joy1?get_joyA_autoCal():get_joyB_autoCal()));
                    WinSendMsg(hwnd, WM_FILLSPB, 0, 0);
                }
                break;
            case SPB_UP:
            case SPB_DOWN:
            case SPB_LEFT:
            case SPB_RIGHT:
                if (SHORT2FROMMP(mp1)==SPBN_ENDSPIN)
                {
                    ULONG val;
                    WinGetSpinVal(hwnd, ctrl, &val);
                    resources_set_value(ctrl==SPB_UP  ? (joy1?"joyAup"  :"joyBup")  :
                                        ctrl==SPB_DOWN? (joy1?"joyAdown":"joyBdown"):
                                        ctrl==SPB_LEFT? (joy1?"joyAleft":"joyBleft"):
                                        (joy1?"joyAright":"joyBright"), (resource_value_t)val);
                }
                break;
            }
            break;
        }
    case WM_COMMAND:
        {
            int cmd = LONGFROMMP(mp1);
            switch (cmd)
            {
            case DID_CLOSE:
                delDlgOpen(DLGO_CALIBRATE);
                break;
            case ID_START:
            case ID_RESET:
            case ID_STOP:
                {
                    if (joy1) set_joyA_autoCal(NULL,   (void*)(cmd!=ID_STOP));
                    else      set_joyB_autoCal(NULL,   (void*)(cmd!=ID_STOP));
                    WinSendMsg(hwnd, WM_ENABLECTRL, 0, (void*)(cmd!=ID_STOP));
                    WinSendMsg(hwnd, WM_FILLSPB, 0, 0);
                    return FALSE;
                }
            }
        }
    case WM_PROCESS:
        if (((int)mp1^(int)mp2)&1) joy1 = (int)mp1;
        WinCheckButton(hwnd, joy1?RB_JOY1:RB_JOY2, 1);
        WinEnableControl(hwnd, RB_JOY1, (ULONG)mp1);
        WinEnableControl(hwnd, RB_JOY2, (ULONG)mp2);

        WinSendMsg(hwnd, WM_ENABLECTRL, (void*)(!mp1 && !mp2),
                   (void*)(joy1?get_joyA_autoCal():get_joyB_autoCal()));
        WinSendMsg(hwnd, WM_FILLSPB, 0, 0);
        break;
    case WM_SETJOY:
        {
            WinEnableControl(hwnd, mp1?RB_JOY2:RB_JOY1, (ULONG)mp2);
            WinSendMsg(hwnd, WM_PROCESS,
                       (void*)WinIsDlgEnabled(hwnd, RB_JOY1),
                       (void*)WinIsDlgEnabled(hwnd, RB_JOY2));
        }
        break;
    case WM_ENABLECTRL:
        WinEnableControl(hwnd, ID_START,  mp1?FALSE: !mp2);
        WinEnableControl(hwnd, ID_STOP,   mp1?FALSE:!!mp2);
        WinEnableControl(hwnd, ID_RESET,  mp1?FALSE:!!mp2);
        WinEnableControl(hwnd, SPB_UP,    mp1?FALSE: !mp2);
        WinEnableControl(hwnd, SPB_DOWN,  mp1?FALSE: !mp2);
        WinEnableControl(hwnd, SPB_LEFT,  mp1?FALSE: !mp2);
        WinEnableControl(hwnd, SPB_RIGHT, mp1?FALSE: !mp2);
        return FALSE;
    case WM_FILLSPB:
        {
            int val;
            resources_get_value(joy1?"JoyAup":"JoyBup", (resource_value_t *) &val);
            WinSetSpinVal(hwnd, SPB_UP, val);
            resources_get_value(joy1?"JoyAdown":"JoyBdown", (resource_value_t *) &val);
            WinSetSpinVal(hwnd, SPB_DOWN, val);
            resources_get_value(joy1?"JoyAleft":"JoyBleft", (resource_value_t *) &val);
            WinSetSpinVal(hwnd, SPB_LEFT, val);
            resources_get_value(joy1?"JoyAright":"JoyBright", (resource_value_t *) &val);
            WinSetSpinVal(hwnd, SPB_RIGHT,val);
        }
        return FALSE;
    }
    return WinDefDlgProc (hwnd, msg, mp1, mp2);
}
#endif

MRESULT EXPENTRY pm_emulator(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    static int first = TRUE;
    const char psz[11][6] =
    {
        " Auto", " 1/1", " 1/2", " 1/3", " 1/4", " 1/5",
        " 1/6",  " 1/7", " 1/8", " 1/9", " 1/10"
    };

    switch (msg)
    {
    case WM_INITDLG:
        setDlgOpen(DLGO_EMULATOR);
        first = TRUE;
        break;
    case WM_COMMAND:
        if (LONGFROMMP(mp1)!=DID_CLOSE) break;
    case WM_CLOSE:
        delDlgOpen(DLGO_EMULATOR);
        break;
    case WM_PAINT:
        {
            if (first)
            {
                int val;
                for (val=0; val<11; val++)
                    WinLboxInsertItem(hwnd, CBS_REFRATE, psz[val]);
                resources_get_value("Speed", (resource_value_t *) &val);
                WinSetSpinVal(hwnd, SPB_SPEED, val);
                resources_get_value("RefreshRate", (resource_value_t *) &val);
                WinLboxSelectItem(hwnd, CBS_REFRATE, val);
                first=FALSE;
            }
        }
        break;
    case WM_CONTROL:
        {
            int ctrl = SHORT1FROMMP(mp1);
            switch (ctrl)
            {
            case SPB_SPEED:
                if (SHORT2FROMMP(mp1)==SPBN_ENDSPIN)
                {
                    ULONG val;
                    WinGetSpinVal(hwnd, ctrl, &val);
                    suspend_speed_eval();
                    resources_set_value("Speed", (resource_value_t)val);
                }
                break;
            case CBS_REFRATE:
                if (SHORT2FROMMP(mp1)==LN_SELECT)
                {
                    resources_set_value("RefreshRate",
                                        (resource_value_t)WinLboxQuerySelectedItem (hwnd, CBS_REFRATE));
                }
                break;
            }
            break;
        }
    }
    return WinDefDlgProc (hwnd, msg, mp1, mp2);
}


/* call to open dialog                                              */
/*----------------------------------------------------------------- */

void sound_dialog(HWND hwnd)
{
    if (dlgOpen(DLGO_SOUND)) return;
    WinLoadDlg(HWND_DESKTOP, hwnd, pm_sound, NULLHANDLE,
               DLG_SOUND, NULL);
}

#ifdef HAS_JOYSTICK
void joystick_dialog(HWND hwnd)
{
    if (dlgOpen(DLGO_JOYSTICK)) return;
    WinLoadDlg(HWND_DESKTOP, hwnd, pm_joystick, NULLHANDLE,
               DLG_JOYSTICK, NULL);
}

void calibrate_dialog(HWND hwnd)
{
    if (dlgOpen(DLGO_CALIBRATE)) return;
    WinLoadDlg(HWND_DESKTOP, hwnd, pm_calibrate, NULLHANDLE,
               DLG_CALIBRATE, NULL);
}
#endif

void drive_dialog(HWND hwnd)
{
    if (dlgOpen(DLGO_DRIVE)) return;
    WinLoadDlg(HWND_DESKTOP, hwnd, pm_drive, NULLHANDLE,
               DLG_DRIVE, NULL);
}

void about_dialog(HWND hwnd)
{
    WinLoadDlg(HWND_DESKTOP, hwnd, WinDefDlgProc, NULLHANDLE,
               DLG_ABOUT, NULL);
}

void datasette_dialog(HWND hwnd)
{
    if (dlgOpen(DLGO_DATASETTE)) return;
    WinLoadDlg(HWND_DESKTOP, hwnd, pm_datasette, NULLHANDLE,
               DLG_DATASETTE, NULL);
}

void emulator_dialog(HWND hwnd)
{
    if (dlgOpen(DLGO_EMULATOR)) return;
    WinLoadDlg(HWND_DESKTOP, hwnd, pm_emulator, NULLHANDLE,
               DLG_EMULATOR, NULL);
}

