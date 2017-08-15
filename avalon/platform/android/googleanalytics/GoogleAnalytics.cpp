#include "avalon/GoogleAnalytics.h"

#include <jni.h>
#include "platform/android/jni/JniHelper.h"
#include <android/log.h>
#include <string>
#include <map>

const char* const HELPER_CLASS_NAME = "com/avalon/googleanalytics/GoogleAnalyticsHelper";

namespace avalon {

static jobject jobjectFromDictionary(const std::map<std::string,std::string> &dictionary)
{
    JNIEnv* env = cocos2d::JniHelper::getEnv();
    jclass mapClass = env->FindClass("java/util/HashMap");
    if(mapClass == NULL)
    {
        return NULL;
    }

    jsize map_len = 1;

    jmethodID init = env->GetMethodID(mapClass, "<init>", "(I)V");
    jobject hashMap = env->NewObject(mapClass, init, map_len);

    jmethodID put = env->GetMethodID(mapClass, "put", "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");

    for(auto &it : dictionary)
    {
        jstring key = env->NewStringUTF(it.first.c_str());
        jstring value = env->NewStringUTF(it.second.c_str());

        env->CallObjectMethod(hashMap, put, key, value);

        env->DeleteLocalRef(key);
        env->DeleteLocalRef(value);
    }

    env->DeleteLocalRef(mapClass);
    return hashMap;
}

class AndroidGAITracker: public GoogleAnalyticsTracker
{
public:
    virtual void setParameter(const std::string &name, const std::string &value) override
    {
        if(_tracker)
        {
            cocos2d::JniMethodInfo methodInfo;
            if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "setParameter", "(Lcom/google/android/gms/analytics/Tracker;Ljava/lang/String;Ljava/lang/String;)V"))
            {
                jstring jName = methodInfo.env->NewStringUTF(name.c_str());
                jstring jValue = methodInfo.env->NewStringUTF(value.c_str());
                methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, _tracker, jName, jValue);
                methodInfo.env->DeleteLocalRef(jName);
                methodInfo.env->DeleteLocalRef(jValue);
                methodInfo.env->DeleteLocalRef(methodInfo.classID);
            }
        }
    }

    virtual std::string getParameter(const std::string &name) const override
    {
        std::string value;
        if(_tracker)
        {
            cocos2d::JniMethodInfo methodInfo;
            if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "getParameter", "(Lcom/google/android/gms/analytics/Tracker;Ljava/lang/String;)Ljava/lang/String;"))
            {
                jstring jName = methodInfo.env->NewStringUTF(name.c_str());
                jstring jValue = (jstring)methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID, _tracker, jName);
                value = cocos2d::JniHelper::jstring2string(jValue);
                methodInfo.env->DeleteLocalRef(jName);
                methodInfo.env->DeleteLocalRef(jValue);
                methodInfo.env->DeleteLocalRef(methodInfo.classID);
            }
        }
        return value;
    }

    virtual void setSampleRate(float value) override
    {
        if(_tracker)
        {
            cocos2d::JniMethodInfo methodInfo;
            if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "setSampleRate", "(Lcom/google/android/gms/analytics/Tracker;F)V"))
            {
                methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, _tracker, (jfloat)value);
                methodInfo.env->DeleteLocalRef(methodInfo.classID);
            }
        }
    }

    virtual float getSampleRate() const override
    {
        float ret = 0;
        if(_tracker)
        {
            cocos2d::JniMethodInfo methodInfo;
            if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "getSampleRate", "(Lcom/google/android/gms/analytics/Tracker;)F"))
            {
                ret = methodInfo.env->CallStaticFloatMethod(methodInfo.classID, methodInfo.methodID, _tracker);
                methodInfo.env->DeleteLocalRef(methodInfo.classID);
            }
        }
        return ret;
    }

    virtual const std::string &getTrackerId() const override
    {
        return _trackerId;
    }
    
    virtual bool getAllowIDFACollection() const override
    {
        bool ret = false;
        if(_tracker)
        {
            cocos2d::JniMethodInfo methodInfo;
            if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "getAllowIDFACollection", "(Lcom/google/android/gms/analytics/Tracker;)Z"))
            {
                ret = methodInfo.env->CallStaticBooleanMethod(methodInfo.classID, methodInfo.methodID, _tracker);
                methodInfo.env->DeleteLocalRef(methodInfo.classID);
            }
        }
        return ret;
    }
    
    virtual void setAllowIDFACollection(bool value) override
    {
        if(_tracker)
        {
            cocos2d::JniMethodInfo methodInfo;
            if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "setAllowIDFACollection", "(Lcom/google/android/gms/analytics/Tracker;Z)V"))
            {
                methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, _tracker, (jboolean)value);
                methodInfo.env->DeleteLocalRef(methodInfo.classID);
            }
        }
    }

    virtual void setScreenName(const std::string &name) override
    {
        if(_tracker)
        {
            cocos2d::JniMethodInfo methodInfo;
            if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "setScreenName", "(Lcom/google/android/gms/analytics/Tracker;Ljava/lang/String;)V"))
            {
                jstring jName = methodInfo.env->NewStringUTF(name.c_str());
                methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, _tracker, jName);
                methodInfo.env->DeleteLocalRef(jName);
                methodInfo.env->DeleteLocalRef(methodInfo.classID);
            }
        }
    }

    virtual void setCustomDimension(int index, const std::string &value) override
    {
        if(_tracker)
        {
            cocos2d::JniMethodInfo methodInfo;
            if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "setCustomDimension", "(Lcom/google/android/gms/analytics/Tracker;ILjava/lang/String;)V"))
            {
                jstring jValue = methodInfo.env->NewStringUTF(value.c_str());
                methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, _tracker, (jint)index, jValue);
                methodInfo.env->DeleteLocalRef(jValue);
                methodInfo.env->DeleteLocalRef(methodInfo.classID);
            }
        }
    }
    virtual void setCustomMetric(int index, double value) override
    {
        if(_tracker)
        {
            cocos2d::JniMethodInfo methodInfo;
            if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "setCustomMetric", "(Lcom/google/android/gms/analytics/Tracker;ID)V"))
            {
                methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, _tracker, (jint)index, (jdouble)value);
                methodInfo.env->DeleteLocalRef(methodInfo.classID);
            }
        }
    }

    virtual void setNewSession() override
    {
        if(_tracker)
        {
            cocos2d::JniMethodInfo methodInfo;
            if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "setNewSession", "(Lcom/google/android/gms/analytics/Tracker;)V"))
            {
                methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, _tracker);
                methodInfo.env->DeleteLocalRef(methodInfo.classID);
            }
        }
    }

    virtual void sendAppView(const std::map<std::string, std::string> &params) override
    {
        if(_tracker)
        {
            cocos2d::JniMethodInfo methodInfo;
            if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "sendAppView", "(Lcom/google/android/gms/analytics/Tracker;Ljava/util/Map;)V"))
            {
                jobject jParams = jobjectFromDictionary(params);
                methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, _tracker, jParams);
                methodInfo.env->DeleteLocalRef(jParams);
                methodInfo.env->DeleteLocalRef(methodInfo.classID);
            }
        }
    }

    virtual void sendEvent(const std::string &category, const std::string &action, const std::string &label, long value, const std::map<std::string, std::string> &params) override
    {
        if(_tracker)
        {
            cocos2d::JniMethodInfo methodInfo;
            if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "sendEvent", "(Lcom/google/android/gms/analytics/Tracker;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;JLjava/util/Map;)V"))
            {
                jstring jCategory = methodInfo.env->NewStringUTF(category.c_str());
                jstring jAction = methodInfo.env->NewStringUTF(action.c_str());
                jstring jLabel = methodInfo.env->NewStringUTF(label.c_str());
                jobject jParams = jobjectFromDictionary(params);
                methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, _tracker, jCategory, jAction, jLabel, (jlong)value, jParams);
                methodInfo.env->DeleteLocalRef(jCategory);
                methodInfo.env->DeleteLocalRef(jAction);
                methodInfo.env->DeleteLocalRef(jLabel);
                methodInfo.env->DeleteLocalRef(jParams);
                methodInfo.env->DeleteLocalRef(methodInfo.classID);
            }
        }
    }

    virtual void sendException(const std::string &description, bool fatal, const std::map<std::string, std::string> &params) override
    {
        if(_tracker)
        {
            cocos2d::JniMethodInfo methodInfo;
            if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "sendException", "(Lcom/google/android/gms/analytics/Tracker;Ljava/lang/String;ZLjava/util/Map;)V"))
            {
                jstring jDescription = methodInfo.env->NewStringUTF(description.c_str());
                jobject jParams = jobjectFromDictionary(params);
                methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, _tracker, jDescription, (jboolean)fatal);
                methodInfo.env->DeleteLocalRef(jDescription);
                methodInfo.env->DeleteLocalRef(jParams);
                methodInfo.env->DeleteLocalRef(methodInfo.classID);
            }
        }
    }

    virtual void sendItem(const std::string &transactionId, const std::string &name, const std::string &sku,const std::string &category, double price, long quantity, const std::string &currencyCode, const std::map<std::string, std::string> &params) override
    {
        if(_tracker)
        {
            cocos2d::JniMethodInfo methodInfo;
            if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "sendItem", "(Lcom/google/android/gms/analytics/Tracker;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;DJLjava/lang/String;Ljava/util/Map;)V"))
            {
                jstring jTransactionId = methodInfo.env->NewStringUTF(transactionId.c_str());
                jstring jName = methodInfo.env->NewStringUTF(transactionId.c_str());
                jstring jSku = methodInfo.env->NewStringUTF(sku.c_str());
                jstring jCategory = methodInfo.env->NewStringUTF(category.c_str());
                jstring jCurrencyCode = methodInfo.env->NewStringUTF(currencyCode.c_str());
                jobject jParams = jobjectFromDictionary(params);
                methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, _tracker, jTransactionId, jName, jSku, jCategory, (jdouble)price ,(jlong)quantity, jCurrencyCode, jParams);
                methodInfo.env->DeleteLocalRef(jTransactionId);
                methodInfo.env->DeleteLocalRef(jName);
                methodInfo.env->DeleteLocalRef(jSku);
                methodInfo.env->DeleteLocalRef(jCategory);
                methodInfo.env->DeleteLocalRef(jCurrencyCode);
                methodInfo.env->DeleteLocalRef(jParams);
                methodInfo.env->DeleteLocalRef(methodInfo.classID);
            }
        }
    }

    virtual void sendSocial(const std::string &network, const std::string &action, const std::string &target, const std::map<std::string, std::string> &params) override
    {
        if(_tracker)
        {
            cocos2d::JniMethodInfo methodInfo;
            if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "sendSocial", "(Lcom/google/android/gms/analytics/Tracker;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/util/Map;)V"))
            {
                jstring jNetwork = methodInfo.env->NewStringUTF(network.c_str());
                jstring jAction = methodInfo.env->NewStringUTF(action.c_str());
                jstring jTarget = methodInfo.env->NewStringUTF(target.c_str());
                jobject jParams = jobjectFromDictionary(params);
                methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, _tracker, jNetwork, jAction, jTarget, jParams);
                methodInfo.env->DeleteLocalRef(jNetwork);
                methodInfo.env->DeleteLocalRef(jAction);
                methodInfo.env->DeleteLocalRef(jTarget);
                methodInfo.env->DeleteLocalRef(jParams);
                methodInfo.env->DeleteLocalRef(methodInfo.classID);
            }
        }
    }

    virtual void sendTiming(const std::string &category, long intervalMillis, const std::string &name, const std::string &label, const std::map<std::string, std::string> &params) override
    {
        if(_tracker)
        {
            cocos2d::JniMethodInfo methodInfo;
            if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "sendTiming", "(Lcom/google/android/gms/analytics/Tracker;Ljava/lang/String;JLjava/lang/String;Ljava/lang/String;Ljava/util/Map;)V"))
            {
                jstring jCategory = methodInfo.env->NewStringUTF(category.c_str());
                jstring jName = methodInfo.env->NewStringUTF(name.c_str());
                jstring jLabel = methodInfo.env->NewStringUTF(label.c_str());
                jobject jParams = jobjectFromDictionary(params);
                methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, _tracker, jCategory, (jlong)intervalMillis, jName, jLabel, jParams);
                methodInfo.env->DeleteLocalRef(jCategory);
                methodInfo.env->DeleteLocalRef(jName);
                methodInfo.env->DeleteLocalRef(jLabel);
                methodInfo.env->DeleteLocalRef(jParams);
                methodInfo.env->DeleteLocalRef(methodInfo.classID);
            }
        }
    }

    virtual void sendTransaction(const std::string &transactionId, const std::string &affiliation, double revenue, double tax, double shipping, const std::string &currencyCode, const std::map<std::string, std::string> &params) override
    {
        if(_tracker)
        {
            cocos2d::JniMethodInfo methodInfo;
            if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "sendTransaction", "(Lcom/google/android/gms/analytics/Tracker;Ljava/lang/String;Ljava/lang/String;DDDLjava/lang/String;Ljava/util/Map;)V"))
            {
                jstring jTransactionId = methodInfo.env->NewStringUTF(transactionId.c_str());
                jstring jAffiliation = methodInfo.env->NewStringUTF(affiliation.c_str());
                jstring jCurrencyCode = methodInfo.env->NewStringUTF(currencyCode.c_str());
                jobject jParams = jobjectFromDictionary(params);
                methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, _tracker, jTransactionId, jAffiliation, (jdouble)revenue, (jdouble)tax, (jdouble)shipping, jCurrencyCode, jParams);
                methodInfo.env->DeleteLocalRef(jTransactionId);
                methodInfo.env->DeleteLocalRef(jAffiliation);
                methodInfo.env->DeleteLocalRef(jCurrencyCode);
                methodInfo.env->DeleteLocalRef(jParams);
                methodInfo.env->DeleteLocalRef(methodInfo.classID);
            }
        }
    }

    AndroidGAITracker(const std::string &trackerId, jobject tracker):_tracker(nullptr),_trackerId(trackerId)
    {
        if(tracker)
        {
            _tracker = cocos2d::JniHelper::getEnv()->NewGlobalRef(tracker);
        }
    }
    virtual ~AndroidGAITracker()
    {
        if(_tracker)
        {
            cocos2d::JniHelper::getEnv()->DeleteGlobalRef(_tracker);
        }
    }

    jobject getTrackerObj() const
    {
        return _tracker;
    }

