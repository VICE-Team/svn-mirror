/*
 * vicemachinecontroller.h - VICEMachineController - base machine controller
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

/* The VICEMachineController posts notifications of the Machine Thread */

@interface VICEMachineController : NSObject
{
}

// ----- Resources -----

// get integer resource
-(NSNumber *)getIntResource:(NSString *)name;
// get string resource
-(NSString *)getStringResource:(NSString *)name;

// set integer resource
-(BOOL)setIntResource:(NSString *)name value:(NSNumber *)value;
// set string resource
-(BOOL)setStringResource:(NSString *)name value:(NSString *)value;

// load resources
-(BOOL)loadResources:(NSString *)path;
// save resources
-(BOOL)saveResources:(NSString *)path;
// reset resources
-(void)resetResources;

// ----- Reset -----
// reset machine
-(void)resetMachine:(BOOL)hardReset;
// reset drive
-(void)resetDrive:(int)unit;

// ----- Monitor -----
// activate monitor
-(void)activateMonitor;

// ----- Keyboard -----
// key pressed
-(void)keyPressed:(unsigned int)code;
// key release
-(void)keyReleased:(unsigned int)code;

// ----- Mouse -----
// mouse moved
-(void)mouseMoveToX:(int)deltaX andY:(int)deltaY;
// mouse pressed
-(void)mousePressed;
// mouse release
-(void)mouseReleased;

// ----- Drive Commands -----
// disk image
-(BOOL)smartAttachImage:(NSString *)path;
// attach disk image
-(BOOL)attachDiskImage:(int)unit path:(NSString *)path;
// detach disk image
-(void)detachDiskImage:(int)unit;
// create disk image
-(BOOL)createDiskImage:(int)type path:(NSString *)path name:(NSString *)name;

// ----- Tape Commands -----
// attach tape image
-(BOOL)attachTapeImage:(NSString *)path;
// detach tape image
-(void)detachTapeImage;

@end

