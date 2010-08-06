/*
 * debuggerwindowcontroller.m - debugger window controller
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
#include "monitor.h"

#import "debuggerwindowcontroller.h"
#import "vicenotifications.h"
#import "viceapplication.h"

@implementation DebuggerWindowController

- (id)initWithWindowNibName:(NSString *)nib memSpace:(int)space;
{
    memSpace = space;
    return [super initWithWindowNibName:nib];
}

-(void)windowDidLoad
{
    [super windowDidLoad];

    // adjust window title: prepend machine and append memspace
    NSString *spaceName[] = {
        @"???",
        @"CPU",
        @"Drive #8",
        @"Drive #9",
        @"Drive #10",
        @"Drive #11"
    };
    NSWindow *window = [self window];
    NSString *title = [window title];
    NSString *newTitle = [NSString stringWithFormat:@"%@ (%@)",title,spaceName[memSpace]];
    [window setTitle:newTitle];
    
    // register for monitor state updates
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(monitorStateChanged:)
                                                 name:VICEMonitorStateNotification
                                               object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(monitorUpdateRequest:)
                                                 name:VICEMonitorUpdateNotification
                                               object:nil];
}

-(void)monitorStateChanged:(NSNotification *)notification
{
    NSWindow *window = [self window];
    int state = [[[notification userInfo] objectForKey:@"state"] intValue];
    switch (state) {
        case VICEMonitorStateOn:
            //NSLog(@"Monitor on");
            [self monitorOn];
            break;
        case VICEMonitorStateOff:
            //NSLog(@"Monitor off");
            [self monitorOff];
            break;
        case VICEMonitorStateSuspend:
            //NSLog(@"Monitor suspend");
            [self monitorSuspend];
            break;
        case VICEMonitorStateResume:
            //NSLog(@"Monitor resume");
            [self monitorResume];
            break;
        default:
            break;
    }
}

-(void)monitorUpdateRequest:(NSNotification *)notification
{
    //NSLog(@"Monitor update");
    [self update];
}

-(void)update
{
}

- (void)monitorOn
{
}

- (void)monitorOff
{
}

- (void)monitorSuspend
{
}

- (void)monitorResume
{
}


- (NSString *)toBinaryString:(unsigned int)value width:(int)w
{
    NSMutableString *str = [NSMutableString stringWithCapacity:w];
    int i;
    for(i=w-1;i>=0;i--) {
        unsigned int mask = (1 << i);
        [str appendString:((value & mask) == mask)?@"1":@"0"];
    }
    return str;
}

@end
