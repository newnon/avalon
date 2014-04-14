#include "avalon/Notifications.h"
#include "platform/android/jni/JniHelper.h"
#include <jni.h>

using namespace std;

namespace avalon {

static avalon::LocalNotificationsDelegate *_localNotificationsDelegate = nullptr;
static avalon::RemoteNotificationsDelegate *_remoteNotificationsDelegate = nullptr;

#define  CLASS_NAME "com/avalon/notifications/Notifications"

void Notifications::schedule(const std::string &message, long long time, int id, const std::string &sound, unsigned badgeNumber, const std::unordered_map<std::string,std::string> &userDict)
{
    cocos2d::JniMethodInfo methodInfo;

    if (cocos2d::JniHelper::getStaticMethodInfo(methodInfo, CLASS_NAME, "showLocalNotification", "(Ljava/lang/String;Ljava/lang/String;JII)V"))
    {
        jstring jmessage = methodInfo.env->NewStringUTF(message.c_str());
        jstring jsound = methodInfo.env->NewStringUTF(sound.c_str());
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jmessage, jsound, time, id, badgeNumber);
        methodInfo.env->DeleteLocalRef(jmessage);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
}

void Notifications::cancel(int id)
{
    cocos2d::JniMethodInfo methodInfo;

    if (cocos2d::JniHelper::getStaticMethodInfo(methodInfo, CLASS_NAME, "cancelLocalNotification", "(I)V")) {
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, id);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
}

void Notifications::cancelAll()
{
    cocos2d::JniMethodInfo methodInfo;

    if (cocos2d::JniHelper::getStaticMethodInfo(methodInfo, CLASS_NAME, "cancelAllLocalNotifications", "()V")) {
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
}

void Notifications::setLocalNotificationsDelegate(LocalNotificationsDelegate *delegate)
{
    _localNotificationsDelegate = delegate;
}
void Notifications::setRemoteNotificationsDelegate(RemoteNotificationsDelegate *delegate)
{
    _remoteNotificationsDelegate = delegate;
}

void Notifications::setBadgeNumber(unsigned value)
{
}

unsigned Notifications::getBadgeNumber()
{
    return 0;
}

void Notifications::registerForRemoteNotification()
{
    cocos2d::JniMethodInfo methodInfo;

    if (cocos2d::JniHelper::getStaticMethodInfo(methodInfo, CLASS_NAME, "registerForRemoteNotification", "()V")) {
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
}

void Notifications::unregisterForRemoteNotifications()
{
    cocos2d::JniMethodInfo methodInfo;

    if (cocos2d::JniHelper::getStaticMethodInfo(methodInfo, CLASS_NAME, "unregisterForRemoteNotification", "()V")) {
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
}

}

extern "C" {

JNIEXPORT void JNICALL Java_com_avalon_notifications_Notifications_delegateOnLocalNotification(JNIEnv* env, jclass clazz, jboolean jActive, jstring jMessage, jstring jSound, jint jBadgeNumber)
{
    if(avalon::_localNotificationsDelegate)
    {
        std::string message = cocos2d::JniHelper::jstring2string(jMessage);
        std::string sound = cocos2d::JniHelper::jstring2string(jSound);
        avalon::_localNotificationsDelegate->onLocalNotification(jActive, message, sound, jBadgeNumber, std::unordered_map<std::string,std::string>());
    }
}

JNIEXPORT void JNICALL Java_com_avalon_notifications_Notifications_delegateOnRemoteNotification(JNIEnv* env, jclass clazz, jboolean jActive, jstring jMessage, jstring jSound, jint jBadgeNumber)
{
    if(avalon::_remoteNotificationsDelegate)
    {
        std::string message = cocos2d::JniHelper::jstring2string(jMessage);
        std::string sound = cocos2d::JniHelper::jstring2string(jSound);
        avalon::_remoteNotificationsDelegate->onRemoteNotification(jActive, message, sound, jBadgeNumber, std::unordered_map<std::string,std::string>());
    }
}

JNIEXPORT void JNICALL Java_com_avalon_notifications_Notifications_delegateOnRegisterForRemoteNotifications(JNIEnv* env, jclass clazz, jstring jData, jstring jError)
{
    if(avalon::_remoteNotificationsDelegate)
    {
        std::string error = cocos2d::JniHelper::jstring2string(jError);
        std::string dataStr = cocos2d::JniHelper::jstring2string(jError);
        std::vector<unsigned char> data;
        if(!dataStr.empty())
            data.assign(dataStr.begin(),--dataStr.end());
        avalon::_remoteNotificationsDelegate->onRegisterForRemoteNotifications(data,error);
    }
}


}
