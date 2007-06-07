/*
 * viceresourcewindowcontroller.m - base class for dialog controllers
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

#import "viceresourcewindowcontroller.h"
#import "vicenotifications.h"
#import "viceapplication.h"

@implementation VICEResourceWindowController

-(void)registerForResourceUpdate:(SEL)selector
{
    // register resource updates
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:selector
                                                 name:VICEChangedResourcesNotification
                                               object:nil];
}

-(int)getIntResource:(NSString *)name
{
    NSNumber *number = [[VICEApplication theMachineController] 
                            getIntResource:name];
    if(number==nil)
        return -1;
    int result = [number intValue];
    return result;
}

-(int)getIntResource:(NSString *)format withNumber:(int)number
{
    NSString *formatString = [NSString stringWithFormat:format,number];
    return [self getIntResource:formatString];
}

-(BOOL)setIntResource:(NSString *)name toValue:(int)value
{
    return [[VICEApplication theMachineController] setIntResource:name 
        value:[NSNumber numberWithInt:value]];
}

-(BOOL)setIntResource:(NSString *)format withNumber:(int)number toValue:(int)value
{
    NSString *formatString = [NSString stringWithFormat:format,number];
    return [self setIntResource:formatString toValue:value];
}

@end
