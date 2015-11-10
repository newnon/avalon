#ifndef __EPOM_H__
#define __EPOM_H__

#include <string>
#include <vector>
#include <map>
#include <list>
#include <memory>
#include "Ads.h"

namespace avalon {
    
/*
 Available AdClient SDK banner view sizes
 */
enum class ACBannerViewSizeType
{
    Size320x50 	= 0,
    Size300x50,
    Size300x250,
    Size768x90,
    Size728x90,
    
    Count // for inner usage
};

/*
 AdClient SDK log verbose level
 */
enum class ACVerboseType
{
    All 	= 0,
    ErrorsOnly,
    None,
    
    Count // for inner usage
};
    
class ACUtils
{
public:
    // 	Changing log level of AdClient SDK. Default value is ACVerboseErrorsOnly
    static void setLogLevel(ACVerboseType verboseLevel);

    // 	Set adclient advertisements server url with slash symbol at the end.
    // Must be called before any other access to AdClient SDK
    static void setAdsServerUrl(const std::string &adsServerUrl);
    
    static const std::string &getVersion();
};
    
enum class ACAdErrorCode {
    
    // Network error occurs while ad loading
    NetworkError = 0,
    
    // Server does not have any ads to display
    NoAdsAvailable,
    
    // Ad request is timed out
    TimedOut,
    
    // Recieved ad content can't be parser and/or displayed
    CantReadWebContent,
    
    // Count of error codes
    CodesCount
};
    
// Class for retrieving and visualising interstitial (full-screen) advertisements
class ACInterstitial: public Interstitial
{
public:
    
    // create ACInterstitial using string identifier of interstitial, optional user location use
    // and test mode enabled. After initialization, view will try to load interstitial ad
    static ACInterstitial* create(const std::string &adUnitID, bool useLocation, bool testMode, InterstitialDelegate *delegate);

    // custom parameters for ad requests
    virtual void setCustomParameters(const std::map<std::string,std::string> &parameters) = 0;
    virtual const std::map<std::string,std::string>& getCustomParameters() const = 0;

    // prefer external browser instead of in-app while opening landing pages, if possible. Default value is "NO"
    virtual void setPreferExternalBrowser(bool value) = 0;
    virtual bool getPreferExternalBrowser() const = 0;

    // access interstitial load timeout. 0 means no timeout load interruption, minimum is 2 seconds. Default is 0.
    virtual void setLoadTimeout(float timeout) = 0;
    virtual float getLoadTimeout() const = 0;

    // initializes ACInterstitialView using string identifier of interstitial, optional user location use
    // and test mode enabled. After initialization, view will try to load interstitial ad
    //virtual bool initWithID(const std::string &interstitialID, bool useLocation, bool testMode) = 0;

    virtual void reload() = 0;
};
    
class ACBanner: public Banner
{
public:
    // Initializes a ACBanner with specified id, size type
    // optional force determining of user location and test mode enabled
    static ACBanner* create(const std::string &adUnitID, ACBannerViewSizeType size, bool useLocation, bool testMode, BannerDelegate *delegate);
    
    // custom parameters for ad requests
    virtual void setCustomParameters(const std::map<std::string,std::string> &parameters) = 0;
    virtual const std::map<std::string,std::string>& getCustomParameters() const = 0;
    
    // prefer external browser instead of in-app while opening landing pages, if possible. Default value is "NO"
    virtual void setPreferExternalBrowser(bool value) = 0;
    virtual bool getPreferExternalBrowser() const = 0;
    
    // refresh time interval accessor. Default value is 15 seconds. Minimal - 5 seconds.
    virtual void setRefreshTimeInterval(float timeInterval) = 0;
    virtual float getRefreshTimeInterval() const = 0;
};
    
}

#endif /* __EPOM_H__ */
