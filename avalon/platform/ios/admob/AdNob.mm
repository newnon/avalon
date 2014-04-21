#include "avalon/AdMob.h"
#import "GADRequest.h"
#import "GADInterstitial.h"
#import "GADBannerView.h"
#import "GADAdSize.h"
#import "GADBannerViewDelegate.h"
#import "GADInterstitialDelegate.h"
#import "GADAdNetworkExtras.h"
#import "GADAdMobExtras.h"

namespace avalon {

class AdMobHelper
{
public:
    static GADRequest* createRequest(const AdMob &adMob)
    {
        GADRequest *request = [GADRequest request];
        NSMutableArray *testDevices = [NSMutableArray arrayWithObject:GAD_SIMULATOR_ID];
        for(auto it : adMob._testDevices)
            [testDevices addObject:[NSString stringWithCString:it.c_str() encoding:NSUTF8StringEncoding]];
        
        request.testDevices = testDevices;
        request.gender = (::GADGender)adMob._gender;
        if(adMob._birthDate.defined)
            [request setBirthdayWithMonth:adMob._birthDate.month day:adMob._birthDate.day year:adMob._birthDate.year];
        if(adMob._location.defined)
            [request setLocationWithLatitude:adMob._location.latitude longitude:adMob._location.longitude accuracy:adMob._location.accuracyInMeters];
        else
            if(!adMob._locationStr.empty())
                [request setLocationWithDescription:[NSString stringWithCString:adMob._locationStr.c_str() encoding:NSUTF8StringEncoding]];
        if(adMob._tagForChildDirectedTreatment.first)
            [request tagForChildDirectedTreatment:adMob._tagForChildDirectedTreatment.second];
        NSMutableArray *keyWords = [NSMutableArray array];
        for(auto it : adMob._keywords)
            [keyWords addObject:[NSString stringWithCString:it.c_str() encoding:NSUTF8StringEncoding]];
        request.keywords = keyWords;
        for(auto it : adMob._extras)
        {
            switch (it.first) {
                case GADAdNetworkExtras::AdMob:
                {
                    GADAdMobExtras *extra = [[GADAdMobExtras alloc] init];
                    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
                    for(auto mapIt : it.second)
                    {
                        [dict setObject:[NSString stringWithCString:mapIt.second.c_str() encoding:NSUTF8StringEncoding] forKey:[NSString stringWithCString:mapIt.first.c_str() encoding:NSUTF8StringEncoding]];
                    }
                    extra.additionalParameters = dict;
                    [request registerAdNetworkExtras:extra];
                }
                    break;
                    
                default:
                    break;
            }
        }
        
        return request;
    }
    static void removeBanner(AdMob &adMob,const GADBannerView *banner)
    {
        for (auto it = adMob._bannerViews.begin();it!=adMob._bannerViews.end();++it)
        {
            if(it->get() == banner)
            {
                adMob._bannerViews.erase(it);
                break;
            }
        }
    }
    static void removeInterstitial(AdMob &adMob,const GADInterstitial *interstitial)
    {
        for (auto it = adMob._interstitials.begin();it!=adMob._interstitials.end();++it)
        {
            if(it->get() == interstitial)
            {
                adMob._interstitials.erase(it);
                break;
            }
        }
    }
};
    
}

@interface GADIOSBannerViewDelegate : NSObject<GADBannerViewDelegate>
{
    avalon::GADBannerViewDelegate *_delegate;
    avalon::GADBannerView *_bannerView;
}

@end


@implementation GADIOSBannerViewDelegate

- (id) initWithDelegate:(avalon::GADBannerViewDelegate*) delegate andBannerView:(avalon::GADBannerView*)bannerView
{
    self = [super init];
    if (self) {
        self->_delegate = delegate;
        self->_bannerView = bannerView;
    }
    
    return self;
}

- (void)adViewDidReceiveAd:(GADBannerView *)view
{
    if(_delegate)
        _delegate->adViewDidReceiveAd(_bannerView);
}
- (void)adView:(GADBannerView *)view didFailToReceiveAdWithError:(GADRequestError *)error
{
    if(_delegate)
        _delegate->adViewDidFailToReceive(_bannerView, [[error localizedDescription] cStringUsingEncoding:NSUTF8StringEncoding]);
    avalon::AdMobHelper::removeBanner(*avalon::AdMob::getInstance(), _bannerView);
}

- (void)adViewWillPresentScreen:(GADBannerView *)adView
{
    if(_delegate)
        _delegate->adViewWillPresentScreen(_bannerView);
}

- (void)adViewWillDismissScreen:(GADBannerView *)adView
{
    if(_delegate)
        _delegate->adViewWillDismissScreen(_bannerView);
}

- (void)adViewDidDismissScreen:(GADBannerView *)adView
{
    if(_delegate)
        _delegate->adViewDidDismissScreen(_bannerView);
    avalon::AdMobHelper::removeBanner(*avalon::AdMob::getInstance(), _bannerView);
}

- (void)adViewWillLeaveApplication:(GADBannerView *)adView
{
    if(_delegate)
        _delegate->adViewWillLeaveApplication(_bannerView);
}

@end

@interface GADIOSInterstitialDelegate : NSObject<GADInterstitialDelegate>
{
    avalon::GADInterstitialDelegate *_delegate;
    avalon::GADInterstitial *_interstitial;
}

@end


@implementation GADIOSInterstitialDelegate

- (id) initWithDelegate:(avalon::GADInterstitialDelegate*) delegate andInterstitial:(avalon::GADInterstitial*)interstitial
{
    self = [super init];
    if (self) {
        self->_delegate = delegate;
        self->_interstitial = interstitial;
    }
    
    return self;
}

