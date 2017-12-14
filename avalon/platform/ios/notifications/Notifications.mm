#include "avalon/Notifications.h"
#import <UIKit/UIKit.h>

@interface NotificationChecker : NSObject
@end

static NSDictionary *_notificationDictionary = nil;

static avalon::LocalNotificationsDelegate *_localNotificationsDelegate = nullptr;
static avalon::RemoteNotificationsDelegate *_remoteNotificationsDelegate = nullptr;

namespace avalon {
    
void Notifications::schedule(const std::string &message, long long time, int id, const std::string &sound, unsigned badgeNumber, const std::unordered_map<std::string,std::string> &userDict)
{
    UILocalNotification *notification = [[UILocalNotification alloc] init];
    
    notification.fireDate = [NSDate dateWithTimeIntervalSince1970:time];
    notification.timeZone = nil;
    notification.alertBody = [NSString stringWithCString:message.c_str() encoding:NSUTF8StringEncoding];
    notification.alertAction = @"Open";
    notification.applicationIconBadgeNumber = badgeNumber;
    if(sound.empty())
        notification.soundName = UILocalNotificationDefaultSoundName;
    else
        notification.soundName = [NSString stringWithCString:sound.c_str() encoding:NSUTF8StringEncoding];
    
    NSNumber* key = [NSNumber numberWithInt:id];
    NSMutableDictionary *infoDict = [NSMutableDictionary dictionaryWithObject:key forKey:@"_id"];
    for(const auto &pair: userDict)
    {
        [infoDict setObject:[NSString stringWithUTF8String:pair.second.c_str()] forKey: [NSString stringWithUTF8String:pair.first.c_str()]];
    }
    notification.userInfo = infoDict;
    
    [[UIApplication sharedApplication] scheduleLocalNotification:notification];
}

void Notifications::cancel(int id)
{
    NSNumber* key = [NSNumber numberWithInt:id];
    for(UILocalNotification *notification in [[UIApplication sharedApplication] scheduledLocalNotifications]) {
        if([[notification.userInfo objectForKey:@"_id"] isEqualToNumber:key]) {
            [[UIApplication sharedApplication] cancelLocalNotification:notification];
        }
    }
}
    
void Notifications::cancelAll()
{
    [[UIApplication sharedApplication] cancelAllLocalNotifications];
}
    
bool Notifications::isScheduled(int id)
{
    NSNumber* key = [NSNumber numberWithInt:id];
    for(UILocalNotification *notification in [[UIApplication sharedApplication] scheduledLocalNotifications]) {
        if([[notification.userInfo objectForKey:@"_id"] isEqualToNumber:key]) {
            return true;
        }
    }
    return false;
}
    
std::vector<int> Notifications::getScheduledIds()
{
    std::vector<int> ret;
    for(UILocalNotification *notification in [[UIApplication sharedApplication] scheduledLocalNotifications]) {
        ret.push_back([[notification.userInfo objectForKey:@"_id"] intValue]);
    }
    return ret;
}
    
void Notifications::setLocalNotificationsDelegate(LocalNotificationsDelegate *delegate)
{
    _localNotificationsDelegate = delegate;
}
void Notifications::setRemoteNotificationsDelegate(RemoteNotificationsDelegate *delegate)
{
    _remoteNotificationsDelegate = delegate;
}
    
void Notifications::setBadgeNumber(unsigned value)
{
    [UIApplication sharedApplication].applicationIconBadgeNumber = value;
}
    
unsigned Notifications::getBadgeNumber()
{
    return  static_cast<unsigned>([UIApplication sharedApplication].applicationIconBadgeNumber);
}
    
void Notifications::registerForRemoteNotification()
{
#if __IPHONE_OS_VERSION_MAX_ALLOWED >= 80000
    if ([[UIApplication sharedApplication] respondsToSelector:@selector(registerUserNotificationSettings:)]) {
        [[UIApplication sharedApplication] registerForRemoteNotifications];
    } else {
        [[UIApplication sharedApplication] registerForRemoteNotificationTypes:UIRemoteNotificationTypeBadge | UIRemoteNotificationTypeSound|UIRemoteNotificationTypeAlert];
    }
#else
    [[UIApplication sharedApplication] registerForRemoteNotificationTypes:UIRemoteNotificationTypeBadge | UIRemoteNotificationTypeSound|UIRemoteNotificationTypeAlert];
#endif
}
    
void Notifications::unregisterForRemoteNotifications()
{
    [[UIApplication sharedApplication] unregisterForRemoteNotifications];
}
    
const Notification* Notifications::getLaunchedNotification()
{
    if(!_notificationDictionary)
        return nullptr;
    
    static Notification notification;
    notification.id = -1;
    notification.badgeNumber = 0;
    notification.message.clear();
    notification.sound.clear();
    
    std::unordered_map<std::string,std::string> params;
    
    for(id keyObject in _notificationDictionary)
    {
        id valueObject = [_notificationDictionary objectForKey:keyObject];
        std::string key = [keyObject cStringUsingEncoding:NSUTF8StringEncoding];
        if (key == "_id")
        {
            notification.id = [valueObject intValue];
        }
        else if (key == "aps")
        {
            id alertObject = [valueObject objectForKey:@"alert"];
            if(alertObject)
                notification.message = [alertObject cStringUsingEncoding:NSUTF8StringEncoding];
            id soundObject = [valueObject objectForKey:@"sound"];
            if(soundObject)
                notification.sound = [soundObject cStringUsingEncoding:NSUTF8StringEncoding];
            id badgeObject = [valueObject objectForKey:@"badge"];
            if(badgeObject)
                notification.badgeNumber = [badgeObject intValue];
        }
        else if (key == "_remote")
        {
            if([valueObject boolValue])
                notification.id = -1;
        }
        else
        {
            if([valueObject respondsToSelector:@selector(cStringUsingEncoding:)])
                params[key] = [valueObject cStringUsingEncoding:NSUTF8StringEncoding];
            else if ([valueObject respondsToSelector:@selector(stringValue)])
                params[key] = [[valueObject stringValue] cStringUsingEncoding:NSUTF8StringEncoding];
        }
    }
        
    notification.userDict = params;
    
    return &notification;
}
    
}


