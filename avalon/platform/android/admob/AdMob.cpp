#include "avalon/AdMob.h"

#include <jni.h>
#include "platform/android/jni/JniHelper.h"
#include <android/log.h>
#include <string>
#include <map>

const char* const HELPER_CLASS_NAME = "com/avalon/admob/AdMobHelper";

namespace avalon {

class AndroidGADInterstitial:public GADInterstitial
{
public:

    AndroidGADInterstitial(jobject object, const std::string &adUnitID, GADInterstitialDelegate *delegate):GADInterstitial(adUnitID),_interstitial(nullptr),_delegate(delegate)
    {
        if(object)
        {
            _interstitial = cocos2d::JniHelper::getEnv()->NewGlobalRef(object);
            cocos2d::JniMethodInfo methodInfo;
            if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "addInterstitialDelegate", "(Lcom/google/android/gms/ads/InterstitialAd;J)V"))
            {
                methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, _interstitial, (jlong)this);
                methodInfo.env->DeleteLocalRef(methodInfo.classID);
            }
        }
    }

    virtual bool isReady() const
    {
        if(!_interstitial)
            return false;
        cocos2d::JniMethodInfo methodInfo;
        bool ret = false;
        if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "isInterstitialReady", "(Lcom/google/android/gms/ads/InterstitialAd;)Z"))
        {
            ret = methodInfo.env->CallStaticBooleanMethod(methodInfo.classID, methodInfo.methodID, _interstitial);
            methodInfo.env->DeleteLocalRef(methodInfo.classID);
        }
        return ret;
    }
    virtual void show() override
    {
        if(_interstitial)
        {
            cocos2d::JniMethodInfo methodInfo;
            bool ret = false;
            if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "showInterstitial", "(Lcom/google/android/gms/ads/InterstitialAd;)V"))
            {
                methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, _interstitial);
                methodInfo.env->DeleteLocalRef(methodInfo.classID);
            }
        }
    }

    virtual ~AndroidGADInterstitial()
    {
        if(_interstitial)
        {
            cocos2d::JniMethodInfo methodInfo;
            if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "addInterstitialDelegate", "(Lcom/google/android/gms/ads/InterstitialAd;J)V"))
            {
                methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, _interstitial, (jlong)0);
                methodInfo.env->DeleteLocalRef(methodInfo.classID);
            }
            cocos2d::JniHelper::getEnv()->DeleteLocalRef(_interstitial);
        }
    }

    void interstitialDidReceiveAd()
    {
        if(_delegate)
            _delegate->interstitialDidReceiveAd(this);
    }
    void interstitialDidFailToReceiveAd(const std::string &error)
    {
        if(_delegate)
            _delegate->interstitialDidFailToReceiveAd(this, error);
        avalon::AdMob::getInstance()->removeInterstitial(this);
    }
    void interstitialWillPresentScreen()
    {
        if(_delegate)
            _delegate->interstitialWillPresentScreen(this);
    }
    void interstitialWillDismissScreen()
    {
        if(_delegate)
            _delegate->interstitialWillDismissScreen(this);
        avalon::AdMob::getInstance()->removeInterstitial(this);
    }
    void interstitialWillLeaveApplication()
    {
        if(_delegate)
            _delegate->interstitialWillLeaveApplication(this);
        avalon::AdMob::getInstance()->removeInterstitial(this);
    }

private:
    jobject _interstitial;
    GADInterstitialDelegate *_delegate;
};


