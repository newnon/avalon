//
//  ACBannerView.h
//  AdClient SDK
//
//  Created by AdClient Developers on 2/18/15.
//  Copyright (c) 2015 AdClient Developers. All rights reserved.
//
//	AdClient SDK ad error class (ACAdError)

#import <Foundation/Foundation.h>

/// AdClient error domain.
extern NSString *const kACAdErrorDomain;

/// NSError codes for AdClient error domain.
typedef NS_ENUM(NSInteger, ACAdErrorCode) {
  
  // Network error occurs while ad loading
  kACAdErrorNetworkError = 0,
  
  // Server does not have any ads to display
  kACAdErrorNoAdsAvailable,
  
  // Ad request is timed out
  kACAdErrorTimedOut,
  
  // Recieved ad content can't be parser and/or displayed
  kACAdErrorCantReadWebContent,
  
  // Count of error codes
  kACAdErrorCodesCount
};

@interface ACAdError : NSError
@end
