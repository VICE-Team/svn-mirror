/*
 * joystick.c - Joystick support for Windows.
 *
 * Written by
 *  Tibor Biczo <crown@mail.matav.hu>
 *  Ettore Perazzoli <ettore@comm2000.it>
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

#include "cmdline.h"
#include "resources.h"

#define DIRECTINPUT_VERSION     0x0500
#include <winerror.h>
#include <ddraw.h>
#include <dinput.h>
#include <mmsystem.h>

#include "joy.h"
#include "joystick.h"
#include "keyboard.h"
#include "maincpu.h"
#include "types.h"
#include "ui.h"
#include "winmain.h"

int joystick_inited;

/* Notice that this has to be `int' to make resources work.  */
static int keyset1[9], keyset2[9];

static int joystick_fire_speed[2];
static int joystick_fire_axis[2];

#ifdef COMMON_KBD
int joystick_port_map[2];
#endif

/* ------------------------------------------------------------------------ */

/* Joystick devices.  */
static joystick_device_t joystick_device_1, joystick_device_2;

static int set_joystick_device_1(resource_value_t v, void *param)
{
    joystick_device_t dev = (joystick_device_t) v;

    joystick_device_1 = dev;
    return 0;
}

static int set_joystick_device_2(resource_value_t v, void *param)
{
    joystick_device_t dev = (joystick_device_t) v;

    joystick_device_2 = dev;
    return 0;
}

static int set_joystick_fire1_speed(resource_value_t v, void *param)
{
int speed = (int)v;

    if(speed < 1) speed = 1;
    if(speed > 32) speed = 32;
    joystick_fire_speed[0] = speed;
    return 0;
}

static int set_joystick_fire2_speed(resource_value_t v, void *param)
{
int speed = (int)v;

    if(speed < 1) speed = 1;
    if(speed > 32) speed = 32;
    joystick_fire_speed[1] = speed;
    return 0;
}

static int set_joystick_fire1_axis(resource_value_t v, void *param)
{
int axis =  (int)v;

    if (axis<0) axis=0;
    if (axis>4) axis=4;
    joystick_fire_axis[0] = axis;
    return 0;
}

static int set_joystick_fire2_axis(resource_value_t v, void *param)
{
int axis =  (int)v;

    if (axis<0) axis=0;
    if (axis>4) axis=4;
    joystick_fire_axis[1] = axis;
    return 0;
}

#define DEFINE_SET_KEYSET(num)                                     \
    static int set_keyset##num(resource_value_t v, void *param)  \
    {                                                              \
        keyset##num[(int)param] = (int) v;                       \
                                                                   \
        return 0;                                                  \
    }

DEFINE_SET_KEYSET(1)
DEFINE_SET_KEYSET(2)


