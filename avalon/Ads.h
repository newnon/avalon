#ifndef __ADS_H__
#define __ADS_H__

namespace avalon {

enum class BannerScaleType
{
    None = 0,
    Proportional,
    Fill
};
    
enum class BannerGravityType
{
    TopLeft = 0,
    CenterLeft,
    BottomLeft,
    TopCenter,
    Center,
    BottomCenter,
    TopRight,
    CenterRight,
    BottomRight
};

enum class AdsErrorCode
{
    INTERNAL_ERROR = 0,
    INVALID_REQUEST,
    NO_FILL,
    NETWORK_ERROR,
    SERVER_ERROR,
    OS_VERSION_TOO_LOW,
    TIMEOUT,
    TO_FREQUENTLY,
    MEDIATION_ERROR
};

class Banner;

class BannerDelegate
{
public:
    
    // Notification: Banner ad started loading.
    virtual void bannerDidStartLoadAd(Banner *banner) {}
    
    // Notification: Banner ad failed to load.
    virtual void bannerDidFailLoadAd(Banner *banner, AdsErrorCode error, int nativeCode, const std::string &message) {}
    
    // Notification: Banner ad successfully loaded.
    virtual void bannerDidLoadAd(Banner *banner) {}
    
    // Notification: Ad provided by Banner will be shown
    virtual void bannerWillShowAd(Banner *banner) {}
    
    // Notification: Ad provided by Banner is shown
    virtual void bannerDidShowAd(Banner *banner) {};
    
    // Notification: Ad provided by Banner has been clicked
    virtual void bannerUserInteraction(Banner *banner) {}
    
    // Notification: Ad provided by Banner will enter modal mode when opening embedded screen view controller
    virtual void bannerWillEnterModalMode(Banner *banner) {}
    
    // Notification: Ad provided by Banner did leave modal mode
    virtual void bannerDidLeaveModalMode(Banner *banner) {}
    
    // Notification: Ad provided by Banner causes to leave application to navigate to Safari, iTunes, etc.
    virtual void bannerWillLeaveApplication(Banner *banner) {}

    virtual ~BannerDelegate() {}
};

class Banner
{
public:
    virtual bool isReady() const = 0;
    virtual bool isVisible() const = 0;
    virtual const std::string &getType() const = 0;
    virtual bool show(int x, int y, int width, int height, BannerScaleType scaleType, BannerGravityType gravity) = 0;
    virtual bool show(BannerScaleType scaleType, BannerGravityType gravity) = 0;
    virtual bool hide() = 0;
    virtual ~Banner() {}
};

class Interstitial;

class InterstitialDelegate
{
public:
    // Notification: Interstitial ad started loading. State is changed to Loading.
    virtual void interstitialDidStartLoadAd(Interstitial *interstitial) {}
    
    // Notification: Interstitial ad failed to load. State is changed to Failed.
    // Error code/type is output to debug console
    virtual void interstitialDidFailLoadAd(Interstitial *interstitial, AdsErrorCode error, int nativeCode, const std::string &message) {}
    
    // Notification: Interstitial ad successfully loaded. State is changed to Ready.
    virtual void interstitialDidLoadAd(Interstitial *interstitial) {}
    
    // Notification: Interstitial ad starts to display ad. State is changed to Active.
    virtual void interstitialWillShow(Interstitial *interstitial) {}
    
    // Notification: Interstitial ad finished to display ad. State is changed to Done.
    virtual void interstitialDidHide(Interstitial *interstitial) {}
    
    // Notification: User has interacted with ad provided by ACInterstitialView. Optional is application leaving to navigate to Safari, iTunes, etc
    virtual void interstitialUserInteraction(Interstitial *interstitial, bool willLeaveApplication) {}

    virtual ~InterstitialDelegate() {}
};

class Interstitial
{
public:
    
    enum class State
    {
        INITIALIZING,
        LOADING,
        FAILED,
        READY,
        ACTIVE,
        DONE
    };
    
    virtual State getState() const = 0;
    virtual const std::string &getType() const = 0;
    virtual int getReward() const { return 0; };
    virtual bool hide() = 0;
    virtual bool show() = 0;
    virtual ~Interstitial() {}
};

}

#endif /* __ADS_H__ */
