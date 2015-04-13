//
//  DDNAClientInfo.m
//  DeltaDNASDK
//
//  Created by David White on 18/07/2014.
//  Copyright (c) 2014 deltadna. All rights reserved.
//

#import "DDNAClientInfo.h"
#import <UIKit/UIDevice.h>
#import <sys/sysctl.h>


@implementation DDNAClientInfo

+ (DDNAClientInfo *) sharedInstance
{
    static dispatch_once_t pred = 0;
    __strong static DDNAClientInfo * _sharedObject = nil;
    dispatch_once(&pred, ^{
        _sharedObject = [[self alloc] init]; // or some other init method
    });
    return _sharedObject;
}

- (id) init
{
    if ((self = [super init]))
    {
        _platform = [self getPlatform];
        _deviceName = [self getDeviceName];
        _deviceModel = [self getDeviceModel];
        _deviceType = [self getDeviceType];
        _hardwareVersion = [self getHardwareVersion];
        _operatingSystem = [self getOperatingSystem];
        _operatingSystemVersion = [self getOperatingSystemVersion];
        _manufacturer = [self getManufacturer];
        _timezoneOffset = [self getTimezoneOffset];
        _countryCode = [self getCountryCode];
        _languageCode = [self getLanguageCode];
        _locale = [self getLocale];
    }
    return self;
}

- (NSString *) getPlatform
{
    NSString * model = [UIDevice currentDevice].model;
    if ([model hasPrefix:@"iPad"]) return @"IOS_TABLET";
    else if ([model hasPrefix:@"iPhone"]) return @"IOS_MOBILE";
    return @"IOS";
}

- (NSString *) getDeviceName
{
    size_t size;
    sysctlbyname("hw.machine", NULL, &size, NULL, 0);
    char *machine = malloc(size);
    sysctlbyname("hw.machine", machine, &size, NULL, 0);
    NSString * platform = [NSString stringWithUTF8String:machine];
    free(machine);
    
    if ([platform isEqualToString:@"iPhone1,1"]) return @"iPhone 1G";
    if ([platform isEqualToString:@"iPhone1,2"]) return @"iPhone 3G";
    if ([platform isEqualToString:@"iPhone2,1"]) return @"iPhone 3GS";
    if ([platform isEqualToString:@"iPhone3,1"]) return @"iPhone 4";
    if ([platform isEqualToString:@"iPhone3,2"]) return @"iPhone 4";
    if ([platform isEqualToString:@"iPhone3,3"]) return @"iPhone 4";
    if ([platform isEqualToString:@"iPhone4,1"]) return @"iPhone 4S";
    if ([platform isEqualToString:@"iPhone5,1"]) return @"iPhone 5";
    if ([platform isEqualToString:@"iPhone5,2"]) return @"iPhone 5";
    if ([platform isEqualToString:@"iPhone5,3"]) return @"iPhone 5C";
    if ([platform isEqualToString:@"iPhone5,4"]) return @"iPhone 5C";
    if ([platform isEqualToString:@"iPhone6,1"]) return @"iPhone 5S";
    if ([platform isEqualToString:@"iPhone6,2"]) return @"iPhone 5S";
    if ([platform isEqualToString:@"iPhone7,2"]) return @"iPhone 6";
    if ([platform isEqualToString:@"iPhone7,1"]) return @"iPhone 6 Plus";
    
    if ([platform isEqualToString:@"iPod1,1"]) return @"iPod Touch 1G";
    if ([platform isEqualToString:@"iPod2,1"]) return @"iPod Touch 2G";
    if ([platform isEqualToString:@"iPod3,1"]) return @"iPod Touch 3G";
    if ([platform isEqualToString:@"iPod4,1"]) return @"iPod Touch 4G";
    if ([platform isEqualToString:@"iPod5,1"]) return @"iPod Touch 5G";
    
    if ([platform isEqualToString:@"iPad1,1"]) return @"iPad 1G";
    if ([platform isEqualToString:@"iPad2,1"]) return @"iPad 2";
    if ([platform isEqualToString:@"iPad2,2"]) return @"iPad 2";
    if ([platform isEqualToString:@"iPad2,3"]) return @"iPad 2";
    if ([platform isEqualToString:@"iPad2,4"]) return @"iPad 2";
    if ([platform isEqualToString:@"iPad3,1"]) return @"iPad 3G";
    if ([platform isEqualToString:@"iPad3,2"]) return @"iPad 3G";
    if ([platform isEqualToString:@"iPad3,3"]) return @"iPad 3G";
    if ([platform isEqualToString:@"iPad3,4"]) return @"iPad 4G";
    if ([platform isEqualToString:@"iPad3,5"]) return @"iPad 4G";
    if ([platform isEqualToString:@"iPad3,6"]) return @"iPad 4G";
    if ([platform isEqualToString:@"iPad4,1"]) return @"iPad Air";
    if ([platform isEqualToString:@"iPad4,2"]) return @"iPad Air";
    if ([platform isEqualToString:@"iPad4,3"]) return @"iPad Air";

    if ([platform isEqualToString:@"iPad2,5"]) return @"iPad Mini 1G";
    if ([platform isEqualToString:@"iPad2,6"]) return @"iPad Mini 1G";
    if ([platform isEqualToString:@"iPad2,7"]) return @"iPad Mini 1G";
    if ([platform isEqualToString:@"iPad4,4"]) return @"iPad Mini 2G";
    if ([platform isEqualToString:@"iPad4,5"]) return @"iPad Mini 2G";
    if ([platform isEqualToString:@"iPad4,6"]) return @"iPad Mini 2G";
    
    if ([platform isEqualToString:@"i386"]) return @"Simulator";
    if ([platform isEqualToString:@"x86_64"]) return @"Simulator";
    
    return platform;
}

