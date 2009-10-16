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

#include "lib.h"
#include "log.h"
#include "videoarch.h"

#import "viceglview.h"
#import "viceapplication.h"
#import "vicenotifications.h"

// import video log
extern log_t video_log;

@implementation VICEGLView

- (id)initWithFrame:(NSRect)frame
{
    // ----- OpenGL PixelFormat -----
    NSOpenGLPixelFormatAttribute attrs[] =
    {
        NSOpenGLPFAWindow,
        NSOpenGLPFAAccelerated,
        NSOpenGLPFADoubleBuffer,
        NSOpenGLPFAColorSize, (NSOpenGLPixelFormatAttribute)8,
//        NSOpenGLPFADepthSize, (NSOpenGLPixelFormatAttribute)16,
        (NSOpenGLPixelFormatAttribute)0
    };
    NSOpenGLPixelFormat * pf = [[NSOpenGLPixelFormat alloc] initWithAttributes:attrs];
    self = [super initWithFrame:frame
                    pixelFormat:pf];
    [pf release];
    if (self==nil)
        return nil;

    // ----- Drag & Drop -----
    [self registerForDraggedTypes:[NSArray arrayWithObject:NSFilenamesPboardType]];

    // ----- Mouse & Keyboard -----
    // setup keyboard
    lastKeyModifierFlags = 0;
    // setup mouse
    trackMouse = NO;
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(toggleMouse:)
                                                 name:VICEToggleMouseNotification
                                               object:nil];
    mouseHideTimer = nil;

    // ----- OpenGL -----
    // OpenGL locking and state
    glLock = [[NSRecursiveLock alloc] init];
    isOpenGLReady = NO;
    postponedReconfigure = NO; 
    
    // ----- DisplayLink -----
    displayLink = nil;
    displayLinkEnabled = NO;

    // ----- Texture -----
    [self initTextures];

    return self;
}

