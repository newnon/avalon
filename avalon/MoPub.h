#ifndef __MOPUB_H__
#define __MOPUB_H__

#include <string>
#include <vector>
#include <map>

namespace avalon {
    
class MPInterstitialAdControllerDelegate;
    
    
class MPInterstitialAdController
{
public:
    /** @name Obtaining an Interstitial Ad */

    /**
     * Returns an interstitial ad object matching the given ad unit ID.
     *
     * The first time this method is called for a given ad unit ID, a new interstitial ad object is
     * created, stored in a shared pool, and returned. Subsequent calls for the same ad unit ID will
     * return that object, unless you have disposed of the object using
     * `removeSharedInterstitialAdController:`.
     *
     * There can only be one interstitial object for an ad unit ID at a given time.
     *
     * @param adUnitId A string representing a MoPub ad unit ID.
     */
    static MPInterstitialAdController *interstitialAdControllerForAdUnitId(const std::string &adUnitId);

    /** @name Setting and Getting the Delegate */

    /**
     * The delegate (`MPInterstitialAdControllerDelegate`) of the interstitial ad object.
     */
    virtual void setDelegate(MPInterstitialAdControllerDelegate *delegate) = 0;
    virtual MPInterstitialAdControllerDelegate* getDelegate() const = 0;

    /** @name Setting Request Parameters */

    /**
     * The MoPub ad unit ID for this interstitial ad.
     *
     * Ad unit IDs are created on the MoPub website. An ad unit is a defined placement in your
     * application set aside for advertising. If no ad unit ID is set, the ad object will use a default
     * ID that only receives test ads.
     */
    virtual const std::string& getAdUnitId() const = 0;

    /**
     * A string representing a set of keywords that should be passed to the MoPub ad server to receive
     * more relevant advertising.
     *
     * Keywords are typically used to target ad campaigns at specific user segments. They should be
     * formatted as comma-separated key-value pairs (e.g. "marital:single,age:24").
     *
     * On the MoPub website, keyword targeting options can be found under the "Advanced Targeting"
     * section when managing campaigns.
     */
    virtual void setLocation(float latitude, float longitude, float accuracyInMeters) = 0;

    /**
     * A `CLLocation` object representing a user's location that should be passed to the MoPub ad server
     * to receive more relevant advertising.
     */
    virtual void setKeywords(const std::string& keywords) = 0;

    /** @name Enabling Test Mode */

    /**
     * A Boolean value that determines whether the interstitial ad object should request ads in test
     * mode.
     *
     * The default value is NO.
     * @warning **Important**: If you set this value to YES, make sure to reset it to NO before
     * submitting your application to the App Store.
     */
    virtual void setTesting(bool testing) = 0;

    /** @name Loading an Interstitial Ad */

    /**
     * Begins loading ad content for the interstitial.
     *
     * You can implement the `interstitialDidLoadAd:` and `interstitialDidFailToLoadAd:` methods of
     * `MPInterstitialAdControllerDelegate` if you would like to be notified as loading succeeds or
     * fails.
     */
    virtual void loadAd() = 0;

    /** @name Detecting Whether the Interstitial Ad Has Loaded */

    /**
     * A Boolean value that represents whether the interstitial ad has loaded an advertisement and is
     * ready to be presented.
     *
     * After obtaining an interstitial ad object, you can use `loadAd` to tell the object to begin
     * loading ad content. Once the content has been loaded, the value of this property will be YES.
     *
     * The value of this property can be NO if the ad content has not finished loading, has already
     * been presented, or has expired. The expiration condition only applies for ads from certain
     * third-party ad networks. See `MPInterstitialAdControllerDelegate` for more details.
     */
    virtual bool getReady() const = 0;

    /** @name Presenting an Interstitial Ad */

    /**
     * Presents the interstitial ad modally from the specified view controller.
     *
     * This method will do nothing if the interstitial ad has not been loaded (i.e. the value of its
     * `ready` property is NO).
     *
     * `MPInterstitialAdControllerDelegate` provides optional methods that you may implement to stay
     * informed about when an interstitial takes over or relinquishes the screen.
     *
     * @param controller The view controller that should be used to present the interstitial ad.
     */
    virtual void show() = 0;

    /** @name Disposing of an Interstitial Ad */

