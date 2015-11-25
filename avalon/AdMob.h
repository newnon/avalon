#ifndef __ADMOB_H__
#define __ADMOB_H__

#include <string>
#include <vector>
#include <map>
#include <list>
#include <memory>
#include "Ads.h"

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

class GADInterstitial: public Interstitial
{
public:
    const std::string &getAdUnitID() const { return _adUnitID; }
    virtual const std::string &getType() const override { static std::string type = "admob"; return type; };
    
protected:
    GADInterstitial(const std::string &adUnitID):_adUnitID(adUnitID) {}
    std::string _adUnitID;
};
    
enum class GADErrorCode
{
    /// Something happened internally; for instance, an invalid response was received from the ad server.
    INTERNAL_ERROR = 0,
    
    /// The ad request is invalid.  The localizedFailureReason error description will have more
    /// details.  Typically this is because the ad did not have the ad unit ID or root view
    /// controller set.
    INVALID_REQUEST,
    
    /// The ad request was successful, but no ad was returned.
    NO_FILL,
    
    /// There was an error loading data from the network.
    NETWORK_ERROR,
    
    /// The ad server experienced a failure processing the request.
    SERVER_ERROR,
    
    /// The current device's OS is below the minimum required version.
    OS_VERSION_TOO_LOW,
    
    /// The request was unable to be loaded before being timed out.
    TIMEOUT,
    
    /// Will not send request because the interstitial object has already been used.
    INTERSTITIAL_ALREADY_USED,
    
    /// The mediation response was invalid.
    MEDIATION_DATA_ERROR,
    
    /// Error finding or creating a mediation ad network adapter.
    MEDIATION_ADAPTER_ERROR,
    
    /// The mediation request was successful, but no ad was returned from any ad networks.
    MEDIATION_NO_FILL,
    
    /// Attempting to pass an invalid ad size to an adapter.
    MEDIATION_INVALID_ADSIZE,
    
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
    SmartBannerLandscape,
};
    
GADAdSize makeCustomGADAdSize(unsigned short width, unsigned short height);

class GADBannerView: public Banner
{
public:
    const std::string &getAdUnitID() const { return _adUnitID; }
    GADAdSize getAdSize() const { return _adSize; }
    virtual const std::string &getType() const override { static std::string type = "facebook"; return type; };
    
protected:
    GADBannerView(const std::string &adUnitID, GADAdSize size):_adUnitID(adUnitID), _adSize(size) {}
    GADAdSize _adSize;
    std::string _adUnitID;
};

class AdMob
{
public:
    
    static AdMob *getInstance();

    const std::string &getSdkVersion() const { return _sdkVersion; }
    
    virtual void setAdNetworkExtras(GADAdNetworkExtras network, const std::map<std::string,std::string> &extras) = 0;
    virtual void setTestDevices(const std::vector<std::string>& devices) = 0;
    virtual void setGender(GADGender gender) = 0;
    virtual void setBirthDate(unsigned month, unsigned day, unsigned year) = 0;
    virtual void setLocation(float latitude, float longitude, float accuracyInMeters) = 0;
    virtual void setLocation(const std::string &location) = 0;
    virtual void setTagForChildDirectedTreatment(bool value) = 0;
    virtual void setKeywords(const std::vector<std::string>& keywords) = 0;
    
    virtual GADInterstitial* createIntestitial(const std::string &adUnitID, InterstitialDelegate *delegate) = 0;
    virtual GADBannerView* createBanner(const std::string &adUnitID, GADAdSize size, BannerDelegate *delegate) = 0;
    
protected:
    AdMob(const std::string &version):_sdkVersion(version) {}
    virtual ~AdMob() {}
    
private:
    std::string _sdkVersion;
};
    
}

#endif /* __ADMOB_H__ */
