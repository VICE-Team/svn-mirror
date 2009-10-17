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

#include <libkern/OSAtomic.h>

#include "lib.h"
#include "log.h"
#include "videoarch.h"

#import "viceglview.h"
#import "viceapplication.h"
#import "vicenotifications.h"

// import video log
extern log_t video_log;

//#define DEBUG_SYNC

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
    glContext = nil;
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
    if([NSOpenGLContext currentContext] != glContext)
        [glContext makeCurrentContext];
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
        
        int numDrawBuffers = video_param.sync_draw_buffers;
        
        [self setupTextures:numDrawBuffers withSize:textureSize];
        
        multiBufferEnabled = (numDrawBuffers > 1);
        
        // enable display link
        if(displayLink == nil) {
            displayLinkSynced = NO;
            displayLinkEnabled = [self setupDisplayLink];
            log_message(video_log, "display link enabled: %s", (displayLinkEnabled ? "ok":"ERROR"));
        }
    } 
    // no sync draw
    else {
        
        [self setupTextures:1 withSize:textureSize];
        
        multiBufferEnabled = NO;
        
        // disable display link
        if(displayLink != nil) {
            [self shutdownDisplayLink];
            displayLinkEnabled = NO;
            log_message(video_log, "display link disabled");
        }
    }
    
    // init buffer setup
    numDrawn = 0;
    writePos = 0;
    displayPos = multiBufferEnabled ? 1 : 0;
    lockTime = 0;
    blendAlpha = 1.0f;
    syncWritePos = 0;
    firstDrawTime = 0;
    lastDrawTime = 0;
    
    int i;
    for(i=0;i<MAX_BUFFERS;i++) {
        texture[i].timeStamp = 0;
        texture[i].frameNo = -1;
    }
    
    // configure GL blending
    [self toggleBlending:multiBufferEnabled];
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

// the emulation wants to draw a new frame (called from machine thread!)
- (BYTE *)beginMachineDraw:(unsigned long)timeStamp frame:(int)frameNo
{
    if(firstDrawTime == 0) {
        firstDrawTime = timeStamp;
    }
    
    // no drawing possible right now
    if(numTextures == 0) {
        log_message(video_log, "FATAL: no textures to draw...");
        return NULL;
    }
    
    if(multiBufferEnabled) {

        // same frame
        if(texture[writePos].frameNo == frameNo) {
#ifdef DEBUG_SYNC
            printf("COMPENSATE: %d\n", frameNo);
#endif
            overwriteBuffer = YES;
        }
        // delta too small
        if((timeStamp - lastDrawTime) < 1000) {
#ifdef DEBUG_SYNC
            printf("COMPENSATE2: %d\n", frameNo);
#endif
            overwriteBuffer = YES;            
        }
        // no buffer free - need to overwrite the last one
        else if(numDrawn == numTextures) {
#ifdef DEBUG_SYNC
            printf("OVERWRITE: %d -> #%d\n", frameNo, writePos);
#endif
            overwriteBuffer = YES;
        } 
        // use next free buffer
        else {
            int oldPos = writePos;
            overwriteBuffer = NO;
            writePos++;
            if(writePos == numTextures)
                writePos = 0;
            
            // copy current image as base for next buffer (VICE does partial updates)
            memcpy(texture[writePos].buffer, texture[oldPos].buffer, textureByteSize);      
        }
    }
    
    // store time stamp
    texture[writePos].timeStamp = timeStamp;
    texture[writePos].frameNo   = frameNo;
    lastDrawTime = timeStamp;
    
    return texture[writePos].buffer;
}

