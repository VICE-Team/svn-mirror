/*
 * macosx/joy.c - Mac OS X joystick support.
 *
 * Written by
 *   Christian Vogelgsang <C.Vogelgsang@web.de>
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

#define JOY_INTERNAL

#include "vice.h"

#include "cmdline.h"
#include "joy.h"
#include "joystick.h"
#include "keyboard.h"
#include "log.h"
#include "resources.h"
#include "types.h"
#include "util.h"

/* (Used by `kbd.c').  */
int joystick_port_map[2] = { JOYDEV_NONE,JOYDEV_NONE };

#ifdef HAS_JOYSTICK

/* ----- Static Data ------------------------------------------------------ */

static int joy_done_init = 0;

/* axis map */
axis_map_t joy_axis_map[] = {
  { "X",kHIDUsage_GD_X },
  { "Y",kHIDUsage_GD_Y },
  { "Z",kHIDUsage_GD_Z },
  { "Rx",kHIDUsage_GD_Rx },
  { "Ry",kHIDUsage_GD_Ry },
  { "Rz",kHIDUsage_GD_Rz },
};

/* the driver holds up to two USB joystick definitions */
joystick_descriptor_t joy_a = { NULL,NULL,NULL,NULL,0,0 };
joystick_descriptor_t joy_b = { NULL,NULL,NULL,NULL,0,0 };

/* ----- VICE Resources --------------------------------------------------- */

static void setup_axis_mapping(joystick_descriptor_t *joy);
static void setup_button_mapping(joystick_descriptor_t *joy);
static void assign_joysticks_from_device_list(void);

static int joyport1select(int val, void *param)
{
  joystick_port_map[0] = val;
  return 0;
}

static int joyport2select(int val, void *param)
{
  joystick_port_map[1] = val;
  return 0;
}

static int set_joy_a_device_name(const char *val,void *param)
{
  util_string_set(&joy_a.device_name,val);
  if(joy_done_init)
    assign_joysticks_from_device_list();
  return 0;
}

static int set_joy_a_x_axis_name(const char *val,void *param)
{
  util_string_set(&joy_a.x_axis_name,val);
  if(joy_done_init)
    setup_axis_mapping(&joy_a);
  return 0;
}

static int set_joy_a_y_axis_name(const char *val,void *param)
{
  util_string_set(&joy_a.y_axis_name,val);
  if(joy_done_init)
    setup_axis_mapping(&joy_a);
  return 0;
}

static int set_joy_a_button_mapping(const char *val,void *param)
{
  util_string_set(&joy_a.button_mapping,val);
  if(joy_done_init)
    setup_button_mapping(&joy_a);
  return 0;
}

static int set_joy_a_x_threshold(int val, void *param)
{
  joy_a.x_threshold = val;
  if(joy_done_init)
    setup_axis_mapping(&joy_a);
  return 0;
}

static int set_joy_a_y_threshold(int val, void *param)
{
  joy_a.y_threshold = val;
  if(joy_done_init)
    setup_axis_mapping(&joy_a);
  return 0;
}

static int set_joy_b_device_name(const char *val,void *param)
{
  util_string_set(&joy_b.device_name,val);
  if(joy_done_init)
    assign_joysticks_from_device_list();
  return 0;
}

static int set_joy_b_x_axis_name(const char *val,void *param)
{
  util_string_set(&joy_b.x_axis_name,val);
  if(joy_done_init)
    setup_axis_mapping(&joy_b);
  return 0;
}

static int set_joy_b_y_axis_name(const char *val,void *param)
{
  util_string_set(&joy_b.y_axis_name,val);
  if(joy_done_init)
    setup_axis_mapping(&joy_b);
  return 0;
}

static int set_joy_b_button_mapping(const char *val,void *param)
{
  util_string_set(&joy_b.button_mapping,val);
  if(joy_done_init)
    setup_button_mapping(&joy_a);
  return 0;
}

static int set_joy_b_x_threshold(int val, void *param)
{
  joy_b.x_threshold = val;
  if(joy_done_init)
    setup_axis_mapping(&joy_b);
  return 0;
}

