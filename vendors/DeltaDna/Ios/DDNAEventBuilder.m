//
//  DDNAEventBuilder.m
//  DeltaDNASDK
//
//  Created by David White on 25/07/2014.
//  Copyright (c) 2014 deltadna. All rights reserved.
//

#import "DDNAEventBuilder.h"
#import "DDNAProductBuilder.h"
#import "NSString+Helpers.h"

@interface DDNAEventBuilder ()
{
    NSMutableDictionary * _dictionary;
}

@end

@implementation DDNAEventBuilder

- (id) init
{
    if ((self = [super init]))
    {
        _dictionary = [NSMutableDictionary dictionary];
    }
    return self;
}

- (void) setString: (NSString *) value forKey: (NSString *) key
{
    if (![NSString stringIsNilOrEmpty:value])
    {
        [_dictionary setObject:value forKey:key];
    }
}

- (void) setInteger: (NSInteger) value forKey: (NSString *) key
{
    [_dictionary setObject:[NSNumber numberWithInteger:value] forKey:key];
}

- (void) setBoolean: (BOOL) value forKey: (NSString *) key
{
    [_dictionary setObject:[NSNumber numberWithBool:value] forKey:key];
}

- (void) setTimestamp: (NSDate *) value forKey: (NSString *) key
{
    NSDateFormatter *dateFormatter = [[NSDateFormatter alloc] init];
    NSLocale *enUSPOSIXLocale = [[NSLocale alloc] initWithLocaleIdentifier:@"en_US_POSIX"];
    [dateFormatter setLocale:enUSPOSIXLocale];
    [dateFormatter setDateFormat:@"yyyy-MM-dd HH:mm:ss.SSS"];
    [_dictionary setObject:[dateFormatter stringFromDate:value] forKey:key];
}

- (void) setEventBuilder: (DDNAEventBuilder *) value forKey: (NSString *) key
{
    if (value != nil)
    {
        [_dictionary setObject:value.dictionary forKey:key];
    }
}

- (void) setProductBuilder: (DDNAProductBuilder *) value forKey: (NSString *) key
{
    if (value != nil)
    {
        [_dictionary setObject:value.dictionary forKey:key];
    }
}

- (NSDictionary *) dictionary
{
    return _dictionary;
}

@end