- (void)dealloc
{
    // ----- DisplayLink -----
    if(displayLink != nil) {
        [self shutdownDisplayLink];
    }
    
    // ----- OpenGL -----
    [glLock lock];
    [[self openGLContext] makeCurrentContext];
    [self deleteAllTextures];
    [glLock unlock];    
    [glLock release];
    
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

// ---------- interface -----------------------------------------------------

// called on startup and every time video param changes
- (void)reconfigure:(struct video_param_s *)param
{
    // copy params
    if(param != NULL) {
        memcpy(&video_param, param, sizeof(struct video_param_s));
    }

    // if OpenGL is not initialized yet then postpone reconfigure
    if(!isOpenGLReady) {
        postponedReconfigure = YES;
        return;
    }
    
    log_message(video_log, "reconfiguring display");
    
    // do sync draw
    if(video_param.sync_draw) {
        [self setupTextures:video_param.sync_draw_buffers withSize:textureSize];
        if(displayLink == nil) {
            displayLinkLocked = NO;
            displayLinkEnabled = [self setupDisplayLink];
            log_message(video_log, "display link enabled: %s", (displayLinkEnabled ? "ok":"ERROR"));
        }
    } 
    // no sync draw
    else {
        [self setupTextures:1 withSize:textureSize];
        if(displayLink != nil) {
            [self shutdownDisplayLink];
            displayLinkEnabled = NO;
            log_message(video_log, "display link disabled");
        }
    }
    
}

// called if the canvas size was changed by the machine (not the user!)
- (void)resize:(NSSize)size
{
    // if OpenGL is not initialized then keep size and return
    if(!isOpenGLReady) {
        textureSize = size;
        return;
    }
    
    log_message(video_log, "resize canvas %g x %g", size.width, size.height);
    [self setupTextures:numTextures withSize:size];
}

// the emulation wants to draw a new frame
- (BYTE *)beginMachineDraw
{
    if(numTextures == 0)
        return NULL;
    
    return texture[0].buffer;
}

// the emulation did finish drawing a new frame
- (void)endMachineDraw
{
    [self updateTexture:0];
    
    if(!displayLinkEnabled) {
        [self setNeedsDisplay:YES];
    }
}

- (int)getCanvasPitch
{
    return textureSize.width * 4;
}

- (int)getCanvasDepth
{
    return 32;
}

- (void)setCanvasId:(int)c
{
    canvasId = c;
}

- (int)canvasId
{
    return canvasId;
}

// ---------- Cocoa Calls ---------------------------------------------------

// prepare open gl: called by view
- (void)prepareOpenGL
{
    [glLock lock];
    
    // sync to VBlank
    GLint swapInt = 1;
    [[self openGLContext] setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];
    
    glDisable (GL_ALPHA_TEST);
    glDisable (GL_DEPTH_TEST);
    glDisable (GL_SCISSOR_TEST);
    glDisable (GL_DITHER);
    glDisable (GL_CULL_FACE);
    glColorMask (GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glDepthMask (GL_FALSE);
    glStencilMask (0);
    glClearColor (0.0f, 0.0f, 0.0f, 0.0f);
    glHint (GL_TRANSFORM_HINT_APPLE, GL_FASTEST);

    glEnable(GL_TEXTURE_RECTANGLE_EXT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
 
    [glLock unlock];
    
    isOpenGLReady = true;

    // call postponed configure
    if(postponedReconfigure) {
        [self reconfigure:NULL];
    }
}

// cocoa calls this if view resized
- (void)reshape
{
    NSRect rect = [self bounds];
    NSSize size = rect.size;

    [glLock lock];
    [[self openGLContext] makeCurrentContext];
    
    // reshape viewport so that the texture size fits in without ratio distortion
    float ratio = size.width / size.height;
    float textureRatio = textureSize.width / textureSize.height;
    viewSize    = size;
    viewOrigin  = NSMakePoint(0.0,0.0);
    if (ratio < (textureRatio-0.01)) {
        // place along y
        viewSize.height = viewSize.width / textureRatio;
        viewOrigin.y = (size.height - viewSize.height) / 2.0; 
    } else if (ratio > (textureRatio+0.01)) {
        // place along x
        viewSize.width = viewSize.height * textureRatio;
        viewOrigin.x = (size.width - viewSize.width) / 2.0;
    }
    
    glViewport(viewOrigin.x, viewOrigin.y, viewSize.width, viewSize.height);
    
    // adjust mouse scales
    mouseXScale = textureSize.width  / viewSize.width;
    mouseYScale = textureSize.height / viewSize.height;

    [glLock unlock];
}

// redraw view
- (void)drawRect:(NSRect)r
{
    [glLock lock];
    [[self openGLContext] makeCurrentContext];

    NSSize size = textureSize;

    glClear(GL_COLOR_BUFFER_BIT);

    if(numTextures > 0) {
        glBindTexture(GL_TEXTURE_RECTANGLE_EXT, texture[0].bindId);
        glBegin(GL_QUADS);
        {
            float alpha = 1.0;
            glColor4f(1.0f,1.0f,1.0f,alpha);
            glTexCoord2i(0, size.height);          glVertex2i(-1, -1);
            glTexCoord2i(0, 0);                    glVertex2i(-1, 1);
            glTexCoord2i(size.width, 0);           glVertex2i(1, 1);
            glTexCoord2i(size.width, size.height); glVertex2i(1, -1);
        }
        glEnd();
    }
    
    //glFlush();
    [[self openGLContext] flushBuffer];
    [glLock unlock];
}

// ---------- Texture Management --------------------------------------------

- (void)initTextures
{
    int i;
    
    numTextures = 0;
    for(i=0;i<MAX_BUFFERS;i++) {
        texture[i].buffer = NULL;
    }
}

- (void)deleteTexture:(int)tid
{
    lib_free(texture[tid].buffer);
    texture[tid].buffer = NULL;
    
    glDeleteTextures(1,&texture[tid].bindId);
}

- (void)deleteAllTextures
{
    int i;
    for(i=0;i<numTextures;i++) {
        [self deleteTexture:i];
    }
    numTextures = 0;
}

- (void)setupTextures:(int)num withSize:(NSSize)size
{
    int i;
    
    log_message(video_log, "setup textures: #%d %g x %g (was: #%d %g x %g)", 
                num, size.width, size.height, 
                numTextures, textureSize.width, textureSize.height);

    // clean up old textures
    if(numTextures > num) {
        for(i=num;i<numTextures;i++) {
            [self deleteTexture:i];
        }
    }
    
    // if size differs then reallocate all otherwise only missing
    int start;
    if((size.width != textureSize.width)||(size.height != textureSize.height)) {
        start = 0;
    } else {
        start = numTextures;
    }
    
    // now adopt values
    textureSize = size;
    numTextures = num;    
    unsigned int dataSize = size.width * size.height * 4;

    // setup texture memory
    for(i=start;i<numTextures;i++) {
        if (texture[i].buffer==NULL)
            texture[i].buffer = lib_malloc(dataSize * sizeof(BYTE));
        else
            texture[i].buffer = lib_realloc(texture[i].buffer, dataSize * sizeof(BYTE));
    
        // memory error
        if(texture[i].buffer == NULL) {
            numTextures = i;
            return;
        }
    
        // clear new texture - make sure alpha is set
        memset(texture[i].buffer,0,dataSize*sizeof(BYTE));
    }
    
    // make GL context current
    [glLock lock];
    [[self openGLContext] makeCurrentContext];

    // bind textures and initialize them
    for(i=start;i<numTextures;i++) {
        glGenTextures(1, &texture[i].bindId);
        glBindTexture(GL_TEXTURE_RECTANGLE_EXT, texture[i].bindId);
        BYTE *data = texture[i].buffer;
        
        glTextureRangeAPPLE(GL_TEXTURE_RECTANGLE_EXT, dataSize, data);
        glPixelStorei(GL_UNPACK_CLIENT_STORAGE_APPLE, GL_TRUE);
        glTexParameteri(GL_TEXTURE_RECTANGLE_EXT,
                        GL_TEXTURE_STORAGE_HINT_APPLE, GL_STORAGE_SHARED_APPLE);

        glTexParameteri(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_WRAP_T, GL_CLAMP);    

        glTexEnvi(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_ENV_MODE, GL_DECAL);

        glTexImage2D(GL_TEXTURE_RECTANGLE_EXT, 0, GL_RGBA,
                     textureSize.width, textureSize.height,
                     0, 
                     GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, 
                     data);
    }

    [glLock unlock];
}

- (void)updateTexture:(int)i
{
    [glLock lock];
    [[self openGLContext] makeCurrentContext];

    glBindTexture(GL_TEXTURE_RECTANGLE_EXT, texture[i].bindId);
    glTexImage2D(GL_TEXTURE_RECTANGLE_EXT, 0, GL_RGBA,
                 textureSize.width, textureSize.height,
                 0, 
                 GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, 
                 texture[i].buffer);

    [glLock unlock];
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

    if (modifierFlags != lastKeyModifierFlags) {
        unsigned int code = [theEvent keyCode];
        unsigned int changedFlags = modifierFlags ^ lastKeyModifierFlags;
        int i;
        for (i=0;i<NUM_KEY_MODIFIERS;i++) {
            unsigned int flag = 1<<i;
            if (changedFlags & flag) {
                modifierKeyCode[i] = code;
                if (modifierFlags & flag) {
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
    if (modifierFlags != lastKeyModifierFlags) {
        unsigned int changedFlags = modifierFlags ^ lastKeyModifierFlags;
        int i;
        for (i=0;i<NUM_KEY_MODIFIERS;i++) {
            unsigned int flag = 1<<i;
            if (changedFlags & flag) {
                unsigned int code = modifierKeyCode[i];
                if (modifierFlags & flag) {
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

- (void)startHideTimer
{
    if (mouseHideTimer==nil) {
        // setup timer for mouse hide
        mouseHideInterval = MOUSE_HIDE_DELAY;
        mouseHideTimer = [NSTimer scheduledTimerWithTimeInterval: 0.5
                                                          target: self 
                                                        selector: @selector(hideTimer:)
                                                        userInfo: nil 
                                                         repeats: YES];
        [mouseHideTimer fire];
        [mouseHideTimer retain];
    }
}

- (void)stopHideTimer:(BOOL)shown
{
    if (mouseHideTimer!=nil) {
        // remove timer
        [mouseHideTimer invalidate];
        [mouseHideTimer release];
        mouseHideTimer = nil;
    }
    
    if (shown) {
        if (mouseHideInterval != MOUSE_IS_SHOWN) {
            [NSCursor setHiddenUntilMouseMoves:NO];
            mouseHideInterval = MOUSE_IS_SHOWN;
        }
    } else {
        if (mouseHideInterval != MOUSE_IS_HIDDEN) {
            [NSCursor setHiddenUntilMouseMoves:YES];
            mouseHideInterval = MOUSE_IS_HIDDEN;
        }
    }
}

- (void)hideTimer:(NSTimer *)timer
{
    if (mouseHideInterval>0) {
        mouseHideInterval--;
    } else if (mouseHideInterval==0) {
        [self stopHideTimer:FALSE];
    }
}

- (void)ensureMouseShown
{
    // in mouse tracking the mouse is always visible
    if (trackMouse)
        return;
    
    // reshow mouse if it was hidden
    if (mouseHideInterval == MOUSE_IS_HIDDEN) {
        [NSCursor setHiddenUntilMouseMoves:NO];
    }
    mouseHideInterval = MOUSE_HIDE_DELAY;
    [self startHideTimer];
}

- (BOOL)becomeFirstResponder
{
    [[self window] setAcceptsMouseMovedEvents:YES];
    
    // report current canvas id to app controller
    [VICEApplication setCurrentCanvasId:canvasId];
    
    // start mouse hide timer
    if (!trackMouse) {
        [self startHideTimer];
    }

    return [super becomeFirstResponder];
}

- (BOOL)resignFirstResponder
{
    [[self window] setAcceptsMouseMovedEvents:NO];
    
    // show mouse again
    if (!trackMouse) {
        [self stopHideTimer:TRUE];
    }

    return [super resignFirstResponder];
}

- (void)mouseMoved:(NSEvent *)theEvent
{
    NSPoint location = [self convertPoint:[theEvent locationInWindow] fromView:nil];
    [self mouseMove:location];
    
    // check if mouse is in view
    BOOL inView = NSPointInRect(location,[self bounds]);
    if (inView) {
        [self ensureMouseShown];        
    } else {
        [self stopHideTimer:TRUE];
    }
}

- (void)mouseDragged:(NSEvent *)theEvent
{
    NSPoint location = [self convertPoint:[theEvent locationInWindow] fromView:nil];
    [self mouseMove:location];
}

- (void)mouseMove:(NSPoint)pos
{
    if (trackMouse) {
        int w = (int)textureSize.width;
        int h = (int)textureSize.height;
        int px = (int)((pos.x-viewOrigin.x) * mouseXScale);
        int py = (int)((pos.y-viewOrigin.y) * mouseYScale);
        py = h - 1 - py;
        if ((px>=0)&&(px<w)&&(py>=0)&&(py<h)) {
            [[VICEApplication theMachineController] mouseMoveToX:px andY:py];
        }
    }
}

- (void)mouseDown:(NSEvent *)theEvent
{
    if ([theEvent type]==NSLeftMouseDown) {
        if (trackMouse) {
            [[VICEApplication theMachineController] mousePressed];
        } else {
            [self stopHideTimer:TRUE];
        }
    }
}

- (void)mouseUp:(NSEvent *)theEvent
{
    if ([theEvent type]==NSLeftMouseUp) {
        if (trackMouse) {
            [[VICEApplication theMachineController] mouseReleased];
        } else {
            [self startHideTimer];
        }
    }
}

- (void)toggleMouse:(NSNotification *)notification
{
    NSDictionary *dict = [notification userInfo];
    trackMouse = [[dict objectForKey:@"mouse"] boolValue];

    if (trackMouse) {
        [self stopHideTimer:TRUE];
    } else {
        [self startHideTimer];
    }
}

// ---------- display link stuff --------------------------------------------

unsigned long last = 0;
unsigned long last_delta = 0;

- (CVReturn)displayLinkCallback
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    
    // not locked yet?
    if(!displayLinkLocked) {
        screenRefreshPeriod = [self getDisplayLinkRefreshPeriod];
        if(screenRefreshPeriod != 0.0) {
            displayLinkLocked = YES;
            float rate = 1000.0f / screenRefreshPeriod;
            log_message(video_log, "locked to screen refresh period=%g ms, rate=%g Hz",
                        screenRefreshPeriod, rate);
        }
    }
    
    // do drawing
    [self drawRect:NSZeroRect];
           
    [pool release];
    
    return kCVReturnSuccess;
}

static CVReturn MyDisplayLinkCallback(CVDisplayLinkRef displayLink, 
                                      const CVTimeStamp* nowTime,
                                      const CVTimeStamp* outputTime, 
                                      CVOptionFlags flagsIn,
                                      CVOptionFlags* flagsOut,
                                      void* displayLinkContext)
{
    VICEGLView *view = (VICEGLView *)displayLinkContext;
    CVReturn result = [view displayLinkCallback];
    return result;
}

- (BOOL)setupDisplayLink
{        
    CVReturn r;
    
    // Create a display link capable of being used with all active displays
    r = CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);
    if( r != kCVReturnSuccess )
        return NO;

    // Set the renderer output callback function
    r = CVDisplayLinkSetOutputCallback(displayLink, &MyDisplayLinkCallback, self);
    if( r != kCVReturnSuccess )
        return NO;

    // Set the display link for the current renderer
    CGLContextObj cglContext = [[self openGLContext] CGLContextObj];
    CGLPixelFormatObj cglPixelFormat = [[self pixelFormat] CGLPixelFormatObj];
    r = CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(displayLink, cglContext, cglPixelFormat);
    if( r != kCVReturnSuccess )
        return NO;

    // Activate the display link
    r = CVDisplayLinkStart(displayLink);
    if( r != kCVReturnSuccess )
        return NO;
    
    return YES;
}

- (void)shutdownDisplayLink
{
    // Release the display link
    CVDisplayLinkRelease(displayLink);
    displayLink = NULL;
}

- (float)getDisplayLinkRefreshPeriod
{
    // return output video rate
    if(displayLink != nil) {
        // in ms!
        return (float)(CVDisplayLinkGetActualOutputVideoRefreshPeriod(displayLink) * 1000.0f);
    } else {
        return 0.0;
    }
}

@end
