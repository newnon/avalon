//
//  DDNAEngageArchive.h
//  DeltaDNASDK
//
//  Created by David White on 24/07/2014.
//  Copyright (c) 2014 deltadna. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface DDNAEngageArchive : NSObject

- (instancetype) initWithArchivePath: (NSString *) archiveDir;
- (instancetype) initWithArchivePath: (NSString *) archiveDir
                          clearStore: (BOOL) clearStore;
- (id) objectForKey: (NSString *) key;
- (void) setObject: (NSObject *) object forKey: (NSString *) key;
- (void) save;

@end
