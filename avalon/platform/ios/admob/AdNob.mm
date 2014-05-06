#include "avalon/AdMob.h"
#import "GADRequest.h"
#import "GADInterstitial.h"
#import "GADBannerView.h"
#import "GADAdSize.h"
#import "GADBannerViewDelegate.h"
#import "GADInterstitialDelegate.h"
#import "GADAdNetworkExtras.h"
#import "GADAdMobExtras.h"

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
    avalon::AdMob::getInstance()->removeBanner(_bannerView);
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
    avalon::AdMob::getInstance()->removeBanner(_bannerView);
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
    avalon::AdMob::getInstance()->removeInterstitial(_interstitial);
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
    avalon::AdMob::getInstance()->removeInterstitial(_interstitial);
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
        return [_interstitial isReady] && ![_interstitial hasBeenUsed];
    }
    
    virtual void show() override
    {
        [_interstitial presentFromRootViewController:[UIApplication sharedApplication].keyWindow.rootViewController];
    }
    
    virtual ~IOSGADInterstitial()
    {
        _interstitial.delegate = nil;
        [_interstitial autorelease];
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
        _bannerView.delegate = nil;
        [_bannerView autorelease];
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

    
    
class IOSAdMob : public AdMob
{
public:
    
    IOSAdMob(const std::string &version):AdMob(version),_gender(GADGender::Unknown),_tagForChildDirectedTreatment(false,false) {}
    
    virtual void setAdNetworkExtras(GADAdNetworkExtras network, const std::map<std::string,std::string> &extras) override
    {
        _extras[network] = extras;
    }
    void setTestDevices(const std::vector<std::string>& devices) override
    {
        _testDevices = devices;
    }
    void setGender(GADGender gender) override
    {
        _gender = gender;
    }
    void setBirthDate(unsigned month, unsigned day, unsigned year) override
    {
        _birthDate.defined = true;
        _birthDate.month = month;
        _birthDate.day = day;
        _birthDate.year = year;
    }
    void setLocation(float latitude, float longitude, float accuracyInMeters) override
    {
        _location.defined = true;
        _location.latitude = latitude;
        _location.longitude = longitude;
        _location.accuracyInMeters = accuracyInMeters;
    }
    void setLocation(const std::string &location) override
    {
        _locationStr = location;
    }
    void setTagForChildDirectedTreatment(bool value) override
    {
        _tagForChildDirectedTreatment.first = true;
        _tagForChildDirectedTreatment.second = value;
    }
    void setKeywords(const std::vector<std::string>& keywords) override
    {
        _keywords = keywords;
    }
    
    std::shared_ptr<GADInterstitial> createIntestitial(const std::string &adUnitID, GADInterstitialDelegate *delegate) override
    {
        _interstitials.emplace_back(new IOSGADInterstitial(createRequest(), adUnitID, delegate));
        return _interstitials.back();
    }
    std::shared_ptr<GADBannerView> createBannerView(const std::string &adUnitID, GADAdSize size, int x, int y, int width, int height, GADBannerViewDelegate *delegate) override
    {
        _bannerViews.emplace_back(new IOSGADBannerView(createRequest(), adUnitID, size, CGRectMake(x, y, width, height), delegate));
        return _bannerViews.back();
    }
    
    virtual std::vector<std::shared_ptr<GADInterstitial>> getReadyInterstitials() const
    {
        std::vector<std::shared_ptr<GADInterstitial>> ret;
        for(auto &it : _interstitials)
        {
            if(it && it->isReady())
                ret.push_back(it);
            
        }
        return ret;
    }
    virtual std::vector<std::shared_ptr<GADBannerView>> getBannerViews() const
    {
        std::vector<std::shared_ptr<GADBannerView>> ret;
        for(auto &it : _bannerViews)
        {
            ret.push_back(it);
        }
        return ret;

    }
    
    GADRequest* createRequest() const
    {
        GADRequest *request = [GADRequest request];
        NSMutableArray *testDevices = [NSMutableArray arrayWithObject:GAD_SIMULATOR_ID];
        for(const auto &it : _testDevices)
            [testDevices addObject:[NSString stringWithCString:it.c_str() encoding:NSUTF8StringEncoding]];
        
        request.testDevices = testDevices;
        request.gender = (::GADGender)_gender;
        if(_birthDate.defined)
            [request setBirthdayWithMonth:_birthDate.month day:_birthDate.day year:_birthDate.year];
        if(_location.defined)
            [request setLocationWithLatitude:_location.latitude longitude:_location.longitude accuracy:_location.accuracyInMeters];
        else
            if(!_locationStr.empty())
                [request setLocationWithDescription:[NSString stringWithCString:_locationStr.c_str() encoding:NSUTF8StringEncoding]];
        if(_tagForChildDirectedTreatment.first)
            [request tagForChildDirectedTreatment:_tagForChildDirectedTreatment.second];
        NSMutableArray *keyWords = [NSMutableArray array];
        for(const auto &it : _keywords)
            [keyWords addObject:[NSString stringWithCString:it.c_str() encoding:NSUTF8StringEncoding]];
        request.keywords = keyWords;
        for(const auto &it : _extras)
        {
            switch (it.first) {
                case GADAdNetworkExtras::AdMob:
                {
                    GADAdMobExtras *extra = [[GADAdMobExtras alloc] init];
                    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
                    for(const auto &mapIt : it.second)
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
    
    virtual void removeBanner(const GADBannerView *bannerView) override
    {
        for (auto it = _bannerViews.begin();it!=_bannerViews.end();++it)
        {
            if(it->get() == bannerView)
            {
                _bannerViews.erase(it);
                break;
            }
        }
    }
    virtual void removeInterstitial(const GADInterstitial *interstitial) override
    {
        for (auto it = _interstitials.begin();it!=_interstitials.end();++it)
        {
            if(it->get() == interstitial)
            {
                _interstitials.erase(it);
                break;
            }
        }
    }
    
private:
    struct BirthDate
    {
        BirthDate():defined(false) {}
        bool defined;
        unsigned month;
        unsigned day;
        unsigned year;
    };
    struct Location
    {
        Location():defined(false) {}
        bool defined;
        unsigned latitude;
        unsigned longitude;
        unsigned accuracyInMeters;
    };
    std::map<GADAdNetworkExtras, std::map<std::string,std::string>> _extras;
    std::vector<std::string> _testDevices;
    GADGender _gender;
    BirthDate _birthDate;
    Location _location;
    std::string _locationStr;
    std::pair<bool,bool> _tagForChildDirectedTreatment;
    std::vector<std::string> _keywords;
    std::list<std::shared_ptr<GADBannerView>> _bannerViews;
    std::list<std::shared_ptr<GADInterstitial>> _interstitials;
};
    
AdMob *AdMob::getInstance()
{
    static IOSAdMob *instance = new IOSAdMob([[GADRequest sdkVersion] cStringUsingEncoding:NSUTF8StringEncoding]);
    return instance;
}

}
