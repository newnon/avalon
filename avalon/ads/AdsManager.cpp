#ifdef AVALON_CONFIG_ADS_ENABLED
#include "avalon/AdsManager.h"

namespace avalon {

bool BannerManager::show(int x, int y, int width, int height, BannerScaleType scaleType, BannerGravityType gravity)
{
    if(_visible)
        return false;
    
    for(const auto &banner:_banners)
    {
        if(banner && banner->isReady())
        {
            banner->show(x, y, width, height, scaleType, gravity);
            _visible = true;
            break;
        }
    }
    return _visible;
}

bool BannerManager::hide()
{
    if(!_visible)
        return false;
    
    _visible = false;
    
    bool ret = false;
    
    for(const auto &banner:_banners)
    {
        if(banner && banner->isVisible())
        {
            banner->hide();
            ret = true;
        }
    }
    return ret;
}

bool BannerManager::isReady() const
{
    for(const auto &banner:_banners)
    {
        if(banner && banner->isReady())
        {
            return true;
        }
    }
    return false;
}

BannerManager::BannerManager(const std::vector<Banner*> &banners):_banners(banners),_visible(false)
{
}

BannerManager::~BannerManager()
{
    for(const auto &banner:_banners)
    {
        if(banner)
            delete banner;
    }
}

bool InterstitialManager::show()
{
    for(const auto &interstitial:_interstitials)
    {
        if(interstitial && interstitial->isReady())
        {
            interstitial->show();
            return true;
        }
    }
    return false;
}

bool InterstitialManager::isReady() const
{
    for(const auto &interstitial:_interstitials)
    {
        if(interstitial && interstitial->isReady())
        {
            return true;
        }
    }
    return false;
}

InterstitialManager::InterstitialManager(const std::vector<Interstitial*> &interstitials):_interstitials(interstitials)
{
}
    
InterstitialManager::~InterstitialManager()
{
    for(const auto &interstitial:_interstitials)
    {
        if(interstitial)
            delete interstitial;
    }
}

void AdsManager::addBanner(const std::function<Banner*()> &createFuntion)
{
    _bannersCreateFunctions.push_back(createFuntion);
}

void AdsManager::addInterstitial(const std::function<Interstitial*()> &createFuntion)
{
    _interstitialsCreateFunctions.push_back(createFuntion);
}
	
std::shared_ptr<BannerManager> AdsManager::createBanner(BannerDelegate *delegate) const
{
    std::vector<Banner*> banners;
    for(const auto &it:_bannersCreateFunctions)
    {
        banners.push_back(it());
    }
    return std::shared_ptr<BannerManager>(new BannerManager(banners));
}

std::shared_ptr<InterstitialManager> AdsManager::createInterstitial(InterstitialDelegate *delegate) const
{
    std::vector<Interstitial*> interstitials;
    for(const auto &it:_interstitialsCreateFunctions)
    {
        interstitials.push_back(it());
    }
    return std::shared_ptr<InterstitialManager>(new InterstitialManager(interstitials));
}

AdsManager::AdsManager()
{
}

AdsManager::~AdsManager()
{
}

}
#endif /* AVALON_CONFIG_ADS_ENABLED */
