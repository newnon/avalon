#include "avalon/MoPub.h"

#include <jni.h>
#include "platform/android/jni/JniHelper.h"
#include <android/log.h>
#include <string>
#include <map>

//const char* const INTERSTITIAL_CLASS_NAME = "com/mopub/mobileads/MoPubInterstitial";
const char* const HELPER_CLASS_NAME = "com/avalon/mopub/MoPubHelper";

namespace avalon {

class AndroidMPInterstitialAdController: public MPInterstitialAdController
{
public:
    AndroidMPInterstitialAdController(const std::string &adUnitId):_delegate(nullptr),_interstitial(nullptr),_adUnitId(adUnitId)
    {
        cocos2d::JniMethodInfo methodInfo;
        if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "createInterstitial", "(Ljava/lang/String;)Lcom/mopub/mobileads/MoPubInterstitial;"))
        {
            jstring jAdUnitID = methodInfo.env->NewStringUTF(adUnitId.c_str());
            jobject interstitial = methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID, jAdUnitID);
            methodInfo.env->DeleteLocalRef(methodInfo.classID);
            methodInfo.env->DeleteLocalRef(jAdUnitID);
            if(interstitial)
                _interstitial = cocos2d::JniHelper::getEnv()->NewGlobalRef(interstitial);
        }
    }
    
    virtual void setDelegate(MPInterstitialAdControllerDelegate *delegate) override
    {
        _delegate = delegate;
        cocos2d::JniMethodInfo methodInfo;
        if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "setDelegate", "(Lcom/mopub/mobileads/MoPubInterstitial;J)V"))
        {
            methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, _interstitial, (jlong)this);
            methodInfo.env->DeleteLocalRef(methodInfo.classID);
        }
    }
    
    virtual MPInterstitialAdControllerDelegate* getDelegate() const override
    {
        return _delegate;
    }
    
    virtual const std::string& getAdUnitId() const override
    {
        return _adUnitId;
    }
    
    virtual void setLocation(float latitude, float longitude, float accuracyInMeters) override
    {
        //[_adController setLocation:[[[CLLocation alloc] initWithLatitude:latitude longitude:longitude] autorelease]];
    }
    
    virtual void setKeywords(const std::string& keywords) override
    {
        cocos2d::JniMethodInfo methodInfo;
        if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "setKeywords", "(Lcom/mopub/mobileads/MoPubInterstitial;Ljava/lang/String;)V"))
        {
            jstring jKeywords = methodInfo.env->NewStringUTF(keywords.c_str());
            methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, _interstitial, jKeywords);
            methodInfo.env->DeleteLocalRef(methodInfo.classID);
            methodInfo.env->DeleteLocalRef(jKeywords);
        }
    }
    
    virtual void setTesting(bool testing) override
    {
        cocos2d::JniMethodInfo methodInfo;
        if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "setTesting", "(Lcom/mopub/mobileads/MoPubInterstitial;Z)V"))
        {
            methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, _interstitial, (jboolean)testing);
            methodInfo.env->DeleteLocalRef(methodInfo.classID);
        }
    }

    virtual void loadAd() override
    {
        cocos2d::JniMethodInfo methodInfo;
        if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "loadInterstitial", "(Lcom/mopub/mobileads/MoPubInterstitial;)V"))
        {
            methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, _interstitial);
            methodInfo.env->DeleteLocalRef(methodInfo.classID);
        }
    }
    
    virtual bool getReady() const override
    {
        cocos2d::JniMethodInfo methodInfo;
        bool ret = false;
        if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "getInterstitialReady", "(Lcom/mopub/mobileads/MoPubInterstitial;)Z"))
        {
            ret = methodInfo.env->CallStaticBooleanMethod(methodInfo.classID, methodInfo.methodID, _interstitial);
            methodInfo.env->DeleteLocalRef(methodInfo.classID);
        }
        return ret;
    }
    
    virtual void show() override
    {
        cocos2d::JniMethodInfo methodInfo;
        if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "showInterstitial", "(Lcom/mopub/mobileads/MoPubInterstitial;)V"))
        {
            methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, _interstitial);
            methodInfo.env->DeleteLocalRef(methodInfo.classID);
        }
    }
    
    virtual ~AndroidMPInterstitialAdController()
    {
        _sharedInterstitialAdControllers.erase(_adUnitId);
        cocos2d::JniMethodInfo methodInfo;
        if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "RemoveInterstitial", "(Lcom/mopub/mobileads/MoPubInterstitial;)V"))
        {
            methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, _interstitial);
            methodInfo.env->DeleteLocalRef(methodInfo.classID);
        }
    }
    
private:
    std::string _adUnitId;
    MPInterstitialAdControllerDelegate *_delegate;
    jobject _interstitial;
};

MPInterstitialAdController *MPInterstitialAdController::interstitialAdControllerForAdUnitId(const std::string &adUnitId)
{
    auto it = _sharedInterstitialAdControllers.find(adUnitId);
    if(it!=_sharedInterstitialAdControllers.end())
        return it->second;

    AndroidMPInterstitialAdController *controller = new AndroidMPInterstitialAdController(adUnitId);
    return _sharedInterstitialAdControllers.insert(std::make_pair(adUnitId, controller)).first->second;
    return nullptr;
}
    
std::map<std::string, MPInterstitialAdController*> MPInterstitialAdController::_sharedInterstitialAdControllers;

}

extern "C" {

JNIEXPORT void JNICALL Java_com_avalon_mopub_MoPubHelper_delegateInterstitialLoaded(JNIEnv* env, jclass clazz, jlong object)
{
    avalon::AndroidMPInterstitialAdController* controller = (avalon::AndroidMPInterstitialAdController*)object;
    controller->getDelegate()->interstitialDidLoadAd(controller);
}
JNIEXPORT void JNICALL Java_com_avalon_mopub_MoPubHelper_delegateInterstitialFailed(JNIEnv* env, jclass clazz, jlong object, jint error)
{
    avalon::AndroidMPInterstitialAdController* controller = (avalon::AndroidMPInterstitialAdController*)object;
    controller->getDelegate()->interstitialDidFailToLoadAd(controller);
}
JNIEXPORT void JNICALL Java_com_avalon_mopub_MoPubHelper_delegateInterstitialShown(JNIEnv* env, jclass clazz, jlong object)
{
    avalon::AndroidMPInterstitialAdController* controller = (avalon::AndroidMPInterstitialAdController*)object;
    controller->getDelegate()->interstitialDidAppear(controller);
}
JNIEXPORT void JNICALL Java_com_avalon_mopub_MoPubHelper_delegateInterstitialClicked(JNIEnv* env, jclass clazz, jlong object)
{
    avalon::AndroidMPInterstitialAdController* controller = (avalon::AndroidMPInterstitialAdController*)object;
    controller->getDelegate()->interstitialClicked(controller);
}
JNIEXPORT void JNICALL Java_com_avalon_mopub_MoPubHelper_delegateInterstitialDismissed(JNIEnv* env, jclass clazz, jlong object)
{
    avalon::AndroidMPInterstitialAdController* controller = (avalon::AndroidMPInterstitialAdController*)object;
    controller->getDelegate()->interstitialDidDisappear(controller);
}

}