class AndroidGADBannerView:public GADBannerView
{
public:
    AndroidGADBannerView(jobject object, const std::string &adUnitID, GADAdSize size, GADBannerViewDelegate *delegate):GADBannerView(adUnitID,size),_bannerView(nullptr),_delegate(delegate)
    {
        if(object)
        {
            _bannerView = cocos2d::JniHelper::getEnv()->NewGlobalRef(object);
            cocos2d::JniMethodInfo methodInfo;
            if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "addAdViewDelegate", "(Lcom/google/android/gms/ads/AdView;J)V"))
            {
                methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, _bannerView, (jlong)this);
                methodInfo.env->DeleteLocalRef(methodInfo.classID);
            }
        }
    }

    virtual ~AndroidGADBannerView()
    {
        if(_bannerView)
        {
            cocos2d::JniMethodInfo methodInfo;
            if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "addAdViewDelegate", "(Lcom/google/android/gms/ads/AdView;J)V"))
            {
                methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, _bannerView, (jlong)0);
                methodInfo.env->DeleteLocalRef(methodInfo.classID);
            }
            cocos2d::JniHelper::getEnv()->DeleteLocalRef(_bannerView);
        }
    }

    void adViewDidReceiveAd()
    {
        if(_delegate)
            _delegate->adViewDidReceiveAd(this);
    }
    void adViewDidFailToReceive(const std::string &error)
    {
        if(_delegate)
            _delegate->adViewDidFailToReceive(this, error);
        avalon::AdMob::getInstance()->removeBanner(this);
    }
    void adViewWillPresentScreen()
    {
        if(_delegate)
            _delegate->adViewWillPresentScreen(this);
    }
    void adViewWillDismissScreen()
    {
        if(_delegate)
            _delegate->adViewWillDismissScreen(this);
        avalon::AdMob::getInstance()->removeBanner(this);
    }
    void adViewWillLeaveApplication()
    {
        if(_delegate)
            _delegate->adViewWillLeaveApplication(this);
        avalon::AdMob::getInstance()->removeBanner(this);
    }

private:
    jobject _bannerView;
    GADBannerViewDelegate *_delegate;
};


class AndroidAdMob : public AdMob
{
public:

    AndroidAdMob(const std::string &version):AdMob(version) {}

    virtual void setAdNetworkExtras(GADAdNetworkExtras network, const std::map<std::string,std::string> &extras) override
    {
    }
    void setTestDevices(const std::vector<std::string>& devices) override
    {
    }
    void setGender(GADGender gender) override
    {
    }
    void setBirthDate(unsigned month, unsigned day, unsigned year) override
    {
    }
    void setLocation(float latitude, float longitude, float accuracyInMeters) override
    {
    }
    void setLocation(const std::string &location) override
    {
    }
    void setTagForChildDirectedTreatment(bool value) override
    {
    }
    void setKeywords(const std::vector<std::string>& keywords) override
    {
    }

    std::shared_ptr<GADInterstitial> createIntestitial(const std::string &adUnitID, GADInterstitialDelegate *delegate) override
    {
        cocos2d::JniMethodInfo methodInfo;

        if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "createInterstitial", "(Ljava/lang/String;)Lcom/google/android/gms/ads/InterstitialAd;"))
        {
            jstring jAdUnitID = methodInfo.env->NewStringUTF(adUnitID.c_str());
            jobject interstitial = methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID, jAdUnitID);
            methodInfo.env->DeleteLocalRef(jAdUnitID);
            methodInfo.env->DeleteLocalRef(methodInfo.classID);
            _interstitials.emplace_back(new AndroidGADInterstitial(interstitial, adUnitID, delegate));
        }
        else
        {
            _interstitials.emplace_back(new AndroidGADInterstitial(nullptr, adUnitID, delegate));
        }
        return _interstitials.back();
    }
    std::shared_ptr<GADBannerView> createBannerView(const std::string &adUnitID, GADAdSize size, int x, int y, int width, int height, GADBannerViewDelegate *delegate) override
    {
        cocos2d::JniMethodInfo methodInfo;

        if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "createBannerView", "(Ljava/lang/String;IIIII)Lcom/google/android/gms/ads/AdView;"))
        {
            jstring jAdUnitID = methodInfo.env->NewStringUTF(adUnitID.c_str());
            jobject bannerView = methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID, jAdUnitID, (jint)size, x, y, width, height);
            methodInfo.env->DeleteLocalRef(jAdUnitID);
            methodInfo.env->DeleteLocalRef(methodInfo.classID);
            _bannerViews.emplace_back(new AndroidGADBannerView(bannerView, adUnitID, size, delegate));
        }
        else
        {
            _bannerViews.emplace_back(new AndroidGADBannerView(nullptr, adUnitID, size, delegate));
        }
        return _bannerViews.back();
    }

    virtual std::vector<std::shared_ptr<GADInterstitial>> getReadyInterstitials() const
    {
        std::vector<std::shared_ptr<GADInterstitial>> ret;
        for(auto &it : _interstitials)
        {
            if(it && it->isReady())
                ret.push_back(it);

        }
        return ret;
    }
    virtual std::vector<std::shared_ptr<GADBannerView>> getBannerViews() const
    {
        std::vector<std::shared_ptr<GADBannerView>> ret;
        for(auto &it : _bannerViews)
        {
            ret.push_back(it);
        }
        return ret;

    }

    virtual void removeBanner(const GADBannerView *bannerView) override
    {
        for (auto it = _bannerViews.begin();it!=_bannerViews.end();++it)
        {
            if(it->get() == bannerView)
            {
                _bannerViews.erase(it);
                break;
            }
        }
    }
    virtual void removeInterstitial(const GADInterstitial *interstitial) override
    {
        for (auto it = _interstitials.begin();it!=_interstitials.end();++it)
        {
            if(it->get() == interstitial)
            {
                _interstitials.erase(it);
                break;
            }
        }
    }

