#include "avalon/AdMob.h"
#ifdef __cplusplus
extern "C" {
#endif
#import "GADRequest.h"
#import "GADInterstitial.h"
#import "GADBannerView.h"
#import "GADAdSize.h"
#import "GADBannerViewDelegate.h"
#import "GADInterstitialDelegate.h"
#import "GADAdNetworkExtras.h"
#import "GADAdMobExtras.h"
#ifdef __cplusplus
}
#endif

namespace avalon {
class IOSGADInterstitial;
class IOSGADBannerView;
    
class IOSAdMob : public AdMob
{
public:
    
    IOSAdMob(const std::string &version);
    virtual void setAdNetworkExtras(GADAdNetworkExtras network, const std::map<std::string,std::string> &extras);
    void setTestDevices(const std::vector<std::string>& devices) override;
    void setGender(GADGender gender) override;
    void setBirthDate(unsigned month, unsigned day, unsigned year) override;
    void setLocation(float latitude, float longitude, float accuracyInMeters) override;
    void setLocation(const std::string &location) override;
    void setTagForChildDirectedTreatment(bool value) override;
    void setKeywords(const std::vector<std::string>& keywords) override;
    
    GADInterstitial* createIntestitial(const std::string &adUnitID, InterstitialDelegate *delegate) override;
    GADBannerView* createBanner(const std::string &adUnitID, GADAdSize size,  BannerDelegate *delegate) override;
    
    GADRequest* createRequest() const;
    
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
}

@interface GADIOSBannerViewDelegate : NSObject<GADBannerViewDelegate>
{
    avalon::BannerDelegate *_delegate;
    avalon::IOSGADBannerView *_bannerView;
}
- (id) initWithDelegate:(avalon::BannerDelegate*) delegate andBannerView:(avalon::IOSGADBannerView*)bannerView;
@end

@interface GADIOSInterstitialDelegate : NSObject<GADInterstitialDelegate>
{
    avalon::InterstitialDelegate *_delegate;
    avalon::IOSGADInterstitial *_interstitial;
}
- (id) initWithDelegate:(avalon::InterstitialDelegate*) delegate andInterstitial:(avalon::IOSGADInterstitial*)interstitial;
@end

namespace avalon {
    
GADAdSize makeCustomGADAdSize(unsigned short width, unsigned short height)
{
    return static_cast<GADAdSize>(width<<16 | height);
};
    
class IOSGADInterstitial:public GADInterstitial
{
public:
    
    IOSGADInterstitial(const std::string &adUnitID, InterstitialDelegate *delegate):GADInterstitial(adUnitID),_visible(false),_interstitial(nullptr)
    {
        _delegate = [[GADIOSInterstitialDelegate alloc] initWithDelegate:delegate andInterstitial:this];
        recreate();
    }
    
    virtual bool isReady() const override
    {
        return [_interstitial isReady] && ![_interstitial hasBeenUsed];
    }
    
    virtual bool isVisible() const override
    {
        return _visible;
    }
    
    virtual bool hide() override
    {
        return false;
    }
    
    virtual bool show() override
    {
        if(!isReady())
            return false;
        [_interstitial presentFromRootViewController:[UIApplication sharedApplication].keyWindow.rootViewController];
        return true;
    }
    
    void setVisible(bool value)
    {
        _visible = value;
    }
    
    void recreate()
    {
        if(_interstitial)
        {
            [_interstitial release];
        }
        _interstitial = [[::GADInterstitial alloc] init];
        _interstitial.adUnitID = [NSString stringWithCString:_adUnitID.c_str() encoding:NSUTF8StringEncoding];
        _interstitial.delegate = _delegate;
        [_interstitial loadRequest: static_cast<IOSAdMob*>(AdMob::getInstance())->createRequest()];
    }
    
