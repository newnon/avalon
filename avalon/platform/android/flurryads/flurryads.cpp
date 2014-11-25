//
//  AnalyticX.cpp
//  AnalyticX
//
//  Created by diwwu on 5/11/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "avalon/FlurryAds.h"

#include <jni.h>
#include "platform/android/jni/JniHelper.h"
#include <android/log.h>
#include <string>
#include <map>

const char* const CLASS_NAME = "com/flurry/android/FlurryAds";
const char* const HELPER_CLASS_NAME = "com/avalon/flurryads/FlurryAdsHelper";

namespace avalon {

static FlurryAdsDelegate *_flurryAdsDelegate = nullptr;

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

void FlurryAds::fetchAdForSpace(const std::string &space, AdSize size)
{
    cocos2d::JniMethodInfo methodInfo;

    if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "fetchAdForSpace", "(Ljava/lang/String;I)V"))
    {
        jstring jSpace = methodInfo.env->NewStringUTF(space.c_str());
        jint jsize = (int)size;
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jSpace, jsize);
        methodInfo.env->DeleteLocalRef(jSpace);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
}

bool FlurryAds::adReadyForSpace(const std::string &space)
{
	cocos2d::JniMethodInfo methodInfo;

	bool ret = false;

	if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "adReadyForSpace", "(Ljava/lang/String;)Z"))
	{
	    jstring jSpace = methodInfo.env->NewStringUTF(space.c_str());
	    ret = methodInfo.env->CallStaticBooleanMethod(methodInfo.classID, methodInfo.methodID, jSpace);
	    methodInfo.env->DeleteLocalRef(jSpace);
	    methodInfo.env->DeleteLocalRef(methodInfo.classID);
	}
	return ret;
}
void FlurryAds::displayAdForSpace(const std::string &space)
{
    cocos2d::JniMethodInfo methodInfo;

    if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "displayAdForSpace", "(Ljava/lang/String;)V"))
    {
        jstring jSpace = methodInfo.env->NewStringUTF(space.c_str());
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jSpace);
        methodInfo.env->DeleteLocalRef(jSpace);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
}
void FlurryAds::displayAdForSpaceModally(const std::string &space)
{
    cocos2d::JniMethodInfo methodInfo;

    if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "displayAdForSpaceModally", "(Ljava/lang/String;)V"))
    {
        jstring jSpace = methodInfo.env->NewStringUTF(space.c_str());
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jSpace);
        methodInfo.env->DeleteLocalRef(jSpace);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
}
void FlurryAds::fetchAndDisplayAdForSpace(const std::string &space, AdSize size)
{
    cocos2d::JniMethodInfo methodInfo;

    if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "fetchAndDisplayAdForSpace", "(Ljava/lang/String;I)V"))
    {
        jstring jSpace = methodInfo.env->NewStringUTF(space.c_str());
        jint jsize = (int)size;
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jSpace, jsize);
        methodInfo.env->DeleteLocalRef(jSpace);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
}

void FlurryAds::removeAdFromSpace(const std::string &space)
{
    cocos2d::JniMethodInfo methodInfo;

    if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "removeAdFromSpace", "(Ljava/lang/String;)V"))
    {
        jstring jSpace = methodInfo.env->NewStringUTF(space.c_str());
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jSpace);
        methodInfo.env->DeleteLocalRef(jSpace);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
}
void FlurryAds::initialize()
{
    cocos2d::JniMethodInfo methodInfo;

    if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "initialize", "()V"))
    {
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
}

void FlurryAds::setAdDelegate(FlurryAdsDelegate *delegate)
{
    _flurryAdsDelegate = delegate;

    cocos2d::JniMethodInfo methodInfo;

    if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "setDelegate", "(Z)V"))
    {
        jboolean jenable = delegate != nullptr;
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jenable);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
}
void FlurryAds::enableTestAds(bool enable)
{
    cocos2d::JniMethodInfo methodInfo;

    if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "enableTestAds", "(Z)V"))
    {
    	jboolean jenable = enable;
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jenable);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
}
void FlurryAds::setUserCookies(const std::map<std::string,std::string> &userCookies)
{
    cocos2d::JniMethodInfo methodInfo;

    if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,CLASS_NAME, "setUserCookies", "(Ljava/util/Map;)V"))
    {
        jobject jParameters = jobjectFromDictionary(userCookies);
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jParameters);
        methodInfo.env->DeleteLocalRef(jParameters);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
}

void FlurryAds::clearUserCookies()
{
    cocos2d::JniMethodInfo methodInfo;

    if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,CLASS_NAME, "clearUserCookies ", "()V"))
    {
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
}

void FlurryAds::setKeywordsForTargeting(const std::map<std::string,std::string> &keywords)
{
    cocos2d::JniMethodInfo methodInfo;

    if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,CLASS_NAME, "setTargetingKeywords", "(Ljava/util/Map;)V"))
    {
        jobject jParameters = jobjectFromDictionary(keywords);
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jParameters);
        methodInfo.env->DeleteLocalRef(jParameters);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
}

