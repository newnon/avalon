#include "avalon/AdMob.h"

#include <jni.h>
#include "platform/android/jni/JniHelper.h"
#include <android/log.h>
#include <string>
#include <map>

const char* const HELPER_CLASS_NAME = "com/avalon/admob/AdMobHelper";

namespace avalon {
class AndroidGADInterstitial;
class AndroidGADBannerView;

class AndroidAdMob : public AdMob
{
public:

    AndroidAdMob(const std::string &version);
    virtual ~AndroidAdMob();
    virtual void setAdNetworkExtras(GADAdNetworkExtras network, const std::map<std::string,std::string> &extras) override;
    virtual void setTestDevices(const std::vector<std::string>& devices) override;
    virtual void setGender(GADGender gender) override;
    virtual void setBirthDate(unsigned month, unsigned day, unsigned year) override;
    virtual void setLocation(float latitude, float longitude, float accuracyInMeters) override;
    virtual void setLocation(const std::string &location) override;
    virtual void setTagForChildDirectedTreatment(bool value) override;
    virtual void setKeywords(const std::vector<std::string>& keywords) override;

    virtual GADInterstitial* createIntestitial(const std::string &adUnitID, InterstitialDelegate *delegate) override;
    virtual GADBannerView* createBanner(const std::string &adUnitID, GADAdSize size, BannerDelegate *delegate) override;
};

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

	jobject createObject(const std::string &adUnitID)
	{
		cocos2d::JniMethodInfo methodInfo;
	    if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "createInterstitial", "(Ljava/lang/String;)Lcom/google/android/gms/ads/InterstitialAd;"))
		{
			jstring jAdUnitID = methodInfo.env->NewStringUTF(adUnitID.c_str());
			jobject interstitial = methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID, jAdUnitID);
			methodInfo.env->DeleteLocalRef(jAdUnitID);
			methodInfo.env->DeleteLocalRef(methodInfo.classID);
			return interstitial;
		}
	    return nullptr;
	}

    AndroidGADInterstitial(const std::string &adUnitID, InterstitialDelegate *delegate):GADInterstitial(adUnitID),_interstitial(nullptr),_delegate(delegate),_visible(false)
    {
    }

    virtual bool isReady() const override
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

    virtual bool isVisible() const override
	{
		return _visible;
	}

    virtual bool prepare() override
	{
		if(_interstitial && isReady())
			return false;
		if(!_interstitial)
			recreate();
		return true;
	}

    virtual bool hide() override
	{
		return false;
	}

    virtual bool show() override
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
            return true;
        }
        return false;
    }

    void recreate()
	{
		if(_interstitial)
		{
			cocos2d::JniMethodInfo methodInfo;
			if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "addInterstitialDelegate", "(Lcom/google/android/gms/ads/InterstitialAd;J)V"))
			{
				methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, _interstitial, (jlong)0);
				methodInfo.env->DeleteLocalRef(methodInfo.classID);
			}
			cocos2d::JniHelper::getEnv()->DeleteGlobalRef(_interstitial);
			_interstitial = nullptr;
		}
		jobject object = createObject(_adUnitID.c_str());
		_interstitial = cocos2d::JniHelper::getEnv()->NewGlobalRef(object);
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
            cocos2d::JniHelper::getEnv()->DeleteGlobalRef(_interstitial);
        }
    }

    void interstitialDidReceiveAd()
    {
        if(_delegate)
            _delegate->interstitialReceiveAd(this);
    }
    void interstitialDidFailToReceiveAd(int error)
    {
        if(_delegate)
            _delegate->interstitialFailedToReceiveAd(this, avalon::AdsErrorCode::INTERNAL_ERROR, error, "");
        prepare();
    }
    void interstitialWillPresentScreen()
    {
    	_visible = true;
    }
    void interstitialWillDismissScreen()
    {
        if(_delegate)
            _delegate->interstitialClose(this);
        _visible = false;
        recreate();
    }
    void interstitialWillLeaveApplication()
    {
        if(_delegate)
            _delegate->interstitialClick(this);
    }

private:
    jobject _interstitial;
    InterstitialDelegate *_delegate;
    bool _visible;
};


