//
//  DeltaDNA.h
//  DeltaDNA
//

#ifndef AVALON_DELTADNA_H
#define AVALON_DELTADNA_H

#include "avalon/dictionary.h"
#include <string>
#include <functional>

namespace avalon {

class DDNASettings
{
public:
    /**
     Instructs the SDK to send a newPlayer event when the game
     is run the first time.
     */
    virtual bool getOnFirstRunSendNewPlayerEvent() const = 0;
    virtual void setOnFirstRunSendNewPlayerEvent(bool value) = 0;
    
    /**
     Instructs the SDK to send a clientDevice event when the
     game is started.
     */
    virtual bool getOnStartSendClientDeviceEvent() const = 0;
    virtual void setOnStartSendClientDeviceEvent(bool value) = 0;
    
    /**
     Instructs the SDK to send a gameStarted event when the
     game is started.
     */
    virtual bool getOnStartSendGameStartedEvent() const = 0;
    virtual void setOnStartSendGameStartedEvent(bool value) = 0;
    
    /**
     Controls the delay in seconds between retrying failed
     HTTP requests.
     */
    virtual int getHttpRequestRetryDelaySeconds() const = 0;
    virtual void setHttpRequestRetryDelaySeconds(int value) = 0;
    
    /**
     Controls the number of times the SDK retries a failed
     HTTP request before giving up.
     */
    virtual int getHttpRequestMaxTries() const = 0;
    virtual void setHttpRequestMaxTries(int value) = 0;
    
    /**
     Controls the timeout in seconds before the SDK decides
     a HTTP request is unresponsive.
     */
    virtual int getHttpRequestTimeoutSeconds() const = 0;
    virtual void setHttpRequestTimeoutSeconds(int value) = 0;
    
    /**
     Controls if the SDK should automatically upload events
     in the background.
     */
    virtual bool getBackgroundEventUpload() const = 0;
    virtual void setBackgroundEventUpload(bool value) = 0;
    
    /**
     Controls how many seconds the SDK waits before starting
     to upload events in the backgroound.
     */
    virtual int getBackgroundEventUploadStartDelaySeconds() const = 0;
    virtual void setBackgroundEventUploadStartDelaySeconds(int value) = 0;
    
    /**
     Controls how frequently the event upload method is called.
     */
    virtual int getBackgroundEventUploadRepeatRateSeconds() const = 0;
    virtual void setBackgroundEventUploadRepeatRateSeconds(int value) = 0;
    
    /**
     Returns the path to the privates settings directory on 
     this device.
     */
    static std::string getPrivateSettingsDirectoryPath();
    
    virtual ~DDNASettings() {}
};

class DDNAPopup
{
    
};

class DDNAProductBuilder
{
public:
    void setRealCurrency(const std::string &type, int amount);
    void addVirtualCurrency(const std::string &type, int amount, const std::string &name);
    void addItem(const std::string &type, int amount, const std::string &name);
    const utils::ValueMap& dictionary() const;
    
private:
    utils::ValueMap _data;
};

class DDNAEventBuilder
{
public:
    void setString(const std::string &value, const std::string &key);
    void setInteger(int value, const std::string &key);
    void setBoolean(bool value, const std::string &key);
    //void setTimestamp(NSDate *value, const std::string &key);
    void setDictionary(const utils::ValueMap &value, const std::string &key);
    const utils::ValueMap& dictionary() const;
    
private:
    utils::ValueMap _data;
};

class DeltaDNA
{
public:
    
    static std::string DDNA_SDK_VERSION();
    static std::string DDNA_ENGAGE_API_VERSION();
    
    static std::string DDNA_EVENT_STORAGE_PATH();
    static std::string DDNA_ENGAGE_STORAGE_PATH();
    static std::string DDNA_COLLECT_URL_PATTERN();
    static std::string DDNA_COLLECT_HASH_URL_PATTERN();
    static std::string DDNA_ENGAGE_URL_PATTERN();
    static std::string DDNA_ENGAGE_HASH_URL_PATTERN();
    
    /**
     Change default SDK behaviour via the settings property.
     See DDNASettings.h for available options.
     */
    virtual DDNASettings* getDDNASettings() const = 0;

    /**
     The hash secret for your environment.  This must be set
     @b before starting the SDK.  You only need to set this
     if hashing as been enabled for this environment.  If 
     hashing is enabled and the secret has not been set Collect
     will return 403 errors.
     */
    virtual std::string getHashSecret() const = 0;
    virtual void setHashSecret(const std::string &hashSecret) = 0;

    /**
     A version string for your game.  This is used to help you
     identify which version of your game is being played on the
     platform.  This must be set @b before starting the SDK.
     */
    virtual std::string getClientVersion() const = 0;
    virtual void setClientVersion(const std::string &clientVersion) = 0;

    /**
     The Apple push notification token. Set this @b before starting
     the SDK to enable DeltaDNA to send push notifications to your
     game.  
     
     @see http://docs.deltadna.com/ios-sdk/#PushNotification for
     an example of how to get the token.
     */
    virtual std::string getPushNotificationToken() const = 0;
    virtual void setPushNotificationToken(const std::string &pushNotificationToken) = 0;

    /// The environment key for this game environment (Dev or Live).
    virtual std::string getEnvironmentKey() const = 0;
    /// The URL for Collect for this environment.
    virtual std::string getCollectURL() const = 0;
    /// The URL for Engage for this environment.
    virtual std::string getEngageURL() const = 0;
    /// The User ID for this game.
    virtual std::string getUserID() const = 0;
    /// The Session ID for this game.
    virtual std::string getSessionID() const = 0;
    /// The platform this game is running on.
    virtual std::string getPlatform() const = 0;

