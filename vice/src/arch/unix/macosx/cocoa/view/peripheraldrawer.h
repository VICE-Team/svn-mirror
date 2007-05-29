/*
 * peripheraldrawer.h - PeripheralDrawer
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

#include "drive.h"

#import "driveview.h"
#import "tapeview.h"

@interface PeripheralDrawer : NSDrawer
{
    /* tape control */
    TapeView * tape_view;

    /* red/green */
    NSColor * led_color[2];

    /* set in enableDriveStatus */
/*    int * drive_led_color;*/

    /* drive controls */
    DriveView * drive_view[DRIVE_NUM];

/*    ui_drive_enable_t enabled_drives;*/
/*    int drive_count;*/
}

- (id)initWithPreferredEdge:(NSRectEdge)edge;

@end

