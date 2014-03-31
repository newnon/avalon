//
//  AnalyticX.cpp
//  AnalyticX
//
//  Created by diwwu on 5/10/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "avalon/Flurry.h"
#import "Flurry.h"

namespace avalon {
namespace flurry {
    
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

void Flurry::setAppVersion(const std::string &version) {
    [::Flurry setAppVersion:[NSString stringWithCString:version.c_str() encoding:NSUTF8StringEncoding]];
}

std::string Flurry::getFlurryAgentVersion() {
    return [[::Flurry getFlurryAgentVersion] cStringUsingEncoding:NSUTF8StringEncoding];
}

void Flurry::setShowErrorInLogEnabled(bool value) {
    [::Flurry setShowErrorInLogEnabled:value];
}

void Flurry::setDebugLogEnabled(bool value) {
    [::Flurry setDebugLogEnabled:value];
}
    
void Flurry::setLogLevel(FlurryLogLevel level)
{
    [::Flurry setLogLevel:(::FlurryLogLevel)level];
}

void Flurry::setSessionContinueSeconds(int seconds) {
    [::Flurry setSessionContinueSeconds:seconds];
}

void Flurry::setSecureTransportEnabled(bool value) {
    [::Flurry setSecureTransportEnabled:value];
}

void Flurry::setCrashReportingEnabled(bool value) {
	
	[::Flurry setCrashReportingEnabled:value];
}

void Flurry::startSession(const std::string &apiKey) {
    [::Flurry startSession:[NSString stringWithCString:apiKey.c_str() encoding:NSUTF8StringEncoding]];
}
    
void Flurry::startSession(const std::string &apiKey, const std::map<std::string,std::string> &options)
{
      [::Flurry startSession:[NSString stringWithCString:apiKey.c_str() encoding:NSUTF8StringEncoding] withOptions:nsDictionaryFromStringMap(options)];
}
    
void Flurry::pauseBackgroundSession()
{
    [::Flurry pauseBackgroundSession];
}

void Flurry::endSession() {
    //Android only
}

void Flurry::logEvent(const std::string &eventName) {
    [::Flurry logEvent:[NSString stringWithCString:eventName.c_str() encoding:NSUTF8StringEncoding]];
}

void Flurry::logEventWithParameters(const std::string &eventName, const std::map<std::string,std::string> &parameters) {
    
    [::Flurry logEvent:[NSString stringWithCString:eventName.c_str() encoding:NSUTF8StringEncoding] withParameters:nsDictionaryFromStringMap(parameters)];
}

void Flurry::logEventTimed(const std::string &eventName, bool timed) {
    [::Flurry logEvent:[NSString stringWithCString:eventName.c_str() encoding:NSUTF8StringEncoding] timed:timed];
}

void Flurry::logEventWithParametersTimed(const std::string &eventName, const std::map<std::string,std::string> &parameters, bool timed) {
    [::Flurry logEvent:[NSString stringWithCString:eventName.c_str() encoding:NSUTF8StringEncoding] withParameters:nsDictionaryFromStringMap(parameters) timed:timed];
}

void Flurry::endTimedEventWithParameters(const std::string &eventName,const std::map<std::string,std::string> &parameters) {
    [::Flurry endTimedEvent:[NSString stringWithCString:eventName.c_str() encoding:NSUTF8StringEncoding] withParameters:nsDictionaryFromStringMap(parameters)];
}

void Flurry::logPageView() {
    [::Flurry logPageView];
}

void Flurry::setUserID(const std::string &userID) {
    [::Flurry setUserID:[NSString stringWithCString:userID.c_str() encoding:NSUTF8StringEncoding]];
}

void Flurry::setAge(int age) {
    [::Flurry setAge:age];
}

void Flurry::setGender(const std::string &gender) {
    [::Flurry setGender:[NSString stringWithCString:gender.c_str() encoding:NSUTF8StringEncoding]];
}

void Flurry::setLatitudeLongitudeHorizontalAccuracyVerticalAccuracy(double latitude, double longitude, float horizontalAccuracy, float verticalAccuracy) {
    [::Flurry setLatitude:latitude longitude:longitude horizontalAccuracy:horizontalAccuracy verticalAccuracy:verticalAccuracy];
}

void Flurry::setSessionReportsOnCloseEnabled(bool sendSessionReportsOnClose) {
    [::Flurry setSessionReportsOnCloseEnabled:sendSessionReportsOnClose];
}

void Flurry::setSessionReportsOnPauseEnabled(bool setSessionReportsOnPauseEnabled) {
    [::Flurry setSessionReportsOnPauseEnabled:setSessionReportsOnPauseEnabled];
}

void Flurry::setEventLoggingEnabled(bool value) {
    [::Flurry setEventLoggingEnabled:value];
}

void Flurry::setReportLocation(bool reportLocation) {
    //Android only
}

} // namespace flurry
} // namespace avalon











