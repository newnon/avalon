//
//  ShareManager.mm
//
//

#include "avalon/DeltaDNA.h"
#include "avalon/platform/ios/utils/utils.h"
#import "DDNASDK.h"

namespace avalon {
    

void DDNAProductBuilder::setRealCurrency(const std::string &type, int amount)
{
    _data["realCurrency"] = amount;
}
    
void DDNAProductBuilder::addVirtualCurrency(const std::string &type, int amount, const std::string &name)
{
    utils::ValueVector& vector = _data.insert(std::pair<std::string, utils::Value>("virtualCurrencies", utils::Value(utils::ValueVector()))).first->second.asValueVector();
    utils::ValueMap object;
    utils::ValueMap virtualCurrency;
    virtualCurrency["virtualCurrencyAmount"] = amount;
    virtualCurrency["virtualCurrencyName"] = name;
    virtualCurrency["virtualCurrencyType"] = type;
    object["virtualCurrency"] = std::move(virtualCurrency);
    vector.push_back(utils::Value(std::move(object)));
}
    
void DDNAProductBuilder::addItem(const std::string &type, int amount, const std::string &name)
{
    utils::ValueVector& vector = _data.insert(std::pair<std::string, utils::Value>("items", utils::Value(utils::ValueVector()))).first->second.asValueVector();
    utils::ValueMap object;
    utils::ValueMap item;
    item["itemType"] = type;
    item["itemAmount"] = amount;
    item["itemName"] = name;
    object["item"] = std::move(item);
    vector.push_back(utils::Value(std::move(object)));
}
    
const utils::ValueMap& DDNAProductBuilder::DDNAProductBuilder::dictionary() const
{
    return _data;
}

    
void DDNAEventBuilder::setString(const std::string &value, const std::string &key)
{
    _data[key] = value;
}
void DDNAEventBuilder::setInteger(int value, const std::string &key)
{
    _data[key] = value;
}
void DDNAEventBuilder::setBoolean(bool value, const std::string &key)
{
    _data[key] = value;
}
//void DDNAEventBuilder::setTimestamp(NSDate *value, const std::string &key)
void DDNAEventBuilder::setDictionary(const utils::ValueMap &value, const std::string &key)
{
    _data[key] = value;
}
const utils::ValueMap& DDNAEventBuilder::dictionary() const
{
    return _data;
}

class iOSDDNASettings: public DDNASettings
{
public:
    iOSDDNASettings(::DDNASDK *sdk):_sdk(sdk)
    {
        
    }
    
    virtual bool getOnFirstRunSendNewPlayerEvent() const override
    {
        return _sdk.settings.onFirstRunSendNewPlayerEvent;
    }
    
    virtual void setOnFirstRunSendNewPlayerEvent(bool value) override
    {
        _sdk.settings.onFirstRunSendNewPlayerEvent = value;
    }
    
    virtual bool getOnStartSendClientDeviceEvent() const override
    {
        return _sdk.settings.onStartSendClientDeviceEvent;
    }
    
    virtual void setOnStartSendClientDeviceEvent(bool value) override
    {
        _sdk.settings.onStartSendClientDeviceEvent = value;
    }
    
    virtual bool getOnStartSendGameStartedEvent() const override
    {
        return _sdk.settings.onStartSendGameStartedEvent;
    }
    
    virtual void setOnStartSendGameStartedEvent(bool value) override
    {
        _sdk.settings.onStartSendGameStartedEvent = value;
    }
    
    virtual int getHttpRequestRetryDelaySeconds() const override
    {
        return _sdk.settings.httpRequestRetryDelaySeconds;
    }
    
    virtual void setHttpRequestRetryDelaySeconds(int value) override
    {
        _sdk.settings.httpRequestRetryDelaySeconds = value;
    }
    
    virtual int getHttpRequestMaxTries() const override
    {
        return _sdk.settings.httpRequestMaxTries;
    }
    
    virtual void setHttpRequestMaxTries(int value) override
    {
        _sdk.settings.httpRequestMaxTries = value;
    }
    
    virtual int getHttpRequestTimeoutSeconds() const override
    {
        return _sdk.settings.httpRequestTimeoutSeconds;
    }
    