    /// Has the SDK been started yet.
    virtual bool getHasStarted() const = 0;

    /**
     Singleton access to the deltaDNA SDK.
     @return The deltaDNA SDK instance.
     */
    static DeltaDNA* getInstance();

    /**
     The SDK must be started once before you can send events.
     @param environmentKey The games's unique environment key.
     @param collectURL The games's unique Collect URL.
     @param engageURL The games's unique EngageURL, use nil if not using Engage.
     */
    virtual void startWithEnvironmentKey(const std::string &environmentKey,
                                         const std::string &collectURL,
                                         const std::string &engageURL) = 0;

    /**
     The SDK must be started once before you can send events.
     @param environmentKey The games's unique environment key.
     @param collectURL The games's unique Collect URL.
     @param engageURL The games's unique EngageURL, use nil if not using Engage.
     @param userID The user id to associate the game events with, use nil if you want the SDK to generate a random one.
     */
    virtual void startWithEnvironmentKey(const std::string &environmentKey,
                                         const std::string &collectURL,
                                         const std::string &engageURL,
                                         const std::string &userID) = 0;

    /**
     Generates a new session id, subsequent events will belong to a new session.
     */
    virtual void newSession() = 0;

    /**
     Sends a 'gameEnded' event to Collect and stops background uploads.
     */
    virtual void stop() = 0;

    /**
     Records an event with no custom parameters.
     @param eventName The name of the event schema.
     */
    virtual void recordEvent(const std::string &eventName) = 0;

    /**
     Records an event with event parameters.  Build the dictionary to
     match the @b eventParams structure of you event schema.
     @param eventName The name of the event schema.
     @param eventParam A dictionary of event parameters.
     */
    virtual void recordEvent(const std::string &eventName, const utils::ValueMap &eventParams) = 0;

    /**
     Records an event with event parameters built from the DDNAEventBuilder helper
     class.
     @param eventName The name of the eventSchema.
     @param eventBuilder The event parameters.
     */
    //virtual void recordEvent(const std::string &eventName, const DDNAEventBuilder &eventBuilder) = 0;

    /**
     @typedef
     
     @abstract Block type for the callback from an Engage request.
     */
    typedef std::function<void(const utils::ValueMap &engageResponse)> DDNAEngagementResponseBlock;

    /**
     Makes an Engage call for a decision point.  If the decision point is
     recognised, the callback block is called with the response parameters.
     @param decisionPoint The decision point.
     @param callback The block to call once Engage returns.
     */
    virtual void requestEngagement(const std::string &decisionPoint, const DDNAEngagementResponseBlock &callback) = 0;

    /**
     Makes an Engage call for a decision point.  If the decision point is
     recognised, the callback block is called with the response parameters.
     @param decisionPoint The decision point.
     @param engageParams A dictionary of parameters for Engage.
     @param callback The block to call once Engage returns.
     */
    virtual void requestEngagement(const std::string &decisionPoint,
                                   const utils::ValueMap &engageParams,
                                   const DDNAEngagementResponseBlock &callback) = 0;

    /**
     Requests an image based engagement for popup on the screen.  This is a convienience around @c requestEngagement that loads the image resource automatically from the original engage request.  Register a block with the popup's afterPrepare block to be notified when the resource has loaded.
     @param decisionPoint The decisionPoint
     @param engageParams A dictionary of parameters for Engage.
     @param imagePopup An object that conforms to the @c DDNAPopup protocol that can handle the response.
     */
    virtual void requestImageMessage(const std::string &decisionPoint,
                                     const utils::ValueMap &engageParams,
                                     DDNAPopup *popup) = 0;

    /**
     Requests an image based engagement for popup on the screen.  This is a convienience around @c requestEngagement that loads the image resource automatically from the original engage request.  Register a block with the popup's afterPrepare block to be notified when the resource has loaded.
     @param decisionPoint The decisionPoint
     @param engageParams A dictionary of parameters for Engage.
     @param imagePopup An object that conforms to the @c DDNAPopup protocol that can handle the response.
     @param callbackBlock A block that is called with the full engage response for custom behaviour.
     */
    virtual void requestImageMessage(const std::string &decisionPoint,
                                     const utils::ValueMap &engageParams,
                                     DDNAPopup *popup,
                                     const DDNAEngagementResponseBlock &callback) = 0;

    /**
     Records receiving a push notification.  Call from @c application:didFinishLaunchingWithOptions and @c application:didReceiveRemoteNotification so we can track the open rate of your notifications.  It is safe to call this method before @c startWithEnvironmentKey:collectURL:engageURL, the event will be queued.
     */
    virtual void recordPushNotification(const utils::ValueMap &pushNotification, bool didLaunch) = 0;

    /**
     Sends recorded events to deltaDNA.  The default SDK behaviour is to call this
     periodically in the background for you.  If you disable background uploading
     you must call this method reguraly to send your game events.  The call is
     non blocking.
     */
    virtual void upload() = 0;

    /**
     Clears persisted data from the device.  This includes any cached events that
     haven't been sent to Collect, cached engagement request responses and the 
     user id.  If the user id was auto generated by the SDK, a new user id will
     be created next time the game runs.  The newPlayer event will also be sent
     again.
     */
    virtual void clearPersistentData() = 0;
    virtual ~DeltaDNA() {}
};

}

#endif //AVALON_DELTADNA_H