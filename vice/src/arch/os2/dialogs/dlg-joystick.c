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

#define INCL_WININPUT     // WM_CHAR
#define INCL_WINBUTTONS
#define INCL_WINDIALOGS
#define INCL_WINSTDSPIN
#define INCL_WINFRAMEMGR  // WM_TANSLATEACCEL
#include "vice.h"

#include <os2.h>

#include "dialogs.h"
#include "dlg-joystick.h"

#include <ctype.h>        // isprint
#include <stdlib.h>       // free

#include "lib.h"
#include "log.h"
#include "joy.h"
#include "resources.h"
#include "snippets\pmwin2.h"
#include "machine.h"
#include "userport_joystick.h"

#ifdef HAS_JOYSTICK

#define JOY_ALL (CB_JOY11 | CB_JOY12 | CB_JOY21 | CB_JOY22)
#define JOY_PORT1 0x100
#define JOY_PORT2 0x200

#define JOYDEV_ALL (JOYDEV_HW1 | JOYDEV_HW2 | JOYDEV_NUMPAD | JOYDEV_KEYSET1 | JOYDEV_KEYSET2)

extern int number_joysticks;

static HWND hwndCalibrate = NULLHANDLE;
static HWND hwndKeyset = NULLHANDLE;

static MRESULT EXPENTRY pm_joystick(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    switch (msg) {
        case WM_INITDLG:
            {
                int joy1, joy2;
                //
                // disable controls of non existing joysticks
                // remark: I think this cannot change while runtime
                //
                if (!(number_joysticks & JOYDEV_HW1)) {
                    WinEnableControl(hwnd, CB_JOY11, 0);
                    WinEnableControl(hwnd, CB_JOY12, 0);
                }
                if (!(number_joysticks&JOYDEV_HW2)) {
                    WinEnableControl(hwnd, CB_JOY21, 0);
                    WinEnableControl(hwnd, CB_JOY22, 0);
                }
                if (number_joysticks == 0) {
                    WinEnableControl(hwnd, ID_CALIBRATE, 0);
                }

                if (machine_class == VICE_MACHINE_VIC20) {
                    WinEnableControl(hwnd, CB_JOY12, 0);
                    WinEnableControl(hwnd, CB_JOY22, 0);
                    WinEnableControl(hwnd, CB_NUMJOY2, 0);
                    WinEnableControl(hwnd, CB_KS1JOY2, 0);
                    WinEnableControl(hwnd, CB_KS2JOY2, 0);
                    WinEnableControl(hwnd, ID_SWAP, 0);
                }

                resources_get_int("JoyDevice1", &joy1);
                resources_get_int("JoyDevice2", &joy2);
                WinSendMsg(hwnd, WM_SETCBS, (void*)joy1, (void*)joy2);
            }
            break;
        case WM_DESTROY:
        case WM_CLOSE:
            {
                if (WinIsWindowVisible(hwndCalibrate)) {
                    WinSendMsg(hwndCalibrate, WM_CLOSE, 0, 0);
                }

                if (WinIsWindowVisible(hwndKeyset)) {
                    WinSendMsg(hwndKeyset, WM_CLOSE, 0, 0);
                }
            }
            break;
        case WM_COMMAND:
            switch(LONGFROMMP(mp1)) {
                case DID_CLOSE:
                    {
                        if (WinIsWindowVisible(hwndCalibrate)) {
                            WinSendMsg(hwndCalibrate, WM_CLOSE, 0, 0);
                        }

                        if (WinIsWindowVisible(hwndKeyset)) {
                            WinSendMsg(hwndKeyset, WM_CLOSE, 0, 0);
                        }
                    }
                    break;
                case ID_SWAP:
                    {
                        int joy1, joy2;

                        resources_get_int("JoyDevice1", &joy1);
                        resources_get_int("JoyDevice2", &joy2);

                        resources_set_int("JoyDevice1", joy2);
                        resources_set_int("JoyDevice2", joy1);

                        WinSendMsg(hwnd, WM_SETCBS,  (void*)joy2, (void*)joy1);
                    }
                    return FALSE;
                case ID_CALIBRATE:
                    calibrate_dialog(hwnd);
                    return FALSE;
                case ID_KEYSET:
                    keyset_dialog(hwnd);
                    return FALSE;;
            }
            break;
        case WM_CONTROL:
            {
                int button =SHORT1FROMMP(mp1);
                int state = WinQueryButtonCheckstate(hwnd, button);
                int port = button & JOY_PORT1;
                int joya, joyb;

                resources_get_int(port ? "JoyDevice1" : "JoyDevice2", &joya);
                resources_get_int(port ? "JoyDevice2" : "JoyDevice1", &joyb);
                if (state) {
                    joya |= button & JOYDEV_ALL;
                } else {
                    joya &= ~(button & JOYDEV_ALL);
                }

                resources_set_int(port ? "JoyDevice1" : "JoyDevice2", joya);
                WinSendMsg(hwnd, WM_SETDLGS, (void*)(port ? joya : joyb), (void*)(port ? joyb : joya));

             }
             break;
        case WM_SETDLGS:
            {
                int joy1 = (int)mp1;
                int joy2 = (int)mp2;
                int joys = joy1 | joy2;

                if (WinIsWindowVisible(hwndCalibrate)) {
                    WinSendMsg(hwndCalibrate, WM_SETJOY, (MPARAM)(joys & JOYDEV_HW1), (MPARAM)(joys & JOYDEV_HW2));
                }

                if (WinIsWindowVisible(hwndKeyset)) {
                    WinSendMsg(hwndKeyset, WM_SETKEY, (MPARAM)(joys&JOYDEV_KEYSET1), (MPARAM)(joys&JOYDEV_KEYSET2));
                }
            }
            break;
        case WM_SETCBS:
            WinCheckButton(hwnd, CB_JOY11, (JOYDEV_HW1 & (int)mp1) ? 1 : 0);
            WinCheckButton(hwnd, CB_JOY12, (JOYDEV_HW1 & (int)mp2) ? 1 : 0);
            WinCheckButton(hwnd, CB_JOY21, (JOYDEV_HW2 & (int)mp1) ? 1 : 0);
            WinCheckButton(hwnd, CB_JOY22, (JOYDEV_HW2 & (int)mp2) ? 1 : 0);
            WinCheckButton(hwnd, CB_NUMJOY1, (JOYDEV_NUMPAD & (int)mp1) ? 1 : 0);
            WinCheckButton(hwnd, CB_NUMJOY2, (JOYDEV_NUMPAD & (int)mp2) ? 1 : 0);
            WinCheckButton(hwnd, CB_KS1JOY1, (JOYDEV_KEYSET1 & (int)mp1) ? 1 : 0);
            WinCheckButton(hwnd, CB_KS1JOY2, (JOYDEV_KEYSET1 & (int)mp2) ? 1 : 0);
            WinCheckButton(hwnd, CB_KS2JOY1, (JOYDEV_KEYSET2 & (int)mp1) ? 1 : 0);
            WinCheckButton(hwnd, CB_KS2JOY2, (JOYDEV_KEYSET2 & (int)mp2) ? 1 : 0);
            break;
    }
    return WinDefDlgProc (hwnd, msg, mp1, mp2);
}

