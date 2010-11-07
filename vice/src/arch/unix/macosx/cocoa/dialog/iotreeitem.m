/*
 * iotreeitem.m - item of the io tree
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

#include "machine.h"

#import "viceapplication.h"
#import "iotreeitem.h"
#import "iotreewindowcontroller.h"

@implementation IOTreeItem

// root item or container without range
-(id)initWithTitle:(NSString *)t
{
    self = [super init];
    if (self == nil) {
        return nil;
    }

    title = [t retain];
    children = nil;
    buildChildren = nil;

    format = nil;
    
    minAddr = -1;
    maxAddr = -1;
    memory = nil;
    
    src = nil;
    
    return self;
}

// container with range
-(id)initWithTitle:(NSString *)t rangeMinAddr:(int)min maxAddr:(int)max
{
    self = [super init];
    if (self == nil) {
        return nil;
    }

    title = [t retain];
    children = nil;
    buildChildren = nil;

    format = nil;
    
    minAddr = min;
    maxAddr = max;
    memory = nil;
    
    src = nil;
    
    return self;    
}

// entry
-(id)initWithTitle:(NSString *)t format:(NSString *)f src:(NSArray *)s
{
    self = [super init];
    if (self == nil) {
        return nil;
    }

    title = [t retain];
    children = nil;
    buildChildren = nil;

    format = f;
    [format retain];
    
    minAddr = -1;
    maxAddr = -1;
    memory = nil;
    
    src = s;
    [src retain];
    
    return self;    
}

-(void)dealloc
{
    [title release];
    [children release];
    [format release];
    [src release];
    [memory release];
    [super dealloc];
}

-(void)setParent:(IOTreeItem *)item
{
    parent = item;
}

-(BOOL)isLeaf
{
    return children == nil;
}

-(IOTreeItem *)childAtIndex:(int)index
{
    return (IOTreeItem *)[children objectAtIndex:index];
}

-(int)numChildren
{
    if (children == nil) {
        return 0;
    } else {
        return [children count];
    }
}

-(void)addChild:(IOTreeItem *)item
{
    if(buildChildren == nil) {
        buildChildren = [[NSMutableArray alloc] init];
    }
    [buildChildren addObject:item];
}

-(IOTreeItem *)hasChildWithTitle:(NSString *)name
{
    if(buildChildren == nil)
        return nil;
    
    int numChilds = [buildChildren count];
    int i;
    for(i=0;i<numChilds;i++) {
        IOTreeItem *child = [buildChildren objectAtIndex:i];
        if([[child title] compare:name] == NSOrderedSame) {
            return child;
        }
    }
    return nil;
}

-(BOOL)addFromDictionaryLoop:(NSDictionary *)dict
{
    NSEnumerator *enumerator = [dict keyEnumerator];
    NSString *key;
    while ((key = [enumerator nextObject])) {
        id val = [dict objectForKey:key];
        // its a dictionary itself
        if ([val isKindOfClass:[NSDictionary class]]) {
            if(![self parseDictionary:val withTitle:key]) {
                return FALSE;
            }
        }
    }        
    return TRUE;
}
    
-(BOOL)parseDictionary:(NSDictionary *)dict withTitle:(NSString *)aTitle
{   
    // is it a container?
    NSDictionary *childs = [dict objectForKey:@"children"];
    IOTreeItem *newItem = nil;
    
    if(childs != nil) {
        NSLog(@"parsing container %@",aTitle);
        
        // check validity of this entry for the current machine
        NSString *valid = [dict objectForKey:@"valid"];
        if(valid != nil) {
            NSString *machineName = [NSString stringWithFormat:@"_%s_", machine_name];
            NSRange range = [valid rangeOfString:machineName];
            if(range.location == NSNotFound) {
                NSLog(@"  entry not valid for %@",machineName);
                return TRUE;
            }
        }
        
        // check range
        NSString *range = [dict objectForKey:@"range"];
        if(range != nil) {
            sscanf([range cStringUsingEncoding:NSUTF8StringEncoding],"%x-%x",&minAddr,&maxAddr);
            newItem = [[IOTreeItem alloc] initWithTitle:aTitle rangeMinAddr:minAddr maxAddr:maxAddr];
        } else {
            newItem = [[IOTreeItem alloc] initWithTitle:aTitle];
        }
    
        // now add children
        [newItem addFromDictionaryLoop:childs];
    
    } else {
        NSLog(@"parsing entry %@",aTitle);
        
        // type?
        NSString *aFormat = [dict objectForKey:@"format"];
        if(aFormat == nil) {
            NSLog(@"ERROR: no type in '%@'",aTitle);
            return FALSE;
        }

        // src array
        NSArray *aSrc = [dict objectForKey:@"src"];
        if(aSrc == nil) {
            NSLog(@"ERROR: no src in '%@'",aTitle);
            return FALSE;
        }

        // create entity
        newItem = [[IOTreeItem alloc] initWithTitle:aTitle format:aFormat src:aSrc];
    }

    // add item to this
    if(newItem != nil) {
        [self addChild:newItem];
        [newItem setParent:self];
    }
    return TRUE;
}

-(void)finalizeChildren
{
    int numChilds = [buildChildren count];
    int i;
    for(i=0;i<numChilds;i++) {
        IOTreeItem *child = [buildChildren objectAtIndex:i];
        [child finalizeChildren];
    }
    
    // sort array and keep
    children = [buildChildren sortedArrayUsingSelector:@selector(compare:)];
    [children retain];
    [buildChildren release];    
}

-(BOOL)addFromDictionary:(NSDictionary *)dict
{
    BOOL result = [self addFromDictionaryLoop:dict];
    [self finalizeChildren];
    return result;
}

-(NSString *)title
{
    return title;
}

-(id)getValue
{
    // no value available
    if(src == nil) {
        return nil;
    }
    
    if([format caseInsensitiveCompare:@"bool"]==NSOrderedSame) {
        // bool
        return value ? @"ON":@"OFF";
    } else {    
        // format value ala printf
        char result[10];
        NSString *finalFormat = [NSString stringWithFormat:@"%%%@", format];
        const char *formatStr = [finalFormat cStringUsingEncoding:NSUTF8StringEncoding];
        snprintf(result,10,formatStr,value);
        return [NSString stringWithCString:result encoding:NSUTF8StringEncoding];
    }
}

-(NSComparisonResult)compare:(IOTreeItem *)item
{
    return [title caseInsensitiveCompare:[item title]];
}

-(void)calcValue
{
    value = 0;
    int numSrc = [src count];
    int i;
    for(i=0;i<numSrc;i++) {
        NSString *srcEntry = [src objectAtIndex:i];
        if(srcEntry != nil) {
            int addr;
            int shift;
            int mask;
            int v;
            const char *str = [srcEntry cStringUsingEncoding:NSUTF8StringEncoding];
            // try: addr & mask < shift
            int num = sscanf(str,"%x&%d<%x",&addr,&mask,&shift);
            if(num == 3) {
                v = [self peek:addr];
                if(v!=-1) {
                    v &= mask;
                    v <<= shift;
                    value |= v;
                } else {
                    NSLog(@"ERROR: can't peek %04x",addr);
                }
            }
            else {
                num = sscanf(str,"%x&%d>%x",&addr,&mask,&shift);
                if(num > 0) {
                    v = [self peek:addr];
                    if(v!=-1) {
                        if(num>1) {
                            v &= mask;
                        }
                        if(num>2) {
                            v >>= shift;
                        }
                        value |= v;
                    } else {
                        NSLog(@"ERROR: can't peek %04x",addr);
                    }
                } else {
                    NSLog(@"ERROR: invalid src: %@",srcEntry);
                }
            }
        }
    }
}

-(void)readMemory:(int)memSpace
{
    // i have a range to read!
    if(minAddr != -1) {
        // fetch new mem dump
        if(memory != nil) {
            [memory release];
        }
        //NSLog(@"reading memory %04x...%04x",minAddr,maxAddr);
        memory = [[VICEApplication theMachineController] readMemory:memSpace bank:0 startAddress:minAddr endAddress:maxAddr];
        [memory retain];
    }
    
    // propagate
    int numChilds = [children count];
    int i;
    for(i=0;i<numChilds;i++) {
        IOTreeItem *child = [children objectAtIndex:i];
        [child readMemory:memSpace];
    }
    
    // calc value
    if(src != nil) {
        [self calcValue];
    }
}

-(int)peek:(int)addr
{
    if(memory != nil) {
        if((addr >= minAddr) && (addr <= maxAddr)) {
            return ((int)*((const char *)[memory bytes] + (addr - minAddr))) & 0xff;            
        }
    }
    
    if(parent != nil) {
        return [parent peek:addr];
    } else {
        return -1;
    }
}

@end
