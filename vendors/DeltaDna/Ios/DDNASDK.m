//
//  DeltaDNASDK.m
//  DeltaDNASDK
//
//  Created by David White on 18/07/2014.
//  Copyright (c) 2014 deltadna. All rights reserved.
//

#import "DDNASDK.h"
#import "DDNALog.h"
#import "DDNAPlayerPrefs.h"
#import "DDNAClientInfo.h"
#import "DDNAEventStore.h"
#import "DDNAEngageArchive.h"
#import "DDNAEventBuilder.h"
#import "NSString+Helpers.h"
#import "NSDictionary+Helpers.h"
#import <CommonCrypto/CommonDigest.h>

@interface DDNASDK ()
{
    dispatch_source_t _timer;
    dispatch_queue_t _taskQueue;
}

@property (nonatomic, strong) DDNAEventStore *eventStore;
@property (nonatomic, strong) DDNAEngageArchive *engageArchive;
@property (nonatomic, assign) BOOL reset;
@property (nonatomic, copy, readwrite) NSString *environmentKey;
@property (nonatomic, copy, readwrite) NSString *collectURL;
@property (nonatomic, copy, readwrite) NSString *engageURL;
@property (nonatomic, copy, readwrite) NSString *userID;
@property (nonatomic, copy, readwrite) NSString *sessionID;
@property (nonatomic, copy, readwrite) NSString *platform;
@property (nonatomic, assign, readwrite) BOOL hasStarted;

- (void)didReceiveNotification:(NSNotification *) notification;

@end

static NSString *const EV_KEY_NAME = @"eventName";
static NSString *const EV_KEY_USER_ID = @"userID";
static NSString *const EV_KEY_SESSION_ID = @"sessionID";
static NSString *const EV_KEY_TIMESTAMP = @"eventTimestamp";
static NSString *const EV_KEY_PARAMS = @"eventParams";

static NSString *const EP_KEY_PLATFORM = @"platform";
static NSString *const EP_KEY_SDK_VERSION = @"sdkVersion";

static NSString *const PP_KEY_FIRST_RUN = @"DDSDK_FIRST_RUN";
static NSString *const PP_KEY_USER_ID = @"DDSDK_USER_ID";
static NSString *const PP_KEY_HASH_SECRET = @"DDSDK_HASH_SECRET";
static NSString *const PP_KEY_CLIENT_VERSION = @"DDSDK_CLIENT_VERSION";
static NSString *const PP_KEY_PUSH_NOTIFICATION_TOKEN = @"DDSDK_PUSH_NOTIFICATION_TOKEN";

static NSString *const DD_EVENT_STARTED = @"DDNASDKStarted";

@implementation DDNASDK

#pragma mark - SingletonAccess

+ (instancetype) sharedInstance
{
    static dispatch_once_t pred = 0;
    __strong static id _sharedObject = nil;
    dispatch_once(&pred, ^{
        _sharedObject = [[self alloc] init];
    });
    return _sharedObject;
}

- (id) init
{
    if ((self = [super init]))
    {
        _reset = NO;
        _settings = [[DDNASettings alloc] init];
        
        _taskQueue = dispatch_queue_create("com.deltadna.TaskQueue", NULL);
        dispatch_suspend(_taskQueue);
        
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(didReceiveNotification:) name:DD_EVENT_STARTED object:nil];
        
        NSString *eventStorePath = [DDNA_EVENT_STORAGE_PATH stringByReplacingOccurrencesOfString:@"{persistent_path}" withString:[DDNASettings getPrivateSettingsDirectoryPath]];
        _eventStore = [[DDNAEventStore alloc] initWithStorePath:eventStorePath clearStore:_reset];
        
        NSString *engageArchivePath = [DDNA_ENGAGE_STORAGE_PATH stringByReplacingOccurrencesOfString:@"{persistent_path}" withString:[DDNASettings getPrivateSettingsDirectoryPath]];
        _engageArchive = [[DDNAEngageArchive alloc] initWithArchivePath:engageArchivePath clearStore:_reset];
    }
    return self;
}

