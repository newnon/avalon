#include "avalon/VKSocialPlugin.h"
#include "avalon/SocialPluginHelpers.h"

#include <jni.h>
#include "platform/android/jni/JniHelper.h"
#include <android/log.h>
#include <string>
#include <map>

const char* const HELPER_CLASS_NAME = "com/avalon/vkontakte/VkontakteHelper";

namespace avalon {
    static std::vector<std::string> split(const char *str, char c)
    {
        std::vector<std::string> result;

        if (str && *str != '\0')
        {
            do
            {
                const char *begin = str;

                while(*str != c && *str)
                    str++;

                result.push_back(std::string(begin, str));
            } while (0 != *str++);
        }

        return result;
    }

    static jobject jobjectFromDictionary(const std::map<std::string, std::string> &dictionary) {
        JNIEnv *env = cocos2d::JniHelper::getEnv();
        jclass mapClass = env->FindClass("java/util/HashMap");
        if (mapClass == NULL) {
            return NULL;
        }

        jsize map_len = 1;

        jmethodID init = env->GetMethodID(mapClass, "<init>", "(I)V");
        jobject hashMap = env->NewObject(mapClass, init, map_len);

        jmethodID put = env->GetMethodID(mapClass, "put",
                                         "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");

        for (auto &it : dictionary) {
            jstring key = env->NewStringUTF(it.first.c_str());
            jstring value = env->NewStringUTF(it.second.c_str());

            env->CallObjectMethod(hashMap, put, key, value);

            env->DeleteLocalRef(key);
            env->DeleteLocalRef(value);
        }

        env->DeleteLocalRef(mapClass);
        return hashMap;
    }

    static jobject jobjectFromVector(const std::vector<std::string> &vector) {
        JNIEnv *env = cocos2d::JniHelper::getEnv();
        jclass clsString = env->FindClass("java/lang/String");
        jobjectArray stringArray = env->NewObjectArray(vector.size(), clsString, 0);

        for (size_t i = 0; i < vector.size(); ++i) {
            jstring tmp = env->NewStringUTF(vector[i].c_str());
            env->SetObjectArrayElement(stringArray, i, tmp);
            env->DeleteLocalRef(tmp);
        }

        env->DeleteLocalRef(clsString);
        return stringArray;
    }

    class VkontakteSocialPluginAndroid : public VKSocialPlugin
    {
    public:
        SocialPermissionsHelper<VKPermission> vkontaktePermissionsHelper;

    public:
        void onLogin(int errorType, const std::string &token, const std::string &errorText)
        {
            if (errorType == 0)
            {
                SocialPluginDelegate::Error socialError = {SocialPluginDelegate::Error::Type::SUCCESS, 0, ""};
                _delegate->onLogin(socialError, token, _readPermissions, {});
            }
            else
            {
                _readPermissions.clear();
                SocialPluginDelegate::Error socialError = {SocialPluginDelegate::Error::Type::UNDEFINED, 0, errorText};
                _delegate->onLogin(socialError, token, {}, {});
            }
        }

        void onMyProfile(void *userData, const std::vector<std::string> &keys, const std::vector<std::string> &values)
        {
            if (_delegate)
            {
                SocialProfile profile;
                for (int i = 0; i < keys.size(); ++i)
                {
                    const std::string &key = keys.at(i);
                    const std::string &value = values.at(i);

                    if (key == "id")
                    {
                        profile.uid = value;
                    }
                    else if (key == "first_name")
                    {
                        profile.firstName = value;
                    }
                    else if (key == "last_name")
                    {
                        profile.lastName = value;
                    }
                    else if (key == "nickname")
                    {
                        profile.nickName = value;
                    }
                    else if (key == "email")
                    {
                        profile.email = value;
                    }
                    else if (key == "bdate")
                    {
                        std::vector<std::string> parts = split(value.c_str(), '.');
                        if(parts.size() == 2)
                            profile.birthDate = SocialProfile::BirthDate(std::stoi(parts[0]), std::stoi(parts[1]), 0);
                        else if(parts.size() == 3)
                            profile.birthDate = SocialProfile::BirthDate(std::stoi(parts[0]), std::stoi(parts[1]), std::stoi(parts[2]));
                    }
                    else if (key == "sex")
                    {
                        profile.gender = _genderHelper.toGender(std::atoi(value.c_str()));
                    }
                    else if (key == "photo_50" || key == "photo_100" || key == "photo_200")
                    {
                        profile.pictureUrl = value;
                    }
                    else
                    {
                        profile.otherValue.emplace_back(key, value);
                    }
                }

                _delegate->onGetMyProfile({SocialPluginDelegate::Error::Type::SUCCESS, 0, ""}, userData, profile);
            }
        }

