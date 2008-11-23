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

#import "videosettingswindowcontroller.h"
#import "viceapplication.h"
#import "vicenotifications.h"
#import "viceappcontroller.h"

static const char *vicii_palettes[]=
{
 "default","c64hq","c64s","ccs64","frodo","godot","pc64","vice",NULL
};

@implementation VideoSettingsWindowController

-(id)init
{
    self = [super initWithWindowNibName:@"VideoSettings"];
    [self registerForResourceUpdate:@selector(updateResources:)];
    return self;
}

-(void)initPalettes
{
    [palette1Popup removeAllItems];
    const char **pal = vicii_palettes;
    while(*pal!=NULL) {
        [palette1Popup addItemWithTitle:[NSString stringWithCString:*pal encoding:NSUTF8StringEncoding]];
        pal++;
    }
}

-(void)windowDidLoad
{
    [self initPalettes];
    [self updateResources:nil];
    [super windowDidLoad];
}

// ----- Resources -----

-(void)updatePaletteResources
{
    BOOL usePal1 = [self getIntResource:@"VICIIExternalPalette"];
    [palette1Toggle setState:usePal1];
    NSString *pal1Name = [self getStringResource:@"VICIIPaletteFile"];

    // make sure palette entry is available
    if([palette1Popup itemWithTitle:pal1Name] == nil) {
        [palette1Popup addItemWithTitle:pal1Name];
    }
    [palette1Popup selectItemWithTitle:pal1Name];
}

-(void)updateColorResources
{
    int saturation = [self getIntResource:@"ColorSaturation"];
    int contrast   = [self getIntResource:@"ColorContrast"];
    int brightness = [self getIntResource:@"ColorBrightness"];
    int gamma      = [self getIntResource:@"ColorGamma"];
    
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
    int blur          = [self getIntResource:@"PALBlur"];
    int scanlineShade = [self getIntResource:@"PALScanLineShade"];
    int oddLinePhase  = [self getIntResource:@"PALOddLinePhase"];
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
    [self updatePaletteResources];
    [self updateColorResources];
    [self updatePALResources];
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
    BOOL on = [sender state];
    [self setIntResource:@"VICIIExternalPalette" toValue:on];
    [self updatePaletteResources];
}

-(IBAction)popupPalette1:(id)sender
{
    NSString *item = [sender titleOfSelectedItem];
    [self setStringResource:@"VICIIPaletteFile" toValue:item];
    [self updatePaletteResources];
}

-(IBAction)pickPalette1:(id)sender
{
    NSArray *types = [NSArray arrayWithObjects:@"vpl", nil];
    VICEAppController *appCtrl = [VICEApplication theAppController];
    NSString *path = [appCtrl pickOpenFileWithTitle:@"Load Palette" types:types];
    if(path!=nil) {
        [self setStringResource:@"VICIIPaletteFile" toValue:path];
        [self updatePaletteResources];
    }
}

@end
