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

#import "vicenotifications.h"
#import "peripheraldrawer.h"
#import "vicewindow.h"
#import "viceapplication.h"

#define STATUS_HEIGHT 22

@implementation VICEWindow

- (id)initWithRect:(NSRect)rect title:(NSString *)title
{
    NSSize size = rect.size;
    
    // setup canvas size
    unsigned int canvas_width = size.width;
    unsigned int canvas_height = size.height;
    original_canvas_size = size;

    // create main window
    unsigned int style =
        NSTitledWindowMask | NSClosableWindowMask |
        NSMiniaturizableWindowMask | NSResizableWindowMask;
    if ([[NSUserDefaults standardUserDefaults] boolForKey:@"Textured Windows"])
        style |= NSTexturedBackgroundWindowMask;
        
    rect.size.height += STATUS_HEIGHT;
    self = [super initWithContentRect:rect
                            styleMask:style
                              backing:NSBackingStoreBuffered
                                defer:NO];
    if(self==nil)
        return nil;

    // setup window
    [self setTitle:title];
    [self setContentMinSize:NSMakeSize(canvas_width / 2, STATUS_HEIGHT + canvas_height / 2)];

    // the container box for the canvas
    rect = NSMakeRect(0, STATUS_HEIGHT, canvas_width, canvas_height);
    canvasContainer = [[NSBox alloc] initWithFrame:rect];
    [canvasContainer setContentViewMargins:NSMakeSize(0, 0)];
    [canvasContainer setTitlePosition:NSNoTitle];
    [canvasContainer setBorderType:NSNoBorder]; // NSNoBorder/NSLineBorder/NSGrooveBorder
    [canvasContainer setAutoresizingMask: (NSViewWidthSizable | NSViewHeightSizable)];
    [[self contentView] addSubview:canvasContainer];
    
    // canvasView: the OpenGL render canvas
    rect = NSMakeRect(0, 0, canvas_width, canvas_height);
    canvasView = [[VICEGLView alloc] initWithFrame:rect];
    [canvasView setupTexture:size];
    [canvasView setAutoresizingMask: (NSViewWidthSizable | NSViewHeightSizable)];
    [canvasContainer addSubview:canvasView];

    // status line rect
    rect = NSMakeRect(20, 0, canvas_width - 50, STATUS_HEIGHT);
    statusBox = [[NSBox alloc] initWithFrame:rect];
    [statusBox setContentViewMargins:NSMakeSize(0, 0)];
    [statusBox setTitlePosition:NSNoTitle];
    [statusBox setBorderType:NSNoBorder]; // NSNoBorder/NSLineBorder/NSGrooveBorder
    [statusBox setAutoresizingMask: (NSViewWidthSizable | NSViewMaxYMargin)];
    [[self contentView] addSubview:statusBox];

    int speedWidth = NSWidth(rect)-(50+STATUS_HEIGHT*2);
    
    speedView = [[NSTextField alloc] initWithFrame:NSMakeRect(0, 0, speedWidth, STATUS_HEIGHT)];
    [speedView setDrawsBackground:NO];
    [speedView setEditable:NO];
    [speedView setBordered:NO];
    [speedView setAutoresizingMask: NSViewWidthSizable];
    [statusBox addSubview:speedView];

    recplayView = [[NSTextField alloc] initWithFrame:NSMakeRect(speedWidth, 0, 50, STATUS_HEIGHT)];
    [recplayView setDrawsBackground:NO];
    [recplayView setEditable:NO];
    [recplayView setBordered:NO];
    [recplayView setAlignment:NSRightTextAlignment];
    [recplayView setAutoresizingMask: NSViewMinXMargin];
    [statusBox addSubview:recplayView];

    joystickView1 = [[JoystickView alloc] initWithFrame:NSMakeRect(speedWidth+50,0,STATUS_HEIGHT,STATUS_HEIGHT)];
    [joystickView1 setAutoresizingMask: NSViewMinXMargin];
    [statusBox addSubview:joystickView1];

    joystickView2 = [[JoystickView alloc] initWithFrame:NSMakeRect(speedWidth+50+STATUS_HEIGHT,0,STATUS_HEIGHT,STATUS_HEIGHT)];
    [joystickView2 setAutoresizingMask: NSViewMinXMargin];
    [statusBox addSubview:joystickView2];

    // drawer
    peripheralDrawer = [[PeripheralDrawer alloc] initWithPreferredEdge:NSMaxXEdge];
    [peripheralDrawer setParentWindow:self];
    [peripheralDrawer open];

    [self setFrameAutosaveName:title];

    // register notifcations
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(displaySpeed:)
                                                 name:VICEDisplaySpeedNotification
                                               object:nil];

    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(displayPause:)
                                                 name:VICETogglePauseNotification
                                               object:nil];

    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(displayRecording:)
                                                 name:VICEDisplayRecordingNotification
                                               object:nil];

    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(displayPlayback:)
                                                 name:VICEDisplayPlaybackNotification
                                               object:nil];

    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(displayEventTime:)
                                                 name:VICEDisplayEventTimeNotification
                                               object:nil];

    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(displayJoystick:)
                                                 name:VICEDisplayJoystickNotification
                                               object:nil];

    // make top-level window
    [self makeKeyAndOrderFront:nil];
    
    isFullscreen = false;
    
    return self;
}

