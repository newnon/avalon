//
//  ACInterstitialView.h
//  AdClient SDK
//
//  Created by AdClient Developers on 10/16/12.
//  Copyright (c) 2014 AdClient Developers. All rights reserved.
//
//	Delegate protocol for ACBannerView.

#import <Foundation/Foundation.h>

@class ACInterstitialView;

// Protocol for ACInterstitialView notifications

@protocol ACInterstitialViewDelegate<NSObject>

@optional

// Notification: Interstitial ad started loading. State is changed to ACInterstitialViewStateLoading.
-(void)acInterstitialViewDidStartLoadAd:(ACInterstitialView *)acInterstitialView;

// Notification: Interstitial ad failed to load. State is changed to ACInterstitialViewStateFailed.
// Error code/type is output to debug console
-(void)acInterstitialViewDidFailLoadAd:(ACInterstitialView *)acInterstitialView withError:(NSObject *)error;

// Notification: Interstitial ad successfully loaded. State is changed to ACInterstitialViewStateReady.
-(void)acInterstitialViewDidLoadAd:(ACInterstitialView *)acInterstitialView;

// Notification: Interstitial ad starts to display ad. State is changed to ACInterstitialViewStateActive.
-(void)acInterstitialViewWillEnterModalMode:(ACInterstitialView *)acInterstitialView;

// Notification: Interstitial ad finished to display ad. State is changed to ACInterstitialViewStateDone.
-(void)acInterstitialViewDidLeaveModalMode:(ACInterstitialView *)acInterstitialView;

// Notification: User has interacted with ad provided by ACInterstitialView. Optional is application leaving to navigate to Safari, iTunes, etc.
-(void)acInterstitialViewUserInteraction:(ACInterstitialView *)acInterstitialView willLeaveApplication:(BOOL)yesOrNo;

@end