static int set_joy_b_y_threshold(int val, void *param)
{
  joy_b.y_threshold = val;
  if(joy_done_init)
    setup_axis_mapping(&joy_b);
  return 0;
}

static const resource_string_t resources_string[] = {
  { "JoyADevice", "", RES_EVENT_NO, NULL,
    &joy_a.device_name, set_joy_a_device_name, NULL },
  { "JoyAXAxis", "X", RES_EVENT_NO, NULL,
    &joy_a.x_axis_name, set_joy_a_x_axis_name, NULL },
  { "JoyAYAxis", "Y", RES_EVENT_NO, NULL,
    &joy_a.y_axis_name, set_joy_a_y_axis_name, NULL },
  { "JoyAButtons", "1:2:0:0:0:0", RES_EVENT_NO, NULL,
    &joy_a.button_mapping, set_joy_a_button_mapping, NULL },

  { "JoyBDevice", "", RES_EVENT_NO, NULL,
    &joy_b.device_name, set_joy_b_device_name, NULL },
  { "JoyBXAxis", "X", RES_EVENT_NO, NULL,
    &joy_b.x_axis_name, set_joy_b_x_axis_name, NULL },
  { "JoyBYAxis", "Y", RES_EVENT_NO, NULL,
    &joy_b.y_axis_name, set_joy_b_y_axis_name, NULL },
  { "JoyBButtons", "1:2:0:0:0:0", RES_EVENT_NO, NULL,
    &joy_b.button_mapping, set_joy_b_button_mapping, NULL },

  { NULL }
};

static const resource_int_t resources_int[] = {
  { "JoyDevice1", 0, RES_EVENT_NO, NULL,
    &joystick_port_map[0], joyport1select, NULL },
  { "JoyDevice2", 0, RES_EVENT_NO, NULL,
    &joystick_port_map[1], joyport2select, NULL },

  { "JoyAXThreshold", 50, RES_EVENT_NO, NULL,
    &joy_a.x_threshold, set_joy_a_x_threshold, NULL },
  { "JoyAYThreshold", 50, RES_EVENT_NO, NULL,
    &joy_a.y_threshold, set_joy_a_y_threshold, NULL },

  { "JoyBXThreshold", 50, RES_EVENT_NO, NULL,
    &joy_b.x_threshold, set_joy_b_x_threshold, NULL },
  { "JoyBYThreshold", 50, RES_EVENT_NO, NULL,
    &joy_b.y_threshold, set_joy_b_y_threshold, NULL },

  { NULL }
};

/* ----- VICE Command-line options ----- */

