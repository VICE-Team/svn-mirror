/**
 * \file macOS-util.m
 * \brief A collection of little macOS helpers.
 *
 * Calling Objective-C code from C is possible, but not very readable.
 * Nicer to just plop a few readable functions in here.
 *
 * \author David Hogan <david.q.hogan@gmail.com>
 */

/* This file is part of VICE, the Versatile Commodore Emulator.
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

#import "vice.h"

#import "macOS-util.h"

#import <Cocoa/Cocoa.h>
#import <mach/mach.h>
#import <mach/mach_time.h>
#import <pthread.h>
#import <os/log.h>
#import <os/signpost.h>

#import "log.h"
#import "resources.h"

#define NANOS_PER_MICRO 1000ULL
#define MICROS_PER_SEC  1000000ULL

#ifdef USE_NATIVE_GTK3
/* For some reason this isn't in the GDK quartz headers */
id gdk_quartz_window_get_nswindow (GdkWindow *window);
#endif

void vice_macos_set_main_thread(void)
{
    /*
     * Cocoa doesn't behave if it doesn't know that it's multithreaded.
     * Starting a new thread via the NSThread interface is enough to flip
     * the bit.
     */
    
    [NSThread detachNewThreadSelector: @selector(class)
                             toTarget: [NSObject class]
                           withObject: nil];

    /*
     * Disable app-nap
     */
    [[NSProcessInfo processInfo] beginActivityWithOptions: NSActivityUserInitiated | NSActivityLatencyCritical
                                                   reason: @"Accurate emulation requires low latency access to resources."];

    /* The main thread benefits from interactive response levels */
    [[NSThread currentThread] setQualityOfService: NSQualityOfServiceUserInteractive];
    [[NSThread currentThread] setThreadPriority: 1.0];
}

void vice_macos_set_vice_thread(void)
{
    [[NSThread currentThread] setQualityOfService: NSQualityOfServiceUserInteractive];
    [[NSThread currentThread] setThreadPriority: 1.0];
}

#ifdef USE_NATIVE_GTK3
void vice_macos_get_widget_frame_and_content_rect(GtkWidget *widget, CGRect *native_frame, CGRect *content_rect)
{
    id native_window  = gdk_quartz_window_get_nswindow(gtk_widget_get_window(widget));
    id content_view   = [native_window contentView];

    *native_frame = [native_window frame];
    *content_rect = [content_view frame];
}
#endif