    void loadAd()
    {
        [_interstitial loadRequest: static_cast<IOSAdMob*>(AdMob::getInstance())->createRequest()];
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
    bool _visible;
};
    
class IOSGADBannerView:public GADBannerView
{
public:
    IOSGADBannerView(const std::string &adUnitID, GADAdSize size, BannerDelegate *delegate):GADBannerView(adUnitID,size),_ready(false)
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
            case GADAdSize::Invalid:
                bannerSize = kGADAdSizeInvalid;
                break;
            default:
                {
                    unsigned short width = static_cast<unsigned short>(static_cast<unsigned int>(size)>>16);
                    unsigned short height = static_cast<unsigned short>(size);
                    bannerSize = GADAdSizeFromCGSize(CGSizeMake(width, height));
                }
                break;
        }
        
        _bannerView = [[::GADBannerView alloc] initWithAdSize:bannerSize];
        _bannerView.rootViewController = [UIApplication sharedApplication].keyWindow.rootViewController;
        _bannerView.adUnitID = [NSString stringWithCString:adUnitID.c_str() encoding:NSUTF8StringEncoding];
        _delegate = [[GADIOSBannerViewDelegate alloc] initWithDelegate:delegate andBannerView:this];
        _bannerView.delegate = _delegate;
        [_bannerView loadRequest: static_cast<IOSAdMob*>(AdMob::getInstance())->createRequest()];
    }
    
    virtual ~IOSGADBannerView()
    {
        [_bannerView removeFromSuperview];
        _bannerView.delegate = nil;
        [_bannerView autorelease];
        [_delegate release];
    }
    
    virtual bool isReady() const override
    {
        return _ready;
    }
    
    virtual bool isVisible() const override
    {
        return [_bannerView isDescendantOfView:[UIApplication sharedApplication].keyWindow.rootViewController.view];
    }
    
    bool show(const CGRect &rect, BannerScaleType scaleType, BannerGravityType gravity)
    {
        if(!_ready)
            return false;
        
        [[UIApplication sharedApplication].keyWindow.rootViewController.view addSubview:_bannerView];
        
        CGRect bounds = [UIApplication sharedApplication].keyWindow.rootViewController.view.bounds;
        
        float xScale = 1.0f;
        float yScale = 1.0f;
        
        switch (scaleType) {
            case BannerScaleType::Fill:
                xScale = bounds.size.width / _bannerView.bounds.size.width;
                yScale = bounds.size.height / _bannerView.bounds.size.height;
                break;
                
            case BannerScaleType::Proportional:
                xScale = bounds.size.width / _bannerView.bounds.size.width;
                yScale = bounds.size.height / _bannerView.bounds.size.height;
                xScale = std::min(xScale, yScale);
                yScale = xScale;
                break;
                
            default:
                break;
        }
        
        _bannerView.transform = CGAffineTransformScale(CGAffineTransformIdentity, xScale, yScale);
        
        switch (gravity) {
            case BannerGravityType::TopLeft:
                _bannerView.layer.anchorPoint = CGPointMake(0, 0);
                _bannerView.center = CGPointMake(rect.origin.x, rect.origin.y);
                break;
            case BannerGravityType::CenterLeft:
                _bannerView.layer.anchorPoint = CGPointMake(0, 0.5);
                _bannerView.center = CGPointMake(rect.origin.x, (rect.origin.y + rect.size.height)/2);
                break;
            case BannerGravityType::BottomLeft:
                _bannerView.layer.anchorPoint = CGPointMake(0, 1.0);
                _bannerView.center = CGPointMake(rect.origin.x, rect.origin.y + rect.size.height);
                break;
            case BannerGravityType::TopCenter:
                _bannerView.layer.anchorPoint = CGPointMake(0.5, 0);
                _bannerView.center = CGPointMake((rect.origin.x + rect.size.width)/2, rect.origin.y);
                break;
            case BannerGravityType::Center:
                _bannerView.layer.anchorPoint = CGPointMake(0.5, 0.5);
                _bannerView.center = CGPointMake((rect.origin.x + rect.size.width)/2, (rect.origin.y + rect.size.height)/2);
                break;
            case BannerGravityType::BottomCenter:
                _bannerView.layer.anchorPoint = CGPointMake(0.5, 1.0);
                _bannerView.center = CGPointMake((rect.origin.x + rect.size.width)/2, rect.origin.y + rect.size.height);
                break;
            case BannerGravityType::TopRight:
                _bannerView.layer.anchorPoint = CGPointMake(1.0, 0);
                _bannerView.center = CGPointMake(rect.origin.x + rect.size.width, rect.origin.y );
                break;
            case BannerGravityType::CenterRight:
                _bannerView.layer.anchorPoint = CGPointMake(1.0, 0.5);
                _bannerView.center = CGPointMake(rect.origin.x + rect.size.width, (rect.origin.y + rect.size.height)/2);;
                break;
            case BannerGravityType::BottomRight:
                _bannerView.layer.anchorPoint = CGPointMake(1.0, 1.0);
                _bannerView.center = CGPointMake(rect.origin.x + rect.size.width, rect.origin.y + rect.size.height);;
                break;
                
            default:
                break;
        }
        return true;
    }
    