    virtual void setHttpRequestTimeoutSeconds(int value) override
    {
        _sdk.settings.httpRequestTimeoutSeconds = value;
    }
    
    virtual bool getBackgroundEventUpload() const override
    {
        return _sdk.settings.backgroundEventUpload;
    }
    
    virtual void setBackgroundEventUpload(bool value) override
    {
        _sdk.settings.backgroundEventUpload = value;
    }
    
    virtual int getBackgroundEventUploadStartDelaySeconds() const override
    {
        return _sdk.settings.backgroundEventUploadStartDelaySeconds;
    }
    
    virtual void setBackgroundEventUploadStartDelaySeconds(int value) override
    {
        _sdk.settings.backgroundEventUploadStartDelaySeconds = value;
    }
    
    virtual int getBackgroundEventUploadRepeatRateSeconds() const override
    {
        return _sdk.settings.backgroundEventUploadRepeatRateSeconds;
    }
    
    virtual void setBackgroundEventUploadRepeatRateSeconds(int value) override
    {
        _sdk.settings.backgroundEventUploadRepeatRateSeconds = value;
    }
    
private:
    ::DDNASDK *_sdk;
};
    
std::string DDNASettings::getPrivateSettingsDirectoryPath()
{
    return [[::DDNASettings getPrivateSettingsDirectoryPath] UTF8String];
}

class iOSDeltaDNA : public DeltaDNA
{
public:
    iOSDeltaDNA(DDNASDK *sdk):_sdk(sdk),_settings(new iOSDDNASettings(sdk))
    {
        
    }
    
    virtual DDNASettings* getDDNASettings() const override
    {
        return _settings;
    }
    
    virtual std::string getHashSecret() const override
    {
        return [_sdk.hashSecret UTF8String];
    }

    virtual void setHashSecret(const std::string &hashSecret) override
    {
        _sdk.hashSecret = [NSString stringWithUTF8String:hashSecret.c_str()];
    }

    virtual std::string getClientVersion() const override
    {
        return [_sdk.clientVersion UTF8String];
    }

    virtual void setClientVersion(const std::string &clientVersion) override
    {
        _sdk.clientVersion = [NSString stringWithUTF8String:clientVersion.c_str()];
    }

    virtual std::string getPushNotificationToken() const override
    {
        return [_sdk.pushNotificationToken UTF8String];
    }

    virtual void setPushNotificationToken(const std::string &pushNotificationToken) override
    {
        _sdk.hashSecret = [NSString stringWithUTF8String:pushNotificationToken.c_str()];
    }

    virtual std::string getEnvironmentKey() const override
    {
        return [_sdk.environmentKey UTF8String];
    }

    virtual std::string getCollectURL() const override
    {
        return [_sdk.collectURL UTF8String];
    }

    virtual std::string getEngageURL() const override
    {
        return [_sdk.engageURL UTF8String];
    }

    virtual std::string getUserID() const override
    {
        return [_sdk.userID UTF8String];
    }

    virtual std::string getSessionID() const override
    {
        return [_sdk.sessionID UTF8String];
    }

    virtual std::string getPlatform() const override
    {
        return [_sdk.platform UTF8String];
    }

    virtual bool getHasStarted() const override
    {
        return _sdk.hasStarted;
    }
    
    virtual void startWithEnvironmentKey(const std::string &environmentKey,
                                         const std::string &collectURL,
                                         const std::string &engageURL) override
    {
        [_sdk startWithEnvironmentKey:[NSString stringWithUTF8String:environmentKey.c_str()] collectURL:[NSString stringWithUTF8String:collectURL.c_str()] engageURL:[NSString stringWithUTF8String:engageURL.c_str()]];
    }
    
    virtual void startWithEnvironmentKey(const std::string &environmentKey,
                                         const std::string &collectURL,
                                         const std::string &engageURL,
                                         const std::string &userID) override
    {
        [_sdk startWithEnvironmentKey:[NSString stringWithUTF8String:environmentKey.c_str()] collectURL:[NSString stringWithUTF8String:collectURL.c_str()] engageURL:[NSString stringWithUTF8String:engageURL.c_str()] userID:[NSString stringWithUTF8String:userID.c_str()]];
    }

    virtual void newSession() override
    {
        [_sdk newSession];
    }

    virtual void stop() override
    {
        [_sdk stop];
    }