static resource_t resources[] = {
    { "JoyDevice1", RES_INTEGER, (resource_value_t) JOYDEV_NONE,
      (resource_value_t *) &joystick_device_1,
      set_joystick_device_1, NULL },
    { "JoyDevice2", RES_INTEGER, (resource_value_t) JOYDEV_NONE,
      (resource_value_t *) &joystick_device_2,
      set_joystick_device_2, NULL },
    { "JoyAutofire1Speed", RES_INTEGER, (resource_value_t) 16,
      (resource_value_t *) &joystick_fire_speed[0], set_joystick_fire1_speed, NULL },
    { "JoyAutofire1Axis", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &joystick_fire_axis[0], set_joystick_fire1_axis, NULL },
    { "JoyAutofire2Speed", RES_INTEGER, (resource_value_t) 16,
      (resource_value_t *) &joystick_fire_speed[1], set_joystick_fire2_speed, NULL },
    { "JoyAutofire2Axis", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &joystick_fire_axis[1], set_joystick_fire2_axis, NULL },
    { "KeySet1NorthWest", RES_INTEGER, (resource_value_t) K_NONE,
      (resource_value_t *) &keyset1[KEYSET_NW],
       set_keyset1, (void *)KEYSET_NW },
    { "KeySet1North", RES_INTEGER, (resource_value_t) K_NONE,
      (resource_value_t *) &keyset1[KEYSET_N],
      set_keyset1, (void *)KEYSET_N },
    { "KeySet1NorthEast", RES_INTEGER, (resource_value_t) K_NONE,
      (resource_value_t *) &keyset1[KEYSET_NE],
      set_keyset1, (void *)KEYSET_NE },
    { "KeySet1East", RES_INTEGER, (resource_value_t) K_NONE,
      (resource_value_t *) &keyset1[KEYSET_E],
      set_keyset1, (void *)KEYSET_E },
    { "KeySet1SouthEast", RES_INTEGER, (resource_value_t) K_NONE,
      (resource_value_t *) &keyset1[KEYSET_SE],
      set_keyset1, (void *)KEYSET_SE },
    { "KeySet1South", RES_INTEGER, (resource_value_t) K_NONE,
      (resource_value_t *) &keyset1[KEYSET_S],
      set_keyset1, (void *)KEYSET_S },
    { "KeySet1SouthWest", RES_INTEGER, (resource_value_t) K_NONE,
      (resource_value_t *) &keyset1[KEYSET_SW],
      set_keyset1, (void *)KEYSET_SW },
    { "KeySet1West", RES_INTEGER, (resource_value_t) K_NONE,
      (resource_value_t *) &keyset1[KEYSET_W],
      set_keyset1, (void *)KEYSET_W },
    { "KeySet1Fire", RES_INTEGER, (resource_value_t) K_NONE,
      (resource_value_t *) &keyset1[KEYSET_FIRE],
      set_keyset1, (void *)KEYSET_FIRE },
    { "KeySet2NorthWest", RES_INTEGER, (resource_value_t) K_NONE,
      (resource_value_t *) &keyset2[KEYSET_NW],
      set_keyset2, (void *)KEYSET_NW },
    { "KeySet2North", RES_INTEGER, (resource_value_t) K_NONE,
      (resource_value_t *) &keyset2[KEYSET_N],
      set_keyset2, (void *)KEYSET_N },
    { "KeySet2NorthEast", RES_INTEGER, (resource_value_t) K_NONE,
      (resource_value_t *) &keyset2[KEYSET_NE],
      set_keyset2, (void *)KEYSET_NE },
    { "KeySet2East", RES_INTEGER, (resource_value_t) K_NONE,
      (resource_value_t *) &keyset2[KEYSET_E],
      set_keyset2, (void *)KEYSET_E },
    { "KeySet2SouthEast", RES_INTEGER, (resource_value_t) K_NONE,
      (resource_value_t *) &keyset2[KEYSET_SE],
      set_keyset2, (void *)KEYSET_SE },
    { "KeySet2South", RES_INTEGER, (resource_value_t) K_NONE,
      (resource_value_t *) &keyset2[KEYSET_S],
      set_keyset2, (void *)KEYSET_S },
    { "KeySet2SouthWest", RES_INTEGER, (resource_value_t) K_NONE,
      (resource_value_t *) &keyset2[KEYSET_SW],
      set_keyset2, (void *)KEYSET_SW },
    { "KeySet2West", RES_INTEGER, (resource_value_t) K_NONE,
      (resource_value_t *) &keyset2[KEYSET_W],
      set_keyset2, (void *)KEYSET_W },
    { "KeySet2Fire", RES_INTEGER, (resource_value_t) K_NONE,
      (resource_value_t *) &keyset2[KEYSET_FIRE],
      set_keyset2, (void *)KEYSET_FIRE },
    { NULL }
};

int joystick_init_resources(void)
{
    return resources_register(resources);
}

/* ------------------------------------------------------------------------- */

