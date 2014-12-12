#include "avalon/FacebookAds.h"

#include <jni.h>
#include "platform/android/jni/JniHelper.h"
#include <android/log.h>
#include <string>
#include <map>

namespace avalon {

const char* const HELPER_CLASS_NAME = "com/avalon/facebookads/FacebookAdsHelper";
const char* const SETTINGS_CLASS_NAME = "com/facebook/ads/AdSettings";

static jobject jobjectFromVector(const std::vector<std::string> &vector)
{
    JNIEnv* env = cocos2d::JniHelper::getEnv();
    jclass clsString = env->FindClass("java/lang/String");
    jobjectArray stringArray = env->NewObjectArray(vector.size(), clsString, 0);

    for(size_t i = 0; i < vector.size(); ++i)
    {
        jstring tmp = env->NewStringUTF(vector[i].c_str());
        env->SetObjectArrayElement(stringArray, i, tmp);
        env->DeleteLocalRef(tmp);
    }

    env->DeleteLocalRef(clsString);
    return stringArray;
}

class FBAndroidInterstitial:public FBInterstitial
{
public:
    
    FBAndroidInterstitial(const std::string &placementID, InterstitialDelegate *delegate):FBInterstitial(placementID),_interstitial(nullptr),_delegate(delegate)
    {
        cocos2d::JniMethodInfo methodInfo;
        if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "createInterstitial", "(Ljava/lang/String;J)Lcom/avalon/facebookads/FacebookAdsHelper$FBInterstitialAd;"))
        {
            jstring jAdUnitID = methodInfo.env->NewStringUTF(placementID.c_str());
            jobject object = methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID, jAdUnitID, (jlong)this);
            methodInfo.env->DeleteLocalRef(jAdUnitID);
            methodInfo.env->DeleteLocalRef(methodInfo.classID);
            _interstitial = cocos2d::JniHelper::getEnv()->NewGlobalRef(object);
        }
    }
    ~FBAndroidInterstitial()
    {
        if(!_interstitial)
            return;
        cocos2d::JniMethodInfo methodInfo;
        if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "interstitialDone", "(Lcom/avalon/facebookads/FacebookAdsHelper$FBInterstitialAd;)V"))
        {
            methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, _interstitial);
            methodInfo.env->DeleteLocalRef(methodInfo.classID);
        }
        cocos2d::JniHelper::getEnv()->DeleteGlobalRef(_interstitial);
    }
    
    virtual bool isReady() const override
    {
        if(!_interstitial)
            return false;
        bool ret = false;
        cocos2d::JniMethodInfo methodInfo;
        if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "interstitialIsReady", "(Lcom/avalon/facebookads/FacebookAdsHelper$FBInterstitialAd;)Z"))
        {
            ret = methodInfo.env->CallStaticBooleanMethod(methodInfo.classID, methodInfo.methodID, _interstitial);
            methodInfo.env->DeleteLocalRef(methodInfo.classID);
        }
        return ret;
    }
    
    virtual bool isVisible() const override
	{
		if(!_interstitial)
			return false;
		bool ret = false;
		cocos2d::JniMethodInfo methodInfo;
		if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "interstitialIsVisible", "(Lcom/avalon/facebookads/FacebookAdsHelper$FBInterstitialAd;)Z"))
		{
			ret = methodInfo.env->CallStaticBooleanMethod(methodInfo.classID, methodInfo.methodID, _interstitial);
			methodInfo.env->DeleteLocalRef(methodInfo.classID);
		}
		return ret;
	};
    
    virtual bool hide() override
    {
        return false;
    }
    
    virtual bool show() override
    {
        if(!_interstitial)
            return false;
        bool ret = false;
        cocos2d::JniMethodInfo methodInfo;
        if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "interstitialShow", "(Lcom/avalon/facebookads/FacebookAdsHelper$FBInterstitialAd;)Z"))
        {
        	ret = methodInfo.env->CallStaticBooleanMethod(methodInfo.classID, methodInfo.methodID, _interstitial);
            methodInfo.env->DeleteLocalRef(methodInfo.classID);
        }
        return ret;
    }
    
    virtual void interstitialReceiveAd()
    {
    	if(_delegate)
    	    _delegate->interstitialReceiveAd(this);
    }
    virtual void interstitialFailedToReceiveAd(int code, int nativeCode, const std::string &message)
    {
    	if(_delegate)
    	    _delegate->interstitialFailedToReceiveAd(this, static_cast<avalon::AdsErrorCode>(code), nativeCode, message);
    }
    virtual void interstitialClose()
    {
    	if(_delegate)
    	    _delegate->interstitialClose(this);
    }
    virtual void interstitialClick()
    {
    	if(_delegate)
    	    _delegate->interstitialClick(this);
    }

private:;
    jobject _interstitial;
    InterstitialDelegate *_delegate;
};

