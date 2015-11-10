#include "avalon/Epom.h"
#import "ACAdError.h"
#import "ACBannerView.h"
#import "ACInterstitialView.h"
#import "ACUtils.h"
#import "ACVersion.h"

namespace avalon {
    class IOSACInterstitial;
    class IOSACBanner;
}

@interface ACIOSBannerViewDelegate : NSObject<ACBannerViewDelegate>
{
    avalon::BannerDelegate *_delegate;
    avalon::IOSACBanner *_bannerView;
}
- (id) initWithDelegate:(avalon::BannerDelegate*) delegate andBannerView:(avalon::IOSACBanner*)bannerView;
@end

@interface ACIOSInterstitialDelegate : NSObject<ACInterstitialViewDelegate>
{
    avalon::InterstitialDelegate *_delegate;
    avalon::IOSACInterstitial *_interstitial;
}
- (id) initWithDelegate:(avalon::InterstitialDelegate*) delegate andInterstitial:(avalon::IOSACInterstitial*)interstitial;
@end

namespace avalon {
    
void ACUtils::setLogLevel(ACVerboseType verboseLevel)
{
    [::ACUtils setLogLevel:(::ACVerboseType)verboseLevel];
}
void ACUtils::setAdsServerUrl(const std::string &adsServerUrl)
{
    [::ACUtils setAdsServerUrl:[NSString stringWithUTF8String:adsServerUrl.c_str()]];
}
    
const std::string &ACUtils::getVersion()
{
    static std::string version = std::to_string(VERSION_MAJOR)+"."+std::to_string(VERSION_MINOR)+"."+std::to_string(VERSION_BUILD);
    return version;
}
    
class IOSACInterstitial:public ACInterstitial
{
public:
    
    IOSACInterstitial(const std::string &adUnitID, bool useLocation, bool testMode, InterstitialDelegate *delegate):_interstitial(nullptr)
    {
        _interstitial = [[::ACInterstitialView alloc] initWithID:[NSString stringWithUTF8String:adUnitID.c_str()] useLocation:useLocation testMode:testMode];
        _delegate = [[ACIOSInterstitialDelegate alloc] initWithDelegate:delegate andInterstitial:this];
        _interstitial.delegate = _delegate;
    }
    
    virtual const std::string &getType() const override
    {
        static std::string type = "epom";
        return type;
    }
    
    virtual void setCustomParameters(const std::map<std::string,std::string> &parameters) override
    {
        _customParameters = parameters;
    }

    virtual const std::map<std::string,std::string>& getCustomParameters() const override
    {
        return _customParameters;
    }

    virtual void setPreferExternalBrowser(bool value) override
    {
        _interstitial.preferExternalBrowser = value;
    }

    virtual bool getPreferExternalBrowser() const override
    {
        return _interstitial.preferExternalBrowser;
    }

    virtual void setLoadTimeout(float timeout) override
    {
        _interstitial.loadTimeout = timeout;
    }

    virtual float getLoadTimeout() const override
    {
        return _interstitial.loadTimeout;
    }
    
    virtual State getState() const override
    {
        return static_cast<State>(_interstitial.state);
    }
    
    virtual bool hide() override
    {
        return false;
    }
    
    virtual bool show() override
    {
        if(_interstitial.state != ACInterstitialViewStateReady)
            return false;
        [_interstitial presentWithViewController:[UIApplication sharedApplication].keyWindow.rootViewController];
        return true;
    }
    
    void reload()
    {
        [_interstitial reload];
    }
    
    virtual ~IOSACInterstitial()
    {
        [NSObject cancelPreviousPerformRequestsWithTarget:_interstitial.delegate];
        _interstitial.delegate = nil;
        [_interstitial autorelease];
        [_delegate release];
    }
    
private:
    ACIOSInterstitialDelegate *_delegate;
    ::ACInterstitialView *_interstitial;
    std::map<std::string,std::string> _customParameters;
};
    
ACInterstitial* ACInterstitial::create(const std::string &adUnitID, bool useLocation, bool testMode, InterstitialDelegate *delegate)
{
    return new IOSACInterstitial(adUnitID, useLocation, testMode, delegate);
}
    
class IOSACBanner:public ACBanner
{
public:
    IOSACBanner(const std::string &adUnitID, ACBannerViewSizeType size, bool useLocation, bool testMode, BannerDelegate *delegate):_ready(false)
    {
        _bannerView = [[::ACBannerView alloc] initWithID:[NSString stringWithUTF8String:adUnitID.c_str()] sizeType:(::ACBannerViewSizeType)size modalViewController:[UIApplication sharedApplication].keyWindow.rootViewController useLocation:useLocation testMode:testMode];
        _delegate = [[ACIOSBannerViewDelegate alloc] initWithDelegate:delegate andBannerView:this];
        _bannerView.delegate = _delegate;
    }
    
    virtual ~IOSACBanner()
    {
        [NSObject cancelPreviousPerformRequestsWithTarget:_bannerView.delegate];
        [_bannerView removeFromSuperview];
        _bannerView.delegate = nil;
        [_bannerView autorelease];
        [_delegate release];
    }
    