static MRESULT EXPENTRY pm_extra_joystick(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    switch (msg) {
        case WM_INITDLG:
            {
                int joy1, joy2;
                int joy_enable, joy_type;
                //
                // disable controls of non existing joysticks
                // remark: I think this cannot change while runtime
                //
                if (!(number_joysticks & JOYDEV_HW1)) {
                    WinEnableControl(hwnd, CB_JOY11, 0);
                    WinEnableControl(hwnd, CB_JOY12, 0);
                }
                if (!(number_joysticks&JOYDEV_HW2)) {
                    WinEnableControl(hwnd, CB_JOY21, 0);
                    WinEnableControl(hwnd, CB_JOY22, 0);
                }
                if (number_joysticks == 0) {
                    WinEnableControl(hwnd, ID_CALIBRATE, 0);
                }

                if (machine_class != VICE_MACHINE_CBM5x0 && machine_class != VICE_MACHINE_PLUS4) {
                    resources_get_int("UserportJoy", &joy_enable);
                    resources_get_int("UserportJoyType", &joy_type);
                    if (joy_enable) {
                        if (joy_type == USERPORT_JOYSTICK_HUMMER || joy_type == USERPORT_JOYSTICK_OEM) {
                            WinEnableControl(hwnd, CB_JOY12, 0);
                            WinEnableControl(hwnd, CB_JOY22, 0);
                            WinEnableControl(hwnd, CB_NUMJOY2, 0);
                            WinEnableControl(hwnd, CB_KS1JOY2, 0);
                            WinEnableControl(hwnd, CB_KS2JOY2, 0);
                            WinEnableControl(hwnd, ID_SWAP, 0);
                        }
                    }
                }

                resources_get_int("JoyDevice3", &joy1);
                resources_get_int("JoyDevice4", &joy2);
                WinSendMsg(hwnd, WM_SETCBS, (void*)joy1, (void*)joy2);
            }
            break;
        case WM_DESTROY:
        case WM_CLOSE:
            {
                if (WinIsWindowVisible(hwndCalibrate)) {
                    WinSendMsg(hwndCalibrate, WM_CLOSE, 0, 0);
                }

                if (WinIsWindowVisible(hwndKeyset)) {
                    WinSendMsg(hwndKeyset, WM_CLOSE, 0, 0);
                }
            }
            break;
        case WM_COMMAND:
            switch(LONGFROMMP(mp1)) {
                case DID_CLOSE:
                    {
                        if (WinIsWindowVisible(hwndCalibrate)) {
                            WinSendMsg(hwndCalibrate, WM_CLOSE, 0, 0);
                        }

                        if (WinIsWindowVisible(hwndKeyset)) {
                            WinSendMsg(hwndKeyset, WM_CLOSE, 0, 0);
                        }
                    }
                    break;
                case ID_SWAP:
                    {
                        int joy1, joy2;

                        resources_get_int("JoyDevice3", &joy1);
                        resources_get_int("JoyDevice4", &joy2);

                        resources_set_int("JoyDevice3", joy2);
                        resources_set_int("JoyDevice4", joy1);

                        WinSendMsg(hwnd, WM_SETCBS,  (void*)joy2, (void*)joy1);
                    }
                    return FALSE;
                case ID_CALIBRATE:
                    calibrate_dialog(hwnd);
                    return FALSE;
                case ID_KEYSET:
                    keyset_dialog(hwnd);
                    return FALSE;;
            }
            break;
        case WM_CONTROL:
            {
                int button =SHORT1FROMMP(mp1);
                int state = WinQueryButtonCheckstate(hwnd, button);
                int port = button & JOY_PORT1;
                int joya, joyb;

                resources_get_int(port ? "JoyDevice3" : "JoyDevice4", &joya);
                resources_get_int(port ? "JoyDevice4" : "JoyDevice3", &joyb);
                if (state) {
                    joya |= button & JOYDEV_ALL;
                } else {
                    joya &= ~(button & JOYDEV_ALL);
                }

                resources_set_int(port ? "JoyDevice3" : "JoyDevice4", joya);
                WinSendMsg(hwnd, WM_SETDLGS, (void*)(port ? joya : joyb), (void*)(port ? joyb : joya));

             }
             break;
        case WM_SETDLGS:
            {
                int joy1 = (int)mp1;
                int joy2 = (int)mp2;
                int joys = joy1 | joy2;

                if (WinIsWindowVisible(hwndCalibrate)) {
                    WinSendMsg(hwndCalibrate, WM_SETJOY, (MPARAM)(joys & JOYDEV_HW1), (MPARAM)(joys & JOYDEV_HW2));
                }

                if (WinIsWindowVisible(hwndKeyset)) {
                    WinSendMsg(hwndKeyset, WM_SETKEY, (MPARAM)(joys&JOYDEV_KEYSET1), (MPARAM)(joys&JOYDEV_KEYSET2));
                }
            }
            break;
        case WM_SETCBS:
            WinCheckButton(hwnd, CB_JOY11, (JOYDEV_HW1 & (int)mp1) ? 1 : 0);
            WinCheckButton(hwnd, CB_JOY12, (JOYDEV_HW1 & (int)mp2) ? 1 : 0);
            WinCheckButton(hwnd, CB_JOY21, (JOYDEV_HW2 & (int)mp1) ? 1 : 0);
            WinCheckButton(hwnd, CB_JOY22, (JOYDEV_HW2 & (int)mp2) ? 1 : 0);
            WinCheckButton(hwnd, CB_NUMJOY1, (JOYDEV_NUMPAD & (int)mp1) ? 1 : 0);
            WinCheckButton(hwnd, CB_NUMJOY2, (JOYDEV_NUMPAD & (int)mp2) ? 1 : 0);
            WinCheckButton(hwnd, CB_KS1JOY1, (JOYDEV_KEYSET1 & (int)mp1) ? 1 : 0);
            WinCheckButton(hwnd, CB_KS1JOY2, (JOYDEV_KEYSET1 & (int)mp2) ? 1 : 0);
            WinCheckButton(hwnd, CB_KS2JOY1, (JOYDEV_KEYSET2 & (int)mp1) ? 1 : 0);
            WinCheckButton(hwnd, CB_KS2JOY2, (JOYDEV_KEYSET2 & (int)mp2) ? 1 : 0);
            break;
    }
    return WinDefDlgProc (hwnd, msg, mp1, mp2);
}

