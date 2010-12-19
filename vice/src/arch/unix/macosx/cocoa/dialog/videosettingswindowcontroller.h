/*
 * videosettingswindowcontroller.h - VideoSettings dialog controller
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

@interface VideoSettingsWindowController : VICEResourceWindowController
{
    IBOutlet NSTabViewItem *colorTab;
    IBOutlet NSTabViewItem *palEmuTab;
    IBOutlet NSTabViewItem *paletteTab;
    
    IBOutlet NSSlider *saturationSlider;
    IBOutlet NSSlider *contrastSlider;
    IBOutlet NSSlider *brightnessSlider;
    IBOutlet NSSlider *gammaSlider;

    IBOutlet NSTextField *saturationText;
    IBOutlet NSTextField *contrastText;
    IBOutlet NSTextField *brightnessText;
    IBOutlet NSTextField *gammaText;

    IBOutlet NSSlider *blurSlider;
    IBOutlet NSSlider *scanlineShadeSlider;
    IBOutlet NSSlider *oddLinePhaseSlider;
    IBOutlet NSSlider *oddLineOffsetSlider;

    IBOutlet NSTextField *blurText;
    IBOutlet NSTextField *scanlineShadeText;
    IBOutlet NSTextField *oddLinePhaseText;
    IBOutlet NSTextField *oddLineOffsetText;

    IBOutlet NSButton      *palette1Toggle;
    IBOutlet NSTextField   *palette1Label;
    IBOutlet NSPopUpButton *palette1Popup;
    IBOutlet NSButton      *palette1Pick;

    IBOutlet NSButton      *palette2Toggle;
    IBOutlet NSTextField   *palette2Label;
    IBOutlet NSPopUpButton *palette2Popup;
    IBOutlet NSButton      *palette2Pick;
    
    BOOL hasColorTab;
    BOOL hasCRTEmuTab;
    
    const char **palette1Entries;
    const char **palette2Entries;
    NSString *chip1Title;
    NSString *chip1File;
    NSString *chip1Ext;
    NSString *chip2Title;
    NSString *chip2File;
    NSString *chip2Ext;
}

-(void)updateResources:(NSNotification *)notification;

// Actions
-(IBAction)slideSaturation:(id)sender;
-(IBAction)slideContrast:(id)sender;
-(IBAction)slideBrightness:(id)sender;
-(IBAction)slideGamma:(id)sender;

-(IBAction)enterSaturation:(id)sender;
-(IBAction)enterContrast:(id)sender;
-(IBAction)enterBrightness:(id)sender;
-(IBAction)enterGamma:(id)sender;

-(IBAction)slideBlur:(id)sender;
-(IBAction)slideScanlineShade:(id)sender;
-(IBAction)slideOddLinePhase:(id)sender;
-(IBAction)slideOddLineOffset:(id)sender;

-(IBAction)enterBlur:(id)sender;
-(IBAction)enterScanlineShade:(id)sender;
-(IBAction)enterOddLinePhase:(id)sender;
-(IBAction)enterOddLineOffset:(id)sender;

-(IBAction)togglePalette1:(id)sender;
-(IBAction)popupPalette1:(id)sender;
-(IBAction)pickPalette1:(id)sender;

-(IBAction)togglePalette2:(id)sender;
-(IBAction)popupPalette2:(id)sender;
-(IBAction)pickPalette2:(id)sender;

@end