- (NSString *) getHardwareVersion
{
    size_t size;
    sysctlbyname("hw.machine", NULL, &size, NULL, 0);
    char *machine = malloc(size);
    sysctlbyname("hw.machine", machine, &size, NULL, 0);
    NSString * platform = [NSString stringWithUTF8String:machine];
    free(machine);
    
    NSError *error = NULL;
    NSRegularExpression *regex = [NSRegularExpression
                                  regularExpressionWithPattern:@"\\w+(\\d+,\\d+)"
                                  options:NSRegularExpressionCaseInsensitive
                                  error:&error];
    
    NSTextCheckingResult *match = [regex firstMatchInString:platform
                                                    options:0
                                                      range:NSMakeRange(0, [platform length])];
    if (match) {
        return [platform substringWithRange:[match rangeAtIndex:1]];
    }
    
    return nil;
}

- (NSString *) getDeviceModel
{
    return [UIDevice currentDevice].model;
}

- (NSString *) getDeviceType
{
    NSString * model = [UIDevice currentDevice].model;
    if ([model hasPrefix:@"iPad"]) return @"TABLET";
    else if ([model hasPrefix:@"iPhone"]) return @"MOBILE_PHONE";
    return @"UNKNOWN";
}

- (NSString *) getOperatingSystem
{
    if ([[UIDevice currentDevice].systemName  isEqual: @"iPhone OS"])
    {
        return @"IOS";
    }
    return @"OSX";
}

- (NSString *) getOperatingSystemVersion
{
    return [UIDevice currentDevice].systemVersion;
}

- (NSString *) getManufacturer
{
    return @"Apple Inc";
}

- (NSString *) getTimezoneOffset
{
    long secondsFromGMT = [[NSTimeZone localTimeZone] secondsFromGMT];
    int hoursFromGMT = floor(secondsFromGMT / 3600.0);
    return [NSString stringWithFormat:@"%+03i", hoursFromGMT];
}

- (NSString *) getCountryCode
{
    return [[NSLocale currentLocale] objectForKey:NSLocaleCountryCode];
}

- (NSString *) getLanguageCode
{
    return [[NSLocale currentLocale] objectForKey:NSLocaleLanguageCode];
}

- (NSString *) getLocale
{
    return ([self getLanguageCode]!=nil && [self getCountryCode]!=nil)?[NSString stringWithFormat:@"%@_%@",[self getLanguageCode],[self getCountryCode]]:nil;
}

@end
