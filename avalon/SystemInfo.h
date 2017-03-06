#ifndef __SYSTEMINFO_H__
#define __SYSTEMINFO_H__

#include <string>

namespace avalon {

class SystemInfo
{
public:
    struct CarierInfo
    {
        std::string carrierName;
        std::string isoCountryCode;
        std::string mobileCountryCode;
        std::string mobileNetworkCode;
    };
    static CarierInfo getCarierInfo();
    static std::string getDeviceModel();
    static std::string getIDFA();
    static std::string getOSVersion();
    static std::string getBundleId();
    static bool isTablet();
};
    
}

#endif /* __SYSTEMINFO_H__ */