#pragma mark - Setup SDK

- (void) startWithEnvironmentKey:(NSString *)environmentKey
                      collectURL:(NSString *)collectURL
                       engageURL:(NSString *)engageURL
{
    [self startWithEnvironmentKey:environmentKey
                       collectURL:collectURL
                        engageURL:engageURL
                           userID:nil];
}

-(void) startWithEnvironmentKey:(NSString *)environmentKey
                     collectURL:(NSString *)collectURL
                      engageURL:(NSString *)engageURL
                         userID:(NSString *)userID
{
    // Ensure this can only be called once
    @synchronized(self)
    {        
        self.environmentKey = environmentKey;
        self.collectURL = [self mungeUrl: collectURL];
        self.engageURL = [self mungeUrl: engageURL];
        
        if ([NSString stringIsNilOrEmpty:userID] && [NSString stringIsNilOrEmpty:self.userID])
        {
            self.userID = [DDNASDK generateUserID];
        }
        else
        {
            self.userID = userID;
        }
        
        DDNALogDebug(@"Starting SDK with user id %@", self.userID);
        
        self.platform = [DDNAClientInfo sharedInstance].platform;
        self.sessionID = [DDNASDK generateSessionID];
        
        self.hasStarted = true;
        [[NSNotificationCenter defaultCenter] postNotificationName:DD_EVENT_STARTED object:self];
        
        // Once we're started, send default events.
        [self triggerDefaultEvents];
        
        // Setup automated event uploads.
        if (_settings.backgroundEventUpload)
        {
            _timer = dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER, 0, 0, dispatch_get_main_queue());
            
            if (_timer)
            {
                uint64_t interval = _settings.backgroundEventUploadRepeatRateSeconds * NSEC_PER_SEC;
                uint64_t leeway = 1ull * NSEC_PER_SEC;
                dispatch_source_set_timer(_timer, dispatch_walltime(NULL, 0), interval, leeway);
                dispatch_source_set_event_handler(_timer, ^{
                    [self upload];
                });
                
                // Trigger after delay
                dispatch_time_t delay = dispatch_time(DISPATCH_TIME_NOW, _settings.backgroundEventUploadStartDelaySeconds * NSEC_PER_SEC);
                dispatch_after(delay, dispatch_get_main_queue(), ^{
                    dispatch_resume(_timer);
                });
            }
        }
    }
}

- (void) newSession
{
    self.sessionID = [DDNASDK generateSessionID];
}

- (void) stop
{
    DDNALogDebug(@"Stoping SDK");
    
    if (_timer)
    {
        dispatch_source_cancel(_timer);
    }
    
    [self recordEvent:@"gameEnded"];
    [self upload];
    self.hasStarted = false;
}

- (void) recordEvent: (NSString *) eventName
{
    NSDictionary * eventParams = [NSDictionary dictionary];
    [self recordEvent:eventName withEventDictionary:eventParams];
}

- (void) recordEvent: (NSString *) eventName
     withEventBuilder: (DDNAEventBuilder *) eventBuilder
{
    [self recordEvent:eventName withEventDictionary:[eventBuilder dictionary]];
}

