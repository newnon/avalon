//
//  ACInterstitialView.h
//  AdClient SDK
//
//  Created by AdClient Developers on 10/16/12.
//  Copyright (c) 2014 AdClient Developers. All rights reserved.
//
//	AdClient SDK InterstitialView class (ACInterstitialView)

#import <UIKit/UIKit.h>

#import "ACEnumerations.h"
#import "ACInterstitialViewDelegate.h"


// Class for retrieving and visualising interstitial (full-screen) advertisements
@interface ACInterstitialView : NSObject

// delegate accessor
@property (readwrite, assign) id<ACInterstitialViewDelegate> delegate;

// return current interstitial state
@property (readonly) ACInterstitialViewStateType state;

// custom parameters for ad requests
@property (readwrite, retain) NSDictionary *customParameters;

// prefer external browser instead of in-app while opening landing pages, if possible. Default value is "NO"
@property (readwrite, assign) BOOL preferExternalBrowser;

// access interstitial load timeout. 0 means no timeout load interruption, minimum is 2 seconds. Default is 0.
@property (readwrite, assign) NSTimeInterval loadTimeout;

// initializes ACInterstitialView using string identifier of interstitial, optional user location use
// and test mode enabled. After initialization, view will try to load interstitial ad
-(id)initWithID:(NSString *)interstitialID useLocation:(BOOL)doUseLocation testMode:(BOOL)testMode;

// shows up interstitial view from view controller.
-(void)presentWithViewController:(UIViewController *)viewController;

// shows up interstitial view on application launch using application window and default image.
// For continue application work, users must listen delegate acInterstitialViewDidLeaveModalMode: message.
// BEWARE: for use only in application delegate's application:didFinishLaunchingWithOptions: method after
// main window makeKeyAndVisible call
-(void)presentAsStartupScreenWithWindow:(UIWindow *)window defaultImage:(UIImage *)image;

// loads another one interstitial ad. Do nothing if ACInterstitialView state is not
// ACInterstitialStateDone nor ACInterstitialStateFailed;
-(void)reload;

@end


