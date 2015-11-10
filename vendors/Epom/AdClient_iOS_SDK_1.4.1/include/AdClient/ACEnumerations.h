//
//  ACEnumerations.h
//  AdClient SDK
//
//  Created by AdClient Developers on 5/31/12.
//  Copyright (c) 2014 AdClient Developers. All rights reserved.



/*
	Available AdClient SDK banner view sizes
*/
typedef enum
{
  ACBannerViewSize320x50 	= 0,
  ACBannerViewSize300x50,
  ACBannerViewSize300x250,
  ACBannerViewSize768x90,
  ACBannerViewSize728x90,
  
  ACBannerViewSizeTypeCount // for inner usage
} ACBannerViewSizeType;

/*
 	Interstitial states
 */
typedef enum
{
	ACInterstitialViewStateInitializing = 0,
	ACInterstitialViewStateLoading,
	ACInterstitialViewStateFailed,
	ACInterstitialViewStateReady,
	ACInterstitialViewStateActive,
	ACInterstitialViewStateDone,
	
	ACInterstitialViewStateTypeCount // for inner usage
} ACInterstitialViewStateType;


/*
	AdClient SDK log verbose level
*/
typedef enum
{
	ACVerboseAll 	= 0,
	ACVerboseErrorsOnly,
	ACVerboseNone,
	
	ACVerboseTypeCount // for inner usage	
} ACVerboseType;
