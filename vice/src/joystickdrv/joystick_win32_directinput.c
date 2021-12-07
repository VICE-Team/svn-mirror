/** \file   joystick_win32_directinput.c
 * \brief   Joystick support for Windows
 *
 * \author  Tibor Biczo <crown@mail.matav.hu>
 * \author  Ettore Perazzoli <ettore@comm2000.it>
 */

/*
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

#include "joyport.h"
#include "joystick.h"
#include "lib.h"

#define DIRECTINPUT_VERSION 0x0800
#define INITGUID
#include <dinput.h>

/* not defined in DirectInput headers prior to 8 */
#ifndef DIDFT_OPTIONAL
#define DIDFT_OPTIONAL 0x80000000
#endif

typedef struct _JoyAxis {
    struct _JoyAxis *next;
    DWORD id;
    char *name;
} JoyAxis;

typedef struct _JoyButton {
    struct _JoyButton *next;
    DWORD id;
    char *name;
} JoyButton;

typedef struct _JoyInfo {
    GUID guid;
    JoyAxis *axes;
    JoyButton *buttons;
    int numaxes;
    int numbuttons;
    int numPOVs;
    LPDIRECTINPUTDEVICE8 di_device;
} JoyInfo;

#ifndef HAVE_DINPUT_LIB
static DIOBJECTDATAFORMAT joystick_objects[] = {
    { &GUID_XAxis, 0, DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_AXIS, DIDOI_ASPECTPOSITION },
    { &GUID_YAxis, 4, DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_AXIS, DIDOI_ASPECTPOSITION },
    { &GUID_ZAxis, 8, DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_AXIS, DIDOI_ASPECTPOSITION },
    { &GUID_RxAxis, 0x0c, DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_AXIS, DIDOI_ASPECTPOSITION },
    { &GUID_RyAxis, 0x10, DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_AXIS, DIDOI_ASPECTPOSITION },
    { &GUID_RzAxis, 0x14, DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_AXIS, DIDOI_ASPECTPOSITION },
    { &GUID_Slider, 0x18, DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_AXIS, DIDOI_ASPECTPOSITION },
    { &GUID_Slider, 0x1c, DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_AXIS, DIDOI_ASPECTPOSITION },
    { &GUID_POV, 0x20, DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_POV, 0 },
    { &GUID_POV, 0x24, DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_POV, 0 },
    { &GUID_POV, 0x28, DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_POV, 0 },
    { &GUID_POV, 0x2c, DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_POV, 0 },
    { NULL, 0x30, DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_BUTTON, 0 },
    { NULL, 0x31, DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_BUTTON, 0 },
    { NULL, 0x32, DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_BUTTON, 0 },
    { NULL, 0x33, DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_BUTTON, 0 },
    { NULL, 0x34, DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_BUTTON, 0 },
    { NULL, 0x35, DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_BUTTON, 0 },
    { NULL, 0x36, DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_BUTTON, 0 },
    { NULL, 0x37, DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_BUTTON, 0 },
    { NULL, 0x38, DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_BUTTON, 0 },
    { NULL, 0x39, DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_BUTTON, 0 },
    { NULL, 0x3a, DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_BUTTON, 0 },
    { NULL, 0x3b, DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_BUTTON, 0 },
    { NULL, 0x3c, DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_BUTTON, 0 },
    { NULL, 0x3d, DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_BUTTON, 0 },
    { NULL, 0x3e, DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_BUTTON, 0 },
    { NULL, 0x3f, DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_BUTTON, 0 },
    { NULL, 0x40, DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_BUTTON, 0 },
    { NULL, 0x41, DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_BUTTON, 0 },
    { NULL, 0x42, DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_BUTTON, 0 },
    { NULL, 0x43, DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_BUTTON, 0 },
    { NULL, 0x44, DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_BUTTON, 0 },
    { NULL, 0x45, DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_BUTTON, 0 },
    { NULL, 0x46, DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_BUTTON, 0 },
    { NULL, 0x47, DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_BUTTON, 0 },
    { NULL, 0x48, DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_BUTTON, 0 },
    { NULL, 0x49, DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_BUTTON, 0 },
    { NULL, 0x4a, DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_BUTTON, 0 },
    { NULL, 0x4b, DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_BUTTON, 0 },
    { NULL, 0x4c, DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_BUTTON, 0 },
    { NULL, 0x4d, DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_BUTTON, 0 },
    { NULL, 0x4e, DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_BUTTON, 0 },
    { NULL, 0x4f, DIDFT_OPTIONAL | DIDFT_ANYINSTANCE | DIDFT_BUTTON, 0 }
};