    /**
     * Removes the given interstitial object from the shared pool of interstitials available to your
     * application.
     *
     * This method removes the mapping from the interstitial's ad unit ID to the interstitial ad
     * object. In other words, you will receive a different ad object if you subsequently call
     * `interstitialAdControllerForAdUnitId:` for the same ad unit ID.
     *
     * @warning **Important**: This method is intended to be used for deallocating the interstitial
     * ad object when it is no longer needed. You should `nil` out any references you have to the
     * object after calling this method.
     *
     * @param controller The interstitial ad object that should be disposed.
     */
    virtual ~MPInterstitialAdController() {}

    /*
     * Returns the shared pool of interstitial objects for your application.
     */
    static const std::map<std::string, MPInterstitialAdController*>& sharedInterstitialAdControllers()
    {
        return _sharedInterstitialAdControllers;
    }
    
protected:
    static std::map<std::string, MPInterstitialAdController*> _sharedInterstitialAdControllers;
};

    
class MPInterstitialAdControllerDelegate
{
public:
    
    /** @name Detecting When an Interstitial Ad is Loaded */
    
    /**
    * Sent when an interstitial ad object successfully loads an ad.
    *
    * @param interstitial The interstitial ad object sending the message.
    */
    virtual void interstitialDidLoadAd(MPInterstitialAdController *interstitial) {}
    
    /**
     * Sent when an interstitial ad object fails to load an ad.
     *
     * @param interstitial The interstitial ad object sending the message.
     */
    virtual void interstitialDidFailToLoadAd(MPInterstitialAdController *interstitial) {}
    
    /** @name Detecting When an Interstitial Ad is Presented */
    
    /**
     * Sent after an interstitial ad object has been presented on the screen.
     *
     * @param interstitial The interstitial ad object sending the message.
     */
    virtual void interstitialDidAppear(MPInterstitialAdController *interstitial) {}
    
    /** @name Detecting When an Interstitial Ad is Dismissed */
    
    /**
     * Sent after an interstitial ad object has been dismissed from the screen, returning control
     * to your application.
     *
     * Your implementation of this method should resume any application activity that was paused
     * prior to the interstitial being presented on-screen.
     *
     * @param interstitial The interstitial ad object sending the message.
     */
    virtual void interstitialDidDisappear(MPInterstitialAdController *interstitial) {}
    
    /**
     * Sent after an interstitial was clicked.
     *
     * @param interstitial The interstitial ad object sending the message.
     */
    virtual void interstitialClicked(MPInterstitialAdController *interstitial) {}
    
    /** @name Detecting When an Interstitial Ad Expires */
    
    /**
     * Sent when a loaded interstitial ad is no longer eligible to be displayed.
     *
     * Interstitial ads from certain networks (such as iAd) may expire their content at any time,
     * even if the content is currently on-screen. This method notifies you when the currently-
     * loaded interstitial has expired and is no longer eligible for display.
     *
     * If the ad was on-screen when it expired, you can expect that the ad will already have been
     * dismissed by the time this message is sent.
     *
     * Your implementation may include a call to `loadAd` to fetch a new ad, if desired.
     *
     * @param interstitial The interstitial ad object sending the message.
     */
    void interstitialDidExpire(MPInterstitialAdController *interstitial) {}
};
    
enum class MPNativeAdOrientation
{
        MPNativeAdOrientationAny,
        MPNativeAdOrientationPortrait,
        MPNativeAdOrientationLandscape
};
    
class MPAdViewDelegate;
    
/**
* The MPAdView class provides a view that can display banner advertisements.
*/
    
enum class UIInterfaceOrientation {
    Portrait,
    PortraitUpsideDown,
    LandscapeLeft,
    LandscapeRight,
};
    
class MPAdView
{

    /** @name Initializing a Banner Ad */

    /**
     * Initializes an MPAdView with the given ad unit ID and banner size.
     *
     * @param adUnitId A string representing a MoPub ad unit ID.
     * @param size The desired ad size. A list of standard ad sizes is available in MPConstants.h.
     * @return A newly initialized ad view corresponding to the given ad unit ID and size.
     */
    static MPAdView *adViewForAdUnitId(const std::string &adUnitId);

    /** @name Setting and Getting the Delegate */