static cmdline_option_t cmdline_options[] = {
    { "-joydev1", SET_RESOURCE, 1, NULL, NULL,
      "JoyDevice1", NULL,
      "<number>", "Set input device for joystick #1" },
    { "-joydev2", SET_RESOURCE, 1, NULL, NULL,
      "JoyDevice2", NULL,
      "<number>", "Set input device for joystick #2" },
    { NULL }
};

int joystick_init_cmdline_options(void)
{
    return cmdline_register_options(cmdline_options);
}

/* ------------------------------------------------------------------------- */

LPDIRECTINPUT               di;
int                         directinput_inited;
extern LPDIRECTINPUTDEVICE  di_mouse;
void mouse_set_format(void);

#if 0
static BOOL CALLBACK EnumCallBack(LPCDIDEVICEINSTANCE lpddi, LPVOID pvref)
{
    log_debug("DirectInput device found : %d",lpddi->dwDevType);
    log_debug(lpddi->tszProductName);
    log_debug(lpddi->tszInstanceName);
    return DIENUM_CONTINUE;
}
#endif

int joy_arch_init(void)
{
    HRESULT result;

    result=DirectInputCreate(winmain_instance,0x0300,&di,NULL);
    if (result!=DI_OK) {
        di=NULL;
        di_mouse=NULL;
        return 0;
    }
    joystick_inited=1;

    /*  Init mouse device */
    result=IDirectInput_CreateDevice(di,(GUID *)&GUID_SysMouse,&di_mouse,NULL);
    if (result!=DI_OK) {
        di_mouse=NULL;
        return 0;
    }
    mouse_set_format();

//    IDirectInput_EnumDevices(di,0,EnumCallBack,0,0);

#ifdef COMMON_KBD
    joystick_port_map[0] = 1;
    joystick_port_map[1] = 2;
#endif

    return 0;
}

int joystick_close(void)
{
    if (di_mouse!=NULL) {
        IDirectInputDevice_Unacquire(di_mouse);
        IDirectInputDevice_Release(di_mouse);
        di_mouse=NULL;
    }
    if (di!=NULL) IDirectInput_Release(di);
    joystick_inited=0;
    return 0;
}

JOYINFOEX   joy_info;
JOYCAPS     joy_caps;

