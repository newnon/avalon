#include "avalon/FacebookAds.h"
#import <FBAudienceNetwork/FBAudienceNetwork.h>

namespace avalon {
class FBIOSInterstitial;
class FBIOSBanner;
}

@interface FBIOSBannerViewDelegate : NSObject<FBAdViewDelegate>
{
    avalon::BannerDelegate *_delegate;
    avalon::FBIOSBanner *_bannerView;
}
- (id) initWithDelegate:(avalon::BannerDelegate*) delegate andBannerView:(avalon::FBIOSBanner*)bannerView;
@end

@interface FBIOSInterstitialDelegate : NSObject<FBInterstitialAdDelegate>
{
    avalon::InterstitialDelegate *_delegate;
    avalon::FBIOSInterstitial *_interstitial;
}
- (id) initWithDelegate:(avalon::InterstitialDelegate*) delegate andInterstitial:(avalon::FBIOSInterstitial*)interstitial;
@end

namespace avalon {
    
FBAdSize makeCustomFBAdSize(unsigned short width, unsigned short height)
{
    return static_cast<FBAdSize>(width<<16 | height);
}

class FBIOSInterstitial:public FBInterstitial
{
public:
    
    FBIOSInterstitial(const std::string &placementID, avalon::InterstitialDelegate *delegate):FBInterstitial(placementID),_visible(false)
    {
        _interstitial = [[::FBInterstitialAd alloc] initWithPlacementID:[NSString stringWithUTF8String:placementID.c_str()]];
        _interstitial.delegate = [[FBIOSInterstitialDelegate alloc] initWithDelegate:delegate andInterstitial:this];
    }
    ~FBIOSInterstitial()
    {
        [_interstitial.delegate release];
        [_interstitial release];
    }
    
    virtual bool isReady() const override
    {
        return [_interstitial isAdValid];
    }
    virtual bool isVisible() const override
    {
        return _visible;
    }
    virtual bool prepare() override
    {
        if(![_interstitial isAdValid])
        {
            [_interstitial loadAd];
            return true;
        }
        return false;
    }
    virtual bool hide() override
    {
        return false;
    }
    virtual bool show() override
    {
        if(![_interstitial isAdValid])
            return false;
        _visible = [_interstitial showAdFromRootViewController:[UIApplication sharedApplication].keyWindow.rootViewController];
        return _visible;
    }
    
    void setVisible(bool value)
    {
        _visible = value;
    }
    
private:;
    ::FBInterstitialAd *_interstitial;
    bool _visible;
};

class FBIOSBanner:public FBBanner
{
public:
    FBIOSBanner(const std::string &placementID, FBAdSize size, BannerDelegate *delegate):FBBanner(placementID, size),_ready(false)
    {
        ::FBAdSize bannerSize;
        switch (size) {
            case FBAdSize::AdSize320x50:
                bannerSize = kFBAdSize320x50;
                break;
            case FBAdSize::AdSizeHeight50Banner:
                bannerSize = kFBAdSizeHeight50Banner;
                break;
            case FBAdSize::AdSizeHeight90Banner:
                bannerSize = kFBAdSizeHeight90Banner;
                break;
            default:
            {
                unsigned short width = static_cast<unsigned short>(static_cast<unsigned int>(size)>>16);
                unsigned short height = static_cast<unsigned short>(size);
                bannerSize.size.width = width;
                bannerSize.size.height = height;
            }
                break;
        }
        
        _bannerView = [[::FBAdView alloc] initWithPlacementID:[NSString stringWithUTF8String:placementID.c_str()] adSize:bannerSize rootViewController:[UIApplication sharedApplication].keyWindow.rootViewController];
        _bannerView.delegate = [[FBIOSBannerViewDelegate alloc] initWithDelegate:delegate andBannerView:this];
    }
    
    ~FBIOSBanner()
    {
        [_bannerView.delegate release];
        [_bannerView release];
    }
    virtual bool isReady() const override
    {
        return _ready;
    }
    virtual bool isVisible() const override
    {
        return [_bannerView isDescendantOfView:[UIApplication sharedApplication].keyWindow];
    };
    virtual bool prepare() override
    {
        if(!_ready)
        {
            [_bannerView loadAd];
            return true;
        }
        return false;
    }
    
    bool show(const CGRect &rect, BannerScaleType scaleType, BannerGravityType gravity)
    {
        if(!_ready)
            return false;
        
        [[UIApplication sharedApplication].keyWindow addSubview:_bannerView];
        
        CGRect frame = [UIApplication sharedApplication].keyWindow.frame;
        
        float xScale = 1.0f;
        float yScale = 1.0f;
        
        switch (scaleType) {
            case BannerScaleType::Fill:
                xScale = frame.size.width / _bannerView.frame.size.width;
                yScale = frame.size.height / _bannerView.frame.size.height;
                break;
                
            case BannerScaleType::Proportional:
                xScale = frame.size.width / _bannerView.frame.size.width;
                yScale = frame.size.height / _bannerView.frame.size.height;
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
        
        CGRect frame = [UIApplication sharedApplication].keyWindow.frame;
        
        rect.origin.y = frame.size.height - rect.size.height - rect.origin.y;
        
        return show(rect, scaleType, gravity);
    }
    
    virtual bool show(BannerScaleType scaleType, BannerGravityType gravity) override
    {
        return show([UIApplication sharedApplication].keyWindow.frame, scaleType, gravity);
    }
    
    virtual void hide() override
    {
        [_bannerView removeFromSuperview];
    }
    
    void setReady(bool value)
    {
        _ready = value;
    }
    
private:
    bool _ready;
    ::FBAdView *_bannerView;
};

class FBIOSAds: public FBAds
{
public:
    
