/*
 * viceglview.h - VICEGLView
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

#include "video.h"

#import <Cocoa/Cocoa.h>

@interface VICEGLView : NSOpenGLView
{
    NSSize  textureSize;
    BYTE   *textureData;
    unsigned int lastKeyModifierFlags;
    #define NUM_MODIFIERS 32
    unsigned int modifierKeyCode[NUM_MODIFIERS];
    BOOL trackMouse;
    float mouseXScale;
    float mouseYScale;
    int oldX;
    int oldY;
}

- (id)initWithFrame:(NSRect)rect;
- (void)dealloc;
- (BOOL)isOpaque;

- (void)prepareOpenGL;
- (void)reshape;
- (void)drawRect:(NSRect)r;

- (void)setupTexture:(NSSize)size;
- (void)updateTexture;

- (BYTE *)getCanvasBuffer;
- (int)getCanvasPitch;
- (int)getCanvasDepth;

- (void)keyDown:(NSEvent *)theEvent;
- (void)keyUp:(NSEvent *)theEvent;
- (void)flagsChanged:(NSEvent *)theEvent;

- (BOOL)becomeFirstResponder;
- (BOOL)resignFirstResponder;
- (void)mouseMoved:(NSEvent *)theEvent;
- (void)mouseDragged:(NSEvent *)theEvent;
- (void)mouseDown:(NSEvent *)theEvent;
- (void)mouseUp:(NSEvent *)theEvent;
- (void)mouseMove:(NSPoint)pos;
- (void)toggleMouse:(NSNotification *)notification;

@end