void joystick_update(void)
{
BYTE        value;
MMRESULT    result;
int         idx;
DWORD       addflag;
UINT        amin;
UINT        amax;
DWORD       apos;

    if((idx = (joystick_device_1==JOYDEV_HW1) ? 0 : ((joystick_device_2==JOYDEV_HW1) ? 1 : -1)) != -1) {
        switch(joystick_fire_axis[idx]) {
            case 1: addflag = JOY_RETURNZ; break;
            case 2: addflag = JOY_RETURNV; break;
            case 3: addflag = JOY_RETURNU; break;
            case 4: addflag = JOY_RETURNR; break;
            default: addflag = 0;
        }
        joy_info.dwFlags=JOY_RETURNBUTTONS | JOY_RETURNCENTERED | JOY_RETURNX | JOY_RETURNY | addflag;
        value=0;
        joy_info.dwSize=sizeof(JOYINFOEX);
        result=joyGetPosEx(JOYSTICKID1,&joy_info);
        if (result==JOYERR_NOERROR) {
            result=joyGetDevCaps(JOYSTICKID1,&joy_caps,sizeof(JOYCAPS));
            if (result==JOYERR_NOERROR) {
                if (joy_info.dwXpos<=joy_caps.wXmin+(joy_caps.wXmax-joy_caps.wXmin)/4) {
                    value|=4;
                }
                if (joy_info.dwXpos>=joy_caps.wXmin+(joy_caps.wXmax-joy_caps.wXmin)/4*3) {
                    value|=8;
                }
                if (joy_info.dwYpos<=joy_caps.wYmin+(joy_caps.wYmax-joy_caps.wYmin)/4) {
                    value|=1;
                }
                if (joy_info.dwYpos>=joy_caps.wYmin+(joy_caps.wYmax-joy_caps.wYmin)/4*3) {
                    value|=2;
                }
                if(joy_info.dwButtons & 0x0001) {
                    value |= 16;
                } else if(joy_info.dwButtons & 0x0002) {
                    if((joystick_fire_axis[idx])&&(joy_info.dwFlags&addflag)) {
                        switch(joystick_fire_axis[idx]) {
                            case 1:
                                amin = joy_caps.wZmin;
                                amax = joy_caps.wZmax;
                                apos = joy_info.dwZpos;
                                break;
                            case 2:
                                amin = joy_caps.wVmin;
                                amax = joy_caps.wVmax;
                                apos = joy_info.dwVpos;       
                                break;
                            case 3:
                                amin = joy_caps.wUmin;
                                amax = joy_caps.wUmax;
                                apos = joy_info.dwUpos;
                                break;
                            case 4:
                                amin = joy_caps.wRmin;
                                amax = joy_caps.wRmax;
                                apos = joy_info.dwRpos;
                                break;
                            default:
                                amin = 0;
                                amax = 32;
                                apos = 16;
                                break;
                        }
                        value |= maincpu_clk/(((amin+apos)*0x2000)/(amax-amin)+1) & 16;
                    } else {
                        value |= (maincpu_clk/(joystick_fire_speed[idx]*0x100)) & 16;
                    }     
                }
                joystick_set_value_absolute(idx+1, value);
            }
        }
    }
    if((idx = (joystick_device_1==JOYDEV_HW2) ? 0 : ((joystick_device_2==JOYDEV_HW2) ? 1 : -1)) != -1) {
        switch(joystick_fire_axis[idx]) {
            case 1: addflag = JOY_RETURNZ; break;
            case 2: addflag = JOY_RETURNV; break;
            case 3: addflag = JOY_RETURNU; break;
            case 4: addflag = JOY_RETURNR; break;
            default: addflag = 0;
        }
        joy_info.dwFlags=JOY_RETURNBUTTONS | JOY_RETURNCENTERED | JOY_RETURNX | JOY_RETURNY | addflag;
        value=0;
        joy_info.dwSize=sizeof(JOYINFOEX);
        result=joyGetPosEx(JOYSTICKID2,&joy_info);
        if (result==JOYERR_NOERROR) {
            result=joyGetDevCaps(JOYSTICKID2,&joy_caps,sizeof(JOYCAPS));
            if (result==JOYERR_NOERROR) {
                if (joy_info.dwXpos<=joy_caps.wXmin+(joy_caps.wXmax-joy_caps.wXmin)/4) {
                    value|=4;
                }
                if (joy_info.dwXpos>=joy_caps.wXmin+(joy_caps.wXmax-joy_caps.wXmin)/4*3) {
                    value|=8;
                }
                if (joy_info.dwYpos<=joy_caps.wYmin+(joy_caps.wYmax-joy_caps.wYmin)/4) {
                    value|=1;
                }
                if (joy_info.dwYpos>=joy_caps.wYmin+(joy_caps.wYmax-joy_caps.wYmin)/4*3) {
                    value|=2;
                }
                if(joy_info.dwButtons & 0x0001) {
                    value |= 16;
                } else if(joy_info.dwButtons & 0x0002) {
                    if((joystick_fire_axis[idx])&&(joy_info.dwFlags&addflag)) {
                        switch(joystick_fire_axis[idx]) {
                            case 1:
                                amin = joy_caps.wZmin;
                                amax = joy_caps.wZmax;
                                apos = joy_info.dwZpos;
                                break;
                            case 2:
                                amin = joy_caps.wVmin;
                                amax = joy_caps.wVmax;
                                apos = joy_info.dwVpos;       
                                break;
                            case 3:
                                amin = joy_caps.wUmin;
                                amax = joy_caps.wUmax;
                                apos = joy_info.dwUpos;
                                break;
                            case 4:
                                amin = joy_caps.wRmin;
                                amax = joy_caps.wRmax;
                                apos = joy_info.dwRpos;
                                break;
                            default:
                                amin = 0;
                                amax = 32;
                                apos = 16;
                                break;
                        }
                        value |= maincpu_clk/(((amin+apos)*0x2000)/(amax-amin)+1) & 16;
                    } else {
                        value |= (maincpu_clk/(joystick_fire_speed[idx]*0x100)) & 16;
                    }     
                }
                joystick_set_value_absolute(idx+1, value);
            }
        }
    }
}

