/*
 * vicewindow.h - VICEWindow
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

#import <Cocoa/Cocoa.h>
#import "viceglview.h"
#import "joystickview.h"
#import "fullscreenwindow.h"

@interface VICEWindow : NSWindow
{
    NSBox *canvasContainer;
    VICEGLView *canvasView;
    
    NSBox * statusBox;
    NSTextField * recplayView;
    NSTextField * speedView;
    JoystickView *joystickView1;
    JoystickView *joystickView2;

    NSDrawer * peripheralDrawer;

    NSSize original_canvas_size;
    BOOL isFullscreen;
    FullscreenWindow *fullscreenWindow;
}

// create the window and setup canvas structure
- (id)initWithRect:(NSRect)rect title:(NSString *)title;

// return the GLView for render access
- (VICEGLView *)getVICEGLView;
// resize canvas
- (void)resizeCanvas:(NSSize)size;

// close window -> terminate app
- (BOOL)windowShouldClose:(id)sender;

// toggle fullscreen of canvas
- (void)toggleFullscreen:(id)sender;
// restore original size
- (void)resizeToCanvasSize:(id)sender;

-(void)updateCanvas:(id)sender;

@end

