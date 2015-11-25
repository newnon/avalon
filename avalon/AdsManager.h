#ifndef __ADSMANAGER_H__
#define __ADSMANAGER_H__

#include <functional>
#include <vector>
#include <memory>
#include <chrono>
#include "Ads.h"

namespace avalon {
    
class BannerManager: public BannerDelegate
{
public:
    void add(Banner* banner);
    
    const Banner* show();
    const Banner* hide();
    void clear();
    
    BannerManager(BannerDelegate *delegate);
    ~BannerManager();
    
    void setBannerParams(int x, int y, int width, int height, BannerScaleType scaleType, BannerGravityType gravity);
    
private:
    virtual void bannerDidLoadAd(Banner *banner) override;
    virtual void bannerUserInteraction(Banner *banner) override;
    virtual void bannerDidFailLoadAd(Banner *banner, AdsErrorCode error, int nativeCode, const std::string &message) override;
    std::vector<Banner*> _banners;
    bool _needToShowBanner;
    int _x;
    int _y;
    int _width;
    int _height;
    BannerScaleType _scaleType;
    BannerGravityType _gravity;
    Banner* _visibleBanner;
    BannerDelegate *_delegate;
};

class InterstitialManager: public InterstitialDelegate
{
public:
    void add(Interstitial* interstitial, float delay);
    
    const Interstitial* show(bool ignoreCounter, bool ignoreTimer);
    const Interstitial* getReadyInterstitial(bool ignoreCounter, bool ignoreTimer) const;
    void clear();
    
    InterstitialManager(InterstitialDelegate *delegate);
    ~InterstitialManager();
    
    void setMinFrequency(int minFrequency);
    int getMinFrequency() const;
    void setMinDelay(float minDelay);
    float getMinDelay() const;
    
private:
    virtual void interstitialDidLoadAd(Interstitial *interstitial) override;
    virtual void interstitialDidFailLoadAd(Interstitial *interstitial, AdsErrorCode error, int nativeCode, const std::string &message) override;
    virtual void interstitialUserInteraction(Interstitial *interstitial, bool willLeaveApplication) override;
    std::vector<std::pair<Interstitial*,float>> _interstitials;
    InterstitialDelegate *_delegate;
    std::chrono::steady_clock::time_point _prevShowTime;
    Interstitial* _lastInterstitial;
    int _interstitialCounter;
    int _minFrequency;
    float _minDelay;
};

}

#endif /* __ADSMANAGER_H__ */
