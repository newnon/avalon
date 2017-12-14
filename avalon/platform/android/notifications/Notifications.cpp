#include "avalon/Notifications.h"
#include "platform/android/jni/JniHelper.h"
#include <jni.h>

using namespace std;

namespace avalon {

static avalon::LocalNotificationsDelegate *_localNotificationsDelegate = nullptr;
static avalon::RemoteNotificationsDelegate *_remoteNotificationsDelegate = nullptr;

#define  CLASS_NAME "com/avalon/notifications/Notifications"

static jobject jobjectFromDictionary(const std::unordered_map<std::string,std::string> &dictionary)
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

static std::unordered_map<std::string,std::string> dictionaryFromJobject(jobject data) {

    std::unordered_map<std::string,std::string> res;

    JNIEnv* env = cocos2d::JniHelper::getEnv();

    // Get the HashMap Class
    jclass jclass_of_hashmap = (env)->GetObjectClass(data);

    // Get link to Method "entrySet"
    jmethodID entrySetMethod = (env)->GetMethodID(jclass_of_hashmap, "entrySet", "()Ljava/util/Set;");

    // Invoke the "entrySet" method on the HashMap object
    jobject jobject_of_entryset = env->CallObjectMethod(data, entrySetMethod);

    // Get the Set Class
    jclass jclass_of_set = (env)->FindClass("java/util/Set"); // Problem during compilation !!!!!

    if (jclass_of_set == nullptr) {
        //qWarning() << "java/util/Set lookup failed\n";
        return res;
    }

    // Get link to Method "iterator"
    jmethodID iteratorMethod = env->GetMethodID(jclass_of_set, "iterator", "()Ljava/util/Iterator;");

    // Invoke the "iterator" method on the jobject_of_entryset variable of type Set
    jobject jobject_of_iterator = env->CallObjectMethod(jobject_of_entryset, iteratorMethod);

    // Get the "Iterator" class
    jclass jclass_of_iterator = (env)->FindClass("java/util/Iterator");

    // Get link to Method "hasNext"
    jmethodID hasNextMethod = env->GetMethodID(jclass_of_iterator, "hasNext", "()Z");

    jmethodID nextMethod = env->GetMethodID(jclass_of_iterator, "next", "()Ljava/lang/Object;");

    jclass jclass_of_entry = env->FindClass("java/util/Map$Entry");
    jmethodID getKeyMethod = env->GetMethodID(jclass_of_entry, "getKey","()Ljava/lang/Object;");
    jmethodID getValueMethod = env->GetMethodID(jclass_of_entry, "getValue","()Ljava/lang/Object;");

    while (env->CallBooleanMethod(jobject_of_iterator, hasNextMethod) ) {
        jobject jEntry = env->CallObjectMethod(jobject_of_iterator, nextMethod);

        jstring jkey = (jstring)env->CallObjectMethod(jEntry, getKeyMethod);
        jstring jvalue = (jstring)env->CallObjectMethod(jEntry, getValueMethod);

        std::string key = cocos2d::JniHelper::jstring2string(jkey);
        std::string value = cocos2d::JniHelper::jstring2string(jvalue);

        env->DeleteLocalRef(jEntry);
        env->DeleteLocalRef(jkey);
        env->DeleteLocalRef(jvalue);

        res[key] = value;
    }

    if (env->ExceptionOccurred()) {
        env->ExceptionDescribe();
        env->ExceptionClear();
    }
    env->DeleteLocalRef(jclass_of_entry);
    env->DeleteLocalRef(jclass_of_hashmap);
    env->DeleteLocalRef(jobject_of_entryset);
    env->DeleteLocalRef(jclass_of_set);
    env->DeleteLocalRef(jobject_of_iterator);
    env->DeleteLocalRef(jclass_of_iterator);

    return res;
}

const Notification* Notifications::getLaunchedNotification()
{
    cocos2d::JniMethodInfo methodInfo;

    if (cocos2d::JniHelper::getStaticMethodInfo(methodInfo, CLASS_NAME, "getLaunchedNotification", "()Ljava/util/HashMap;"))
    {
        jobject jObj = methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID);
        std::unordered_map<std::string,std::string> params;
        if(jObj)
        {
            params = dictionaryFromJobject(jObj);
        }
        methodInfo.env->DeleteLocalRef(jObj);
        if(!params.empty())
        {
            static Notification notification;
            auto idIt = params.find("id");
            if(idIt != params.end())
            {
                notification.id = std::atoi(idIt->second.c_str());
                params.erase(idIt);
            }
            else
            {
                notification.id = 0;
            }

            auto messageIt = params.find("message");
            if(messageIt != params.end())
            {
                notification.message = messageIt->second;
                params.erase(messageIt);
            }
            else
            {
                notification.message.clear();;
            }

            auto soundIt = params.find("sound");
            if(soundIt != params.end())
            {
                notification.sound = soundIt->second;
                params.erase(soundIt);
            }
            else
            {
                notification.sound.clear();;
            }

            auto badgeIt = params.find("badge");
            if(badgeIt != params.end())
            {
                notification.badgeNumber = std::atoi(badgeIt->second.c_str());
                params.erase(badgeIt);
            }
            else
            {
                notification.badgeNumber = 0;
            }

            notification.userDict = params;

            return &notification;
        }
    }
    return nullptr;
}

