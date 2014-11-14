#include "avalon/AdGeneration.h"

#include <jni.h>
#include "platform/android/jni/JniHelper.h"
#include <android/log.h>
#include <string>
#include <map>

const char* const BANNER_CLASS_NAME = "com/avalon/adgeneration/ADGBanner";

namespace avalon {

AdGenerationSize makeCustomAdGenerationAdSize(unsigned short width, unsigned short height)
{
	return static_cast<AdGenerationSize>(width<<16 | height);
}

class AndroidAdGenerationBannerView:public AdGenerationBannerView
{
public:
	virtual bool isVisible()
	{
        if(!_banner)
            return false;
        cocos2d::JniMethodInfo methodInfo;
        bool ret = false;
        if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo , BANNER_CLASS_NAME, "isVisible", "(Lcom/avalon/adgeneration/ADGBanner;)Z"))
        {
            ret = methodInfo.env->CallStaticBooleanMethod(methodInfo.classID, methodInfo.methodID, _banner);
            methodInfo.env->DeleteLocalRef(methodInfo.classID);
        }
        return ret;
	}

	virtual void show(int x, int y, int width, int height, BannerScaleType scaleType, BannerGravityType gravity)
	{
		if(_banner)
		{
			cocos2d::JniMethodInfo methodInfo;
			if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo, BANNER_CLASS_NAME, "showBanner", "(Lcom/avalon/adgeneration/ADGBanner;IIII)V"))
			{
				methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, _banner, (jint)x, (jint)y, (jint)width, (jint)height);
				methodInfo.env->DeleteLocalRef(methodInfo.classID);
			}
		}
	}

	virtual void hide()
	{
		if(_banner)
		{
			cocos2d::JniMethodInfo methodInfo;
			if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo, BANNER_CLASS_NAME, "hideBanner", "(Lcom/avalon/adgeneration/ADGBanner;)V"))
			{
				methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, _banner);
				methodInfo.env->DeleteLocalRef(methodInfo.classID);
			}
		}
	}

	AndroidAdGenerationBannerView(const std::string &adUnitID, AdGenerationSize size):AdGenerationBannerView(adUnitID, size)
	{
		cocos2d::JniMethodInfo methodInfo;
		if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo, BANNER_CLASS_NAME, "createBannerView", "(Ljava/lang/String;IJ)Lcom/avalon/adgeneration/ADGBanner;"))
		{
			jstring jAdUnitID = methodInfo.env->NewStringUTF(adUnitID.c_str());
			jobject banner = methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID, jAdUnitID, (jint)size, (jlong)nullptr);
			_banner = cocos2d::JniHelper::getEnv()->NewGlobalRef(banner);
			methodInfo.env->DeleteLocalRef(banner);
			methodInfo.env->DeleteLocalRef(jAdUnitID);
			methodInfo.env->DeleteLocalRef(methodInfo.classID);
		}
		else
		{
			_banner = nullptr;
		}
	}
	~AndroidAdGenerationBannerView()
	{
		if(_banner)
			cocos2d::JniHelper::getEnv()->DeleteGlobalRef(_banner);
	}
private:
	jobject _banner;
};

AdGenerationBannerView* AdGenerationBannerView::create(const std::string &adUnitID, AdGenerationSize size)
{
    return new AndroidAdGenerationBannerView(adUnitID, size);
}
}
