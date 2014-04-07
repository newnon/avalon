//
//  FlurryAdX.mm
//  MedPed
//
//  Created by Sergey on 26.03.14.
//
//

#include "avalon/FlurryAds.h"
#import "FlurryAds.h"

namespace avalon {
    
static NSDictionary *nsDictionaryFromStringMap(const std::map<std::string,std::string> &stringMap) {
    if (stringMap.empty()) {
        return NULL;
    }
    
    NSMutableDictionary *nsDict = [NSMutableDictionary dictionaryWithCapacity:stringMap.size()];
    
    for(auto it=stringMap.begin();it!=stringMap.end();++it)
    {
        NSString *value = [[[NSString alloc] initWithBytes:it->first.c_str() length:it->first.size() encoding:NSUTF8StringEncoding] autorelease];
        [nsDict setValue:value forKey:[NSString stringWithCString:it->second.c_str() encoding:NSUTF8StringEncoding]];
        
    }
    return nsDict;
}


void FlurryAds::fetchAdForSpace(const std::string &space, int x,int y,int width,int heignt, AdSize size)
{
    [::FlurryAds fetchAdForSpace:[NSString stringWithCString:space.c_str() encoding:NSUTF8StringEncoding] frame:CGRectMake(x, y, width, heignt) size:(::FlurryAdSize)size];
}

bool FlurryAds::adReadyForSpace(const std::string &space)
{
    return [::FlurryAds adReadyForSpace:[NSString stringWithCString:space.c_str() encoding:NSUTF8StringEncoding]] == YES;
}
void FlurryAds::displayAdForSpace(const std::string &space)
{
    [::FlurryAds displayAdForSpace:[NSString stringWithCString:space.c_str() encoding:NSUTF8StringEncoding] onView:[UIApplication sharedApplication].keyWindow];
}
void FlurryAds::displayAdForSpaceModally(const std::string &space)
{
    [::FlurryAds displayAdForSpace:[NSString stringWithCString:space.c_str() encoding:NSUTF8StringEncoding] modallyForViewController:[UIApplication sharedApplication].keyWindow.rootViewController];
}
void FlurryAds::fetchAndDisplayAdForSpace(const std::string &space, int x, int y, int width, int heignt, AdSize size, int timeout)
{
    [::FlurryAds showAdForSpace:[NSString stringWithCString:space.c_str() encoding:NSUTF8StringEncoding] view:nil size:(FlurryAdSize)size timeout:timeout];
}

void FlurryAds::removeAdFromSpace(const std::string &space)
{
    [::FlurryAds removeAdFromSpace:[NSString stringWithCString:space.c_str() encoding:NSUTF8StringEncoding]];
}
void FlurryAds::initialize()
{
    [::FlurryAds initialize:[UIApplication sharedApplication].keyWindow.rootViewController];
}

void FlurryAds::setAdDelegate(FlurryAdsDelegate *delegate)
{
    
}
void FlurryAds::enableTestAds(bool enable)
{
    [::FlurryAds enableTestAds:enable];
}
void FlurryAds::setUserCookies(const std::map<std::string,std::string> &userCookies)
{
    [::FlurryAds setUserCookies:nsDictionaryFromStringMap(userCookies)];
}
void FlurryAds::clearUserCookies()
{
    [::FlurryAds clearUserCookies];
}
void FlurryAds::setKeywordsForTargeting(const std::map<std::string,std::string> &keywords)
{
    [::FlurryAds setKeywordsForTargeting:nsDictionaryFromStringMap(keywords)];
}
void FlurryAds::clearKeywords()
{
    [::FlurryAds clearKeywords];
}
/*void FlurryAdsX::addCustomAdNetwork(void *adNetworkClass, void *adNetworkProperties)
{
    
}*/

} // namespace avalon
