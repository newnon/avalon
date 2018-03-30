//
//  Firebase.cpp
//  Firebase
//
//  Created by Vladimir Shlobin on 29.03.2018.
//

#include "../Firebase.h"
#include "firebase/analytics.h"
#include "firebase/app.h"

#if defined(__ANDROID__)
#include "platform/android/jni/JniHelper.h"
#endif

namespace avalon {

namespace firebase {

void initialize()
{
#if defined(__ANDROID__)
    auto appInstance = ::firebase::App::Create(cocos2d::JniHelper::getEnv(), cocos2d::JniHelper::getActivity());
#else
    auto appInstance = ::firebase::App::Create();
#endif
    if (appInstance)
        ::firebase::analytics::Initialize(*appInstance);
}

void terminate()
{
    ::firebase::analytics::Terminate();
}

void setAnalyticsCollectionEnabled(bool enabled)
{
    ::firebase::analytics::SetAnalyticsCollectionEnabled(enabled);
}

void logEvent(const std::string &name, const std::map<std::string, std::string> &params)
{
    std::vector<::firebase::analytics::Parameter> array;
    array.reserve(params.size());
    for (const auto &param : params)
        array.push_back(::firebase::analytics::Parameter(param.first.c_str(), param.second.c_str()));
    ::firebase::analytics::LogEvent(name.c_str(), array.data(), array.size());
}

void setUserProperty(const std::string &name, const std::string &property)
{
    ::firebase::analytics::SetUserProperty(name.c_str(), property.c_str());
}

void setUserId(const std::string & userId)
{
    ::firebase::analytics::SetUserId(userId.c_str());
}

void setMinimumSessionDuration(int64_t milliseconds)
{
    ::firebase::analytics::SetMinimumSessionDuration(milliseconds);
}

void setSessionTimeoutDuration(int64_t milliseconds)
{
    ::firebase::analytics::SetSessionTimeoutDuration(milliseconds);
}

void setCurrentScreen(const std::string &screenName)
{
    ::firebase::analytics::SetCurrentScreen(screenName.c_str(), nullptr);
}

}  // namespace analytics

}  // namespace firebase
