#include "avalon/MoPub.h"
#import "MoPub.h"
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

@interface IOSMPRewardedVideoDelegate : NSObject<MPRewardedVideoDelegate>
{
    avalon::MPRewardedVideoDelegate* _delegate;
}
@end


@implementation IOSMPRewardedVideoDelegate

- (id) initWithDelegate:(avalon::MPRewardedVideoDelegate*) delegate
{
    self = [super init];
    _delegate = delegate;
    return self;
}

- (void)rewardedVideoAdDidLoadForAdUnitID:(NSString *)adUnitID
{
    _delegate->rewardedVideoAdDidLoadForAdUnitID([adUnitID UTF8String]);
}

- (void)rewardedVideoAdDidFailToLoadForAdUnitID:(NSString *)adUnitID error:(NSError *)error
{
    _delegate->rewardedVideoAdDidFailToLoadForAdUnitID([adUnitID UTF8String], [error.localizedDescription UTF8String], (int)error.code);
}

- (void)rewardedVideoAdDidExpireForAdUnitID:(NSString *)adUnitID
{
    _delegate->rewardedVideoAdDidExpireForAdUnitID([adUnitID UTF8String]);
}

- (void)rewardedVideoAdDidFailToPlayForAdUnitID:(NSString *)adUnitID error:(NSError *)error
{
    _delegate->rewardedVideoAdDidFailToPlayForAdUnitID([adUnitID UTF8String], [error.localizedDescription UTF8String], (int)error.code);
}

- (void)rewardedVideoAdWillAppearForAdUnitID:(NSString *)adUnitID
{
    _delegate->rewardedVideoAdWillAppearForAdUnitID([adUnitID UTF8String]);
}

- (void)rewardedVideoAdDidAppearForAdUnitID:(NSString *)adUnitID
{
    _delegate->rewardedVideoAdDidAppearForAdUnitID([adUnitID UTF8String]);
}

- (void)rewardedVideoAdWillDisappearForAdUnitID:(NSString *)adUnitID
{
    _delegate->rewardedVideoAdWillDisappearForAdUnitID([adUnitID UTF8String]);
}

- (void)rewardedVideoAdDidDisappearForAdUnitID:(NSString *)adUnitID
{
    _delegate->rewardedVideoAdDidDisappearForAdUnitID([adUnitID UTF8String]);
}

- (void)rewardedVideoAdDidReceiveTapEventForAdUnitID:(NSString *)adUnitID
{
    _delegate->rewardedVideoAdDidReceiveTapEventForAdUnitID([adUnitID UTF8String]);
}

- (void)rewardedVideoAdWillLeaveApplicationForAdUnitID:(NSString *)adUnitID
{
    _delegate->rewardedVideoAdWillLeaveApplicationForAdUnitID([adUnitID UTF8String]);
}

- (void)rewardedVideoAdShouldRewardForAdUnitID:(NSString *)adUnitID reward:(MPRewardedVideoReward *)reward
{
    _delegate->rewardedVideoAdShouldRewardForAdUnitID([adUnitID UTF8String], avalon::MPRewardedVideoReward{[reward.currencyType UTF8String], [reward.amount intValue]});
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
        [_adController setLocation:[[CLLocation alloc] initWithLatitude:latitude longitude:longitude]];
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
    
class IOSMPRewardedVideo: public MPRewardedVideo
{
public:
    virtual void initializeRewardedVideoWithGlobalMediationSettings(void */*(NSArray *)*/globalMediationSettings, MPRewardedVideoDelegate &delegate) override
    {
        _delegate = [[IOSMPRewardedVideoDelegate alloc] initWithDelegate:&delegate];
        [[MoPub sharedInstance] initializeRewardedVideoWithGlobalMediationSettings:nil delegate:_delegate];
    }

    virtual void loadRewardedVideoAdWithAdUnitID(const std::string &adUnitID, void*/*(NSArray *)*/mediationSettings) override
    {
        [::MPRewardedVideo loadRewardedVideoAdWithAdUnitID:[NSString stringWithUTF8String:adUnitID.c_str()] withMediationSettings:nil];
    }

    virtual void loadRewardedVideoAdWithAdUnitID(const std::string &adUnitID, const std::string &keywords, void*/*(CLLocation *)*/ location, void*/*(NSArray *)*/mediationSettings) override
    {
        [::MPRewardedVideo loadRewardedVideoAdWithAdUnitID:[NSString stringWithUTF8String:adUnitID.c_str()] keywords:[NSString stringWithUTF8String:keywords.c_str()] location:nil mediationSettings:nil];
    }

    virtual void loadRewardedVideoAdWithAdUnitID(const std::string &adUnitID, const std::string &keywords, void*/*(CLLocation *)*/ location, const std::string &customerId, void*/*(NSArray *)*/mediationSettings) override
    {
        [::MPRewardedVideo loadRewardedVideoAdWithAdUnitID:[NSString stringWithUTF8String:adUnitID.c_str()] keywords:[NSString stringWithUTF8String:keywords.c_str()] location:nil customerId:[NSString stringWithUTF8String:customerId.c_str()] mediationSettings:nil];
    }

    virtual bool hasAdAvailableForAdUnitID(const std::string &adUnitID) override
    {
        return [::MPRewardedVideo hasAdAvailableForAdUnitID:[NSString stringWithUTF8String:adUnitID.c_str()]];
    }

    virtual std::vector<MPRewardedVideoReward> availableRewardsForAdUnitID(const std::string &adUnitID) override
    {
        std::vector<MPRewardedVideoReward> ret;
        NSArray *rewards = [::MPRewardedVideo availableRewardsForAdUnitID:[NSString stringWithUTF8String:adUnitID.c_str()]];
        for(::MPRewardedVideoReward *reward in rewards)
            ret.push_back(MPRewardedVideoReward{[reward.currencyType UTF8String], [reward.amount intValue]});
        return ret;
    }

    virtual MPRewardedVideoReward selectedRewardForAdUnitID(const std::string &adUnitID) override
    {
        ::MPRewardedVideoReward *reward = [::MPRewardedVideo selectedRewardForAdUnitID:[NSString stringWithUTF8String:adUnitID.c_str()]];
        if(reward)
            return MPRewardedVideoReward{[reward.currencyType UTF8String], [reward.amount intValue]};
        else
            return MPRewardedVideoReward{"", 0};
    }

    virtual void presentRewardedVideoAdForAdUnitID(const std::string &adUnitID, const MPRewardedVideoReward &reward) override
    {
        NSArray *rewards = [::MPRewardedVideo availableRewardsForAdUnitID:[NSString stringWithUTF8String:adUnitID.c_str()]];
        ::MPRewardedVideoReward *realReward = nil;
        for(::MPRewardedVideoReward *rewardElement in rewards)
        {
            if([rewardElement.currencyType isEqualToString:[NSString stringWithUTF8String:reward.currencyType.c_str()]] && rewardElement.amount.intValue == reward.amount)
                realReward = rewardElement;
        }
        [::MPRewardedVideo presentRewardedVideoAdForAdUnitID:[NSString stringWithUTF8String:adUnitID.c_str()] fromViewController:[UIApplication sharedApplication].keyWindow.rootViewController withReward: realReward];
    }
private:
    IOSMPRewardedVideoDelegate *_delegate = nil;
};

MPRewardedVideo* MPRewardedVideo::getInstance()
{
    static IOSMPRewardedVideo instance;
    return &instance;
}
    
}
