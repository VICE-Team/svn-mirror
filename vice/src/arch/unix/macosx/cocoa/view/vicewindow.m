/*
 * vicewindow.m - VICEWindow
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

#import "vicewindow.h"
#import "viceapplication.h"

@implementation VICEWindow

- (id)initWithContentRect:(NSRect)rect title:(NSString *)title
{    
    // store initial canvas size
    NSSize size = rect.size;
    unsigned int canvas_width = size.width;
    unsigned int canvas_height = size.height;
    original_canvas_size = size;

    // set window style
    unsigned int style =
        NSTitledWindowMask | NSClosableWindowMask |
        NSMiniaturizableWindowMask | NSResizableWindowMask;
    if ([[NSUserDefaults standardUserDefaults] boolForKey:@"Textured Windows"])
        style |= NSTexturedBackgroundWindowMask;
    
    // create window
    self = [super initWithContentRect:rect
                            styleMask:style
                              backing:NSBackingStoreBuffered
                                defer:NO];
    if (self==nil)
        return nil;

    // setup window
    [self setTitle:title];
    [self setFrameAutosaveName:title];
    [self setContentMinSize:NSMakeSize(canvas_width / 2, canvas_height / 2)];

    // now size could have changed due to prefences size
    // so determine current canvas size
    NSRect bounds = [[self contentView] bounds];
    unsigned int cw = NSWidth(bounds);
    unsigned int ch = NSHeight(bounds);

    // the container box for the canvas
    rect = NSMakeRect(0, 0, cw, ch);
    canvasContainer = [[NSBox alloc] initWithFrame:rect];
    [canvasContainer setContentViewMargins:NSMakeSize(0, 0)];
    [canvasContainer setTitlePosition:NSNoTitle];
    [canvasContainer setBorderType:NSNoBorder]; // NSNoBorder/NSLineBorder/NSGrooveBorder
    [canvasContainer setAutoresizingMask: (NSViewWidthSizable | NSViewHeightSizable)];
    [[self contentView] addSubview:canvasContainer];
    
    // canvasView: the OpenGL render canvas
    rect = NSMakeRect(0, 0, cw, ch);
    canvasView = [[VICEGLView alloc] initWithFrame:rect];
    [canvasView setupTexture:size];
    [canvasView setAutoresizingMask: (NSViewWidthSizable | NSViewHeightSizable)];
    [canvasContainer addSubview:canvasView];

    isFullscreen = false;
    return self;
}

- (void)close
{
    // disable fullscreen on close
    if (isFullscreen)
        [self toggleFullscreen:nil];
        
    [super close];
}

- (void)dealloc
{
    [canvasView release];
    [canvasContainer release];
    [super dealloc];
}

- (VICEGLView *)getVICEGLView
{
    return canvasView;
}

- (void)updateCanvas:(id)sender
{
    [canvasView updateTexture];
}

- (void)resizeCanvas:(NSSize)size
{
    // do not resize if same size
    if ((original_canvas_size.width  == size.width) && 
       (original_canvas_size.height == size.height)) {
      return;
    }
    
    original_canvas_size = size;
    [canvasView setupTexture:size];

    [self setContentMinSize:NSMakeSize(size.width / 2, size.height / 2)];
    [self resizeCanvasToMultipleSize:nil];
}

- (void)resizeCanvasToMultipleSize:(id)sender
{
    int factor = 1;
    if (sender!=nil)
        factor = [sender tag];
    
    NSRect f = [self frame];
    NSSize s = [[self contentView] bounds].size;
    
    f.size.width  = original_canvas_size.width * factor;
    f.size.height += original_canvas_size.height * factor - s.height;
    f.origin.y    += s.height - original_canvas_size.height;
    [self setFrame:f display:YES];
}

- (BOOL)windowShouldClose:(id)sender
{
    // tell application to terminate
    [NSApp terminate:nil];

    return NO;
}

// adjust resizing to be proportional
- (NSSize)windowWillResize:(NSWindow *)sender toSize:(NSSize)proposedFrameSize
{
    UInt32 modifierMask = [[self currentEvent] modifierFlags];
    if (modifierMask & NSShiftKeyMask ||
        ![[self contentView] inLiveResize])
    {
        return proposedFrameSize;
    }

    float titleHeight = NSHeight([self frame]) - NSHeight([[self contentView] bounds]);
    float contentHeight = proposedFrameSize.height - titleHeight;

    if (modifierMask & NSAlternateKeyMask)
    {
        float ratio = floor(contentHeight / original_canvas_size.height + 0.5);
        if (ratio < 1.0)
            ratio = 1.0;
        contentHeight = original_canvas_size.height * ratio;
    }

    float aspect_ratio = original_canvas_size.width / original_canvas_size.height;
    float scaledWidth = contentHeight * aspect_ratio;
    return NSMakeSize(scaledWidth, contentHeight + titleHeight);
}

// toggle fullscreen
- (void)toggleFullscreen:(id)sender
{
    // fade out display
    CGDisplayFadeReservationToken displayFadeReservation;
    CGAcquireDisplayFadeReservation (
        kCGMaxDisplayReservationInterval,
        &displayFadeReservation
        );
    CGDisplayFade (
        displayFadeReservation,
        0.5,						// 0.5 seconds
        kCGDisplayBlendNormal,		// starting state
        kCGDisplayBlendSolidColor,	// ending state
        0.0, 0.0, 0.0,				// black
        TRUE						// wait for completion
        );

    // reattach canvasView to my canvasContainer or attach to fullscreen window
    if (isFullscreen) {
        [canvasContainer setContentView:canvasView];
        [fullscreenWindow close];
        fullscreenWindow = nil;
        [self makeKeyAndOrderFront:nil];
        isFullscreen = FALSE;
    } else {
        [self orderOut:nil];
        fullscreenWindow = [[FullscreenWindow alloc] init];
        [fullscreenWindow setToggler:self];
        [fullscreenWindow setContentView:canvasView];
        [fullscreenWindow makeKeyAndOrderFront:self];
        isFullscreen = TRUE;
    }

    // fade in
    CGDisplayFade (
        displayFadeReservation,
        0.5,						// 0.5seconds
        kCGDisplayBlendSolidColor,	// starting state
        kCGDisplayBlendNormal,		// ending state
        0.0, 0.0, 0.0,				// black
        FALSE						// don't wait for completion
        );
    CGReleaseDisplayFadeReservation (displayFadeReservation);
}

// ----- copy & paste support -----

-(IBAction)copy:(id)sender
{
    NSString *data = [[VICEApplication theMachineController] readScreenOutput];
    if (data) {
        NSPasteboard *pb = [NSPasteboard generalPasteboard];
        [pb declareTypes:[NSArray arrayWithObject:NSStringPboardType]
            owner:self];
        [pb setString:data forType:NSStringPboardType];
    } else {
        NSBeep();
    }
}

-(IBAction)paste:(id)sender
{
    NSPasteboard *pb = [NSPasteboard generalPasteboard];
    NSString *type = [pb availableTypeFromArray:
                        [NSArray arrayWithObject:NSStringPboardType]];
    if (type) {
        NSString *value = [pb stringForType:NSStringPboardType];
        // type string on keyboard
        [[VICEApplication theMachineController] typeStringOnKeyboard:value toPetscii:YES];
    } else {
        NSBeep();
    }
}

@end
