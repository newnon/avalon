//
//  FlurryAdX.mm
//  MedPed
//
//  Created by Sergey on 26.03.14.
//
//

#include "avalon/FlurryAds.h"
#import "FlurryAds.h"
#import "FlurryAdDelegate.h"

static avalon::FlurryAdsDelegate* _delegate = nullptr;
static UIView* _bannerView;

@interface InternalFlurryDelegate : NSObject<FlurryAdDelegate> {
    
}
@end

@implementation InternalFlurryDelegate

- (void) spaceDidReceiveAd:(NSString*)adSpace
{
    if(_delegate)
    {
        std::string space = [adSpace cStringUsingEncoding:NSUTF8StringEncoding];
        _delegate->onDidReceiveAd(space);
    }
}

- (void) spaceDidFailToReceiveAd:(NSString*)adSpace error:(NSError *)error
{
    if(_delegate)
    {
        std::string space = [adSpace cStringUsingEncoding:NSUTF8StringEncoding];
        std::string errorStr = [[error localizedDescription] cStringUsingEncoding:NSUTF8StringEncoding];;
        _delegate->onDidFailToReceiveAd(space, errorStr);
    }
}

- (BOOL) spaceShouldDisplay:(NSString*)adSpace interstitial:(BOOL)interstitial
{
    if(!_delegate)
        return YES;
    
    std::string space = [adSpace cStringUsingEncoding:NSUTF8StringEncoding];
    return _delegate->shouldDisplayAd(space, interstitial);
}

- (void) spaceDidFailToRender:(NSString *)adSpace error:(NSError *)error
{
    if(_delegate)
    {
        std::string space = [adSpace cStringUsingEncoding:NSUTF8StringEncoding];
        std::string errorStr = [[error localizedDescription] cStringUsingEncoding:NSUTF8StringEncoding];;
        _delegate->onRenderFailed(space, errorStr);
    }
}

- (void)spaceWillDismiss:(NSString *)adSpace interstitial:(BOOL)interstitial
{
}

- (void)spaceDidDismiss:(NSString *)adSpace interstitial:(BOOL)interstitial
{
    if(_delegate)
    {
        std::string space = [adSpace cStringUsingEncoding:NSUTF8StringEncoding];
        _delegate->onAdClosed(space, interstitial);
    }
}

- (void)spaceWillLeaveApplication:(NSString *)adSpace
{
    if(_delegate)
    {
        std::string space = [adSpace cStringUsingEncoding:NSUTF8StringEncoding];
        _delegate->onApplicationExit(space);
    }
}

- (void) spaceWillExpand:(NSString *)adSpace
{
    if(_delegate)
    {
        std::string space = [adSpace cStringUsingEncoding:NSUTF8StringEncoding];
        _delegate->onAdOpened(space);
    }
}

- (void) spaceWillCollapse:(NSString *)adSpace
{
    
}

- (void) spaceDidCollapse:(NSString *)adSpace
{
}

- (void) spaceDidReceiveClick:(NSString*)adSpace
{
    if(_delegate)
    {
        std::string space = [adSpace cStringUsingEncoding:NSUTF8StringEncoding];
        _delegate->onAdClicked(space);
    }
}

- (void)videoDidFinish:(NSString *)adSpace
{
    if(_delegate)
    {
        std::string space = [adSpace cStringUsingEncoding:NSUTF8StringEncoding];
        _delegate->onVideoCompleted(space);
    }
}

@end

static InternalFlurryDelegate* _internalDelegate = nil;

