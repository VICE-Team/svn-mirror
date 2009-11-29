/*
 * sidsettingswindowcontroller.m - SIDSettings dialog controller
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

#include "vice.h"
#include "machine.h"
#include "sid.h"

#import "sidsettingswindowcontroller.h"
#import "viceapplication.h"
#import "vicenotifications.h"

static const int c64baseaddress[] = { 0xd4, 0xd5, 0xd6, 0xd7, 0xde, 0xdf, -1 };
static const int c128baseaddress[] = { 0xd4, 0xd7, 0xde, 0xdf, -1 };
static const int cbm2baseaddress[] = { 0xda, -1 };

static char *engine_model_names[] = {
#ifdef HAVE_RESID
    "DTVSID (reSID-DTV)",
#endif
    "6581 (Fast SID)",
    "8580 (Fast SID)",
#ifdef HAVE_RESID
    "6581 (ReSID)",
    "8580 (ReSID)",
    "8580 + digiboost (ReSID)",
#endif
#ifdef HAVE_RESID_FP
    "6581R3 4885 (ReSID-fp)",
    "6581R3 0486S (ReSID-fp)",
    "6581R3 3984 (ReSID-fp)",
    "6581R4AR 3789 (ReSID-fp)",
    "6581R3 4485 (ReSID-fp)",
    "6581R4 1986S (ReSID-fp)",
    "8580R5 3691 (ReSID-fp)",
    "8580R5 3691 + digiboost (ReSID-fp)",
    "8580R5 1489 (ReSID-fp)",
    "8580R5 1489 + digiboost (ReSID-fp)",
#endif
    NULL
};

static const int engine_model_values[] = {
#ifdef HAVE_RESID
    SID_RESID_DTVSID,
#endif
    SID_FASTSID_6581,
    SID_FASTSID_8580,
#ifdef HAVE_RESID
    SID_RESID_6581,
    SID_RESID_8580,
    SID_RESID_8580D,
#endif
#ifdef HAVE_RESID_FP
    SID_RESIDFP_6581R3_4885,
    SID_RESIDFP_6581R3_0486S,
    SID_RESIDFP_6581R3_3984,
    SID_RESIDFP_6581R4AR_3789,
    SID_RESIDFP_6581R3_4485,
    SID_RESIDFP_6581R4_1986S,
    SID_RESIDFP_8580R5_3691,
    SID_RESIDFP_8580R5_3691D,
    SID_RESIDFP_8580R5_1489,
    SID_RESIDFP_8580R5_1489D,
#endif
    -1
};

@implementation SIDSettingsWindowController

-(id)init
{
    self = [super initWithWindowNibName:@"SIDSettings"];
    if (!self) {
        return self;
    }

    // machine config
    stereoAddrs = NULL;
    if(machine_class == VICE_MACHINE_C64DTV) {
        hasFilters = NO;
        hasStereo  = NO;
    } else {
        hasFilters = YES;
        hasStereo  = YES;
        if(machine_class == VICE_MACHINE_C64) {
            stereoAddrs = c64baseaddress;
        } else if(machine_class == VICE_MACHINE_C128) {
            stereoAddrs = c128baseaddress;
        } else if((machine_class == VICE_MACHINE_CBM6x0)||
                  (machine_class == VICE_MACHINE_CBM5x0)) {
            stereoAddrs = cbm2baseaddress;
        }
    }

    [self registerForResourceUpdate:@selector(updateResources:)];

    return self;
}

-(void)enableReSIDControls:(BOOL)on
{
    [residSampleMethodButton setEnabled:on];
    [residPassbandSlider setEnabled:on];
    [residPassbandText setEnabled:on];    
}

-(void)windowDidLoad
{
#ifndef HAVE_RESID
    // disable RESID
    [self enabledReSIDControls:NO];
#endif    
    
    // toggle stereo/filters
    [stereoSidEnableButton setEnabled:hasStereo];
    [stereoSidAddressButton setEnabled:hasStereo];
    [filterEnableButton setEnabled:hasFilters];
    
    // setup stereo addr
    if(stereoAddrs != NULL) {
        int i=0;
        while(stereoAddrs[i]!=-1) {
            int hi = stereoAddrs[i];
            int lo;
            for (lo = (hi > 0 ? 0x0 : 0x20); lo < 0x100; lo += 0x20) {
                int addr = hi * 0x100 + lo;
                NSString *title = [NSString stringWithFormat:@"$%04X",addr];
                [stereoSidAddressButton addItemWithTitle:title];
            }
            i++;
        }
    }
    
    // setup sid model
    modelOffset = 1;
#ifdef HAVE_RESID
    if(machine_class == VICE_MACHINE_C64DTV) {
        modelOffset = 0;
    }
#endif
    int offset = modelOffset;
    while(engine_model_names[offset] != NULL) {
        const char *name = engine_model_names[offset];
        [engineModelButton addItemWithTitle:[NSString stringWithCString:name 
                                            encoding:NSUTF8StringEncoding]];
        offset++;
    }
    
    [self updateResources:nil];
    [super windowDidLoad];
}

-(void)updateResidPassbandResources
{
    int residPassband     = [self getIntResource:@"SidResidPassband"];
    [residPassbandSlider setIntValue:residPassband];
    [residPassbandText setIntValue:residPassband];    
}

-(void)updateResources:(NSNotification *)notification
{
    // set model
    int i = modelOffset;
    int engine = [self getIntResource:@"SidEngine"];
    int model  = [self getIntResource:@"SidModel"];
    int pair   = engine << 8 | model;
    while(engine_model_values[i] != -1) {
        if(engine_model_values[i] == pair) {
            break;
        }
        i++;
    }
    i -= modelOffset;
    [engineModelButton selectItemAtIndex:i];
    
    // is reSID enabled?
    BOOL reSIDenabled = (engine == SID_ENGINE_RESID) || (engine == SID_ENGINE_RESID_FP);
    [self enableReSIDControls:reSIDenabled];
    
    if(hasStereo) {
        int sidStereo = [self getIntResource:@"SidStereo"];
        [stereoSidEnableButton setState:(sidStereo ? NSOnState:NSOffState)];
        int sidStereoBase = [self getIntResource:@"SidStereoAddressStart"];
        NSString *title = [NSString stringWithFormat:@"$%04X",sidStereoBase];
        [stereoSidAddressButton selectItemWithTitle:title];
        [stereoSidAddressButton setEnabled:sidStereo ? NSOnState:NSOffState];
    }
    if(hasFilters) {
        int sidFilter = [self getIntResource:@"SidFilters"];
        [filterEnableButton setState:(sidFilter ? NSOnState : NSOffState)];
    }
    
    // reSID
#ifdef HAVE_RESID
    int residSampleMethod = [self getIntResource:@"SidResidSampling"];
    [residSampleMethodButton selectItemWithTag:residSampleMethod];
    [self updateResidPassbandResources];
#endif
}

// ----- Actions -----

-(IBAction)popupEngineModel:(id)sender
{
    int modelIndex = modelOffset + [engineModelButton indexOfSelectedItem];
    int pair = engine_model_values[modelIndex];
    int engine = pair >> 8;
    int model  = pair & 0xff;
    [self setIntResource:@"SidModel" toValue:model];
    [self setIntResource:@"SidEngine" toValue:engine];
    [self updateResources:nil];
}

-(IBAction)toggleStereoSid:(id)sender
{
    int stereoSid = [sender state] == NSOnState;
    [self setIntResource:@"SidStereo" toValue:stereoSid];
    [self updateResources:nil];
}

-(IBAction)popupStereoSidAddress:(id)sender
{
    NSString *title = [sender title];
    int addr;
    sscanf([title cStringUsingEncoding:NSUTF8StringEncoding],"$%x",&addr);
    [self setIntResource:@"SidStereoAddressStart" toValue:addr];
}

-(IBAction)toggleSidFilter:(id)sender
{
    int sidFilter = ([sender state] == NSOnState);
    [self setIntResource:@"SidFilters" toValue:sidFilter];
}

-(IBAction)popupResidSampleMethod:(id)sender
{
    [self setIntResource:@"SidResidSampling" toValue:[sender tag]];
}

-(IBAction)slideResidPasspand:(id)sender
{
    [self setIntResource:@"SidResidPassband" toValue:[sender intValue]];
    [self updateResidPassbandResources];
}

-(IBAction)enterResidPassband:(id)sender
{
    [self setIntResource:@"SidResidPassband" toValue:[sender intValue]];
    [self updateResidPassbandResources];
}

@end
