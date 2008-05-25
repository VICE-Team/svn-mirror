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

// ----- Snapshot -----
// read a snapshot from a file
-(void)loadSnapshot:(NSString *)path;
// write a snapshot to a file
-(void)saveSnapshot:(NSString *)path withROMS:(BOOL)saveRoms andDisks:(BOOL)saveDisks;
// scan for quick snapshot files
- (NSString *)getQuickSnapshotFileName:(BOOL)load;
// read a quick snapshot from a file
-(void)loadQuickSnapshot;
// save a quick snapshot to a file
-(void)saveQuickSnapshot;

// ----- Media -----
// start recording media/save screen shot
-(BOOL)startRecordingMedia:(NSString *)driver 
                fromCanvas:(int)canvasId
                    toFile:(NSString *)path;
// is recording a screenshot
-(BOOL)isRecordingMedia;
// stop recording screenshot
-(void)stopRecordingMedia;
// enumerate media
-(NSArray *)enumMediaDrivers;
// has driver container
-(BOOL)mediaDriverHasOptions:(NSString *)driver;
// enumerate media formats
-(NSArray *)enumMediaFormats:(NSString *)driver;
// extension for media 
-(NSString *)defaultExtensionForMediaDriver:(NSString *)driver andFormat:(NSString *)format;

// ----- Keyboard -----
// key pressed
-(void)keyPressed:(unsigned int)code;
// key release
-(void)keyReleased:(unsigned int)code;
// dump keymap
-(BOOL)dumpKeymap:(NSString *)path;

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

// query disk name
-(NSString *)getDiskName:(int)unit;

// ----- Tape Commands -----
// attach tape image
-(BOOL)attachTapeImage:(NSString *)path;
// detach tape image
-(void)detachTapeImage;

// ----- Printer -----
// send form feed (unit #4=0,#5=1,Userport=2)
-(void)printerFormFeed:(int)unit;

@end