private:
    jobject _tracker;
    std::string _trackerId;
};

void GoogleAnalytics::setDispatchInterval(int value)
{
    cocos2d::JniMethodInfo methodInfo;
    if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "setLocalDispatchPeriod", "(I)V"))
    {
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, (jint)value);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
}
int GoogleAnalytics::getDispatchInterval() const
{
	int ret = 0;
    cocos2d::JniMethodInfo methodInfo;
    if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "getLocalDispatchPeriod", "()I"))
    {
    	ret = methodInfo.env->CallStaticIntMethod(methodInfo.classID, methodInfo.methodID);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
    return ret;
}

void GoogleAnalytics::setTrackUncaughtExceptions(bool value)
{
	if(_defaultTracker)
	{
		cocos2d::JniMethodInfo methodInfo;
		if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "setTrackUncaughtExceptions", "(ZLcom/google/android/gms/analytics/Tracker;)V"))
		{
			methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, (jboolean)value, static_cast<AndroidGAITracker*>(_defaultTracker)->getTrackerObj());
			methodInfo.env->DeleteLocalRef(methodInfo.classID);
		}
	}
}
bool GoogleAnalytics::getTrackUncaughtExceptions() const
{
	bool ret = false;
    cocos2d::JniMethodInfo methodInfo;
    if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "getTrackUncaughtExceptions", "()Z"))
    {
    	ret = methodInfo.env->CallStaticBooleanMethod(methodInfo.classID, methodInfo.methodID);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
    return ret;
}

