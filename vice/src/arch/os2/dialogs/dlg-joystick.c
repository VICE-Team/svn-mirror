/*
 * dlg-joystick.c - The joystick-dialog.
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

#include "vice.h"
#include "dialogs.h"

#include "joystick.h"
#include "resources.h"

#ifdef HAS_JOYSTICK

#define JOY_ALL (CB_JOY11|CB_JOY12|CB_JOY21|CB_JOY22)
#define JOY_PORT1 0x100
#define JOY_PORT2 0x200

extern int get_joyA_autoCal(void);
extern int get_joyB_autoCal(void);

static MRESULT EXPENTRY pm_joystick(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    static int first = TRUE;

    const int ID_ON  = 1;
    const int ID_OFF = 2;
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

static MRESULT EXPENTRY pm_calibrate(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
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

/* call to open dialog                                              */
/*----------------------------------------------------------------- */

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

