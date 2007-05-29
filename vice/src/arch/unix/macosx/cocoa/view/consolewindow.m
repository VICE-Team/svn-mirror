/*
 * consolewindow.m - ConsoleWindow
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

#import "consolewindow.h"
#import "logview.h"

@implementation ConsoleWindow

- (id)initWithContentRect:(NSRect)rect title:(NSString *)title
{
    unsigned int style = NSUtilityWindowMask | NSTitledWindowMask |
        NSClosableWindowMask | NSMiniaturizableWindowMask | NSResizableWindowMask;
    self = [super initWithContentRect:rect
                            styleMask:style
                              backing:NSBackingStoreBuffered
                                defer:NO];
    if(self==nil)
        return nil;

    [self setReleasedWhenClosed:NO];

    scroll = [[NSScrollView alloc] initWithFrame:NSMakeRect(0, 0,
                                                            NSWidth(rect), NSHeight(rect))];
    [scroll setAutoresizingMask: (NSViewWidthSizable | NSViewHeightSizable)];
    [scroll setHasVerticalScroller:YES];
    [scroll setHasHorizontalScroller:NO];

    NSSize size = [scroll contentSize];

    log_view = [[LogView alloc] initWithFrame:NSMakeRect(0, 0, size.width, size.height)];
    [log_view setFont:[NSFont userFixedPitchFontOfSize:0]];
    [log_view setAutoresizingMask: (NSViewWidthSizable | NSViewHeightSizable)];
    [log_view setEditable:NO];
    [log_view setHorizontallyResizable:YES];
    [[log_view textContainer] setContainerSize:NSMakeSize(FLT_MAX, FLT_MAX)];
    [[log_view textContainer] setWidthTracksTextView:YES];

    [scroll setDocumentView:log_view];
    [[self contentView] addSubview:scroll];

    // set title
    [self setTitle:title];
    [self setFrameAutosaveName:[self title]];

    // init log file
    log_file = 0;
    log_pipe = nil;

    return self;
}

#define BUFFER_SIZE 1024

- (void)appendText:(NSString*)text
{
    BOOL doFlush = false;
    if(buffer==nil)
        buffer = [[NSMutableString alloc] initWithCapacity:BUFFER_SIZE];

    [buffer appendString:text];
    if([buffer length] >= BUFFER_SIZE)
        [self flushBuffer];
}

- (void)flushBuffer
{
    if(buffer==nil)
        return;
    
    NSRange end = [log_view rangeForUserTextChange];
    [log_view replaceCharactersInRange:end withString:buffer];
    NSRange newEnd = [log_view rangeForUserTextChange];
    [log_view scrollRangeToVisible:newEnd];
    
    [buffer release];
    buffer = nil;
}

- (void)dealloc
{    
    [scroll release];
    [log_view release];

    fclose(log_file);
    [log_pipe dealloc];

    [super dealloc];
}

- (void)orderOut:(id)sender
{
    if ([log_view isEditable])
        [NSApp stopModal];

    [super orderOut:sender];
}

- (BOOL)canBecomeMainWindow
{
    return NO;
}

- (void)endConsoleInput:(id)v
{
    [NSApp stopModal];
}

- (NSString*)readline:(NSString *)prompt
{
    [self makeKeyAndOrderFront:nil];

    [self appendText:prompt];
    if(buffer!=nil)
        [self flushBuffer];

    [log_view setEditable:YES];
    [(LogView*)log_view setTarget:self];
    [(LogView*)log_view setAction:@selector(endConsoleInput:)];
    [NSApp runModalForWindow:self];
    [log_view setEditable:NO];

    return [(LogView*)log_view lastInput];
}

- (void)handleAsyncRead:(NSNotification*)notification
{
    NSData * data = [[notification userInfo]
        objectForKey:NSFileHandleNotificationDataItem];

    [self appendText:[NSString stringWithCString:[data bytes] length:[data length]]];

    [[notification object] readInBackgroundAndNotify];
}

- (FILE *)fileForWriting
{
    // cleate a pipe
    log_pipe = [[NSPipe alloc] init];
    NSFileHandle * fh = [log_pipe fileHandleForReading];
    [[NSNotificationCenter defaultCenter]
        addObserver:self
           selector:@selector(handleAsyncRead:)
               name:NSFileHandleReadCompletionNotification
             object:fh];
    [fh readInBackgroundAndNotify];

    // redirect default log file to console
    int oldFd = [[log_pipe fileHandleForWriting] fileDescriptor];
    int newFd = dup(oldFd);
    FILE *file = fdopen(newFd,"w");
    setlinebuf(file);
    return file;
}

@end
