/*
 * c64controller.h - C64 app controller
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
#import "viceappcontroller.h"

@interface C64Controller : VICEAppController
{
    // Machine Outlets
    IBOutlet NSMenuItem *cartridgeFreezeMenuItem;
    IBOutlet NSMenu *expertCartridgeModeMenu;
    // Option Outlets
    IBOutlet NSMenuItem *mouseEmulationMenuItem;
    
    IBOutlet NSMenuItem *vicIIVideoCacheMenuItem;
    IBOutlet NSMenuItem *vicIIDoubleSizeMenuItem;
    IBOutlet NSMenuItem *vicIIDoubleScanMenuItem;
    IBOutlet NSMenuItem *vicIIScale2xMenuItem;
    IBOutlet NSMenuItem *palEmulationMenuItem;
}

// Cartridge
-(IBAction)attachCartridge:(id)sender;
-(IBAction)attachExpertCartridge:(id)sender;
-(IBAction)detachCartridge:(id)sender;
-(IBAction)freezeCartridge:(id)sender;
-(IBAction)controlExpertCartridge:(id)sender;

// Options
- (IBAction)toggleMouseEmulation:(id)sender;

- (IBAction)toggleVICIIVideoCache:(id)sender;
- (IBAction)toggleVICIIDoubleSize:(id)sender;
- (IBAction)toggleVICIIDoubleScan:(id)sender;
- (IBAction)toggleVICIIScale2x:(id)sender;
- (IBAction)togglePALEmulation:(id)sender;

@end