- (void)interstitialDidReceiveAd:(GADInterstitial *)ad
{
    if(_delegate)
        _delegate->interstitialDidReceiveAd(_interstitial);
}

- (void)interstitial:(GADInterstitial *)ad didFailToReceiveAdWithError:(GADRequestError *)error
{
    if(_delegate)
        _delegate->interstitialDidFailToReceiveAd(_interstitial,[[error localizedDescription] cStringUsingEncoding:NSUTF8StringEncoding]);
    avalon::AdMobHelper::removeInterstitial(*avalon::AdMob::getInstance(), _interstitial);
}

- (void)interstitialWillPresentScreen:(GADInterstitial *)ad
{
    if(_delegate)
        _delegate->interstitialWillPresentScreen(_interstitial);
}

- (void)interstitialWillDismissScreen:(GADInterstitial *)ad
{
    if(_delegate)
        _delegate->interstitialWillDismissScreen(_interstitial);
}

- (void)interstitialDidDismissScreen:(GADInterstitial *)ad
{
    if(_delegate)
        _delegate->interstitialDidDismissScreen(_interstitial);
    avalon::AdMobHelper::removeInterstitial(*avalon::AdMob::getInstance(), _interstitial);
}

- (void)interstitialWillLeaveApplication:(GADInterstitial *)ad
{
    if(_delegate)
        _delegate->interstitialWillLeaveApplication(_interstitial);
}
@end

namespace avalon {
    
class IOSGADInterstitial:public GADInterstitial
{
public:
    
    IOSGADInterstitial(GADRequest *request, const std::string &adUnitID, GADInterstitialDelegate *delegate):GADInterstitial(adUnitID)
    {
        _interstitial = [[::GADInterstitial alloc] init];
        _interstitial.adUnitID = [NSString stringWithCString:adUnitID.c_str() encoding:NSUTF8StringEncoding];
        _delegate = [[GADIOSInterstitialDelegate alloc] initWithDelegate:delegate andInterstitial:this];
        _interstitial.delegate = _delegate;
        [_interstitial loadRequest:request];
    }
    
    virtual bool isReady() const
    {
        return [_interstitial isReady];
    }
    virtual bool hasBeenUsed() const
    {
        return [_interstitial hasBeenUsed];
    }
    
    virtual void show() override
    {
        [_interstitial presentFromRootViewController:[UIApplication sharedApplication].keyWindow.rootViewController];
    }
    
    virtual ~IOSGADInterstitial()
    {
        [_interstitial release];
        [_delegate release];
    }
    
private:
    GADIOSInterstitialDelegate *_delegate;
    ::GADInterstitial *_interstitial;
};
    
    
class IOSGADBannerView:public GADBannerView
{
public:
    IOSGADBannerView(GADRequest *request, const std::string &adUnitID, GADAdSize size, CGRect rect, GADBannerViewDelegate *delegate):GADBannerView(adUnitID,size)
    {
        ::GADAdSize bannerSize;
        switch (size) {
            case GADAdSize::Banner:
                bannerSize = kGADAdSizeBanner;
                break;
            case GADAdSize::MediumRectangle:
                bannerSize = kGADAdSizeMediumRectangle;
                break;
            case GADAdSize::FullBanner:
                bannerSize = kGADAdSizeFullBanner;
                break;
            case GADAdSize::Leaderboard:
                bannerSize = kGADAdSizeLeaderboard;
                break;
            case GADAdSize::Skyscraper:
                bannerSize = kGADAdSizeSkyscraper;
                break;
            case GADAdSize::SmartBannerPortrait:
                bannerSize = kGADAdSizeSmartBannerPortrait;
                break;
            case GADAdSize::SmartBannerLandscape:
                bannerSize = kGADAdSizeSmartBannerLandscape;
                break;
            default:
                bannerSize = kGADAdSizeInvalid;
                break;
        }
        _bannerView = [[::GADBannerView alloc] initWithAdSize:bannerSize origin:CGPointMake(rect.origin.x, rect.origin.y)];
        _bannerView.frame = rect;
        _bannerView.adUnitID = [NSString stringWithCString:adUnitID.c_str() encoding:NSUTF8StringEncoding];
        _delegate = [[GADIOSBannerViewDelegate alloc] initWithDelegate:delegate andBannerView:this];
        _bannerView.delegate = _delegate;
        [_bannerView loadRequest:request];
    }
    
    virtual ~IOSGADBannerView()
    {
        [_bannerView release];
        [_delegate release];
    }
    
    virtual bool hasAutoRefreshed() const
    {
        return [_bannerView hasAutoRefreshed];
    }
    
private:
    GADIOSBannerViewDelegate *_delegate;
    ::GADBannerView *_bannerView;
};
    
AdMob *AdMob::getInstance()
{
    static AdMob *instance = new AdMob([[GADRequest sdkVersion] cStringUsingEncoding:NSUTF8StringEncoding]);
    return instance;
}
    
const std::shared_ptr<GADInterstitial>& AdMob::createIntestitial(const std::string &adUnitID, GADInterstitialDelegate *delegate)
{
    _interstitials.emplace_back(new IOSGADInterstitial(AdMobHelper::createRequest(*this), adUnitID, delegate));
    return _interstitials.back();
}
    
const std::shared_ptr<GADBannerView>& AdMob::createBannerView(const std::string &adUnitID, GADAdSize size, int x, int y, int width, int height, GADBannerViewDelegate *delegate)
{
    _bannerViews.emplace_back(new IOSGADBannerView(AdMobHelper::createRequest(*this), adUnitID, size, CGRectMake(x, y, width, height), delegate));
    return _bannerViews.back();
}

}
