#ifndef __ADMOB_H__
#define __ADMOB_H__

#include <string>
#include <vector>
#include <map>
#include <list>
#include <memory>
#include "Ads.h"

namespace avalon {

/*!
 @class FBInterstitialAd
 
 @abstract A modal view controller to represent a Facebook interstitial ad. This
 is a full-screen ad shown in your application.
 
 */
class FBInterstitial: public Interstitial
{
public:
    const std::string &getPlacementID() const { return _placementID; }
    
protected:
    FBInterstitial(const std::string &placementID):_placementID(placementID) {}
    std::string _placementID;
};

/*!
 @enum class FBAdSize
 
 @abstract
 Represents the ad size.
 */
enum class FBAdSize
{
    /*!
     @abstract Represents the fixed banner ad size - 320pt by 50pt.
     */
    AdSize320x50,
    
    /*!
     @abstract Represents the flexible banner ad size, where banner width depends on
     its container width, and banner height is fixed as 50pt.
     */
    AdSizeHeight50Banner,
    
    /*!
     @abstract Represents the flexible banner ad size, where banner width depends on
     its container width, and banner height is fixed as 90pt.
     */
    AdSizeHeight90Banner
};
    
FBAdSize makeCustomFBAdSize(unsigned short width, unsigned short height);

/*!
 @class FBAdView
 
 @abstract A customized UIView to represent a Facebook ad (a.k.a. banner ad).
 */
class FBBanner: public Banner
{
public:
    const std::string &getPlacementID() const { return _placementID; }
    FBAdSize getAdSize() const { return _adSize; }
    
protected:
    FBBanner(const std::string &placementID, FBAdSize size):_placementID(placementID), _adSize(size) {}
    FBAdSize _adSize;
    std::string _placementID;
};
    
class FBAds
{
public:
    static FBAds *getInstance();
    
    virtual const std::string& getVersion() const = 0;
    
    /*!
     @method
     
     @abstract
     This is a method to initialize an FBInterstitialAd matching the given placement id.
     
     @param placementID The id of the ad placement. You can create your placement id from Facebook developers page.
     */
    virtual FBInterstitial* createInterstitial(const std::string &placementID, InterstitialDelegate *delegate) = 0;
    
    /*!
     @method
     
     @abstract
     This is a method to initialize an FBAdView matching the given placement id.
     
     @param placementID The id of the ad placement. You can create your placement id from Facebook developers page.
     @param adSize The size of the ad; for example, kFBAdSizeHeight50Banner or kFBAdSizeHeight90Banner.
     @param rootViewController The view controller that will be used to present the ad and the app store view.
     */
    virtual FBBanner* createBanner(const std::string &placementID, FBAdSize adSize, BannerDelegate *delegate) = 0;
    
    /*!
     @method
     
     @abstract
     Adds a test device.
     
     @param deviceHash The id of the device to use test mode, can be obtained from debug log
     
     @discussion
     Copy the current device Id from debug log and add it as a test device to get test ads. Apps
     running on emulator will automatically get test ads. Test devices should be added before loadAd is called.
     */
    virtual void addTestDevice(const std::string &deviceHash) = 0;
    
    /*!
     @method
     
     @abstract
     Add a collection of test devices. See `+addTestDevices:` for details.
     
     @param deviceHash The array of the device id to use test mode, can be obtained from debug log
     */
    virtual void addTestDevices(const std::vector<std::string> &deviceHash) = 0;
    
    /*!
     @method
     
     @abstract
     Clear all the added test devices
     */
    virtual void clearTestDevices() = 0;
    
    /*!
     @method
     
     @abstract
     Configures the ad control for treatment as child-directed.
     
     @param isChildDirected Indicates whether you would like your ad control to be treated as child-directed
     
     @discussion
     Note that you may have other legal obligations under the Children's Online Privacy Protection Act (COPPA).
     Please review the FTC's guidance and consult with your own legal counsel.
     */
    virtual void setIsChildDirected(bool isChildDirected) = 0;
};
}

#endif /* __ADMOB_H__ */