class FBAndroidBanner:public FBBanner
{
public:
    FBAndroidBanner(const std::string &placementID, FBAdSize size, BannerDelegate *delegate):FBBanner(placementID, size),_banner(nullptr),_delegate(delegate)
    {
    	cocos2d::JniMethodInfo methodInfo;
		if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "createBanner", "(Ljava/lang/String;IJ)Lcom/avalon/facebookads/FacebookAdsHelper$FBBanner;"))
		{
			jstring jAdUnitID = methodInfo.env->NewStringUTF(placementID.c_str());
			jobject object = methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID, jAdUnitID, (jint)size, (jlong)this);
			methodInfo.env->DeleteLocalRef(jAdUnitID);
			methodInfo.env->DeleteLocalRef(methodInfo.classID);
			_banner = cocos2d::JniHelper::getEnv()->NewGlobalRef(object);
		}
    }
    
    ~FBAndroidBanner()
    {
        if(!_banner)
            return;
        cocos2d::JniMethodInfo methodInfo;
        if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "bannerDone", "(Lcom/avalon/facebookads/FacebookAdsHelper$FBBanner;)V"))
        {
            methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, _banner);
            methodInfo.env->DeleteLocalRef(methodInfo.classID);
        }
        cocos2d::JniHelper::getEnv()->DeleteGlobalRef(_banner);
    }
    
    virtual bool isReady() const override
    {
        if(!_banner)
            return false;
        bool ret = false;
        cocos2d::JniMethodInfo methodInfo;
        if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "bannerIsReady", "(Lcom/avalon/facebookads/FacebookAdsHelper$FBBanner;)Z"))
        {
            ret = methodInfo.env->CallStaticBooleanMethod(methodInfo.classID, methodInfo.methodID, _banner);
            methodInfo.env->DeleteLocalRef(methodInfo.classID);
        }
        return ret;
    }
    
    virtual bool isVisible() const override
    {
		if(!_banner)
			return false;
		bool ret = false;
		cocos2d::JniMethodInfo methodInfo;
		if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "bannerIsVisible", "(Lcom/avalon/facebookads/FacebookAdsHelper$FBBanner;)Z"))
		{
			ret = methodInfo.env->CallStaticBooleanMethod(methodInfo.classID, methodInfo.methodID, _banner);
			methodInfo.env->DeleteLocalRef(methodInfo.classID);
		}
		return ret;
    };
    
    virtual bool show(int x, int y, int width, int height, BannerScaleType scaleType, BannerGravityType gravity) override
    {
    	if(!_banner)
			return false;
    	bool ret = false;
		cocos2d::JniMethodInfo methodInfo;
		if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "bannerShow", "(Lcom/avalon/facebookads/FacebookAdsHelper$FBBanner;IIIIII)Z"))
		{
			ret = methodInfo.env->CallStaticBooleanMethod(methodInfo.classID, methodInfo.methodID, _banner, (jint)x, (jint)y, (jint)width, (jint)height, (jint)scaleType, (jint)gravity);
			methodInfo.env->DeleteLocalRef(methodInfo.classID);
		}
		return ret;
    }
    
    virtual bool show(BannerScaleType scaleType, BannerGravityType gravity) override
    {
    	return show(0, 0, 0, 0, scaleType, gravity);
    }
    
    virtual bool hide() override
    {
    	if(!_banner)
			return false;
		bool ret = false;
		cocos2d::JniMethodInfo methodInfo;
		if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "bannerHide", "(Lcom/avalon/facebookads/FacebookAdsHelper$FBBanner;)Z"))
		{
			ret = methodInfo.env->CallStaticBooleanMethod(methodInfo.classID, methodInfo.methodID, _banner);
			methodInfo.env->DeleteLocalRef(methodInfo.classID);
		}
		return ret;
    }
    
    virtual void bannerReceiveAd()
    {
    	if(_delegate)
    		_delegate->bannerReceiveAd(this);
    }
    virtual void bannerClick()
    {
    	if(_delegate)
    	    _delegate->bannerClick(this);
    }
    virtual void bannerFailedToReceiveAd(int code, int nativeCode, const std::string &message)
    {
    	if(_delegate)
    	    _delegate->bannerFailedToReceiveAd(this, static_cast<avalon::AdsErrorCode>(code), nativeCode, message);
    }
private:
    jobject _banner;
    BannerDelegate *_delegate;
};

class FBAndroidAds: public FBAds
{
public:
    
    virtual const std::string& getVersion() const override
    {
        return _version;
    }
    
    virtual FBInterstitial* createInterstitial(const std::string &placementID, InterstitialDelegate *delegate) override
    {
        return new FBAndroidInterstitial(placementID, delegate);
    }
    virtual FBBanner* createBanner(const std::string &placementID, FBAdSize adSize, BannerDelegate *delegate) override
    {
        return new FBAndroidBanner(placementID, adSize, delegate);
    }