    virtual bool show(int x, int y, int width, int height, BannerScaleType scaleType, BannerGravityType gravity) override
    {
        CGRect rect = CGRectMake(x, y, width, height);
        
        if ([[UIScreen mainScreen] respondsToSelector:@selector(scale)] == YES) {
            float scale = [[UIScreen mainScreen] scale];
            rect.origin.x /= scale;
            rect.origin.y /= scale;
            rect.size.width /= scale;
            rect.size.height /= scale;
        }
        
        CGRect bounds = [UIApplication sharedApplication].keyWindow.rootViewController.view.bounds;
        
        rect.origin.y = bounds.size.height - rect.size.height - rect.origin.y;
        return show(rect, scaleType, gravity);
    }
    
    virtual bool show(BannerScaleType scaleType, BannerGravityType gravity)
    {
        return show([UIApplication sharedApplication].keyWindow.rootViewController.view.bounds, scaleType, gravity);
    }
    void loadAd()
    {
        [_bannerView loadRequest: static_cast<IOSAdMob*>(AdMob::getInstance())->createRequest()];
    }
    
    virtual bool hide() override
    {
        if(!isVisible())
            return false;
        [_bannerView removeFromSuperview];
        return true;
    }
    void setReady(bool value)
    {
        _ready = value;
    }
    
private:
    GADIOSBannerViewDelegate *_delegate;
    ::GADBannerView *_bannerView;
    bool _ready;
};

    
IOSAdMob::IOSAdMob(const std::string &version):AdMob(version),_gender(GADGender::Unknown),_tagForChildDirectedTreatment(false,false) {}

void IOSAdMob::setAdNetworkExtras(GADAdNetworkExtras network, const std::map<std::string,std::string> &extras)
{
    _extras[network] = extras;
}
void IOSAdMob::setTestDevices(const std::vector<std::string>& devices)
{
    _testDevices = devices;
}
void IOSAdMob::setGender(GADGender gender)
{
    _gender = gender;
}
void IOSAdMob::setBirthDate(unsigned month, unsigned day, unsigned year)
{
    _birthDate.defined = true;
    _birthDate.month = month;
    _birthDate.day = day;
    _birthDate.year = year;
}
void IOSAdMob::setLocation(float latitude, float longitude, float accuracyInMeters)
{
    _location.defined = true;
    _location.latitude = latitude;
    _location.longitude = longitude;
    _location.accuracyInMeters = accuracyInMeters;
}
void IOSAdMob::setLocation(const std::string &location)
{
    _locationStr = location;
}
void IOSAdMob::setTagForChildDirectedTreatment(bool value)
{
    _tagForChildDirectedTreatment.first = true;
    _tagForChildDirectedTreatment.second = value;
}
void IOSAdMob::setKeywords(const std::vector<std::string>& keywords)
{
    _keywords = keywords;
}

GADInterstitial* IOSAdMob::createIntestitial(const std::string &adUnitID, InterstitialDelegate *delegate)
{
    return new IOSGADInterstitial(adUnitID, delegate);
}
    
GADBannerView* IOSAdMob::createBanner(const std::string &adUnitID, GADAdSize size,  BannerDelegate *delegate)
{
    return new IOSGADBannerView(adUnitID, size, delegate);
}

GADRequest* IOSAdMob::createRequest() const
{
    GADRequest *request = [GADRequest request];
    NSMutableArray *testDevices = [NSMutableArray arrayWithObject:GAD_SIMULATOR_ID];
    //[testDevices addObject:@"9ad070fbd963423d1d537b142209fcb5" ];
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
    

AdMob *AdMob::getInstance()
{
    static IOSAdMob *instance = new IOSAdMob([[GADRequest sdkVersion] cStringUsingEncoding:NSUTF8StringEncoding]);
    return instance;
}
    
}
    
@implementation GADIOSBannerViewDelegate

- (id) initWithDelegate:(avalon::BannerDelegate*) delegate andBannerView:(avalon::IOSGADBannerView*)bannerView
{
    self = [super init];
    if (self) {
        self->_delegate = delegate;
        self->_bannerView = bannerView;
    }
    
    return self;
}

- (void) dealloc
{
    [NSObject cancelPreviousPerformRequestsWithTarget:self];
    [super dealloc];
}

- (void)adViewDidReceiveAd:(GADBannerView *)view
{
    _bannerView->setReady(true);
    if(_delegate)
        _delegate->bannerReceiveAd(_bannerView);
}

- (void)loadAd
{
    _bannerView->loadAd();
}

- (void)adView:(GADBannerView *)view didFailToReceiveAdWithError:(GADRequestError *)error
{
    if(_delegate)
        _delegate->bannerFailedToReceiveAd(_bannerView, avalon::AdsErrorCode::INTERNAL_ERROR, (int)error.code, [error.localizedDescription UTF8String]);
    [self performSelector:@selector(loadAd) withObject:nil afterDelay:10];
}

- (void)adViewWillLeaveApplication:(GADBannerView *)adView
{
    if(_delegate)
        _delegate->bannerClick(_bannerView);
}

@end
    
@implementation GADIOSInterstitialDelegate

- (id) initWithDelegate:(avalon::InterstitialDelegate*) delegate andInterstitial:(avalon::IOSGADInterstitial*)interstitial
{
    self = [super init];
    if (self) {
        self->_delegate = delegate;
        self->_interstitial = interstitial;
    }
    
    return self;
}

- (void) dealloc
{
    [NSObject cancelPreviousPerformRequestsWithTarget:self];
    [super dealloc];
}

- (void)interstitialDidReceiveAd:(GADInterstitial *)ad
{
    if(_delegate)
        _delegate->interstitialReceiveAd(_interstitial);
}

- (void)loadAd
{
    _interstitial->loadAd();
}

- (void)interstitial:(GADInterstitial *)ad didFailToReceiveAdWithError:(GADRequestError *)error
{
    if(_delegate)
        _delegate->interstitialFailedToReceiveAd(_interstitial, avalon::AdsErrorCode::INTERNAL_ERROR, (int)error.code, [[error localizedDescription] UTF8String]);
    [self performSelector:@selector(loadAd) withObject:nil afterDelay:10];
}

- (void)interstitialWillPresentScreen:(GADInterstitial *)ad
{
    _interstitial->setVisible(true);
}

- (void)interstitialWillDismissScreen:(GADInterstitial *)ad
{
}

- (void)interstitialDidDismissScreen:(GADInterstitial *)ad
{
    if(_delegate)
        _delegate->interstitialClose(_interstitial);
    _interstitial->setVisible(false);
    _interstitial->recreate();
}

- (void)interstitialWillLeaveApplication:(GADInterstitial *)ad
{
    if(_delegate)
        _delegate->interstitialClick(_interstitial);
}
@end

