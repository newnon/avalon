#ifdef AVALON_CONFIG_ADS_ENABLED
#include "avalon/AdsManager.h"
#include <limits.h>

namespace avalon {

void BannerManager::add(Banner* banner)
{
    if(banner)
        _banners.push_back(banner);
}

const Banner* BannerManager::show()
{
    _needToShowBanner = true;
    for(const auto &it:_banners)
    {
        if(it->isReady())
        {
            if(_needToShowBanner)
            {
                _needToShowBanner = false;
                _visibleBanner = it;
                if(_width != 0 && _height != 0)
                    it->show(_x, _y, _width, _height, _scaleType, _gravity);
                else
                    it->show(_scaleType, _gravity);
                return it;
            }
        }
    }
    return nullptr;
}

const Banner* BannerManager::hide()
{
    _needToShowBanner = false;
    if(_visibleBanner)
    {
        _visibleBanner->hide();
        _visibleBanner = nullptr;
        return _visibleBanner;
    }
    return nullptr;
}
    
void BannerManager::clear()
{
    for(const auto &it:_banners)
    {
        delete it;
    }
}

BannerManager::BannerManager(BannerDelegate *delegate)
    :_needToShowBanner(false)
    ,_x(0)
    ,_y(0)
    ,_width(0)
    ,_height(0)
    ,_scaleType(BannerScaleType::None)
    ,_gravity(BannerGravityType::BottomCenter)
    ,_visibleBanner(nullptr)
    ,_delegate(delegate)
{

}
    
void BannerManager::setBannerParams(int x, int y, int width, int height, BannerScaleType scaleType, BannerGravityType gravity)
{
    _x = x;
    _y = y;
    _width = width;
    _height = height;
    _scaleType = scaleType;
    _gravity = gravity;
}

BannerManager::~BannerManager()
{
    for(const auto &it:_banners)
    {
        delete it;
    }
}

void BannerManager::bannerReceiveAd(Banner *banner)
{
    if(_delegate)
        _delegate->bannerReceiveAd(banner);
    if(_needToShowBanner)
    {
        _visibleBanner = banner;
        if(_width != 0 && _height != 0)
            banner->show(_x, _y, _width, _height, _scaleType, _gravity);
        else
            banner->show(_scaleType, _gravity);
        _needToShowBanner = false;
    }
}

void BannerManager::bannerClick(Banner *banner)
{
    if(_delegate)
        _delegate->bannerClick(banner);
}

void BannerManager::bannerFailedToReceiveAd(Banner *banner, AdsErrorCode error, int nativeCode, const std::string &message)
{
    if(_delegate)
        _delegate->bannerFailedToReceiveAd(banner, error, nativeCode, message);
}
    
void InterstitialManager::add(Interstitial* interstitial)
{
    if(interstitial)
        _interstitials.push_back(interstitial);
}
    
const Interstitial* InterstitialManager::show(bool ignoreCounter, bool ignoreTimer)
{
    auto deltaSec = _lastInterstitial?std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - _prevShowTime).count():INT_MAX;
    bool show = false;
    if(ignoreCounter)
    {
        if(ignoreTimer || deltaSec>=_minDelay)
            show = true;
    }
    else
    {
        if((_interstitialCounter++ % _minFrequency == 0)&&(ignoreTimer || deltaSec>=_minDelay))
            show = true;
    }

    for(const auto &it:_interstitials)
    {
        if(it->isReady())
        {
            if(show && (_lastInterstitial != it || deltaSec>=_minDelayOnSameNetwork))
            {
                _prevShowTime = std::chrono::steady_clock::now();
                _lastInterstitial = it;
                it->show();
                return it;
            }
        }
    }
    return nullptr;
}
    
void InterstitialManager::clear()
{
    for(const auto &it:_interstitials)
    {
        delete it;
    }
}

InterstitialManager::InterstitialManager(InterstitialDelegate *delegate)
    :_delegate(delegate)
    ,_minFrequency(0)
    ,_minDelay(0)
    ,_minDelayOnSameNetwork(0)
    ,_lastInterstitial(nullptr)
    ,_interstitialCounter(0)
    ,_prevShowTime(std::chrono::steady_clock::now())
{
}
    
InterstitialManager::~InterstitialManager()
{
    for(const auto &it:_interstitials)
    {
        delete it;
    }
}

void InterstitialManager::setMinFrequency(int minFrequency)
{
    _minFrequency = minFrequency;
}
int InterstitialManager::getMinFrequency() const
{
    return _minFrequency;
}

void InterstitialManager::setMinDelay(float minDelay)
{
    _minDelay = minDelay;
}

float InterstitialManager::getMinDelay() const
{
    return _minDelay;
}

void InterstitialManager::setMinDelayOnSameNetwork(float minDelayOnSameNetwork)
{
    _minDelayOnSameNetwork = minDelayOnSameNetwork;
}

float InterstitialManager::getMinDelayOnSameNetwork() const
{
    return _minDelayOnSameNetwork;
}
    
void InterstitialManager::interstitialReceiveAd(Interstitial *interstitial)
{
    if(_delegate)
        _delegate->interstitialReceiveAd(interstitial);
}
    
void InterstitialManager::interstitialFailedToReceiveAd(Interstitial *interstitial, AdsErrorCode error, int nativeCode, const std::string &message)
{
    if(_delegate)
        _delegate->interstitialFailedToReceiveAd(interstitial, error, nativeCode, message);
}
    
void InterstitialManager::interstitialClose(Interstitial *interstitial)
{
    if(_delegate)
        _delegate->interstitialClose(interstitial);;
}
    
void InterstitialManager::interstitialClick(Interstitial *interstitial)
{
    if(_delegate)
        _delegate->interstitialClick(interstitial);
}


}
#endif /* AVALON_CONFIG_ADS_ENABLED */
