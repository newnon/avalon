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

void BannerManager::bannerDidLoadAd(Banner *banner)
{
    if(_delegate)
        _delegate->bannerDidLoadAd(banner);
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

void BannerManager::bannerUserInteraction(Banner *banner)
{
    if(_delegate)
        _delegate->bannerUserInteraction(banner);
}

void BannerManager::bannerDidFailLoadAd(Banner *banner, AdsErrorCode error, int nativeCode, const std::string &message)
{
    if(_delegate)
        _delegate->bannerDidFailLoadAd(banner, error, nativeCode, message);
}
    
void InterstitialManager::add(Interstitial* interstitial, float delay)
{
    if(interstitial)
        _interstitials.push_back(std::make_pair(interstitial, delay));
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
        if((_minFrequency == 0)||((_interstitialCounter % _minFrequency == 0)&&(ignoreTimer || deltaSec>=_minDelay)))
            show = true;
    }
    
    ++_interstitialCounter;

    for(const auto &it:_interstitials)
    {
        if(it.first->getState() == Interstitial::State::READY)
        {
            if(show && (_lastInterstitial != it.first || deltaSec>=it.second))
            {
                _prevShowTime = std::chrono::steady_clock::now();
                _lastInterstitial = it.first;
                it.first->show();
                return it.first;
            }
        }
    }
    return nullptr;
}
    
const Interstitial* InterstitialManager::getReadyInterstitial(bool ignoreCounter, bool ignoreTimer) const
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
        if((_minFrequency == 0)||((_interstitialCounter % _minFrequency == 0)&&(ignoreTimer || deltaSec>=_minDelay)))
            show = true;
    }
    
    for(const auto &it:_interstitials)
    {
        if(it.first->getState() == Interstitial::State::READY)
        {
            if(show && (_lastInterstitial != it.first || deltaSec>=it.second))
            {
                return it.first;
            }
        }
    }
    return nullptr;
}
    
void InterstitialManager::clear()
{
    for(const auto &it:_interstitials)
    {
        delete it.first;
    }
    _interstitials.clear();
}

InterstitialManager::InterstitialManager(InterstitialDelegate *delegate)
    :_delegate(delegate)
    ,_minFrequency(0)
    ,_minDelay(0)
    ,_lastInterstitial(nullptr)
    ,_interstitialCounter(0)
    ,_prevShowTime(std::chrono::steady_clock::now())
{
}
    
InterstitialManager::~InterstitialManager()
{
    for(const auto &it:_interstitials)
    {
        delete it.first;
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
    
void InterstitialManager::interstitialDidLoadAd(Interstitial *interstitial)
{
    if(_delegate)
        _delegate->interstitialDidLoadAd(interstitial);
}
    
void InterstitialManager::interstitialDidFailLoadAd(Interstitial *interstitial, AdsErrorCode error, int nativeCode, const std::string &message)
{
    if(_delegate)
        _delegate->interstitialDidFailLoadAd(interstitial, error, nativeCode, message);
}
    
void InterstitialManager::interstitialUserInteraction(Interstitial *interstitial, bool willLeaveApplication)
{
    if(_delegate)
        _delegate->interstitialUserInteraction(interstitial, willLeaveApplication);;
}

}
#endif /* AVALON_CONFIG_ADS_ENABLED */
