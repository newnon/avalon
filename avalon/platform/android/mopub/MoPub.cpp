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

class AndroidMPRewardedVideo: public MPRewardedVideo
{
public:
    virtual void initializeRewardedVideoWithGlobalMediationSettings(void */*(NSArray *)*/globalMediationSettings, MPRewardedVideoDelegate &delegate) override
    {
        _delegate = &delegate;
        cocos2d::JniMethodInfo methodInfo;
        if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo, HELPER_CLASS_NAME, "initializeRewardedVideo", "()V"))
        {
            methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID);
            methodInfo.env->DeleteLocalRef(methodInfo.classID);
        }
    }

    virtual void loadRewardedVideoAdWithAdUnitID(const std::string &adUnitID, void*/*(NSArray *)*/mediationSettings) override
    {
        cocos2d::JniMethodInfo methodInfo;
        if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo, HELPER_CLASS_NAME, "loadRewardedVideo", "(Ljava/lang/String;)V"))
        {
            jstring jAdUnitID = methodInfo.env->NewStringUTF(adUnitID.c_str());
            methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jAdUnitID);
            methodInfo.env->DeleteLocalRef(methodInfo.classID);
            methodInfo.env->DeleteLocalRef(jAdUnitID);
        }
    }

    virtual void loadRewardedVideoAdWithAdUnitID(const std::string &adUnitID, const std::string &keywords, void*/*(CLLocation *)*/ location, void*/*(NSArray *)*/mediationSettings) override
    {
        cocos2d::JniMethodInfo methodInfo;
        if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "loadRewardedVideo", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V"))
        {
            jstring jAdUnitID = methodInfo.env->NewStringUTF(adUnitID.c_str());
            jstring jKeywords = methodInfo.env->NewStringUTF(keywords.c_str());
            methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jAdUnitID, jKeywords, nullptr);
            methodInfo.env->DeleteLocalRef(methodInfo.classID);
            methodInfo.env->DeleteLocalRef(jAdUnitID);
            methodInfo.env->DeleteLocalRef(jKeywords);
        }
    }

    virtual void loadRewardedVideoAdWithAdUnitID(const std::string &adUnitID, const std::string &keywords, void*/*(CLLocation *)*/ location, const std::string &customerId, void*/*(NSArray *)*/mediationSettings) override
    {
        cocos2d::JniMethodInfo methodInfo;
        if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "loadRewardedVideo", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V"))
        {
            jstring jAdUnitID = methodInfo.env->NewStringUTF(adUnitID.c_str());
            jstring jKeywords = methodInfo.env->NewStringUTF(keywords.c_str());
            jstring jCustomerId = methodInfo.env->NewStringUTF(customerId.c_str());
            methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jAdUnitID, jKeywords, jCustomerId);
            methodInfo.env->DeleteLocalRef(methodInfo.classID);
            methodInfo.env->DeleteLocalRef(jAdUnitID);
            methodInfo.env->DeleteLocalRef(jKeywords);
            methodInfo.env->DeleteLocalRef(jCustomerId);
        }
    }

    virtual bool hasAdAvailableForAdUnitID(const std::string &adUnitID) override
    {
        cocos2d::JniMethodInfo methodInfo;
        bool ret = false;
        if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "hasRewardedVideo", "(Ljava/lang/String;)Z"))
        {
            jstring jAdUnitID = methodInfo.env->NewStringUTF(adUnitID.c_str());
            ret = methodInfo.env->CallStaticBooleanMethod(methodInfo.classID, methodInfo.methodID, jAdUnitID);
            methodInfo.env->DeleteLocalRef(methodInfo.classID);
            methodInfo.env->DeleteLocalRef(jAdUnitID);
        }
        return ret;
    }

    virtual std::vector<MPRewardedVideoReward> availableRewardsForAdUnitID(const std::string &adUnitID) override
    {
        std::vector<MPRewardedVideoReward> ret;

        cocos2d::JniMethodInfo methodInfo;
        if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo, HELPER_CLASS_NAME, "getAvailableRewards", "(Ljava/lang/String;)[Lcom/mopub/common/MoPubReward;"))
        {
            jstring jAdUnitID = methodInfo.env->NewStringUTF(adUnitID.c_str());
            jobjectArray jResult = (jobjectArray)methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID, jAdUnitID);
            methodInfo.env->DeleteLocalRef(methodInfo.classID);
            methodInfo.env->DeleteLocalRef(jAdUnitID);

            cocos2d::JniMethodInfo getLabelMethodInfo;
            cocos2d::JniMethodInfo getAmountMethodInfo;

            bool hasGetLabelMethod = cocos2d::JniHelper::getMethodInfo(getLabelMethodInfo, "com/mopub/common/MoPubReward", "getLabel", "()Ljava/lang/String;");
            bool getAmountMethod = cocos2d::JniHelper::getMethodInfo(getAmountMethodInfo, "com/mopub/common/MoPubReward", "getAmount", "()I");

            jsize keysSize = methodInfo.env->GetArrayLength(jResult);

            for (int i = 0; i < keysSize; ++i) {
                jobject element = methodInfo.env->GetObjectArrayElement(jResult, i);

                std::string curency;
                int amount = 0;

                if(hasGetLabelMethod)
                    curency = cocos2d::JniHelper::jstring2string((jstring)getLabelMethodInfo.env->CallObjectMethod(element, getLabelMethodInfo.methodID));

                if(getAmountMethod)
                    amount = getLabelMethodInfo.env->CallIntMethod(element, getAmountMethodInfo.methodID);

                ret.push_back(MPRewardedVideoReward{curency, amount});

                methodInfo.env->DeleteLocalRef(element);
            }
        }
        return ret;
    }

    virtual MPRewardedVideoReward selectedRewardForAdUnitID(const std::string &adUnitID) override
    {
        std::vector<MPRewardedVideoReward>  result = availableRewardsForAdUnitID(adUnitID);
        if(result.empty())
            return MPRewardedVideoReward{"", 0};
        else
            return result.front();
    }

    virtual void presentRewardedVideoAdForAdUnitID(const std::string &adUnitID, const MPRewardedVideoReward &reward) override
    {
        cocos2d::JniMethodInfo methodInfo;
        if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "showRewardedVideo", "(Ljava/lang/String;Ljava/lang/String;I)V"))
        {
            jstring jAdUnitID = methodInfo.env->NewStringUTF(adUnitID.c_str());
            jstring jCurrencyType = methodInfo.env->NewStringUTF(reward.currencyType.c_str());
            methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jAdUnitID, jCurrencyType, (jint)reward.amount);
            methodInfo.env->DeleteLocalRef(methodInfo.classID);
            methodInfo.env->DeleteLocalRef(jAdUnitID);
            methodInfo.env->DeleteLocalRef(jCurrencyType);
        }
    }

    MPRewardedVideoDelegate* getDelegate() const { return _delegate; }

