/*
 * joy.h - Joystick support for Mac OS X.
 *
 * Written by
 *  Christian Vogelgsang <chris@vogelgsang.org>
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

#ifndef VICE_JOY_H
#define VICE_JOY_H

#define JOYDEV_NONE         0
#define JOYDEV_NUMPAD       1
#define JOYDEV_KEYSET1      2
#define JOYDEV_KEYSET2      3
#define JOYDEV_HID_0        4
#define JOYDEV_HID_1        5

/* obsolete fallbacks for X11/Gtk */
#define JOYDEV_ANALOG_0     4
#define JOYDEV_ANALOG_1     5
#define JOYDEV_ANALOG_2     6
#define JOYDEV_ANALOG_3     7
#define JOYDEV_ANALOG_4     8
#define JOYDEV_ANALOG_5     9

#define JOYSTICK_DESCRIPTOR_MAX_BUTTONS  32
#define JOYSTICK_DESCRIPTOR_MAX_AXIS     6

#define HID_FIRE        0
#define HID_ALT_FIRE    1
#define HID_LEFT        2
#define HID_RIGHT       3
#define HID_UP          4
#define HID_DOWN        5
#define HID_NUM_BUTTONS 6

/* virtual joystick mapping */ 
extern int joystick_port_map[2];

#ifdef HAS_JOYSTICK

#ifndef JOY_INTERNAL
typedef void * pRecDevice;
typedef void * pRecElement;
#else
/* NOTE: We use the HID Utilites Library provided by Apple for free

   http://developer.apple.com/samplecode/HID_Utilities_Source/index.html

   Make sure to install this (static) library first!
*/
#include <Carbon/Carbon.h>
#include <IOKit/hid/IOHIDKeys.h>
#include <IOKit/hid/IOHIDUsageTables.h>
#include "HID_Utilities_External.h"
#endif

/* axis map */
struct axis_map
{
  const char *name;
  int tag;
};
typedef struct axis_map axis_map_t;

extern axis_map_t joy_axis_map[];

/* calibration data for an axis */
struct calibration
{
  int  min_threshold;
  int  max_threshold;
};
typedef struct calibration calibration_t;

/* describe a joystick HID device */
struct joystick_descriptor 
{
  /* resources/settings for joystick */
  char         *device_name;      /* device name: vid:pid:num */
  
  char         *x_axis_name;      /* set x axis */
  char         *y_axis_name;      /* sety y axis */
    
  char         *button_mapping;   /* set button mapping */
  int          x_threshold;
  int          y_threshold;
  
  /* filled in from HID utils after setup */
  pRecDevice   device;
  
  pRecElement  x_axis;
  pRecElement  y_axis;
  
  pRecElement  mapped_buttons[HID_NUM_BUTTONS];
  
  /* fill list of all buttons and axis */
  int          num_buttons;
  pRecElement  buttons[JOYSTICK_DESCRIPTOR_MAX_BUTTONS];
  int          num_axis;
  pRecElement  axis[JOYSTICK_DESCRIPTOR_MAX_AXIS];
  
  /* calibration */
  calibration_t x_calib;
  calibration_t y_calib;
};
typedef struct joystick_descriptor joystick_descriptor_t;

/* access HID joystick A,B */
extern joystick_descriptor_t joy_a;
extern joystick_descriptor_t joy_b;

/* functions */
extern int joy_arch_init(void);
extern void joystick_close(void);
extern void joystick(void);

/* reload device list */
extern void reload_device_list(void);
/* build device list */
extern int build_device_list(pRecDevice **devices);
/* get serial of device */
extern int get_device_serial(pRecDevice last_device);

/* detect axis */
extern int detect_axis(joystick_descriptor_t *joy,int x_axis);
/* detect button */
extern int detect_button(joystick_descriptor_t *joy);
/* find axis name */
extern const char *find_axis_name(int tag);

#endif

#endif

