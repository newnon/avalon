#include "avalon/FacebookAds.h"

#include <jni.h>
#include "platform/android/jni/JniHelper.h"
#include <android/log.h>
#include <string>
#include <map>

namespace avalon {
class FBAndroidInterstitial;
class FBAndroidBanner;

//com.facebook.ads.AdSettings
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
    
    FBAndroidInterstitial(const std::string &placementID, avalon::InterstitialDelegate *delegate):FBInterstitial(placementID),_visible(false)
    {
    }
    ~FBAndroidInterstitial()
    {
    }
    
    virtual bool isReady() const override
    {
        return false;
    }
    
    virtual bool isVisible() const override
    {
        return _visible;
    }
    
    virtual bool hide() override
    {
        return false;
    }
    
    virtual bool show() override
    {
        return _visible;
    }
    
    void loadAd()
    {
    }
    
    void setVisible(bool value)
    {
        _visible = value;
    }
    
private:;
    bool _visible;
};

class FBAndroidBanner:public FBBanner
{
public:
    FBAndroidBanner(const std::string &placementID, FBAdSize size, BannerDelegate *delegate):FBBanner(placementID, size),_ready(false)
    {
    }
    
    ~FBAndroidBanner()
    {
    }
    
    virtual bool isReady() const override
    {
        return _ready;
    }
    
    virtual bool isVisible() const override
    {
        return false;
    };
    
    virtual bool show(int x, int y, int width, int height, BannerScaleType scaleType, BannerGravityType gravity) override
    {
        return false;
    }
    
    virtual bool show(BannerScaleType scaleType, BannerGravityType gravity) override
    {
        return false;
    }
    
    virtual bool hide() override
    {
        return false;
    }
    
    void loadAd()
    {
    }
    
    void setReady(bool value)
    {
        _ready = value;
    }
    
private:
    bool _ready;
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
