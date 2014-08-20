#include "avalon/Notifications.h"

@interface NotificationChecker : NSObject
@end

static BOOL _launchedWithNotification = NO;
static NSDictionary *_notificationDictionary = nil;

static avalon::LocalNotificationsDelegate *_localNotificationsDelegate = nullptr;
static avalon::RemoteNotificationsDelegate *_remoteNotificationsDelegate = nullptr;

namespace avalon {
    
void Notifications::schedule(const std::string &message, long long time, int id, const std::string &sound, unsigned badgeNumber, const std::unordered_map<std::string,std::string> &userDict)
{
}

void Notifications::cancel(int id)
{
}
    
void Notifications::cancelAll()
{
}
    
bool Notifications::isScheduled(int id)
{
    return false;
}
    
std::vector<std::string> Notifications::getScheduledIds()
{
    std::vector<std::string> ret;
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
}
    
unsigned Notifications::getBadgeNumber()
{
    return 0;
}
    
void Notifications::registerForRemoteNotification()
{
}
    
void Notifications::unregisterForRemoteNotifications()
{
}
    
    
    
}


@implementation NotificationChecker

+ (void)load
{
    [NSApplication sharedApplication].delegate = nil;
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(didFinishLaunching:)
                                                 name: NSApplicationDidFinishLaunchingNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(didReceiveLocalNotification:)
                                                 name: @"NSApplicationDidReceiveLocalNotification" object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(didReceiveRemoteNotification:)
                                                 name: @"NSApplicationDidReceiveRemoteNotification" object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(willEnterForeground:)
                                                 name: NSApplicationWillBecomeActiveNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(didRegisterForRemoteNotifications:)
                                                 name: @"NSApplicationDidRegisterForRemoteNotificationsNotification" object:nil];
}

+ (void)willEnterForeground:(NSNotification *)notification
{
    _launchedWithNotification = NO;
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
    bool active = true;
    if(!_launchedWithNotification && !active)
        _launchedWithNotification = YES;
    
    if(_localNotificationsDelegate)
    {
        std::string message;
        std::string sound;
        unsigned badge = 0;
        std::unordered_map<std::string,std::string> userParams;
        
        NSEnumerator *enumerator = [notification.userInfo keyEnumerator];
        id key;
        while ((key = [enumerator nextObject])) {
            id value = [notification.userInfo objectForKey:key];
            if ([value respondsToSelector:@selector(cStringUsingEncoding:)])
            {
                const char *temp =[value cStringUsingEncoding:NSUTF8StringEncoding];
                if(temp)
                    userParams.insert(std::make_pair([key cStringUsingEncoding:NSUTF8StringEncoding], temp));
            }
        }
        _localNotificationsDelegate->onLocalNotification(active, message, sound, badge, userParams);
    }
}

+ (void)didReceiveRemoteNotification:(NSNotification *)notification
{
    bool active = true;
    if(!_launchedWithNotification && !active)
        _launchedWithNotification = YES;
    
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
    
    NSDictionary *userInfo = [launchOptions objectForKey: NSApplicationLaunchUserNotificationKey];
    if (userInfo)
    {
        _launchedWithNotification = YES;
        NSDictionary *dict = [NSDictionary dictionaryWithDictionary:userInfo];
        [dict setValue:[NSNumber numberWithBool:YES] forKey:@"remote"];
        [dict setValue:[NSNumber numberWithBool:YES] forKey:@"active"];
        _notificationDictionary = dict;
        [_notificationDictionary retain];
        return;
    }
}

@end

