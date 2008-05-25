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
#include "printer.h"
#include "archdep.h"
#include "log.h"
#include "screenshot.h"
#include "gfxoutput.h"
#include "videoarch.h"
#include "interrupt.h"
#include "tape.h"
#include "mouse.h"
#include "vdrive-internal.h"
#include "gfxoutputdrv/ffmpegdrv.h"

#import "vicemachinecontroller.h"
#import "vicemachine.h"

@implementation VICEMachineController

-(id)init
{
    return self;
}

-(NSString *)checkAndGetPrefDir
{
    NSFileManager *fileManager = [NSFileManager defaultManager];
    NSString *prefPath = [NSString stringWithCString:archdep_pref_path encoding:NSUTF8StringEncoding];
    if(![fileManager fileExistsAtPath:prefPath]) {
        if(![fileManager createDirectoryAtPath:prefPath attributes:nil]) {
            [[theVICEMachine app] runErrorMessage:@"Error creating Preferences Dir"];
            return nil;
        }
    }
    BOOL isDir;
    if([fileManager fileExistsAtPath:prefPath isDirectory:&isDir] && isDir) {
        return prefPath;
    }
    [[theVICEMachine app] runErrorMessage:@"Invalid Preferences Dir"];
    return nil;
}

// ----- Resources -----

-(NSNumber *)getIntResource:(NSString *)name
{
    int value;
    if(resources_get_int([name cStringUsingEncoding:NSUTF8StringEncoding],&value)==0) {
        return [NSNumber numberWithInt:value];
    }
    return nil;
}

-(NSString *)getStringResource:(NSString *)name
{
    const char *value;
    if(resources_get_string([name cStringUsingEncoding:NSUTF8StringEncoding],&value)==0) {
        return [NSString stringWithCString:value encoding:NSUTF8StringEncoding];
    }
    return nil;
}

-(BOOL)setIntResource:(NSString *)name value:(NSNumber *)value
{
    return resources_set_int([name cStringUsingEncoding:NSUTF8StringEncoding],
                             [value intValue]) == 0;
}

