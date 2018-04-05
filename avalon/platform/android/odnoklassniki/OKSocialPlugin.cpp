//
//  OKSocialPlugin.cpp
//  Odnoklassniki Android
//
//  Created by Vladimir Shlobin on 03.04.2018.
//

#include <string>
#include <vector>
#include <jni.h>
#include "platform/android/jni/JniHelper.h"
#include "avalon/OKSocialPlugin.h"
#include "avalon/SocialPluginHelpers.h"

const char* const HELPER_CLASS_NAME = "com/avalon/odnoklassniki/OdnoklassnikiHelper";

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

static jobject jobjectFromVector(const std::vector<std::string> &vector)
{
    JNIEnv *env = cocos2d::JniHelper::getEnv();
    jclass clsString = env->FindClass("java/lang/String");
    jobjectArray stringArray = env->NewObjectArray(vector.size(), clsString, 0);

    for (size_t i = 0; i < vector.size(); ++i)
    {
        jstring tmp = env->NewStringUTF(vector[i].c_str());
        env->SetObjectArrayElement(stringArray, i, tmp);
        env->DeleteLocalRef(tmp);
    }

    env->DeleteLocalRef(clsString);
    return stringArray;
}

static std::vector<std::string> vectorOfStringsFromJobjectArray(JNIEnv *env, jobjectArray values)
{
    jsize size = env->GetArrayLength(values);
    std::vector<std::string> result;
    for (jsize i = 0; i < size; ++i)
    {
        jstring value = (jstring)env->GetObjectArrayElement(values, i);
        result.emplace_back(cocos2d::JniHelper::jstring2string(value));
        env->DeleteLocalRef(value);
    }
    return result;
}

class OKSocialPluginAndroid : public OKSocialPlugin {
public:
    static OKSocialPluginAndroid &getInstanceAndroid();
    OKSocialPluginAndroid();
    void setDelegate(SocialPluginDelegate *delegate) override;
    void setDebug(bool value) override;
    void login() override;
    void login(const std::vector<SocialPermission> &permissions) override;
    void requestReadPermissions(const std::vector<SocialPermission> &permissions) override;
    void requestPublishPermissions(const std::vector<SocialPermission> &permissions) override;
    void logout() override;
    bool isLoggedIn() const override;
    std::string getUserID() const override;
    std::string getAccessToken() const override;
    std::string getAppId() const override;
    std::vector<SocialPermission> getReadPermissionList() const override;
    std::vector<SocialPermission> getPublishPermissionList() const override;
    void getMyProfile(int preferedPictureSize, void *userData, const std::vector<std::string> &additionalFields) override;
    void getProfiles(const std::vector<std::string> &userIds, void *userData, int preferedPictureSize, const std::vector<std::string> &additionalFields) override;
    void getAppFriends(int preferedPictureSize, void *userData, const std::vector<std::string> &additionalFields) override;
    void onLogin(int errorType, const std::string &token, const std::string &userId, const std::string &errorText);
    void onMyProfile(int errorType, void *userData, const std::string &pictureField, const std::vector<std::string> &keys, const std::vector<std::string> &values, const std::string &errorText);
private:
    enum class ErrorType {SUCCESS, USER_CANCEL, ERROR};
    SocialPermissionsHelper<OKPermission> _permissionsHelper;
    GenderHelper<std::string> _genderHelper;
    std::map<int, std::string> _picturesMap;
    std::string _pictureIDKey;
    SocialProfile _emptyProfile;
    SocialPluginDelegate *_delegate = nullptr;
    bool _debug = false;
    std::string _accessToken;
    std::string _userID;
    std::vector<SocialPermission> _readPermissions;
    std::vector<SocialPermission> _publishPermissions;
};

OKSocialPluginAndroid::OKSocialPluginAndroid()
: _permissionsHelper({
    {SocialPermission::Type::PUBLIC_PROFILE, "VALUABLE_ACCESS"},
    {SocialPermission::Type::EMAIL, "GET_EMAIL"}})
, _genderHelper("male", "female", "")
, _picturesMap{
    {50, "pic_1"},
    {128, "pic_2"},
    {190, "pic_3"},
    {228, "pic_4"},
    {768, "pic_full"}}
