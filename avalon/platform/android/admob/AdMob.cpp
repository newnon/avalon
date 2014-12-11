#include "avalon/AdMob.h"

#include <jni.h>
#include "platform/android/jni/JniHelper.h"
#include <android/log.h>
#include <string>
#include <map>

const char* const HELPER_CLASS_NAME = "com/avalon/admob/AdMobHelper";

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

class AndroidGADInterstitial:public GADInterstitial
{
public:
    AndroidGADInterstitial(const std::string &adUnitID, InterstitialDelegate *delegate):GADInterstitial(adUnitID),_interstitial(nullptr),_delegate(delegate)
    {
        cocos2d::JniMethodInfo methodInfo;
        if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "createInterstitial", "(Ljava/lang/String;J)Lcom/avalon/admob/AdMobHelper$AdmobInterstitialAd;"))
        {
            jstring jAdUnitID = methodInfo.env->NewStringUTF(adUnitID.c_str());
            jobject object = methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID, jAdUnitID, (jlong)this);
            methodInfo.env->DeleteLocalRef(jAdUnitID);
            methodInfo.env->DeleteLocalRef(methodInfo.classID);
            _interstitial = cocos2d::JniHelper::getEnv()->NewGlobalRef(object);
        }
    }

    ~AndroidGADInterstitial()
    {
		if(!_interstitial)
			return;
		cocos2d::JniMethodInfo methodInfo;
		if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "interstitialDone", "(Lcom/avalon/admob/AdMobHelper$AdmobInterstitialAd;)V"))
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
		if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "interstitialIsReady", "(Lcom/avalon/admob/AdMobHelper$AdmobInterstitialAd;)Z"))
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
		if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "interstitialIsVisible", "(Lcom/avalon/admob/AdMobHelper$AdmobInterstitialAd;)Z"))
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
		if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "interstitialShow", "(Lcom/avalon/admob/AdMobHelper$AdmobInterstitialAd;)Z"))
		{
			ret = methodInfo.env->CallStaticBooleanMethod(methodInfo.classID, methodInfo.methodID, _interstitial);
			methodInfo.env->DeleteLocalRef(methodInfo.classID);
		}
		return ret;
	}

    void interstitialDidReceiveAd()
    {
        if(_delegate)
            _delegate->interstitialReceiveAd(this);
    }
    void interstitialDidFailToReceiveAd(int code, int nativeCode, const std::string &message)
    {
        if(_delegate)
            _delegate->interstitialFailedToReceiveAd(this, static_cast<avalon::AdsErrorCode>(code), nativeCode, message);
    }
    void interstitialWillPresentScreen()
    {
    }
    void interstitialWillDismissScreen()
    {
        if(_delegate)
            _delegate->interstitialClose(this);
    }
    void interstitialWillLeaveApplication()
    {
        if(_delegate)
            _delegate->interstitialClick(this);
    }

private:
    jobject _interstitial;
    InterstitialDelegate *_delegate;
};


class AndroidGADBannerView:public GADBannerView
{
public:
    AndroidGADBannerView(const std::string &adUnitID, GADAdSize size, BannerDelegate *delegate):GADBannerView(adUnitID,size),_banner(nullptr),_delegate(delegate)
    {
    	cocos2d::JniMethodInfo methodInfo;
		if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "createBanner", "(Ljava/lang/String;IJ)Lcom/avalon/admob/AdMobHelper$AdmobBanner;"))
		{
			jstring jAdUnitID = methodInfo.env->NewStringUTF(adUnitID.c_str());
			jobject object = methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID, jAdUnitID, (jint)size, (jlong)this);
			methodInfo.env->DeleteLocalRef(jAdUnitID);
			methodInfo.env->DeleteLocalRef(methodInfo.classID);
			_banner = cocos2d::JniHelper::getEnv()->NewGlobalRef(object);
		}
    }

    virtual ~AndroidGADBannerView()
    {
        if(!_banner)
            return;
        cocos2d::JniMethodInfo methodInfo;
        if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "bannerDone", "(Lcom/avalon/admob/AdMobHelper$AdmobBanner;)V"))
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
		if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "bannerIsReady", "(Lcom/avalon/admob/AdMobHelper$AdmobBanner;)Z"))
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
		if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "bannerIsVisible", "(Lcom/avalon/admob/AdMobHelper$AdmobBanner;)Z"))
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
		if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "bannerShow", "(Lcom/avalon/admob/AdMobHelper$AdmobBanner;IIIIII)Z"))
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
		if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "bannerHide", "(Lcom/avalon/admob/AdMobHelper$AdmobBanner;)Z"))
		{
			ret = methodInfo.env->CallStaticBooleanMethod(methodInfo.classID, methodInfo.methodID, _banner);
			methodInfo.env->DeleteLocalRef(methodInfo.classID);
		}
		return ret;
	}

    void adViewDidReceiveAd()
    {
        if(_delegate)
            _delegate->bannerReceiveAd(this);
    }
    void adViewDidFailToReceive(int code, int nativeCode, const std::string &message)
    {
        if(_delegate)
            _delegate->bannerFailedToReceiveAd(this, static_cast<avalon::AdsErrorCode>(code), nativeCode, message);
    }
    void adViewWillPresentScreen()
    {
    }
    void adViewWillDismissScreen()
    {
    }
    void adViewWillLeaveApplication()
    {
        if(_delegate)
            _delegate->bannerClick(this);
    }

private:
    jobject _banner;
    BannerDelegate *_delegate;
};

class AndroidAdMob : public AdMob
{
public:

    AndroidAdMob(const std::string &version):AdMob(version)
    {
    }
    virtual ~AndroidAdMob()
    {

    }
    virtual void setAdNetworkExtras(GADAdNetworkExtras network, const std::map<std::string,std::string> &extras) override
    {
    	cocos2d::JniMethodInfo methodInfo;
    	if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "setAdNetworkExtras", "(ILjava/util/Map;)V"))
		{
			jobject jExtras = jobjectFromDictionary(extras);
			methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, (int)network, jExtras);
			methodInfo.env->DeleteLocalRef(methodInfo.classID);
			methodInfo.env->DeleteLocalRef(jExtras);
		}
	}
    virtual void setTestDevices(const std::vector<std::string>& devices) override
	{
		cocos2d::JniMethodInfo methodInfo;
		if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "setTestDevices", "([Ljava/lang/String;)V"))
		{
			jobject jDevices = jobjectFromVector(devices);
			methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jDevices);
			methodInfo.env->DeleteLocalRef(methodInfo.classID);
			methodInfo.env->DeleteLocalRef(jDevices);
		}
	}
    virtual void setGender(GADGender gender) override
	{
		cocos2d::JniMethodInfo methodInfo;
		if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "setGender", "(I)V"))
		{
			methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, (int)gender);
			methodInfo.env->DeleteLocalRef(methodInfo.classID);
		}
	}
    virtual void setBirthDate(unsigned month, unsigned day, unsigned year) override
	{
		cocos2d::JniMethodInfo methodInfo;
		if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "setBirthday", "(III)V"))
		{
			methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, (jint)year, (jint)month, (jint)day);
			methodInfo.env->DeleteLocalRef(methodInfo.classID);
		}
	}
    virtual void setLocation(float latitude, float longitude, float accuracyInMeters) override
	{
		cocos2d::JniMethodInfo methodInfo;
		if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "setLocation", "(FFF)V"))
		{
			methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, (jfloat)latitude, (jfloat)longitude, (jfloat)accuracyInMeters);
			methodInfo.env->DeleteLocalRef(methodInfo.classID);
		}
	}
    virtual void setLocation(const std::string &location) override
	{

	}
    virtual void setTagForChildDirectedTreatment(bool value) override
	{
		cocos2d::JniMethodInfo methodInfo;
		if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "setTagForChildDirectedTreatment", "(Z)V"))
		{
			methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, (jboolean)value);
			methodInfo.env->DeleteLocalRef(methodInfo.classID);
		}
	}
    virtual void setKeywords(const std::vector<std::string>& keywords) override
    {
    	cocos2d::JniMethodInfo methodInfo;
    	if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "setKeyWords", "([Ljava/lang/String;)V"))
    	{
    		jobject jKeywords = jobjectFromVector(keywords);
    		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jKeywords);
    		methodInfo.env->DeleteLocalRef(methodInfo.classID);
    		methodInfo.env->DeleteLocalRef(jKeywords);
    	}
    }

    virtual GADInterstitial* createIntestitial(const std::string &adUnitID, InterstitialDelegate *delegate) override
	{
		return new AndroidGADInterstitial(adUnitID, delegate);
	}
    virtual GADBannerView* createBanner(const std::string &adUnitID, GADAdSize size, BannerDelegate *delegate) override
	{
		return new AndroidGADBannerView(adUnitID, size, delegate);
	}
};

AdMob *AdMob::getInstance()
{
    static AndroidAdMob *instance = new AndroidAdMob("");
    return instance;
}

}

extern "C" {

JNIEXPORT void JNICALL Java_com_avalon_admob_AdMobHelper_delegateBannerDidReceiveAd(JNIEnv* env, jclass clazz, jlong object)
{
    ((avalon::AndroidGADBannerView*)object)->adViewDidReceiveAd();
}
JNIEXPORT void JNICALL Java_com_avalon_admob_AdMobHelper_delegateBannerDidFailToReceiveAd(JNIEnv* env, jclass clazz, jlong object, jint code, jint nativeCode, jstring descriptrion)
{
    ((avalon::AndroidGADBannerView*)object)->adViewDidFailToReceive(code, nativeCode, cocos2d::JniHelper::jstring2string(descriptrion));
}
JNIEXPORT void JNICALL Java_com_avalon_admob_AdMobHelper_delegateBannerWillPresentScreen(JNIEnv* env, jclass clazz, jlong object)
{
    ((avalon::AndroidGADBannerView*)object)->adViewWillPresentScreen();
}
JNIEXPORT void JNICALL Java_com_avalon_admob_AdMobHelper_delegateBannerWillDismissScreen(JNIEnv* env, jclass clazz, jlong object)
{
    ((avalon::AndroidGADBannerView*)object)->adViewWillDismissScreen();
}
JNIEXPORT void JNICALL Java_com_avalon_admob_AdMobHelper_delegateBannerWillLeaveApplication(JNIEnv* env, jclass clazz, jlong object)
{
    ((avalon::AndroidGADBannerView*)object)->adViewWillLeaveApplication();
}

JNIEXPORT void JNICALL Java_com_avalon_admob_AdMobHelper_delegateInterstitialDidReceiveAd(JNIEnv* env, jclass clazz, jlong object)
{
    ((avalon::AndroidGADInterstitial*)object)->interstitialDidReceiveAd();
}
JNIEXPORT void JNICALL Java_com_avalon_admob_AdMobHelper_delegateInterstitialDidFailToReceiveAd(JNIEnv* env, jclass clazz, jlong object, jint code, jint nativeCode, jstring descriptrion)
{
    ((avalon::AndroidGADInterstitial*)object)->interstitialDidFailToReceiveAd(code, nativeCode, cocos2d::JniHelper::jstring2string(descriptrion));
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