    virtual const std::string& getVersion() const override
    {
        return _version;
    }
    
    virtual FBInterstitial* createInterstitial(const std::string &placementID, InterstitialDelegate *delegate) override
    {
        return new FBIOSInterstitial(placementID, delegate);
    }
    virtual FBBanner* createBanner(const std::string &placementID, FBAdSize adSize, BannerDelegate *delegate) override
    {
        return new FBIOSBanner(placementID, adSize, delegate);
    }

    virtual void addTestDevice(const std::string &deviceHash) override
    {
        [FBAdSettings addTestDevice:[NSString stringWithUTF8String:deviceHash.c_str()]];
    }

    virtual void addTestDevices(const std::vector<std::string> &deviceHash) override
    {
        NSMutableArray *array = [NSMutableArray arrayWithCapacity:deviceHash.size()];
        for(size_t i=0;i<deviceHash.size();++i)
            [array addObject:[NSString stringWithUTF8String:deviceHash[i].c_str()]];
        [FBAdSettings addTestDevices:array];
    }

    virtual void clearTestDevices() override
    {
        [FBAdSettings clearTestDevices];
    }

    virtual void setIsChildDirected(bool isChildDirected) override
    {
        [FBAdSettings setIsChildDirected:isChildDirected];
    }
    
    FBIOSAds():_version([FB_AD_SDK_VERSION UTF8String])
    {
        [FBAdSettings addTestDevice:@"f1dfefaa375f507d56a9cf6c6cd4c17b5332b156"];
    }
    
    
private:
    std::string _version;
};

FBAds *FBAds::getInstance()
{
    static FBIOSAds *instance = new FBIOSAds();
    return instance;
}
    
}

@implementation FBIOSBannerViewDelegate

- (id) initWithDelegate:(avalon::BannerDelegate*) delegate andBannerView:(avalon::FBIOSBanner*)bannerView
{
    self = [super init];
    if (self) {
        self->_delegate = delegate;
        self->_bannerView = bannerView;
    }
    
    return self;
}

- (void)adViewDidClick:(FBAdView *)adView
{
    if(_delegate)
        _delegate->bannerClick(_bannerView);
}

- (void)adViewDidLoad:(FBAdView *)adView
{
    _bannerView->setReady(true);
    if(_delegate)
        _delegate->bannerReceiveAd(_bannerView);
}

- (void)prepare
{
    _bannerView->prepare();
}

- (void)adView:(FBAdView *)adView didFailWithError:(NSError *)error
{
    _bannerView->setReady(false);
    [self performSelector:@selector(prepare) withObject:nil afterDelay:0];
    //[self performSelector:@selector(prepare)];
    if(_delegate)
        _delegate->bannerFailedToReceiveAd(_bannerView, error.code == 1001?avalon::AdsErrorCode::NO_FILL:avalon::AdsErrorCode::INTERNAL_ERROR, error.code, [[error localizedDescription] UTF8String]);
}
@end



@implementation FBIOSInterstitialDelegate

- (id) initWithDelegate:(avalon::InterstitialDelegate*) delegate andInterstitial:(avalon::FBIOSInterstitial*)interstitial
{
    self = [super init];
    if (self) {
        self->_delegate = delegate;
        self->_interstitial = interstitial;
    }
    
    return self;
}

- (void)interstitialAdDidClick:(FBInterstitialAd *)interstitialAd
{
    if(_delegate)
        _delegate->interstitialClick(_interstitial);
}

- (void)interstitialAdDidClose:(FBInterstitialAd *)interstitialAd
{
    _interstitial->setVisible(false);
    _interstitial->prepare();
    if(_delegate)
        _delegate->interstitialClose(_interstitial);
}

- (void)interstitialAdDidLoad:(FBInterstitialAd *)interstitialAd
{
    if(_delegate)
        _delegate->interstitialReceiveAd(_interstitial);
}

- (void)interstitialAd:(FBInterstitialAd *)interstitialAd didFailWithError:(NSError *)error
{
    _interstitial->prepare();
    if(_delegate)
        _delegate->interstitialFailedToReceiveAd(_interstitial, error.code == 1001?avalon::AdsErrorCode::NO_FILL:avalon::AdsErrorCode::INTERNAL_ERROR, error.code, [[error localizedDescription] UTF8String]);
}

@end
