#ifndef __ADMOB_H__
#define __ADMOB_H__

#include <string>
#include <vector>
#include <map>
#include <list>
#include <memory>

namespace avalon {

enum class GADGender
{
    Unknown,
    Male,
    Female
};

enum class GADAdNetworkExtras
{
    AdMob
};

class GADInterstitial
{
public:
    const std::string &getAdUnitID() const { return _adUnitID; }
    const std::string &getAdNetworkClassName() const { return _adNetworkClassName; }
    virtual bool isReady() const = 0;
    virtual bool hasBeenUsed() const = 0;
    virtual void show() = 0;
    virtual ~GADInterstitial() {}
    
protected:
    GADInterstitial(const std::string &adUnitID):_adUnitID(adUnitID) {}
    std::string _adUnitID;
    std::string _adNetworkClassName;
};
    
class GADInterstitialDelegate
{
public:
    virtual void interstitialDidReceiveAd(GADInterstitial *view) {}
    virtual void interstitialDidFailToReceiveAd(GADInterstitial *view, const std::string &error) {}
    virtual void interstitialWillPresentScreen(GADInterstitial *adView) {}
    virtual void interstitialWillDismissScreen(GADInterstitial *adView) {}
    virtual void interstitialDidDismissScreen(GADInterstitial *adView) {}
    virtual void interstitialWillLeaveApplication(GADInterstitial *adView) {}
    virtual ~GADInterstitialDelegate() {}
};

enum class GADAdSize
{
    Invalid,
    Banner,
    MediumRectangle,
    FullBanner,
    Leaderboard,
    Skyscraper,
    SmartBannerPortrait,
    SmartBannerLandscape
};

class GADBannerView
{
public:
    const std::string &getAdUnitID() const { return _adUnitID; }
    const std::string &getAdNetworkClassName() const { return _adNetworkClassName; }
    GADAdSize getAdSize() const { return _adSize; }
    virtual bool hasAutoRefreshed() const = 0;
    virtual ~GADBannerView() {}
    
protected:
    GADBannerView(const std::string &adUnitID, GADAdSize size):_adUnitID(adUnitID), _adSize(size) {}
    GADAdSize _adSize;
    std::string _adUnitID;
    std::string _adNetworkClassName;
};
    
class GADBannerViewDelegate
{
public:
    virtual void adViewDidReceiveAd(GADBannerView *view) {}
    virtual void adViewDidFailToReceive(GADBannerView *view, const std::string &error) {}
    virtual void adViewWillPresentScreen(GADBannerView *adView) {}
    virtual void adViewWillDismissScreen(GADBannerView *adView) {}
    virtual void adViewDidDismissScreen(GADBannerView *adView) {}
    virtual void adViewWillLeaveApplication(GADBannerView *adView) {}
};

class AdMob
{
public:
    
    static AdMob *getInstance();

    std::string &getSdkVersion() { return _sdkVersion; }
    
    void setAdNetworkExtras(GADAdNetworkExtras network, const std::map<std::string,std::string> &extras)
    {
        _extras[network] = extras;
    }
    void setTestDevices(const std::vector<std::string>& devices)
    {
        _testDevices = devices;
    }
    void setGender(GADGender gender)
    {
        _gender = gender;
    }
    void setBirthDate(unsigned month, unsigned day, unsigned year)
    {
        _birthDate.defined = true;
        _birthDate.month = month;
        _birthDate.day = day;
        _birthDate.year = year;
    }
    void setLocation(float latitude, float longitude, float accuracyInMeters)
    {
        _location.defined = true;
        _location.latitude = latitude;
        _location.longitude = longitude;
        _location.accuracyInMeters = accuracyInMeters;
    }
    void setLocation(const std::string &location)
    {
        _locationStr = location;
    }
    void setTagForChildDirectedTreatment(bool value)
    {
        _tagForChildDirectedTreatment.first = true;
        _tagForChildDirectedTreatment.second = value;
    }
    void setKeywords(const std::vector<std::string>& keywords)
    {
        _keywords = keywords;
    }
    
    const std::shared_ptr<GADInterstitial>& createIntestitial(const std::string &adUnitID, GADInterstitialDelegate *delegate);
    const std::shared_ptr<GADBannerView>& createBannerView(const std::string &adUnitID, GADAdSize size, int x, int y, int width, int height, GADBannerViewDelegate *delegate);
    
    unsigned getReadyInterstitialCount() const
    {
        unsigned ret = 0;
        for(auto &it : _interstitials)
        {
            if(it && it->isReady() && !it->hasBeenUsed())
                ++ret;
                
        }
        return ret;
    }
    GADInterstitial *getReadyInterstitial() const
    {
        for(auto &it : _interstitials)
        {
            if(it && it->isReady() && !it->hasBeenUsed())
                return it.get();
            
        }
        return nullptr;
    }
    
    const std::list<std::shared_ptr<GADBannerView>> &getBannerViews() const
    {
        return _bannerViews;
    }
    
    
private:
    friend class AdMobHelper;
    struct BirthDate
    {
        BirthDate():defined(false) {}
        bool defined;
        unsigned month;
        unsigned day;
        unsigned year;
    };
    struct Location
    {
        Location():defined(false) {}
        bool defined;
        unsigned latitude;
        unsigned longitude;
        unsigned accuracyInMeters;
    };
    AdMob(const std::string &version):_sdkVersion(version),_gender(GADGender::Unknown),_tagForChildDirectedTreatment(false,false) {}
    std::string _sdkVersion;
    std::map<GADAdNetworkExtras, std::map<std::string,std::string>> _extras;
    std::vector<std::string> _testDevices;
    GADGender _gender;
    BirthDate _birthDate;
    Location _location;
    std::string _locationStr;
    std::pair<bool,bool> _tagForChildDirectedTreatment;
    std::vector<std::string> _keywords;
    std::list<std::shared_ptr<GADBannerView>> _bannerViews;
    std::list<std::shared_ptr<GADInterstitial>> _interstitials;
};
    
}

#endif /* __ADMOB_H__ */