    virtual void recordEvent(const std::string &eventName) override
    {
        [_sdk recordEvent:[NSString stringWithUTF8String:eventName.c_str()]];
    }
    
    virtual void recordEvent(const std::string &eventName, const utils::ValueMap &eventParams) override
    {
        NSMutableDictionary *dictionary = [NSMutableDictionary dictionary];
        for(const auto &it:eventParams)
        {
            [dictionary setObject:valueToId(it.second) forKey:[NSString stringWithUTF8String:it.first.c_str()]];
        }
        
        [_sdk recordEvent:[NSString stringWithUTF8String:eventName.c_str()] withEventDictionary:dictionary];
    }

    /*virtual void recordEvent(const std::string &eventName, const DDNAEventBuilder &eventBuilder) override
    {
    }*/

    virtual void requestEngagement(const std::string &decisionPoint, const DDNAEngagementResponseBlock &callback) override
    {
        [_sdk requestEngagement:[NSString stringWithUTF8String:decisionPoint.c_str()] callbackBlock:^(NSDictionary *engageResponse) {
            
        }];
    }

    virtual void requestEngagement(const std::string &decisionPoint,
                                   const utils::ValueMap &engageParams,
                                   const DDNAEngagementResponseBlock &callback) override
    {
        [_sdk requestEngagement:[NSString stringWithUTF8String:decisionPoint.c_str()] withEngageParams:nil callbackBlock:^(NSDictionary *engageResponse) {
            
        }];
    }

    virtual void requestImageMessage(const std::string &decisionPoint,
                                     const utils::ValueMap &engageParams,
                                     DDNAPopup *popup) override
    {
        [_sdk requestImageMessage:[NSString stringWithUTF8String:decisionPoint.c_str()] withEngageParams:nil imagePopup:nil];
    }
    
    virtual void requestImageMessage(const std::string &decisionPoint,
                                     const utils::ValueMap &engageParams,
                                     DDNAPopup *popup,
                                     const DDNAEngagementResponseBlock &callback) override
    {
        [_sdk requestImageMessage:[NSString stringWithUTF8String:decisionPoint.c_str()] withEngageParams:nil imagePopup:nil callbackBlock:^(NSDictionary *engageResponse) {
            
        }];
    }

    virtual void recordPushNotification(const utils::ValueMap &pushNotification, bool didLaunch) override
    {
        [_sdk recordPushNotification:nil didLaunch:didLaunch];
    }

    virtual void upload() override
    {
        [_sdk upload];
    }

    virtual void clearPersistentData() override
    {
        [_sdk clearPersistentData];
    }
    
private:
    ::DDNASDK *_sdk;
    iOSDDNASettings *_settings;
};
    
DeltaDNA* DeltaDNA::getInstance()
{
    static DeltaDNA* instance = new iOSDeltaDNA([::DDNASDK sharedInstance]);
    return instance;
}

std::string DeltaDNA::DDNA_SDK_VERSION()
{
    return [::DDNA_SDK_VERSION UTF8String];
}
    
std::string DeltaDNA::DDNA_ENGAGE_API_VERSION()
{
    return [::DDNA_ENGAGE_API_VERSION UTF8String];
}

std::string DeltaDNA::DDNA_EVENT_STORAGE_PATH()
{
    return [::DDNA_EVENT_STORAGE_PATH UTF8String];
}

std::string DeltaDNA::DDNA_ENGAGE_STORAGE_PATH()
{
    return [::DDNA_ENGAGE_STORAGE_PATH UTF8String];
}

std::string DeltaDNA::DDNA_COLLECT_URL_PATTERN()
{
    return [::DDNA_COLLECT_URL_PATTERN UTF8String];
}

std::string DeltaDNA::DDNA_COLLECT_HASH_URL_PATTERN()
{
    return [::DDNA_COLLECT_HASH_URL_PATTERN UTF8String];
}

std::string DeltaDNA::DDNA_ENGAGE_URL_PATTERN()
{
    return [::DDNA_ENGAGE_URL_PATTERN UTF8String];
}

std::string DeltaDNA::DDNA_ENGAGE_HASH_URL_PATTERN()
{
    return [::DDNA_ENGAGE_HASH_URL_PATTERN UTF8String];
}

} // namespace avalon
