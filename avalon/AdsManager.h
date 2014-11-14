#ifndef __ADSMANAGER_H__
#define __ADSMANAGER_H__

#include <functional>
#include <vector>
#include <memory>
#include "Ads.h"

namespace avalon {

class BannerManager
{
friend class AdsManager;
public:
    bool show(int x, int y, int width, int height, BannerScaleType scaleType, BannerGravityType gravity);
    bool hide();
    bool isVisible() const;
    bool isReady() const;
    ~BannerManager();

private:
    BannerManager(const std::vector<Banner*> &banners);
    std::vector<Banner*> _banners;
    bool _visible;
};

class InterstitialManager
{
friend class AdsManager;
public:
    bool show();
    bool isReady() const;
    ~InterstitialManager();

private:
    InterstitialManager(const std::vector<Interstitial*> &interstitials);
    std::vector<Interstitial*> _interstitials;
};

class AdsManager
{
public:
	void addBanner(const std::function<Banner*()> &createFuntion);
	void addInterstitial(const std::function<Interstitial*()> &createFuntion);
	
	std::shared_ptr<BannerManager> createBanner(BannerDelegate *delegate) const;
	std::shared_ptr<InterstitialManager> createInterstitial(InterstitialDelegate *delegate) const;

	AdsManager();
	~AdsManager();
    
private:
    std::vector<const std::function<Banner*()>> _bannersCreateFunctions;
    std::vector<const std::function<Interstitial*()>> _interstitialsCreateFunctions;
};

}

#endif /* __ADSMANAGER_H__ */
