/*
 * drivesettingswindowcontroller.m - DriveSettings dialog controller
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

#import "drivesettingswindowcontroller.h"
#import "viceapplication.h"
#import "vicenotifications.h"

#include "drive.h"

@implementation DriveSettingsWindowController

-(id)init
{
    self = [super initWithWindowNibName:@"DriveSettings"];
    [self registerForResourceUpdate:@selector(updateResources:)];
    
    int map[14] = {
        DRIVE_TYPE_1541, DRIVE_TYPE_1541II, DRIVE_TYPE_1551,
        DRIVE_TYPE_1570, DRIVE_TYPE_1571,   DRIVE_TYPE_1571CR,
        DRIVE_TYPE_1581, DRIVE_TYPE_2031,   DRIVE_TYPE_2040,
        DRIVE_TYPE_3040, DRIVE_TYPE_4040,   DRIVE_TYPE_1001,
        DRIVE_TYPE_8050, DRIVE_TYPE_8250
    };
    int i;
    numDriveTypes = 14;
    for(i=0;i<numDriveTypes;i++)
        driveTypeMap[i] = map[i];
    
    // TODO: set emulator dependent
    driveOffset = 8;
    driveCount = 4;

    return self;
}

-(void)windowDidLoad
{
    // setup tab view labels
    int i;
    for(i=0;i<driveCount;i++) {
        NSTabViewItem *item = [tabView tabViewItemAtIndex:i];
        NSString *driveName = [NSString stringWithFormat:@"Drive %d",i+driveOffset];
        [item setLabel:driveName];
    }
    
    // remove unused views
    while(i<4) {
        NSTabViewItem *item = [tabView tabViewItemAtIndex:driveCount];
        [tabView removeTabViewItem:item];
        i++;
    }
    
    // select first tab
    [tabView selectFirstTabViewItem:self];
    
    [self updateResources:nil];
    [super windowDidLoad];
}

-(void)updateResources:(NSNotification *)notification
{
    NSMatrix *driveType[4] = { driveType0,driveType1,
                               driveType2,driveType3 };
    NSMatrix *trackHandling[4] = { trackHandling0,trackHandling1,
                                   trackHandling2,trackHandling3 };
    NSMatrix *idleMethod[4] = { idleMethod0, idleMethod1,
                                idleMethod2, idleMethod3 };
    NSButton *parallelCable[4] = { parallelCable0,parallelCable1,
                                   parallelCable1,parallelCable3 };
    NSButton *driveExpansion2000[4] = { driveExpansion0_2000,driveExpansion1_2000,
                                       driveExpansion2_2000,driveExpansion3_2000 };
    NSButton *driveExpansion4000[4] = { driveExpansion0_4000,driveExpansion1_4000,
                                       driveExpansion2_4000,driveExpansion3_4000 };
    NSButton *driveExpansion6000[4] = { driveExpansion0_6000,driveExpansion1_6000,
                                       driveExpansion2_6000,driveExpansion3_6000 };
    NSButton *driveExpansion8000[4] = { driveExpansion0_8000,driveExpansion1_8000,
                                       driveExpansion2_8000,driveExpansion3_8000 };
    NSButton *driveExpansionA000[4] = { driveExpansion0_A000,driveExpansion1_A000,
                                       driveExpansion2_A000,driveExpansion3_A000 };
        
    int trueEmu = [self getIntResource:@"DriveTrueEmulation"];
    int i;
    for(i=0;i<driveCount;i++) {
        int driveNum = driveOffset + i;
        int isIecDrive = [self getIntResource:@"IECDevice%d" withNumber:driveNum];
        int driveEnabled = trueEmu && !isIecDrive;
        
        // drive is enabled
        if(driveEnabled) {
            // type selector is enabled
            [driveType[i] setEnabled:true];

            // enable valid drive types
            int j;
            for(j=0;j<numDriveTypes;j++) {
                int isDriveValid = drive_check_type([self mapToDriveType:j],i);
                id cell = [driveType[i] cellAtRow:j column:0];
                [cell setEnabled:isDriveValid];
            }
        
            // set current drive type
            int driveTypeVal = [self getIntResource:@"Drive%dType" withNumber:driveNum];
            int driveId = [self mapFromDriveType:driveTypeVal];
            [driveType[i] selectCellAtRow:driveId column:0];

            // extend track policy
            int canExtendPolicy = drive_check_extend_policy(driveTypeVal);
            [trackHandling[i] setEnabled:canExtendPolicy];
            int extendPolicyVal = [self getIntResource:@"Drive%dExtendImagePolicy" withNumber:driveNum];
            [trackHandling[i] selectCellAtRow:extendPolicyVal column:0];

            // idle method
            int canIdleMethod = drive_check_idle_method(driveTypeVal);
            [idleMethod[i] setEnabled:canIdleMethod];
            int idleMethodVal = [self getIntResource:@"Drive%dIdleMethod" withNumber:driveNum];
            [idleMethod[i] selectCellAtRow:idleMethodVal column:0];

            // expansion ram
            int canRam,hasRam;
            canRam = drive_check_expansion2000(driveTypeVal);
            [driveExpansion2000[i] setEnabled:canRam];
            hasRam = [self getIntResource:@"Drive%dRAM2000" withNumber:driveNum];
            [driveExpansion2000[i] setState:hasRam];

            canRam = drive_check_expansion4000(driveTypeVal);
            [driveExpansion4000[i] setEnabled:canRam];
            hasRam = [self getIntResource:@"Drive%dRAM4000" withNumber:driveNum];
            [driveExpansion4000[i] setState:hasRam];

            canRam = drive_check_expansion6000(driveTypeVal);
            [driveExpansion6000[i] setEnabled:canRam];
            hasRam = [self getIntResource:@"Drive%dRAM6000" withNumber:driveNum];
            [driveExpansion6000[i] setState:hasRam];

            canRam = drive_check_expansion8000(driveTypeVal);
            [driveExpansion8000[i] setEnabled:canRam];
            hasRam = [self getIntResource:@"Drive%dRAM8000" withNumber:driveNum];
            [driveExpansion8000[i] setState:hasRam];

            canRam = drive_check_expansionA000(driveTypeVal);
            [driveExpansionA000[i] setEnabled:canRam];
            hasRam = [self getIntResource:@"Drive%dRAMA000" withNumber:driveNum];
            [driveExpansionA000[i] setState:hasRam];

            // select current driv
            int canParallel = drive_check_parallel_cable(driveTypeVal);
            [parallelCable[i] setEnabled:canParallel];
            int parallelCableVal = [self getIntResource:@"Drive%dParallelCable" withNumber:driveNum];
            [parallelCable[i] setState:parallelCableVal];
            
        } else {
            // disable all controls
            [driveType[i] setEnabled:false];
            [trackHandling[i] setEnabled:false];
            [driveExpansion2000[i] setEnabled:false];
            [driveExpansion4000[i] setEnabled:false];
            [driveExpansion6000[i] setEnabled:false];
            [driveExpansion8000[i] setEnabled:false];
            [driveExpansionA000[i] setEnabled:false];
            [idleMethod[i] setEnabled:false];
            [parallelCable[i] setEnabled:false];
        }
    }
}

-(int)mapToDriveType:(int)driveId
{
    if((driveId>=0)&&(driveId<numDriveTypes))
        return driveTypeMap[driveId];
    return DRIVE_TYPE_NONE;
}

-(int)mapFromDriveType:(int)driveType
{
    int i;
    for(i=0;i<numDriveTypes;i++) {
        if(driveType == driveTypeMap[i])
            return i;
    }
    return numDriveTypes;
}

// ----- Actions -----

-(void)changedDriveType:(id)sender
{
    int driveNum = [sender tag] + driveOffset;
    id cell = [sender selectedCell];
    int driveId = [cell tag];
    int driveType = [self mapToDriveType:driveId];
        
    [self setIntResource:@"Drive%dType" 
              withNumber:driveNum
                 toValue:driveType];
    [self updateResources:nil];
}

-(void)changedTrackHandling:(id)sender
{
    int driveNum = [sender tag] + driveOffset;
    id cell = [sender selectedCell];
    int type = [cell tag];
    
    [self setIntResource:@"Drive%dExtendImagePolicy" 
              withNumber:driveNum 
                 toValue:type];
}

-(void)changedDriveExpansion2000:(id)sender
{
    int driveNum = [sender tag] + driveOffset;
    id cell = [sender selectedCell];
    int on = [cell state];
    
    [self setIntResource:@"Drive%dRAM2000"
              withNumber:driveNum
                 toValue:on];
}

-(void)changedDriveExpansion4000:(id)sender
{
    int driveNum = [sender tag] + driveOffset;
    id cell = [sender selectedCell];
    int on = [cell state];

    [self setIntResource:@"Drive%dRAM4000"
              withNumber:driveNum
                 toValue:on];
}

-(void)changedDriveExpansion6000:(id)sender
{
    int driveNum = [sender tag] + driveOffset;
    id cell = [sender selectedCell];
    int on = [cell state];

    [self setIntResource:@"Drive%dRAM6000"
              withNumber:driveNum
                 toValue:on];
}

-(void)changedDriveExpansion8000:(id)sender
{
    int driveNum = [sender tag] + driveOffset;
    id cell = [sender selectedCell];
    int on = [cell state];
    
    [self setIntResource:@"Drive%dRAM8000"
              withNumber:driveNum
                 toValue:on];
}

-(void)changedDriveExpansionA000:(id)sender
{
    int driveNum = [sender tag] + driveOffset;
    id cell = [sender selectedCell];
    int on = [cell state];

    [self setIntResource:@"Drive%dRAMA000"
              withNumber:driveNum
                 toValue:on];
}

-(void)changedIdleMethod:(id)sender
{
    int driveNum = [sender tag] + driveOffset;
    id cell = [sender selectedCell];
    int type = [cell tag];

    [self setIntResource:@"Drive%dIdleMethod"
              withNumber:driveNum
                 toValue:type];
}

-(void)toggledParallelCable:(id)sender
{
    int driveNum = [sender tag] + driveOffset;
    int on = [sender state];

    [self setIntResource:@"Drive%dParallelCable"
              withNumber:driveNum
                 toValue:on];
}

@end
