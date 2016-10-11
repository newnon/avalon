//
//  SystemInfo.cpp
//
//

#include "avalon/SystemInfo.h"
#include <jni.h>
#include "platform/android/jni/JniHelper.h"

const char* const CLASS_NAME = "com/avalon/systeminfo/SystemInfoHelper";

namespace avalon {

SystemInfo::CarierInfo SystemInfo::getCarierInfo()
{
    CarierInfo ret;
    ret.carrierName = cocos2d::JniHelper::callStaticStringMethod(CLASS_NAME, "getCarrierName");
    ret.isoCountryCode = cocos2d::JniHelper::callStaticStringMethod(CLASS_NAME, "getIsoCountryCode ");
    ret.mobileCountryCode = cocos2d::JniHelper::callStaticStringMethod(CLASS_NAME, "getMobileCountryCode"); 
    ret.mobileNetworkCode = cocos2d::JniHelper::callStaticStringMethod(CLASS_NAME, "getMobileNetworkCode");
    return ret;
}

std::string SystemInfo::getDeviceModel()
{
    return cocos2d::JniHelper::callStaticStringMethod(CLASS_NAME, "getDeviceModel");
}

std::string SystemInfo::getIDFA()
{
    return cocos2d::JniHelper::callStaticStringMethod(CLASS_NAME, "getIDFA");
}

std::string SystemInfo::getOSVersion()
{
    return cocos2d::JniHelper::callStaticStringMethod(CLASS_NAME, "getOSVersion");
}

bool SystemInfo::isTablet()
{
    return cocos2d::JniHelper::callStaticBooleanMethod(CLASS_NAME, "isTablet");
}

} // namespace avalon
