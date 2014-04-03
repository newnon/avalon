//
//  AnalyticX.h
//  AnalyticX
//
//  Created by diwwu on 5/10/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef AVALON_FLURRY_FLURRY_H
#define AVALON_FLURRY_FLURRY_H

#include <string>
#include <map>

namespace avalon {
    
    enum class FlurryLogLevel{
        None = 0,         //No output
        CriticalOnly,     //Default, outputs only critical log events
        Debug,            //Debug level, outputs critical and main log events
        All               //Highest level, outputs all log events
    };

class Flurry {
    
public:
    /*
     optional sdk settings that should be called before start session
     */
    /*
    + (void)setAppVersion:(NSString *)version;		// override the app version
    + (NSString *)getFlurryAgentVersion;			// get the Flurry Agent version number
    + (void)setShowErrorInLogEnabled:(BOOL)value;	// default is NO
    + (void)setDebugLogEnabled:(BOOL)value;			// generate debug logs for Flurry support, default is NO
    + (void)setLogLevel:(FlurryLogLevel)value;      // generates debug logs to console
    + (void)setSessionContinueSeconds:(int)seconds; // default is 10 seconds
    + (void)setSecureTransportEnabled:(BOOL)value; // set data to be sent over SSL, default is NO
     */
    
    //iOS + Android
    static void setAppVersion(const std::string &version);
    //iOS + Android
    static std::string getFlurryAgentVersion();
    //iOS
    static void setShowErrorInLogEnabled(bool value);
    //iOS + Android
    static void setDebugLogEnabled(bool value);
    //iOS
    static void setLogLevel(FlurryLogLevel level);
    //iOS + Android. The param is in second. Will be converted to millisecond internally.
    static void setSessionContinueSeconds(int seconds);
    //iOS + Android
    static void setSecureTransportEnabled(bool value);
    //iOS
	static void setCrashReportingEnabled(bool value);
    
    /*
     start session, attempt to send saved sessions to server 
     */
    /*
    + (void)startSession:(NSString *)apiKey;
     */
    //iOS + Android
    static void startSession(const std::string &apiKey);
    //iOS
    static void startSession(const std::string &apiKey, const std::map<std::string,std::string> &options);
    //Ios
    static void pauseBackgroundSession();
    //Android
    static void endSession();

    /*
     log events or errors after session has started
     */
    /*
    + (void)logEvent:(NSString *)eventName;
    + (void)logEvent:(NSString *)eventName withParameters:(NSDictionary *)parameters;
    + (void)logError:(NSString *)errorID message:(NSString *)message exception:(NSException *)exception;
    + (void)logError:(NSString *)errorID message:(NSString *)message error:(NSError *)error;
     */
    //iOS & android
    void setCaptureUncaughtExceptions(bool isEnabled);
    //iOS
    static void logException(const std::string &errorID, const std::string &message, void *error);
    //iOS + Android
    static void logEvent(const std::string &eventName);
    //iOS + Android
    static void logEventWithParameters(const std::string &eventName, const std::map<std::string,std::string> &parameters);

    /* 
     start or end timed events
     */
    /*
    + (void)logEvent:(NSString *)eventName timed:(BOOL)timed;
    + (void)logEvent:(NSString *)eventName withParameters:(NSDictionary *)parameters timed:(BOOL)timed;
    + (void)endTimedEvent:(NSString *)eventName withParameters:(NSDictionary *)parameters;	// non-nil parameters will update the parameters
     */
    //iOS + Android
    static void logEventTimed(const std::string &eventName, bool timed);
    //iOS + Android
    static void logEventWithParametersTimed(const std::string &eventName, const std::map<std::string,std::string> &parameters, bool timed);
    //iOS + Android. On Android, the *parameters* will be ignored
    static void endTimedEventWithParameters(const std::string &eventName, const std::map<std::string,std::string> &parameters); // non-nil parameters will update the parameters
    
    /*
     count page views
     */
    /*
    + (void)logAllPageViews:(id)target;		// automatically track page view on UINavigationController or UITabBarController
    + (void)logPageView;					// manually increment page view by 1
     */
    //iOS + Android
    static void logPageView();
    
    /*
     set user info
     */
    /*
    + (void)setUserID:(NSString *)userID;	// user's id in your system
    + (void)setAge:(int)age;				// user's age in years
    + (void)setGender:(NSString *)gender;	// user's gender m or f
     */
    //iOS + Android
    static void setUserID(const std::string &userID);
    //iOS + Android
    static void setAge(int age);
    //iOS + Android. "m" for male, "f" for female
    static void setGender(const std::string &gender);
    
    /*
     set location information
     */
    /*
    + (void)setLatitude:(double)latitude longitude:(double)longitude horizontalAccuracy:(float)horizontalAccuracy verticalAccuracy:(float)verticalAccuracy;
     */
    //iOS
    static void setLatitudeLongitudeHorizontalAccuracyVerticalAccuracy(double latitude, double longitude, float horizontalAccuracy, float verticalAccuracy);
    //Android
    static void setReportLocation(bool reportLocation);
    
    /*
     optional session settings that can be changed after start session
     */
    /*
    + (void)setSessionReportsOnCloseEnabled:(BOOL)sendSessionReportsOnClose;	// default is YES
    + (void)setSessionReportsOnPauseEnabled:(BOOL)setSessionReportsOnPauseEnabled;	// default is NO
    + (void)setEventLoggingEnabled:(BOOL)value;		// default is YES
     */
    //iOS
    static void setSessionReportsOnCloseEnabled(bool sendSessionReportsOnClose);
    //iOS
    static void setSessionReportsOnPauseEnabled(bool setSessionReportsOnPauseEnabled);
    //iOS
    static void setEventLoggingEnabled(bool value);
};
    
} // namespace avalon

#endif
