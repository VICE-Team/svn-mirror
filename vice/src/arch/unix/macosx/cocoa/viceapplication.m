/*
 * viceapplication.m - VICEApplication - main application class
 *
 * Written by
 *  Christian Vogelgsang <chris@vogelgsang.org>
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

#include "videoarch.h"
#include "viewport.h"

#import "viceapplication.h"
#import "vicemachine.h"
#import "vicewindow.h"
#import "consolewindow.h"

// from archdep.c:
extern FILE *default_log_file;

@implementation VICEApplication

// initial start up of application
- (void)runWithArgC:(int)argc argV:(char**)argv
{
    // build args array
    argsArray = [[NSMutableArray alloc] init];
    int i;
    for(i=0;i<argc;i++) {
        [argsArray addObject:[NSString stringWithCString:argv[i]]];
    }

    postponeAutostart = YES;

    // enter run loop here
    [super run];
}

// application is ready to run, so fire up machine thread
- (void)applicationDidFinishLaunching:(NSNotification*)aNotification
{
    // reset state
    machine = nil;
    canTerminate = NO;
    canvasCount = 0;
    monitorWindow = nil;
 
    // open default log console
    consoleWindow = [[ConsoleWindow alloc] 
         initWithContentRect:NSMakeRect(600, 360, 500, 200)
                       title:[NSString stringWithCString:_("VICE: Console")]];
    
    // set as new default console
    default_log_file = [consoleWindow fileForWriting];

    // create connection with 2 ports
    NSPort *port1 = [NSPort port];
    NSPort *port2 = [NSPort port];
    NSConnection *con = [[NSConnection alloc] initWithReceivePort:port1
                                                         sendPort:port2];
    [con setRootObject:self];
 
    // build an array for machine thread
    NSArray *args = [NSArray arrayWithObjects:
        port2,port1,argsArray,nil];
    
    // start thread
    [NSThread detachNewThreadSelector:@selector(startConnected:)
                             toTarget:[VICEMachine class] 
                           withObject:args];
}

// initial callback from machine thread after startup
-(void)setMachine:(id)aMachineObject
{
    // get machine object proxy back from connection
    [aMachineObject setProtocolForProxy:@protocol(VICEMachineProtocol)];
    machine = (id<VICEMachineProtocol>)[aMachineObject retain];
    postponeAutostart = NO;
}

- (id<VICEMachineProtocol>)machine
{
    return machine;
}

// access the machine proxy object from the app thread
+ (id<VICEMachineProtocol>)theMachine
{
    VICEApplication *app = (VICEApplication *)[self sharedApplication];
    return [app machine];
}

- (VICEMachineController *)machineController
{
    return [machine getMachineController];
}

+ (VICEMachineController *)theMachineController
{
    VICEApplication *app = (VICEApplication *)[self sharedApplication];
    return [app machineController];
}

// ----- Termination -----

// ask the user if the application should be terminated
- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication*)app
{
    // machine thread is dead so we can actuall die
    if(canTerminate)
        return NSTerminateNow;
    
    // machine thread is not dead now so terminate it...
    VICEMachineController *controller = [machine getMachineController];

    // ask only if user query is enabled
    NSNumber *confirmOnExit = [controller getIntResource:@"ConfirmOnExit"];
    if([confirmOnExit intValue]) {
        int result = NSRunAlertPanel(@"Quit Application",
                                     @"Do you really want to exit?",
                                     @"No",@"Yes",nil);
        if(result!=NSAlertAlternateReturn)
            return NSTerminateCancel;
    }

    // save resources?
    NSNumber *saveResourcesOnExit = [controller getIntResource:@"SaveResourcesOnExit"];
    if([saveResourcesOnExit intValue]) {
        if(![controller saveResources:nil]) {
            NSRunAlertPanel(@"Error",@"Error saving resources!",@"Ok",nil,nil);
        }
    }

    // tell machine thread to shutdown and exit
    [machine stopMachine];

    // always cancel in this stage as the threadWillExit will trigger the
    // final terminate
    return NSTerminateCancel;
}

// shutdown application
- (void)applicationWillTerminate:(NSNotification*)notification
{
    [consoleWindow close];
}

// machine did stop
- (void)machineDidStop
{
    canTerminate = YES;
    [NSApp terminate:nil];
}

// ----- File Drops or File Start -----

// a file was dropped
- (BOOL)application:(NSApplication*)app openFile:(NSString*)file
{
    // if the machine is not ready yet then postpone autostart
    if(postponeAutostart==YES) {
        // add file to args if not already there
        if([argsArray indexOfObject:file]==NSNotFound) {
            [argsArray addObject:file];
        }
        return YES;
    } else {
        return [[machine getMachineController] smartAttachImage:[NSString stringWithString:file]];
    }
}

// ----- Video -----

-(void)createCanvas:(NSData *)canvasPtr withRect:(NSRect)rect
{
    video_canvas_t *canvas = *(video_canvas_t **)[canvasPtr bytes];
    
    canvasCount++;
    NSString *title;
    if(canvasCount==1)
        title = [NSString stringWithCString:canvas->viewport->title];
    else
        title = [NSString stringWithFormat:@"%s #%d",canvas->viewport->title,canvasCount];
    
    // create a new vice window
    VICEWindow *window = [[VICEWindow alloc] 
        initWithRect:rect
               title:title];

    // embedded gl view
    VICEGLView *glView = [window getVICEGLView];

    // fill canvas structure
    canvas->window = window;
    canvas->buffer = [glView getCanvasBuffer];
    canvas->pitch  = [glView getCanvasPitch];
    canvas->depth  = [glView getCanvasDepth];
}

-(void)destroyCanvas:(NSData *)canvasPtr
{
    video_canvas_t *canvas = *(video_canvas_t **)[canvasPtr bytes];
    
    // release vice windows
    if(canvas->window!=nil) {
        [canvas->window close];
    }
}

-(void)resizeCanvas:(NSData *)canvasPtr withSize:(NSSize)size
{
    video_canvas_t *canvas = *(video_canvas_t **)[canvasPtr bytes];

    [canvas->window resizeCanvas:size];

    // fetch results
    VICEGLView *glView = [canvas->window getVICEGLView];
    canvas->buffer = [glView getCanvasBuffer];
    canvas->pitch  = [glView getCanvasPitch];
}

-(void)updateCanvas:(NSData *)canvasPtr
{
    video_canvas_t *canvas = *(video_canvas_t **)[canvasPtr bytes];
    [[canvas->window getVICEGLView] updateTexture];
}

// ----- Monitor -----

-(void)openMonitor
{
    if(monitorWindow==nil) {
        monitorWindow = [[ConsoleWindow alloc] 
                initWithContentRect:NSMakeRect(600, 560, 500, 200)
                              title:[NSString stringWithCString:_("VICE: Monitor")]];
    }
    oldKeyWindow = [self keyWindow];
    [monitorWindow makeKeyAndOrderFront:self];
}

-(void)closeMonitor
{
    [monitorWindow orderOut:self];
    [oldKeyWindow makeKeyAndOrderFront:self];
}

-(void)printMonitorMessage:(NSString *)msg
{
    [monitorWindow appendText:msg];
}

-(NSString *)readMonitorLine:(NSString *)prompt
{
    return [monitorWindow readline:prompt];
}

// ----- Notifications -----

- (void)postRemoteNotification:(NSArray *)array 
{
    NSString *notificationName = (NSString *)[array objectAtIndex:0];
    NSDictionary *userInfo = (NSDictionary *)[array objectAtIndex:1];
    
    // post notification in our UI thread's default notification center
    [[NSNotificationCenter defaultCenter] postNotificationName:notificationName
                                                        object:self
                                                      userInfo:userInfo];
}

// ----- Console Window -----

- (void)toggleConsoleWindow:(id)sender
{
    if([consoleWindow isVisible]) {
        [consoleWindow orderOut:sender];
    } else {
        [consoleWindow makeKeyAndOrderFront:sender];
    }
}

- (void)toggleMonitorWindow:(id)sender
{
    if(monitorWindow==nil)
        return;
    
    if([monitorWindow isVisible]) {
        [monitorWindow orderOut:sender];
    } else {
        [monitorWindow makeKeyAndOrderFront:sender];
    }
}

- (BOOL)validateMenuItem:(NSMenuItem *)menuItem
{
    if([menuItem action]==@selector(toggleConsoleWindow:)) {
        if([consoleWindow isVisible]) {
            [menuItem setState:NSOnState];
        } else {
            [menuItem setState:NSOffState];
        }
    }
    else if([menuItem action]==@selector(toggleMonitorWindow:)) {
        if(monitorWindow==nil)
            return NO;
        if([monitorWindow isVisible]) {
            [menuItem setState:NSOnState];
        } else {
            [menuItem setState:NSOffState];
        }
    }
    return [super validateMenuItem:menuItem];  
}

// ----- Alerts & Dialogs -----

+ (void)runErrorMessage:(NSString *)message
{
    VICEApplication *app = (VICEApplication *)[self sharedApplication];
    [app runErrorMessage:message];
}

+ (void)runWarningMessage:(NSString *)message
{
    VICEApplication *app = (VICEApplication *)[self sharedApplication];
    [app runWarningMessage:message];
}

- (void)runErrorMessage:(NSString *)message
{
    NSAlert *alert = [[NSAlert alloc] init];
    [alert addButtonWithTitle:@"Close"];
    [alert setMessageText:@"VICE Error!"];
    [alert setInformativeText:message];
    [alert runModal];
    [alert release];
}

- (void)runWarningMessage:(NSString *)message
{
    NSAlert *alert = [[NSAlert alloc] init];
    [alert addButtonWithTitle:@"Close"];
    [alert setMessageText:@"VICE Warning!"];
    [alert setInformativeText:message];
    [alert setAlertStyle:NSWarningAlertStyle];
    [alert runModal];
    [alert release];
}

- (int)runCPUJamDialog:(NSString *)message
{
    NSAlert *alert = [[NSAlert alloc] init];
    [alert addButtonWithTitle:@"Reset"];
    [alert addButtonWithTitle:@"Monitor"];
    [alert addButtonWithTitle:@"Hard Reset"];
    [alert setMessageText:@"VICE CPU JAM!"];
    [alert setInformativeText:message];
    int result = [alert runModal];
    [alert release];
    
    if(result==NSAlertFirstButtonReturn)
        return 0;
    else if(result=NSAlertSecondButtonReturn)
        return 1;
    else
        return 2;
}

- (BOOL)runExtendImageDialog
{
    NSAlert *alert = [[NSAlert alloc] init];
    [alert addButtonWithTitle:@"Yes"];
    [alert addButtonWithTitle:@"No"];
    [alert setMessageText:@"VICE Extend Image"];
    [alert setInformativeText:@"Extend image to 40 Tracks?"];
    int result = [alert runModal];
    [alert release];
    return result == NSAlertFirstButtonReturn;
}

@end
