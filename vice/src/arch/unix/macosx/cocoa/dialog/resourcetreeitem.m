/*
 * resourcetreeitem.m - item of the resource tree
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

#import "resourcetreeitem.h"
#import "resourceeditorcontroller.h"

@implementation ResourceTreeItem

-(id)initWithTitle:(NSString *)t
{
    self = [super init];
    if(self == nil)
        return nil;
    
    title = [t retain];
    resource = nil;
    children = nil;
    cacheValue = nil;
    dataCell = nil;
    extraCell = nil;
    
    return self;
}

-(void)dealloc
{
    [title release];
    [resource release];
    [children release];
    [cacheValue release];
    [dataCell release];
    [extraCell release];
    [args release];
    [super dealloc];
}

-(BOOL)isLeaf
{
    return children == nil;
}

-(ResourceTreeItem *)childAtIndex:(int)index
{
    return (ResourceTreeItem *)[children objectAtIndex:index];
}

-(int)numChildren
{
    if(children==nil)
        return 0;
    else
        return [children count];
}

-(BOOL)addFromDictionary:(NSDictionary *)dict
{
    NSMutableArray *a = [[NSMutableArray alloc] init];
    NSEnumerator *enumerator = [dict keyEnumerator];
    id key;
    while((key = [enumerator nextObject])) {
        ResourceTreeItem *item = [[ResourceTreeItem alloc] initWithTitle:key];
        [a addObject:item];
        
        id value = [dict objectForKey:key];
        
        // its a dictionary itself
        if([value isKindOfClass:[NSDictionary class]]) {
            if(![item addFromDictionary:value])
                return FALSE;
        }
        // its a string
        else if([value isKindOfClass:[NSString class]]) {
            if(![item parseResourceString:(NSString *)value])
                return FALSE;
        }
        // unknown
        else {
            NSLog(@"Invalid Class in Dictionary: %@",[value class]);
            return FALSE;
        }
    }
    
    // sort array and keep
    children = [a sortedArrayUsingSelector:@selector(compare:)];
    [children retain];
    [a release];
    return TRUE;
}

-(BOOL)parseResourceString:(NSString *)string
{
    args = [string componentsSeparatedByString:@","];
    [args retain];
    numArgs = [args count];
    if(numArgs < 2) {
        NSLog(@"ERROR: resource string invalid: %@",string);
        return FALSE;
    }
    
    resource = (NSString *)[args objectAtIndex:0];
    [resource retain];

    NSString *typeStr = (NSString *)[args objectAtIndex:1];
    unichar ch = [typeStr characterAtIndex:0];
    switch(ch) {
    case 'i': // integer
        type = ResourceTreeItemTypeInteger;
        hint = ResourceTreeItemHintNone;
        break;
    case 'b': // boolean
        type = ResourceTreeItemTypeInteger;
        hint = ResourceTreeItemHintBoolean;
        break;
    case 'e': // enum
        type = ResourceTreeItemTypeInteger;
        hint = ResourceTreeItemHintEnum;
        break;
    case 'E': // enum direct
        type = ResourceTreeItemTypeInteger;
        hint = ResourceTreeItemHintEnumDirect;
        break;
    case 'r': // range
        type = ResourceTreeItemTypeInteger;
        hint = ResourceTreeItemHintRange;
        break;
    case 's': // string
        type = ResourceTreeItemTypeString;
        hint = ResourceTreeItemHintNone;
        break;
    case 'f': // file
        {
            unichar mode = [typeStr characterAtIndex:1];
            type = ResourceTreeItemTypeString;
            switch(mode) {
            case 'o':
                hint = ResourceTreeItemHintFileOpen;
                break;
            case 's':
                hint = ResourceTreeItemHintFileSave;
                break;
            case 'd':
                hint = ResourceTreeItemHintFileDir;
                break;
            default:
                NSLog(@"ERROR: Invalid file resource mode: %@",typeStr);
                return false;
            }
        }
        break;
    default:
        NSLog(@"ERROR: Invalid resource type: %@",typeStr);
        return FALSE;
    }

    return TRUE;
}

-(NSString *)title
{
    return title;
}

-(int)parseIntFromString:(NSString *)string
{
    int len = [string length];
    if(len==0)
        return 0;

    int value = 0;
    const char *str = [string cStringUsingEncoding:NSUTF8StringEncoding];
    if(*str=='$') {
        sscanf(str+1,"%x",&value);
    } else {
        sscanf(str,"%d",&value);
    }
    return value;
}

-(id)getValue:(id)ctl
{
    ResourceEditorController *controller = (ResourceEditorController *)ctl;
    if([self isLeaf]) {
        if(cacheValue == nil) {
            if(type == ResourceTreeItemTypeInteger) {
                int value = [controller getIntResource:resource];
//                NSLog(@"read integer: %@ %d",resource,value);
                
                // enum
                if(hint == ResourceTreeItemHintEnum) {
                    cacheValue = [[args objectAtIndex:value+2] retain];
                } 
                // direct enum
                else if(hint == ResourceTreeItemHintEnumDirect) {
                    int i;
                    for(i=2;i<numArgs;i++) {
                        NSString *argString = (NSString *)[args objectAtIndex:i];
                        int argValue = [self parseIntFromString:argString];
                        if(argValue == value) {
                            cacheValue = [argString retain];
                            break;
                        }
                    }
                }
                // integer
                else {
                    cacheValue = [[NSNumber alloc] initWithInt:value];
                }
            }
            else {
                NSString *value = [controller getStringResource:resource];
//                NSLog(@"read string: %@ %@",resource,value);
                cacheValue = [value retain];
            }
        }
        return cacheValue; 
    }
    else
        return @"";
}

-(void)setIntResourceToValue:(int)value withController:(id)ctl
{
    ResourceEditorController *controller = (ResourceEditorController *)ctl;
    [controller setIntResource:resource toValue:value];
    [self invalidateCache];
    [controller reloadItem:self];
//    NSLog(@"set int resource %@ %d",resource,value);
}

-(void)setStringResourceToValue:(NSString *)value withController:(id)ctl
{
    ResourceEditorController *controller = (ResourceEditorController *)ctl;
    [controller setStringResource:resource toValue:value];
    [self invalidateCache];
    [controller reloadItem:self];
//    NSLog(@"set string resource %@ %d",resource,value);    
}

-(void)setValue:(id)ctl toObject:(id)object
{
    NSString *string = (NSString *)object;
    if(type == ResourceTreeItemTypeInteger) {
        int value = 0;
        // enum resource
        if(hint==ResourceTreeItemHintEnum ||
           hint==ResourceTreeItemHintEnumDirect) {
            int i;
            for(i=2;i<numArgs;i++) {
                NSString *argVal = (NSString *)[args objectAtIndex:i];
                if([argVal isEqualToString:string]) {
                    if(hint==ResourceTreeItemHintEnum)
                        value = i-2;
                    else 
                        value = [self parseIntFromString:argVal];
                    break;
                }
            }
        } 
        // range
        else if(hint==ResourceTreeItemHintRange) {
            if(numArgs==4) {
                int min = [[args objectAtIndex:2] intValue];
                int max = [[args objectAtIndex:3] intValue];
                if(value<min)
                    value = min;
                else if(value>max)
                    value = max;
            } else {
                NSLog(@"ERROR: range invalid: %@",args);
            }
        }
        // integer resource
        else {
            value = [string intValue];
        }
        [self setIntResourceToValue:value withController:ctl];
    } else {
        [self setStringResourceToValue:string withController:ctl];
    }
}

-(void)setValueExtra:(id)ctl
{
    ResourceEditorController *controller = (ResourceEditorController *)ctl;
    switch(hint) {
    case ResourceTreeItemHintFileOpen:
        {
            NSString *file = [controller pickOpenFileWithTitle:title types:nil];
            if(file!=nil)
                [self setStringResourceToValue:file withController:ctl];
            break;
        }
    case ResourceTreeItemHintFileSave:
        {
            NSString *file = [controller pickSaveFileWithTitle:title types:nil];
            if(file!=nil)
                [self setStringResourceToValue:file withController:ctl];
            break;
        }
    case ResourceTreeItemHintFileDir:
        {
            NSString *file = [controller pickDirectoryWithTitle:title];
            if(file!=nil)
                [self setStringResourceToValue:file withController:ctl];
            break;
        }
    }
}

-(NSCell *)dataCell:(NSCell *)colCell
{
    if([self isLeaf]) {
        if(dataCell!=nil)
            return dataCell;
            
        switch(hint) {
        // boolean cell
        case ResourceTreeItemHintBoolean:
            {
                NSButtonCell *bcell = [[NSButtonCell alloc] initTextCell:@""];
                [bcell setButtonType:NSSwitchButton];
                dataCell = bcell;
                break;
            }
        case ResourceTreeItemHintEnum:
        case ResourceTreeItemHintEnumDirect:
            {
                NSComboBoxCell *ccell = [[NSComboBoxCell alloc] initTextCell:@""];
                int i;
                [ccell setButtonBordered:FALSE];
                for(i=2;i<numArgs;i++) {
                    [ccell addItemWithObjectValue:[args objectAtIndex:i]];
                }
                dataCell = ccell;
                break;
            }
        default:
            return colCell;
        }
        // reuse default font
        [dataCell setFont:[colCell font]];
        return dataCell;
    }
    return colCell;
}

-(NSCell *)extraCell:(NSCell *)colCell
{
    if([self isLeaf]) {
        if(extraCell!=nil) {
            return extraCell;
        }
        
        switch(hint) {
        // file name cell
        case ResourceTreeItemHintFileOpen:
        case ResourceTreeItemHintFileSave:
        case ResourceTreeItemHintFileDir:
            {
                NSButtonCell *bcell = [[NSButtonCell alloc] initTextCell:@""];
                [bcell setButtonType:NSMomentaryLightButton];
                [bcell setTitle:@"..."];
                extraCell = bcell;                
                break;
            }
        default:
            return colCell;
        }
        [extraCell setFont:[colCell font]];
        return extraCell;
    }
    return colCell;
}

-(NSComparisonResult)compare:(ResourceTreeItem *)item
{
    return [title caseInsensitiveCompare:[item title]];
}

-(void)invalidateCache
{
    if([self isLeaf]) {
        [cacheValue release];
        cacheValue = nil;
    } else {
        int num = [children count];
        int i;
        for(i=0;i<num;i++) {
            [(ResourceTreeItem *)[children objectAtIndex:i] invalidateCache];
        }
    }
}

@end
