#ifndef __ADS_H__
#define __ADS_H__

namespace avalon {

enum class BannerScaleType
{
    None,
    Proportional,
    Fill
};
    
enum class BannerGravityType
{
    TopLeft,
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
    TIMEOUT
};

class Banner;

class BannerDelegate
{
public:
    virtual void bannerReceiveAd(Banner *banner) {}
    virtual void bannerFailedToReceiveAd(Banner *banner, AdsErrorCode error) {}
    virtual void bannerWillLeaveApplication(Banner *banner) {}
    virtual ~BannerDelegate() {}
};

class Banner
{
public:
    virtual bool isReady() const = 0;
    virtual bool isVisible() const = 0;
    virtual void show(int x, int y, int width, int height, BannerScaleType scaleType, BannerGravityType gravity) = 0;
    virtual void hide() = 0;
    virtual ~Banner() {}
};

class Interstitial;

class InterstitialDelegate
{
public:
    virtual void interstitialReceiveAd(Interstitial *interstitial) {}
    virtual void interstitialFailedToReceiveAd(Interstitial *interstitial, AdsErrorCode error) {}
    virtual void interstitialWillPresentScreen(Interstitial *interstitial) {}
    virtual void interstitialWillDismissScreen(Interstitial *interstitial) {}
    virtual void interstitialWillLeaveApplication(Interstitial *interstitial) {}
    virtual ~InterstitialDelegate() {}
};

class Interstitial
{
public:
    virtual bool isReady() const = 0;
    virtual void show() = 0;
    virtual ~Interstitial() {}
};

}

#endif /* __ADS_H__ */