    virtual void addTestDevice(const std::string &deviceHash) override
    {
        cocos2d::JniMethodInfo methodInfo;
        if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,SETTINGS_CLASS_NAME, "addTestDevice", "(Ljava/lang/String;)V"))
        {
            jstring jDeviceHash = methodInfo.env->NewStringUTF(deviceHash.c_str());
            methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jDeviceHash);
            methodInfo.env->DeleteLocalRef(methodInfo.classID);
            methodInfo.env->DeleteLocalRef(jDeviceHash);
        }
    }

    virtual void addTestDevices(const std::vector<std::string> &deviceHash) override
    {
        cocos2d::JniMethodInfo methodInfo;
        if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,SETTINGS_CLASS_NAME, "addTestDevices", "(Ljava/util/Collection;)V"))
        {
            jobject jDevicesHash = jobjectFromVector(deviceHash);
            methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jDevicesHash);
            methodInfo.env->DeleteLocalRef(methodInfo.classID);
            methodInfo.env->DeleteLocalRef(jDevicesHash);
        }
    }

    virtual void clearTestDevices() override
    {
        cocos2d::JniMethodInfo methodInfo;
        if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,SETTINGS_CLASS_NAME, "clearTestDevices", "()V"))
        {
            methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID);
            methodInfo.env->DeleteLocalRef(methodInfo.classID);
        }
    }

    virtual void setIsChildDirected(bool isChildDirected) override
    {
        cocos2d::JniMethodInfo methodInfo;
        if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,SETTINGS_CLASS_NAME, "setIsChildDirected", "(Z)V"))
        {
            methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, (jboolean)isChildDirected);
            methodInfo.env->DeleteLocalRef(methodInfo.classID);
        }
    }
    
    FBAndroidAds(const std::string &version):_version(version)
    {
    }
    
    
private:
    std::string _version;
};

FBAds *FBAds::getInstance()
{
    std::string version;
    cocos2d::JniMethodInfo methodInfo;
    if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "getSdkVersion", "()Ljava/lang/String;"))
    {
        jstring str = (jstring)methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
        version = cocos2d::JniHelper::jstring2string(str);
        methodInfo.env->DeleteLocalRef(str);
    }
    static FBAndroidAds *instance = new FBAndroidAds(version);
    return instance;
}
    
}

extern "C" {

JNIEXPORT void JNICALL Java_com_avalon_facebookads_FacebookAdsHelper_delegateBannerOnError(JNIEnv* env, jclass clazz, jlong object, jint code, jint nativeCode, jstring descriptrion)
{
    ((avalon::FBAndroidBanner*)object)->bannerFailedToReceiveAd(code, nativeCode, cocos2d::JniHelper::jstring2string(descriptrion));
}

JNIEXPORT void JNICALL Java_com_avalon_facebookads_FacebookAdsHelper_delegateBannerOnLoaded(JNIEnv* env, jclass clazz, jlong object)
{
    ((avalon::FBAndroidBanner*)object)->bannerReceiveAd();
}

JNIEXPORT void JNICALL Java_com_avalon_facebookads_FacebookAdsHelper_delegateBannerOnClicked(JNIEnv* env, jclass clazz, jlong object)
{
    ((avalon::FBAndroidBanner*)object)->bannerClick();
}


JNIEXPORT void JNICALL Java_com_avalon_facebookads_FacebookAdsHelper_delegateInterstitialOnError(JNIEnv* env, jclass clazz, jlong object, jint code, jint nativeCode, jstring descriptrion)
{
    ((avalon::FBAndroidInterstitial*)object)->interstitialFailedToReceiveAd(code, nativeCode, cocos2d::JniHelper::jstring2string(descriptrion));
}

JNIEXPORT void JNICALL Java_com_avalon_facebookads_FacebookAdsHelper_delegateInterstitialOnLoaded(JNIEnv* env, jclass clazz, jlong object)
{
    ((avalon::FBAndroidInterstitial*)object)->interstitialReceiveAd();
}

JNIEXPORT void JNICALL Java_com_avalon_facebookads_FacebookAdsHelper_delegateInterstitialOnClicked(JNIEnv* env, jclass clazz, jlong object)
{
    ((avalon::FBAndroidInterstitial*)object)->interstitialClick();
}

JNIEXPORT void JNICALL Java_com_avalon_facebookads_FacebookAdsHelper_delegateInterstitialOnDisplayed(JNIEnv* env, jclass clazz, jlong object)
{
}

JNIEXPORT void JNICALL Java_com_avalon_facebookads_FacebookAdsHelper_delegateInterstitialOnDismissed(JNIEnv* env, jclass clazz, jlong object)
{
    ((avalon::FBAndroidInterstitial*)object)->interstitialClose();
}
}
