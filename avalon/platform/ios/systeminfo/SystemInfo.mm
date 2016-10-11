//
//  SystemInfo.mm
//
//

#include "avalon/SystemInfo.h"
#import <CoreTelephony/CTCarrier.h>
#import <CoreTelephony/CTTelephonyNetworkInfo.h>
#import <AdSupport/AdSupport.h>
#import <sys/utsname.h>

namespace avalon {
    
SystemInfo::CarierInfo SystemInfo::getCarierInfo()
{
    CarierInfo ret;
    Class CTTelephonyNetworkInfoClass = NSClassFromString(@"CTTelephonyNetworkInfo");
    if(CTTelephonyNetworkInfoClass)
    {
        id myNetworkInfo = [[CTTelephonyNetworkInfoClass alloc] init];
        CTCarrier *myCarrier = [myNetworkInfo subscriberCellularProvider];
        if(myCarrier)
        {
            if(myCarrier.carrierName)
                ret.carrierName = [myCarrier.carrierName cStringUsingEncoding:NSUTF8StringEncoding];
            if(myCarrier.isoCountryCode)
                ret.isoCountryCode = [myCarrier.isoCountryCode cStringUsingEncoding:NSUTF8StringEncoding];
            if(myCarrier.mobileCountryCode)
                ret.mobileCountryCode = [myCarrier.mobileCountryCode cStringUsingEncoding:NSUTF8StringEncoding];
            if(myCarrier.mobileNetworkCode)
                ret.mobileNetworkCode = [myCarrier.mobileNetworkCode cStringUsingEncoding:NSUTF8StringEncoding];
        }
    }
    return ret;
}

std::string SystemInfo::getDeviceModel()
{
    struct utsname systemInfo;
    uname(&systemInfo);
    return systemInfo.machine;
}

std::string SystemInfo::getIDFA()
{
    Class ASIdentifierManagerClass = NSClassFromString(@"ASIdentifierManager");
    if (ASIdentifierManagerClass) {
        NSString *adId = [[[ASIdentifierManagerClass sharedManager] advertisingIdentifier] UUIDString];
        return adId?[adId cStringUsingEncoding:NSUTF8StringEncoding]:"";
    }
    return "";
}

std::string SystemInfo::getOSVersion()
{
    return [[UIDevice currentDevice].systemVersion cStringUsingEncoding:NSUTF8StringEncoding];
}
    
bool SystemInfo::isTablet()
{
    return UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad;
}
    
} // namespace avalon