, _pictureIDKey("photo_id")
{
    cocos2d::JniMethodInfo methodInfo;
    if (cocos2d::JniHelper::getStaticMethodInfo(methodInfo, HELPER_CLASS_NAME, "init", "()V"))
    {
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
}

void OKSocialPluginAndroid::setDelegate(SocialPluginDelegate *delegate)
{
    _delegate = delegate;
}

void OKSocialPluginAndroid::setDebug(bool value)
{
    _debug = value;
}

void OKSocialPluginAndroid::login()
{
    login({SocialPermission::Type::PUBLIC_PROFILE, SocialPermission::Type::FRIENDS, SocialPermission::Type::EMAIL});
}

void OKSocialPluginAndroid::login(const std::vector<SocialPermission> &permissions)
{
    requestReadPermissions(permissions);
}

void OKSocialPluginAndroid::requestReadPermissions(const std::vector<SocialPermission> &permissions)
{
    cocos2d::JniMethodInfo methodInfo;
    if (cocos2d::JniHelper::getStaticMethodInfo(methodInfo, HELPER_CLASS_NAME, "requestReadPermissions", "([Ljava/lang/String;Z)V"))
    {
        _readPermissions = permissions;

        std::vector<std::string> stringPermissions;
        for (const auto &permission : permissions)
        {
            const auto &permissionString = _permissionsHelper.toString(permission);
            if (!permissionString.empty())
                stringPermissions.emplace_back(permissionString);
        }
        stringPermissions.emplace_back("LONG_ACCESS_TOKEN");

        jobject jStringPermissions = jobjectFromVector(stringPermissions);
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jStringPermissions, (jboolean)_debug);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
        methodInfo.env->DeleteLocalRef(jStringPermissions);
    }
}

void OKSocialPluginAndroid::requestPublishPermissions(const std::vector<SocialPermission> &permissions)
{
    // TODO -
}

void OKSocialPluginAndroid::logout()
{
    if (!isLoggedIn()) {
        return;
    }

    _accessToken.clear();
    _userID.clear();

    cocos2d::JniMethodInfo methodInfo;
    if (cocos2d::JniHelper::getStaticMethodInfo(methodInfo, HELPER_CLASS_NAME, "logout", "(Z)V"))
    {
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, (jboolean)_debug);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }

    if (_delegate)
        _delegate->onLogout({SocialPluginDelegate::Error::Type::SUCCESS, 0, ""});
}

bool OKSocialPluginAndroid::isLoggedIn() const
{
    return !_accessToken.empty();
}

std::string OKSocialPluginAndroid::getUserID() const
{
    return _userID;
}

std::string OKSocialPluginAndroid::getAccessToken() const
{
    return _accessToken;
}

std::string OKSocialPluginAndroid::getAppId() const
{
    cocos2d::JniMethodInfo methodInfo;
    if (cocos2d::JniHelper::getStaticMethodInfo(methodInfo, HELPER_CLASS_NAME, "getAppId", "()Ljava/lang/String;"))
    {
        jstring jValue = (jstring)methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID);
        auto result = cocos2d::JniHelper::jstring2string(jValue);
        methodInfo.env->DeleteLocalRef(jValue);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
        return result;
    }
    return "";
}

std::vector<SocialPermission> OKSocialPluginAndroid::getReadPermissionList() const
{
    return _readPermissions;
}

std::vector<SocialPermission> OKSocialPluginAndroid::getPublishPermissionList() const
{
    return _publishPermissions;
}

void OKSocialPluginAndroid::getMyProfile(int preferedPictureSize, void *userData, const std::vector<std::string> &additionalFields)
{
    if (!isLoggedIn())
    {
        if (_delegate)
            _delegate->onGetMyProfile({SocialPluginDelegate::Error::Type::NO_LOGIN, 0, ""}, userData, _emptyProfile);
        return;
    }

    std::string fields = "uid,first_name,last_name,email,birthday,gender";
    for (const auto &field : additionalFields)
        fields += "," + field;

    auto picturesMapIterator = _picturesMap.lower_bound(preferedPictureSize);
    const auto &pictureField = (picturesMapIterator == _picturesMap.end()) ? _picturesMap.rbegin()->second : picturesMapIterator->second;
    fields += "," + _pictureIDKey + "," + pictureField;

    cocos2d::JniMethodInfo methodInfo;
    if (cocos2d::JniHelper::getStaticMethodInfo(methodInfo, HELPER_CLASS_NAME, "getMyProfile", "(Ljava/lang/String;JLjava/lang/String;Z)V"))
    {
        jstring jFields = methodInfo.env->NewStringUTF(fields.c_str());
        jstring jPictureField = methodInfo.env->NewStringUTF(pictureField.c_str());
        methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, jFields, (jlong)userData, jPictureField, (jboolean)_debug);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
//        methodInfo.env->DeleteLocalRef(jFields); // TODO
//        methodInfo.env->DeleteLocalRef(jPictureField); // TODO
    }
}