- (void)close
{
    // disable fullscreen on close
    if(isFullscreen)
        [self toggleFullscreen:nil];
        
    [super close];
}

- (void)dealloc
{
    [canvasView release];
    [recplayView release];
    [speedView release];
    [peripheralDrawer release];
    [statusBox release];

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
    original_canvas_size = size;
    [canvasView setupTexture:size];

    [self setContentMinSize:NSMakeSize(size.width / 2, STATUS_HEIGHT + size.height / 2)];
}

- (void)resizeToCanvasSize:(id)sender
{
    NSRect f = [self frame];
    NSSize s = [[self contentView] bounds].size;
    
    f.size.width  = original_canvas_size.width;
    f.size.height += original_canvas_size.height - s.height + STATUS_HEIGHT;
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
    float contentHeight = proposedFrameSize.height - titleHeight - STATUS_HEIGHT;

    if (modifierMask & NSAlternateKeyMask)
    {
        float ratio = floor(contentHeight / original_canvas_size.height + 0.5);
        if (ratio < 1.0)
            ratio = 1.0;
        contentHeight = original_canvas_size.height * ratio;
    }

    float aspect_ratio = original_canvas_size.width / original_canvas_size.height;
    float scaledWidth = contentHeight * aspect_ratio;
    return NSMakeSize(scaledWidth, contentHeight + titleHeight + STATUS_HEIGHT);
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
    if(isFullscreen) {
        [canvasContainer setContentView:canvasView];
        [fullscreenWindow close];
        fullscreenWindow = nil;
        [self makeKeyAndOrderFront:nil];
        isFullscreen = FALSE;
    } else {
        [self orderOut:nil];
        fullscreenWindow = [[FullscreenWindow alloc] init];
        [fullscreenWindow setDelegate:self];
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

// ---------- Status Display ----------

- (void)displaySpeed:(NSNotification*)notification
{
    NSDictionary *dict = [notification userInfo];
    NSString *s = [NSString stringWithFormat:@"%.1f%%, %.1f FPS%s",
                          [[dict objectForKey:@"speed"] floatValue],
                          [[dict objectForKey:@"frame_rate"] floatValue],
                          [[dict objectForKey:@"warp_enabled"] boolValue] ? " (warp)" : ""];

    [speedView setStringValue:s];
}

- (void)displayPause:(NSNotification*)notification
{
    BOOL pauseFlag = [[notification object] boolValue];
    if (pauseFlag)
        [speedView setStringValue:@"PAUSE"];
    else
        [speedView setStringValue:@""];
}

- (void)displayJoystick:(NSNotification *)notification
{
    NSDictionary *dict = [notification userInfo];
    int joyVal1 = [[dict objectForKey:@"joy1"] intValue];
    int joyVal2 = [[dict objectForKey:@"joy2"] intValue];
    
    [joystickView1 setJoyValue:joyVal1];
    [joystickView2 setJoyValue:joyVal2];
}

- (void)displayRecording:(NSNotification*)notification
{
    [recplayView setStringValue:[notification object]];
}

- (void)displayEventTime:(unsigned int)current totalTime:(unsigned int)total
{
    if (total > 0)
        [recplayView setToolTip:[NSString stringWithFormat:@"%.f%%", (float)current/total*100]];
    else
        [recplayView setToolTip:@""];
//    [recplayView animate:self];
}

@end
