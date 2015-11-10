//
//  ViewController.m
//  test
//
//  Created by AdClient Developers on 6/11/12.
//  Copyright (c) 2012 AdClient Developers. All rights reserved.
//

#import "ViewController.h"

#import "ACAdError.h"
#import "ACBannerView.h"
#import "ACInterstitialView.h"
#import "ACUtils.h"

static const int kActivityIndicatorTag = 'tact';

@interface ViewController ()
@end

@implementation ViewController

- (void)viewDidLoad
{
  [super viewDidLoad];
	// Do any additional setup after loading the view, typically from a nib.
	[ACUtils setAdsServerUrl:@"http://n29adshostnet.com/"];
    
	[ACUtils setLogLevel:ACVerboseAll];
	ACBannerView *acBannerView = [[[ACBannerView alloc] initWithID:@"b5314a01653a0eabd2db5918368044ac"
														  sizeType:ACBannerViewSize320x50
											   modalViewController:self
													   useLocation:NO
														  testMode:NO] autorelease];
	acBannerView.refreshTimeInterval = 10.0;
	acBannerView.delegate = self;
	
	[self.view addSubview: acBannerView];
}

- (void)viewDidUnload
{
    [super viewDidUnload];
    // Release any retained subviews of the main view.
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
	return (interfaceOrientation != UIInterfaceOrientationPortraitUpsideDown);
}

- (BOOL)shouldAutorotate
{
	return YES;
}

- (NSUInteger)supportedInterfaceOrientations
{
	return UIInterfaceOrientationMaskPortrait | UIInterfaceOrientationMaskPortraitUpsideDown;
}

#pragma mark -- ACBannerViewDelegate protocol implementation

// Notification: Ad provided by ACBannerView will be shown
-(void)acBannerViewWillShowAd:(ACBannerView *)acBannerView
{
	NSLog(@"ACBannerView will show ad");
}

// Notification: Ad provided by ACBannerView is shown
-(void)acBannerViewDidShowAd:(ACBannerView *)acBannerView
{
	NSLog(@"ACBannerView did show ad");
}

// Notification: Ad provided by ACBannerView has been tapped
-(void)acBannerViewAdHasBeenTapped:(ACBannerView *)acBannerView
{
	NSLog(@"ACBannerView advertisement has been tapped");	
}

// Notification: Ad provided by ACBannerView will enter modal mode when opening embedded screen view controller
-(void)acBannerViewWillEnterModalMode:(ACBannerView *)acBannerView
{
	NSLog(@"ACBannerView will enter modal mode");
}

// Notification: Ad provided by ACBannerView did leave modal mode
-(void)acBannerViewDidLeaveModalMode:(ACBannerView *)acBannerView
{
	NSLog(@"ACBannerView will leave modal mode");
}

// Notification: Ad provided by ACBannerView causes to leave application to navigate to Safari, iTunes, etc.
-(void)acBannerViewWillLeaveApplication:(ACBannerView *)acBannerView
{
	NSLog(@"ACBannerView causes application leaving");
}

#pragma mark -- ACInterstitialView support

-(void)onBtnDisplayInterstitial:(id)sender
{
	ACInterstitialView *acInterstitialView = [[ACInterstitialView alloc] initWithID:@"cc82905ae1baa12adc4771d3290793a2"
																		 useLocation:YES
																			testMode:YES];
	acInterstitialView.delegate = self;
}

-(void)showInterstitialActivityIndicator
{
	UIActivityIndicatorView *view = [[[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleGray] autorelease];
	view.center = CGPointMake(CGRectGetMidX(btnDisplayInterstitial.bounds), CGRectGetMidY(btnDisplayInterstitial.bounds));
	view.tag = kActivityIndicatorTag;
	[view startAnimating];
	[btnDisplayInterstitial addSubview:view];
	btnDisplayInterstitial.enabled = NO;
}

-(void)hideInterstitialActivityIndicator
{
	UIView *view = [btnDisplayInterstitial viewWithTag:kActivityIndicatorTag];
	[view removeFromSuperview];
	btnDisplayInterstitial.enabled = YES;
}

#pragma mark -- ACInterstitialView delegate implementation

// Notification: Interstitial ad started loading. State is changed to ACInterstitialViewStateLoading.
-(void)acInterstitialViewDidStartLoadAd:(ACInterstitialView *)acInterstitial
{
	[self showInterstitialActivityIndicator];
	NSLog(@"ACInterstitialView did start loading ad");
}

// Notification: Interstitial ad failed to load. State is changed to ACInterstitialViewStateFailed.
// Error code/type is output to debug console
-(void)acInterstitialViewDidFailLoadAd:(ACInterstitialView *)acInterstitial withError:(NSObject *)error
{
	[self hideInterstitialActivityIndicator];
	
	[acInterstitial release];
	
	NSLog(@"ACInterstitialView did fail to load ad");
	
	UIAlertView *alert = [[[UIAlertView alloc] initWithTitle:@"Error"
													 message:@"Failed to load interstitial ad. See console log output."
													delegate:nil
										   cancelButtonTitle:@"OK"
										   otherButtonTitles:nil, nil] autorelease];
	[alert show];
}

// Notification: Interstitial ad successfully loaded. State is changed to ACInterstitialViewStateReady.
-(void)acInterstitialViewDidLoadAd:(ACInterstitialView *)acInterstitial
{
	NSLog(@"ACInterstitialView loaded ad successfully");
	
	[acInterstitial presentWithViewController:self];
}

// Notification: Interstitial ad starts to display ad. State is changed to ACInterstitialViewStateActive.
-(void)acInterstitialViewWillEnterModalMode:(ACInterstitialView *)acInterstitial
{
	NSLog(@"ACInterstitialView will enter modal mode");
}

// Notification: Interstitial ad finished to display ad. State is changed to ACInterstitialViewStateDone.
-(void)acInterstitialViewDidLeaveModalMode:(ACInterstitialView *)acInterstitial
{
	NSLog(@"ACInterstitialView did leave modal mode");
	
	[acInterstitial release];

	[self hideInterstitialActivityIndicator];
}

// Notification: User has interacted with ad provided by ACInterstitialView. Optional is application leaving to navigate to Safari, iTunes, etc.
-(void)acInterstitialViewUserInteraction:(ACInterstitialView *)acInterstitialView willLeaveApplication:(BOOL)yesOrNo;
{
	NSLog(@"ACInterstitialView user interaction. Will leave application: %@", yesOrNo ? @"yes" : @"no");
}

@end
