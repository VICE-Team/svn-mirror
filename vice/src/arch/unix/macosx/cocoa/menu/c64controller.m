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

// ----- Cartridge -----

-(IBAction)attachCartridge:(id)sender
{
    NSDictionary *cartTypes = [NSDictionary dictionaryWithObjectsAndKeys:
        [NSNumber numberWithInt:CARTRIDGE_CRT], @"CRT",
        
        [NSNumber numberWithInt:CARTRIDGE_GENERIC_8KB], @"Generic 8KB",
        [NSNumber numberWithInt:CARTRIDGE_GENERIC_16KB], @"Generic 16KB",
        [NSNumber numberWithInt:CARTRIDGE_ACTION_REPLAY], @"Action Replay",
        [NSNumber numberWithInt:CARTRIDGE_ACTION_REPLAY3], @"Action Replay III",
        [NSNumber numberWithInt:CARTRIDGE_ACTION_REPLAY4], @"Action Replay IV",
        
        [NSNumber numberWithInt:CARTRIDGE_ATOMIC_POWER], @"Atomic Power",
        [NSNumber numberWithInt:CARTRIDGE_EPYX_FASTLOAD], @"Epyx Fastload",
        [NSNumber numberWithInt:CARTRIDGE_IDE64], @"IDE64",
        [NSNumber numberWithInt:CARTRIDGE_IEEE488], @"IEEE488",
        [NSNumber numberWithInt:CARTRIDGE_MMC_REPLAY], @"MMC Replay",
        [NSNumber numberWithInt:CARTRIDGE_RETRO_REPLAY], @"Retro Replay",
        
        [NSNumber numberWithInt:CARTRIDGE_STARDOS], @"StarDOS",        
        [NSNumber numberWithInt:CARTRIDGE_STRUCTURED_BASIC], @"Structured Basic",
        [NSNumber numberWithInt:CARTRIDGE_SUPER_SNAPSHOT], @"Super Snapshot 4",
        [NSNumber numberWithInt:CARTRIDGE_SUPER_SNAPSHOT_V5], @"Super Snapshot 5",
        
        nil, nil];

    NSArray *result = [[self getFilePanel] pickAttachFileWithTitle:@"Attach Cartridge Image"
                                                 andTypeDictionary:cartTypes
                                                 defaultType:@"CRT"];
    if(result != nil) {
        NSString *filename   = [result objectAtIndex:0];
        NSNumber *typeNumber = [result objectAtIndex:1];
        int type = [typeNumber intValue];
        
        if ([(C64MachineController *)[VICEApplication theMachineController] 
                                      attachCartridge:type 
                                                image:filename]) {
            [(C64MachineController *)[VICEApplication theMachineController] 
                setDefaultCartridge];                
            [self updateMachineResources];
        } else {
            [VICEApplication runErrorMessage:@"Error attaching image!"];
        }        
    }
}

-(IBAction)detachCartridge:(id)sender
{
    [(C64MachineController *)[VICEApplication theMachineController]
        detachCartridge:-1];
    [(C64MachineController *)[VICEApplication theMachineController]
        setDefaultCartridge];
    [self updateMachineResources];
}

-(IBAction)freezeCartridge:(id)sender
{
    [(C64MachineController *)[VICEApplication theMachineController]
        freezeCartridge];
}

-  (BOOL)validateMenuItem:(NSMenuItem *)item
{
    if (item==cartridgeFreezeMenuItem) {
        return [self getIntResource:@"CartridgeType"] != CARTRIDGE_NONE;
    }
    return YES;
}

@end
