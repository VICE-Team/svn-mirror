/*
 * tapeview.h - TapeView
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

@interface TapeView : NSView
{
    /* text field for track display */
    NSTextField * tape_counter;
    NSTextField * tape_control;

    int tape_motor_status;
    int tape_control_status;
}

- (id)initWithFrame:(NSRect)frame;

- (void)setImageFile:(NSString*)image;

- (void)displayImage:(NSNotification*)notification;
- (void)displayCounter:(NSNotification*)notification;
- (void)displayControlStatus:(NSNotification*)notification;
- (void)displayMotorStatus:(NSNotification*)notification;

- (void)updateTapeStatus;

- (void)setEnabled:(BOOL)flag;

@end