// the emulation did finish drawing a new frame (called from machine thread!)
- (void)endMachineDraw
{
    // update drawn texture
    [self updateTexture:writePos];
    
    if(multiBufferEnabled) {    
        // count written buffer
        if(!overwriteBuffer) {
            OSAtomicIncrement32(&numDrawn);
        }

        unsigned long ltime = 0;
        if(lockTime != 0) {
            ltime = texture[writePos].timeStamp - firstDrawTime;
            ltime /= 1000;
        }    
#ifdef DEBUG_SYNC
        printf("D %lu:  - draw #%d (total %d, frame %d)\n", ltime, writePos, numDrawn, texture[writePos].frameNo);
#endif
    }
    
    // if no display link then trigger redraw here
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
    glContext = [self openGLContext];
    
    // sync to VBlank
    GLint swapInt = 1;
    [glContext setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];
    
    glDisable (GL_ALPHA_TEST);
    glDisable (GL_DEPTH_TEST);
    glDisable (GL_SCISSOR_TEST);
    glDisable (GL_DITHER);
    glDisable (GL_CULL_FACE);
    glColorMask (GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glDepthMask (GL_FALSE);
    glStencilMask (0);
    glClearColor (0.0f, 0.0f, 0.0f, 0.0f);
    glDisable (GL_BLEND);
    glHint (GL_TRANSFORM_HINT_APPLE, GL_FASTEST);

    glEnable(GL_TEXTURE_RECTANGLE_EXT);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
 
    [glLock unlock];
    
    isOpenGLReady = true;

    // call postponed configure
    if(postponedReconfigure) {
        [self reconfigure:NULL];
    }
}

- (void)toggleBlending:(BOOL)on
{
    [glLock lock];
    if([NSOpenGLContext currentContext] != glContext)
        [glContext makeCurrentContext];

    if(on)
        glEnable(GL_BLEND);
    else
        glDisable(GL_BLEND);
        
    [glLock unlock];
}

// cocoa calls this if view resized
- (void)reshape
{
    NSRect rect = [self bounds];
    NSSize size = rect.size;

    [glLock lock];
    if([NSOpenGLContext currentContext] != glContext)
        [glContext makeCurrentContext];
    
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

- (void)drawQuad:(float)alpha
{
    glBegin(GL_QUADS);
    {
        glColor4f(1.0f,1.0f,1.0f,alpha);
        glTexCoord2i(0, textureSize.height);          glVertex2i(-1, -1);
        glTexCoord2i(0, 0);                           glVertex2i(-1, 1);
        glTexCoord2i(textureSize.width, 0);           glVertex2i(1, 1);
        glTexCoord2i(textureSize.width, textureSize.height); glVertex2i(1, -1);
    }
    glEnd();
}

// redraw view
- (void)drawRect:(NSRect)r
{
    [glLock lock];
    if([NSOpenGLContext currentContext] != glContext)
        [glContext makeCurrentContext];

    glClear(GL_COLOR_BUFFER_BIT);

    // anything to draw?
    if(numTextures > 0) {
        // multi buffer blends two textures
        if(multiBufferEnabled) {

            // calc blend position and determine weights
            int pos = [self calcBlend:vsyncarch_gettime()];
            //printf("\tdisplay #%d (alpha=%g)\n", pos, blendAlpha);

            if(blendAlpha > 0.0f) {
                glBindTexture(GL_TEXTURE_RECTANGLE_EXT, texture[pos].bindId);
                [self drawQuad:blendAlpha];
            }

            float beta = 1.0f - blendAlpha;
            if(beta > 0.0f) {
                pos++;
                if(pos == numTextures)
                    pos = 0;
                glBindTexture(GL_TEXTURE_RECTANGLE_EXT, texture[pos].bindId);
                [self drawQuad:1.0 - blendAlpha];
            }
        }
        // non-multi normal draw
        else {
            glBindTexture(GL_TEXTURE_RECTANGLE_EXT, texture[0].bindId);
            [self drawQuad:1.0f];
        }
    }
    
    [[self openGLContext] flushBuffer];
    [glLock unlock];
}

// ---------- Multi Buffer Blending -----------------------------------------

- (int)calcBlend:(unsigned long)now
{
    // need a lock?
    if(lockTime == 0) {
        // nothing to do yet
        if((numDrawn==0)||(firstDrawTime==0)||(screenRefreshPeriod==0.0f)) {
            blendAlpha = 1.0f;
            return displayPos;
        }
    
        // set lock time and determine display vs draw time delta
        lockTime = firstDrawTime;
        drawDisplayDelta = (unsigned long)(screenRefreshPeriod * 500.0f);
        
#ifdef DEBUG_SYNC
        printf("locked with delta: %lu ns\n", drawDisplayDelta);
#endif
    }
    
    // convert now render time to frame time
    unsigned long frameNow = now - drawDisplayDelta;
    
    // find display frame interval where we fit in
    int np = displayPos;
    int nd = numDrawn;    
    int i = 0;

    for(i=0;i<nd;i++) {
        // next timestamp is larger -> keep i
        if(texture[np].timeStamp > frameNow)
            break;
        
        // next slot in ring buffer
        np++;
        if(np == numTextures)
            np = 0;            
    }
    
    // display is beyond current frame
    BOOL beyond = NO;
    BOOL before = NO;    
    if(i==0) {
        before = YES;
    } else {
        if(i == nd) {
            beyond = YES;
        }
        i--;
    }
    
    unsigned long ltime = frameNow - firstDrawTime;
    ltime /= 1000;
    
    // skip now unused frames... make room for drawing
    if(i>0) {
        OSAtomicAdd32(-i, &numDrawn);
        //printf("R %lu: skipping %d -> numDrawn: %d\n", ltime, i, numDrawn);
    }
    
    // before first frame
    if(before) {
        displayPos = (displayPos + i) % numTextures;
        blendAlpha = 1.0f;
        
#ifdef DEBUG_SYNC
        unsigned long delta = texture[displayPos].timeStamp - frameNow; 
        printf("R %lu: BEFORE: #%d delta=%lu skip=%d\n", ltime, displayPos, delta, i);
#endif
    }
    // beyond last frame
    else if(beyond) {
        displayPos = (displayPos + i) % numTextures;
        blendAlpha = 1.0f;
        
#ifdef DEBUG_SYNC
        unsigned long delta = frameNow - texture[displayPos].timeStamp;        
        printf("R %lu: BEYOND: #%d delta=%lu skip=%d\n", ltime, displayPos, delta, i);
#endif
    } 
    // between two frames
    else {
        int a = (displayPos + i) % numTextures;
        int b = (displayPos + i + 1) % numTextures;
        
        displayPos = a;
        unsigned long frameDelta = texture[b].timeStamp - texture[a].timeStamp;
        unsigned long dispDelta  = texture[b].timeStamp - frameNow;
        blendAlpha = (float)dispDelta / (float)frameDelta;

#ifdef DEBUG_SYNC
        printf("R %lu: between: #%d [%d=-%lu,%d=%lu] skip=%d -> alpha=%g\n", 
               ltime, displayPos,
               a, frameNow - texture[a].timeStamp,
               b, texture[b].timeStamp - frameNow,
               i,
               blendAlpha);
#endif
    }
    
    return displayPos;
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
        [glLock lock];
        if([NSOpenGLContext currentContext] != glContext)
            [glContext makeCurrentContext];

        for(i=num;i<numTextures;i++) {
            [self deleteTexture:i];
        }
        [glLock unlock];
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
    textureByteSize = size.width * size.height * 4;

    // setup texture memory
    for(i=start;i<numTextures;i++) {
        if (texture[i].buffer==NULL)
            texture[i].buffer = lib_malloc(textureByteSize);
        else
            texture[i].buffer = lib_realloc(texture[i].buffer, textureByteSize);
    
        // memory error
        if(texture[i].buffer == NULL) {
            numTextures = i;
            return;
        }
    
        // clear new texture - make sure alpha is set
        memset(texture[i].buffer, 0, textureByteSize);
    }
    
    // make GL context current
    [glLock lock];
    if([NSOpenGLContext currentContext] != glContext)
        [glContext makeCurrentContext];

    // bind textures and initialize them
    for(i=start;i<numTextures;i++) {
        glGenTextures(1, &texture[i].bindId);
        glBindTexture(GL_TEXTURE_RECTANGLE_EXT, texture[i].bindId);
        BYTE *data = texture[i].buffer;
        
        glTextureRangeAPPLE(GL_TEXTURE_RECTANGLE_EXT, textureByteSize, data);
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
    if([NSOpenGLContext currentContext] != glContext)
        [glContext makeCurrentContext];

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

- (CVReturn)displayLinkCallback
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    
    // not synced yet?
    if(!displayLinkSynced) {
        screenRefreshPeriod = [self getDisplayLinkRefreshPeriod];
        if(screenRefreshPeriod != 0.0) {
            displayLinkSynced = YES;
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