- (void) recordEvent: (NSString *) eventName
  withEventDictionary: (NSDictionary *) eventParams
{
    if (!self.hasStarted)
    {
        NSException * e = [NSException exceptionWithName:@"NotStartedException"
                                                  reason:@"You must first start the DeltaDNA SDK"
                                                userInfo:nil];
        @throw e;
    }
    
    // The header for every event is eventName, userID, sessionID and timestamp.
    NSMutableDictionary * eventRecord = [NSMutableDictionary dictionary];
    [eventRecord setObject:eventName forKey:EV_KEY_NAME];
    [eventRecord setObject:self.userID forKey:EV_KEY_USER_ID];
    [eventRecord setObject:self.sessionID forKey:EV_KEY_SESSION_ID];
    [eventRecord setObject:[DDNASDK getCurrentTimestamp] forKey:EV_KEY_TIMESTAMP];
    
    NSMutableDictionary * mutableEventParams = [NSMutableDictionary dictionaryWithDictionary:eventParams];
    
    // Every template should support sdkVersion and platform in it's event params.
    if (![mutableEventParams objectForKey:EP_KEY_PLATFORM])
    {
        [mutableEventParams setObject:self.platform forKey:EP_KEY_PLATFORM];
    }
    
    if (![mutableEventParams objectForKey:EP_KEY_SDK_VERSION])
    {
        [mutableEventParams setObject:DDNA_SDK_VERSION forKey:EP_KEY_SDK_VERSION];
    }
    
    [eventRecord setObject:mutableEventParams forKey:EV_KEY_PARAMS];
    
    // Push onto the event store.
    
    if (![_eventStore pushEvent:eventRecord])
    {
        DDNALogDebug(@"Event Store full, unable to record event");
    }
}

- (void) requestEngagement: (NSString *) decisionPoint
             callbackBlock: (DDNAEngagementResponseBlock) callback
{
    [self requestEngagement:decisionPoint withEngageParams:nil callbackBlock:callback];
}

- (void) requestEngagement: (NSString *) decisionPoint
          withEngageParams: (NSDictionary *) engageParams
             callbackBlock: (DDNAEngagementResponseBlock) callback
{
    if (!self.hasStarted)
    {
        NSException * e = [NSException exceptionWithName:@"NotStartedException"
                                                  reason:@"You must first start the DeltaDNA SDK"
                                                userInfo:nil];
        @throw e;
    }

    if ([NSString stringIsNilOrEmpty:_engageURL])
    {
        DDNALogWarn(@"Engage URL not configured, can not make engagement.");
        return;
    }
    
    if ([NSString stringIsNilOrEmpty:decisionPoint])
    {
        DDNALogWarn(@"No decision point set, can not make engagement.");
        return;
    }
    
    @try
    {
        DDNALogDebug(@"Starting engagement for '%@'", decisionPoint);
        
        NSMutableDictionary *engageRequest = [NSMutableDictionary dictionary];
        [engageRequest setObject:self.userID forKey:@"userID"];
        [engageRequest setObject:decisionPoint forKey:@"decisionPoint"];
        [engageRequest setObject:self.sessionID forKey:@"sessionID"];
        [engageRequest setObject:DDNA_ENGAGE_API_VERSION forKey:@"version"];
        [engageRequest setObject:DDNA_SDK_VERSION forKey:@"sdkVersion"];
        [engageRequest setObject:self.platform forKey:@"platform"];
        
        NSNumberFormatter * f = [[NSNumberFormatter alloc] init];
        [f setNumberStyle:NSNumberFormatterDecimalStyle];
        NSNumber * timezoneOffset = [f numberFromString:[DDNAClientInfo sharedInstance].timezoneOffset];
        if (timezoneOffset != nil)
        {
            [engageRequest setObject:timezoneOffset forKey:@"timezoneOffset"];
        }
        
        if ([DDNAClientInfo sharedInstance].locale != nil)
        {
            [engageRequest setObject:[DDNAClientInfo sharedInstance].locale forKey:@"locale"];
        }
        
        if (engageParams != nil)
        {
            [engageRequest setObject:engageParams forKey:@"parameters"];
        }
        
        [self engageRequest:[NSString stringWithContentsOfDictionary:engageRequest]
          completionHandler:^(NSString * response)
         {
             bool usingCache = false;
             if (response != nil)
             {
                 DDNALogDebug(@"Using live engagement: %@.", response);
                 [_engageArchive setObject:response forKey:decisionPoint];
                 [_engageArchive save];
             }
             else
             {
                 NSString * cachedResponse = [_engageArchive objectForKey:decisionPoint];
                 if (cachedResponse != nil)
                 {
                     DDNALogWarn(@"Engage request failed, using cached response.");
                     usingCache = true;
                     response = cachedResponse;
                 }
                 else
                 {
                     DDNALogWarn(@"Engage request failed");
                 }
             }
             
             NSDictionary * result = [NSDictionary dictionaryWithJSONString:response];
             
             if (usingCache)
             {
                 NSMutableDictionary * result2 = [NSMutableDictionary dictionaryWithDictionary:result];
                 [result2 setObject:@YES forKey:@"isCachedResponse"];
                 result = result2;
             }
             callback(result);
         }];
    }
    @catch (NSException *exception)
    {
        DDNALogDebug(@"Engagement request failed: %@", exception.reason);
    }
}