void Notifications::schedule(const std::string &message, long long time, int id, const std::string &sound, unsigned badgeNumber, const std::unordered_map<std::string,std::string> &userDict)
{
    cocos2d::JniMethodInfo methodInfo;

    if (cocos2d::JniHelper::getStaticMethodInfo(methodInfo, CLASS_NAME, "showLocalNotification", "(Ljava/lang/String;Ljava/lang/String;JIILjava/util/HashMap;)V"))
    {
        jstring jmessage = methodInfo.env->NewStringUTF(message.c_str());
        jstring jsound = methodInfo.env->NewStringUTF(sound.c_str());
        jobject jParams = jobjectFromDictionary(userDict);
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jmessage, jsound, time, id, badgeNumber, jParams);
        methodInfo.env->DeleteLocalRef(jParams);
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

bool Notifications::isScheduled(int id)
{
    bool ret = false;
    cocos2d::JniMethodInfo methodInfo;

    if (cocos2d::JniHelper::getStaticMethodInfo(methodInfo, CLASS_NAME, "isScheduled", "(I)Z")) {
        ret = methodInfo.env->CallStaticBooleanMethod(methodInfo.classID, methodInfo.methodID, id);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
    return ret;
}

std::vector<int> Notifications::getScheduledIds()
{
    std::vector<int> ret;
    cocos2d::JniMethodInfo methodInfo;

    if (cocos2d::JniHelper::getStaticMethodInfo(methodInfo, CLASS_NAME, "getScheduledIds", "()[I")) {
        jintArray idsArrayObject = (jintArray)methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID);

        JNIEnv* env = cocos2d::JniHelper::getEnv();
        jsize size = env->GetArrayLength(idsArrayObject);
        jint *ptr = env->GetIntArrayElements(idsArrayObject, nullptr);
        for(jsize i=0; i < size; ++i)
            ret.push_back(ptr[i]);
        env->ReleaseIntArrayElements(idsArrayObject, ptr, NULL);
        methodInfo.env->DeleteLocalRef(idsArrayObject);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
    return ret;
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

JNIEXPORT void JNICALL Java_com_avalon_notifications_Notifications_delegateOnLocalNotification(JNIEnv* env, jclass clazz, jboolean jActive, jint jId, jstring jMessage, jstring jSound, jint jBadgeNumber)
{
    if(avalon::_localNotificationsDelegate)
    {
        std::string message = cocos2d::JniHelper::jstring2string(jMessage);
        std::string sound = cocos2d::JniHelper::jstring2string(jSound);
        avalon::_localNotificationsDelegate->onLocalNotification(jActive, jId, message, sound, jBadgeNumber, std::unordered_map<std::string,std::string>());
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
