/*
 * c64controller.m - C64 app controller
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

#include "cartridge.h"

#import "c64controller.h"
#import "viceapplication.h"
#import "c64machinecontroller.h"

@implementation C64Controller

// Cartridge

-(IBAction)attachCartridge:(id)sender
{
    NSArray *cartTypes = [NSArray arrayWithObjects:
        @"CRT",
        @"Generic 8KB",
        @"Generic 16KB",
        @"Action Replay",
        @"Action Replay III",
        
        @"Atomic Power",
        @"Epyx Fastload",
        @"IEEE488",
        @"Retro Replay",
        @"IDE64",
        
        @"Super Snapshot 4",
        @"Super Snapshot 5",
        @"Structured Basic",
        
        nil];
    int cartTypeIds[] = {
        CARTRIDGE_CRT,
        CARTRIDGE_GENERIC_8KB,
        CARTRIDGE_GENERIC_16KB,
        CARTRIDGE_ACTION_REPLAY,
        CARTRIDGE_ACTION_REPLAY3,
        
        CARTRIDGE_ATOMIC_POWER,
        CARTRIDGE_EPYX_FASTLOAD,
        CARTRIDGE_IEEE488,
        CARTRIDGE_RETRO_REPLAY,
        CARTRIDGE_IDE64,
        
        CARTRIDGE_SUPER_SNAPSHOT,
        CARTRIDGE_SUPER_SNAPSHOT_V5,
        CARTRIDGE_STRUCTURED_BASIC
    };

    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    NSOpenPanel * panel = [NSOpenPanel openPanel];
    NSView * accessories = [[NSView alloc] initWithFrame:NSMakeRect(0, 0, 310, 49)];
    [accessories autorelease];

    NSTextField * type_label = [[NSTextField alloc] initWithFrame:NSMakeRect(4, 18, 80, 17)];
    [type_label autorelease];
    [type_label setAlignment:NSRightTextAlignment];
    [type_label setEditable:NO];
    [type_label setBordered:NO];
    [type_label setDrawsBackground:NO];
    [type_label setStringValue:@"Type:"];

    NSPopUpButton * type_button = [[NSPopUpButton alloc] initWithFrame:NSMakeRect(86, 12, 98, 26)];
    [type_button addItemsWithTitles:cartTypes];
    [type_button autorelease];

    [accessories addSubview:type_button];
    [accessories addSubview:type_label];

    [panel setAccessoryView:accessories];
    [panel setTitle:@"Attach Cartridge Image"];
    [panel setPrompt:@"Attach"];

    if ([panel runModalForDirectory:nil file:nil] == NSOKButton) {
        int type = cartTypeIds[[type_button indexOfSelectedItem]];
        if([(C64MachineController *)[VICEApplication theMachineController] 
                                      attachCartridge:type 
                                                image:[panel filename]]) {
            [(C64MachineController *)[VICEApplication theMachineController] 
                setDefaultCartridge];                
            [self updateMachineResources];
        } else {
            [VICEApplication runErrorMessage:@"Error attaching image!"];
        }
    }
    [pool release];
}

// ----- Cartridge -----

-(IBAction)attachExpertCartridge:(id)sender
{
    [(C64MachineController *)[VICEApplication theMachineController]
        attachExpertCartridge];
    [(C64MachineController *)[VICEApplication theMachineController]
        setDefaultCartridge];
    [self updateMachineResources];
}

-(IBAction)detachCartridge:(id)sender
{
    [(C64MachineController *)[VICEApplication theMachineController]
        detachCartridge];
    [(C64MachineController *)[VICEApplication theMachineController]
        setDefaultCartridge];
    [self updateMachineResources];
}

-(IBAction)freezeCartridge:(id)sender
{
    [(C64MachineController *)[VICEApplication theMachineController]
        freezeCartridge];
}

-(IBAction)controlExpertCartridge:(id)sender
{
    [self setIntResource:@"CartridgeMode" toValue:[sender tag]];
    [self updateMachineResources];
}

- (void)updateMachineResources
{
    // Expert Cartridge
    int expertCartridgeMode = [self getIntResource:@"CartridgeMode"];
    int i;
    for(i=0;i<3;i++) {
        NSMenuItem *item = [expertCartridgeModeMenu itemAtIndex:i];
        [item setState:(i == expertCartridgeMode) ? NSOnState : NSOffState];
    }
    
    [super updateMachineResources];
}

-  (BOOL)validateMenuItem:(NSMenuItem *)item
{
    if(item==cartridgeFreezeMenuItem) {
        return [self getIntResource:@"CartridgeType"] != CARTRIDGE_NONE;
    }
    else if([item menu]==expertCartridgeModeMenu) {
        return [self getIntResource:@"CartridgeType"] == CARTRIDGE_EXPERT;
    }
    return YES;
}

// ----- Options -----

// Mouse

- (IBAction)toggleMouseEmulation:(id)sender
{
    [self setIntResource:@"Mouse" toValue:![sender state]];
    [self updateOptionsResources];
}

// Video Rendering

- (IBAction)toggleVICIIVideoCache:(id)sender
{
    [self setIntResource:@"VICIIVideoCache" toValue:![sender state]];
    [self updateOptionsResources];
}

- (IBAction)toggleVICIIDoubleSize:(id)sender
{
    [self setIntResource:@"VICIIDoubleSize" toValue:![sender state]];
    [self updateOptionsResources];
}

- (IBAction)toggleVICIIDoubleScan:(id)sender
{
    [self setIntResource:@"VICIIDoubleScan" toValue:![sender state]];
    [self updateOptionsResources];
}

- (IBAction)toggleVICIIScale2x:(id)sender
{
    [self setIntResource:@"VICIIScale2x" toValue:![sender state]];
    [self updateOptionsResources];
}

- (IBAction)togglePALEmulation:(id)sender
{
    [self setIntResource:@"PALEmulation" toValue:![sender state]];
    [self updateOptionsResources];
}

// ----- Update Resources -----

- (void)updateOptionsResources
{
    // Mouse
    [mouseEmulationMenuItem setState:[self getIntResource:@"Mouse"]];
    
    // Video Rendering
    [vicIIVideoCacheMenuItem setState:[self getIntResource:@"VICIIVideoCache"]];
    [vicIIDoubleSizeMenuItem setState:[self getIntResource:@"VICIIDoubleSize"]];
    [vicIIDoubleScanMenuItem setState:[self getIntResource:@"VICIIDoubleScan"]];
    [vicIIScale2xMenuItem setState:[self getIntResource:@"VICIIScale2x"]];
    [palEmulationMenuItem setState:[self getIntResource:@"PALEmulation"]];
        
    [super updateOptionsResources];
}

@end