static MRESULT EXPENTRY pm_calibrate(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    static int joy1 = TRUE;

    switch (msg) {
        case WM_INITDLG:
            {
                int j1, j2;

                resources_get_int("JoyDevice1", &j1);
                resources_get_int("JoyDevice2", &j2);
                WinSendMsg(hwnd, WM_PROCESS, (void*)!!((j1 | j2) & JOYDEV_HW1), (void*)!!((j1 | j2) & JOYDEV_HW2));
            }
            break;
        case WM_COMMAND:
            {
                int cmd = LONGFROMMP(mp1);

                switch (cmd) {
                    case ID_START:
                    case ID_RESET:
                    case ID_STOP:
                        if (joy1) {
                            set_joyA_autoCal(NULL, (void*)(cmd != ID_STOP));
                        } else {
                            set_joyB_autoCal(NULL, (void*)(cmd != ID_STOP));
                        }
                        WinSendMsg(hwnd, WM_ENABLECTRL, 0, (void*)(cmd != ID_STOP));
                        WinSendMsg(hwnd, WM_FILLSPB, 0, 0);
                        return FALSE;
                }
                break;
            }
        case WM_CONTROL:
            {
                int ctrl = SHORT1FROMMP(mp1);

                switch (ctrl) {
                    case RB_JOY1:
                    case RB_JOY2:
                        if (!(ctrl == RB_JOY1 && joy1) && !(ctrl == RB_JOY2 && !joy1)) {
                            joy1 = !joy1;
                            WinSendMsg(hwnd, WM_ENABLECTRL, 0, (void*)(get_joy_autoCal(joy1 ? 0 : 1)));
                            WinSendMsg(hwnd, WM_FILLSPB, 0, 0);
                        }
                        break;
                    case SPB_UP:
                    case SPB_DOWN:
                    case SPB_LEFT:
                    case SPB_RIGHT:
                        if (SHORT2FROMMP(mp1) == SPBN_ENDSPIN) {
                            const ULONG val = WinGetSpinVal((HWND)mp2);

                            resources_set_int(ctrl == SPB_UP ? (joy1 ? "JoyAup" : "JoyBup") : ctrl == SPB_DOWN ? (joy1 ? "JoyAdown" : "JoyBdown") :
                                              ctrl == SPB_LEFT ? (joy1 ? "JoyAleft" : "JoyBleft") : (joy1 ? "JoyAright" : "JoyBright"), val);
                        }
                        break;
                }
                break;
            }
        case WM_PROCESS:
            if (((int)mp1 ^ (int)mp2) & 1) {
                joy1 = (int)mp1;
            }

            WinCheckButton(hwnd, joy1?RB_JOY1:RB_JOY2, 1);
            WinEnableControl(hwnd, RB_JOY1, (ULONG)mp1);
            WinEnableControl(hwnd, RB_JOY2, (ULONG)mp2);

            WinSendMsg(hwnd, WM_ENABLECTRL, (void*)(!mp1 && !mp2), (void*)get_joy_autoCal(joy1 ? 0 : 1));
            WinSendMsg(hwnd, WM_FILLSPB, 0, 0);
            break;
        case WM_SETJOY:
            {
                ULONG state1 = mp1 ? 1 : 0;
                ULONG state2 = mp2 ? 1 : 0;
                WinEnableControl(hwnd, RB_JOY1, state1);
                WinEnableControl(hwnd, RB_JOY2, state2);
                WinSendMsg(hwnd, WM_PROCESS, (void*)state1, (void*)state2);
            }
            break;
        case WM_ENABLECTRL:
            WinEnableControl(hwnd, ID_START, mp1 ? FALSE : !mp2);
            WinEnableControl(hwnd, ID_STOP, mp1 ? FALSE : !!mp2);
            WinEnableControl(hwnd, ID_RESET, mp1 ? FALSE : !!mp2);
            WinEnableControl(hwnd, SPB_UP, mp1 ? FALSE : !mp2);
            WinEnableControl(hwnd, SPB_DOWN, mp1 ? FALSE : !mp2);
            WinEnableControl(hwnd, SPB_LEFT, mp1 ? FALSE : !mp2);
            WinEnableControl(hwnd, SPB_RIGHT, mp1 ? FALSE : !mp2);
            return FALSE;
        case WM_FILLSPB:
            {
                int val;

                resources_get_int(joy1 ? "JoyAup" : "JoyBup", &val);
                WinSetDlgSpinVal(hwnd, SPB_UP, val);
                resources_get_int(joy1 ? "JoyAdown" : "JoyBdown", &val);
                WinSetDlgSpinVal(hwnd, SPB_DOWN, val);
                resources_get_int(joy1 ? "JoyAleft" : "JoyBleft", &val);
                WinSetDlgSpinVal(hwnd, SPB_LEFT, val);
                resources_get_int(joy1 ? "JoyAright" : "JoyBright", &val);
                WinSetDlgSpinVal(hwnd, SPB_RIGHT, val);
            }
            return FALSE;
    }
    return WinDefDlgProc (hwnd, msg, mp1, mp2);
}