void OKSocialPluginAndroid::getProfiles(const std::vector<std::string> &userIds, void *userData, int preferedPictureSize, const std::vector<std::string> &additionalFields)
{
    // TODO -
}

void OKSocialPluginAndroid::getAppFriends(int preferedPictureSize, void *userData, const std::vector<std::string> &additionalFields)
{
    // TODO -
}

void OKSocialPluginAndroid::onLogin(int errorType, const std::string &token, const std::string &userId, const std::string &errorText)
{
    switch (static_cast<ErrorType>(errorType))
    {
    case ErrorType::SUCCESS:
        {
            _accessToken = token;
            _userID = userId;
            SocialPluginDelegate::Error socialError = {SocialPluginDelegate::Error::Type::SUCCESS, 0, ""};
            if (_delegate)
                _delegate->onLogin(socialError, token, _readPermissions, {});
        }
        break;
    case ErrorType::USER_CANCEL:
        {
            SocialPluginDelegate::Error socialError = {SocialPluginDelegate::Error::Type::USER_CANCEL, 0, errorText};
            if (_delegate)
                _delegate->onLogin(socialError, token, _readPermissions, {});
        }
        break;
    case ErrorType::ERROR:
        {
            SocialPluginDelegate::Error socialError = {SocialPluginDelegate::Error::Type::UNDEFINED, 0, errorText};
            if (_delegate)
                _delegate->onLogin(socialError, token, _readPermissions, {});
        }
        break;
    }
}

void OKSocialPluginAndroid::onMyProfile(int errorType, void *userData, const std::string &pictureField, const std::vector<std::string> &keys, const std::vector<std::string> &values, const std::string &errorText)
{
    if (!_delegate)
        return;

    switch (static_cast<ErrorType>(errorType))
    {
    case ErrorType::SUCCESS:
        {
            SocialProfile profile;
            for (size_t i = 0, size = keys.size(); i < size; ++i)
            {
                const auto &key = keys.at(i);
                const auto &value = values.at(i);

                if (key == "uid")
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
                else if (key == pictureField)
                {
                    profile.pictureUrl = value;
                }
                else if (key == "email")
                {
                    profile.email = value;
                }
                else if (key == _pictureIDKey)
                {
                    profile.pictureId = value;
                }
                else if (key == "birthday")
                {
                    std::vector<std::string> parts = split(value.c_str(), '-');
                    if (parts.size() == 3)
                        profile.birthDate = SocialProfile::BirthDate(std::stoi(parts[2]), std::stoi(parts[1]), std::stoi(parts[0]));
                }
                else if (key == "gender")
                {
                    profile.gender = _genderHelper.toGender(value);
                }
            }

            // Placeholder image case
            if (profile.pictureId.empty())
                profile.pictureUrl.clear();

            _delegate->onGetMyProfile({SocialPluginDelegate::Error::Type::SUCCESS, 0, ""}, userData, profile);
        }
        break;
    case ErrorType::ERROR:
        {
            _delegate->onGetMyProfile({SocialPluginDelegate::Error::Type::UNDEFINED, errorType, errorText}, userData, _emptyProfile);
        }
        break;
    }
}

OKSocialPluginAndroid &OKSocialPluginAndroid::getInstanceAndroid()
{
    static OKSocialPluginAndroid instance;
    return instance;
}

OKSocialPlugin &OKSocialPlugin::getInstance()
{
    return OKSocialPluginAndroid::getInstanceAndroid();
}

} // namespace avalon

extern "C" {

JNIEXPORT void JNICALL
Java_com_avalon_odnoklassniki_OdnoklassnikiHelper_delegateOnLogin(JNIEnv *env, jclass clazz, jint errorType, jstring token, jstring userId, jstring errorText)
{
    avalon::OKSocialPluginAndroid::getInstanceAndroid().onLogin(errorType, cocos2d::JniHelper::jstring2string(token), cocos2d::JniHelper::jstring2string(userId), cocos2d::JniHelper::jstring2string(errorText));
}

JNIEXPORT void JNICALL
Java_com_avalon_odnoklassniki_OdnoklassnikiHelper_delegateOnMyProfile(JNIEnv *env, jclass clazz, jint errorType, jlong userData, jstring pictureField, jobjectArray keys, jobjectArray values, jstring errorText)
{
    avalon::OKSocialPluginAndroid::getInstanceAndroid().onMyProfile(errorType, (void *)userData, cocos2d::JniHelper::jstring2string(pictureField), avalon::vectorOfStringsFromJobjectArray(env, keys), avalon::vectorOfStringsFromJobjectArray(env, values), cocos2d::JniHelper::jstring2string(errorText));
}

} // extern "C"