void FlurryAds::clearKeywords()
{
    cocos2d::JniMethodInfo methodInfo;

    if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,CLASS_NAME, "clearTargetingKeywords ", "()V"))
    {
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
}

} // namespace avalon

extern "C" {

JNIEXPORT jboolean JNICALL Java_com_avalon_flurryads_FlurryAdsHelper_delegateShouldDisplayAd(JNIEnv* env, jclass clazz, jstring adSpace, jboolean interstitial)
{
    if(avalon::_flurryAdsDelegate)
    {
        std::string space = cocos2d::JniHelper::jstring2string(adSpace);
        return avalon::_flurryAdsDelegate->shouldDisplayAd(space, interstitial);
    }
    return true;
}

JNIEXPORT void JNICALL Java_com_avalon_flurryads_FlurryAdsHelper_delegateOnAdClosed(JNIEnv* env, jclass clazz, jstring adSpace, jboolean interstitial)
{
    if(avalon::_flurryAdsDelegate)
    {
        std::string space = cocos2d::JniHelper::jstring2string(adSpace);
        avalon::_flurryAdsDelegate->onAdClosed(space, interstitial);
    }
}

JNIEXPORT void JNICALL Java_com_avalon_flurryads_FlurryAdsHelper_delegateOnApplicationExit(JNIEnv* env, jclass clazz, jstring adSpace)
{
    if(avalon::_flurryAdsDelegate)
    {
        std::string space = cocos2d::JniHelper::jstring2string(adSpace);
        avalon::_flurryAdsDelegate->onApplicationExit(space);
    }
}

JNIEXPORT void JNICALL Java_com_avalon_flurryads_FlurryAdsHelper_delegateOnRendered(JNIEnv* env, jclass clazz, jstring adSpace)
{
    if(avalon::_flurryAdsDelegate)
    {
        std::string space = cocos2d::JniHelper::jstring2string(adSpace);
        avalon::_flurryAdsDelegate->onRendered(space);
    }
}

JNIEXPORT void JNICALL Java_com_avalon_flurryads_FlurryAdsHelper_delegateOnRenderFailed(JNIEnv* env, jclass clazz, jstring adSpace, jstring jerror)
{
    if(avalon::_flurryAdsDelegate)
    {
        std::string space = cocos2d::JniHelper::jstring2string(adSpace);
        std::string error = cocos2d::JniHelper::jstring2string(jerror);
        avalon::_flurryAdsDelegate->onRenderFailed(space, error);
    }
}

JNIEXPORT void JNICALL Java_com_avalon_flurryads_FlurryAdsHelper_delegateOnDidReceiveAd(JNIEnv* env, jclass clazz, jstring adSpace)
{
    if(avalon::_flurryAdsDelegate)
    {
        std::string space = cocos2d::JniHelper::jstring2string(adSpace);
        avalon::_flurryAdsDelegate->onDidReceiveAd(space);
    }
}

JNIEXPORT void JNICALL Java_com_avalon_flurryads_FlurryAdsHelper_delegateOnDidFailToReceiveAd(JNIEnv* env, jclass clazz, jstring adSpace, jstring jerror)
{
    if(avalon::_flurryAdsDelegate)
    {
        std::string space = cocos2d::JniHelper::jstring2string(adSpace);
        std::string error = cocos2d::JniHelper::jstring2string(jerror);
        avalon::_flurryAdsDelegate->onDidFailToReceiveAd(space, error);
    }
}

JNIEXPORT void JNICALL Java_com_avalon_flurryads_FlurryAdsHelper_delegateOnAdClicked(JNIEnv* env, jclass clazz, jstring adSpace)
{
    if(avalon::_flurryAdsDelegate)
    {
        std::string space = cocos2d::JniHelper::jstring2string(adSpace);
        avalon::_flurryAdsDelegate->onAdClicked(space);
    }
}

JNIEXPORT void JNICALL Java_com_avalon_flurryads_FlurryAdsHelper_delegateOnAdOpened(JNIEnv* env, jclass clazz, jstring adSpace)
{
    if(avalon::_flurryAdsDelegate)
    {
        std::string space = cocos2d::JniHelper::jstring2string(adSpace);
        avalon::_flurryAdsDelegate->onAdOpened(space);
    }
}

JNIEXPORT void JNICALL Java_com_avalon_flurryads_FlurryAdsHelper_delegateOnVideoCompleted(JNIEnv* env, jclass clazz, jstring adSpace)
{
    if(avalon::_flurryAdsDelegate)
    {
        std::string space = cocos2d::JniHelper::jstring2string(adSpace);
        avalon::_flurryAdsDelegate->onVideoCompleted(space);
    }
}

}




