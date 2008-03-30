/*
 * joy_mac.c - Mac OS X joystick support.
 *
 * Written by
 *   Christian Vogelsang <C.Vogelgsang@web.de>
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


/* NOTE: We use the HID Utilites Library provided by Apple for free

   http://developer.apple.com/samplecode/HID_Utilities_Source/index.html

   Make sure to install this (static) library first!
*/
#include <Carbon/Carbon.h>
#include <IOKit/hid/IOHIDKeys.h>
#include <IOKit/hid/IOHIDUsageTables.h>
#include "HID_Utilities_External.h"

/* ----- Static Data ------------------------------------------------------- */

#define MAX_MAC_JOYSTICK 2
#define MAX_MAC_BUTTONS  4
/* combine all relevant info we need to know about a HID joystick in a single
   struct */
struct joystick_descriptor 
{
  pRecDevice   device;
  pRecElement  x_axis;
  pRecElement  y_axis;
  pRecElement  buttons[MAX_MAC_BUTTONS];
  int          num_buttons;
  long         x_min,x_max;
  long         y_min,y_max;
};
typedef struct joystick_descriptor joystick_descriptor_t;

static int num_joysticks = 0;
static joystick_descriptor_t joysticks[MAX_MAC_JOYSTICK];

/* ----- Tool Functions ---------------------------------------------------- */

static int setup_descriptor(joystick_descriptor_t *desc,pRecDevice device)
{
  pRecElement element;
  int i;
  int num_buttons = 0;
  
  desc->device = device;
  desc->x_axis = NULL;
  desc->y_axis = NULL;
  for(i=0;i<MAX_MAC_BUTTONS;i++) {
    desc->buttons[i] = NULL;
  }

  /* browse through all HIDInput elements of the device */
  for(element = HIDGetFirstDeviceElement(device, kHIDElementTypeInput); 
      element != NULL;
      element = HIDGetNextDeviceElement(element, kHIDElementTypeInput)) {
      
    /* axis elements */
    if(element->usagePage == kHIDPage_GenericDesktop) {
      if(element->usage == kHIDUsage_GD_X) {
        /* calc value range */
        long valid_range = (element->userMax - element->userMin) / 3;
        desc->x_min  = element->userMin + valid_range;
        desc->x_max  = element->userMax - valid_range;
        desc->x_axis = element;
      }
      else if(element->usage == kHIDUsage_GD_Y) {
        /* calc value range */
        long valid_range = (element->userMax - element->userMin) / 3;
        desc->y_min  = element->userMin + valid_range;
        desc->y_max  = element->userMax - valid_range;
        desc->y_axis = element;
      }
    }
    /* button elements */
    else if(element->usagePage == kHIDPage_Button) {
      if((element->usage >= 1) && (element->usage <= MAX_MAC_BUTTONS)) {
        desc->buttons[num_buttons++] = element;
      }
    }
  }
  desc->num_buttons = num_buttons;
  return 0;
}

/* ----- Vice Interface ---------------------------------------------------- */

/* query for available joysticks and set them up */
int joy_arch_init(void)
{
  int num_devices;
  pRecDevice device;

  /* build device list */
  HIDBuildDeviceList(kHIDPage_GenericDesktop,0);

  /* no device list? -> no joysticks! */
  if(!HIDHaveDeviceList()) {
    return 0;
  }
  
  /* get number of devices */
  num_devices = HIDCountDevices();
  if(num_devices==0) {
    return 0;
  }
  
  /* iterate through all devices */
  num_joysticks = 0;
  for(device = HIDGetFirstDevice();device != NULL;device = HIDGetNextDevice(device)) {
    
    /* check if its a joystick or game pad device */
    if((device->usage == kHIDUsage_GD_Joystick) ||
       (device->usage == kHIDUsage_GD_GamePad)) {
       
      /* store joystick */
      if(num_joysticks < MAX_MAC_JOYSTICK) {
        /* setup descriptor for this device */
        if(setup_descriptor(&joysticks[num_joysticks],device)==0) {
          num_joysticks++;
        }
      } else {
        printf("mac_joy: too many suitable devices found... skipping\n");
      }
    }    
  
  }
  printf("mac_joy: found %d joysticks/gamepads in %d HID devices\n",
         num_joysticks,num_devices);
  return 0;
}

/* close the device */
void joystick_close(void)
{
  /* cleanup device list */
  HIDReleaseDeviceList();
}

/* poll joystick */
void joystick(void)
{
  int i,j;
  int button_state;
  long x_axis_state;
  long y_axis_state;
  BYTE joy_bits;
  
  joystick_descriptor_t *desc;

  /* handle both virtual cbm joystick ports */
  for(i=0;i<2;i++) {
    /* what kind of device is connected to the virtual port? */ 
    int joy_port = joystick_port_map[i];
    
    /* is one of our joystick descriptors connected to it?
       we map the mac joysticks to analog 0 and analog 1.. in vice */
    int descriptor_pos = -1;
    if((joy_port == JOYDEV_ANALOG_0)&&(num_joysticks>0)) {
      descriptor_pos = 0;
    }
    else if((joy_port == JOYDEV_ANALOG_1)&&(num_joysticks>1)) {
      descriptor_pos = 1;
    }
    /* no valid mapping found - abort */
    if(descriptor_pos==-1)
      continue;
    
    /* access our joystick descriptor */
    desc = &joysticks[descriptor_pos];
    
    /* reset bits of cbm joystick */
    joy_bits = 0;
    
    /* read buttons: accumulate state of all valid buttons */
    button_state = 0;
    for(j=0;j<desc->num_buttons;j++) {
      if(HIDGetElementValue(desc->device, desc->buttons[j])>0) {
        button_state ++;
      }
    }
    if(button_state>0)
      joy_bits |= 16;
    
    /* read x axis */
    x_axis_state = HIDGetElementValue(desc->device, desc->x_axis); 
		x_axis_state = HIDCalibrateValue(x_axis_state, desc->x_axis);
		x_axis_state = HIDScaleValue(x_axis_state, desc->x_axis);
    if(x_axis_state < desc->x_min)
      joy_bits |= 4;
    else if(x_axis_state > desc->x_max)
      joy_bits |= 8;
 
    /* read y axis */
    y_axis_state = HIDGetElementValue(desc->device, desc->y_axis); 
		y_axis_state = HIDCalibrateValue(y_axis_state, desc->y_axis);
		y_axis_state = HIDScaleValue(y_axis_state, desc->y_axis);
    if(y_axis_state < desc->y_min)
      joy_bits |= 1;
    else if(y_axis_state > desc->y_max)
      joy_bits |= 2;

    /* set virtual joystick */
    joystick_set_value_absolute(i+1,joy_bits);
  }
}