private:
    MPRewardedVideoDelegate *_delegate = nullptr;
};

MPRewardedVideo* MPRewardedVideo::getInstance()
{
    static AndroidMPRewardedVideo instance;
    return &instance;
}

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


JNIEXPORT void JNICALL Java_com_avalon_mopub_MoPubHelper_delegateRewardedVideoLoadSuccess(JNIEnv* env, jclass clazz, jstring adUnitId)
{
    avalon::MPRewardedVideoDelegate *delegate = static_cast<avalon::AndroidMPRewardedVideo*> (avalon::MPRewardedVideo::getInstance())->getDelegate();
    if(delegate)
        delegate->rewardedVideoAdDidLoadForAdUnitID(cocos2d::JniHelper::jstring2string(adUnitId));
}

JNIEXPORT void JNICALL Java_com_avalon_mopub_MoPubHelper_delegateRewardedVideoLoadFailure(JNIEnv* env, jclass clazz, jstring adUnitId, jstring error, jint errorCode)
{
    avalon::MPRewardedVideoDelegate *delegate = static_cast<avalon::AndroidMPRewardedVideo*> (avalon::MPRewardedVideo::getInstance())->getDelegate();
    if(delegate)
        delegate->rewardedVideoAdDidFailToLoadForAdUnitID(cocos2d::JniHelper::jstring2string(adUnitId), cocos2d::JniHelper::jstring2string(error), (int)errorCode);
}

JNIEXPORT void JNICALL Java_com_avalon_mopub_MoPubHelper_delegateRewardedVideoStarted(JNIEnv* env, jclass clazz, jstring adUnitId)
{
    avalon::MPRewardedVideoDelegate *delegate = static_cast<avalon::AndroidMPRewardedVideo*> (avalon::MPRewardedVideo::getInstance())->getDelegate();
    if(delegate)
        delegate->rewardedVideoAdDidAppearForAdUnitID(cocos2d::JniHelper::jstring2string(adUnitId));
}

JNIEXPORT void JNICALL Java_com_avalon_mopub_MoPubHelper_delegateRewardedVideoPlaybackError(JNIEnv* env, jclass clazz, jstring adUnitId, jstring error, jint errorCode)
{
    avalon::MPRewardedVideoDelegate *delegate = static_cast<avalon::AndroidMPRewardedVideo*> (avalon::MPRewardedVideo::getInstance())->getDelegate();
    if(delegate)
        delegate->rewardedVideoAdDidFailToPlayForAdUnitID(cocos2d::JniHelper::jstring2string(adUnitId), cocos2d::JniHelper::jstring2string(error), (int)errorCode);
}

JNIEXPORT void JNICALL Java_com_avalon_mopub_MoPubHelper_delegateRewardedVideoClicked(JNIEnv* env, jclass clazz, jstring adUnitId)
{
    avalon::MPRewardedVideoDelegate *delegate = static_cast<avalon::AndroidMPRewardedVideo*> (avalon::MPRewardedVideo::getInstance())->getDelegate();
    if(delegate)
        delegate->rewardedVideoAdWillLeaveApplicationForAdUnitID(cocos2d::JniHelper::jstring2string(adUnitId));
}

JNIEXPORT void JNICALL Java_com_avalon_mopub_MoPubHelper_delegateRewardedVideoClosed(JNIEnv* env, jclass clazz, jstring adUnitId)
{
    avalon::MPRewardedVideoDelegate *delegate = static_cast<avalon::AndroidMPRewardedVideo*> (avalon::MPRewardedVideo::getInstance())->getDelegate();
    if(delegate)
        delegate->rewardedVideoAdDidDisappearForAdUnitID(cocos2d::JniHelper::jstring2string(adUnitId));
}

JNIEXPORT void JNICALL Java_com_avalon_mopub_MoPubHelper_delegateRewardedVideoCompleted(JNIEnv* env, jclass clazz, jstring adUnitId, jstring currencyType, int amount)
{
    avalon::MPRewardedVideoDelegate *delegate = static_cast<avalon::AndroidMPRewardedVideo*> (avalon::MPRewardedVideo::getInstance())->getDelegate();
    if(delegate)
        delegate->rewardedVideoAdShouldRewardForAdUnitID(cocos2d::JniHelper::jstring2string(adUnitId), avalon::MPRewardedVideoReward{cocos2d::JniHelper::jstring2string(currencyType), amount});
}

}
