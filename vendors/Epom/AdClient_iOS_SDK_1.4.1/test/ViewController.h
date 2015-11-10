//
//  ViewController.h
//  test
//
//  Created by AdClient Developers on 6/11/12.
//  Copyright (c) 2012 AdClient Developers. All rights reserved.
//

#import <UIKit/UIKit.h>

#import "ACBannerViewDelegate.h"
#import "ACInterstitialViewDelegate.h"

@interface ViewController : UIViewController<ACBannerViewDelegate, ACInterstitialViewDelegate>
{
	IBOutlet UIButton *btnDisplayInterstitial;
}

-(IBAction)onBtnDisplayInterstitial:(id)sender;

@end
