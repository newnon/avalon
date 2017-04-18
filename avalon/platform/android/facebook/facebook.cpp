#include "avalon/FacebookSocialPlugin.h"
#include "avalon/SocialPluginHelpers.h"

#include <jni.h>
#include "platform/android/jni/JniHelper.h"
#include <android/log.h>
#include <string>
#include <map>

const char* const HELPER_CLASS_NAME = "com/avalon/facebook/FacebookHelper";

namespace avalon {
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

    class FacebookSocialPluginAndroid : public FacebookSocialPlugin
    {
    public:
        SocialPermissionsHelper<FacebookPermission> facebookPermissionsHelper;

    public:
        void onLogin(int errorType, const std::string &token, const std::string &errorText,
                     const std::vector<SocialPermission> &grantedReadPermissions,
                     const std::vector<SocialPermission> &grantedPublishPermissions,
                     const std::vector<SocialPermission> &declinedPermissions)
        {
            _publishPermissions.assign(grantedPublishPermissions.begin(), grantedPublishPermissions.end());
            _readPermissions.assign(grantedReadPermissions.begin(), grantedReadPermissions.end());

            if (_delegate)
            {
                std::vector<SocialPermission> allGrantedPermissions;
                allGrantedPermissions.assign(_publishPermissions.begin(), _publishPermissions.end());
                allGrantedPermissions.insert(allGrantedPermissions.end(), _readPermissions.begin(), _readPermissions.end());

                if (errorType == 0)
                {
                    SocialPluginDelegate::Error socialError = {SocialPluginDelegate::Error::Type::SUCCESS, 0, ""};
                    _delegate->onLogin(socialError, token, allGrantedPermissions, declinedPermissions);
                }
                else if (errorType == 1)
                {
                    SocialPluginDelegate::Error socialError = {SocialPluginDelegate::Error::Type::USER_CANCEL, 0, errorText};
                    _delegate->onLogin(socialError, token, allGrantedPermissions, declinedPermissions);
                }
                else
                {
                    SocialPluginDelegate::Error socialError = {SocialPluginDelegate::Error::Type::UNDEFINED, 0, errorText};
                    _delegate->onLogin(socialError, token, allGrantedPermissions, declinedPermissions);
                }
            }
        }

        void onMyProfile(int preferedPictureSize, const std::vector<std::string> &keys, const std::vector<std::string> &values)
        {
            if (_delegate)
            {
                SocialProfile profile;
                profile.birthDay = std::numeric_limits<long long>::min();
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
                    else if (key == "middle_name")
                    {
                        profile.middleName = value;
                    }
                    else if (key == "email")
                    {
                        profile.email = value;
                    }
                    else if (key == "gender")
                    {
                        profile.gender = _genderHelper.toGender(value);
                    }
                    else if (key == "picture")
                    {
                        profile.pictureUrl = preferedPictureSize == 0 ? value : "";
                    }
                    else
                    {
                        profile.otherValue.emplace_back(key, value);
                    }
                }

                if (preferedPictureSize != 0)
                {
                    const std::string &size = std::to_string(preferedPictureSize);
                    profile.pictureUrl = "https://graph.facebook.com/" + profile.uid + "/picture?width=" + size + "&height=" + size;
                }

                _delegate->onGetMyProfile({SocialPluginDelegate::Error::Type::SUCCESS, 0, ""}, profile);
            }
        }