static DIDATAFORMAT data_format_struct = {
    sizeof(DIDATAFORMAT),
    sizeof(DIOBJECTDATAFORMAT),
    DIDF_ABSAXIS,
    sizeof(DIJOYSTATE),
    sizeof(joystick_objects) / sizeof(*joystick_objects),
    joystick_objects
};
#endif

static LPDIRECTINPUT8 di = NULL;
static BOOL CALLBACK EnumJoyAxes(LPCDIDEVICEOBJECTINSTANCE lpddoi, LPVOID pvRef)
{
    JoyAxis *axis;
    JoyInfo *joy;

    joy = (JoyInfo*)pvRef;

    /*  Save info about axis */
    axis = lib_malloc(sizeof(JoyAxis));
    axis->next = NULL;
    axis->id = DIDFT_GETINSTANCE(lpddoi->dwType);
    axis->name = lib_strdup(lpddoi->tszName);

    /*  Link axis into list for this joystick */
    if (joy->axes == NULL) {
        joy->axes = axis;
    } else {
        JoyAxis *s;
        s = joy->axes;
        while (s->next != NULL) {
            s = s->next;
        }
        s->next = axis;
    }
    joy->numaxes += 1;
    return DIENUM_CONTINUE;
}

static BOOL CALLBACK EnumJoyButtons(LPCDIDEVICEOBJECTINSTANCE lpddoi, LPVOID pvRef)
{
    JoyButton *button;
    JoyInfo *joy;

    joy = (JoyInfo*)pvRef;

    /*  Save info about button */
    button = lib_malloc(sizeof(JoyButton));
    button->next = NULL;
    button->id = DIDFT_GETINSTANCE(lpddoi->dwType);
    button->name = lib_strdup(lpddoi->tszName);

    /*  Link button into list for this joystick */
    if (joy->buttons == NULL) {
        joy->buttons = button;
    } else {
        JoyButton *s;
        s = joy->buttons;
        while (s->next != NULL) {
            s = s->next;
        }
        s->next = button;
    }
    joy->numbuttons += 1;
    return DIENUM_CONTINUE;
}

static BOOL CALLBACK EnumJoyPOVs(LPCDIDEVICEOBJECTINSTANCE lpddoi, LPVOID pvRef)
{
    JoyInfo *joy;

    joy = (JoyInfo*)pvRef;

    /*  Save info about POV */
    joy->numPOVs += 1;

    return DIENUM_CONTINUE;
}

static void joystick_release_axes(JoyAxis *axis)
{
    while (axis != NULL) {
        JoyAxis *next;

        next = axis->next;
        lib_free(axis->name);
        lib_free(axis);
        axis = next;
    }
}
static void joystick_release_buttons(JoyButton *button)
{
    while (button != NULL) {
        JoyButton *next;

        next = button->next;
        lib_free(button->name);
        lib_free(button);
        button = next;
    }
}

static void joystick_di5_update(int joyport, void* priv)
{
    BYTE value;
    int i;
    DIPROPRANGE prop;
    DIJOYSTATE js;
    JoyInfo *joy;

    value = 0;
    joy = priv;

    IDirectInputDevice8_Poll(joy->di_device);
    IDirectInputDevice8_GetDeviceState(joy->di_device, sizeof(DIJOYSTATE), &js);

    /* Get boundary values for X axis */
    prop.diph.dwSize = sizeof(DIPROPRANGE);
    prop.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    prop.diph.dwObj = 0;    /* Offset of X axis */
    prop.diph.dwHow = DIPH_BYOFFSET;
    IDirectInputDevice8_GetProperty(joy->di_device, DIPROP_RANGE, (DIPROPHEADER*)&prop);
    if (js.lX <= prop.lMin + (prop.lMax - prop.lMin) / 4) {
        joy_axis_event(joyport, 0, JOY_AXIS_NEGATIVE);
    } else if (js.lX >= prop.lMin + (prop.lMax - prop.lMin) / 4 * 3) {
        joy_axis_event(joyport, 0, JOY_AXIS_POSITIVE);
    } else {
        joy_axis_event(joyport, 0, JOY_AXIS_MIDDLE);
    }

    /* Get boundary values for Y axis */
    prop.diph.dwSize = sizeof(DIPROPRANGE);
    prop.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    prop.diph.dwObj = 4;    /* Offset of Y axis */
    prop.diph.dwHow = DIPH_BYOFFSET;
    IDirectInputDevice8_GetProperty(joy->di_device, DIPROP_RANGE, (DIPROPHEADER*)&prop);
    if (js.lY <= prop.lMin + (prop.lMax - prop.lMin) / 4) {
        joy_axis_event(joyport, 1, JOY_AXIS_NEGATIVE);
    } else if (js.lY >= prop.lMin + (prop.lMax - prop.lMin) / 4 * 3) {
        joy_axis_event(joyport, 1, JOY_AXIS_POSITIVE);
    } else {
        joy_axis_event(joyport, 1, JOY_AXIS_MIDDLE);
    }

    for (i = 0; i < joy->numPOVs; ++i) {
        if (LOWORD(js.rgdwPOV[i]) != 0xffff) {
            if (js.rgdwPOV[i] > 20250 && js.rgdwPOV[i] < 33750) {
                value |= 4;
            }
            if (js.rgdwPOV[i] > 2250 && js.rgdwPOV[i] < 15750) {
                value |= 8;
            }
            if (js.rgdwPOV[i] > 29250 || js.rgdwPOV[i] < 6750) {
                value |= 1;
            }
            if (js.rgdwPOV[i] > 11250 && js.rgdwPOV[i] < 24750) {
                value |= 2;
            }
        }
        joy_hat_event(joyport, i, value);
    }
    for (i = 0; i < joy->numbuttons; ++i) {
        joy_button_event(joyport, i, (js.rgbButtons[i] & 0x80));
    }
}

