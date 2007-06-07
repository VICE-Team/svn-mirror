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

#include "joy.h"

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

-(void)windowDidLoad
{
    // define buttons array
    buttonsA[KEYSET_NORTH]     = buttonANorth;
    buttonsA[KEYSET_SOUTH]     = buttonASouth;
    buttonsA[KEYSET_WEST]      = buttonAWest;
    buttonsA[KEYSET_EAST]      = buttonAEast;
    buttonsA[KEYSET_NORTHWEST] = buttonANorthWest;
    buttonsA[KEYSET_SOUTHWEST] = buttonASouthWest;
    buttonsA[KEYSET_NORTHEAST] = buttonANorthEast;
    buttonsA[KEYSET_SOUTHEAST] = buttonASouthEast;
    buttonsA[KEYSET_FIRE]      = buttonAFire;

    buttonsB[KEYSET_NORTH]     = buttonBNorth;
    buttonsB[KEYSET_SOUTH]     = buttonBSouth;
    buttonsB[KEYSET_WEST]      = buttonBWest;
    buttonsB[KEYSET_EAST]      = buttonBEast;
    buttonsB[KEYSET_NORTHWEST] = buttonBNorthWest;
    buttonsB[KEYSET_SOUTHWEST] = buttonBSouthWest;
    buttonsB[KEYSET_NORTHEAST] = buttonBNorthEast;
    buttonsB[KEYSET_SOUTHEAST] = buttonBSouthEast;
    buttonsB[KEYSET_FIRE]      = buttonBFire;

    [self updateResources:nil];
    [super windowDidLoad];
}

-(void)updateResources:(NSNotification *)notification
{
    int joy1Mode = [self getIntResource:@"JoyDevice1"];
    int joy2Mode = [self getIntResource:@"JoyDevice2"];
    [joystick1Mode selectItemAtIndex:joy1Mode];
    [joystick2Mode selectItemAtIndex:joy2Mode];
    
    int i;
    for(i=0;i<KEYSET_SIZE;i++) {
        NSString *res1 = [NSString stringWithFormat:@"KeySet1%s",keyNames[i]];
        int keyCode1 = [self getIntResource:res1];
        NSString *val1;
        if(keyCode1==-1)
            val1 = @"N/A";
        else
            val1 = [NSString stringWithFormat:@"%04x",keyCode1];
        [buttonsA[i] setTitle:val1];

        NSString *res2 = [NSString stringWithFormat:@"KeySet2%s",keyNames[i]];
        int keyCode2 = [self getIntResource:res2];
        NSString *val2;
        if(keyCode2==-1)
            val2 = @"N/A";
        else
            val2 = [NSString stringWithFormat:@"%04x",keyCode2];
        [buttonsB[i] setTitle:val2];
    }    
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

-(IBAction)defineKeysetAButton:(id)sender
{
    int i;
    for(i=0;i<KEYSET_SIZE;i++) {
        if(buttonsA[i]==sender) {
            break;
        }
    }
    if(i==KEYSET_SIZE)
        return;

    int keyCode = [keyPressView keyCode];

    NSString *res = [NSString stringWithFormat:@"KeySet1%s",keyNames[i]];
    [self setIntResource:res toValue:keyCode];
    
    NSString *val = [NSString stringWithFormat:@"%04x",keyCode];
    [buttonsA[i] setTitle:val];
}

-(IBAction)defineKeysetBButton:(id)sender
{
    int i;
    for(i=0;i<KEYSET_SIZE;i++) {
        if(buttonsB[i]==sender) {
            break;
        }
    }
    if(i==KEYSET_SIZE)
        return;

    int keyCode = [keyPressView keyCode];

    NSString *res = [NSString stringWithFormat:@"KeySet2%s",keyNames[i]];
    [self setIntResource:res toValue:keyCode];
    
    NSString *val = [NSString stringWithFormat:@"%04x",keyCode];
    [buttonsB[i] setTitle:val];
}

@end