void GoogleAnalytics::setDryRun(bool value)
{
    cocos2d::JniMethodInfo methodInfo;
    if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "setDryRunEnabled", "(Z)V"))
    {
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, (jboolean)value);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
}
bool GoogleAnalytics::getDryRun() const
{
	bool ret = false;
    cocos2d::JniMethodInfo methodInfo;
    if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "isDryRunEnabled", "()Z"))
    {
    	ret = methodInfo.env->CallStaticBooleanMethod(methodInfo.classID, methodInfo.methodID);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
    return ret;
}

void GoogleAnalytics::setOptOut(bool value)
{
    cocos2d::JniMethodInfo methodInfo;
    if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "setAppOptOut", "(Z)V"))
    {
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, (jboolean)value);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
}
bool GoogleAnalytics::getOptOut() const
{
	bool ret = false;
    cocos2d::JniMethodInfo methodInfo;
    if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "getAppOptOut", "()Z"))
    {
    	ret = methodInfo.env->CallStaticBooleanMethod(methodInfo.classID, methodInfo.methodID);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
    return ret;
}

GoogleAnalytics *GoogleAnalytics::getInstance()
{
    static GoogleAnalytics* instance = new GoogleAnalytics();
    return instance;
}

GoogleAnalyticsTracker* GoogleAnalytics::getTracker(const std::string &trackingId)
{
	AndroidGAITracker *ret = nullptr;
    cocos2d::JniMethodInfo methodInfo;
    if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "getTracker", "(Ljava/lang/String;)Lcom/google/android/gms/analytics/Tracker;"))
    {
    	jstring jTrackingId = methodInfo.env->NewStringUTF(trackingId.c_str());
    	jobject tracker = methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID, jTrackingId);
    	ret = new AndroidGAITracker(trackingId,tracker);
    	_trackers.insert(std::make_pair(trackingId, ret));
        if(!_defaultTracker)
            _defaultTracker = ret;
    	methodInfo.env->DeleteLocalRef(jTrackingId);
    	methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
    return ret;

}
void GoogleAnalytics::removeTracker(GoogleAnalyticsTracker *tracker)
{
    if(tracker)
    {
    	const std::string &trackerId = tracker->getTrackerId();
        cocos2d::JniMethodInfo methodInfo;
        if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "removeTracker", "(Ljava/lang/String;)V"))
        {
        	jstring jTrackingId = methodInfo.env->NewStringUTF(trackerId.c_str());
        	methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jTrackingId);
        	methodInfo.env->DeleteLocalRef(jTrackingId);
        	methodInfo.env->DeleteLocalRef(methodInfo.classID);
        }
        _trackers.erase(trackerId);
        delete tracker;
    }
}