static const char *GetDirection(USHORT id)
{
    switch (id) {
        case SPB_N:
            return "North";
        case SPB_NE:
            return "NorthEast";
        case SPB_E:
            return "East";
        case SPB_SE:
            return "SouthEast";
        case SPB_S:
            return "South";
        case SPB_SW:
            return "SouthWest";
        case SPB_W:
            return "West";
        case SPB_NW:
            return "NorthWest";
        case SPB_FIRE:
            return "Fire";
    }
    return "";
}

static int ResGetKeyVal(int num, USHORT id)
{
    int val;
    char *res = lib_msprintf("KeySet%d%s", num ? 1 : 2, GetDirection(id));

    resources_get_int(res, &val);
    lib_free (res);

    return val;
}

static void ResSetKeyVal(int num, USHORT id, int val)
{
    char *res = lib_msprintf("KeySet%d%s", num ? 1 : 2, GetDirection(id));

    resources_set_int(res, val);
    lib_free (res);
}

static void UpdateKeyVal(HWND hwnd, USHORT id, int num)
{
    int   val;
    char *msg;

    val = ResGetKeyVal(num, id);

    msg = lib_msprintf("%03d", val);
    WinSendDlgMsg(hwnd, id, SPBM_SETARRAY, &msg, 1);
    WinSetDlgSpinVal(hwnd, id, 0);
    lib_free(msg);
}