- (void) requestImageMessage:(NSString *)decisionPoint withEngageParams:(NSDictionary *)engageParams imagePopup:(id <DDNAPopup>)popup
{
    [self requestImageMessage:decisionPoint withEngageParams:engageParams imagePopup:popup callbackBlock:nil];
}

- (void) requestImageMessage:(NSString *)decisionPoint withEngageParams:(NSDictionary *)engageParams imagePopup:(id <DDNAPopup>)popup callbackBlock:(DDNAEngagementResponseBlock)callback
{
    [self requestEngagement:decisionPoint
          withEngageParams:engageParams
             callbackBlock:^(NSDictionary * response) {
                 if (response != nil) {
                     if (response[@"image"]) {
                         [popup prepareWithImage:response[@"image"]];
                     }
                     
                     if (callback != nil) {
                         callback(response);
                     }
                 }
             }
     ];

}

- (void) recordPushNotification:(NSDictionary *)pushNotification didLaunch:(BOOL)didLaunch
{
    DDNALogDebug(@"Received push notification: %@", pushNotification);
 
    if (_hasStarted) {
        
        NSString *notificationId = pushNotification[@"_ddId"];
        NSString *notificationName = pushNotification[@"_ddName"];
        
        NSMutableDictionary *eventParams = [NSMutableDictionary dictionary];
        if (notificationId) {
            [eventParams setObject:[NSNumber numberWithInteger:[notificationId integerValue]] forKey:@"notificationId"];
        }
        if (notificationName) {
            [eventParams setObject:notificationName forKey:@"notificationName"];
        }
        [eventParams setObject:[NSNumber numberWithBool:didLaunch] forKey:@"notificationLaunch"];
        
        [self recordEvent:@"notificationOpened" withEventDictionary:eventParams];
    }
    else {
        // wait until the SDK has been started
        __typeof(self) __weak weakSelf = self;
        dispatch_async(_taskQueue, ^{
            [weakSelf recordPushNotification:pushNotification didLaunch:didLaunch];
        });
    }
}

- (void) upload
{
    if (!self.hasStarted)
    {
        NSException * e = [NSException exceptionWithName:@"NotStartedException"
                                                  reason:@"You must first start the DeltaDNA SDK"
                                                userInfo:nil];
        @throw e;
    }
    
    @try
    {
        // Swap over the event queue.
        [_eventStore swap];
        
        // Create bulk event message to post to Collect.
        NSArray *events = [_eventStore read];
        
        if (events.count > 0)
        {
            DDNALogDebug(@"Sending latest events to Collect");
            [self postEvents:events completionHandler:^(bool succeeded)
             {
                 if (succeeded)
                 {
                     DDNALogDebug(@"Event upload successful");
                     [_eventStore clear];
                 }
                 else
                 {
                     DDNALogDebug(@"Event upload failed - try again later...");
                 }
             }];
        }
        else
        {
            DDNALogDebug(@"No events to upload");
        }
    }
    @catch (NSException *exception)
    {
        DDNALogDebug(@"Event upload failed: %@", exception.reason);
    }
}

- (void) clearPersistentData
{
    [DDNAPlayerPrefs clear];
    _reset = YES;
}

#pragma mark - Client Configuration Properties

- (NSString *) userID
{
    return [DDNAPlayerPrefs getObjectForKey:PP_KEY_USER_ID withDefault:nil];
}

