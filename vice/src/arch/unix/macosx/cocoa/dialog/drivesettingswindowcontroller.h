/*
 * drivesettingswindowcontroller.h - DriveSettings dialog controller
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


#import <Cocoa/Cocoa.h>
#import "viceresourcewindowcontroller.h"

@interface DriveSettingsWindowController : VICEResourceWindowController
{
    IBOutlet NSTabView *tabView;
    
    IBOutlet NSMatrix *driveType0;
    IBOutlet NSMatrix *trackHandling0;
    IBOutlet NSMatrix *idleMethod0;
    IBOutlet NSButton *parallelCable0;
    IBOutlet NSButton *driveExpansion0_2000;
    IBOutlet NSButton *driveExpansion0_4000;
    IBOutlet NSButton *driveExpansion0_6000;
    IBOutlet NSButton *driveExpansion0_8000;
    IBOutlet NSButton *driveExpansion0_A000;

    IBOutlet NSMatrix *driveType1;
    IBOutlet NSMatrix *trackHandling1;
    IBOutlet NSMatrix *driveExpansion1;
    IBOutlet NSMatrix *idleMethod1;
    IBOutlet NSButton *parallelCable1;
    IBOutlet NSButton *driveExpansion1_2000;
    IBOutlet NSButton *driveExpansion1_4000;
    IBOutlet NSButton *driveExpansion1_6000;
    IBOutlet NSButton *driveExpansion1_8000;
    IBOutlet NSButton *driveExpansion1_A000;

    IBOutlet NSMatrix *driveType2;
    IBOutlet NSMatrix *trackHandling2;
    IBOutlet NSMatrix *driveExpansion2;
    IBOutlet NSMatrix *idleMethod2;
    IBOutlet NSButton *parallelCable2;
    IBOutlet NSButton *driveExpansion2_2000;
    IBOutlet NSButton *driveExpansion2_4000;
    IBOutlet NSButton *driveExpansion2_6000;
    IBOutlet NSButton *driveExpansion2_8000;
    IBOutlet NSButton *driveExpansion2_A000;

    IBOutlet NSMatrix *driveType3;
    IBOutlet NSMatrix *trackHandling3;
    IBOutlet NSMatrix *driveExpansion3;
    IBOutlet NSMatrix *idleMethod3;
    IBOutlet NSButton *parallelCable3;
    IBOutlet NSButton *driveExpansion3_2000;
    IBOutlet NSButton *driveExpansion3_4000;
    IBOutlet NSButton *driveExpansion3_6000;
    IBOutlet NSButton *driveExpansion3_8000;
    IBOutlet NSButton *driveExpansion3_A000;
    
    int driveTypeMap[14];
    int numDriveTypes;
    int driveOffset;
    int driveCount;
}

-(void)updateResources:(NSNotification *)notification;

-(IBAction)changedDriveType:(id)sender;
-(IBAction)changedTrackHandling:(id)sender;
-(IBAction)changedDriveExpansion2000:(id)sender;
-(IBAction)changedDriveExpansion4000:(id)sender;
-(IBAction)changedDriveExpansion6000:(id)sender;
-(IBAction)changedDriveExpansion8000:(id)sender;
-(IBAction)changedDriveExpansionA000:(id)sender;
-(IBAction)changedIdleMethod:(id)sender;
-(IBAction)toggledParallelCable:(id)sender;

-(int)mapToDriveType:(int)driveId;
-(int)mapFromDriveType:(int)driveType;

@end