    /**
     * The delegate (`MPAdViewDelegate`) of the ad view.
     *
     * @warning **Important**: Before releasing an instance of `MPAdView`, you must set its delegate
     * property to `nil`.
     */
    virtual void setDelegate(MPAdViewDelegate *delegate) = 0;
    virtual MPAdViewDelegate* getDelegate() const = 0;

    /** @name Setting Request Parameters */

    /**
     * The MoPub ad unit ID for this ad view.
     *
     * Ad unit IDs are created on the MoPub website. An ad unit is a defined placement in your
     * application set aside for advertising. If no ad unit ID is set, the ad view will use a default
     * ID that only receives test ads.
     */
    virtual const std::string& getAdUnitId() const = 0;

    /**
     * A string representing a set of keywords that should be passed to the MoPub ad server to receive
     * more relevant advertising.
     *
     * Keywords are typically used to target ad campaigns at specific user segments. They should be
     * formatted as comma-separated key-value pairs (e.g. "marital:single,age:24").
     *
     * On the MoPub website, keyword targeting options can be found under the "Advanced Targeting"
     * section when managing campaigns.
     */
    virtual void setLocation(float latitude, float longitude, float accuracyInMeters) = 0;
    
    /**
     * A `CLLocation` object representing a user's location that should be passed to the MoPub ad server
     * to receive more relevant advertising.
     */
    virtual void setKeywords(const std::string& keywords) = 0;

    /** @name Enabling Test Mode */

    /**
     * A Boolean value that determines whether the ad view should request ads in test mode.
     *
     * The default value is NO.
     * @warning **Important**: If you set this value to YES, make sure to reset it to NO before
     * submitting your application to the App Store.
     */
    virtual void setTesting(bool testing) = 0;

    /** @name Loading a Banner Ad */

    /**
     * Requests a new ad from the MoPub ad server.
     *
     * If the ad view is already loading an ad, this call will be ignored. You may use `forceRefreshAd`
     * if you would like cancel any existing ad requests and force a new ad to load.
     */
    virtual void loadAd() = 0;

    /**
     * Requests a new ad from the MoPub ad server.
     *
     * If the ad view is already loading an ad, this call will be ignored. You may use `forceRefreshAd`
     * if you would like cancel any existing ad requests and force a new ad to load.
     *
     * **Warning**: This method has been deprecated. Use `loadAd` instead.
     */
    virtual void refreshAd() = 0;

    /**
     * Cancels any existing ad requests and requests a new ad from the MoPub ad server.
     */
    virtual void forceRefreshAd() = 0;

    /** @name Handling Orientation Changes */

    /**
     * Informs the ad view that the device orientation has changed.
     *
     * Banners from some third-party ad networks have orientation-specific behavior. You should call
     * this method when your application's orientation changes if you want mediated ads to acknowledge
     * their new orientation.
     *
     * If your application layout needs to change based on the size of the mediated ad, you may want to
     * check the value of `adContentViewSize` after calling this method, in case the orientation change
     * causes the mediated ad to resize.
     *
     * @param newOrientation The new interface orientation (after orientation changes have occurred).
     */
    virtual void rotateToOrientation(UIInterfaceOrientation newOrientation) = 0;

    /**
     * Forces third-party native ad networks to only use ads sized for the specified orientation.
     *
     * Banners from some third-party ad networks have orientation-specific behaviors and/or sizes.
     * You may use this method to lock ads to a certain orientation. For instance,
     * if you call this with MPInterfaceOrientationPortrait, native networks (e.g. iAd) will never
     * return ads sized for the landscape orientation.
     *
     * @param orientation An MPNativeAdOrientation enum value.
     *
     * <pre><code>typedef enum {
     *          MPNativeAdOrientationAny,
     *          MPNativeAdOrientationPortrait,
     *          MPNativeAdOrientationLandscape
     *      } MPNativeAdOrientation;
     * </pre></code>
     *
     * @see unlockNativeAdsOrientation
     * @see allowedNativeAdsOrientation
     */

    virtual void lockNativeAdsToOrientation(MPNativeAdOrientation orientation);

    /**
     * Allows third-party native ad networks to use ads sized for any orientation.
     *
     * You do not need to call this method unless you have previously called
     * `lockNativeAdsToOrientation:`.
     *
     * @see lockNativeAdsToOrientation:
     * @see allowedNativeAdsOrientation
     */
    virtual void unlockNativeAdsOrientation();

