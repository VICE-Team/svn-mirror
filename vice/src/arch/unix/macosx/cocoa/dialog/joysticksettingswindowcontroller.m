/*
 * joysticksettingswindowcontroller.m - JoystickSettings dialog controller
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

#import "joysticksettingswindowcontroller.h"
#import "viceapplication.h"
#import "vicenotifications.h"

static char *keyNames[KEYSET_SIZE] = {
    "North",
    "South",
    "West",
    "East",
    "NorthWest",
    "SouthWest",
    "NorthEast",
    "SouthEast",
    "Fire"
};

@implementation JoystickSettingsWindowController

-(id)init
{
    self = [super initWithWindowNibName:@"JoystickSettings"];
    if(!self)
        return self;
    
    [self registerForResourceUpdate:@selector(updateResources:)];

    // force linkage of key press view
    [KeyPressView class];

    return self;
}

-(void)setupHidAxis
{
    [hidXAxis removeAllItems];
    [hidYAxis removeAllItems];

#ifdef HAS_JOYSTICK
    // fetch axis map from joy driver
    axis_map_t *am = joy_axis_map;
    while(am->name!=NULL) {
        NSString *name = [NSString stringWithCString:am->name encoding:NSUTF8StringEncoding];
        [hidXAxis addItemWithTitle:name];
        [hidYAxis addItemWithTitle:name];
        am++;
    }
#endif

    [hidXAxis addItemWithTitle:@"Disabled"];
    [hidYAxis addItemWithTitle:@"Disabled"];
}

-(void)setupHidDeviceList
{
    [hidName removeAllItems];
    [hidName addItemWithTitle:@"Autodetect"];

#ifdef HAS_JOYSTICK    
    pRecDevice *devices = NULL;
    int numDevices = build_device_list(&devices);
    if(numDevices>0) {
        int i;
        for(i=0;i<numDevices;i++) {
           pRecDevice dev = devices[i];
           int serial = get_device_serial(dev);
           NSString *desc = [NSString stringWithFormat:@"%04x:%04x:%d %s",
               dev->vendorID,dev->productID,serial,dev->product];
           [hidName addItemWithTitle:desc];
        }
        free(devices);
    }
#endif
}

-(void)windowDidLoad
{
    // define buttons array
    keyButtons[KEYSET_NORTH]     = keySetNorth;
    keyButtons[KEYSET_SOUTH]     = keySetSouth;
    keyButtons[KEYSET_WEST]      = keySetWest;
    keyButtons[KEYSET_EAST]      = keySetEast;
    keyButtons[KEYSET_NORTHWEST] = keySetNorthWest;
    keyButtons[KEYSET_SOUTHWEST] = keySetSouthWest;
    keyButtons[KEYSET_NORTHEAST] = keySetNorthEast;
    keyButtons[KEYSET_SOUTHEAST] = keySetSouthEast;
    keyButtons[KEYSET_FIRE]      = keySetFire;

    hidButtons[HID_FIRE]     = hidFire;
    hidButtons[HID_ALT_FIRE] = hidAltFire;
    hidButtons[HID_LEFT]     = hidWest;
    hidButtons[HID_RIGHT]    = hidEast;
    hidButtons[HID_UP]       = hidNorth;
    hidButtons[HID_DOWN]     = hidSouth;

    [self setupHidAxis];
    [self setupHidDeviceList];

    [self updateResources:nil];

    [super windowDidLoad];
}

// ---------- update ----------

-(void)updateKeysetDisplay
{
    // key set setup
    int keySetNum = [keySetSelect indexOfSelectedItem];
    int i;
    for(i=0;i<KEYSET_SIZE;i++) {
        NSString *res1 = [NSString stringWithFormat:@"KeySet%d%s",
                          keySetNum+1,keyNames[i]];
        int keyCode1 = [self getIntResource:res1];
        NSString *val1;
        if(keyCode1==-1)
            val1 = @"N/A";
        else
            val1 = [NSString stringWithFormat:@"%04x",keyCode1];
        [keyButtons[i] setTitle:val1];
    }
}

-(void)updateHidName
{
    int hidDeviceNum = [hidDeviceSelect indexOfSelectedItem];
    NSString *device = [self getStringResource:
         [NSString stringWithFormat:@"Joy%cDevice",'A'+hidDeviceNum]];

    if([device length]==0) {
        // select "automatic"
        [hidName selectItemAtIndex:0];
    } else {
        int pos;
        int found = -1;
        for(pos=0;pos<[hidName numberOfItems];++pos) {
            NSString *title = [hidName itemTitleAtIndex:pos];
            if([title hasPrefix:device]) {
                found = pos;
                break;
            }
        }
        if(found!=-1) {
            [hidName selectItemAtIndex:pos];
        } else {
            // reset to automatic
            NSLog(@"Reset Joy Device");
            [hidName selectItemAtIndex:0];
            [self setStringResource:
                 [NSString stringWithFormat:@"Joy%cDevice",'A'+hidDeviceNum] toValue:@""];
        }
    }
}

-(void)updateHidXAxis
{
    int hidDeviceNum = [hidDeviceSelect indexOfSelectedItem];

    NSString *axisName = [self getStringResource:
        [NSString stringWithFormat:@"Joy%cXAxis",'A'+hidDeviceNum]];
    [hidXAxis selectItemWithTitle:axisName];
    if([hidXAxis selectedItem]==nil)
        [hidXAxis selectItem:[hidXAxis lastItem]];

    int threshold = [self getIntResource:
        [NSString stringWithFormat:@"Joy%cXThreshold",'A'+hidDeviceNum]];
    [hidXThreshold setIntValue:threshold];
}

-(void)updateHidYAxis
{
    int hidDeviceNum = [hidDeviceSelect indexOfSelectedItem];

    NSString *axisName = [self getStringResource:
        [NSString stringWithFormat:@"Joy%cYAxis",'A'+hidDeviceNum]];
    [hidYAxis selectItemWithTitle:axisName];
    if([hidYAxis selectedItem]==nil)
        [hidYAxis selectItem:[hidYAxis lastItem]];

    int threshold = [self getIntResource:
        [NSString stringWithFormat:@"Joy%cYThreshold",'A'+hidDeviceNum]];
    [hidYThreshold setIntValue:threshold];
}

-(void)getHidButtons:(int *)ids
{
    int hidDeviceNum = [hidDeviceSelect indexOfSelectedItem];
    NSString *buttons = [self getStringResource:
         [NSString stringWithFormat:@"Joy%cButtons",'A'+hidDeviceNum]];
     const char *buttonsStr = [buttons cStringUsingEncoding:NSUTF8StringEncoding];
     sscanf(buttonsStr,"%d:%d:%d:%d:%d:%d",&ids[0],&ids[1],&ids[2],&ids[3],&ids[4],&ids[5]);
}

-(void)updateHidButtons
{
    int i;
    int ids[HID_NUM_BUTTONS] = { 0,0,0,0,0,0 };
    [self getHidButtons:ids];
    for(i=0;i<6;i++) {
        NSString *desc;
        if(ids[i]==0)
            desc = @"N/A";
        else
            desc = [NSString stringWithFormat:@"%d",ids[i]];
        [hidButtons[i] setTitle:desc];
    }
}

-(void)updateResources:(NSNotification *)notification
{
    int joy1Mode = [self getIntResource:@"JoyDevice1"];
    int joy2Mode = [self getIntResource:@"JoyDevice2"];
    [joystick1Mode selectItemAtIndex:joy1Mode];
    [joystick2Mode selectItemAtIndex:joy2Mode];
    
    [self updateKeysetDisplay];
    
    [self updateHidName];
    [self updateHidXAxis];
    [self updateHidYAxis];
    [self updateHidButtons];
}

// ----- Actions -----

-(IBAction)changeJoystick1Mode:(id)sender
{
    int joy1Mode = [joystick1Mode indexOfSelectedItem];
    [self setIntResource:@"JoyDevice1" toValue:joy1Mode];
}

-(IBAction)changeJoystick2Mode:(id)sender
{
    int joy2Mode = [joystick2Mode indexOfSelectedItem];
    [self setIntResource:@"JoyDevice2" toValue:joy2Mode];
}

-(IBAction)toggleKeyset:(id)sender
{
    [self updateKeysetDisplay];
}

-(IBAction)defineKeysetButton:(id)sender
{
    int i;
    for(i=0;i<KEYSET_SIZE;i++) {
        if(keyButtons[i]==sender) {
            break;
        }
    }
    if(i==KEYSET_SIZE)
        return;

    int keyCode = [keyPressView keyCode];
    int keySetNum = [keySetSelect indexOfSelectedItem];

    NSString *res = [NSString stringWithFormat:@"KeySet%d%s",keySetNum+1,keyNames[i]];
    [self setIntResource:res toValue:keyCode];
    
    NSString *val = [NSString stringWithFormat:@"%04x",keyCode];
    [keyButtons[i] setTitle:val];
}

-(IBAction)toggleHidDevice:(id)sender
{
    [self updateHidXAxis];
    [self updateHidYAxis];
    [self updateHidButtons];
}

-(IBAction)refreshHidList:(id)sender
{
#ifdef HAS_JOYSTICK
    reload_device_list();
#endif    

    [self setupHidDeviceList];
    [self updateHidName];
}

-(IBAction)pickHidName:(id)sender
{
    NSString *joyDevice;
    
    // automatic
    if([hidName indexOfSelectedItem]==0) {    
        joyDevice = @"";
    } 
    // picked device
    else {
        NSString *value = [hidName titleOfSelectedItem];
        NSArray *components = [value componentsSeparatedByString:@" "];
        joyDevice = (NSString *)[components objectAtIndex:0];
    }
    
    int hidDeviceNum = [hidDeviceSelect indexOfSelectedItem];
    NSString *tag = [NSString stringWithFormat:@"Joy%cDevice",'A'+hidDeviceNum];
    [self setStringResource:tag toValue:joyDevice];
    
    // update display
    [self updateHidName];
}

-(IBAction)pickXAxis:(id)sender
{
    NSString *axisValue;
    if([hidXAxis selectedItem] == [hidXAxis lastItem])
        axisValue = @"";
    else
        axisValue = [hidXAxis titleOfSelectedItem];

    int hidDeviceNum = [hidDeviceSelect indexOfSelectedItem];        
    [self setStringResource:[NSString stringWithFormat:@"Joy%cXAxis",'A'+hidDeviceNum]
         toValue:axisValue];

    [self updateHidXAxis];
}

-(IBAction)pickYAxis:(id)sender
{
    NSString *axisValue;
    if([hidXAxis selectedItem] == [hidYAxis lastItem])
        axisValue = @"";
    else
        axisValue = [hidYAxis titleOfSelectedItem];

    int hidDeviceNum = [hidDeviceSelect indexOfSelectedItem];        
    [self setStringResource:[NSString stringWithFormat:@"Joy%cYAxis",'A'+hidDeviceNum]
         toValue:axisValue];

    [self updateHidYAxis];
}

-(IBAction)detectXAxis:(id)sender
{
    int hidDeviceNum = [hidDeviceSelect indexOfSelectedItem];
    NSString *axisName = @"";
#ifdef HAS_JOYSTICK
    int axisTag = detect_axis(hidDeviceNum ? &joy_b : &joy_a,1);
    const char *axisNameC = find_axis_name(axisTag);
    if(axisNameC!=NULL)
        axisName = [NSString stringWithCString:axisNameC encoding:NSUTF8StringEncoding];
#endif
    
    [self setStringResource:[NSString stringWithFormat:@"Joy%cXAxis",'A'+hidDeviceNum]
         toValue:axisName];

    [self updateHidXAxis];
}

-(IBAction)detectYAxis:(id)sender
{
    NSString *axisName = @"";
    int hidDeviceNum = [hidDeviceSelect indexOfSelectedItem];
#ifdef HAS_JOYSTICK
    int axisTag = detect_axis(hidDeviceNum ? &joy_b : &joy_a,0);
    const char *axisNameC = find_axis_name(axisTag);
    if(axisNameC!=NULL)
        axisName = [NSString stringWithCString:axisNameC encoding:NSUTF8StringEncoding];
#endif
    
    [self setStringResource:[NSString stringWithFormat:@"Joy%cYAxis",'A'+hidDeviceNum]
         toValue:axisName];

    [self updateHidYAxis];
}

-(IBAction)setXThreshold:(id)sender
{
    int hidDeviceNum = [hidDeviceSelect indexOfSelectedItem];
    int threshold = [hidXThreshold intValue];
    if((threshold>0)&&(threshold<100)) {
      [self setIntResource:[NSString stringWithFormat:@"Joy%cXThreshold",'A'+hidDeviceNum]
          toValue:threshold];
    }
    [self updateHidXAxis];
}

-(IBAction)setYThreshold:(id)sender
{
    int hidDeviceNum = [hidDeviceSelect indexOfSelectedItem];
    int threshold = [hidYThreshold intValue];
    if((threshold>0)&&(threshold<100)) {
      [self setIntResource:[NSString stringWithFormat:@"Joy%cYThreshold",'A'+hidDeviceNum]
          toValue:threshold];
    }
    [self updateHidYAxis];
}

-(IBAction)defineHidButton:(id)sender
{
    int hidDeviceNum = [hidDeviceSelect indexOfSelectedItem];
    int buttonId  = [sender tag];
    int buttonVal = 0;

#ifdef HAS_JOYSTICK
    buttonVal = detect_button(hidDeviceNum ? &joy_b : &joy_a);
#endif

    int ids[HID_NUM_BUTTONS] = { 0,0,0,0,0,0 };
    [self getHidButtons:ids];
    ids[buttonId] = buttonVal;
    
    [self setStringResource:[NSString stringWithFormat:@"Joy%cButtons",'A'+hidDeviceNum]
        toValue:[NSString stringWithFormat:@"%d:%d:%d:%d:%d:%d",
            ids[0],ids[1],ids[2],ids[3],ids[4],ids[5]]];
    
    [self updateHidButtons];
}

@end
