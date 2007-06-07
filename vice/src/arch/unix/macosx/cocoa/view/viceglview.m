/*
 * viceglview.m - VICEGLView
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

#include <OpenGL/gl.h>
#include <OpenGL/glext.h>

#include "lib.h"

#import "viceglview.h"
#import "viceapplication.h"
#import "vicenotifications.h"

@implementation VICEGLView

- (id)initWithFrame:(NSRect)frame
{
    NSOpenGLPixelFormatAttribute attrs[] =
    {
        NSOpenGLPFAFullScreen,
        NSOpenGLPFAAccelerated,
        NSOpenGLPFADoubleBuffer,
        NSOpenGLPFANoRecovery,
        NSOpenGLPFAColorSize, 8,
        nil
    };
    
    // init with given format
    NSOpenGLPixelFormat * pf = [[NSOpenGLPixelFormat alloc] initWithAttributes:attrs];
    self = [super initWithFrame:frame
                    pixelFormat:pf];
    [pf release];
    if(self==nil)
        return nil;
    
    // init texture
    textureSize = NSMakeSize(0,0);
    textureData = NULL;

    // setup Drag & Drop
    [self registerForDraggedTypes:[NSArray arrayWithObject:NSFilenamesPboardType]];

    // setup keyboard
    lastKeyModifierFlags = 0;
    // setup mouse
    trackMouse = NO;
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(toggleMouse:)
                                                 name:VICEToggleMouseNotification
                                               object:nil];

    return self;
}

- (void)dealloc
{
    if(textureData!=NULL) {
        lib_free(textureData);
    }
    [super dealloc];
}

- (BOOL)isOpaque
{
    return YES;
}

- (BOOL)acceptsFirstResponder
{
    return YES;
}

// prepare open gl
- (void)prepareOpenGL
{
    glBindTexture(GL_TEXTURE_RECTANGLE_EXT, 1);
    glTexParameteri(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_WRAP_T, GL_CLAMP);    
}

// cocoa calls this if view resized
- (void)reshape
{
    [[self openGLContext] makeCurrentContext];

    NSRect rect = [self bounds];
    NSSize size = rect.size;
    glViewport(0, 0, size.width, size.height);
    
    // adjust mouse scales
    mouseXScale = textureSize.width  / size.width;
    mouseYScale = textureSize.height / size.height;
}

// redraw view
- (void)drawRect:(NSRect)r
{
    [[self openGLContext] makeCurrentContext];

    NSSize size = textureSize;

    glEnable(GL_TEXTURE_RECTANGLE_EXT);
    glBegin(GL_QUADS);
    {
        glTexCoord2i(0, size.height);          glVertex2i(-1, -1);
        glTexCoord2i(0, 0);                    glVertex2i(-1, 1);
        glTexCoord2i(size.width, 0);           glVertex2i(1, 1);
        glTexCoord2i(size.width, size.height); glVertex2i(1, -1);
    }
    glEnd();
    glFlush();
}

- (void)setupTexture:(NSSize)size
{
    textureSize = size;
    unsigned int dataSize = size.width * size.height * 4;

    if(textureData==NULL)
        textureData = (BYTE *)lib_malloc(dataSize*sizeof(BYTE));
    else
        textureData = (BYTE *)lib_realloc(textureData,dataSize*sizeof(BYTE));
    
    // make canvas context current
    [[self openGLContext] makeCurrentContext];

    // bind texture
    glBindTexture(GL_TEXTURE_RECTANGLE_EXT, 1);
    glTextureRangeAPPLE(GL_TEXTURE_RECTANGLE_EXT, dataSize, textureData);
    glPixelStorei(GL_UNPACK_CLIENT_STORAGE_APPLE, GL_TRUE);
    glTexParameteri(GL_TEXTURE_RECTANGLE_EXT,
                    GL_TEXTURE_STORAGE_HINT_APPLE, GL_STORAGE_SHARED_APPLE);

    glTexImage2D(GL_TEXTURE_RECTANGLE_EXT, 0, GL_RGBA,
                 textureSize.width, textureSize.height,
                 0, 
                 GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, 
                 textureData);
}

- (void)updateTexture
{
    [[self openGLContext] makeCurrentContext];

    glTexImage2D(GL_TEXTURE_RECTANGLE_EXT, 0, GL_RGBA,
                 textureSize.width, textureSize.height,
                 0, 
                 GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, 
                 textureData);

    [self setNeedsDisplay:YES];    
}

- (BYTE *)getCanvasBuffer
{
    return textureData;
}

- (int)getCanvasPitch
{
    return textureSize.width * 4;
}

- (int)getCanvasDepth
{
    return 32;
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
    return [[VICEApplication theMachineController] smartAttachImage:path];
}

// ----- Keyboard -----

- (void)keyUp:(NSEvent *)theEvent
{
    unsigned int code = [theEvent keyCode];
    [[VICEApplication theMachineController] keyReleased:code];
}

- (void)flagsChanged:(NSEvent *)theEvent
{
    unsigned int modifierFlags = [theEvent modifierFlags] &
        (NSAlphaShiftKeyMask | NSShiftKeyMask | NSControlKeyMask | NSAlternateKeyMask);

    if(modifierFlags != lastKeyModifierFlags) {
        unsigned int code = [theEvent keyCode];
        unsigned int changedFlags = modifierFlags ^ lastKeyModifierFlags;
        int i;
        for(i=0;i<NUM_MODIFIERS;i++) {
            unsigned int flag = 1<<i;
            if(changedFlags & flag) {
                modifierKeyCode[i] = code;
                if(modifierFlags & flag) {
                    [[VICEApplication theMachineController] keyPressed:code];
                } else {
                    [[VICEApplication theMachineController] keyReleased:code];
                }
            }
        }
        lastKeyModifierFlags = modifierFlags;
    }
}


- (void)keyDown:(NSEvent *)theEvent
{
    unsigned int modifierFlags = [theEvent modifierFlags] &
        (NSAlphaShiftKeyMask | NSShiftKeyMask | NSControlKeyMask | NSAlternateKeyMask);

    // modifiers have changed, too!
    /* this happens if e.g. a context menu was activated by Ctrl-click */
    if(modifierFlags != lastKeyModifierFlags) {
        unsigned int changedFlags = modifierFlags ^ lastKeyModifierFlags;
        int i;
        for (i=0;i<NUM_MODIFIERS;i++) {
            unsigned int flag = 1<<i;
            if(changedFlags & flag) {
                unsigned int code = modifierKeyCode[i];
                if(modifierFlags & flag) {
                    [[VICEApplication theMachineController] keyPressed:code];
                } else {
                    [[VICEApplication theMachineController] keyReleased:code];
                }
            }
        }
        lastKeyModifierFlags = modifierFlags;
    }

    if (![theEvent isARepeat] &&
        !([theEvent modifierFlags] & NSCommandKeyMask)) {
        unsigned int code = [theEvent keyCode];
        [[VICEApplication theMachineController] keyPressed:code];
    }
}