        FacebookSocialPluginAndroid()
        : facebookPermissionsHelper({{SocialPermission::Type::PUBLIC_PROFILE, "public_profile"},
                                     {SocialPermission::Type::EMAIL, "email"},
                                     {SocialPermission::Type::FRIENDS, "user_friends"}})
        ,_genderHelper("male", "female", "")
        {
            cocos2d::JniMethodInfo methodInfo;
            if (cocos2d::JniHelper::getStaticMethodInfo(methodInfo, HELPER_CLASS_NAME, "init", "()V"))
            {
                methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID);
                methodInfo.env->DeleteLocalRef(methodInfo.classID);
            }
        }

        virtual void setLoginBehavior(LoginBehavior type) override
        {
            int intType = -1;
            switch (type)
            {
                case LoginBehavior::Native:
                    intType = 1;
                    break;
                case LoginBehavior::Browser:
                    intType = 2;
                    break;
                case LoginBehavior::SystemAccount:
                    intType = 3;
                    break;
                case LoginBehavior::Web:
                    intType = 4;
                    break;
             };

            cocos2d::JniMethodInfo methodInfo;
            if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo, HELPER_CLASS_NAME, "setLoginBehavior", "(I)V"))
            {
                methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, intType);
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
                std::vector<std::string> stringPermissions;
                for (const auto &it : permissions)
                {
                    stringPermissions.emplace_back(facebookPermissionsHelper.toString(it));
                }

                jobject jStringPermissions = jobjectFromVector(stringPermissions);
                methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jStringPermissions);
                methodInfo.env->DeleteLocalRef(methodInfo.classID);
                methodInfo.env->DeleteLocalRef(jStringPermissions);
            }
        }

        virtual void requestPublishPermissions(const std::vector<SocialPermission> &permissions) override
        {
            cocos2d::JniMethodInfo methodInfo;
            if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo, HELPER_CLASS_NAME, "requestPublishPermissions", "([Ljava/lang/String;)V"))
            {
                std::vector<std::string> stringPermissions;
                for (const auto &it : permissions)
                {
                    stringPermissions.emplace_back(facebookPermissionsHelper.toString(it));
                }

                jobject jStringPermissions = jobjectFromVector(stringPermissions);
                methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jStringPermissions);
                methodInfo.env->DeleteLocalRef(methodInfo.classID);
                methodInfo.env->DeleteLocalRef(jStringPermissions);
            }
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

        virtual void getMyProfile(int preferedPictureSize, const std::vector<std::string> &additionalFields) override
        {
            if (isLoggedIn())
            {
                std::string fields = "id,first_name,picture,last_name,middle_name,email,gender,birthday";
                for (const auto &it : additionalFields)
                {
                    if (!it.empty())
                    {
                        fields += "," + it;
                    }
                }

                cocos2d::JniMethodInfo methodInfo;
                if (cocos2d::JniHelper::getStaticMethodInfo(methodInfo, HELPER_CLASS_NAME, "getMyProfile", "(Ljava/lang/String;I)V"));
                {
                    jstring jFields = methodInfo.env->NewStringUTF(fields.c_str());
                    methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jFields, preferedPictureSize);
                    methodInfo.env->DeleteLocalRef(methodInfo.classID);
                    methodInfo.env->DeleteLocalRef(jFields);
                }
            }
            else
            {
                if (_delegate)
                    _delegate->onGetMyProfile({SocialPluginDelegate::Error::Type::NO_LOGIN, 0, ""}, _emptyProfile);
            }
        }

        virtual void getProfiles(const std::vector<std::string> &userIds, int preferedPictureSize, const std::vector<std::string> &additionalFields) override
        {
        }

        virtual void getAppFriends(int preferedPictureSize, const std::vector<std::string> &additionalFields) override
        {
        }

    private:
        SocialPluginDelegate *_delegate = nullptr;

        std::vector<SocialPermission> _readPermissions;
        std::vector<SocialPermission> _publishPermissions;

        SocialProfile _emptyProfile;
        GenderHelper<std::string> _genderHelper;
    };

    FacebookSocialPlugin &FacebookSocialPlugin::getInstance()
    {
        static FacebookSocialPluginAndroid facebook;
        return facebook;
    }
}
extern "C" {

JNIEXPORT void JNICALL
Java_com_avalon_facebook_FacebookHelper_delegateOnLogin(JNIEnv *env, jclass clazz, jint errorType, jstring token, jstring errorText,
                                                        jobjectArray grantedReadPermission, jobjectArray grantedPublishPermission, jobjectArray deniedPermissions)
{
    avalon::FacebookSocialPluginAndroid &facebook = static_cast<avalon::FacebookSocialPluginAndroid&>(avalon::FacebookSocialPlugin::getInstance());

    jsize grantedReadSize = env->GetArrayLength(grantedReadPermission);
    std::vector<avalon::SocialPermission> grantedReadPermissions;
    for (int i = 0; i < grantedReadSize; ++i)
    {
        jstring permision = (jstring)env->GetObjectArrayElement(grantedReadPermission, i);
        grantedReadPermissions.emplace_back(facebook.facebookPermissionsHelper.fromString(cocos2d::JniHelper::jstring2string(permision)));
        env->DeleteLocalRef(permision);
    }

    jsize grantedPublishSize = env->GetArrayLength(grantedPublishPermission);
    std::vector<avalon::SocialPermission> grantedPublishPermissions;
    for (int i = 0; i < grantedPublishSize; ++i)
    {
        jstring permision = (jstring)env->GetObjectArrayElement(grantedPublishPermission, i);
        grantedPublishPermissions.emplace_back(facebook.facebookPermissionsHelper.fromString(cocos2d::JniHelper::jstring2string(permision)));
        env->DeleteLocalRef(permision);
    }

    jsize deniedSize = env->GetArrayLength(deniedPermissions);
    std::vector<avalon::SocialPermission> declinedPermissions;
    for (int i = 0; i < deniedSize; ++i)
    {
        jstring permision = (jstring)env->GetObjectArrayElement(deniedPermissions, i);
        declinedPermissions.emplace_back(facebook.facebookPermissionsHelper.fromString(cocos2d::JniHelper::jstring2string(permision)));
        env->DeleteLocalRef(permision);
    }

    facebook.onLogin(errorType, cocos2d::JniHelper::jstring2string(token), cocos2d::JniHelper::jstring2string(errorText), grantedReadPermissions, grantedPublishPermissions, declinedPermissions);
}

JNIEXPORT void JNICALL
Java_com_avalon_facebook_FacebookHelper_delegateOnMyProfile(JNIEnv *env, jclass clazz, jint preferedPictureSize, jobjectArray keys, jobjectArray values)
{
    avalon::FacebookSocialPluginAndroid &facebook = static_cast<avalon::FacebookSocialPluginAndroid&>(avalon::FacebookSocialPlugin::getInstance());

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

    facebook.onMyProfile(preferedPictureSize, keysVector, valuesVector);
}
}