/*
 * tapeview.m - TapeView
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

#import "tapeview.h"
#import "vicenotifications.h"
#import "viceapplication.h"

@implementation TapeView

- (id)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame:frame];
    if(self==nil)
        return nil;
    
    float w = NSWidth(frame);
    tape_counter = [[NSTextField alloc]
        initWithFrame:NSMakeRect(0 , 0, w * 0.65, NSHeight(frame))];
    [tape_counter setDrawsBackground:NO];
    [tape_counter setAlignment:NSRightTextAlignment];
    [tape_counter setEditable:NO];
    [tape_counter setBordered:NO];
    [tape_counter setAutoresizingMask: (NSViewWidthSizable | NSViewMaxXMargin)];

    tape_control = [[NSTextField alloc]
        initWithFrame:NSMakeRect(w * 0.65, 0, w * 0.35, 0.75 * NSHeight(frame))];
    [tape_control setFont:[NSFont fontWithName:@"Apple Symbols" size:10.0]];
    [tape_control setDrawsBackground:NO];
    [tape_control setAlignment:NSLeftTextAlignment];
    [tape_control setEditable:NO];
    [tape_control setBordered:NO];
    [tape_control setAutoresizingMask: (NSViewWidthSizable | NSViewMinXMargin)];

    [self addSubview:tape_counter];
    [self addSubview:tape_control];

    [self setImageFile:nil]; /* set tool tip */

    NSMenu * datasetteControls = [[NSMenu alloc] initWithTitle:@""];
    //uiattach_append_tape_menu(datasetteControls);
    [datasetteControls addItem:[NSMenuItem separatorItem]];
    //uidatasette_append_datasette_menu(datasetteControls, 0);

    [self setMenu:datasetteControls];
    [datasetteControls release];

    [self registerForDraggedTypes:[NSArray arrayWithObject:NSFilenamesPboardType]];

    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(displayImage:)
                                                 name:VICEDisplayTapeImageNotification
                                               object:nil];

    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(displayControlStatus:)
                                                 name:VICEDisplayTapeControlStatusNotification
                                               object:nil];

    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(displayMotorStatus:)
                                                 name:VICEDisplayTapeMotorStatusNotification
                                               object:nil];

    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(displayCounter:)
                                                 name:VICEDisplayTapeCounterNotification
                                               object:nil];

    return self;
}

- (void)dealloc
{
    [tape_counter release];
    [tape_control release];

    [super dealloc];
}

- (void)setImageFile:(NSString*)image
{
    [self setToolTip:(image == nil) ? @"<no tape>" : image];
}

- (void)displayImage:(NSNotification*)notification
{
    NSDictionary * dict = [notification userInfo];
    [self setImageFile:[dict objectForKey:@"image"]];
}


- (void)displayCounter:(NSNotification*)notification
{
    int counter = [[[notification userInfo] objectForKey:@"counter"] intValue];
    NSString * s = [NSString stringWithFormat:@"T: %03d", counter];
    [tape_counter setStringValue:s];
}

- (void)updateTapeStatus
{
    static const char * tape_controls[] =
    {
        "\xe2\x96\xa1", "\xe2\x96\xa0",                           /* stop    */
        "\xe2\x96\xb7", "\xe2\x96\xb6",                             /* play    */
        "\xe2\x96\xb7\xe2\x96\xb7", "\xe2\x96\xb6\xe2\x96\xb6",     /* forward */
        "\xe2\x97\x81\xe2\x97\x81", "\xe2\x97\x80\xe2\x97\x80",     /* rewind  */
        "\xe2\x97\x8b", "\xe2\x97\x8f"                              /* record  */
    };

    NSString * s = [NSString stringWithUTF8String:tape_controls[tape_control_status*2+tape_motor_status]];
    [tape_control setStringValue:s];
}

- (void)displayControlStatus:(NSNotification*)notification
{
    tape_control_status = [[[notification userInfo] objectForKey:@"status"] intValue];
    [self updateTapeStatus];
}

- (void)displayMotorStatus:(NSNotification*)notification
{
    tape_motor_status = [[[notification userInfo] objectForKey:@"status"] intValue];
    [self updateTapeStatus];
}

- (void)setEnabled:(BOOL)flag
{
    if (!flag)
    {
        [tape_control setStringValue:@""];
        [tape_counter setStringValue:@""];
    }
    else
    {
        [self updateTapeStatus];
    }
}


- (void)mouseDown:(NSEvent*)event
{
    [NSMenu popUpContextMenu:[self menu] withEvent:event forView:self];
}

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
    return [[VICEApplication theMachineController] attachTapeImage:path];
}

@end
