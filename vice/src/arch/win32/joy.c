/*
 * joy.c - Joystick support for Windows.
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

#include "lib.h"
#include "joy.h"
#include "joystick.h"
#include "keyboard.h"
#include "maincpu.h"
#include "res.h"
#include "translate.h"
#include "types.h"
#include "ui.h"
#include "winmain.h"

int joystick_inited = 0;

/* Notice that this has to be `int' to make resources work.  */
static int keyset1[9], keyset2[9];

static int joystick_fire_speed[2];
static int joystick_fire_axis[2];
static int joystick_autofire_button[2];

static int joystick_fire_button[2];

#ifdef COMMON_KBD
int joystick_port_map[2];
#endif

/* ------------------------------------------------------------------------ */

/* Joystick devices.  */
static LPDIRECTINPUTDEVICE  joystick_di_devices[2] = {NULL, NULL};
static LPDIRECTINPUTDEVICE2  joystick_di_devices2[2] = {NULL, NULL};
int                         di_version;
LPDIRECTINPUT               di;

typedef struct _JoyAxis {
    struct _JoyAxis *next;
    DWORD           id;
    char            *name;
    DWORD           dwOffs;
} JoyAxis;

typedef struct _JoyButton {
    struct _JoyButton   *next;
    DWORD               id;
    char                *name;
    DWORD               dwOffs;
} JoyButton;

typedef struct _JoyInfo {
    struct _JoyInfo *next;
    GUID            guid;
    char            *name;
    JoyAxis         *axes;
    JoyButton       *buttons;
} JoyInfo;

static JoyInfo  *joystick_list = NULL;


static BOOL CALLBACK EnumJoyAxes(LPCDIDEVICEOBJECTINSTANCE lpddoi, LPVOID pvRef)
{
JoyAxis *axis;
JoyInfo *joy;

//    log_debug("Axis, offset : %s %d", lpddoi->tszName, lpddoi->dwOfs);

    joy = (JoyInfo*)pvRef;

    //  Save info about axis
    axis = lib_malloc(sizeof(JoyAxis));
    axis->next = NULL;
    axis->id = DIDFT_GETINSTANCE(lpddoi->dwType);
    axis->name = lib_stralloc(lpddoi->tszName);
    axis->dwOffs = lpddoi->dwOfs;

    //  Link axis into list for this joystick
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
    return DIENUM_CONTINUE;
}