    /**
     * Returns the banner orientations that third-party ad networks are allowed to use.
     *
     * @return An enum value representing an allowed set of orientations.
     *
     * @see lockNativeAdsToOrientation:
     * @see unlockNativeAdsOrientation
     */
    virtual MPNativeAdOrientation allowedNativeAdsOrientation() = 0;

    /** @name Obtaining the Size of the Current Ad */

    /**
     * Returns the size of the current ad being displayed in the ad view.
     *
     * Ad sizes may vary between different ad networks. This method returns the actual size of the
     * underlying mediated ad. This size may be different from the original, initialized size of the
     * ad view. You may use this size to determine to adjust the size or positioning of the ad view
     * to avoid clipping or border issues.
     *
     * @returns The size of the underlying mediated ad.
     */
    virtual void adContentViewSize(int &width, int &height) = 0;

    /** @name Managing the Automatic Refreshing of Ads */

    /**
     * Stops the ad view from periodically loading new advertisements.
     *
     * By default, an ad view is allowed to automatically load new advertisements if a refresh interval
     * has been configured on the MoPub website. This method prevents new ads from automatically loading,
     * even if a refresh interval has been specified.
     *
     * As a best practice, you should call this method whenever the ad view will be hidden from the user
     * for any period of time, in order to avoid unnecessary ad requests. You can then call
     * `startAutomaticallyRefreshingContents` to re-enable the refresh behavior when the ad view becomes
     * visible.
     *
     * @see startAutomaticallyRefreshingContents
     */
    virtual void stopAutomaticallyRefreshingContents() = 0;

    /**
     * Causes the ad view to periodically load new advertisements in accordance with user-defined
     * refresh settings on the MoPub website.
     *
     * Calling this method is only necessary if you have previously stopped the ad view's refresh
     * behavior using `stopAutomaticallyRefreshingContents`. By default, an ad view is allowed to
     * automatically load new advertisements if a refresh interval has been configured on the MoPub
     * website. This method has no effect if a refresh interval has not been set.
     *
     * @see stopAutomaticallyRefreshingContents
     */
    virtual void startAutomaticallyRefreshingContents() = 0;

};
    
////////////////////////////////////////////////////////////////////////////////////////////////////
    
    
/**
* The delegate of an `MPAdView` object must adopt the `MPAdViewDelegate` protocol. It must
* implement `viewControllerForPresentingModalView` to provide a root view controller from which
* the ad view should present modal content.
*
* Optional methods of this protocol allow the delegate to be notified of banner success or
* failure, as well as other lifecycle events.
*/
    
class MPAdViewDelegate
{
public:
    
    /** @name Detecting When a Banner Ad is Loaded */
    
    /**
     * Sent when an ad view successfully loads an ad.
     *
     * Your implementation of this method should insert the ad view into the view hierarchy, if you
     * have not already done so.
     *
     * @param view The ad view sending the message.
     */
    virtual void adViewDidLoadAd(MPAdView *view) = 0;
    
    /**
     * Sent when an ad view fails to load an ad.
     *
     * To avoid displaying blank ads, you should hide the ad view in response to this message.
     *
     * @param view The ad view sending the message.
     */
    virtual void adViewDidFailToLoadAd(MPAdView *view) = 0;
    
    /** @name Detecting When a User Interacts With the Ad View */
    
    /**
     * Sent when an ad view is about to present modal content.
     *
     * This method is called when the user taps on the ad view. Your implementation of this method
     * should pause any application activity that requires user interaction.
     *
     * @param view The ad view sending the message.
     * @see `didDismissModalViewForAd:`
     */
    virtual void willPresentModalViewForAd(MPAdView *view) = 0;
    
    /**
     * Sent when an ad view has dismissed its modal content, returning control to your application.
     *
     * Your implementation of this method should resume any application activity that was paused
     * in response to `willPresentModalViewForAd:`.
     *
     * @param view The ad view sending the message.
     * @see `willPresentModalViewForAd:`
     */
    virtual void didDismissModalViewForAd(MPAdView *view) = 0;
    
    /**
     * Sent when a user is about to leave your application as a result of tapping
     * on an ad.
     *
     * Your application will be moved to the background shortly after this method is called.
     *
     * @param view The ad view sending the message.
     */
    virtual void willLeaveApplicationFromAd(MPAdView *view) = 0;
    
};

} // namespace avalon

#endif /* __MOPUB_H__ */
