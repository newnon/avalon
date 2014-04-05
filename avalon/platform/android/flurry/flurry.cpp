//
//  AnalyticX.cpp
//  AnalyticX
//
//  Created by diwwu on 5/11/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "avalon/Flurry.h"

#include <jni.h>
#include "platform/android/jni/JniHelper.h"
#include <android/log.h>
#include <string>
#include <map>

const char* const CLASS_NAME = "com/flurry/android/FlurryAgent";
const char* const HELPER_CLASS_NAME = "com/avalon/flurry/FlurryHelper";

#define  LOG_TAG    "Flurry Debug"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)

using namespace std;

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

void Flurry::logEvent(const std::string &eventName) {

    cocos2d::JniMethodInfo methodInfo;

    if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,CLASS_NAME, "logEvent", "(Ljava/lang/String;)V"))
    {
        jstring jEventName = methodInfo.env->NewStringUTF(eventName.c_str());
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jEventName);
        methodInfo.env->DeleteLocalRef(jEventName);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
}

void Flurry::logEventTimed(const std::string &eventName, bool timed) {
    
    cocos2d::JniMethodInfo methodInfo;

    if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,CLASS_NAME, "logEvent", "(Ljava/lang/String;Z)V"))
    {
        jstring jEventName = methodInfo.env->NewStringUTF(eventName.c_str());
        jboolean jTimed = timed;
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jEventName, jTimed);
        methodInfo.env->DeleteLocalRef(jEventName);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    };
}

void Flurry::logEventWithParameters(const std::string &eventName, const std::map<std::string, std::string> &parameters) {

    cocos2d::JniMethodInfo methodInfo;

    if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,CLASS_NAME, "logEvent", "(Ljava/lang/String;Ljava/util/Map;)V"))
    {
        jstring jEventName = methodInfo.env->NewStringUTF(eventName.c_str());
        jobject jParameters = jobjectFromDictionary(parameters);
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jEventName, jParameters);
        methodInfo.env->DeleteLocalRef(jEventName);
        methodInfo.env->DeleteLocalRef(jParameters);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    };
}

void Flurry::logEventWithParametersTimed(const std::string &eventName, const std::map<std::string,std::string> &parameters, bool timed) {

    cocos2d::JniMethodInfo methodInfo;

    if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,CLASS_NAME, "logEvent", "(Ljava/lang/String;Ljava/util/Map;Z)V"))
    {
        jstring jEventName = methodInfo.env->NewStringUTF(eventName.c_str());
        jobject jParameters = jobjectFromDictionary(parameters);
        jboolean jTimed = timed;
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jEventName, jParameters, jTimed);
        methodInfo.env->DeleteLocalRef(jEventName);
        methodInfo.env->DeleteLocalRef(jParameters);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    };
}

// Since Flurry for Android does not support *parameters* in *endTimedEvent()*.
// This *parameters* will be ignored when running in Android
void Flurry::endTimedEventWithParameters(const std::string &eventName, const std::map<std::string,std::string> &parameters) {
    
    cocos2d::JniMethodInfo methodInfo;

    if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,CLASS_NAME, "endTimedEvent", "(Ljava/lang/String;Ljava/util/Map;)V"))
    {
        jstring jEventName = methodInfo.env->NewStringUTF(eventName.c_str());
        jobject jParameters = jobjectFromDictionary(parameters);
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jEventName, jParameters);
        methodInfo.env->DeleteLocalRef(jEventName);
        methodInfo.env->DeleteLocalRef(jParameters);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    };
}

void Flurry::logPageView() {
    
    cocos2d::JniMethodInfo methodInfo;

    if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,CLASS_NAME, "onPageView", "()V"))
    {
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    };
}

void Flurry::setCaptureUncaughtExceptions(bool isEnabled)
{
    cocos2d::JniMethodInfo methodInfo;

    if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,CLASS_NAME, "setCaptureUncaughtExceptions", "(Z)V"))
    {
        jboolean jEnabled= isEnabled;
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jEnabled);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    };
}

void Flurry::logException(const std::string &errorID, const std::string &message, const std::string &errorClass)
{
    cocos2d::JniMethodInfo methodInfo;

    if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,CLASS_NAME, "onError", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V"))
    {
        jstring jErrorID = methodInfo.env->NewStringUTF(errorID.c_str());
        jstring jMessage = methodInfo.env->NewStringUTF(message.c_str());
        jstring jErrorClass = methodInfo.env->NewStringUTF(errorClass.c_str());
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jErrorID, jMessage, jErrorClass);
        methodInfo.env->DeleteLocalRef(jErrorID);
        methodInfo.env->DeleteLocalRef(jMessage);
        methodInfo.env->DeleteLocalRef(jErrorClass);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    };
}