static const cmdline_option_t cmdline_options[] = {
  { "-joydev1", SET_RESOURCE, 1, NULL, NULL, "JoyDevice1", NULL,
    "<0-5>", N_("Set device for joystick port 1") },
  { "-joydev2", SET_RESOURCE, 1, NULL, NULL, "JoyDevice2", NULL,
    "<0-5>", N_("Set device for joystick port 2") },

  { "-joyAdevice", SET_RESOURCE, 1, NULL, NULL, "JoyADevice", NULL,
    "<vid:pid:sn>", N_("Set HID A device") },
  { "-joyAxaxis", SET_RESOURCE, 1, NULL, NULL, "JoyAXAxis", NULL,
    "<X,Y,Z,Rx,Ry,Rz>", N_("Set X Axis for HID A device") },
  { "-joyAyaxis", SET_RESOURCE, 1, NULL, NULL, "JoyAYAxis", NULL,
    "<X,Y,Z,Rx,Ry,Rz>", N_("Set Y Axis for HID A device") },
  { "-joyAbuttons", SET_RESOURCE, 1, NULL, NULL, "JoyAButtons", NULL,
    "<f:af:l:r:u:d>", N_("Set Y Axis for HID A device") },
  { "-joyAxthreshold", SET_RESOURCE, 1, NULL, NULL, "JoyAXThreshold", NULL,
    "<0-100>", N_("Set X Axis Threshold in Percent of HID A device") },
  { "-joyAythreshold", SET_RESOURCE, 1, NULL, NULL, "JoyAYThreshold", NULL,
    "<0-100>", N_("Set Y Axis Threshold in Percent of HID A device") },

  { "-joyBdevice", SET_RESOURCE, 1, NULL, NULL, "JoyBDevice", NULL,
    "<vid:pid:sn>", N_("Set HID B device") },
  { "-joyBxaxis", SET_RESOURCE, 1, NULL, NULL, "JoyBXAxis", NULL,
    "<X,Y,Z,Rx,Ry,Rz>", N_("Set X Axis for HID B device") },
  { "-joyByaxis", SET_RESOURCE, 1, NULL, NULL, "JoyBYAxis", NULL,
    "<X,Y,Z,Rx,Ry,Rz>", N_("Set Y Axis for HID B device") },
  { "-joyBbuttons", SET_RESOURCE, 1, NULL, NULL, "JoyBButtons", NULL,
    "<f:af:l:r:u:d>", N_("Set Y Axis for HID B device") },
  { "-joyBxthreshold", SET_RESOURCE, 1, NULL, NULL, "JoyBXThreshold", NULL,
    "<0-100>", N_("Set X Axis Threshold in Percent of HID B device") },
  { "-joyBythreshold", SET_RESOURCE, 1, NULL, NULL, "JoyBYThreshold", NULL,
    "<0-100>", N_("Set Y Axis Threshold in Percent of HID B device") },

  { NULL },
};

int joystick_arch_init_resources(void)
{
  int ok = resources_register_string(resources_string);
  if(ok<0)
    return ok;
  return resources_register_int(resources_int);
}

int joystick_init_cmdline_options(void)
{
  return cmdline_register_options(cmdline_options);
}

/* ----- Tool Functions --------------------------------------------------- */

static int find_axis_tag(const char *name,int def)
{
  if(name==NULL)
    return def;
  int i;
  for(i=0;i<JOYSTICK_DESCRIPTOR_MAX_AXIS;i++) {
    axis_map_t *ptr = &joy_axis_map[i];
    if(strcmp(ptr->name,name)==0)
      return ptr->tag;
  }
  return def;
}

const char *find_axis_name(int tag)
{
  int i;
  for(i=0;i<JOYSTICK_DESCRIPTOR_MAX_AXIS;i++) {
    axis_map_t *ptr = &joy_axis_map[i];
    if(ptr->tag == tag)
      return ptr->name;
  }
  return NULL;
}

static pRecElement find_axis_element(joystick_descriptor_t *joy,int tag)
{
  int i;
  for(i=0;i<joy->num_axis;i++) {
    pRecElement elem = joy->axis[i];
    if(elem->usage==tag)
      return elem;
  }
  return NULL;
}

static pRecElement find_button_element(joystick_descriptor_t *joy,int id)
{
  int i;
  for(i=0;i<joy->num_buttons;i++) {
    pRecElement elem = joy->buttons[i];
    if(elem->usage==id)
      return elem;
  }
  return NULL;  
}

/* ----- Setup Joystick Descriptor ---------------------------------------- */

static void build_joystick_button_axis_lists(joystick_descriptor_t *joy)
{
  pRecElement element;

  joy->num_buttons = 0;
  joy->num_axis = 0;

  for(element = HIDGetFirstDeviceElement(joy->device, kHIDElementTypeInput); 
      element != NULL;
      element = HIDGetNextDeviceElement(element, kHIDElementTypeInput)) {
    /* axis elements */
    if(element->usagePage == kHIDPage_GenericDesktop) {
      const char *name = find_axis_name(element->usage);
      if(name!=NULL) {
        if(joy->num_axis==JOYSTICK_DESCRIPTOR_MAX_AXIS) {
          log_message(LOG_DEFAULT,"may_joy: TOO MANY AXIS FOUND!");
        } else {
          joy->axis[joy->num_axis] = element;
          joy->num_axis++;
        }
      }
    }
    /* button elements */
    else if(element->usagePage == kHIDPage_Button) {
      if(element->usage >= 1) {
        if(joy->num_buttons==JOYSTICK_DESCRIPTOR_MAX_BUTTONS) {
          log_message(LOG_DEFAULT,"mac_joy: TOO MANY BUTTONS FOUND!");
        } else {
          joy->buttons[joy->num_buttons] = element;
          joy->num_buttons++;
        }
      }
    }
  }
}