- (void) setUserID:(NSString *)userID
{
    if (![NSString stringIsNilOrEmpty:userID])
    {
        [DDNAPlayerPrefs setObject:userID forKey:PP_KEY_USER_ID];
        [DDNAPlayerPrefs save];
    }
}

- (NSString *) hashSecret
{
    NSString *v = [DDNAPlayerPrefs getObjectForKey:PP_KEY_HASH_SECRET withDefault:nil];
    if ([NSString stringIsNilOrEmpty:v])
    {
        return nil;
    }
    return v;
}

- (void) setHashSecret:(NSString *)hashSecret
{
    [DDNAPlayerPrefs setObject:hashSecret forKey:PP_KEY_HASH_SECRET];
    [DDNAPlayerPrefs save];
}

- (NSString *) clientVersion
{
    NSString *v = [DDNAPlayerPrefs getObjectForKey:PP_KEY_CLIENT_VERSION withDefault:nil];
    if ([NSString stringIsNilOrEmpty:v])
    {
        return nil;
    }
    return v;
}

- (void) setClientVersion:(NSString *)clientVersion
{
    if (![NSString stringIsNilOrEmpty:clientVersion])
    {
        [DDNAPlayerPrefs setObject:clientVersion forKey:PP_KEY_CLIENT_VERSION];
        [DDNAPlayerPrefs save];
    }
}

- (NSString *) pushNotificationToken
{
    NSString *v = [DDNAPlayerPrefs getObjectForKey:PP_KEY_PUSH_NOTIFICATION_TOKEN withDefault:nil];
    if ([NSString stringIsNilOrEmpty:v])
    {
        DDNALogWarn(@"No push notification token set, sending push notifications will be unavailable");
        return nil;
    }
    return v;
}

- (void) setPushNotificationToken:(NSString *)pushNotificationToken
{
    if (![NSString stringIsNilOrEmpty:pushNotificationToken])
    {
        [DDNAPlayerPrefs setObject:pushNotificationToken forKey:PP_KEY_PUSH_NOTIFICATION_TOKEN];
        [DDNAPlayerPrefs save];
    }
}


#pragma mark - Private Helpers

- (NSString *) mungeUrl: (NSString *)url
{
    NSString *lowerCase = [url lowercaseString];
    if (![lowerCase hasPrefix:@"http://"] && ![lowerCase hasPrefix:@"https://"]) {
        return [@"http://" stringByAppendingString:url];
    } else {
        return url;
    }
}

- (void) postEvents:(NSArray *)events completionHandler: (void(^)(bool)) callback
{
    NSString *joinedEvents = [events componentsJoinedByString:@","];
    NSString *bulkEvent = [NSString stringWithFormat:@"%@%@%@", @"{\"eventList\":[", joinedEvents, @"]}"];
    
    NSString *url;
    if (self.hashSecret != nil)
    {
        NSString *md5Hash = [DDNASDK generateHashForData:bulkEvent
                                          withHashSecret:self.hashSecret];
        
        url = [DDNASDK formatURIWithPattern:DDNA_COLLECT_HASH_URL_PATTERN
                                    forHost:self.collectURL
                             forEnvironment:self.environmentKey
                                withMD5Hash:md5Hash];
    }
    else
    {
        url = [DDNASDK formatURIWithPattern:DDNA_COLLECT_URL_PATTERN
                                    forHost:self.collectURL
                             forEnvironment:self.environmentKey];
    }
    
    // Wrap up calling HttpPost method so it can be called repeatedly in the background with GCD.
    [self httpPostDispatcher:url
                    withData:bulkEvent
                    attempts:self.settings.httpRequestMaxTries
           completionHandler:^(bool success) {
               callback(success);
           }];
}