namespace avalon {
    
static NSDictionary *nsDictionaryFromStringMap(const std::map<std::string,std::string> &stringMap) {
    if (stringMap.empty()) {
        return NULL;
    }
    
    NSMutableDictionary *nsDict = [NSMutableDictionary dictionaryWithCapacity:stringMap.size()];
    
    for(auto it=stringMap.begin();it!=stringMap.end();++it)
    {
        NSString *value = [NSString stringWithCString:it->second.c_str() encoding:NSUTF8StringEncoding];
        [nsDict setValue:value forKey:[NSString stringWithCString:it->first.c_str() encoding:NSUTF8StringEncoding]];
        
    }
    return nsDict;
}


void FlurryAds::fetchAdForSpace(const std::string &space, AdSize size)
{
    [::FlurryAds fetchAdForSpace:[NSString stringWithCString:space.c_str() encoding:NSUTF8StringEncoding] frame:[UIApplication sharedApplication].keyWindow.rootViewController.view.bounds size:(::FlurryAdSize)size];
}

bool FlurryAds::adReadyForSpace(const std::string &space)
{
    return [::FlurryAds adReadyForSpace:[NSString stringWithCString:space.c_str() encoding:NSUTF8StringEncoding]] == YES;
}
void FlurryAds::displayAdForSpace(const std::string &space)
{
    if(!_bannerView)
    {
        _bannerView = [[UIView alloc] initWithFrame:[UIApplication sharedApplication].keyWindow.rootViewController.view.bounds];
        [[UIApplication sharedApplication].keyWindow.rootViewController.view addSubview:_bannerView];
    }
    [::FlurryAds displayAdForSpace:[NSString stringWithCString:space.c_str() encoding:NSUTF8StringEncoding] onView:_bannerView viewControllerForPresentation:[UIApplication sharedApplication].keyWindow.rootViewController];
}
void FlurryAds::displayAdForSpaceModally(const std::string &space)
{
    [::FlurryAds displayAdForSpace:[NSString stringWithCString:space.c_str() encoding:NSUTF8StringEncoding] modallyForViewController:[UIApplication sharedApplication].keyWindow.rootViewController];
}
void FlurryAds::fetchAndDisplayAdForSpace(const std::string &space, AdSize size)
{
    if(!_bannerView)
    {
        _bannerView = [[UIView alloc] initWithFrame:[UIApplication sharedApplication].keyWindow.rootViewController.view.bounds];
        [[UIApplication sharedApplication].keyWindow.rootViewController.view addSubview:_bannerView];
    }
    [::FlurryAds fetchAndDisplayAdForSpace:[NSString stringWithCString:space.c_str() encoding:NSUTF8StringEncoding] view:_bannerView viewController:[UIApplication sharedApplication].keyWindow.rootViewController size:(::FlurryAdSize)size];
}
    
void FlurryAds::removeAdFromSpace(const std::string &space)
{
    [::FlurryAds removeAdFromSpace:[NSString stringWithCString:space.c_str() encoding:NSUTF8StringEncoding]];
}

void FlurryAds::initialize()
{
    [::FlurryAds initialize:[UIApplication sharedApplication].keyWindow.rootViewController];
}

void FlurryAds::setAdDelegate(FlurryAdsDelegate *delegate)
{
    if(delegate)
    {
        if(!_internalDelegate)
        {
            _internalDelegate = [[InternalFlurryDelegate alloc] init];
            [::FlurryAds setAdDelegate:_internalDelegate];
        }
    }
    else
    {
        if(_internalDelegate)
        {
            [_internalDelegate release];
            _internalDelegate = nil;
            [::FlurryAds setAdDelegate:nil];
        }
    }
     _delegate = delegate;
}
void FlurryAds::enableTestAds(bool enable)
{
    [::FlurryAds enableTestAds:enable];
}
void FlurryAds::setUserCookies(const std::map<std::string,std::string> &userCookies)
{
    [::FlurryAds setUserCookies:nsDictionaryFromStringMap(userCookies)];
}
void FlurryAds::clearUserCookies()
{
    [::FlurryAds clearUserCookies];
}
void FlurryAds::setKeywordsForTargeting(const std::map<std::string,std::string> &keywords)
{
    [::FlurryAds setKeywordsForTargeting:nsDictionaryFromStringMap(keywords)];
}
void FlurryAds::clearKeywords()
{
    [::FlurryAds clearKeywords];
}
/*void FlurryAdsX::addCustomAdNetwork(void *adNetworkClass, void *adNetworkProperties)
{
    
}*/

} // namespace avalon