static void setup_axis_calibration(pRecElement element,calibration_t *calib,
                                   int threshold,const char *desc)
{
  const char *name = find_axis_name(element->usage);
  int min = element->min;
  int max = element->max;
  int range = max - min;;
  int safe  = range * threshold / 200;
  int t_min = min + safe;
  int t_max = max - safe;
  log_message(LOG_DEFAULT,
    "mac_joy: %s axis: mapped to %s with range=[%d;%d] and null=[%d;%d] from threshold=%d%%",
    desc,name,min,max,t_min,t_max,threshold);
  calib->min_threshold = t_min;
  calib->max_threshold = t_max;
}

static void setup_axis_mapping(joystick_descriptor_t *joy)
{
  /* extract tag from resource */
  int x_axis_id = find_axis_tag(joy->x_axis_name,-1);
  int y_axis_id = find_axis_tag(joy->y_axis_name,-1);

  /* find element in current device */
  joy->x_axis = find_axis_element(joy,x_axis_id);
  joy->y_axis = find_axis_element(joy,y_axis_id);

  /* setup calibration for axis */
  if(joy->x_axis) {
    setup_axis_calibration(joy->x_axis,&joy->x_calib,joy->x_threshold,"horizontal");
  } else {
    log_message(LOG_DEFAULT,"mac_joy: horizontal axis not mapped!");
  }
  if(joy->y_axis) {
    setup_axis_calibration(joy->y_axis,&joy->y_calib,joy->y_threshold,"vertical");
  } else {
    log_message(LOG_DEFAULT,"mac_joy: vertical axis not mapped!");
  }
}

static void setup_button_mapping(joystick_descriptor_t *joy)
{
  /* setup button mapping */
  int i;
  int ids[HID_NUM_BUTTONS] = { 1,2,3,4,5,6 };
  if(joy->button_mapping && strlen(joy->button_mapping)>0) {
    if(sscanf(joy->button_mapping,"%d:%d:%d:%d:%d:%d",
              &ids[0],&ids[1],&ids[2],&ids[3],&ids[4],&ids[5])!=6)
      return;
  }
  
  for(i=0;i<HID_NUM_BUTTONS;i++)
    joy->mapped_buttons[i]  = find_button_element(joy,ids[i]);
  
  log_message(LOG_DEFAULT,"mac_joy: buttons: fire=%ld alt_fire=%ld left=%ld right=%ld up=%ld down=%ld",
    joy->mapped_buttons[HID_FIRE]      ? joy->mapped_buttons[HID_FIRE]->usage : 0,
    joy->mapped_buttons[HID_ALT_FIRE]  ? joy->mapped_buttons[HID_ALT_FIRE]->usage : 0,
    joy->mapped_buttons[HID_LEFT]      ? joy->mapped_buttons[HID_LEFT]->usage : 0,
    joy->mapped_buttons[HID_RIGHT]     ? joy->mapped_buttons[HID_RIGHT]->usage : 0,
    joy->mapped_buttons[HID_UP]        ? joy->mapped_buttons[HID_UP]->usage : 0,
    joy->mapped_buttons[HID_DOWN]      ? joy->mapped_buttons[HID_DOWN]->usage : 0);
}

static void setup_joystick(joystick_descriptor_t *joy,pRecDevice device,const char *desc)
{
  log_message(LOG_DEFAULT,"mac_joy: setting up %s HID joystick",desc);
  
  joy->device = device;
  build_joystick_button_axis_lists(joy);
  setup_axis_mapping(joy);
  setup_button_mapping(joy);
}

/* ---------- Query Joystick Device --------------------------------------- */

