#ifdef AVALON_CONFIG_GAMECENTER_ENABLED

#include <avalon/GameCenter.h>

#include <jni.h>
#include <string>
#include "cocos2d.h"
#include "platform/android/jni/JniHelper.h"

namespace avalon {

/**
 * C++ -->> Java
 */

namespace helper {
namespace gamecenter {

const char* const CLASS_NAME = "com/avalon/GameCenter";

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
    cocos2d::JniMethodInfo t;
    if (cocos2d::JniHelper::getStaticMethodInfo(t, CLASS_NAME, name, "()Z")) {
        bool result = (t.env->CallStaticBooleanMethod(t.classID, t.methodID) == JNI_TRUE);
        t.env->DeleteLocalRef(t.classID);
        return result;
    } else {
        return false;
    }
}

void callStaticVoidMethodWithString(const char* name, const char* idName)
{
    cocos2d::JniMethodInfo t;
    if (cocos2d::JniHelper::getStaticMethodInfo(t, CLASS_NAME, name, "(Ljava/lang/String;)V")) {
        jstring jIdName = t.env->NewStringUTF(idName);
        t.env->CallStaticVoidMethod(t.classID, t.methodID, jIdName);
        t.env->DeleteLocalRef(t.classID);
        t.env->DeleteLocalRef(jIdName);
    }
}

void callStaticVoidMethodWithStringAndInt(const char* name, const char* idName, const int score)
{
    cocos2d::JniMethodInfo t;
    if (cocos2d::JniHelper::getStaticMethodInfo(t, CLASS_NAME, name, "(Ljava/lang/String;I)V")) {
        jstring jIdName = t.env->NewStringUTF(idName);
        t.env->CallStaticVoidMethod(t.classID, t.methodID, jIdName, (jint)score);
        t.env->DeleteLocalRef(t.classID);
        t.env->DeleteLocalRef(jIdName);
    }
}

std::string callStaticStringMethod(const char *name)
{
	cocos2d::JniMethodInfo t;
	if (cocos2d::JniHelper::getStaticMethodInfo(t, CLASS_NAME, name, "()Ljava/lang/String;")) {
		jstring rv = (jstring) t.env->CallObjectMethod(t.classID, t.methodID);
		t.env->DeleteLocalRef(t.classID);
		const char *js = t.env->GetStringUTFChars(rv, NULL);
		std::string cs(js);
		t.env->ReleaseStringUTFChars(rv, js);
		return cs;
	}
	else
		return "";
}

} // namespace gamecenter
} // namespace helper

/**
 * Public API
 */

void GameCenter::login()
{
    helper::gamecenter::callStaticVoidMethod("login");
}

bool GameCenter::showAchievements()
{
    return helper::gamecenter::callStaticBoolMethod("showAchievements");
}

void GameCenter::postAchievement(const char* idName, int percentComplete, bool showBanner)
{
    helper::gamecenter::callStaticVoidMethodWithStringAndInt("postAchievement", idName, percentComplete);
}

void GameCenter::clearAllAchievements()
{
    helper::gamecenter::callStaticVoidMethod("clearAllAchievements");
}

bool GameCenter::showScores()
{
    return helper::gamecenter::callStaticBoolMethod("showScores");
}

void GameCenter::postScore(const char* idName, int score)
{
    helper::gamecenter::callStaticVoidMethodWithStringAndInt("postScore", idName, score);
}

void GameCenter::clearAllScores()
{
    helper::gamecenter::callStaticVoidMethod("clearAllScores");
}

std::string GameCenter::getPlayerId()
{
	return helper::gamecenter::callStaticStringMethod("getPlayerId");
}

void GameCenter::signIn()
{
    helper::gamecenter::callStaticVoidMethod("signIn");
}

void GameCenter::signOut()
{
    helper::gamecenter::callStaticVoidMethod("signOut");
}

bool GameCenter::isSignedIn()
{
    return helper::gamecenter::callStaticBoolMethod("isSignedIn");

}

} // namespace avalon

#endif /* AVALON_CONFIG_GAMECENTER_ENABLED */
