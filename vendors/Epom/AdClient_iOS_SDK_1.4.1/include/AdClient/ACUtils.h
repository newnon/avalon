//
//  ACUtils.h
//  AdClient SDK
//
//  Created by AdClient Developers on 10/16/12.
//  Copyright (c) 2014 AdClient Developers. All rights reserved.
//
//	AdClient SDK Utilities class (ACUtils)

#import "ACEnumerations.h"

/*
 	Utilities class for different auxiliary purposes
 */

@interface ACUtils : NSObject

// 	Changing log level of AdClient SDK. Default value is ACVerboseErrorsOnly
+(void) setLogLevel:(ACVerboseType)verboseLevel;

// 	Set adclient advertisements server url with slash symbol at the end.
// Must be called before any other access to AdClient SDK
+(void) setAdsServerUrl:(NSString *)adsServerUrl;

@end