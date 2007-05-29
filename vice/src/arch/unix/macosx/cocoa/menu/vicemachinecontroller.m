/*
 * vicemachinecontroller.m - VICEMachineController - base machine controller
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

#include "resources.h"
#include "autostart.h"
#include "attach.h"
#include "vsync.h"
#include "drivecpu.h"
#include "monitor.h"
#include "machine.h"
#include "keyboard.h"
#include "diskimage.h"
#include "mousedrv.h"

#import "vicemachinecontroller.h"
#import "vicemachine.h"

@implementation VICEMachineController

// ----- Resources -----

-(NSNumber *)getIntResource:(NSString *)name
{
    int value;
    if(resources_get_int([name cString],&value)==0) {
        return [NSNumber numberWithInt:value];
    }
    return nil;
}

-(NSString *)getStringResource:(NSString *)name
{
    const char *value;
    if(resources_get_string([name cString],&value)==0) {
        return [NSString stringWithCString:value];
    }
    return nil;
}

-(BOOL)setIntResource:(NSString *)name value:(NSNumber *)value
{
    return resources_set_int([name cString],[value intValue]) == 0;
}

-(BOOL)setStringResource:(NSString *)name value:(NSString *)value
{
    return resources_set_string([name cString],[value cString]) == 0;
}

-(BOOL)loadResources:(NSString *)path
{
    const char *cPath;
    if(path==nil)
        cPath = NULL;
    else
        cPath = [path fileSystemRepresentation];
    
    BOOL result = resources_load(cPath) == 0;
    
    // trigger resource upate in u
    [[theVICEMachine machineNotifier] postChangedResourcesNotification];
    
    return result;
}

-(BOOL)saveResources:(NSString *)path
{
    const char *cPath;
    if(path==nil)
        cPath = NULL;
    else
        cPath = [path fileSystemRepresentation];
    
    return resources_save(cPath) == 0;
}

-(void)resetResources
{
    resources_set_defaults();

    // trigger resource upate
    [[theVICEMachine machineNotifier] postChangedResourcesNotification];
}

// ----- Reset -----

-(void)resetMachine:(BOOL)hardReset
{
    if(hardReset)
        machine_trigger_reset(MACHINE_RESET_MODE_HARD);
    else
        machine_trigger_reset(MACHINE_RESET_MODE_SOFT);
}

-(void)resetDrive:(int)unit
{
    drivecpu_trigger_reset(unit);
}

// ----- Monitor -----

-(void)activateMonitor
{
    if([theVICEMachine isPaused])
        monitor_startup();
    else
        monitor_startup_trap();
}

// ----- Keyboard -----

-(void)keyPressed:(unsigned int)code
{
    // tell VICE a key was pressed
    keyboard_key_pressed(code);
}

-(void)keyReleased:(unsigned int)code
{
    // tell VICE a key was released
    keyboard_key_released(code);
}

// ----- Mouse -----

-(void)mouseMoveToX:(int)x andY:(int)y
{
    mouse_move(x,y);
}

-(void)mousePressed
{
    mouse_button_left(1);
}

-(void)mouseReleased
{
    mouse_button_left(0);
}

// ----- Drive -----

-(BOOL)smartAttachImage:(NSString *)path
{
    return autostart_autodetect([path fileSystemRepresentation],
        NULL,0,AUTOSTART_MODE_RUN) == 0;
}

-(BOOL)attachDiskImage:(int)unit path:(NSString *)path
{
    return file_system_attach_disk(unit,[path fileSystemRepresentation]) == 0;
}

-(void)detachDiskImage:(int)unit
{
    file_system_detach_disk(unit);
}

-(BOOL)createDiskImage:(int)type path:(NSString *)path name:(NSString *)name
{
    const int ids[] = {
        DISK_IMAGE_TYPE_D64, DISK_IMAGE_TYPE_D67, DISK_IMAGE_TYPE_D71,
        DISK_IMAGE_TYPE_D80, DISK_IMAGE_TYPE_D81, DISK_IMAGE_TYPE_D82,
        DISK_IMAGE_TYPE_G64, DISK_IMAGE_TYPE_X64
    };
    return vdrive_internal_create_format_disk_image([path fileSystemRepresentation],
                                                    [name cString],
                                                    ids[type]) == 0;
}

// ----- Tape -----

-(BOOL)attachTapeImage:(NSString *)path
{
    return tape_image_attach(1,[path fileSystemRepresentation]) == 0;
}

-(void)detachTapeImage
{
    tape_image_detach(1);
}

@end
