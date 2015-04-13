//
//  DDNAEventBuilder.h
//  DeltaDNASDK
//
//  Created by David White on 25/07/2014.
//  Copyright (c) 2014 deltadna. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "DDNAProductBuilder.h"

@interface DDNAEventBuilder : NSObject

- (void) setString: (NSString *) value forKey: (NSString *) key;
- (void) setInteger: (NSInteger) value forKey: (NSString *) key;
- (void) setBoolean: (BOOL) value forKey: (NSString *) key;
- (void) setTimestamp: (NSDate *) value forKey: (NSString *) key;
- (void) setEventBuilder: (DDNAEventBuilder *) value forKey: (NSString *) key;
- (void) setProductBuilder: (DDNAProductBuilder *) value forKey: (NSString *) key;
- (NSDictionary *) dictionary;

@end
