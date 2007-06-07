/*
 * joysticksettingswindowcontroller.h - JoystickSettings dialog controller
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


#import <Cocoa/Cocoa.h>
#import "viceresourcewindowcontroller.h"
#import "keypressview.h"

enum {
    KEYSET_NORTH=0,
    KEYSET_SOUTH=1,
    KEYSET_WEST=2,
    KEYSET_EAST=3,
    KEYSET_NORTHWEST=4,
    KEYSET_SOUTHWEST=5,
    KEYSET_NORTHEAST=6,
    KEYSET_SOUTHEAST=7,
    KEYSET_FIRE=8,
    KEYSET_SIZE=9
} JoystickKeyset;

@interface JoystickSettingsWindowController : VICEResourceWindowController
{
    IBOutlet NSPopUpButton *joystick1Mode;
    IBOutlet NSPopUpButton *joystick2Mode;

    IBOutlet KeyPressView *keyPressView;
    
    IBOutlet NSButton *buttonANorth;
    IBOutlet NSButton *buttonASouth;
    IBOutlet NSButton *buttonAWest;
    IBOutlet NSButton *buttonAEast;
    IBOutlet NSButton *buttonANorthWest;
    IBOutlet NSButton *buttonANorthEast;
    IBOutlet NSButton *buttonASouthWest;
    IBOutlet NSButton *buttonASouthEast;
    IBOutlet NSButton *buttonAFire;

    IBOutlet NSButton *buttonBNorth;
    IBOutlet NSButton *buttonBSouth;
    IBOutlet NSButton *buttonBWest;
    IBOutlet NSButton *buttonBEast;
    IBOutlet NSButton *buttonBNorthWest;
    IBOutlet NSButton *buttonBNorthEast;
    IBOutlet NSButton *buttonBSouthWest;
    IBOutlet NSButton *buttonBSouthEast;
    IBOutlet NSButton *buttonBFire;

    NSButton *buttonsA[KEYSET_SIZE];
    NSButton *buttonsB[KEYSET_SIZE];
}

-(void)updateResources:(NSNotification *)notification;

-(IBAction)changeJoystick1Mode:(id)sender;
-(IBAction)changeJoystick2Mode:(id)sender;
-(IBAction)defineKeysetAButton:(id)sender;
-(IBAction)defineKeysetBButton:(id)sender;

@end

