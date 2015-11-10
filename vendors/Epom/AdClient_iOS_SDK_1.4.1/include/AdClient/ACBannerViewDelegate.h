//
//  ACBannerViewDelegateProtocol.h
//  AdClient SDK
//
//  Created by AdClient Developers on 5/31/12.
//  Copyright (c) 2014 AdClient Developers. All rights reserved.
//
//	Delegate protocol for ACBannerView.

#import <Foundation/Foundation.h>

@class ACBannerView;

// Protocol for ACBannerView notifications

@protocol ACBannerViewDelegate<NSObject>

@optional

// Notification: Banner ad started loading.
-(void)acBannerViewDidStartLoadAd:(ACBannerView *)acBannerView;
  
// Notification: Banner ad failed to load.
// Error code/type is output to debug console
-(void)acBannerViewDidFailLoadAd:(ACBannerView *)acBannerView withError:(NSObject *)error;
  
// Notification: Banner ad successfully loaded.
-(void)acBannerViewDidLoadAd:(ACBannerView *)acBannerView;
  
// Notification: Ad provided by ACBannerView will be shown
-(void)acBannerViewWillShowAd:(ACBannerView *)acBannerView;

// Notification: Ad provided by ACBannerView is shown
-(void)acBannerViewDidShowAd:(ACBannerView *)acBannerView;

// Notification: Ad provided by ACBannerView has been tapped
-(void)acBannerViewAdHasBeenTapped:(ACBannerView *)acBannerView;

// Notification: Ad provided by ACBannerView will enter modal mode when opening embedded screen view controller
-(void)acBannerViewWillEnterModalMode:(ACBannerView *)acBannerView;

// Notification: Ad provided by ACBannerView did leave modal mode
-(void)acBannerViewDidLeaveModalMode:(ACBannerView *)acBannerView;

// Notification: Ad provided by ACBannerView causes to leave application to navigate to Safari, iTunes, etc.
-(void)acBannerViewWillLeaveApplication:(ACBannerView *)acBannerView;

@end
