#include "avalon/FacebookAds.h"

namespace avalon {
class FBAndroidInterstitial;
class FBAndroidBanner;
    
FBAdSize makeCustomFBAdSize(unsigned short width, unsigned short height)
{
    return static_cast<FBAdSize>(width<<16 | height);
}

class FBAndroidInterstitial:public FBInterstitial
{
public:
    
    FBAndroidInterstitial(const std::string &placementID, avalon::InterstitialDelegate *delegate):FBInterstitial(placementID),_visible(false)
    {
    }
    ~FBAndroidInterstitial()
    {
    }
    
    virtual bool isReady() const override
    {
        return false;
    }
    
    virtual bool isVisible() const override
    {
        return _visible;
    }
    
    virtual bool hide() override
    {
        return false;
    }
    
    virtual bool show() override
    {
        return _visible;
    }
    
    void loadAd()
    {
    }
    
    void setVisible(bool value)
    {
        _visible = value;
    }
    
private:;
    bool _visible;
};

class FBAndroidBanner:public FBBanner
{
public:
    FBAndroidBanner(const std::string &placementID, FBAdSize size, BannerDelegate *delegate):FBBanner(placementID, size),_ready(false)
    {
    }
    
    ~FBAndroidBanner()
    {
    }
    
    virtual bool isReady() const override
    {
        return _ready;
    }
    
    virtual bool isVisible() const override
    {
        return false;
    };
    
    virtual bool show(int x, int y, int width, int height, BannerScaleType scaleType, BannerGravityType gravity) override
    {
        return false;
    }
    
    virtual bool show(BannerScaleType scaleType, BannerGravityType gravity) override
    {
        return false;
    }
    
    virtual bool hide() override
    {
        return false;
    }
    
    void loadAd()
    {
    }
    
    void setReady(bool value)
    {
        _ready = value;
    }
    
private:
    bool _ready;
};

class FBAndroidAds: public FBAds
{
public:
    
    virtual const std::string& getVersion() const override
    {
        return _version;
    }
    
    virtual FBInterstitial* createInterstitial(const std::string &placementID, InterstitialDelegate *delegate) override
    {
        return new FBAndroidInterstitial(placementID, delegate);
    }
    virtual FBBanner* createBanner(const std::string &placementID, FBAdSize adSize, BannerDelegate *delegate) override
    {
        return new FBAndroidBanner(placementID, adSize, delegate);
    }

    virtual void addTestDevice(const std::string &deviceHash) override
    {
    }

    virtual void addTestDevices(const std::vector<std::string> &deviceHash) override
    {
    }

    virtual void clearTestDevices() override
    {
    }

    virtual void setIsChildDirected(bool isChildDirected) override
    {
    }
    
    FBAndroidAds():_version("dummy")
    {
    }
    
    
private:
    std::string _version;
};

FBAds *FBAds::getInstance()
{
    static FBAndroidAds *instance = new FBAndroidAds();
    return instance;
}
    
}