        VkontakteSocialPluginAndroid()
        : vkontaktePermissionsHelper({{SocialPermission::Type::PUBLIC_PROFILE, ""},
                                     {SocialPermission::Type::EMAIL, "email"},
                                     {SocialPermission::Type::FRIENDS, "friends"}})
        , _genderHelper(2, 1, 0)
        {
            cocos2d::JniMethodInfo methodInfo;
            if (cocos2d::JniHelper::getStaticMethodInfo(methodInfo, HELPER_CLASS_NAME, "init", "()V"))
            {
                methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID);
                methodInfo.env->DeleteLocalRef(methodInfo.classID);
            }
        }

        virtual void setDelegate(SocialPluginDelegate *delegate) override
        {
            _delegate = delegate;
        }

        virtual void login() override
        {
            requestReadPermissions({SocialPermission::Type::PUBLIC_PROFILE, SocialPermission::Type::FRIENDS, SocialPermission::Type::EMAIL});
        }

        virtual void login(const std::vector<SocialPermission> &permissions) override
        {
            requestReadPermissions(permissions);
        }

        virtual void requestReadPermissions(const std::vector<SocialPermission> &permissions) override
        {
            cocos2d::JniMethodInfo methodInfo;
            if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "requestReadPermissions", "([Ljava/lang/String;)V"))
            {
                _readPermissions.assign(permissions.begin(), permissions.end());

                std::vector<std::string> stringPermissions;
                for (const auto &it : permissions)
                {
                    auto name = vkontaktePermissionsHelper.toString(it);
                    if (!name.empty())
                    {
                        stringPermissions.emplace_back(name);
                    }
                }

                jobject jStringPermissions = jobjectFromVector(stringPermissions);
                methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jStringPermissions);
                methodInfo.env->DeleteLocalRef(methodInfo.classID);
                methodInfo.env->DeleteLocalRef(jStringPermissions);
            }
        }

        virtual void requestPublishPermissions(const std::vector<SocialPermission> &permissions) override
        {
        }

        virtual void logout() override
        {
            if (!isLoggedIn())
            {
                cocos2d::JniMethodInfo methodInfo;
                if (cocos2d::JniHelper::getStaticMethodInfo(methodInfo, HELPER_CLASS_NAME, "logout", "()V"));
                {
                    methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID);
                    methodInfo.env->DeleteLocalRef(methodInfo.classID);
                }

                if (_delegate)
                    _delegate->onLogout({SocialPluginDelegate::Error::Type::SUCCESS, 0, ""});
            }
        }

        virtual bool isLoggedIn() const override
        {
            cocos2d::JniMethodInfo methodInfo;
            if (cocos2d::JniHelper::getStaticMethodInfo(methodInfo, HELPER_CLASS_NAME, "isLoggedIn", "()Z"));
            {
                jboolean result = methodInfo.env->CallStaticBooleanMethod(methodInfo.classID, methodInfo.methodID);
                methodInfo.env->DeleteLocalRef(methodInfo.classID);
                return result;
            }

            return false;
        }

        virtual std::string getUserID() const override
        {
            if (isLoggedIn())
            {
                cocos2d::JniMethodInfo methodInfo;
                if (cocos2d::JniHelper::getStaticMethodInfo(methodInfo, HELPER_CLASS_NAME, "getUserID", "()(Ljava/lang/String;"));
                {
                    jstring jValue = (jstring)methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID);
                    std::string userId = cocos2d::JniHelper::jstring2string(jValue);

                    methodInfo.env->DeleteLocalRef(jValue);
                    methodInfo.env->DeleteLocalRef(methodInfo.classID);
                    return userId;
                }
            }

            return "";
        }

        virtual std::string getAccessToken() const override
        {
            if (isLoggedIn())
            {
                cocos2d::JniMethodInfo methodInfo;
                if (cocos2d::JniHelper::getStaticMethodInfo(methodInfo, HELPER_CLASS_NAME, "getAccessToken", "()(Ljava/lang/String;"));
                {
                    jstring jValue = (jstring)methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID);
                    std::string userId = cocos2d::JniHelper::jstring2string(jValue);

                    methodInfo.env->DeleteLocalRef(jValue);
                    methodInfo.env->DeleteLocalRef(methodInfo.classID);
                    return userId;
                }
            }
            return "";
        }

        virtual std::vector<SocialPermission> getReadPermissionList() const override
        {
            return _readPermissions;
        }

        virtual std::vector<SocialPermission> getPublishPermissionList() const override
        {
            return _publishPermissions;
        }

        virtual void getMyProfile(int preferedPictureSize, void *userData, const std::vector<std::string> &additionalFields) override
        {
            if (isLoggedIn())
            {
                std::string fields = "id,first_name,last_name,nickname,sex,bdate";
                for (const auto &it : additionalFields)
                {
                    if (!it.empty())
                        fields += "," + it;
                }

                if (preferedPictureSize < 75)
                    fields += ",photo_50";
                else if (preferedPictureSize < 150)
                    fields += ",photo_100";
                else
                    fields += ",photo_200";

                cocos2d::JniMethodInfo methodInfo;
                if (cocos2d::JniHelper::getStaticMethodInfo(methodInfo, HELPER_CLASS_NAME, "getMyProfile", "(Ljava/lang/String;J)V"));
                {
                    jstring jFields = methodInfo.env->NewStringUTF(fields.c_str());
                    methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jFields, (jlong)userData);
                    methodInfo.env->DeleteLocalRef(methodInfo.classID);
                    methodInfo.env->DeleteLocalRef(jFields);
                }
            }
            else
            {
                if (_delegate)
                    _delegate->onGetMyProfile({SocialPluginDelegate::Error::Type::NO_LOGIN, 0, ""}, userData, _emptyProfile);
            }
        }

        virtual void getProfiles(const std::vector<std::string> &userIds, void *userData, int preferedPictureSize, const std::vector<std::string> &additionalFields) override
        {
        }

        virtual void getAppFriends(int preferedPictureSize, void *userData, const std::vector<std::string> &additionalFields) override
        {
        }

    private:
        SocialPluginDelegate *_delegate = nullptr;

        std::vector<SocialPermission> _readPermissions;
        std::vector<SocialPermission> _publishPermissions;

        SocialProfile _emptyProfile;
        GenderHelper<int> _genderHelper;
    };

    VKSocialPlugin &VKSocialPlugin::getInstance()
    {
        static VkontakteSocialPluginAndroid vkontakte;
        return vkontakte;
    }
}
extern "C" {

JNIEXPORT void JNICALL
Java_com_avalon_vkontakte_VkontakteHelper_delegateOnLogin(JNIEnv *env, jclass clazz, jint errorType, jstring token, jstring errorText)
{
    avalon::VkontakteSocialPluginAndroid &vkontakte = static_cast<avalon::VkontakteSocialPluginAndroid&>(avalon::VKSocialPlugin::getInstance());
    vkontakte.onLogin(errorType, cocos2d::JniHelper::jstring2string(token), cocos2d::JniHelper::jstring2string(errorText));
}

JNIEXPORT void JNICALL
Java_com_avalon_vkontakte_VkontakteHelper_delegateOnMyProfile(JNIEnv *env, jclass clazz, jlong userData, jobjectArray keys, jobjectArray values)
{
    avalon::VkontakteSocialPluginAndroid &vkontakte = static_cast<avalon::VkontakteSocialPluginAndroid&>(avalon::VKSocialPlugin::getInstance());

    jsize keysSize = env->GetArrayLength(keys);
    std::vector<std::string> keysVector;
    for (int i = 0; i < keysSize; ++i)
    {
        jstring value = (jstring)env->GetObjectArrayElement(keys, i);
        keysVector.emplace_back(cocos2d::JniHelper::jstring2string(value));
        env->DeleteLocalRef(value);
    }

    jsize valuesSize = env->GetArrayLength(values);
    std::vector<std::string> valuesVector;
    for (int i = 0; i < valuesSize; ++i)
    {
        jstring value = (jstring)env->GetObjectArrayElement(values, i);
        valuesVector.emplace_back(cocos2d::JniHelper::jstring2string(value));
        env->DeleteLocalRef(value);
    }

    vkontakte.onMyProfile((void*)userData, keysVector, valuesVector);
}
}