// ----- Mouse -----

- (BOOL)becomeFirstResponder
{
    if(trackMouse)
        [[self window] setAcceptsMouseMovedEvents:YES];
    return [super becomeFirstResponder];
}

- (BOOL)resignFirstResponder
{
    if(trackMouse)
        [[self window] setAcceptsMouseMovedEvents:NO];    
    return [super resignFirstResponder];
}

- (void)mouseMoved:(NSEvent *)theEvent
{
    [self mouseMove:[theEvent locationInWindow]];
}

- (void)mouseDragged:(NSEvent *)theEvent
{
    [self mouseMove:[theEvent locationInWindow]];
}

- (void)mouseMove:(NSPoint)pos
{
    if(trackMouse) {
        int px = (int)(pos.x * mouseXScale);
        int py = (int)(pos.y * mouseYScale);
        int w = (int)textureSize.width;
        int h = (int)textureSize.height;
        if((px>=0)&&(px<w)&&(py>=0)&&(py<h)) {
            [[VICEApplication theMachineController] mouseMoveToX:px andY:py];
        }
    }
}

- (void)mouseDown:(NSEvent *)theEvent
{
    if(trackMouse) {
        if([theEvent type]==NSLeftMouseDown) {
            [[VICEApplication theMachineController] mousePressed];
        }
    }
}

- (void)mouseUp:(NSEvent *)theEvent
{
    if(trackMouse) {
        if([theEvent type]==NSLeftMouseUp) {
            [[VICEApplication theMachineController] mouseReleased];
        }
    }
}

- (void)toggleMouse:(NSNotification *)notification
{
    NSDictionary *dict = [notification userInfo];
    trackMouse = [[dict objectForKey:@"mouse"] boolValue];

    if(trackMouse)
        [[self window] setAcceptsMouseMovedEvents:YES];
}

@end