@implementation NotificationChecker

+ (void)load
{
    [UIApplication sharedApplication].delegate = nil;
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(didFinishLaunching:)
                                                 name: UIApplicationDidFinishLaunchingNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(didReceiveLocalNotification:)
                                                 name: @"UIApplicationDidReceiveLocalNotification" object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(didReceiveRemoteNotification:)
                                                 name: @"UIApplicationDidReceiveRemoteNotification" object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(willEnterForeground:)
                                                 name: UIApplicationWillEnterForegroundNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(didRegisterForRemoteNotifications:)
                                                 name: @"UIApplicationDidRegisterForRemoteNotificationsNotification" object:nil];
}

+ (void)willEnterForeground:(NSNotification *)notification
{
    _notificationDictionary = nil;
}

+ (void)didRegisterForRemoteNotifications:(NSNotification *)notification
{
    if(_remoteNotificationsDelegate)
    {
        std::vector<unsigned char> token;
        std::string errorString;
        NSData *data = [notification.userInfo objectForKey:@"data"];
        if(data)
        {
            token.resize(data.length);
            memcpy(&token.front(), data.bytes, data.length);
        }
        NSError *error = [notification.userInfo objectForKey:@"error"];
        if(error)
        {
            errorString = [[error localizedDescription] cStringUsingEncoding:NSUTF8StringEncoding];
        }
        _remoteNotificationsDelegate->onRegisterForRemoteNotifications(token, errorString);
    }
}

+ (void)didReceiveLocalNotification:(NSNotification *)notification
{
    bool active = [UIApplication sharedApplication].applicationState  == UIApplicationStateActive;
    if(!active)
    {
        UILocalNotification *localNotification = [notification.userInfo objectForKey:@"notification"];
        if (localNotification)
        {
            NSMutableDictionary *dict = [NSMutableDictionary dictionaryWithDictionary:localNotification.userInfo];
            [dict setObject:[NSNumber numberWithBool:NO] forKey:@"_remote"];
            [dict setObject:[NSDictionary dictionaryWithObjectsAndKeys:localNotification.alertBody, @"alert", [NSNumber numberWithInteger:localNotification.applicationIconBadgeNumber], @"badge", localNotification.soundName, @"sound", nil] forKey:@"aps"];
            _notificationDictionary = dict;
            return;
        }
    }
    
    if(_localNotificationsDelegate)
    {
        std::string message;
        std::string sound;
        unsigned badge = 0;
        std::unordered_map<std::string,std::string> userParams;
    
        UILocalNotification *localNotification = [notification.userInfo objectForKey:@"notification"];
        
        if(localNotification.alertBody)
            message = [localNotification.alertBody cStringUsingEncoding:NSUTF8StringEncoding];
        if(localNotification.soundName)
            sound = [localNotification.soundName cStringUsingEncoding:NSUTF8StringEncoding];
        badge = static_cast<unsigned>(localNotification.applicationIconBadgeNumber);
        
        NSEnumerator *enumerator = [notification.userInfo keyEnumerator];
        id key;
        while ((key = [enumerator nextObject])) {
            id value = [notification.userInfo objectForKey:key];
            if (![key isEqualToString:@"_id"] && [value respondsToSelector:@selector(cStringUsingEncoding:)])
            {
                const char *temp =[value cStringUsingEncoding:NSUTF8StringEncoding];
                if(temp)
                    userParams.insert(std::make_pair([key cStringUsingEncoding:NSUTF8StringEncoding], temp));
            }
        }
        _localNotificationsDelegate->onLocalNotification(active, [[notification.userInfo objectForKey:@"_id"] intValue], message, sound, badge, userParams);
    }
}

