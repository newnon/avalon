//
//  AppsFlyer.h
//

#ifndef AVALON_APPSFLYER_H
#define AVALON_APPSFLYER_H

#include "avalon/dictionary.h"
#include <string>
#include <functional>

namespace avalon {

/*
 * This delegate should be use if you want to use AppsFlyer conversion data. See AppsFlyer iOS
 * Tracking SDK documentation for more details http://support.appsflyer.com/entries/25458906-iOS-SDK-Integration-Guide-v2-5-3-x-New-API-
 */
class AppsFlyerTrackerDelegate
{
public:
    virtual void onConversionDataReceived(const utils::ValueMap &installData) {};
    virtual void onConversionDataRequestFailure(int code, const std::string &message) {};
    virtual void onAppOpenAttribution(const utils::ValueMap &attributionData) {};
    virtual void onAppOpenAttributionFailure(int code, const std::string &message) {};
    virtual ~AppsFlyerTrackerDelegate() {}
};

class AppsFlyerTracker
{
public:
    // In app event names constants
    static const std::string EventLevelAchieved;
    static const std::string EventAddPaymentInfo;
    static const std::string EventAddToCart;
    static const std::string EventAddToWishlist;
    static const std::string EventCompleteRegistration;
    static const std::string EventTutorial_completion;
    static const std::string EventInitiatedCheckout;
    static const std::string EventPurchase;
    static const std::string EventRate;
    static const std::string EventSearch;
    static const std::string EventSpentCredits;
    static const std::string EventAchievementUnlocked;
    static const std::string EventContentView;
    static const std::string EventListView;
    static const std::string EventTravelBooking;
    static const std::string EventShare;
    static const std::string EventInvite;
    static const std::string EventLogin;
    static const std::string EventReEngage;
    static const std::string EventUpdate;
    static const std::string EventOpenedFromPushNotification;
    
    // In app event parameter names
    static const std::string EventParamLevel;
    static const std::string EventParamScore;
    static const std::string EventParamSuccess;
    static const std::string EventParamPrice;
    static const std::string EventParamContentType;
    static const std::string EventParamContentId;
    static const std::string EventParamCurrency;
    static const std::string EventParamQuantity;
    static const std::string EventParamRegistrationMethod;
    static const std::string EventParamPaymentInfoAvailable;
    static const std::string EventParamMaxRatingValue;
    static const std::string EventParamRatingValue;
    static const std::string EventParamSearchString;
    static const std::string EventParamDateA;
    static const std::string EventParamDateB;
    static const std::string EventParamDestinationA;
    static const std::string EventParamDestinationB;
    static const std::string EventParamDescription;
    static const std::string EventParamClass;
    static const std::string EventParamEventStart;
    static const std::string EventParamEventEnd;
    static const std::string EventParamLat;
    static const std::string EventParamLong;
    static const std::string EventParamCustomerUserId;
    static const std::string EventParamValidated;
    static const std::string EventParamRevenue;
    static const std::string EventParamReceiptId;
    static const std::string EventParam1;
    static const std::string EventParam2;
    static const std::string EventParam3;
    static const std::string EventParam4;
    static const std::string EventParam5;
    static const std::string EventParam6;
    static const std::string EventParam7;
    static const std::string EventParam8;
    static const std::string EventParam9;
    static const std::string EventParam10;
    
    /* In case you use your own user ID in your app, you can set this property to that ID. */
    virtual std::string getCustomerUserID() const = 0;
    virtual void setCustomerUserID(const std::string &customerUserID) = 0;

    /* Use this property to set your AppsFlyer's dev key. */
    virtual std::string getAppsFlyerDevKey() const = 0;
    virtual void setAppsFlyerDevKey(const std::string &appsFlyerDevKey) = 0;

    /* Use this property to set your app's Apple ID (taken from the app's page on iTunes Connect) */
    virtual std::string getAppleAppID() const = 0;
    virtual void setAppleAppID(const std::string &appleAppID) = 0;

    /*
     * In case of in app purchase events, you can set the currency code your user has purchased with.
     * The currency code is a 3 letter code according to ISO standards. Example: "USD"
     */
    virtual std::string getCurrencyCode() const = 0;
    virtual void setCurrencyCode(const std::string &currencyCode) = 0;

    /* 
     * AppsFLyer SDK collect Apple's advertisingIdentifier if the AdSupport framework included in the SDK.
     * You can disable this behavior by setting the following property to YES.
     */
    virtual bool getDisableAppleAdSupportTracking() const = 0;
    virtual void setDisableAppleAdSupportTracking(bool value) = 0;

    /* 
     * Prints our messages to the log. This property should only be used in DEBUG mode. The default value 
     * is NO.
     */
    virtual bool getIsDebug() const = 0;
    virtual void setIsDebug(bool value) = 0;

    /*
     * Opt-out tracking for specific user
     */
    virtual bool getDeviceTrackingDisabled() const = 0;
    virtual void setDeviceTrackingDisabled(bool value) = 0;

    /*
     * Opt-out tracking for iAd attributions
     */
    virtual bool getDisableIAdTracking() const = 0;
    virtual void setDisableIAdTracking(bool value) = 0;

    /*
     * AppsFlyer delegate. See AppsFlyerTrackerDelegate abvoe
     */
    virtual AppsFlyerTrackerDelegate* getDelegate() const = 0;
    virtual void setDelegate(AppsFlyerTrackerDelegate *delegate) = 0;

    /*
     * In app purchase receipt validation Apple environment (production or sandbox). The default value
     * is NO.
     */
    virtual bool getUseReceiptValidationSandbox() const = 0;
    virtual void setUseReceiptValidationSandbox(bool value) = 0;

    static AppsFlyerTracker* getInstance();

    /* Track application launch*/
    virtual void trackAppLaunch() = 0;

    /*
     * Use this method to track an events with mulitple values. See AppsFlyer's documentation for details. 
     *
     */
    virtual void trackEvent(const std::string &eventName, const utils::ValueMap &values) = 0;

    /*
     * To track in app purchases you can call this method from the completeTransaction: method on 
     * your SKPaymentTransactionObserver.
     */
    virtual void validateAndTrackInAppPurchase(const std::string &productIdentifier,
                                               const std::string &price,
                                               const std::string &currency,
                                               const std::string &tranactionId,
                                               std::function<void(const utils::ValueMap &response)> &successCallback,
                                               std::function<void(int code, const std::string &message)> failedCallback) = 0;

    /* 
     * This method returns AppsFLyer's internal user ID (unique for your app)
     */
    virtual std::string getAppsFlyerUID() const = 0;

    /*
     * In case you want to track deep linking, call this method from your delegate's openURL method with refferer.
     */
    //virtual void handleOpenURL:(NSURL *)url sourceApplication:(NSString *)sourceApplication withAnnotaion:(id) annotation;
    
    virtual ~AppsFlyerTracker() {}

};

}

#endif //AVALON_APPSFLYER_H
