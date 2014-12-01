#include <avalon/Appirater.h>

#include <jni.h>
#include "cocos2d.h"
#include "platform/android/jni/JniHelper.h"

namespace avalon {

/**
* C++ -->> Java
*/

namespace appirater {
namespace helper {

const char* const CLASS_NAME = "com/sbstrm/appirater/Appirater";

void callStaticVoidMethod(const char* name)
{
    cocos2d::JniMethodInfo t;
    if (cocos2d::JniHelper::getStaticMethodInfo(t, CLASS_NAME, name, "()V")) {
        t.env->CallStaticVoidMethod(t.classID, t.methodID);
        t.env->DeleteLocalRef(t.classID);
    }
}

bool callStaticBoolMethod(const char* name)
{
	bool ret = false;
    cocos2d::JniMethodInfo t;
    if (cocos2d::JniHelper::getStaticMethodInfo(t, CLASS_NAME, name, "()Z")) {
    	ret = t.env->CallStaticBooleanMethod(t.classID, t.methodID);
        t.env->DeleteLocalRef(t.classID);
    }
    return true;
}

void callStaticVoidMethodWithString(const char* name, const char* argument)
{
    cocos2d::JniMethodInfo t;
    if (cocos2d::JniHelper::getStaticMethodInfo(t, CLASS_NAME, name, "(Ljava/lang/String;)V")) {
        jstring jProductId = t.env->NewStringUTF(argument);

        t.env->CallStaticVoidMethod(t.classID, t.methodID, jProductId);
        t.env->DeleteLocalRef(t.classID);

        t.env->DeleteLocalRef(jProductId);
    }
}

void callStaticVoidMethodWithBool(const char* name, const bool flag)
{
    cocos2d::JniMethodInfo t;
    if (cocos2d::JniHelper::getStaticMethodInfo(t, CLASS_NAME, name, "(Z)V")) {
        t.env->CallStaticVoidMethod(t.classID, t.methodID, flag);
        t.env->DeleteLocalRef(t.classID);
    }
}

void callStaticVoidMethodWithInt(const char* name, const int value)
{
    cocos2d::JniMethodInfo t;
    if (cocos2d::JniHelper::getStaticMethodInfo(t, CLASS_NAME, name, "(I)V")) {
        t.env->CallStaticVoidMethod(t.classID, t.methodID, value);
        t.env->DeleteLocalRef(t.classID);
    }
}

} // namespace helper
} // namespace appirater

Appirater::Appirater()
{
#if AVALON_PLATFORM_IS_ANDROID_AMAZON
    appirater::helper::callStaticVoidMethodWithString("setMarketUrl", "http://www.amazon.com/gp/mas/dl/android?p=%s");
#else
    appirater::helper::callStaticVoidMethodWithString("setMarketUrl", "market://details?id=%s");
#endif
}

Appirater* Appirater::getInstance()
{
    static Appirater* instance = new Appirater();
    return instance;
}

void Appirater::appLaunched(bool canPromptForRating)
{
	appirater::helper::callStaticVoidMethodWithBool("appLaunched", canPromptForRating);
}

void Appirater::appEnteredForeground(bool canPromptForRating)
{
	appirater::helper::callStaticVoidMethodWithBool("appLaunched", canPromptForRating);
}

void Appirater::setDaysUntilPrompt(double value)
{
    appirater::helper::callStaticVoidMethodWithInt("setInitialDaysUntilPrompt", value);
}

void Appirater::setUsesUntilPrompt(int value)
{

}

void Appirater::setSignificantEventsUntilPrompt(const int events)
{
    appirater::helper::callStaticVoidMethodWithInt("setSignificantEventsUntilPrompt", events);
}

void Appirater::setTimeBeforeReminding(double value)
{
	appirater::helper::callStaticVoidMethodWithInt("setReminderDaysUntilPrompt", value);
}

void Appirater::setDebug(const bool flag)
{
    appirater::helper::callStaticVoidMethodWithBool("setDebug", flag);
}

void Appirater::setDelegate(AppiraterDelegate *delegate)
{

}

void Appirater::userDidSignificantEvent(bool canPromptForRating)
{
    appirater::helper::callStaticVoidMethodWithBool("userDidSignificantEvent", canPromptForRating);
}

bool Appirater::tryToShowPrompt()
{
    return appirater::helper::callStaticBoolMethod("showIfNeeded");
}

void Appirater::forceShowPrompt(bool displayRateLaterButton)
{
    appirater::helper::callStaticVoidMethod("showRateDialog");
}

void Appirater::rateApp()
{
	appirater::helper::callStaticVoidMethod("rateApp");
}

void Appirater::setAppId(const char *appName)
{

}

void Appirater::setOpenInAppStore(bool openInAppStore)
{

}

void Appirater::setUsesAnimation(bool animation)
{

}

void Appirater::setAlwaysUseMainBundle(bool useMainBundle)
{

}

void Appirater::closeModal()
{

}

} // namespace avalon