    virtual const std::string &getType() const override
    {
        static std::string type = "epom";
        return type;
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
    
    virtual void setCustomParameters(const std::map<std::string,std::string> &parameters) override
    {
        _customParameters = parameters;
    }
    
    virtual const std::map<std::string,std::string>& getCustomParameters() const override
    {
        return _customParameters;
    }
    
    virtual void setPreferExternalBrowser(bool value) override
    {
        _bannerView.preferExternalBrowser = value;
    }
    
    virtual bool getPreferExternalBrowser() const override
    {
        return _bannerView.preferExternalBrowser;
    }
    
    virtual void setRefreshTimeInterval(float timeout) override
    {
        _bannerView.refreshTimeInterval = timeout;
    }
    
    virtual float getRefreshTimeInterval() const override
    {
        return _bannerView.refreshTimeInterval;
    }
    
private:
    ACIOSBannerViewDelegate *_delegate;
    ::ACBannerView *_bannerView;
    bool _ready;
    std::map<std::string,std::string> _customParameters;
};
    
ACBanner* ACBanner::create(const std::string &adUnitID, ACBannerViewSizeType size, bool useLocation, bool testMode, BannerDelegate *delegate)
{
    return new IOSACBanner(adUnitID, size, useLocation, testMode, delegate);
}

}
    
@implementation ACIOSBannerViewDelegate

- (id) initWithDelegate:(avalon::BannerDelegate*) delegate andBannerView:(avalon::IOSACBanner*)bannerView;
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

-(void)acBannerViewDidStartLoadAd:(ACBannerView *)acBannerView
{
}

-(void)acBannerViewDidFailLoadAd:(ACBannerView *)acBannerView withError:(NSObject *)error
{
    _bannerView->setReady(true);
    if(_delegate)
        _delegate->bannerDidFailLoadAd(_bannerView, avalon::AdsErrorCode::INTERNAL_ERROR, 0, "");
}

-(void)acBannerViewDidLoadAd:(ACBannerView *)acBannerView
{
    _bannerView->setReady(true);
    if(_delegate)
        _delegate->bannerDidLoadAd(_bannerView);
}

-(void)acBannerViewWillShowAd:(ACBannerView *)acBannerView
{
    if(_delegate)
        _delegate->bannerWillShowAd(_bannerView);
}

-(void)acBannerViewDidShowAd:(ACBannerView *)acBannerView
{
    if(_delegate)
        _delegate->bannerDidShowAd(_bannerView);
    _bannerView->setReady(false);
}

-(void)acBannerViewAdHasBeenTapped:(ACBannerView *)acBannerView
{
    if(_delegate)
        _delegate->bannerUserInteraction(_bannerView);
}

-(void)acBannerViewWillEnterModalMode:(ACBannerView *)acBannerView
{
    if(_delegate)
        _delegate->bannerWillEnterModalMode(_bannerView);
}

-(void)acBannerViewDidLeaveModalMode:(ACBannerView *)acBannerView
{
    if(_delegate)
        _delegate->bannerDidLeaveModalMode(_bannerView);
}

-(void)acBannerViewWillLeaveApplication:(ACBannerView *)acBannerView
{
    if(_delegate)
        _delegate->bannerWillLeaveApplication(_bannerView);
}

@end
    
@implementation ACIOSInterstitialDelegate

- (id) initWithDelegate:(avalon::InterstitialDelegate*) delegate andInterstitial:(avalon::IOSACInterstitial*)interstitial;
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

- (void)reload
{
    _interstitial->reload();
}

-(void)acInterstitialViewDidStartLoadAd:(ACInterstitialView *)acInterstitialView
{
    if(_delegate)
        _delegate->interstitialDidStartLoadAd(_interstitial);
}

-(void)acInterstitialViewDidFailLoadAd:(ACInterstitialView *)acInterstitialView withError:(NSObject *)error
{
    if(_delegate)
        _delegate->interstitialDidFailLoadAd(_interstitial, avalon::AdsErrorCode::INTERNAL_ERROR, 0, "");
    [self performSelector:@selector(reload) withObject:nil afterDelay:10];
}

-(void)acInterstitialViewDidLoadAd:(ACInterstitialView *)acInterstitialView
{
    if(_delegate)
        _delegate->interstitialDidLoadAd(_interstitial);
}

-(void)acInterstitialViewWillEnterModalMode:(ACInterstitialView *)acInterstitialView
{
    if(_delegate)
        _delegate->interstitialWillShow(_interstitial);
}

-(void)acInterstitialViewDidLeaveModalMode:(ACInterstitialView *)acInterstitialView
{
    if(_delegate)
        _delegate->interstitialDidHide(_interstitial);
    [self performSelector:@selector(reload) withObject:nil afterDelay:10];
}

-(void)acInterstitialViewUserInteraction:(ACInterstitialView *)acInterstitialView willLeaveApplication:(BOOL)yesOrNo
{
    _delegate->interstitialUserInteraction(_interstitial, yesOrNo);
}
@end

