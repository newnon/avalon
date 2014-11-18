#ifndef __ADGENERATION_H__
#define __ADGENERATION_H__

#include <string>
#include <vector>
#include <map>
#include <list>
#include <memory>
#include "Ads.h"

namespace avalon {

enum class AdGenerationSize
{
    SP,
	LARGE,
	RECT,
    TABLET
};
    
AdGenerationSize makeCustomAdGenerationAdSize(unsigned short width, unsigned short height);

class AdGenerationBannerView
{
public:
    static AdGenerationBannerView* create(const std::string &adUnitID, AdGenerationSize size);
    const std::string &getAdUnitID() const { return _adUnitID; }
    AdGenerationSize getAdSize() const { return _adSize; }
    virtual bool isVisible() = 0;
    virtual void show(int x, int y, int width, int height, BannerScaleType scaleType, BannerGravityType gravity) = 0;
    virtual void hide() = 0;
    virtual ~AdGenerationBannerView() {}
    
protected:
    AdGenerationBannerView(const std::string &adUnitID, AdGenerationSize size):_adUnitID(adUnitID), _adSize(size) {}
    AdGenerationSize _adSize;
    std::string _adUnitID;
};
    
class AdGenerationViewDelegate
{
public:
    virtual void adViewDidReceiveAd(AdGenerationBannerView *view) {}
    virtual void adViewDidFailToReceive(AdGenerationBannerView *view) {}
    virtual void adViewWillPresentScreen(AdGenerationBannerView *adView) {}
    virtual void adViewWillDismissScreen(AdGenerationBannerView *adView) {}
    virtual void adViewWillLeaveApplication(AdGenerationBannerView *adView) {}
    virtual ~AdGenerationViewDelegate() {}
};
    
}

#endif /* __ADGENERATION_H__ */