- (void) engageRequest:(NSString *) data completionHandler: (void(^)(NSString *)) callback
{
    NSString *url;
    if (self.hashSecret != nil)
    {
        NSString *md5Hash = [DDNASDK generateHashForData:data
                                          withHashSecret:self.hashSecret];
        
        url = [DDNASDK formatURIWithPattern:DDNA_ENGAGE_HASH_URL_PATTERN
                                    forHost:self.engageURL
                             forEnvironment:self.environmentKey
                                withMD5Hash:md5Hash];
    }
    else
    {
        url = [DDNASDK formatURIWithPattern:DDNA_ENGAGE_URL_PATTERN
                                    forHost:self.engageURL
                             forEnvironment:self.environmentKey];
    }
    
    [self httpPost:url withData:data completionHandler:^(int status, NSString * response) {
           if (status == 200)
           {
               callback(response);
           }
           else
           {
               DDNALogDebug(@"Error requesting engagement, Engage returned: %i", status);
               callback(nil);
           }
       }];
}

- (void) httpPostDispatcher: (NSString *) url
                   withData: (NSString *) data
                   attempts: (int) attempts
          completionHandler: (void(^)(bool)) callback
{
    if (attempts > 0)
    {
        [self httpPost:url withData:data completionHandler:^(int status, NSString *response) {
            if (status == 200 || status == 204)
            {
                callback(true);
            }
            else
            {
                // trigger again after delay
                DDNALogDebug(@"Retrying request, %i attempts remain", attempts);
                dispatch_time_t delay = dispatch_time(DISPATCH_TIME_NOW,
                                                      self.settings.httpRequestRetryDelaySeconds*NSEC_PER_SEC);
                dispatch_after(delay, dispatch_get_main_queue(), ^{
                    [self httpPostDispatcher:url withData:data attempts:attempts-1 completionHandler:callback];
                });
            }
        }];
    }
    else
    {
        callback(false);
    }
}

- (void) httpPost:(NSString *) url withData:(NSString *) data completionHandler: (void(^)(int, NSString *)) callback
{
    DDNALogDebug(@"HttpPost called with %@ and data %@", url, data);
    
    NSMutableURLRequest *request = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:url]
                                                           cachePolicy:NSURLRequestUseProtocolCachePolicy
                                                       timeoutInterval:self.settings.httpRequestTimeoutSeconds];
    
    NSData *postData = [data dataUsingEncoding:NSUTF8StringEncoding];
    
    [request setHTTPMethod:@"POST"];
    [request setHTTPBody:postData];
    [request setValue:@"application/json" forHTTPHeaderField:@"Accept"];
    [request setValue:@"application/json" forHTTPHeaderField:@"Content-Type"];
    [request setValue:@"100-continue" forHTTPHeaderField:@"Expect"];
    
    [NSURLConnection sendAsynchronousRequest:request
                                       queue:[NSOperationQueue mainQueue]
                           completionHandler:^(NSURLResponse *response, NSData *data, NSError *connectionError) {
        NSHTTPURLResponse *httpResponse = (NSHTTPURLResponse *)response;
        NSString *responseStr = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
        DDNALogDebug(@"Server responded: status %li response: %@ error:%@",
                     (long)httpResponse.statusCode, responseStr, connectionError);
        callback((int)httpResponse.statusCode, responseStr);
    }];
}

+ (NSString *) getCurrentTimestamp
{
    NSDateFormatter *dateFormatter = [[NSDateFormatter alloc] init];
    NSLocale *enUSPOSIXLocale = [[NSLocale alloc] initWithLocaleIdentifier:@"en_US_POSIX"];
    [dateFormatter setLocale:enUSPOSIXLocale];
    [dateFormatter setDateFormat:@"yyyy-MM-dd HH:mm:ss.SSS"];
    
    NSDate *now = [NSDate date];
    return [dateFormatter stringFromDate:now];
}

+ (NSString *) formatURIWithPattern: (NSString *) pattern
                            forHost: (NSString *) host
                     forEnvironment: (NSString *) environmentKey
{
    return [self formatURIWithPattern:pattern
                              forHost:host
                       forEnvironment:environmentKey
                          withMD5Hash:nil];
}