static void joystick_release_joystick(void* priv)
{
    JoyInfo *joystick = priv;
    joystick_release_axes(joystick->axes);
    joystick_release_buttons(joystick->buttons);
    IDirectInputDevice8_Unacquire(joystick->di_device);
    IDirectInputDevice8_Release(joystick->di_device);
    lib_free(joystick);
}

static joystick_driver_t win32_directinput_joystick_driver = {
    .poll = joystick_di5_update,
    .close = joystick_release_joystick
};

static BOOL CALLBACK EnumCallBack(LPCDIDEVICEINSTANCE lpddi, LPVOID pvref)
{
    JoyInfo *new_joystick;
    HINSTANCE ui_active_window = GetModuleHandle(NULL); /* FIXME */

    new_joystick = lib_malloc(sizeof(JoyInfo));
    memcpy(&new_joystick->guid, &lpddi->guidInstance, sizeof(GUID));
    new_joystick->axes = NULL;
    new_joystick->buttons = NULL;
    new_joystick->numPOVs = 0;
    new_joystick->numaxes = 0;
    new_joystick->numbuttons = 0;
    IDirectInput8_CreateDevice(di, &new_joystick->guid, &new_joystick->di_device, NULL);
#ifdef HAVE_DINPUT_LIB
    IDirectInputDevice8_SetDataFormat(new_joystick->di_device, &c_dfDIJoystick);
#else
    IDirectInputDevice8_SetDataFormat(new_joystick->di_device, &data_format_struct);
#endif
    IDirectInputDevice8_SetCooperativeLevel(new_joystick->di_device,
        (HWND)ui_active_window, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);
    IDirectInputDevice8_Acquire(new_joystick->di_device);
    IDirectInputDevice8_EnumObjects(new_joystick->di_device, EnumJoyAxes, (LPVOID)new_joystick, DIDFT_AXIS);
    IDirectInputDevice8_EnumObjects(new_joystick->di_device, EnumJoyButtons, (LPVOID)new_joystick, DIDFT_BUTTON);
    IDirectInputDevice8_EnumObjects(new_joystick->di_device, EnumJoyPOVs, (LPVOID)new_joystick, DIDFT_POV);
    register_joystick_driver(&win32_directinput_joystick_driver,
        lpddi->tszInstanceName,
        new_joystick,
        new_joystick->numaxes,
        new_joystick->numbuttons,
        new_joystick->numPOVs
    );

    return DIENUM_CONTINUE;
}

int win32_directinput_joystick_init(void)
{
#ifndef HAVE_DINPUT_LIB
    HRESULT res;
#endif

    HINSTANCE winmain_instance = GetModuleHandle(NULL); /* FIXME */
#ifdef HAVE_DINPUT_LIB
    if (DirectInput8Create(winmain_instance, DIRECTINPUT_VERSION, &IID_IDirectInput8, (LPVOID*)&di, NULL) != DI_OK) {
        return 0;
    }
#else
    res = CoCreateInstance(&CLSID_DirectInput8, NULL, CLSCTX_INPROC_SERVER, &IID_IDirectInput8, (PVOID*)&di);
    if (res != S_OK) {
        return 0;
    }
    if (IDirectInput8_Initialize(di, winmain_instance, DIRECTINPUT_VERSION) != S_OK) {
        IDirectInput8_Release(di);
        return 0;
    }
#endif
    IDirectInput8_EnumDevices(di, DIDEVTYPE_JOYSTICK, EnumCallBack, NULL, DIEDFL_ALLDEVICES);
    return 1;
}
