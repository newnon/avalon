#ifndef __IAD_H__
#define __IAD_H__

#include <string>
#include <vector>
#include <map>
#include <list>
#include <memory>
#include "Ads.h"

namespace avalon {
    
class ADInterstitialAdDelegate;
    
class ADInterstitialAd
{
public:
    static ADInterstitialAd* create();
    /*!
     * @property delegate
     *
     * @discussion
     * The interstitial ad delegate is notified when it has loaded, when errors
     * occur in getting ads, when actions begin and end, and when it has unloaded.
     *
     * On iOS 5 and later, this property is a weak reference and cannot be used with
     * objects that modify the behavior of release or retain.
     */
    virtual void setDelegate(ADInterstitialAdDelegate *delegate) = 0;
    virtual ADInterstitialAdDelegate* getDelegate() const = 0;

    /*!
     * @property loaded
     *
     * @return
     * YES if an ad is loaded, NO otherwise. This property should always be checked
     * before the interstitial ad is presented.
     */
    virtual bool isLoaded() const = 0;

    /*!
     * @property actionInProgress
     *
     * @discussion
     * Actions display full screen content in a modal session. Use this property to
     * determine if such an action is currently in progress.
     */
    virtual bool isActionInProgress() const = 0;

    /*!
     * @property cancelAction
     *
     * @discussion
     * Cancels the current in-progress action. This should only be used in cases
     * where the user's attention is required immediately. If this method is called,
     * -interstitialAdActionDidFinish: will not be called.
     */
    virtual void cancelAction() = 0;

    /*
     * @method presentInView:
     *
     * @discussion
     * This method should be used to display the interstitial in a view hierarchy.
     * The view hierarchy must be managed by a view controller and the size of the
     * container view must fit the following constraints:
     *
     *   • Width must be DEVICE_WIDTH for the current orientation.
     *   • Height must be at least (DEVICE_HEIGHT - STATUS_BAR_HEIGHT - NAVIGATION_BAR_HEIGHT - TAB_BAR_HEIGHT).
     *   • Height must not exceed DEVICE_HEIGHT
     *
     * @throws NSInternalInconsistencyException
     * If the container view is not already in a view controller's managed hierarchy,
     * an exception will be thrown.
     *
     * @throws NSInternalInconsistencyException
     * If the interstitial has not loaded at the time this method is invoked, an
     * exception will be thrown.
     *
     * @return
     * YES if the interstitial could be presented, NO otherwise.
     */
    virtual bool presentInView(/*UIView *containerView*/) = 0;
    
    virtual ~ADInterstitialAd() {}
    
protected:
    ADInterstitialAd() {}
};

enum class ADError
{
    Unknown = 0,
    ServerFailure = 1,
    LoadingThrottled = 2,
    InventoryUnavailable = 3,
    ConfigurationError = 4,
    BannerVisibleWithoutContent = 5,
    ApplicationInactive = 6,
    AdUnloaded = 7,
};
    
class ADInterstitialAdDelegate
{
public:

    /*!
     * @method interstitialAdDidUnload:
     *
     * @discussion
     * When this method is invoked, if the application is using -presentInView:, the
     * content will be unloaded from the container shortly after this method is
     * called and no new content will be loaded. This may occur either as a result
     * of user actions or if the ad content has expired.
     *
     * In the case of an interstitial presented via -presentInView:, the layout of
     * the app should be updated to reflect that an ad is no longer visible. e.g.
     * by removing the view used for presentation and replacing it with another view.
     */
    virtual void interstitialAdDidUnload(ADInterstitialAd *interstitialAd) = 0;

    /*!
     * @method interstitialAd:didFailWithError:
     *
     * @discussion
     * Called when an error has occurred attempting to get ad content.
     *
     * @see ADError for a list of possible error codes.
     */
    virtual void interstitialDidFailWithError(ADInterstitialAd *interstitialAd, ADError error) = 0;

    /*!
     * @method interstitialAdWillLoad:
     *
     * @discussion
     * Called when the interstitial has confirmation that an ad will be presented,
     * but before the ad has loaded resources necessary for presentation.
     */
    virtual void interstitialAdWillLoad(ADInterstitialAd *interstitialAd) {}

    /*!
     * @method interstitialAdDidLoad:
     *
     * @discussion
     * Called when the interstitial ad has finished loading ad content. The delegate
     * should implement this method so it knows when the interstitial ad is ready to
     * be presented.
     */
    virtual void interstitialAdDidLoad(ADInterstitialAd *interstitialAd) {}

    /*!
     * @method interstitialAdActionShouldBegin:willLeaveApplication:
     *
     * @discussion
     * Called when the user chooses to interact with the interstitial ad.
     *
     * The delegate may return NO to block the action from taking place, but this
     * should be avoided if possible because most ads pay significantly more when
     * the action takes place and, over the longer term, repeatedly blocking actions
     * will decrease the ad inventory available to the application.
     *
     * Applications should reduce their own activity while the advertisement's action
     * executes.
     */
    virtual bool interstitialAdActionShouldBegin(ADInterstitialAd *interstitialAd, bool willLeave) { return true; }