static void SetKeyVal(HWND hwnd, USHORT id, int num, int val)
{
    char *msg;

    ResSetKeyVal(num, id, val);

    msg = lib_msprintf("%03d", val);
    WinSendDlgMsg(hwnd, id, SPBM_SETARRAY, &msg, 1);
    WinSetDlgSpinVal(hwnd, id, 0);
    lib_free(msg);
}

static MRESULT EXPENTRY pm_keyset(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    static int set1 = TRUE;
    static int id = 0;

    switch (msg) {
        case WM_INITDLG:
            {
                int j1, j2;

                resources_get_int("JoyDevice1", &j1);
                resources_get_int("JoyDevice2", &j2);
                WinSendMsg(hwnd, WM_KPROCESS, (void*)!!((j1 | j2) & JOYDEV_KEYSET1), (void*)!!((j1 | j2) & JOYDEV_KEYSET2));
            }
            break;
        case WM_CONTROL:
            {
                int ctrl = SHORT1FROMMP(mp1);

                switch (ctrl) {
                    case RB_SET1:
                    case RB_SET2:
                        if (!(ctrl == RB_SET1 && set1) && !(ctrl == RB_SET2 && !set1)) {
                            set1 = !set1;
                            WinSendMsg(hwnd, WM_KENABLECTRL, 0, 0);
                            WinSendMsg(hwnd, WM_KFILLSPB, 0, 0);
                        }
                        break;
                    case SPB_N:
                    case SPB_NE:
                    case SPB_E:
                    case SPB_SE:
                    case SPB_S:
                    case SPB_SW:
                    case SPB_W:
                    case SPB_NW:
                    case SPB_FIRE:
                        if (SHORT2FROMMP(mp1) == SPBN_SETFOCUS) {
                            id = ctrl;
                        }
                        break;
                }
                break;
            }
        case WM_KPROCESS:
            if (((int)mp1 ^ (int)mp2) & 1) {
                set1 = (int)mp1;
            }

            WinCheckButton(hwnd, set1 ? RB_SET1 : RB_SET2, 1);
            WinEnableControl(hwnd, RB_SET1, (ULONG)mp1);
            WinEnableControl(hwnd, RB_SET2, (ULONG)mp2);

            WinSendMsg(hwnd, WM_KENABLECTRL, (void*)(!mp1 && !mp2), NULL);
            WinSendMsg(hwnd, WM_KFILLSPB, 0, 0);
            break;
        case WM_TRANSLATEACCEL:
            if (id >= SPB_N && id <= SPB_FIRE) {
                //
                // Returning FALSE and using WM_CHAR doesn't work
                // for all keys - I don't know why
                //
                SetKeyVal(hwnd, id, set1, CHAR4FROMMP(((QMSG*)mp1)->mp1));
                return FALSE;
            }
            break;
        case WM_SETKEY:
            {
                ULONG state1 = mp1 ? 1 : 0;
                ULONG state2 = mp2 ? 1 : 0;

                WinEnableControl(hwnd, RB_SET1, state1);
                WinEnableControl(hwnd, RB_SET2, state2);
                WinSendMsg(hwnd, WM_KPROCESS, (void*)state1, (void*)state2);
            }
            break;
        case WM_KENABLECTRL:
            WinEnableControl(hwnd, SPB_N, mp1 ? 0 : 1);
            WinEnableControl(hwnd, SPB_E, mp1 ? 0 : 1);
            WinEnableControl(hwnd, SPB_S, mp1 ? 0 : 1);
            WinEnableControl(hwnd, SPB_W, mp1 ? 0 : 1);
            WinEnableControl(hwnd, SPB_NE, mp1 ? 0 : 1);
            WinEnableControl(hwnd, SPB_SE, mp1 ? 0 : 1);
            WinEnableControl(hwnd, SPB_SW, mp1 ? 0 : 1);
            WinEnableControl(hwnd, SPB_NW, mp1 ? 0 : 1);
            WinEnableControl(hwnd, SPB_FIRE, mp1 ? 0 : 1);
            return FALSE;
        case WM_KFILLSPB:
            UpdateKeyVal(hwnd, SPB_N, set1);
            UpdateKeyVal(hwnd, SPB_E, set1);
            UpdateKeyVal(hwnd, SPB_S, set1);
            UpdateKeyVal(hwnd, SPB_W, set1);
            UpdateKeyVal(hwnd, SPB_NE, set1);
            UpdateKeyVal(hwnd, SPB_SE, set1);
            UpdateKeyVal(hwnd, SPB_SW, set1);
            UpdateKeyVal(hwnd, SPB_NW, set1);
            UpdateKeyVal(hwnd, SPB_FIRE, set1);
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
    static HWND hwnd2 = NULLHANDLE;

    if (WinIsWindowVisible(hwnd2)) {
        return;
    }

    hwnd2 = WinLoadStdDlg(hwnd, pm_joystick, DLG_JOYSTICK, NULL);
}

void joystick_extra_dialog(HWND hwnd)
{
    static HWND hwnd3 = NULLHANDLE;

    if (WinIsWindowVisible(hwnd3)) {
        return;
    }

    hwnd3 = WinLoadStdDlg(hwnd, pm_extra_joystick, DLG_EXTRA_JOYSTICK, NULL);
}

void calibrate_dialog(HWND hwnd)
{
    if (WinIsWindowVisible(hwndCalibrate)) {
        return;
    }

    hwndCalibrate = WinLoadStdDlg(hwnd, pm_calibrate, DLG_CALIBRATE, NULL);
}

void keyset_dialog(HWND hwnd)
{
    if (WinIsWindowVisible(hwndKeyset)) {
        return;
    }

    hwndKeyset = WinLoadStdDlg(hwnd, pm_keyset, DLG_KEYSET, NULL);
}
#endif
