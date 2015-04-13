//
//  ShareManager.mm
//
//

#include "avalon/DeltaDNA.h"
#import "DDNASDK.h"
#include <cstdlib>
#include <sstream>
#include <iomanip>

namespace avalon {
    

void DDNAProductBuilder::setRealCurrency(const std::string &type, int amount)
{
    _data["realCurrency"] = amount;
}
    
void DDNAProductBuilder::addVirtualCurrency(const std::string &type, int amount, const std::string &name)
{
    DNAValueVector& vector = _data.insert(std::pair<std::string, DNAValue>("virtualCurrencies", DNAValue(DNAValueVector()))).first->second.asValueVector();
    DNAValueMap object;
    DNAValueMap virtualCurrency;
    virtualCurrency["virtualCurrencyAmount"] = amount;
    virtualCurrency["virtualCurrencyName"] = name;
    virtualCurrency["virtualCurrencyType"] = type;
    object["virtualCurrency"] = std::move(virtualCurrency);
    vector.push_back(DNAValue(std::move(object)));
}
    
void DDNAProductBuilder::addItem(const std::string &type, int amount, const std::string &name)
{
    DNAValueVector& vector = _data.insert(std::pair<std::string, DNAValue>("items", DNAValue(DNAValueVector()))).first->second.asValueVector();
    DNAValueMap object;
    DNAValueMap item;
    item["itemType"] = type;
    item["itemAmount"] = amount;
    item["itemName"] = name;
    object["item"] = std::move(item);
    vector.push_back(DNAValue(std::move(object)));
}
    
const DNAValueMap& DDNAProductBuilder::DDNAProductBuilder::dictionary() const
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
void DDNAEventBuilder::setDictionary(const DNAValueMap &value, const std::string &key)
{
    _data[key] = value;
}
const DNAValueMap& DDNAEventBuilder::dictionary() const
{
    return _data;
}

    
DNAValue::DNAValue()
: _type(Type::NONE)
{
    memset(&_field, 0, sizeof(_field));
}

DNAValue::DNAValue(int v)
: _type(Type::INTEGER)
{
    _field.intVal = v;
}

DNAValue::DNAValue(float v)
: _type(Type::FLOAT)
{
    _field.floatVal = v;
}

DNAValue::DNAValue(double v)
: _type(Type::DOUBLE)
{
    _field.doubleVal = v;
}

DNAValue::DNAValue(bool v)
: _type(Type::BOOLEAN)
{
    _field.boolVal = v;
}

DNAValue::DNAValue(const char* v)
: _type(Type::STRING)
{
    _field.strVal = new std::string();
    if (v)
    {
        *_field.strVal = v;
    }
}

DNAValue::DNAValue(const std::string& v)
: _type(Type::STRING)
{
    _field.strVal = new std::string();
    *_field.strVal = v;
}

DNAValue::DNAValue(const DNAValueVector& v)
: _type(Type::VECTOR)
{
    _field.vectorVal = new (std::nothrow) DNAValueVector();
    *_field.vectorVal = v;
}

DNAValue::DNAValue(DNAValueVector&& v)
: _type(Type::VECTOR)
{
    _field.vectorVal = new (std::nothrow) DNAValueVector();
    *_field.vectorVal = std::move(v);
}

DNAValue::DNAValue(const DNAValueMap& v)
: _type(Type::MAP)
{
    _field.mapVal = new (std::nothrow) DNAValueMap();
    *_field.mapVal = v;
}

DNAValue::DNAValue(DNAValueMap&& v)
: _type(Type::MAP)
{
    _field.mapVal = new (std::nothrow) DNAValueMap();
    *_field.mapVal = std::move(v);
}

DNAValue::DNAValue(const DNAValue& other)
: _type(Type::NONE)
{
    *this = other;
}

DNAValue::DNAValue(DNAValue&& other)
: _type(Type::NONE)
{
    *this = std::move(other);
}

DNAValue::~DNAValue()
{
    clear();
}

DNAValue& DNAValue::operator= (const DNAValue& other)
{
    if (this != &other) {
        reset(other._type);
        
        switch (other._type) {
            case Type::INTEGER:
                _field.intVal = other._field.intVal;
                break;
            case Type::FLOAT:
                _field.floatVal = other._field.floatVal;
                break;
            case Type::DOUBLE:
                _field.doubleVal = other._field.doubleVal;
                break;
            case Type::BOOLEAN:
                _field.boolVal = other._field.boolVal;
                break;
            case Type::STRING:
                if (_field.strVal == nullptr)
                {
                    _field.strVal = new std::string();
                }
                *_field.strVal = *other._field.strVal;
                break;
            case Type::VECTOR:
                if (_field.vectorVal == nullptr)
                {
                    _field.vectorVal = new (std::nothrow) DNAValueVector();
                }
                *_field.vectorVal = *other._field.vectorVal;
                break;
            case Type::MAP:
                if (_field.mapVal == nullptr)
                {
                    _field.mapVal = new (std::nothrow) DNAValueMap();
                }
                *_field.mapVal = *other._field.mapVal;
                break;
            default:
                break;
        }
    }
    return *this;
}

DNAValue& DNAValue::operator= (DNAValue&& other)
{
    if (this != &other)
    {
        clear();
        switch (other._type)
        {;
            case Type::INTEGER:
                _field.intVal = other._field.intVal;
                break;
            case Type::FLOAT:
                _field.floatVal = other._field.floatVal;
                break;
            case Type::DOUBLE:
                _field.doubleVal = other._field.doubleVal;
                break;
            case Type::BOOLEAN:
                _field.boolVal = other._field.boolVal;
                break;
            case Type::STRING:
                _field.strVal = other._field.strVal;
                break;
            case Type::VECTOR:
                _field.vectorVal = other._field.vectorVal;
                break;
            case Type::MAP:
                _field.mapVal = other._field.mapVal;
                break;
            default:
                break;
        }
        _type = other._type;
        
        memset(&other._field, 0, sizeof(other._field));
        other._type = Type::NONE;
    }
    
    return *this;
}

DNAValue& DNAValue::operator= (int v)
{
    reset(Type::INTEGER);
    _field.intVal = v;
    return *this;
}

DNAValue& DNAValue::operator= (float v)
{
    reset(Type::FLOAT);
    _field.floatVal = v;
    return *this;
}

DNAValue& DNAValue::operator= (double v)
{
    reset(Type::DOUBLE);
    _field.doubleVal = v;
    return *this;
}

DNAValue& DNAValue::operator= (bool v)
{
    reset(Type::BOOLEAN);
    _field.boolVal = v;
    return *this;
}

DNAValue& DNAValue::operator= (const char* v)
{
    reset(Type::STRING);
    *_field.strVal = v ? v : "";
    return *this;
}

DNAValue& DNAValue::operator= (const std::string& v)
{
    reset(Type::STRING);
    *_field.strVal = v;
    return *this;
}

DNAValue& DNAValue::operator= (const DNAValueVector& v)
{
    reset(Type::VECTOR);
    *_field.vectorVal = v;
    return *this;
}

DNAValue& DNAValue::operator= (DNAValueVector&& v)
{
    reset(Type::VECTOR);
    *_field.vectorVal = std::move(v);
    return *this;
}

DNAValue& DNAValue::operator= (const DNAValueMap& v)
{
    reset(Type::MAP);
    *_field.mapVal = v;
    return *this;
}

DNAValue& DNAValue::operator= (DNAValueMap&& v)
{
    reset(Type::MAP);
    *_field.mapVal = std::move(v);
    return *this;
}

bool DNAValue::operator!= (const DNAValue& v)
{
    return !(*this == v);
}
bool DNAValue::operator!= (const DNAValue& v) const
{
    return !(*this == v);
}

bool DNAValue::operator== (const DNAValue& v)
{
    const auto &t = *this;
    return t == v;
}
bool DNAValue::operator== (const DNAValue& v) const
{
    if (this == &v) return true;
    if (v._type != this->_type) return false;
    if (this->isNull()) return true;
    switch (_type)
    {
        case Type::INTEGER: return v._field.intVal    == this->_field.intVal;
        case Type::BOOLEAN: return v._field.boolVal   == this->_field.boolVal;
        case Type::STRING:  return *v._field.strVal   == *this->_field.strVal;
        case Type::FLOAT:   return fabs(v._field.floatVal  - this->_field.floatVal)  <= FLT_EPSILON;
        case Type::DOUBLE:  return fabs(v._field.doubleVal - this->_field.doubleVal) <= FLT_EPSILON;
        case Type::VECTOR:
        {
            const auto &v1 = *(this->_field.vectorVal);
            const auto &v2 = *(v._field.vectorVal);
            const auto size = v1.size();
            if (size == v2.size())
            {
                for (size_t i = 0; i < size; i++)
                {
                    if (v1[i] != v2[i]) return false;
                }
            }
            return true;
        }
        case Type::MAP:
        {
            const auto &map1 = *(this->_field.mapVal);
            const auto &map2 = *(v._field.mapVal);
            for (const auto &kvp : map1)
            {
                auto it = map2.find(kvp.first);
                if (it == map2.end() || it->second != kvp.second)
                {
                    return false;
                }
            }
            return true;
        }
        default:
            break;
    };
    
    return false;
}

int DNAValue::asInt() const
{
    assert(_type != Type::VECTOR && _type != Type::MAP && "Only base type (bool, string, float, double, int) could be converted");
    if (_type == Type::INTEGER)
    {
        return _field.intVal;
    }
    
    if (_type == Type::STRING)
    {
        return atoi(_field.strVal->c_str());
    }
    
    if (_type == Type::FLOAT)
    {
        return static_cast<int>(_field.floatVal);
    }
    
    if (_type == Type::DOUBLE)
    {
        return static_cast<int>(_field.doubleVal);
    }
    
    if (_type == Type::BOOLEAN)
    {
        return _field.boolVal ? 1 : 0;
    }
    
    return 0;
}

float DNAValue::asFloat() const
{
    assert(_type != Type::VECTOR && _type != Type::MAP && "Only base type (bool, string, float, double, int) could be converted");
    if (_type == Type::FLOAT)
    {
        return _field.floatVal;
    }
    
    if (_type == Type::STRING)
    {
        return std::atof(_field.strVal->c_str());
    }
    
    if (_type == Type::INTEGER)
    {
        return static_cast<float>(_field.intVal);
    }
    
    if (_type == Type::DOUBLE)
    {
        return static_cast<float>(_field.doubleVal);
    }
    
    if (_type == Type::BOOLEAN)
    {
        return _field.boolVal ? 1.0f : 0.0f;
    }
    
    return 0.0f;
}

double DNAValue::asDouble() const
{
    assert(_type != Type::VECTOR && _type != Type::MAP && "Only base type (bool, string, float, double, int) could be converted");
    if (_type == Type::DOUBLE)
    {
        return _field.doubleVal;
    }
    
    if (_type == Type::STRING)
    {
        return static_cast<double>(std::atof(_field.strVal->c_str()));
    }
    
    if (_type == Type::INTEGER)
    {
        return static_cast<double>(_field.intVal);
    }
    
    if (_type == Type::FLOAT)
    {
        return static_cast<double>(_field.floatVal);
    }
    
    if (_type == Type::BOOLEAN)
    {
        return _field.boolVal ? 1.0 : 0.0;
    }
    
    return 0.0;
}

bool DNAValue::asBool() const
{
    assert(_type != Type::VECTOR && _type != Type::MAP && "Only base type (bool, string, float, double, int) could be converted");
    if (_type == Type::BOOLEAN)
    {
        return _field.boolVal;
    }
    
    if (_type == Type::STRING)
    {
        return (*_field.strVal == "0" || *_field.strVal == "false") ? false : true;
    }
    
    if (_type == Type::INTEGER)
    {
        return _field.intVal == 0 ? false : true;
    }
    
    if (_type == Type::FLOAT)
    {
        return _field.floatVal == 0.0f ? false : true;
    }
    
    if (_type == Type::DOUBLE)
    {
        return _field.doubleVal == 0.0 ? false : true;
    }
    
    return false;
}

std::string DNAValue::asString() const
{
    assert(_type != Type::VECTOR && _type != Type::MAP && "Only base type (bool, string, float, double, int) could be converted");
    
    if (_type == Type::STRING)
    {
        return *_field.strVal;
    }
    
    std::stringstream ret;
    
    switch (_type)
    {
        case Type::INTEGER:
            ret << _field.intVal;
            break;
        case Type::FLOAT:
            ret << std::fixed << std::setprecision( 7 )<< _field.floatVal;
            break;
        case Type::DOUBLE:
            ret << std::fixed << std::setprecision( 16 ) << _field.doubleVal;
            break;
        case Type::BOOLEAN:
            ret << (_field.boolVal ? "true" : "false");
            break;
        default:
            break;
    }
    return ret.str();
}

DNAValueVector& DNAValue::asValueVector()
{
    assert(_type == Type::VECTOR && "The value type isn't Type::VECTOR");
    return *_field.vectorVal;
}

const DNAValueVector& DNAValue::asValueVector() const
{
    assert(_type == Type::VECTOR && "The value type isn't Type::VECTOR");
    return *_field.vectorVal;
}

DNAValueMap& DNAValue::asValueMap()
{
    assert(_type == Type::MAP && "The value type isn't Type::MAP");
    return *_field.mapVal;
}

const DNAValueMap& DNAValue::asValueMap() const
{
    assert(_type == Type::MAP && "The value type isn't Type::MAP");
    return *_field.mapVal;
}

void DNAValue::clear()
{
    // Free memory the old value allocated
    switch (_type)
    {
        case Type::INTEGER:
            _field.intVal = 0;
            break;
        case Type::FLOAT:
            _field.floatVal = 0.0f;
            break;
        case Type::DOUBLE:
            _field.doubleVal = 0.0;
            break;
        case Type::BOOLEAN:
            _field.boolVal = false;
            break;
        case Type::STRING:
            delete _field.strVal;
            _field.strVal = nullptr;
            break;
        case Type::VECTOR:
            delete _field.vectorVal;
            _field.vectorVal = nullptr;
            break;
        case Type::MAP:
            delete _field.mapVal;
            _field.mapVal = nullptr;
            break;
        default:
            break;
    }
    
    _type = Type::NONE;
}

void DNAValue::reset(Type type)
{
    if (_type == type)
        return;
    
    clear();
    
    // Allocate memory for the new value
    switch (type)
    {
        case Type::STRING:
            _field.strVal = new std::string();
            break;
        case Type::VECTOR:
            _field.vectorVal = new (std::nothrow) DNAValueVector();
            break;
        case Type::MAP:
            _field.mapVal = new (std::nothrow) DNAValueMap();
            break;
        default:
            break;
    }
    
    _type = type;
}
    
static id valueToId(const DNAValue &value)
{
    switch (value.getType()) {
        case DNAValue::Type::INTEGER:
            return [NSNumber numberWithInt:value.asInt()];
            break;
            
        case DNAValue::Type::FLOAT:
            return [NSNumber numberWithFloat:value.asFloat()];
            break;
            
        case DNAValue::Type::DOUBLE:
            return [NSNumber numberWithDouble:value.asDouble()];
            break;
            
        case DNAValue::Type::BOOLEAN:
            return [NSNumber numberWithBool:value.asBool()];
            break;
            
        case DNAValue::Type::STRING:
            return [NSString stringWithUTF8String:value.asString().c_str()];
            break;
            
        case DNAValue::Type::VECTOR:
        {
            NSMutableArray * array = [NSMutableArray array];
            const DNAValueVector &vector = value.asValueVector();
            for(const auto &it:vector)
                [array addObject:valueToId(it)];
            return array;
        }
            break;
            
        case DNAValue::Type::MAP:
        {
            NSMutableDictionary * dictionary = [NSMutableDictionary dictionary];
            const DNAValueMap &map = value.asValueMap();
            for(const auto &it:map)
                [dictionary setObject:valueToId(it.second) forKey:[NSString stringWithUTF8String:it.first.c_str()]];
            return dictionary;
        }
            break;
            
        default:
            return nil;
            break;
    }
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
    
    virtual void recordEvent(const std::string &eventName, const DNAValueMap &eventParams) override
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
                                   const DNAValueMap &engageParams,
                                   const DDNAEngagementResponseBlock &callback) override
    {
        [_sdk requestEngagement:[NSString stringWithUTF8String:decisionPoint.c_str()] withEngageParams:nil callbackBlock:^(NSDictionary *engageResponse) {
            
        }];
    }

    virtual void requestImageMessage(const std::string &decisionPoint,
                                     const DNAValueMap &engageParams,
                                     DDNAPopup *popup) override
    {
        [_sdk requestImageMessage:[NSString stringWithUTF8String:decisionPoint.c_str()] withEngageParams:nil imagePopup:nil];
    }
    
    virtual void requestImageMessage(const std::string &decisionPoint,
                                     const DNAValueMap &engageParams,
                                     DDNAPopup *popup,
                                     const DDNAEngagementResponseBlock &callback) override
    {
        [_sdk requestImageMessage:[NSString stringWithUTF8String:decisionPoint.c_str()] withEngageParams:nil imagePopup:nil callbackBlock:^(NSDictionary *engageResponse) {
            
        }];
    }

    virtual void recordPushNotification(const DNAValueMap &pushNotification, bool didLaunch) override
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