-(BOOL)setStringResource:(NSString *)name value:(NSString *)value
{
    return resources_set_string([name cStringUsingEncoding:NSUTF8StringEncoding],
                                [value cStringUsingEncoding:NSUTF8StringEncoding]) == 0;
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

// ----- Snapshot -----

static char *snapshotName;
static int snapshotSaveRoms;
static int snapshotSaveDisks;

static void loadSnapshotTrap(WORD unusedWord, void *unusedData)
{
    if (machine_read_snapshot(snapshotName, 0) < 0) {
        [[theVICEMachine app] runErrorMessage:@"Error loading Snapshot!"];
    } else {
        log_message(LOG_DEFAULT,"loaded snapshot '%s'",snapshotName);
    }
    free(snapshotName);
}

static void saveSnapshotTrap(WORD unusedWord, void *unusedData)
{
    if(machine_write_snapshot(snapshotName, snapshotSaveRoms, snapshotSaveDisks, 0) < 0) {
        [[theVICEMachine app] runErrorMessage:@"Error saving Snapshot!"];        
    } else {
        log_message(LOG_DEFAULT,"saved snapshot '%s'",snapshotName);
    }
    free(snapshotName);
}

-(void)loadSnapshot:(NSString *)path
{
    snapshotName = strdup([path fileSystemRepresentation]);
    if([theVICEMachine isPaused])
        loadSnapshotTrap(0,NULL);
    else
        interrupt_maincpu_trigger_trap(loadSnapshotTrap, NULL);
}

-(void)saveSnapshot:(NSString *)path withROMS:(BOOL)saveRoms andDisks:(BOOL)saveDisks
{
    snapshotName = strdup([path fileSystemRepresentation]);
    snapshotSaveRoms = saveRoms;
    snapshotSaveDisks = saveDisks;
    if([theVICEMachine isPaused])
        saveSnapshotTrap(0,NULL);
    else
        interrupt_maincpu_trigger_trap(saveSnapshotTrap, NULL);
}

- (NSString *)getQuickSnapshotFileName:(BOOL)load
{
    NSFileManager *fileManager = [NSFileManager defaultManager];
    NSString *prefPath = [self checkAndGetPrefDir];
    if(prefPath==nil)
        return nil;

    // find file with highest index
    int i;
    int highestIndex = -1;
    NSString *path = nil;
    NSString *lastPath = nil;
    for(i=0;i<10;i++) {
        lastPath = path;
        path = [NSString stringWithFormat:@"%@/quicksnap%d.vsf",prefPath,i];
        if(![fileManager fileExistsAtPath:path]) {
            break;
        }
        highestIndex = i;
    }
    
    if(load) {
        if(lastPath==nil)
            [[theVICEMachine app] runErrorMessage:@"No Quick Snapshot File found!"];
        
        if(highestIndex==9)
            return path;
        else
            return lastPath;
    }
    else {
        // move all files one up to make space for new save
        if(highestIndex==9) {
            NSString *newPath = [NSString stringWithFormat:@"%@/quicksnap0.vsf",prefPath];
            [fileManager removeFileAtPath:newPath handler:nil];
            for(i=1;i<10;i++) {
                NSString *oldPath = [NSString stringWithFormat:@"%@/quicksnap%d.vsf",prefPath,i];
                [fileManager movePath:oldPath toPath:newPath handler:nil];
                newPath = oldPath;
            }
        }

        return path;
    }
}

-(void)saveQuickSnapshot
{
    NSString *path = [self getQuickSnapshotFileName:FALSE];
    if(path!=nil) {
        [self saveSnapshot:path withROMS:FALSE andDisks:FALSE];
    }
}

-(void)loadQuickSnapshot
{
    NSString *path = [self getQuickSnapshotFileName:TRUE];
    if(path!=nil) {
        [self loadSnapshot:path];
    }
}

// ----- Media -----

-(BOOL)startRecordingMedia:(NSString *)driver fromCanvas:(int)canvasId
                                                  toFile:(NSString *)path
{
    // fetch real canvas for id
    video_canvas_t *canvas = [theVICEMachine getCanvasForId:canvasId];
    if(canvas==NULL)
        return false;
    
    int result = screenshot_save([driver cStringUsingEncoding:NSUTF8StringEncoding],
                                 [path cStringUsingEncoding:NSUTF8StringEncoding],
                                 canvas);
    return result == 0;
}

-(BOOL)isRecordingMedia
{
    return screenshot_is_recording();
}

-(void)stopRecordingMedia
{
    screenshot_stop_recording();
}

-(NSArray *)enumMediaDrivers
{
    int i;
    gfxoutputdrv_t *driver;
    
    NSMutableArray *a = [[NSMutableArray alloc] init];
    driver = gfxoutput_drivers_iter_init();
    for (i = 0; i < gfxoutput_num_drivers(); i++) {
        NSString *name = [NSString stringWithCString:driver->name encoding:NSUTF8StringEncoding];
        [a addObject:name];
        driver = gfxoutput_drivers_iter_next();
    }
    return [a autorelease];
}

-(BOOL)mediaDriverHasOptions:(NSString *)driver
{
    if([driver compare:@"FFMPEG"]==NSOrderedSame)
        return TRUE;
    else
        return FALSE;
}

-(NSArray *)enumMediaFormats:(NSString *)driver
{
    // currently only FFMPEG here
    int i;
    NSMutableArray *a = [[NSMutableArray alloc] init];
    for (i=0;ffmpegdrv_formatlist[i].name!=NULL;i++) {
        NSString *fname = [NSString stringWithCString:ffmpegdrv_formatlist[i].name encoding:NSUTF8StringEncoding];
        
        // fetch video codecs
        int j;
        ffmpegdrv_codec_t *video_codecs = ffmpegdrv_formatlist[i].video_codecs;
        NSMutableDictionary *video = [[NSMutableDictionary alloc] init];
        if(video_codecs!=NULL) {
            for(j=0;video_codecs->name!=NULL;j++) {
                NSString *vname = [NSString stringWithCString:video_codecs->name encoding:NSUTF8StringEncoding];
                [video setObject:vname forKey:[NSNumber numberWithInt:video_codecs->id]];
                video_codecs++;
            }
        }
        
        // fetch audio codecs
        ffmpegdrv_codec_t *audio_codecs = ffmpegdrv_formatlist[i].audio_codecs;
        NSMutableDictionary *audio = [[NSMutableDictionary alloc] init];
        if(audio_codecs!=NULL) {
            for(j=0;audio_codecs->name!=NULL;j++) {
                NSString *aname = [NSString stringWithCString:audio_codecs->name encoding:NSUTF8StringEncoding];
                [audio setObject:aname forKey:[NSNumber numberWithInt:audio_codecs->id]];
                audio_codecs++;                
            }
        }

        [a addObject:[NSArray arrayWithObjects:fname,video,audio,nil]];
    }
    return [a autorelease];
}

-(NSString *)defaultExtensionForMediaDriver:(NSString *)driver andFormat:(NSString *)format
{
    if(format==nil) {
        return [driver lowercaseString];
    } else {
        return format;
    }
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

-(BOOL)dumpKeymap:(NSString *)path
{
    return (keyboard_keymap_dump([path fileSystemRepresentation]) == 0);
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
                                                    [name cStringUsingEncoding:NSUTF8StringEncoding],
                                                    ids[type]) == 0;
}

-(NSString *)getDiskName:(int)unit
{
    const char *diskName = file_system_get_disk_name(unit);
    if(diskName==NULL)
        return nil;
    else
        return [NSString stringWithCString:diskName encoding:NSUTF8StringEncoding];
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

// ----- Printer -----

-(void)printerFormFeed:(int)unit
{
    printer_formfeed(unit);
}

@end
