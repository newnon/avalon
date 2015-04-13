//
//  DDNAClientInfo.h
//  DeltaDNASDK
//
//  Created by David White on 18/07/2014.
//  Copyright (c) 2014 deltadna. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface DDNAClientInfo : NSObject

@property (nonatomic, copy, readonly) NSString *platform;
@property (nonatomic, copy, readonly) NSString *deviceName;
@property (nonatomic, copy, readonly) NSString *deviceModel;
@property (nonatomic, copy, readonly) NSString *deviceType;
@property (nonatomic, copy, readonly) NSString *hardwareVersion;
@property (nonatomic, copy, readonly) NSString *operatingSystem;
@property (nonatomic, copy, readonly) NSString *operatingSystemVersion;
@property (nonatomic, copy, readonly) NSString *manufacturer;
@property (nonatomic, copy, readonly) NSString *timezoneOffset;
@property (nonatomic, copy, readonly) NSString *countryCode;
@property (nonatomic, copy, readonly) NSString *languageCode;
@property (nonatomic, copy, readonly) NSString *locale;

+ (DDNAClientInfo *) sharedInstance;

@end
