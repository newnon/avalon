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
    virtual bool isReady() const = 0;
    virtual void show() = 0;
    virtual ~GADInterstitial() {}
    
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
    
class GADInterstitialDelegate
{
public:
    virtual void interstitialDidReceiveAd(GADInterstitial *view) {}
    virtual void interstitialDidFailToReceiveAd(GADInterstitial *view, GADErrorCode error) {}
    virtual void interstitialWillPresentScreen(GADInterstitial *adView) {}
    virtual void interstitialWillDismissScreen(GADInterstitial *adView) {}
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
    SmartBannerLandscape,
    CustomSize
};
    
enum class BannerScaleType
{
    None,
    Proportional,
    Fill
    
};
    
enum class BannerGravityType
{
    TopLeft,
    CenterLeft,
    BottomLeft,
    TopCenter,
    Center,
    BottomCenter,
    TopRight,
    CenterRight,
    BottomRight
};

class GADBannerView
{
public:
    const std::string &getAdUnitID() const { return _adUnitID; }
    GADAdSize getAdSize() const { return _adSize; }
    virtual void setVisible(bool value)  = 0;
    virtual bool isVisible() = 0;
    virtual ~GADBannerView() {}
    
protected:
    GADBannerView(const std::string &adUnitID, GADAdSize size):_adUnitID(adUnitID), _adSize(size) {}
    GADAdSize _adSize;
    std::string _adUnitID;
};
    
class GADBannerViewDelegate
{
public:
    virtual void adViewDidReceiveAd(GADBannerView *view) {}
    virtual void adViewDidFailToReceive(GADBannerView *view, GADErrorCode error) {}
    virtual void adViewWillPresentScreen(GADBannerView *adView) {}
    virtual void adViewWillDismissScreen(GADBannerView *adView) {}
    virtual void adViewWillLeaveApplication(GADBannerView *adView) {}
    virtual ~GADBannerViewDelegate() {}
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
    
    virtual std::shared_ptr<GADInterstitial> createIntestitial(const std::string &adUnitID, GADInterstitialDelegate *delegate) = 0;
    virtual std::shared_ptr<GADBannerView> createBannerView(const std::string &adUnitID, GADAdSize size, int x, int y, int width, int height, BannerScaleType scaleType, BannerGravityType gravity, GADBannerViewDelegate *delegate) = 0;
    
    virtual std::vector<std::shared_ptr<GADInterstitial>> getReadyInterstitials() const = 0;
    virtual std::vector<std::shared_ptr<GADBannerView>> getBannerViews() const = 0;
    
    virtual void removeBanner(const GADBannerView *bannerView) = 0;
    virtual void removeInterstitial(const GADInterstitial *interstitial) = 0;
    
protected:
    AdMob(const std::string &version):_sdkVersion(version) {}
    virtual ~AdMob() {}
    
private:
    std::string _sdkVersion;
};
    
}

#endif /* __ADMOB_H__ */