+ (void)didReceiveRemoteNotification:(NSNotification *)notification
{
    bool active = [UIApplication sharedApplication].applicationState == UIApplicationStateActive;
    if(!active)
    {
        NSMutableDictionary *dict = [NSMutableDictionary dictionaryWithDictionary:notification.userInfo];
        [dict setObject:[NSNumber numberWithBool:YES] forKey:@"_remote"];
        _notificationDictionary = dict;
    }
    if(_remoteNotificationsDelegate)
    {
        NSDictionary *userInfo = notification.userInfo;
        std::string message;
        std::string sound;
        unsigned badge = 0;
        std::unordered_map<std::string,std::string> userParams;
        
        NSEnumerator *enumerator = [notification.userInfo keyEnumerator];
        id key;
        while ((key = [enumerator nextObject])) {
            if([key isEqualToString:@"aps"])
            {
                NSDictionary *apsInfo = [userInfo objectForKey:key];
                
                const char *pMessage = [[apsInfo objectForKey:@"alert"] cStringUsingEncoding:NSUTF8StringEncoding];
                if (pMessage)
                    message = pMessage;
                
                const char *pSound = [[apsInfo objectForKey:@"sound"] cStringUsingEncoding:NSUTF8StringEncoding];
                if (pSound)
                    sound = pSound;
                
                badge = [[apsInfo objectForKey:@"badge"] intValue];
            }
            else
            {
                id value = [notification.userInfo objectForKey:key];
                if ([value respondsToSelector:@selector(cStringUsingEncoding:)])
                {
                    const char *temp =[value cStringUsingEncoding:NSUTF8StringEncoding];
                    if(temp)
                        userParams.insert(std::make_pair([key cStringUsingEncoding:NSUTF8StringEncoding], temp));
                }
            }
        }
        _remoteNotificationsDelegate->onRemoteNotification(active, message, sound, badge, userParams);
    }
}

+ (void)didFinishLaunching:(NSNotification *)notification
{
    NSDictionary *launchOptions = [notification userInfo] ;
    
    if ([UIApplication instancesRespondToSelector:@selector(registerUserNotificationSettings:)]){
        [[UIApplication sharedApplication] registerUserNotificationSettings:[UIUserNotificationSettings settingsForTypes:UIUserNotificationTypeAlert|UIUserNotificationTypeBadge|UIUserNotificationTypeSound categories:nil]];
    }
    
    UILocalNotification *localNotification = [launchOptions objectForKey: UIApplicationLaunchOptionsLocalNotificationKey];
    if (localNotification)
    {
        NSMutableDictionary *dict = [NSMutableDictionary dictionaryWithDictionary:localNotification.userInfo];
        [dict setObject:[NSNumber numberWithBool:NO] forKey:@"_remote"];
        [dict setObject:[NSDictionary dictionaryWithObjectsAndKeys:localNotification.alertBody, @"alert", [NSNumber numberWithInteger:localNotification.applicationIconBadgeNumber], @"badge", localNotification.soundName, @"sound", nil] forKey:@"aps"];
        _notificationDictionary = dict;
        return;
    }
    NSDictionary *userInfo = [launchOptions objectForKey: UIApplicationLaunchOptionsRemoteNotificationKey];
    if (userInfo)
    {
        NSMutableDictionary *dict = [NSMutableDictionary dictionaryWithDictionary:userInfo];
        [dict setObject:[NSNumber numberWithBool:YES] forKey:@"_remote"];
        _notificationDictionary = dict;
        return;
    }
}

@end