GoogleAnalyticsTracker* GoogleAnalytics::getDefaultTracker()
{
    return _defaultTracker;
}

void GoogleAnalytics::setDefaultTracker(GoogleAnalyticsTracker *tracker)
{
    _defaultTracker = tracker;
}

void GoogleAnalytics::dispatch()
{
    cocos2d::JniMethodInfo methodInfo;
    if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "dispatchLocalHits", "()V"))
    {
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
}

bool GoogleAnalytics::dispatchAndWait(int maxTimeSeconds)
{
	dispatch();
	return false;
}

void GoogleAnalytics::sendHitsInBackground() 
{
}

void GoogleAnalytics::setLogLevel(GoogleAnalyticsLogLevel logLevel)
{
    cocos2d::JniMethodInfo methodInfo;
    if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "setLogLevel", "(I)V"))
    {
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, (jint)logLevel);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
}

GoogleAnalyticsLogLevel GoogleAnalytics::getLogLevel() const
{
	GoogleAnalyticsLogLevel ret = GoogleAnalyticsLogLevel::NONE;
    cocos2d::JniMethodInfo methodInfo;
    if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "getLogLevel", "()I"))
    {
    	ret = static_cast<GoogleAnalyticsLogLevel>(methodInfo.env->CallStaticIntMethod(methodInfo.classID, methodInfo.methodID));
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
    return ret;
}

void GoogleAnalytics::startSession()
{

}

void GoogleAnalytics::endSession()
{

}

GoogleAnalytics::GoogleAnalytics():_defaultTracker(nullptr)
{

}
GoogleAnalytics::~GoogleAnalytics()
{

}

}
