#include "avalon/iAd.h"
#import <iAd/iAd.h>

@interface ADIOSBannerViewDelegate : NSObject<ADBannerViewDelegate>
{
    avalon::ADBannerView *_bannerView;
}

@property (nonatomic, assign) avalon::ADBannerViewDelegate *delegate;

@end

@implementation ADIOSBannerViewDelegate

- (id) initWithBannerView:(avalon::ADBannerView*)bannerView
{
    self = [super init];
    if (self) {
        self->_delegate = nullptr;
        self->_bannerView = bannerView;
    }
    
    return self;
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

@interface ADIOSInterstitialDelegate : NSObject<ADInterstitialAdDelegate>
{
    avalon::ADInterstitialAd *_interstitial;
}

@property (nonatomic, assign) avalon::ADInterstitialAdDelegate *delegate;

@end


@implementation ADIOSInterstitialDelegate

- (id) initWithInterstitial:(avalon::ADInterstitialAd*)interstitial
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
}

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
        return [_interstitial presentInView:[UIApplication sharedApplication].keyWindow];
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
        [[UIApplication sharedApplication].keyWindow.rootViewController.view addSubview:_bannerView];
        _bannerView.hidden = YES;
    }
    ~IOSADBannerView()
    {
        [_bannerView removeFromSuperview];
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
    
    virtual void show(int x, int y, int width, int height, BannerScaleType scaleType, BannerGravityType gravity) override
    {
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
        _bannerView.hidden = NO;
    }
    virtual void hide() override
    {
        _bannerView.hidden = YES;
    }
    
private:
    ADAdType _type;
    ADIOSBannerViewDelegate *_delegate;
    ::ADBannerView *_bannerView;
};
    

ADBannerView* ADBannerView::createWithAdType(ADAdType type)
{
    return new IOSADBannerView(type);
}
    
}
