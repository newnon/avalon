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

@property (nonatomic, assign) avalon::ADBannerViewDelegate *delegate;

- (id) initWithBannerView:(avalon::IOSADBannerView*)bannerView;

@end

@interface ADIOSInterstitialDelegate : NSObject<ADInterstitialAdDelegate>
{
    avalon::IOSADInterstitialAd *_interstitial;
}

@property (nonatomic, assign) avalon::ADInterstitialAdDelegate *delegate;

- (id) initWithInterstitial:(avalon::IOSADInterstitialAd*)interstitial;

@end

namespace avalon {
    
class IOSADInterstitialAd:public ADInterstitialAd
{
public:
    IOSADInterstitialAd()
    {
        _interstitial = [[::ADInterstitialAd alloc] init];
        _delegate = [[ADIOSInterstitialDelegate alloc] initWithInterstitial:this];
        _interstitial.delegate = _delegate;
    }
    ~IOSADInterstitialAd()
    {
        
    }
    void closed()
    {
        [_interstitial release];
        _interstitial = [[::ADInterstitialAd alloc] init];
        _delegate = [[ADIOSInterstitialDelegate alloc] initWithInterstitial:this];
        _interstitial.delegate = _delegate;
    }
    void error()
    {
        [_interstitial release];
        _interstitial = [[::ADInterstitialAd alloc] init];
        _delegate = [[ADIOSInterstitialDelegate alloc] initWithInterstitial:this];
        _interstitial.delegate = _delegate;
    }
    virtual void setDelegate(ADInterstitialAdDelegate *delegate) override
    {
        _delegate.delegate = delegate;
    }
    virtual ADInterstitialAdDelegate* getDelegate() const override
    {
        return _delegate.delegate;
    }
    virtual bool isLoaded() const override
    {
        return _interstitial.loaded;
    }
    virtual bool isActionInProgress() const override
    {
        return _interstitial.actionInProgress;
    }
    virtual void cancelAction() override
    {
        [_interstitial cancelAction];
    }
    virtual bool presentInView(/*UIView *containerView*/) override
    {
        [_interstitial presentFromViewController:[UIApplication sharedApplication].keyWindow.rootViewController];
        return true;
    }
    
private:
    ADIOSInterstitialDelegate *_delegate;
    ::ADInterstitialAd *_interstitial;
};
    
ADInterstitialAd* ADInterstitialAd::create()
{
    return new IOSADInterstitialAd;
}
    
class IOSADBannerView:public ADBannerView
{
public:
    IOSADBannerView(ADAdType type):_type(type)
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
        CGRect contentFrame = [UIApplication sharedApplication].keyWindow.rootViewController.view.bounds;
        if (contentFrame.size.width < contentFrame.size.height) {
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
    virtual void setDelegate(ADBannerViewDelegate *delegate) override
    {
        _delegate.delegate = delegate;
    }
    virtual ADBannerViewDelegate* getDelegate() const override
    {
        return _delegate.delegate;
    }
    virtual bool isBannerLoaded() const override
    {
        return _bannerView.bannerLoaded;
    }
    virtual bool isBannerViewActionInProgress() const override
    {
        return _bannerView.bannerViewActionInProgress;
    }
    virtual void cancelBannerViewAction() override
    {
        [_bannerView cancelBannerViewAction];
    }
    
    virtual bool isVisible() override
    {
        return [_bannerView isDescendantOfView:[UIApplication sharedApplication].keyWindow.rootViewController.view];
    }
    
    void showWithLastParams()
    {
        show(_lastX,_lastY,_lastWidth,_lastHeight,_lastScaleType,_lastGravity);
    }
    
    virtual void show(int x, int y, int width, int height, BannerScaleType scaleType, BannerGravityType gravity) override
    {
        _lastX = x;
        _lastY = y;
        _lastWidth = width;
        _lastHeight = height;
        _lastScaleType = scaleType;
        _lastGravity = gravity;
        
        
        [[UIApplication sharedApplication].keyWindow.rootViewController.view addSubview:_bannerView];
        CGRect rect = CGRectMake(x, y, width, height);
        
        CGRect contentFrame = [UIApplication sharedApplication].keyWindow.rootViewController.view.bounds;
        if (contentFrame.size.width < contentFrame.size.height) {
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
        
        rect.origin.y = contentFrame.size.height - rect.size.height - rect.origin.y;
        
        float xScale = 1.0f;
        float yScale = 1.0f;
        
        switch (scaleType) {
            case BannerScaleType::Fill:
                xScale = contentFrame.size.width / _bannerView.frame.size.width;
                yScale = contentFrame.size.height / _bannerView.frame.size.height;
                break;
                
            case BannerScaleType::Proportional:
                xScale = contentFrame.size.width / _bannerView.frame.size.width;
                yScale = contentFrame.size.height / _bannerView.frame.size.height;
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
    }
    virtual void hide() override
    {
        [_bannerView removeFromSuperview];
    }
    
    
private:
    ADAdType _type;
    ADIOSBannerViewDelegate *_delegate;
    ::ADBannerView *_bannerView;
    int _lastX;
    int _lastY;
    int _lastWidth;
    int _lastHeight;
    BannerScaleType _lastScaleType;
    BannerGravityType _lastGravity;
};
    

ADBannerView* ADBannerView::createWithAdType(ADAdType type)
{
    return new IOSADBannerView(type);
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

- (void)bannerViewWillLoadAd:(ADBannerView *)banner
{
    if(_delegate)
        _delegate->bannerViewWillLoadAd(_bannerView);
}
- (void)bannerViewDidLoadAd:(ADBannerView *)banner
{
    if(_delegate)
        _delegate->bannerViewWillLoadAd(_bannerView);
}
- (void)bannerView:(ADBannerView *)banner didFailToReceiveAdWithError:(NSError *)error
{
    if(_delegate)
        _delegate->bannerViewDidFailToReceiveAdWithError(_bannerView, static_cast<avalon::ADError>(error.code));
}
- (BOOL)bannerViewActionShouldBegin:(ADBannerView *)banner willLeaveApplication:(BOOL)willLeave
{
    if(_delegate)
        return _delegate->bannerViewActionShouldBegin(_bannerView, willLeave);
    return true;
}
- (void)bannerViewActionDidFinish:(ADBannerView *)banner
{
    if(_delegate)
        _delegate->bannerViewActionDidFinish(_bannerView);
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
        _delegate->interstitialAdDidUnload(_interstitial);
}
- (void)interstitialAd:(ADInterstitialAd *)interstitialAd didFailWithError:(NSError *)error
{
    if(_delegate)
        _delegate->interstitialDidFailWithError(_interstitial, static_cast<avalon::ADError>(error.code));
    _interstitial->error();
}

- (void)interstitialAdWillLoad:(ADInterstitialAd *)interstitialAd
{
    if(_delegate)
        _delegate->interstitialAdWillLoad(_interstitial);
}

- (void)interstitialAdDidLoad:(ADInterstitialAd *)interstitialAd
{
    if(_delegate)
        _delegate->interstitialAdDidLoad(_interstitial);
}

- (BOOL)interstitialAdActionShouldBegin:(ADInterstitialAd *)interstitialAd willLeaveApplication:(BOOL)willLeave
{
    if(_delegate)
        return _delegate->interstitialAdActionShouldBegin(_interstitial, willLeave);
    return true;
}

- (void)interstitialAdActionDidFinish:(ADInterstitialAd *)interstitialAd
{
    if(_delegate)
        _delegate->interstitialAdActionDidFinish(_interstitial);
    _interstitial->closed();
}

@end