class AndroidGADBannerView:public GADBannerView
{
public:
	jobject createObject(const std::string &adUnitID, GADAdSize size)
	{
	    cocos2d::JniMethodInfo methodInfo;
		if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "createBannerView", "(Ljava/lang/String;I)Lcom/google/android/gms/ads/AdView;"))
		{
			jstring jAdUnitID = methodInfo.env->NewStringUTF(adUnitID.c_str());
			jobject bannerView = methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID, jAdUnitID, (jint)size);
			methodInfo.env->DeleteLocalRef(jAdUnitID);
			methodInfo.env->DeleteLocalRef(methodInfo.classID);
			return bannerView;
		}
		return nullptr;
	}

    AndroidGADBannerView(const std::string &adUnitID, GADAdSize size, BannerDelegate *delegate):GADBannerView(adUnitID,size),_bannerView(nullptr),_delegate(delegate),_ready(false)
    {
    	jobject object = createObject(adUnitID, size);
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
            cocos2d::JniHelper::getEnv()->DeleteGlobalRef(_bannerView);
        }
    }
    
    virtual bool show(int x, int y, int width, int height, BannerScaleType scaleType, BannerGravityType gravity) override
    {
        if(_bannerView)
        {
            cocos2d::JniMethodInfo methodInfo;
            if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "showBanner", "(Lcom/google/android/gms/ads/AdView;IIIIII)V"))
            {
                methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, _bannerView, x, y, width, height, static_cast<int>(scaleType), static_cast<int>(gravity));
                methodInfo.env->DeleteLocalRef(methodInfo.classID);
            }
            return true;
        }
        return false;
    }

    virtual bool show(BannerScaleType scaleType, BannerGravityType gravity) override
    {
    	return show(0,0,0,0,scaleType,gravity);
    }
        
    virtual bool hide() override
    {
    	if(!isVisible())
    		return false;
        if(_bannerView)
        {
            cocos2d::JniMethodInfo methodInfo;
            if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "hideBanner", "(Lcom/google/android/gms/ads/AdView;)V"))
            {
                methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, _bannerView);
                methodInfo.env->DeleteLocalRef(methodInfo.classID);
            }
        }
    }
    
    virtual bool isReady() const override
    {
    	return _ready;
    }

    virtual bool isVisible() const override
    {
        if(!_bannerView)
            return false;
        cocos2d::JniMethodInfo methodInfo;
        bool ret = false;
        if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "isBannerVisible", "(Lcom/google/android/gms/ads/AdView;)Z"))
        {
            ret = methodInfo.env->CallStaticBooleanMethod(methodInfo.classID, methodInfo.methodID, _bannerView);
            methodInfo.env->DeleteLocalRef(methodInfo.classID);
        }
        return ret;
    }

    void adViewDidReceiveAd()
    {
    	_ready = true;
        if(_delegate)
            _delegate->bannerReceiveAd(this);
    }
    void adViewDidFailToReceive(int error)
    {
        if(_delegate)
            _delegate->bannerFailedToReceiveAd(this, avalon::AdsErrorCode::INTERNAL_ERROR, error, "");
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
    jobject _bannerView;
    BannerDelegate *_delegate;
    bool _ready;
};

AndroidAdMob::AndroidAdMob(const std::string &version):AdMob(version) {}
AndroidAdMob::~AndroidAdMob() {}

void AndroidAdMob::setAdNetworkExtras(GADAdNetworkExtras network, const std::map<std::string,std::string> &extras) override
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

void AndroidAdMob::setTestDevices(const std::vector<std::string>& devices) override
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

void AndroidAdMob::setGender(GADGender gender) override
{
	cocos2d::JniMethodInfo methodInfo;
	if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "setGender", "(I)V"))
	{
		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, (int)gender);
		methodInfo.env->DeleteLocalRef(methodInfo.classID);
	}
}
void AndroidAdMob::setBirthDate(unsigned month, unsigned day, unsigned year) override
{
	cocos2d::JniMethodInfo methodInfo;
	if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "setBirthday", "(III)V"))
	{
		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, (jint)year, (jint)month, (jint)day);
		methodInfo.env->DeleteLocalRef(methodInfo.classID);
	}
}
void AndroidAdMob::setLocation(float latitude, float longitude, float accuracyInMeters) override
{
	cocos2d::JniMethodInfo methodInfo;
	if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "setLocation", "(FFF)V"))
	{
		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, (jfloat)latitude, (jfloat)longitude, (jfloat)accuracyInMeters);
		methodInfo.env->DeleteLocalRef(methodInfo.classID);
	}
}
void AndroidAdMob::setLocation(const std::string &location) override
{
}
void AndroidAdMob::setTagForChildDirectedTreatment(bool value) override
{
	cocos2d::JniMethodInfo methodInfo;
	if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "setTagForChildDirectedTreatment", "(Z)V"))
	{
		methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, (jboolean)value);
		methodInfo.env->DeleteLocalRef(methodInfo.classID);
	}
}
void AndroidAdMob::setKeywords(const std::vector<std::string>& keywords) override
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

GADInterstitial* AndroidAdMob::createIntestitial(const std::string &adUnitID, InterstitialDelegate *delegate)
{
	return new AndroidGADInterstitial(adUnitID, delegate);
}
GADBannerView* AndroidAdMob::createBanner(const std::string &adUnitID, GADAdSize size, BannerDelegate *delegate)
{
	return new AndroidGADBannerView(adUnitID, size, delegate);
}

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
JNIEXPORT void JNICALL Java_com_avalon_admob_AdMobHelper_delegateAdViewDidFailToReceiveAd(JNIEnv* env, jclass clazz, jlong object, jint error)
{
    ((avalon::AndroidGADBannerView*)object)->adViewDidFailToReceive(error);
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
JNIEXPORT void JNICALL Java_com_avalon_admob_AdMobHelper_delegateInterstitialDidFailToReceiveAd(JNIEnv* env, jclass clazz, jlong object, jint error)
{
    ((avalon::AndroidGADInterstitial*)object)->interstitialDidFailToReceiveAd(error);
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
