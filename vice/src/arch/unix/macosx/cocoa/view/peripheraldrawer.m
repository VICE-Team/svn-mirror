/*
 * peripheraldrawer.m - PeripheralDrawer
 *
 * Written by
 *  Christian Vogelgsang <chris@vogelgsang.org>
 *  Michael Klein <michael.klein@puffin.lb.shuttle.de>
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
#include "log.h"
#include "machine.h"

#import <Cocoa/Cocoa.h>
#import "peripheraldrawer.h"
#import "vicenotifications.h"
#import "vicewindow.h"

#define PERIPH_WIDTH            100
#define PERIPH_HEIGHT           22

/* ============================================================== */

@interface FlippedView : NSView
@end

@implementation FlippedView : NSView
- (BOOL)isFlipped
{
    return YES;
}
@end

/* ============================================================== */

@implementation PeripheralDrawer

- (id)initWithPreferredEdge:(NSRectEdge)edge
{
    if ((self = [super initWithContentSize:NSMakeSize(PERIPH_WIDTH, PERIPH_HEIGHT)
                             preferredEdge:edge]) != nil)
    {
        [self setMinContentSize:NSMakeSize(PERIPH_WIDTH, 5*PERIPH_HEIGHT)];

        led_color[0] = [[NSColor redColor] retain];
        led_color[1] = [[NSColor greenColor] retain];

        FlippedView * v = [[FlippedView alloc]
            initWithFrame:NSMakeRect(0,0,PERIPH_WIDTH,PERIPH_HEIGHT)];
        [v setAutoresizingMask:(NSViewWidthSizable | NSViewHeightSizable)];

        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(enableDriveStatus:)
                                                     name:VICEEnableDriveStatusNotification
                                                   object:nil];

        int i;
        for (i = 0; i < DRIVE_NUM; i++)
        {
            NSRect rect = NSMakeRect(0, 0, PERIPH_WIDTH, PERIPH_HEIGHT*2);
            drive_view[i] = [[DriveView alloc] initWithFrame:rect driveNumber:i];
            [drive_view[i] setAutoresizingMask: NSViewWidthSizable];
        }

        tape_view = [[TapeView alloc] initWithFrame:NSMakeRect(0, 0, PERIPH_WIDTH, PERIPH_HEIGHT)];
        [tape_view setAutoresizingMask: NSViewWidthSizable];

//        [self setTapeStatus:0];
        [v addSubview:tape_view];

        [self setContentView:v];
    }

    return self;
}

- (void)dealloc
{
    int i;
    [led_color[0] release];
    [led_color[1] release];

    for (i = 0; i < DRIVE_NUM; i++)
    {
        [drive_view[i] release];
    }

    [tape_view release];
    [super dealloc];
}

- (void)enableDriveStatus:(NSNotification*)notification
{
    NSDictionary * dict = [notification userInfo];
    int drive_led_color = [[dict objectForKey:@"drive_led_color"] intValue];
    int enable = [[dict objectForKey:@"enabled_drives"] intValue];

    float width = NSWidth([[self contentView] bounds]);

    int i, count = 0;
    for(i = 0; i < DRIVE_NUM; i++)
    {
        int flag = 1<<i;
        if(enable & flag)
        {
            int colorIndex = (drive_led_color & flag) != 0 ? 1 : 0; 
            [[self contentView] addSubview:drive_view[i]];
            [drive_view[i] initLedColor:led_color[colorIndex]];
            [drive_view[i] setFrame:NSMakeRect(0, count * PERIPH_HEIGHT * 2, width, PERIPH_HEIGHT*2)];
            count++;
        }
        else
        {
            [drive_view[i] removeFromSuperview];
        }
    }

    // reposition tape display and rec/play field
    [tape_view setFrame:NSMakeRect(0, count++ * PERIPH_HEIGHT * 2, width, PERIPH_HEIGHT)];
    [[self contentView] setNeedsDisplay:YES];
}

- (void)setTapeStatus:(int)status
{
    [tape_view setEnabled:status];
    [[self contentView] setNeedsDisplay:YES];
}

@end