static BOOL CALLBACK EnumJoyButtons(LPCDIDEVICEOBJECTINSTANCE lpddoi, LPVOID pvRef)
{
JoyButton *button;
JoyInfo *joy;

//    log_debug("Button, offset : %s %d", lpddoi->tszName, lpddoi->dwOfs);

    joy = (JoyInfo*)pvRef;

    //  Save info about button
    button = lib_malloc(sizeof(JoyButton));
    button->next = NULL;
    button->id = DIDFT_GETINSTANCE(lpddoi->dwType);
    button->name = lib_stralloc(lpddoi->tszName);
    button->dwOffs = lpddoi->dwOfs;

    //  Link button into list for this joystick
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

static void joystick_release_joysticks(void)
{
JoyInfo     *joystick;

    joystick = joystick_list;

    while (joystick != NULL) {
        JoyInfo *next;

        next = joystick->next;
        lib_free(joystick->name);
        joystick_release_axes(joystick->axes);
        joystick_release_buttons(joystick->buttons);
        lib_free(joystick);
        joystick = next;
    }
}

int joystick_di_open(int index, int dev)
{
JoyInfo *joy;
int     i = 0;

    joy = joystick_list;
    while (joy && i < dev - JOYDEV_HW1) {
        joy = joy->next;
        i++;
    }
    if (joy) {
        IDirectInput_CreateDevice(di, &joy->guid, &joystick_di_devices[index], NULL);
        IDirectInputDevice_QueryInterface(joystick_di_devices[index], &IID_IDirectInputDevice2, (LPVOID*)&joystick_di_devices2[index]);
        IDirectInputDevice_SetDataFormat(joystick_di_devices[index], &c_dfDIJoystick);
        IDirectInputDevice_SetCooperativeLevel(joystick_di_devices[index], ui_active_window, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);
        IDirectInputDevice_Acquire(joystick_di_devices[index]);
        if (joy->axes) {
            joystick_release_axes(joy->axes);
            joy->axes = NULL;
        }
        IDirectInputDevice_EnumObjects(joystick_di_devices[index], EnumJoyAxes, (LPVOID)joy, DIDFT_AXIS);
        if (joy->buttons) {
            joystick_release_buttons(joy->buttons);
            joy->buttons = NULL;
        }
        IDirectInputDevice_EnumObjects(joystick_di_devices[index], EnumJoyButtons, (LPVOID)joy, DIDFT_BUTTON);
        return 1;
    } else {
        return 0;
    }
}

void joystick_di_close(int index)
{
    if (joystick_di_devices[index]) {
        IDirectInputDevice_Unacquire(joystick_di_devices[index]);
        if (joystick_di_devices2[index]) IDirectInputDevice2_Release(joystick_di_devices2[index]);
        IDirectInputDevice_Release(joystick_di_devices[index]);
    }
    joystick_di_devices[index] = NULL;
    joystick_di_devices2[index] = NULL;
}


static int set_joystick_device_1(int val, void *param)
{
    joystick_device_t dev = (joystick_device_t)val;

    if (!joystick_inited) joy_arch_init();

    if ((di_version == 5) && (joystick_port_map[0] >= JOYDEV_HW1)) {
        joystick_di_close(0);
    }

    if ((di_version == 5) && (dev >= JOYDEV_HW1)) {
        if (joystick_di_open(0, dev)) {
            joystick_port_map[0] = dev;
        }
    } else {
        joystick_port_map[0] = dev;
    }

    return 0;
}

static int set_joystick_device_2(int val, void *param)
{
    joystick_device_t dev = (joystick_device_t)val;

    if (!joystick_inited) joy_arch_init();

    if ((di_version == 5) && (joystick_port_map[1] >= JOYDEV_HW1)) {
        joystick_di_close(1);
    }


    if ((di_version == 5) && (dev >= JOYDEV_HW1)) {
        if (joystick_di_open(1, dev)) {
            joystick_port_map[1] = dev;
        }
    } else {
        joystick_port_map[1] = dev;
    }

    return 0;
}

static int set_joystick_fire1_speed(int speed, void *param)
{
    if (speed < 1)
        speed = 1;
    if (speed > 32)
        speed = 32;

    joystick_fire_speed[0] = speed;

    return 0;
}

static int set_joystick_fire2_speed(int speed, void *param)
{
    if (speed < 1)
        speed = 1;
    if (speed > 32)
        speed = 32;

    joystick_fire_speed[1] = speed;

    return 0;
}

static int set_joystick_fire1_axis(int axis, void *param)
{
    if (axis < 0)
        axis = 0;

    joystick_fire_axis[0] = axis;

    return 0;
}

static int set_joystick_fire2_axis(int axis, void *param)
{
    if (axis < 0)
        axis = 0;

    joystick_fire_axis[1] = axis;

    return 0;
}

static int set_joystick_autofire1_button(int button, void *param)
{
    if (button < 0)
        button = 0;

    joystick_autofire_button[0] = button;

    return 0;
}

static int set_joystick_autofire2_button(int button, void *param)
{
    if (button < 0)
        button = 0;

    joystick_autofire_button[1] = button;

    return 0;
}

static int set_joystick_fire1_button(int button, void *param)
{
    if (button < 0)
        button = 0;

    joystick_fire_button[0] = button;

    return 0;
}

static int set_joystick_fire2_button(int button, void *param)
{
    if (button < 0)
        button = 0;

    joystick_fire_button[1] = button;

    return 0;
}



static const resource_int_t resources_int[] = {
    { "JoyDevice1", JOYDEV_NONE, RES_EVENT_NO, NULL,
      &joystick_port_map[0], set_joystick_device_1, NULL },
    { "JoyDevice2", JOYDEV_NONE, RES_EVENT_NO, NULL,
      &joystick_port_map[1], set_joystick_device_2, NULL },
    { "JoyAutofire1Speed", 16, RES_EVENT_NO, NULL,
      &joystick_fire_speed[0], set_joystick_fire1_speed, NULL },
    { "JoyAutofire1Axis", 0, RES_EVENT_NO, NULL,
      &joystick_fire_axis[0], set_joystick_fire1_axis, NULL },
    { "JoyAutofire1Button", 0, RES_EVENT_NO, NULL,
      &joystick_autofire_button[0], set_joystick_autofire1_button, NULL },
    { "JoyAutofire2Speed", 16, RES_EVENT_NO, NULL,
      &joystick_fire_speed[1], set_joystick_fire2_speed, NULL },
    { "JoyAutofire2Axis", 0, RES_EVENT_NO, NULL,
      &joystick_fire_axis[1], set_joystick_fire2_axis, NULL },
    { "JoyAutofire2Button", 0, RES_EVENT_NO, NULL,
      &joystick_autofire_button[1], set_joystick_autofire2_button, NULL },
    { "JoyFire1Button", 0, RES_EVENT_NO, NULL,
      &joystick_fire_button[0], set_joystick_fire1_button, NULL },
    { "JoyFire2Button", 0, RES_EVENT_NO, NULL,
      &joystick_fire_button[1], set_joystick_fire2_button, NULL },
    { NULL }
};

int joystick_arch_init_resources(void)
{
    return resources_register_int(resources_int);
}

/* ------------------------------------------------------------------------- */

static const cmdline_option_t cmdline_options[] = {
    { "-joydev1", SET_RESOURCE, 1, NULL, NULL,
      "JoyDevice1", NULL,
      IDS_P_NUMBER, IDS_SET_INPUT_JOYSTICK_1 },
    { "-joydev2", SET_RESOURCE, 1, NULL, NULL,
      "JoyDevice2", NULL,
      IDS_P_NUMBER, IDS_SET_INPUT_JOYSTICK_2 },
    { NULL }
};

int joystick_init_cmdline_options(void)
{
    return cmdline_register_options(cmdline_options);
}

/* ------------------------------------------------------------------------- */

int                         directinput_inited;
extern LPDIRECTINPUTDEVICE  di_mouse;
void mouse_set_format(void);

static BOOL CALLBACK EnumCallBack(LPCDIDEVICEINSTANCE lpddi, LPVOID pvref)
{
JoyInfo *new_joystick;

//    log_debug("ProductName: %s", lpddi->tszProductName);
//    log_debug("Instance Name: %s", lpddi->tszInstanceName);

    new_joystick = lib_malloc(sizeof(JoyInfo));
    new_joystick->next = NULL;
    memcpy(&new_joystick->guid, &lpddi->guidInstance, sizeof(GUID));
    new_joystick->name = lib_stralloc(lpddi->tszInstanceName);
    new_joystick->axes = NULL;
    new_joystick->buttons = NULL;

    if (joystick_list == NULL) {
        joystick_list = new_joystick;
    } else {
        JoyInfo *s;
        s = joystick_list;
        while (s->next != NULL) {
            s = s->next;
        }
        s->next = new_joystick;
    }
    return DIENUM_CONTINUE;
}

int joy_arch_init(void)
{
    HRESULT result;

    if (!joystick_inited) {
        di_version = 5;
        result = DirectInputCreate(winmain_instance, 0x0500, &di, NULL);
        if (result == DIERR_OLDDIRECTINPUTVERSION) {
            di_version = 3;
            result = DirectInputCreate(winmain_instance, 0x0300, &di, NULL);
        }
        if (result != DI_OK) {
            di = NULL;
            di_mouse = NULL;
            return 0;
        }
        if (di_version == 5 ) {
            IDirectInput_EnumDevices(di, DIDEVTYPE_JOYSTICK, EnumCallBack, NULL, DIEDFL_ALLDEVICES);
        }

        joystick_inited = 1;

        /*  Init mouse device */
        result = IDirectInput_CreateDevice(di, (GUID *)&GUID_SysMouse, &di_mouse,
                                           NULL);
        if (result != DI_OK) {
            di_mouse = NULL;
            return 0;
        }
        mouse_set_format();

    //    IDirectInput_EnumDevices(di, 0, EnumCallBack, 0, 0);

    }

    return 0;
}

int joystick_close(void)
{
    if (di_mouse != NULL) {
        IDirectInputDevice_Unacquire(di_mouse);
        IDirectInputDevice_Release(di_mouse);
        di_mouse = NULL;
    }
    if ((di_version == 5) && (joystick_port_map[0] >= JOYDEV_HW1)) {
        joystick_di_close(0);
    }
    if ((di_version == 5) && (joystick_port_map[1] >= JOYDEV_HW1)) {
        joystick_di_close(1);
    }
    joystick_release_joysticks();
    if (di != NULL)
        IDirectInput_Release(di);
    joystick_inited = 0;
    return 0;
}

JOYINFOEX   joy_info;
JOYCAPS     joy_caps;

static BYTE joystick_di5_update(int joy_no)
{
BYTE value;
int i;
DIPROPRANGE    prop;
UINT amin;
UINT amax;
DWORD apos;
DIJOYSTATE  js;
JoyInfo *joy;
JoyButton *button;
int     afire_button;
int		fire_button;

    value = 0;

    IDirectInputDevice2_Poll(joystick_di_devices2[joy_no]);
    IDirectInputDevice_GetDeviceState(joystick_di_devices[joy_no], sizeof(DIJOYSTATE), &js);

    //  Get boundary values for X axis
    prop.diph.dwSize = sizeof(DIPROPRANGE);
    prop.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    prop.diph.dwObj = 0;    // Offset of X axis
    prop.diph.dwHow = DIPH_BYOFFSET;
    IDirectInputDevice_GetProperty(joystick_di_devices[joy_no], DIPROP_RANGE, (DIPROPHEADER*)&prop);
    if (js.lX <= prop.lMin + (prop.lMax - prop.lMin) / 4) {
        value |= 4;
    }
    if (js.lX >= prop.lMin + (prop.lMax - prop.lMin) / 4 * 3) {
        value |= 8;
    }

    //  Get boundary values for Y axis
    prop.diph.dwSize = sizeof(DIPROPRANGE);
    prop.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    prop.diph.dwObj = 4;    // Offset of Y axis
    prop.diph.dwHow = DIPH_BYOFFSET;
    IDirectInputDevice_GetProperty(joystick_di_devices[joy_no], DIPROP_RANGE, (DIPROPHEADER*)&prop);
    if (js.lY <= prop.lMin + (prop.lMax - prop.lMin) / 4) {
        value |= 1;
    }
    if (js.lY >= prop.lMin + (prop.lMax - prop.lMin) / 4 * 3) {
        value |= 2;
    }

    //  Find the joystick object

    afire_button = -1;
	fire_button = -1;
    joy = joystick_list;
    i = 0;
    while (joy && i < joystick_port_map[joy_no] - JOYDEV_HW1) {
        joy = joy->next;
        i++;
    }
    if (joy && (joystick_autofire_button[joy_no] > 0)) {
        button = joy->buttons;
        i = 0;
        while (button && i < joystick_autofire_button[joy_no] - 1) {
            button = button->next;
            i++;
        }
        if (button) {
            afire_button = button->dwOffs - 48;
        }
    }
    if ((afire_button >= 32) || (afire_button < -1)) afire_button = -1;
	if (joy && (joystick_fire_button[joy_no] > 0)) {
        button = joy->buttons;
        i = 0;
        while (button && i < joystick_fire_button[joy_no] - 1) {
            button = button->next;
            i++;
        }
        if (button) {
            fire_button = button->dwOffs - 48;
        }
	}
    if ((fire_button >= 32) || (fire_button < -1)) fire_button = -1;

	//	If fire button is not in valid range [0..31] then it means every button is
	//	treated as fire button, otherwise the only one selected.
	if (fire_button != -1) {
		if ((fire_button != afire_button) && (js.rgbButtons[fire_button] & 0x80)) value |= 16;
	} else {
		for (i = 0; i < 32; i++) {
			if ((i != afire_button) && (js.rgbButtons[i] & 0x80)) value |= 16;
		}
	}
    if ((afire_button != -1) && (js.rgbButtons[afire_button] & 0x80)) {
        if (joystick_fire_axis[joy_no]) {
            amin = 0;
            amax = 32;
            apos = 16;
            if (joy) {
                //  Find axis
                JoyAxis *axis;

                axis = joy->axes;
                i = 0;
                while (axis && i < joystick_fire_axis[joy_no] - 1) {
                    axis = axis->next;
                    i++;
                }
                if (axis) {
                    //  Get boundary values for axis
                    prop.diph.dwSize = sizeof(DIPROPRANGE);
                    prop.diph.dwHeaderSize = sizeof(DIPROPHEADER);
                    prop.diph.dwObj = axis->id;
                    prop.diph.dwHow = DIPH_BYID;
                    IDirectInputDevice_GetProperty(joystick_di_devices[joy_no], DIPROP_RANGE, (DIPROPHEADER*)&prop);
                    amin = prop.lMin;
                    amax = prop.lMax;
                    apos = *(DWORD*)(((BYTE*)&js) + axis->dwOffs);
                }
            }
            value |= maincpu_clk / (((amin + apos) * 0x2000)
                    / (amax - amin) + 1) & 16;
        } else {
            value |= (maincpu_clk / (joystick_fire_speed[joy_no]
                    * 0x100)) & 16;
        }
    }
    return value;
}

void joystick_update(void)
{
    BYTE value;
    MMRESULT result;
    int idx;
    DWORD addflag;
    UINT amin;
    UINT amax;
    DWORD apos;
    int afire_button;
	int fire_button;
    int j;

    if (di_version == 5) {
        int i;
        for (i = 0; i < 2; i++) {
            if (joystick_port_map[i] >= JOYDEV_HW1) {
                joystick_set_value_absolute(i + 1, joystick_di5_update(i));
            }
        }
    } else {
        if ((idx = (joystick_port_map[0] == JOYDEV_HW1)
            ? 0 : ((joystick_port_map[1] == JOYDEV_HW1) ? 1 : -1)) != -1) {
            switch (joystick_fire_axis[idx]) {
            case 1:
                addflag = JOY_RETURNZ;
                break;
            case 2:
                addflag = JOY_RETURNV;
                break;
            case 3:
                addflag = JOY_RETURNU;
                break;
            case 4:
                addflag = JOY_RETURNR;
                break;
            default:
                addflag = 0;
            }
            joy_info.dwFlags = JOY_RETURNBUTTONS | JOY_RETURNCENTERED
                            | JOY_RETURNX | JOY_RETURNY | addflag;
            value = 0;
            joy_info.dwSize = sizeof(JOYINFOEX);
            result = joyGetPosEx(JOYSTICKID1, &joy_info);
            if (result == JOYERR_NOERROR) {
                result = joyGetDevCaps(JOYSTICKID1, &joy_caps, sizeof(JOYCAPS));
                if (result == JOYERR_NOERROR) {
                    if (joy_info.dwXpos <= joy_caps.wXmin
                        + (joy_caps.wXmax - joy_caps.wXmin) / 4) {
                        value |= 4;
                    }
                    if (joy_info.dwXpos >= joy_caps.wXmin
                        + (joy_caps.wXmax - joy_caps.wXmin) / 4 * 3) {
                        value |= 8;
                    }
                    if (joy_info.dwYpos <= joy_caps.wYmin
                        + (joy_caps.wYmax - joy_caps.wYmin) / 4) {
                        value |= 1;
                    }
                    if (joy_info.dwYpos >= joy_caps.wYmin
                        + (joy_caps.wYmax - joy_caps.wYmin) / 4 * 3) {
                        value |= 2;
                    }
                    afire_button = joystick_autofire_button[idx] - 1;
					fire_button = joystick_fire_button[idx] - 1;
					if (fire_button != -1) {
						if ((fire_button != afire_button) && (joy_info.dwButtons & (1 << fire_button))) value |= 16;
					} else {
						for (j = 0; j < 32; j++) {
							if ((j != afire_button) && (joy_info.dwButtons & (1 << j))) value |= 16;
						}
					}
                    if ((afire_button != -1) && (joy_info.dwButtons & (1 << afire_button))) {
                        if ((joystick_fire_axis[idx])
                            && (joy_info.dwFlags & addflag)) {
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
                            value |= maincpu_clk / (((amin + apos) * 0x2000)
                                    / (amax - amin) + 1) & 16;
                        } else {
                            value |= (maincpu_clk / (joystick_fire_speed[idx]
                                    * 0x100)) & 16;
                        }     
                    }
                    joystick_set_value_absolute(idx + 1, value);
                }
            }
        }
        if ((idx = (joystick_port_map[0] == JOYDEV_HW2)
            ? 0 : ((joystick_port_map[1] == JOYDEV_HW2) ? 1 : -1)) != -1) {
            switch(joystick_fire_axis[idx]) {
            case 1:
                addflag = JOY_RETURNZ;
                break;
            case 2:
                addflag = JOY_RETURNV;
                break;
            case 3:
                addflag = JOY_RETURNU;
                break;
            case 4:
                addflag = JOY_RETURNR;
                break;
            default: addflag = 0;
            }
            joy_info.dwFlags = JOY_RETURNBUTTONS | JOY_RETURNCENTERED
                            | JOY_RETURNX | JOY_RETURNY | addflag;
            value = 0;
            joy_info.dwSize = sizeof(JOYINFOEX);
            result = joyGetPosEx(JOYSTICKID2,&joy_info);
            if (result == JOYERR_NOERROR) {
                result = joyGetDevCaps(JOYSTICKID2, &joy_caps, sizeof(JOYCAPS));
                if (result == JOYERR_NOERROR) {
                    if (joy_info.dwXpos <= joy_caps.wXmin
                        + (joy_caps.wXmax - joy_caps.wXmin) / 4) {
                        value |= 4;
                    }
                    if (joy_info.dwXpos >= joy_caps.wXmin
                        + (joy_caps.wXmax - joy_caps.wXmin) / 4 * 3) {
                        value |= 8;
                    }
                    if (joy_info.dwYpos <= joy_caps.wYmin
                        + (joy_caps.wYmax - joy_caps.wYmin) / 4) {
                        value |= 1;
                    }
                    if (joy_info.dwYpos >= joy_caps.wYmin
                        + (joy_caps.wYmax - joy_caps.wYmin) / 4 * 3) {
                        value |= 2;
                    }
                    afire_button = joystick_autofire_button[idx] - 1;
					fire_button = joystick_fire_button[idx] - 1;
					if (fire_button != -1) {
						if ((fire_button != afire_button) && (joy_info.dwButtons & (1 << fire_button))) value |= 16;
					} else {
						for (j = 0; j < 32; j++) {
							if ((j != afire_button) && (joy_info.dwButtons & (1 << j))) value |= 16;
						}
					}
                    if ((afire_button != -1) && (joy_info.dwButtons & (1 << afire_button))) {
                        if ((joystick_fire_axis[idx])
                            && (joy_info.dwFlags & addflag)) {
                            switch (joystick_fire_axis[idx]) {
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
                            value |= maincpu_clk / (((amin + apos) * 0x2000)
                                    / (amax - amin) + 1) & 16;
                        } else {
                            value |= (maincpu_clk / (joystick_fire_speed[idx]
                                    * 0x100)) & 16;
                        }     
                    }
                    joystick_set_value_absolute(idx + 1, value);
                }
            }
        }
    }
}

void joystick_calibrate(HWND hwnd)
{
    IDirectInput_RunControlPanel(di,hwnd,0);
}


void joystick_ui_get_device_list(HWND joy_hwnd)
{
JoyInfo *joy;

    if (di_version == 5) {
        joy = joystick_list;
        while (joy) {
            SendMessage(joy_hwnd, CB_ADDSTRING, 0, (LPARAM)joy->name);
            joy = joy->next;
        }
    } else {
        SendMessage(joy_hwnd, CB_ADDSTRING, 0, (LPARAM)"PC joystick #1");
        SendMessage(joy_hwnd, CB_ADDSTRING, 0, (LPARAM)"PC joystick #2");
    }
}

void joystick_ui_get_autofire_axes(HWND joy_hwnd, int device)
{
JoyInfo *joy;
JoyAxis *axis;
int     i;

    if (di_version == 5) {
        device = device - JOYDEV_HW1;
        joy = joystick_list;
        i = 0;
        while (joy && i < device) {
            joy = joy->next;
            i++;
        }
        if (joy) {
            axis = joy->axes;
            while (axis) {
                SendMessage(joy_hwnd, CB_ADDSTRING, 0, (LPARAM)axis->name);
                axis = axis->next;
            }
        }
    } else {
        SendMessage(joy_hwnd, CB_ADDSTRING, 0, (LPARAM)"Z-axis");
        SendMessage(joy_hwnd, CB_ADDSTRING, 0, (LPARAM)"V-axis");
        SendMessage(joy_hwnd, CB_ADDSTRING, 0, (LPARAM)"U-axis");
        SendMessage(joy_hwnd, CB_ADDSTRING, 0, (LPARAM)"R-axis");
    }
}

void joystick_ui_get_autofire_buttons(HWND joy_hwnd, int device)
{
JoyInfo *joy;
JoyButton *button;
int     i;

    if (di_version == 5) {
        device = device - JOYDEV_HW1;
        joy = joystick_list;
        i = 0;
        while (joy && i < device) {
            joy = joy->next;
            i++;
        }
        if (joy) {
            button = joy->buttons;
            while (button) {
                SendMessage(joy_hwnd, CB_ADDSTRING, 0, (LPARAM)button->name);
                button = button->next;
            }
        }
    } else {
        SendMessage(joy_hwnd, CB_ADDSTRING, 0, (LPARAM)"Button 1");
        SendMessage(joy_hwnd, CB_ADDSTRING, 0, (LPARAM)"Button 2");
        SendMessage(joy_hwnd, CB_ADDSTRING, 0, (LPARAM)"Button 3");
        SendMessage(joy_hwnd, CB_ADDSTRING, 0, (LPARAM)"Button 4");
        SendMessage(joy_hwnd, CB_ADDSTRING, 0, (LPARAM)"Button 5");
        SendMessage(joy_hwnd, CB_ADDSTRING, 0, (LPARAM)"Button 6");
        SendMessage(joy_hwnd, CB_ADDSTRING, 0, (LPARAM)"Button 7");
        SendMessage(joy_hwnd, CB_ADDSTRING, 0, (LPARAM)"Button 8");
    }
}
