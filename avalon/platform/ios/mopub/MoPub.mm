#include "avalon/MoPub.h"
#import "MPInterstitialAdController.h"
#include <map>


@interface IOSMPInterstitialAdControllerDelegate : NSObject<MPInterstitialAdControllerDelegate>
{
    avalon::MPInterstitialAdControllerDelegate *_delegate;
    avalon::MPInterstitialAdController *_interstitial;
}
@end


@implementation IOSMPInterstitialAdControllerDelegate

- (id) initWithDelegate:(avalon::MPInterstitialAdControllerDelegate*) delegate andInterstitial:(avalon::MPInterstitialAdController*)interstitial
{
    self = [super init];
    if (self) {
        self->_delegate = delegate;
        self->_interstitial = interstitial;
    }
    
    return self;
}

-(void)dealloc
{
    [super dealloc];
}

- (void)interstitialDidLoadAd:(MPInterstitialAdController *)interstitial
{
    if(_delegate)
        _delegate->interstitialDidLoadAd(_interstitial);
}
- (void)interstitialDidFailToLoadAd:(MPInterstitialAdController *)interstitial
{
    if(_delegate)
        _delegate->interstitialDidFailToLoadAd(_interstitial);
}

- (void)interstitialDidAppear:(MPInterstitialAdController *)interstitial
{
    if(_delegate)
        _delegate->interstitialDidAppear(_interstitial);
}

- (void)interstitialDidDisappear:(MPInterstitialAdController *)interstitial
{
    if(_delegate)
        _delegate->interstitialDidDisappear(_interstitial);
}

- (void)interstitialDidExpire:(MPInterstitialAdController *)interstitial
{
    if(_delegate)
        _delegate->interstitialDidExpire(_interstitial);
}
@end

namespace avalon {
    
class IOSMPInterstitialAdController: public MPInterstitialAdController
{
public:
    IOSMPInterstitialAdController(const std::string &adUnitId):_delegate(nullptr),_nativeDelegate(nullptr),_adController(nullptr),_adUnitId(adUnitId)
    {
        _adController = [::MPInterstitialAdController interstitialAdControllerForAdUnitId:[NSString stringWithCString:adUnitId.c_str() encoding:NSUTF8StringEncoding]];
    }
    
    virtual void setDelegate(MPInterstitialAdControllerDelegate *delegate) override
    {
        [_nativeDelegate release];
        _nativeDelegate = nullptr;
        if(delegate)
        {
            _nativeDelegate = [[IOSMPInterstitialAdControllerDelegate alloc] initWithDelegate:delegate andInterstitial:this];
        }
        [_adController setDelegate:_nativeDelegate];
        _delegate = delegate;
    }
    
    virtual MPInterstitialAdControllerDelegate* getDelegate() const override
    {
        return _delegate;
    }
    
    virtual const std::string& getAdUnitId() const override
    {
        return _adUnitId;
    }
    
    virtual void setLocation(float latitude, float longitude, float accuracyInMeters) override
    {
        [_adController setLocation:[[[CLLocation alloc] initWithLatitude:latitude longitude:longitude] autorelease]];
    }
    
    virtual void setKeywords(const std::string& keywords) override
    {
        _adController.keywords = [NSString stringWithCString:keywords.c_str() encoding:NSUTF8StringEncoding];
    }
    
    virtual void setTesting(bool testing) override
    {
        _adController.testing = testing;
    }
    
    virtual void loadAd() override
    {
        [_adController loadAd];
    }
    
    virtual bool getReady() const override
    {
        return _adController.ready;
    }
    
    virtual void show() override
    {
        [_adController showFromViewController:[UIApplication sharedApplication].keyWindow.rootViewController];
    }
    
    virtual ~IOSMPInterstitialAdController()
    {
        [_adController removeFromParentViewController];
        [_nativeDelegate release];
        [::MPInterstitialAdController removeSharedInterstitialAdController:_adController];
    }
    
private:
    std::string _adUnitId;
    MPInterstitialAdControllerDelegate *_delegate;
    IOSMPInterstitialAdControllerDelegate* _nativeDelegate;
    ::MPInterstitialAdController *_adController;
};
    
MPInterstitialAdController *MPInterstitialAdController::interstitialAdControllerForAdUnitId(const std::string &adUnitId)
{
    auto it = _sharedInterstitialAdControllers.find(adUnitId);
    if(it!=_sharedInterstitialAdControllers.end())
        return it->second;
    
    IOSMPInterstitialAdController *controller = new IOSMPInterstitialAdController(adUnitId);
    return _sharedInterstitialAdControllers.insert(std::make_pair(adUnitId, controller)).first->second;
    return nullptr;
}
    
std::map<std::string, MPInterstitialAdController*> MPInterstitialAdController::_sharedInterstitialAdControllers;

}
