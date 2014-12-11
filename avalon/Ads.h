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
    virtual void bannerReceiveAd(Banner *banner) {}
    virtual void bannerClick(Banner *banner) {}
    virtual void bannerFailedToReceiveAd(Banner *banner, AdsErrorCode error, int nativeCode, const std::string &message) {}
    virtual ~BannerDelegate() {}
};

class Banner
{
public:
    virtual bool isReady() const = 0;
    virtual bool isVisible() const = 0;
    virtual bool show(int x, int y, int width, int height, BannerScaleType scaleType, BannerGravityType gravity) = 0;
    virtual bool show(BannerScaleType scaleType, BannerGravityType gravity) = 0;
    virtual bool hide() = 0;
    virtual ~Banner() {}
};

class Interstitial;

class InterstitialDelegate
{
public:
    virtual void interstitialReceiveAd(Interstitial *interstitial) {}
    virtual void interstitialFailedToReceiveAd(Interstitial *interstitial, AdsErrorCode error, int nativeCode, const std::string &message) {}
    virtual void interstitialClose(Interstitial *interstitial) {}
    virtual void interstitialClick(Interstitial *interstitial) {}
    virtual ~InterstitialDelegate() {}
};

class Interstitial
{
public:
    /*!
     * @return
     * YES if an ad is loaded, NO otherwise. This property should always be checked
     * before the interstitial ad is presented.
     */
    virtual bool isReady() const = 0;
    virtual bool isVisible() const = 0;
    virtual bool hide() = 0;
    virtual bool show() = 0;
    virtual ~Interstitial() {}
};

}

#endif /* __ADS_H__ */
