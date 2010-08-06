/*
 * vicewindowcontroller.m - generic window controller
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

#include "vice.h"
#include "machine.h"

#import "vicewindowcontroller.h"

@implementation VICEWindowController

-(id)initWithWindowNibName:(NSString *)name
{
    self = [super initWithWindowNibName:name];
    return self;
}

-(void)dealloc
{
}

-(void)windowDidLoad
{
    [super windowDidLoad];

    // adjust window title: prepend machine
    NSWindow *window = [self window];
    NSString *title = [window title];
    NSString *newTitle = [NSString stringWithFormat:@"VICE: %s %@",machine_get_name(),title];
    [window setTitle:newTitle];
    
    // set autosave name
    [self setWindowFrameAutosaveName:newTitle];
}

- (void)toggleWindow:(id)sender
{
    BOOL isLoaded = [self isWindowLoaded];
    NSWindow *window = [self window];
    if(isLoaded) {
        if([window isVisible]) {
            [window orderOut:sender];
        } else {
            [self showWindow:sender];
        }
    }
}

- (void)checkMenuItem:(NSMenuItem *)menuItem
{
    BOOL isLoaded = [self isWindowLoaded];
    if(isLoaded) {
        NSWindow *window = [self window];
        if([window isVisible]) {
            [menuItem setState:NSOnState];
        } else {
            [menuItem setState:NSOffState];
        }
    }
}

@end
