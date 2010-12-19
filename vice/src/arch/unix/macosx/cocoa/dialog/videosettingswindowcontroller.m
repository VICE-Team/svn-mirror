/*
 * videosettingswindowcontroller.m - VideoSettings dialog controller
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

#import "videosettingswindowcontroller.h"
#import "viceapplication.h"
#import "vicenotifications.h"
#import "viceappcontroller.h"

static const char *vicii_palettes[] = {
    "default",
    "c64hq",
    "c64s",
    "ccs64",
    "frodo",
    "godot",
    "pc64",
    "vice",
    NULL
};

static const char *vic_palettes[] = {
    "default",
    NULL
};

static const char *crtc_palettes[] = {
    "amber",
    "green",
    "white",
    NULL
};

static const char *vdc_palettes[] = {
    "vdc_deft",
    "vdc_comp",
    NULL
};

static const char *ted_palettes[] = {
    "default",
    "vice",
    NULL
};

@implementation VideoSettingsWindowController

-(id)init
{
    self = [super initWithWindowNibName:@"VideoSettings"];
    return self;
}

-(void)initPalettes
{
    palette1Entries = NULL;
    palette2Entries = NULL;

    chip1Title = nil;
    chip1File  = nil;
    chip1Ext   = nil;

    chip2Title = nil;
    chip2File  = nil;
    chip2Ext   = nil;
    
    hasColorTab = YES;
    hasCRTEmuTab = YES;
    
    // determine machine setup
    switch(machine_class) {
        case VICE_MACHINE_C128:
            palette2Entries = vdc_palettes;
            chip2Title = @"VDC Palette";
            chip2File  = @"VDCPaletteFile";
            chip2Ext   = @"VDCExternalPalette";
            // fall through!
        case VICE_MACHINE_C64:
        case VICE_MACHINE_C64DTV:
            palette1Entries = vicii_palettes;
            chip1Title = @"VICII Palette";
            chip1File  = @"VICIIPaletteFile";    
            chip1Ext   = @"VICIIExternalPalette";
            break;
        case VICE_MACHINE_VIC20:
            palette1Entries = vic_palettes;
            chip1Title = @"VIC Palette";
            chip1File  = @"VICPaletteFile";    
            chip1Ext   = @"VICExternalPalette";
            break;
        case VICE_MACHINE_PET:
        case VICE_MACHINE_CBM5x0:
        case VICE_MACHINE_CBM6x0:
            palette1Entries = crtc_palettes;
            chip1Title = @"CRTC Palette";
            chip1File  = @"CRTCPaletteFile";    
            chip1Ext   = @"CRTCExternalPalette";
            break;
        case VICE_MACHINE_PLUS4:
            palette1Entries = ted_palettes;
            chip1Title = @"TED Palette";
            chip1File  = @"TEDPaletteFile";    
            chip1Ext   = @"TEDExternalPalette";
            break;
    }
    
    //NSLog(@"chip1: title=%@ file=%@ ext=%@", chip1Title, chip1File, chip1Ext);
    //NSLog(@"chip2: title=%@ file=%@ ext=%@", chip2Title, chip2File, chip2Ext);
    
    if(!hasColorTab) {
        [[colorTab tabView] removeTabViewItem:colorTab];
    }
    if(!hasCRTEmuTab) {
        [[palEmuTab tabView] removeTabViewItem:palEmuTab];
    }
    
    // setup palette1
    if(chip1Title != nil) {
        [palette1Label setStringValue:chip1Title];
        [palette1Popup removeAllItems];
        const char **pal = palette1Entries;
        while (*pal != NULL) {
            [palette1Popup addItemWithTitle:[NSString stringWithCString:*pal encoding:NSUTF8StringEncoding]];
            pal++;
        }
        [palette1Toggle setTitle:chip1Title];
        if(chip1Ext == nil) {
            [palette1Toggle removeFromSuperview];
            palette1Toggle = nil;
        }
    } else {
        [palette1Toggle removeFromSuperview];
        [palette1Label removeFromSuperview];
        [palette1Popup removeFromSuperview];
        [palette1Pick removeFromSuperview];
    }
    
    // setup palette2
    if(chip2Title != nil) {
        [palette2Label setStringValue:chip2Title];
        [palette2Popup removeAllItems];
        const char **pal = palette2Entries;
        while (*pal != NULL) {
            [palette2Popup addItemWithTitle:[NSString stringWithCString:*pal encoding:NSUTF8StringEncoding]];
            pal++;
        }
        [palette2Toggle setTitle:chip2Title];
        if(chip2Ext == nil) {
            [palette2Toggle removeFromSuperview];
            palette2Toggle = nil;
        }
    } else {
        [palette2Toggle removeFromSuperview];
        [palette2Label removeFromSuperview];
        [palette2Popup removeFromSuperview];
        [palette2Pick removeFromSuperview];        
    }
    
}

-(void)windowDidLoad
{
    [self initPalettes];
    [self updateResources:nil];
    [self registerForResourceUpdate:@selector(updateResources:)];
    [super windowDidLoad];
}

// ----- Resources -----

-(void)updatePalette1Resources
{
    if(chip1Ext != nil) {
        BOOL usePal1 = [self getIntResource:chip1Ext];
        [palette1Toggle setState:usePal1];
    }

    NSString *pal1Name = [self getStringResource:chip1File];

    // make sure palette entry is available
    if ([palette1Popup itemWithTitle:pal1Name] == nil) {
        [palette1Popup addItemWithTitle:pal1Name];
    }
    [palette1Popup selectItemWithTitle:pal1Name];
}

-(void)updatePalette2Resources
{
    if(chip2Ext != nil) {
        BOOL usePal2 = [self getIntResource:chip2Ext];
        [palette2Toggle setState:usePal2];
    }
    
    NSString *pal2Name = [self getStringResource:chip2File];

    // make sure palette entry is available
    if ([palette2Popup itemWithTitle:pal2Name] == nil) {
        [palette2Popup addItemWithTitle:pal2Name];
    }
    [palette2Popup selectItemWithTitle:pal2Name];
}

-(void)updateColorResources
{
    int saturation = [self getIntResource:@"ColorSaturation"];
    int contrast = [self getIntResource:@"ColorContrast"];
    int brightness = [self getIntResource:@"ColorBrightness"];
    int gamma = [self getIntResource:@"ColorGamma"];

    [saturationSlider setFloatValue:saturation];
    [saturationText   setFloatValue:saturation];
    [contrastSlider   setFloatValue:contrast];
    [contrastText     setFloatValue:contrast];
    [brightnessSlider setFloatValue:brightness];
    [brightnessText   setFloatValue:brightness];
    [gammaSlider      setFloatValue:gamma];
    [gammaText        setFloatValue:gamma];
}

-(void)updatePALResources
{
    int blur = [self getIntResource:@"PALBlur"];
    int scanlineShade = [self getIntResource:@"PALScanLineShade"];
    int oddLinePhase = [self getIntResource:@"PALOddLinePhase"];
    int oddLineOffset = [self getIntResource:@"PALOddLineOffset"];

    [blurSlider            setFloatValue:blur];
    [blurText              setFloatValue:blur];
    [scanlineShadeSlider   setFloatValue:scanlineShade];
    [scanlineShadeText     setFloatValue:scanlineShade];
    [oddLinePhaseSlider    setFloatValue:oddLinePhase];
    [oddLinePhaseText      setFloatValue:oddLinePhase];
    [oddLineOffsetSlider   setFloatValue:oddLineOffset];
    [oddLineOffsetText     setFloatValue:oddLineOffset];
}

-(void)updateResources:(NSNotification *)notification
{
    if(chip1Title != nil) {
        [self updatePalette1Resources];
    }
    if(chip2Title != nil) {
        [self updatePalette2Resources];
    }
     
    if(hasColorTab) {
        [self updateColorResources];
    }
    if(hasCRTEmuTab) {
        [self updatePALResources];
    }
}

// ----- Actions -----

// --- Color ---

-(IBAction)slideSaturation:(id)sender
{
    [self setIntResource:@"ColorSaturation" toValue:[sender intValue]];
    [self updateColorResources];
}

-(IBAction)slideContrast:(id)sender
{
    [self setIntResource:@"ColorContrast" toValue:[sender intValue]];
    [self updateColorResources];
}

-(IBAction)slideBrightness:(id)sender
{
    [self setIntResource:@"ColorBrightness" toValue:[sender intValue]];
    [self updateColorResources];
}

-(IBAction)slideGamma:(id)sender
{    
    [self setIntResource:@"ColorGamma" toValue:[sender intValue]];
    [self updateColorResources];
}

-(IBAction)enterSaturation:(id)sender
{
    [self setIntResource:@"ColorSaturation" toValue:[sender intValue]];
    [self updateColorResources];
}

-(IBAction)enterContrast:(id)sender
{
    [self setIntResource:@"ColorContrast" toValue:[sender intValue]];
    [self updateColorResources];
}

-(IBAction)enterBrightness:(id)sender
{
    [self setIntResource:@"ColorBrightness" toValue:[sender intValue]];
    [self updateColorResources];
}

-(IBAction)enterGamma:(id)sender
{
    [self setIntResource:@"ColorGamma" toValue:[sender intValue]];
    [self updateColorResources];
}

// --- PAL Emu ---

-(IBAction)slideBlur:(id)sender
{
    [self setIntResource:@"PALBlur" toValue:[sender intValue]];
    [self updatePALResources];
}

-(IBAction)slideScanlineShade:(id)sender
{
    [self setIntResource:@"PALScanLineShade" toValue:[sender intValue]];
    [self updatePALResources];
}

-(IBAction)slideOddLinePhase:(id)sender
{
    [self setIntResource:@"PALOddLinePhase" toValue:[sender intValue]];
    [self updatePALResources];
}

-(IBAction)slideOddLineOffset:(id)sender
{
    [self setIntResource:@"PALOddLineOffset" toValue:[sender intValue]];
    [self updatePALResources];
}

-(IBAction)enterBlur:(id)sender
{
    [self setIntResource:@"PALBlur" toValue:[sender intValue]];
    [self updatePALResources];
}

-(IBAction)enterScanlineShade:(id)sender
{
    [self setIntResource:@"PALScanLineShade" toValue:[sender intValue]];
    [self updatePALResources];
}

-(IBAction)enterOddLinePhase:(id)sender
{
    [self setIntResource:@"PALOddLinePhase" toValue:[sender intValue]];
    [self updatePALResources];
}

-(IBAction)enterOddLineOffset:(id)sender
{
    [self setIntResource:@"PALOddLineOffset" toValue:[sender intValue]];
    [self updatePALResources];
}

// ----- Palette -----

-(IBAction)togglePalette1:(id)sender
{
    if(chip1Ext != nil) {
        BOOL on = [sender state];
        [self setIntResource:chip1Ext toValue:on];
        [self updatePalette1Resources];
    } else {
        [sender setEnabled:YES];
    }
}

-(IBAction)popupPalette1:(id)sender
{
    NSString *item = [sender titleOfSelectedItem];
    [self setStringResource:chip1File toValue:item];
    [self updatePalette1Resources];
}

-(IBAction)pickPalette1:(id)sender
{
    VICEAppController *appCtrl = [VICEApplication theAppController];
    NSString *path = [[appCtrl getFilePanel] pickOpenFileWithType:@"Palette"];
    if (path != nil) {
        [self setStringResource:chip1File toValue:path];
        [self updatePalette1Resources];
    }
}

-(IBAction)togglePalette2:(id)sender
{
    if(chip2Ext != nil) {
        BOOL on = [sender state];
        [self setIntResource:chip2Ext toValue:on];
        [self updatePalette2Resources];
    } else {
        [sender setEnabled:YES];
    }
}

-(IBAction)popupPalette2:(id)sender
{
    NSString *item = [sender titleOfSelectedItem];
    [self setStringResource:chip2File toValue:item];
    [self updatePalette2Resources];
}

-(IBAction)pickPalette2:(id)sender
{
    VICEAppController *appCtrl = [VICEApplication theAppController];
    NSString *path = [[appCtrl getFilePanel] pickOpenFileWithType:@"Palette"];
    if (path != nil) {
        [self setStringResource:chip2File toValue:path];
        [self updatePalette2Resources];
    }
}

@end
