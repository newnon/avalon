#ifndef __IAD_H__
#define __IAD_H__

#include <string>
#include <vector>
#include <map>
#include <list>
#include <memory>
#include "Ads.h"

namespace avalon {
    
/*!
 * ADInterstitialAd is a full screen advertisement, available on iPad since
 * iOS 4.3, and on iPhone since iOS 7.0.
 *
 * There are three ways to display an ADInterstitialAd:
 *
 *   • By providing a container view and calling -presentInView:
 *   • By providing a view controller and calling -presentFromViewController:
 *   • Indirectly, by setting the interstitial presentation policy on a view
 *     controller to Automatic or Manual (via -[UIViewController setInterstitialPresentationPolicy:]),
 *     and letting the framework manage presentation.
 *
 * An ADInterstitialAd should not be presented until it has loaded content. This
 * is indicated via the -interstitialAdDidLoad: delegate method, and can also be
 * queried via the loaded property. If the interstitial ad is presented when
 * loaded=NO, an exception will be thrown.
 *
 * Note that using interstitial ads on iPhones running iOS < 7.0 will cause an
 * exception to be thrown.
 */
class IADInterstitial: public Interstitial
{
public:
    static IADInterstitial* create(InterstitialDelegate *delegate);
};

enum class ADErrorCodes
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

enum class ADAdType
{
    Banner,
    MediumRectangle
};
    
/*!
 * @class ADBanner
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
class IADBanner: public Banner
{
public:
    /*!
     * @method initWithAdType:
     *
     * @discussion
     * Initialize the view with a specific ad type. The ad type cannot be changed
     * after initialization.
     */
    static IADBanner* create(ADAdType type, BannerDelegate *delegate);
};

}

#endif /* __IAD_H__ */
