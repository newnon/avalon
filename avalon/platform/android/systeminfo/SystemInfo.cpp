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
    return ret;
}

std::string SystemInfo::getDeviceModel()
{
    return JniHelper::callStaticStringMethod(CLASS_NAME, "getDeviceModel");
}

std::string SystemInfo::getIDFA()
{
    return JniHelper::callStaticStringMethod(CLASS_NAME, "getIDFA");
}

std::string SystemInfo::getOSVersion()
{
    return JniHelper::callStaticStringMethod(CLASS_NAME, "getOSVersion");
}

bool SystemInfo::isTablet()
{
    return JniHelper::callStaticBooleanMethod(CLASS_NAME, "isTablet");
}

} // namespace avalon
