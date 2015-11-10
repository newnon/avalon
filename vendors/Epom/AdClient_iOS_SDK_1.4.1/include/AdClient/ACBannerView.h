//
//  ACBannerView.h
//  AdClient SDK
//
//  Created by AdClient Developers on 5/29/12.
//  Copyright (c) 2014 AdClient Developers. All rights reserved.
//
//	AdClient SDK BannerView class (ACBannerView)

#import <UIKit/UIKit.h>

#import "ACEnumerations.h"
#import "ACBannerViewDelegate.h"

// UIView-based class for retrieving and visualising banner advertisements
@interface ACBannerView : UIView

// delegate accessor
@property (readwrite, assign) id<ACBannerViewDelegate> delegate;

// refresh time interval accessor. Default value is 15 seconds. Minimal - 5 seconds.
@property (readwrite, assign) NSTimeInterval refreshTimeInterval;

// custom parameters for ad requests
@property (readwrite, retain) NSDictionary *customParameters;

// prefer external browser instead of in-app while opening landing pages, if possible. Default value is "NO
@property (readwrite, assign) BOOL preferExternalBrowser;

// Initializes a ACBannerView with specified id, size type, view controller for presenting
// modal views, optional force determining of user location and test mode enabled
-(id)initWithID:(NSString*)ID
	   sizeType:(ACBannerViewSizeType)size
modalViewController:(UIViewController *)modalViewController
	useLocation:(BOOL)doUseLocation
	   testMode:(BOOL)testMode;

// Force banner to load ad
-(void)loadAd;

@end