+ (NSString *) formatURIWithPattern: (NSString *)pattern
                            forHost: (NSString *)host
                     forEnvironment: (NSString *)environmentKey
                        withMD5Hash: (NSString *)md5Hash
{
    NSString *uri = [pattern stringByReplacingOccurrencesOfString:@"{host}" withString:host];
    uri = [uri stringByReplacingOccurrencesOfString:@"{env_key}" withString:environmentKey];
    if (md5Hash != nil)
    {
        uri = [uri stringByReplacingOccurrencesOfString:@"{hash}" withString:md5Hash];
    }
    return uri;
}

+ (NSString *) generateHashForData: (NSString *) data
                    withHashSecret: (NSString *) hashSecret
{
    NSString *input = [data stringByAppendingString:hashSecret];
    return [input md5];
}

+ (NSString *) generateUserID
{
    return [[NSUUID UUID] UUIDString];
}

+ (NSString *) generateSessionID
{
    return [[NSUUID UUID] UUIDString];
}

- (void) triggerDefaultEvents
{
    if (_settings.onFirstRunSendNewPlayerEvent && [DDNAPlayerPrefs getIntegerForKey:PP_KEY_FIRST_RUN withDefault:1])
    {
        DDNALogDebug(@"Sending 'newPlayer' event");
        
        NSMutableDictionary *eventParams = [NSMutableDictionary dictionary];
        if ([DDNAClientInfo sharedInstance].countryCode!=nil) {
            [eventParams setObject:[DDNAClientInfo sharedInstance].countryCode forKey:@"userCountry"];
        }
        
        [self recordEvent:@"newPlayer" withEventDictionary:eventParams];
        
        [DDNAPlayerPrefs setInteger:0 forKey:PP_KEY_FIRST_RUN];
        [DDNAPlayerPrefs save];
    }
    
    if (_settings.onStartSendGameStartedEvent)
    {
        DDNALogDebug(@"Sending 'gameStarted' event");
        
        NSMutableDictionary *eventParams = [NSMutableDictionary dictionary];
        if (self.clientVersion != nil)
        {
            [eventParams setObject:self.clientVersion forKey:@"clientVersion"];
        }
        
        if (self.pushNotificationToken != nil)
        {
            [eventParams setObject:self.pushNotificationToken forKey:@"pushNotificationToken"];
        }
        
        [self recordEvent:@"gameStarted" withEventDictionary:eventParams];
    }
    
    if (_settings.onStartSendClientDeviceEvent)
    {
        DDNALogDebug(@"Sending 'clientDevice' event");
        
        NSMutableDictionary *eventParams = [NSMutableDictionary dictionary];
        [eventParams setObject:[DDNAClientInfo sharedInstance].deviceName forKey:@"deviceName"];
        [eventParams setObject:[DDNAClientInfo sharedInstance].deviceType forKey:@"deviceType"];
        if ([DDNAClientInfo sharedInstance].hardwareVersion!=nil) {
            [eventParams setObject:[DDNAClientInfo sharedInstance].hardwareVersion forKey:@"hardwareVersion"];
        }
        [eventParams setObject:[DDNAClientInfo sharedInstance].operatingSystem forKey:@"operatingSystem"];
        [eventParams setObject:[DDNAClientInfo sharedInstance].operatingSystemVersion forKey:@"operatingSystemVersion"];
        [eventParams setObject:[DDNAClientInfo sharedInstance].manufacturer forKey:@"manufacturer"];
        [eventParams setObject:[DDNAClientInfo sharedInstance].timezoneOffset forKey:@"timezoneOffset"];
        if ([DDNAClientInfo sharedInstance].languageCode!=nil) {
            [eventParams setObject:[DDNAClientInfo sharedInstance].languageCode forKey:@"userLanguage"];
        }
        
        [self recordEvent:@"clientDevice" withEventDictionary:eventParams];
    }
}

- (void) didReceiveNotification:(NSNotification *)notification
{
    if ([[notification name] isEqualToString:DD_EVENT_STARTED])
    {
        DDNALogDebug(@"Received SDK started notification");
        dispatch_resume(_taskQueue);
    }
}

@end