/* Joystick-through-keyboard.  */

int handle_keyset_mapping(joystick_device_t device, int *set,
                          kbd_code_t kcode, int pressed)
{
    if (joystick_device_1 == device || joystick_device_2 == device) {
        BYTE value = 0;

        if (kcode == set[KEYSET_NW])    /* North-West */
            value = 5;
        else if (kcode == set[KEYSET_N]) /* North */
            value = 1;
        else if (kcode == set[KEYSET_NE]) /* North-East */
            value = 9;
        else if (kcode == set[KEYSET_E]) /* East */
            value = 8;
        else if (kcode == set[KEYSET_SE]) /* South-East */
            value = 10;
        else if (kcode == set[KEYSET_S]) /* South */
            value = 2;
        else if (kcode == set[KEYSET_SW]) /* South-West */
            value = 6;
        else if (kcode == set[KEYSET_W]) /* West */
            value = 4;
        else if (kcode == set[KEYSET_FIRE]) /* Fire */
            value = 16;
        else
            return 0;

        if (pressed) {
            if (joystick_device_1 == device)
                joystick_set_value_or(1, value);
            if (joystick_device_2 == device)
                joystick_set_value_or(2, value);
        } else {
            if (joystick_device_1 == device)
                joystick_set_value_and(1, (BYTE) ~value);
            if (joystick_device_2 == device)
                joystick_set_value_and(2, (BYTE) ~value);
        }
        return 1;
    }

    return 0;
}

int joystick_handle_key(kbd_code_t kcode, int pressed)
{
    BYTE value = 0;

    /* The numpad case is handled specially because it allows users to use
       both `5' and `2' for "down".  */
    if (joystick_device_1 == JOYDEV_NUMPAD
        || joystick_device_2 == JOYDEV_NUMPAD) {

        switch (kcode) {
          case K_KP7:               /* North-West */
            value = 5;
            break;
          case K_KP8:               /* North */
            value = 1;
            break;
          case K_KP9:               /* North-East */
            value = 9;
            break;
          case K_KP6:               /* East */
            value = 8;
            break;
          case K_KP3:               /* South-East */
            value = 10;
            break;
          case K_KP2:               /* South */
          case K_KP5:
            value = 2;
            break;
          case K_KP1:               /* South-West */
            value = 6;
            break;
          case K_KP4:               /* West */
            value = 4;
            break;
          case K_KP0:
          case K_RIGHTCTRL:
            value = 16;
            break;
          default:
            /* (make compiler happy) */
            ;
        }

        if (pressed) {
            if (joystick_device_1 == JOYDEV_NUMPAD)
                joystick_set_value_or(1, value);
            if (joystick_device_2 == JOYDEV_NUMPAD)
                joystick_set_value_or(2, value);
        } else {
            if (joystick_device_1 == JOYDEV_NUMPAD)
                joystick_set_value_and(1, (BYTE) ~value);
            if (joystick_device_2 == JOYDEV_NUMPAD)
                joystick_set_value_and(2, (BYTE) ~value);
        }
    }

    /* (Notice we have to handle all the keysets even when one key is used
       more than once (the most intuitive behavior), so we use `|' instead of
       `||'.)  */
    return (value
            | handle_keyset_mapping(JOYDEV_KEYSET1, keyset1, kcode, pressed)
            | handle_keyset_mapping(JOYDEV_KEYSET2, keyset2, kcode, pressed));
}

void joystick_calibrate(HWND hwnd)
{
    IDirectInput_RunControlPanel(di,hwnd,0);
}