    /*!
     * @method interstitialAdActionDidFinish:
     * This message is sent when the action has completed and control is returned to
     * the application. Games, media playback, and other activities that were paused
     * in response to the beginning of the action should resume at this point.
     */
    virtual void interstitialAdActionDidFinish(ADInterstitialAd *interstitialAd) {}
    
    virtual ~ADInterstitialAdDelegate() {}

};

enum class ADAdType
{
    Banner,
    MediumRectangle
};
    
class ADBannerViewDelegate;
    
/*!
 * @class ADBannerView
 *
 * @discussion
 * Provides a view for displaying iAds in an application. iAds are automatically
 * loaded, presented, and refreshed. When a banner view is tapped, the iAd will
 * begin its action. In most cases, the action displays a fullscreen interactive
 * iAd.
 *
 * Note: ADBannerViews must be added to a view hierarchy managed by a
 * UIViewController. If view controller containment is being used, the controller
 * managing the banner view must be correctly configured to ensure banner action
 * presentation works correctly.
 */
class ADBannerView{
public:
    /*!
     * @method initWithAdType:
     *
     * @discussion
     * Initialize the view with a specific ad type. The ad type cannot be changed
     * after initialization.
     */
    static ADBannerView* createWithAdType(ADAdType type);

    /*!
     * @property adType
     *
     * @return
     * The banner view's ad type.
     */
    virtual ADAdType getADAdType() const = 0;

    /*!
     * @property delegate
     *
     * @discussion
     * The banner view delegate is notified when events related to the banner view
     * and current ad occur, such as when a new ad is loaded, when errors occur
     * while fetching ads, or when banner actions begin and end.
     *
     * On iOS 5 and later, this property is a weak reference and cannot be used with
     * objects that modify the behavior of release or retain.
     */
    virtual void setDelegate(ADBannerViewDelegate *delegate) = 0;
    virtual ADBannerViewDelegate* getDelegate() const = 0;

    /*!
     * @property bannerLoaded
     *
     * @return
     * YES if an ad is currently loaded, NO otherwise.
     */
    virtual bool isBannerLoaded() const = 0;

    /*!
     * @property bannerViewActionInProgress
     *
     * @return
     * YES if the user is currently engaged with a fullscreen interactive ad.
     */
    virtual bool isBannerViewActionInProgress() const = 0;

    /*!
     * @method cancelBannerViewAction
     *
     * @discussion
     * Cancels the current in-progress banner view action. This should only be used
     * in cases where the user's attention is required immediately.
     */
    virtual void cancelBannerViewAction() = 0;
    
    virtual bool isVisible() = 0;
    virtual void show(int x, int y, int width, int height, BannerScaleType scaleType, BannerGravityType gravity) = 0;
    virtual void hide() = 0;
    
    virtual ~ADBannerView() {}
    
protected:
    ADBannerView() {}
};
    
class ADBannerViewDelegate
{
public:
    
    /*!
     * @method bannerViewWillLoadAd:
     *
     * @discussion
     * Called when a banner has confirmation that an ad will be presented, but
     * before the resources necessary for presentation have loaded.
     */
    virtual void bannerViewWillLoadAd(ADBannerView *banner) {}
    
    /*!
     * @method bannerViewDidLoadAd:
     *
     * @discussion
     * Called each time a banner loads a new ad. Once a banner has loaded an ad, it
     * will display it until another ad is available.
     *
     * It's generally recommended to show the banner view when this method is called,
     * and hide it again when bannerView:didFailToReceiveAdWithError: is called.
     */
    virtual void bannerViewDidLoadAd(ADBannerView *banner) {}
    
    /*!
     * @method bannerView:didFailToReceiveAdWithError:
     *
     * @discussion
     * Called when an error has occurred while attempting to get ad content. If the
     * banner is being displayed when an error occurs, it should be hidden
     * to prevent display of a banner view with no ad content.
     *
     * @see ADError for a list of possible error codes.
     */
    virtual void bannerViewDidFailToReceiveAdWithError(ADBannerView *banner, ADError error) {}
    
    /*!
     * @method bannerViewActionShouldBegin:willLeaveApplication:
     *
     * Called when the user taps on the banner and some action is to be taken.
     * Actions either display full screen content modally, or take the user to a
     * different application.
     *
     * The delegate may return NO to block the action from taking place, but this
     * should be avoided if possible because most ads pay significantly more when
     * the action takes place and, over the longer term, repeatedly blocking actions
     * will decrease the ad inventory available to the application.
     *
     * Applications should reduce their own activity while the advertisement's action
     * executes.
     */
    virtual bool bannerViewActionShouldBegin(ADBannerView *banner, bool willLeave) { return true; }
    
    /*!
     * @method bannerViewActionDidFinish:
     *
     * Called when a modal action has completed and control is returned to the
     * application. Games, media playback, and other activities that were paused in
     * bannerViewActionShouldBegin:willLeaveApplication: should resume at this point.
     */
    virtual void bannerViewActionDidFinish(ADBannerView *banner) {}
    
    virtual ~ADBannerViewDelegate() {}
    
};

}

#endif /* __IAD_H__ */