private:
    std::list<std::shared_ptr<GADBannerView>> _bannerViews;
    std::list<std::shared_ptr<GADInterstitial>> _interstitials;
};

AdMob *AdMob::getInstance()
{
    static AndroidAdMob *instance = new AndroidAdMob("");
    return instance;
}

}

extern "C" {

JNIEXPORT void JNICALL Java_com_avalon_admob_AdMobHelper_delegateAdViewDidReceiveAd(JNIEnv* env, jclass clazz, jlong object)
{
    ((avalon::AndroidGADBannerView*)object)->adViewDidReceiveAd();
}
JNIEXPORT void JNICALL Java_com_avalon_admob_AdMobHelper_delegateAdViewDidFailToReceiveAd(JNIEnv* env, jclass clazz, jlong object, jstring error)
{
    std::string errorStr = cocos2d::JniHelper::jstring2string(error);
    ((avalon::AndroidGADBannerView*)object)->adViewDidFailToReceive(errorStr);
}
JNIEXPORT void JNICALL Java_com_avalon_admob_AdMobHelper_delegateAdViewWillPresentScreen(JNIEnv* env, jclass clazz, jlong object)
{
    ((avalon::AndroidGADBannerView*)object)->adViewWillPresentScreen();
}
JNIEXPORT void JNICALL Java_com_avalon_admob_AdMobHelper_delegateAdViewWillDismissScreen(JNIEnv* env, jclass clazz, jlong object)
{
    ((avalon::AndroidGADBannerView*)object)->adViewWillDismissScreen();
}
JNIEXPORT void JNICALL Java_com_avalon_admob_AdMobHelper_delegateAdViewWillLeaveApplication(JNIEnv* env, jclass clazz, jlong object)
{
    ((avalon::AndroidGADBannerView*)object)->adViewWillLeaveApplication();
}

JNIEXPORT void JNICALL Java_com_avalon_admob_AdMobHelper_delegateInterstitialDidReceiveAd(JNIEnv* env, jclass clazz, jlong object)
{
    ((avalon::AndroidGADInterstitial*)object)->interstitialDidReceiveAd();
}
JNIEXPORT void JNICALL Java_com_avalon_admob_AdMobHelper_delegateInterstitialDidFailToReceiveAd(JNIEnv* env, jclass clazz, jlong object, jstring error)
{
    std::string errorStr = cocos2d::JniHelper::jstring2string(error);
    ((avalon::AndroidGADInterstitial*)object)->interstitialDidFailToReceiveAd(errorStr);
}
JNIEXPORT void JNICALL Java_com_avalon_admob_AdMobHelper_delegateInterstitialWillPresentScreen(JNIEnv* env, jclass clazz, jlong object)
{
    ((avalon::AndroidGADInterstitial*)object)->interstitialWillPresentScreen();
}
JNIEXPORT void JNICALL Java_com_avalon_admob_AdMobHelper_delegateInterstitialWillDismissScreen(JNIEnv* env, jclass clazz, jlong object)
{
    ((avalon::AndroidGADInterstitial*)object)->interstitialWillDismissScreen();
}
JNIEXPORT void JNICALL Java_com_avalon_admob_AdMobHelper_delegateInterstitialWillLeaveApplication(JNIEnv* env, jclass clazz, jlong object)
{
    ((avalon::AndroidGADInterstitial*)object)->interstitialWillLeaveApplication();
}
}
