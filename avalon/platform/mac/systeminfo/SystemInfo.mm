//
//  SystemInfo.mm
//
//

#include "avalon/SystemInfo.h"
#include <sys/types.h>
#include <sys/sysctl.h>

namespace avalon {

SystemInfo::CarierInfo SystemInfo::getCarierInfo()
{
    CarierInfo ret;
    return ret;
}
    
std::string SystemInfo::getDeviceModel()
{
    size_t len = 0;
    sysctlbyname("hw.model", NULL, &len, NULL, 0);
    
    if (len) {
        char *model = (char *)malloc(len*sizeof(char));
        sysctlbyname("hw.model", model, &len, NULL, 0);
        std::string ret = model;
        free(model);
        return ret;
    }
    
    return "Apple Computer"; //incase model name can't be read
}
    
std::string SystemInfo::getIDFA()
{
    return "";
}
    
std::string SystemInfo::getOSVersion()
{
#ifdef MAC_OS_X_VERSION_10_10
    NSOperatingSystemVersion version = [[NSProcessInfo processInfo] operatingSystemVersion];
    NSString *OSVersion = [NSString stringWithFormat:@"%d_%d_%d", (int)version.majorVersion, (int)version.minorVersion, (int)version.patchVersion];
#else
    SInt32 major, minor, bugfix;
    Gestalt(gestaltSystemVersionMajor, &major);
    Gestalt(gestaltSystemVersionMinor, &minor);
    Gestalt(gestaltSystemVersionBugFix, &bugfix);
    NSString *OSVersion = [NSString stringWithFormat:@"%d_%d_%d", major, minor, bugfix];
#endif
    return [OSVersion cStringUsingEncoding:NSUTF8StringEncoding];
}
    
bool SystemInfo::isTablet()
{
    return true;
}

} // namespace avalon
