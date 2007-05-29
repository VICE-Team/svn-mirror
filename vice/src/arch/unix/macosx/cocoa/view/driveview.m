/*
 * driveview.m - DriveView
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

#import "driveview.h"
#import "vicenotifications.h"
#import "viceapplication.h"

@implementation DriveView

- (id)initWithFrame:(NSRect)frame driveNumber:(int)drive
{
    self = [super initWithFrame:frame];
    if(self==nil)
        return nil;
    
    driveNumber = drive;
    activeLedColor = nil;
    inactiveLedColor = [NSColor blackColor];
    [inactiveLedColor retain];

    // calc layout
    float fw = NSWidth(frame);
    float fh = NSHeight(frame);
    
    float ledH = fh / 2.0;
    float ledW = ledH * 2.0;

    NSRect trackRect = NSMakeRect(0,0,fw - ledW,ledH);
    NSRect ledRect   = NSMakeRect(fw-ledW,0,ledW,ledH);
    NSRect imageRect = NSMakeRect(0,ledH,fw,ledH);
    
    // text box for track display
    trackText = [[NSTextField alloc] initWithFrame:trackRect];
    [trackText setDrawsBackground:NO];
    [trackText setAutoresizingMask:NSViewWidthSizable];
    [trackText setEditable:NO];
    [trackText setBordered:NO];
    [trackText setAlignment:NSLeftTextAlignment];
    [self addSubview:trackText];
    
    // color field for drive led
    driveLed = [[NSColorWell alloc] initWithFrame:ledRect];
    [driveLed setEnabled:NO];
    [driveLed setBordered:YES];
    [driveLed setAutoresizingMask:NSViewMinXMargin];
    [self addSubview:driveLed];

    // image name text field
    imageText = [[NSTextField alloc] initWithFrame:imageRect];
    [imageText setDrawsBackground:NO];
    [imageText setAutoresizingMask:NSViewWidthSizable];
    [imageText setEditable:NO];
    [imageText setBordered:NO];
    [imageText setAlignment:NSLeftTextAlignment];
    [[imageText cell] setLineBreakMode:NSLineBreakByTruncatingHead];
    [self addSubview:imageText];

    // register drag & drop
    [self registerForDraggedTypes:[NSArray arrayWithObject:NSFilenamesPboardType]];

    // register notifications
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(displayLed:)
                                                 name:VICEDisplayDriveLedNotification
                                               object:nil];

    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(displayImage:)
                                                 name:VICEDisplayDriveImageNotification
                                               object:nil];

    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(displayTrack:)
                                                 name:VICEDisplayDriveTrackNotification
                                               object:nil];

    return self;
}

- (void)dealloc
{
    [driveLed release];
    [trackText release];
    [imageText release];
    
    [activeLedColor release];
    [inactiveLedColor release];
    
    [super dealloc];
}

- (void)initLedColor:(NSColor*)color
{
    if(activeLedColor==nil) {
        activeLedColor = color;
        [activeLedColor retain];
    }
}

- (void)displayLed:(NSNotification*)notification
{
    NSDictionary * dict = [notification userInfo];
    int drive = [[dict objectForKey:@"drive"] intValue];
    
    if (drive == driveNumber) {
        BOOL active = [[dict objectForKey:@"active"] boolValue];
        NSColor *color;
        if(active && (activeLedColor!=nil)) {
            color = activeLedColor;
        } else {
            color = inactiveLedColor;
        }
        [driveLed setColor:color];
    }
}

- (void)displayImage:(NSNotification*)notification
{
    NSDictionary * dict = [notification userInfo];
    int drive = [[dict objectForKey:@"drive"] intValue];
    
    if (drive == driveNumber) {
        NSString *image = [dict objectForKey:@"image"];
        [image retain];
        [imageText setStringValue:image];
    }
}

- (void)displayTrack:(NSNotification*)notification
{
    NSDictionary * dict = [notification userInfo];
    int drive = [[dict objectForKey:@"drive"] intValue];
    int curDriveBase = [[dict objectForKey:@"drive_base"] intValue];

    // store in object
    driveBase = curDriveBase;

    if (drive == driveNumber)
    {
        float track = [[dict objectForKey:@"track"] floatValue];
        NSString * s = [NSString stringWithFormat:@"%d: %.1f", drive + driveBase, track];
        [trackText setStringValue:s];
    }
}


#if 0
- (void)driveMenuChanged:(NSNotification*)notification
{
    NSDictionary * dict = [notification userInfo];
    int drive = [[dict objectForKey:@"drive"] intValue];

    if (drive == driveNumber)
        [self setMenu:[dict objectForKey:@"menu"]];
}

- (void)mouseDown:(NSEvent*)event
{
    [NSMenu popUpContextMenu:[self menu] withEvent:event forView:self];
}
#endif

// ----- Drag & Drop -----

- (NSDragOperation)draggingEntered:(id <NSDraggingInfo>)sender
{
    if (NSDragOperationGeneric & [sender draggingSourceOperationMask])
        return NSDragOperationLink;
    else
        return NSDragOperationNone;
}

- (BOOL)performDragOperation:(id <NSDraggingInfo>)sender
{
    id files = [[sender draggingPasteboard] propertyListForType:NSFilenamesPboardType];
    NSString * path = [files objectAtIndex:0];
    return [[VICEApplication theMachineController] attachDiskImage:driveNumber + driveBase
                                                              path:path];
}


@end