/* count devices with same vid:pid */
int get_device_serial(pRecDevice last_device)
{
  int count = 0;
  pRecDevice device;

  /* iterate through all devices */
  for(device = HIDGetFirstDevice();device != last_device;device = HIDGetNextDevice(device)) {
    if((device->vendorID==last_device->vendorID) && 
       (device->productID==last_device->productID)) {
      count++;
    }
  }
  return count;
}

/* determine if the given device matches the joystick descriptor */
static int match_joystick(joystick_descriptor_t *joy,pRecDevice device,int serial_num)
{
  /* match by device name */
  if(joy->device_name && strlen(joy->device_name)>0) {
    int vid,pid;
    int want_serial;
    if(sscanf(joy->device_name,"%x:%x:%d",&vid,&pid,&want_serial)!=3) {
      return 0;
    }
    return (vid == device->vendorID) && (pid == device->productID) &&
           (serial_num == want_serial);
  }
  /* no match */
  return 0;
}

/* is the joystick auto assignable? */
static int auto_assign_joystick(joystick_descriptor_t *joy)
{
  return ( (joy->device_name== NULL) || (strlen(joy->device_name)==0) );
}

static void assign_joysticks_from_device_list(void)
{ 
  pRecDevice device;
  int num_joysticks = 0;
  int auto_assign_a = auto_assign_joystick(&joy_a);
  int auto_assign_b = auto_assign_joystick(&joy_b);

  /* reset both joysticks */
  joy_a.device = NULL;
  joy_b.device = NULL;
  
  /* iterate through all devices */
  for(device = HIDGetFirstDevice();device != NULL;device = HIDGetNextDevice(device)) {
    
    /* check if its a joystick or game pad device */
    if((device->usage == kHIDUsage_GD_Joystick) ||
       (device->usage == kHIDUsage_GD_GamePad)) {
         
      int serial_num = get_device_serial(device);

      log_message(LOG_DEFAULT,"mac_joy: found #%d joystick/gamepad: %04lx:%04lx:%d %s",
        num_joysticks,device->vendorID,device->productID,serial_num,device->product);

      /* query joy A */
      if(!auto_assign_a && match_joystick(&joy_a,device,serial_num)) {
        setup_joystick(&joy_a,device,"matched A");
      }
      /* query joy B */
      else if(!auto_assign_b && match_joystick(&joy_b,device,serial_num)) {
        setup_joystick(&joy_b,device,"matched B");
      }
      /* auto assign a */
      else if(auto_assign_a && (joy_a.device == NULL)) {
        setup_joystick(&joy_a,device,"auto-assigned A");
      }
      /* auto assign b */
      else if(auto_assign_b && (joy_b.device == NULL)) {
        setup_joystick(&joy_b,device,"auto-assigned B");
      }

      num_joysticks++;
    }
  }
  
  /* check if matched */
  if(!auto_assign_a && (joy_a.device==NULL)) {
    log_message(LOG_DEFAULT,"mac_joy: joystick A not matched!");
  }
  if(!auto_assign_b && (joy_b.device==NULL)) {
    log_message(LOG_DEFAULT,"mac_joy: joystick B not matched!");
  }
}

int build_device_list(pRecDevice **devices)
{
  int count = 0;
  int i=0;
  pRecDevice device;
  
  /* iterate through all devices */
  for(device = HIDGetFirstDevice();device != NULL;device = HIDGetNextDevice(device)) {
    
    /* check if its a joystick or game pad device */
    if((device->usage == kHIDUsage_GD_Joystick) ||
       (device->usage == kHIDUsage_GD_GamePad)) {
      count++;
    }
  }
  
  if(count==0)
    return 0;

  *devices = (pRecDevice *)malloc(sizeof(pRecDevice)*count);
  
  /* iterate through all devices */
  for(device = HIDGetFirstDevice();device != NULL;device = HIDGetNextDevice(device)) {
    
    /* check if its a joystick or game pad device */
    if((device->usage == kHIDUsage_GD_Joystick) ||
       (device->usage == kHIDUsage_GD_GamePad)) {
      (*devices)[i++] = device;
    }
  }
  
  return count;
}

