//
//  DDNAEngageArchive.m
//  DeltaDNASDK
//
//  Created by David White on 24/07/2014.
//  Copyright (c) 2014 deltadna. All rights reserved.
//

#import "DDNAEngageArchive.h"
#import "DDNALog.h"

static NSString *const FILENAME = @"engagements.plist";

@interface DDNAEngageArchive ()
{
    NSString * path;
    NSMutableDictionary *archive;
}

@end

@implementation DDNAEngageArchive

- (instancetype) initWithArchivePath: (NSString *) archiveDir
{
    return [self initWithArchivePath:archiveDir clearStore:NO];
}

- (instancetype) initWithArchivePath: (NSString *) archiveDir clearStore: (BOOL) clearStore
{
    if ((self = [super init]))
    {
        path = archiveDir;
        
        if (!clearStore)
        {
            [self loadArchive:path];
        }
        else
        {
            archive = [NSMutableDictionary dictionary];
        }
    }
    return self;
}

- (id) objectForKey: (NSString *) key
{
    @synchronized(self)
    {
        return [archive objectForKey:key];
    }
}

- (void) setObject: (NSObject *) object forKey: (NSString *) key
{
    @synchronized(self)
    {
        [archive setObject:object forKey:key];
    }
}

- (void) save
{
    NSString *filename = [path stringByAppendingPathComponent:FILENAME];
    DDNALogDebug(@"Saving engagements to: %@", filename);
    @synchronized(self)
    {
        [archive writeToFile:filename atomically:YES];
    }
}

- (void) loadArchive: (NSString *) archiveDir
{
    NSString *filename = [archiveDir stringByAppendingPathComponent:FILENAME];
    archive = [NSMutableDictionary dictionaryWithContentsOfFile:filename];
    
    // If not file exists create a new empty directory
    if (archive == nil)
    {
        archive = [NSMutableDictionary dictionary];
    }
}

@end
