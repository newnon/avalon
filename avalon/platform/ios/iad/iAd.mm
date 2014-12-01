#include "avalon/iAd.h"
#import <iAd/iAd.h>

namespace avalon {
    class IOSADInterstitialAd;
    class IOSADBannerView;
}

@interface ADIOSBannerViewDelegate : NSObject<ADBannerViewDelegate>
{
    avalon::IOSADBannerView *_bannerView;
    BOOL lastStateVisible;
}

@property (nonatomic, assign) avalon::BannerDelegate *delegate;

- (id) initWithBannerView:(avalon::IOSADBannerView*)bannerView;

@end

@interface ADIOSInterstitialDelegate : NSObject<ADInterstitialAdDelegate>
{
    avalon::IOSADInterstitialAd *_interstitial;
}

@property (nonatomic, assign) avalon::InterstitialDelegate *delegate;

- (id) initWithInterstitial:(avalon::IOSADInterstitialAd*)interstitial;

@end

namespace avalon {
    
class IOSADInterstitialAd:public IADInterstitial
{
public:
    IOSADInterstitialAd(InterstitialDelegate *delegate):_interstitial(nullptr)
    {
        _delegate = [[ADIOSInterstitialDelegate alloc] initWithInterstitial:this];
        _delegate.delegate = delegate;
        recreate();
    }
    ~IOSADInterstitialAd()
    {
        if(_interstitial)
            [_interstitial release];
        [_delegate release];
    }
    void closed()
    {
        [_interstitial release];
        _interstitial = [[::ADInterstitialAd alloc] init];
        _interstitial.delegate = _delegate;
    }
    void error()
    {
        [_interstitial release];
        _interstitial = [[::ADInterstitialAd alloc] init];
        _interstitial.delegate = _delegate;
    }
    virtual bool isReady() const override
    {
        if(!_interstitial)
            return false;
        return _interstitial.loaded;
    }
    virtual bool isVisible() const override
    {
        if(!_interstitial)
            return false;
        return _interstitial.actionInProgress;
    }
    virtual bool hide() override
    {
        if(!_interstitial)
            return false;
        [_interstitial cancelAction];
        return true;
    }
    virtual bool show() override
    {
        if(!_interstitial || !_interstitial.loaded )
            return false;
        [_interstitial presentFromViewController:[UIApplication sharedApplication].keyWindow.rootViewController];
        return true;
    }
    
    void recreate()
    {
        if(_interstitial)
        {
            [_interstitial release];
        }
        _interstitial = [[::ADInterstitialAd alloc] init];
        _interstitial.delegate = _delegate;
    }
    
    
private:
    ADIOSInterstitialDelegate *_delegate;
    ::ADInterstitialAd *_interstitial;
};
    
IADInterstitial* IADInterstitial::create(InterstitialDelegate *delegate)
{
    return new IOSADInterstitialAd(delegate);
}
    
class IOSADBannerView:public IADBanner
{
public:
    IOSADBannerView(ADAdType type, BannerDelegate *delegate):_type(type)
    {
        if ([::ADBannerView instancesRespondToSelector:@selector(initWithAdType:)])
        {
            _bannerView = [[::ADBannerView alloc] initWithAdType:ADAdTypeBanner];
        } else
        {
            _bannerView = [[::ADBannerView alloc] init];
        }
        _delegate = [[ADIOSBannerViewDelegate alloc] initWithBannerView:this];
        _bannerView.delegate = _delegate;
        CGRect bounds = [UIApplication sharedApplication].keyWindow.rootViewController.view.bounds;
        if (bounds.size.width < bounds.size.height) {
            _bannerView.currentContentSizeIdentifier = ADBannerContentSizeIdentifierPortrait;
        } else {
            _bannerView.currentContentSizeIdentifier = ADBannerContentSizeIdentifierLandscape;
        }
    }
    ~IOSADBannerView()
    {
        [_bannerView removeFromSuperview];
        _bannerView.delegate = nil;
        [_bannerView release];
        [_delegate release];
    }
    virtual ADAdType getADAdType() const
    {
        return _type;
    }
    virtual bool isReady() const override
    {
        return _bannerView.bannerLoaded;
    }

    virtual bool isVisible() const override
    {
        return [_bannerView isDescendantOfView:[UIApplication sharedApplication].keyWindow.rootViewController.view];
    }
    
    virtual bool prepare()
    {
        return false;
    }
    
    void showWithLastParams()
    {
        show(_lastRect, _lastScaleType, _lastGravity);
    }
    