static int load_device_list(void)
{
  int num_devices;

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
  
  return num_devices;
}

static void unload_device_list(void)
{
  /* cleanup device list */
  HIDReleaseDeviceList();  
}

void reload_device_list(void)
{
  unload_device_list();
  if(load_device_list()>0) {
    assign_joysticks_from_device_list();
  }
}

/* ---------- Read Joystick Device Values --------------------------------- */

static BYTE read_button_element(pRecDevice device,pRecElement element,BYTE value)
{
  if(element==NULL)
    return 0;
  if(!HIDIsValidElement(device,element))
    return 0;

  if(HIDGetElementValue(device, element)>0)
    return value;
  else
    return 0;
}

static BYTE read_axis_element(pRecDevice device,pRecElement element,
                              calibration_t *calib,
                              BYTE min,BYTE max)
{
  if(element==NULL)
    return 0;
  if(!HIDIsValidElement(device,element))
    return 0;
  
  int value = HIDGetElementValue(device, element);
  if(value < calib->min_threshold)
    return min;
  else if(value > calib->max_threshold)
    return max;
  else
    return 0;
}

static BYTE read_joystick(joystick_descriptor_t *joy)
{
  pRecDevice device = joy->device;

  /* read buttons */
  BYTE joy_bits = read_button_element(device,joy->mapped_buttons[HID_FIRE],16)
                | read_button_element(device,joy->mapped_buttons[HID_ALT_FIRE],16)
                | read_button_element(device,joy->mapped_buttons[HID_LEFT],4)
                | read_button_element(device,joy->mapped_buttons[HID_RIGHT],8)
                | read_button_element(device,joy->mapped_buttons[HID_UP],1)
                | read_button_element(device,joy->mapped_buttons[HID_DOWN],2);
                
  /* axis */
  joy_bits |= read_axis_element(device,joy->x_axis,&joy->x_calib,4,8)
            | read_axis_element(device,joy->y_axis,&joy->y_calib,1,2);

  return joy_bits;
}

/* ---------- Detect ------------------------------------------------------ */

extern int detect_axis(joystick_descriptor_t *joy,int x_axis)
{
  int i;
  for(i=0;i<joy->num_axis;i++) {
    pRecElement element = joy->axis[i];
    if(read_axis_element(joy->device,element,x_axis ? &joy->x_calib : &joy->y_calib,1,1)==1) {
      return element->usage;
    }
  }
  return 0;
}

extern int detect_button(joystick_descriptor_t *joy)
{
  int i;
  for(i=0;i<joy->num_buttons;i++) {
    pRecElement element = joy->buttons[i];
    if(read_button_element(joy->device,element,1)==1) {
      return element->usage;
    }
  }
  return 0;
}

/* ----- Vice Interface ---------------------------------------------------- */

/* query for available joysticks and set them up */
int joy_arch_init(void)
{
  if(load_device_list()==0)
    return 0;
  
  /* now assign HID joystick A,B if available */
  assign_joysticks_from_device_list();
  
  joy_done_init = 1;
  
  return 0;
}

/* close the device */
void joystick_close(void)
{
  unload_device_list();
}

/* poll joystick */
void joystick(void)
{
  int i;
  
  /* handle both virtual cbm joystick ports */
  for(i=0;i<2;i++) {
    /* what kind of device is connected to the virtual port? */ 
    int joy_port = joystick_port_map[i];
    
    /* is HID joystick A mapped? */
    if(joy_port == JOYDEV_HID_0) {
      if(joy_a.device!=NULL) {
        BYTE joy_bits = read_joystick(&joy_a);
        joystick_set_value_absolute(i+1,joy_bits);
      }
    }
    /* is HID joystick B mapped? */
    else if(joy_port == JOYDEV_HID_1) {
      if(joy_b.device!=NULL) {
        BYTE joy_bits = read_joystick(&joy_b);
        joystick_set_value_absolute(i+1,joy_bits);
      }
    }
  }
}

#endif /* HAS_JOYSTICK */