void Flurry::setAppVersion(const std::string &version) {
    
    cocos2d::JniMethodInfo methodInfo;

    if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,CLASS_NAME, "setVersionName", "(Ljava/lang/String;)V"))
    {
        jstring jVersion = methodInfo.env->NewStringUTF(version.c_str());
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jVersion);
        methodInfo.env->DeleteLocalRef(jVersion);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    };
}

std::string Flurry::getFlurryAgentVersion() {
    
    cocos2d::JniMethodInfo methodInfo;

    if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,CLASS_NAME, "getAgentVersion", "()I"))
    {
        jint version = methodInfo.env->CallStaticIntMethod(methodInfo.classID, methodInfo.methodID);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
        static char result[20];
        sprintf(result, "%d", version);
        return result;
    }
    else
        return "";
}

void Flurry::setShowErrorInLogEnabled(bool value) {
    //do nothing on Android
    //only supported in iOS
}

void Flurry::setDebugLogEnabled(bool value) {
    cocos2d::JniMethodInfo methodInfo;

    if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,CLASS_NAME, "setLogEnabled", "(Z)V"))
    {
        jboolean jValue = value;
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jValue);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    };
}

void Flurry::setLogLevel(FlurryLogLevel level)
{
    cocos2d::JniMethodInfo methodInfo;

    if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,CLASS_NAME, "setLogLevel", "(I)V"))
    {
        jint jLevel = (int)level;
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jLevel);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    };
}

void Flurry::setSessionContinueSeconds(int seconds) {
    cocos2d::JniMethodInfo methodInfo;

    if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,CLASS_NAME, "setContinueSessionMillis", "(J)V"))
    {
        jlong jValue = seconds * 1000;
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jValue);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    };
}

void Flurry::setSecureTransportEnabled(bool value) {
    cocos2d::JniMethodInfo methodInfo;

    if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,CLASS_NAME, "setUseHttps", "(Z)V"))
    {
        jboolean jValue = value;
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jValue);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    };
}

void Flurry::startSession(const std::string &apiKey) {
    
    cocos2d::JniMethodInfo methodInfo;

    if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "onStartSession", "(Ljava/lang/String;)V"))
    {
        jstring jEventName = methodInfo.env->NewStringUTF(apiKey.c_str());
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jEventName);
        methodInfo.env->DeleteLocalRef(jEventName);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
}

void Flurry::startSession(const std::string &apiKey, const std::map<std::string,std::string> &options)
{
    cocos2d::JniMethodInfo methodInfo;

    if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "onStartSession", "(Ljava/lang/String;)V"))
    {
        jstring jEventName = methodInfo.env->NewStringUTF(apiKey.c_str());
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jEventName);
        methodInfo.env->DeleteLocalRef(jEventName);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
}

void Flurry::endSession() {
    
    cocos2d::JniMethodInfo methodInfo;

    if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "onEndSession", "()V"))
    {
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
}

void Flurry::setUserID(const std::string &userID) {
    
    cocos2d::JniMethodInfo methodInfo;

    if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,CLASS_NAME, "setUserId", "(Ljava/lang/String;)V"))
    {
        jstring jUserID = methodInfo.env->NewStringUTF(userID.c_str());
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jUserID);
        methodInfo.env->DeleteLocalRef(jUserID);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
}

void Flurry::setAge(int age) {
    
    cocos2d::JniMethodInfo methodInfo;

    if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,CLASS_NAME, "setAge", "(I)V"))
    {
        jint jAge = age;
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jAge);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
}


void Flurry::setGender(const std::string &gender) {
    
    cocos2d::JniMethodInfo methodInfo;

    if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,CLASS_NAME, "setGender", "(B)V"))
    {
        jbyte jGender = -1;
        if (gender == "m")
            jGender = 1;
        if (gender == "f")
            jGender = 0;
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jGender);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
}

void Flurry::setLatitudeLongitudeHorizontalAccuracyVerticalAccuracy(double latitude, double longitude, float horizontalAccuracy, float verticalAccuracy) {
    // Not supported in Android
}

void Flurry::setReportLocation(bool reportLocation) {
    cocos2d::JniMethodInfo methodInfo;

    if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,CLASS_NAME, "setReportLocation", "(Z)V"))
    {
        jboolean jValue = reportLocation;
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jValue);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    };
}

void Flurry::pauseBackgroundSession() {
    //iOS only
}

void Flurry::setSessionReportsOnCloseEnabled(bool sendSessionReportsOnClose) {
    //iOS only
}

void Flurry::setSessionReportsOnPauseEnabled(bool setSessionReportsOnPauseEnabled) {
    //iOS only
}

void Flurry::setEventLoggingEnabled(bool value) {
    //iOS only
}

void Flurry::setCrashReportingEnabled(bool value){
	//iOS only
}

} // namespace avalon