    virtual bool show(const CGRect &rect, BannerScaleType scaleType, BannerGravityType gravity)
    {
        if(!isReady())
            return false;
        
        _lastRect = rect;
        _lastScaleType = scaleType;
        _lastGravity = gravity;
        
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
        
        CGRect bounds = [UIApplication sharedApplication].keyWindow.rootViewController.view.bounds;
        if (bounds.size.width < bounds.size.height) {
            _bannerView.currentContentSizeIdentifier = ADBannerContentSizeIdentifierPortrait;
        } else {
            _bannerView.currentContentSizeIdentifier = ADBannerContentSizeIdentifierLandscape;
        }
        
        if ([[UIScreen mainScreen] respondsToSelector:@selector(scale)] == YES) {
            float scale = [[UIScreen mainScreen] scale];
            rect.origin.x /= scale;
            rect.origin.y /= scale;
            rect.size.width /= scale;
            rect.size.height /= scale;
        }
        
        rect.origin.y = bounds.size.height - rect.size.height - rect.origin.y;
        return show(rect, scaleType, gravity);
    }
    virtual bool show(BannerScaleType scaleType, BannerGravityType gravity) override
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
    
    
private:
    ADAdType _type;
    ADIOSBannerViewDelegate *_delegate;
    ::ADBannerView *_bannerView;
    CGRect _lastRect;
    BannerScaleType _lastScaleType;
    BannerGravityType _lastGravity;
};
    
IADBanner* IADBanner::create(ADAdType type, BannerDelegate *delegate)
{
    return new IOSADBannerView(type, delegate);
}

}

@implementation ADIOSBannerViewDelegate

- (id) initWithBannerView:(avalon::IOSADBannerView*)bannerView
{
    self = [super init];
    if (self) {
        self->_delegate = nullptr;
        self->_bannerView = bannerView;
        self->lastStateVisible = NO;
    }
    
    [[NSNotificationCenter defaultCenter] addObserver: self
                                             selector: @selector(handleEnteredBackground:)
                                                 name: UIApplicationWillResignActiveNotification
                                               object: nil];
    
    [[NSNotificationCenter defaultCenter] addObserver: self
                                             selector: @selector(handleEnteredForeground:)
                                                 name: UIApplicationDidBecomeActiveNotification
                                               object: nil];
    
    return self;
}

- (void) dealloc
{
    [super dealloc];
    [[NSNotificationCenter defaultCenter] removeObserver:self];
}

- (void)handleEnteredBackground:(UIApplication*) application
{
    lastStateVisible = _bannerView->isVisible();
    if(lastStateVisible)
        _bannerView->hide();
}

- (void)handleEnteredForeground:(UIApplication*) application
{
    if(lastStateVisible)
        _bannerView->showWithLastParams();
}

- (void)bannerViewDidLoadAd:(ADBannerView *)banner
{
    if(_delegate)
        _delegate->bannerReceiveAd(_bannerView);
}
- (void)bannerView:(ADBannerView *)banner didFailToReceiveAdWithError:(NSError *)error
{
    if(_delegate)
        _delegate->bannerFailedToReceiveAd(_bannerView, avalon::AdsErrorCode::INTERNAL_ERROR, (int)error.code, [error.localizedDescription UTF8String]);
}
- (BOOL)bannerViewActionShouldBegin:(ADBannerView *)banner willLeaveApplication:(BOOL)willLeave
{
    if(_delegate)
        _delegate->bannerClick(_bannerView);
    return true;
}

@end

@implementation ADIOSInterstitialDelegate

- (id) initWithInterstitial:(avalon::IOSADInterstitialAd*)interstitial
{
    self = [super init];
    if (self) {
        self->_delegate = nullptr;
        self->_interstitial = interstitial;
    }
    
    return self;
}

- (void)interstitialAdDidUnload:(ADInterstitialAd *)interstitialAd
{
    if(_delegate)
        _delegate->interstitialClose(_interstitial);
}
- (void)interstitialAd:(ADInterstitialAd *)interstitialAd didFailWithError:(NSError *)error
{
    if(_delegate)
        _delegate->interstitialFailedToReceiveAd(_interstitial, avalon::AdsErrorCode::INTERNAL_ERROR, (int)error.code, [error.localizedDescription UTF8String]);
    _interstitial->error();
}

- (void)interstitialAdDidLoad:(ADInterstitialAd *)interstitialAd
{
    if(_delegate)
        _delegate->interstitialReceiveAd(_interstitial);
}

- (BOOL)interstitialAdActionShouldBegin:(ADInterstitialAd *)interstitialAd willLeaveApplication:(BOOL)willLeave
{
    if(_delegate)
        _delegate->interstitialClick(_interstitial);
    return true;
}

- (void)interstitialAdActionDidFinish:(ADInterstitialAd *)interstitialAd
{
}

@end
