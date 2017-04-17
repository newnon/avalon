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

    class FacebookSocialPluginAndroid : public FacebookSocialPlugin {
        enum class FacebookPermissionType {
            NONE = 0,
        };

    public:
        void onLogin(const std::string &token)
        {
            int a =0;
            a += 1;
        }

        FacebookSocialPluginAndroid()
        : _facebookPermissionsHelper({{SocialPermission::Type::PUBLIC_PROFILE, "public_profile"}, {SocialPermission::Type::EMAIL, "email"},  {SocialPermission::Type::FRIENDS, "user_friends"}})
        ,_genderHelper("male", "female", "")
        {
            cocos2d::JniMethodInfo methodInfo;
            if (cocos2d::JniHelper::getStaticMethodInfo(methodInfo, HELPER_CLASS_NAME, "init", "()V"))
            {
                methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID);
                methodInfo.env->DeleteLocalRef(methodInfo.classID);
            }
        }

        virtual void setLoginBehavior(LoginBehavior type) override {
            // switch (type)
            // {
            //     case LoginBehavior::Native:
            //         [_loginManager setLoginBehavior:FBSDKLoginBehaviorNative];
            //         break;
            //     case LoginBehavior::Browser:
            //         [_loginManager setLoginBehavior:FBSDKLoginBehaviorBrowser];
            //         break;
            //     case LoginBehavior::SystemAccount:
            //         [_loginManager setLoginBehavior:FBSDKLoginBehaviorSystemAccount];
            //         break;
            //     case LoginBehavior::Web:
            //         [_loginManager setLoginBehavior:FBSDKLoginBehaviorWeb];
            //         break;
            // };
        }

        virtual void setDelegate(SocialPluginDelegate *delegate) override {
            _delegate = delegate;
        }

        virtual void login() override {
            requestReadPermissions(
                    {SocialPermission::Type::PUBLIC_PROFILE, SocialPermission::Type::FRIENDS,
                     SocialPermission::Type::EMAIL});
        }

        virtual void login(const std::vector<SocialPermission> &permissions) override {
            requestReadPermissions(permissions);
        }

        virtual void
        requestReadPermissions(const std::vector<SocialPermission> &permissions) override
        {
            cocos2d::JniMethodInfo methodInfo;
            if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo ,HELPER_CLASS_NAME, "requestReadPermissions", "([Ljava/lang/String;)V"))
            {
                std::vector<std::string> stringPermissions;
                for (const auto &it : permissions)
                {
                    stringPermissions.emplace_back(_facebookPermissionsHelper.toString(it));
                }

                jobject jStringPermissions = jobjectFromVector(stringPermissions);
                methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jStringPermissions);
                methodInfo.env->DeleteLocalRef(methodInfo.classID);
                methodInfo.env->DeleteLocalRef(jStringPermissions);
            }
        }

        virtual void requestPublishPermissions(const std::vector<SocialPermission> &permissions) override {
            cocos2d::JniMethodInfo methodInfo;
            if(cocos2d::JniHelper::getStaticMethodInfo(methodInfo, HELPER_CLASS_NAME, "requestPublishPermissions", "([Ljava/lang/String;)V"))
            {
                std::vector<std::string> stringPermissions;
                for (const auto &it : permissions)
                {
                    stringPermissions.emplace_back(_facebookPermissionsHelper.toString(it));
                }

                jobject jStringPermissions = jobjectFromVector(stringPermissions);
                methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jStringPermissions);
                methodInfo.env->DeleteLocalRef(methodInfo.classID);
                methodInfo.env->DeleteLocalRef(jStringPermissions);
            }
        }

        virtual void logout() override {
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

        virtual bool isLoggedIn() const override {
            cocos2d::JniMethodInfo methodInfo;
            if (cocos2d::JniHelper::getStaticMethodInfo(methodInfo, HELPER_CLASS_NAME, "isLoggedIn", "()Z"));
            {
                jboolean result = methodInfo.env->CallStaticBooleanMethod(methodInfo.classID, methodInfo.methodID);
                methodInfo.env->DeleteLocalRef(methodInfo.classID);
                return result;
            }

            return false;
        }

        virtual std::string getUserID() const override {
            // if (isLoggedIn())
            // {
            //     NSString *userString = [FBSDKAccessToken currentAccessToken].userID;
            //     return [userString UTF8String];
            // }

            return "";
        }

        virtual std::string getAccessToken() const override {
            // if (isLoggedIn())
            // {
            //     NSString *tokenString = [FBSDKAccessToken currentAccessToken].tokenString;
            //     return [tokenString UTF8String];
            // }

            return "";
        }

        virtual std::vector<SocialPermission> getReadPermissionList() const override {
            return _readPermissions;
        }

        virtual std::vector<SocialPermission> getPublishPermissionList() const override {
            return _publishPermissions;
        }

        virtual void getMyProfile(int preferedPictureSize,
                                  const std::vector<std::string> &additionalFields) override {
            if (isLoggedIn()) {

            } else {
                if (_delegate)
                    _delegate->onGetMyProfile({SocialPluginDelegate::Error::Type::NO_LOGIN, 0, ""},
                                              _emptyProfile);
            }
        }

        virtual void getProfiles(const std::vector<std::string> &userIds, int preferedPictureSize,
                                 const std::vector<std::string> &additionalFields) override {
        }

        virtual void getAppFriends(int preferedPictureSize,
                                   const std::vector<std::string> &additionalFields) override {
        }

    private:

    private:
        using FacebookPermission = PlatformSocialPermission<FacebookPermissionType>;

        SocialPluginDelegate *_delegate = nullptr;

        std::vector<SocialPermission> _readPermissions;
        std::vector<SocialPermission> _publishPermissions;

        SocialProfile _emptyProfile;
        GenderHelper _genderHelper;

        SocialPermissionsHelper<FacebookPermission> _facebookPermissionsHelper;
    };


    FacebookSocialPlugin &FacebookSocialPlugin::getInstance() {
        static FacebookSocialPluginAndroid facebook;
        return facebook;
    }
}
extern "C" {

JNIEXPORT void JNICALL
Java_com_avalon_facebook_FacebookHelper_delegateOnLogin(JNIEnv *env, jclass clazz, jstring token) {
    avalon::FacebookSocialPluginAndroid &facebook = static_cast<avalon::FacebookSocialPluginAndroid&>(avalon::FacebookSocialPlugin::getInstance());
    facebook.onLogin(cocos2d::JniHelper::jstring2string(token));
}
}