#include "avalon/Notifications.h"

@interface MacNotificationDelegate : NSObject<NSUserNotificationCenterDelegate>
{
    avalon::LocalNotificationsDelegate* _delegate;
}
-(id)initWithDelegate:(avalon::LocalNotificationsDelegate*) delegate;
@end

static MacNotificationDelegate *macDelegate = nil;

static avalon::LocalNotificationsDelegate *_localNotificationsDelegate = nullptr;
static avalon::RemoteNotificationsDelegate *_remoteNotificationsDelegate = nullptr;

namespace avalon {
    
void Notifications::schedule(const std::string &message, long long time, int id, const std::string &sound, unsigned badgeNumber, const std::unordered_map<std::string,std::string> &userDict)
{
    //Initalize new notification
    NSUserNotification *notification = [[NSUserNotification alloc] init];
    //Set the title of the notification
    [notification setTitle:[[[NSBundle mainBundle] localizedInfoDictionary] objectForKey:@"CFBundleDisplayName"]];
    //Set the text of the notification
    [notification setInformativeText:[NSString stringWithCString:message.c_str() encoding:NSUTF8StringEncoding]];
    //Set the time and date on which the nofication will be deliverd (for example 20 secons later than the current date and time)
    [notification setDeliveryDate:[NSDate dateWithTimeIntervalSince1970:time]];
    //Set the sound, this can be either nil for no sound, NSUserNotificationDefaultSoundName for the default sound (tri-tone) and a string of a .caf file that is in the bundle (filname and extension)
    if(sound.empty())
        notification.soundName = NSUserNotificationDefaultSoundName;
    else
        notification.soundName = [NSString stringWithCString:sound.c_str() encoding:NSUTF8StringEncoding];
    
    notification.hasActionButton = NO;
    
    NSNumber* key = [NSNumber numberWithInt:id];
    NSDictionary *infoDict = [NSDictionary dictionaryWithObject:key forKey:@"_id"];
    notification.userInfo = infoDict;
    
    //Get the default notification center
    NSUserNotificationCenter *center = [NSUserNotificationCenter defaultUserNotificationCenter];
    //Scheldule our NSUserNotification
    [center scheduleNotification:notification];
}

void Notifications::cancel(int id)
{
    NSUserNotificationCenter *center = [NSUserNotificationCenter defaultUserNotificationCenter];
    NSNumber* key = [NSNumber numberWithInt:id];
    for(NSUserNotification *notification in [center scheduledNotifications]) {
        if([[notification.userInfo objectForKey:@"_id"] isEqualToNumber:key]) {
            [center removeScheduledNotification:notification];
        }
    }
}
    
void Notifications::cancelAll()
{
    NSUserNotificationCenter *center = [NSUserNotificationCenter defaultUserNotificationCenter];
    for(NSUserNotification *notification in [center scheduledNotifications]) {
        [center removeScheduledNotification:notification];
    }

}
    
bool Notifications::isScheduled(int id)
{
    NSUserNotificationCenter *center = [NSUserNotificationCenter defaultUserNotificationCenter];
    NSNumber* key = [NSNumber numberWithInt:id];
    for(NSUserNotification *notification in [center scheduledNotifications]) {
        if([[notification.userInfo objectForKey:@"_id"] isEqualToNumber:key]) {
            return true;
        }
    }
    return false;
}
    
std::vector<int> Notifications::getScheduledIds()
{
    NSUserNotificationCenter *center = [NSUserNotificationCenter defaultUserNotificationCenter];
    std::vector<int> ret;
    for(NSUserNotification *notification in [center scheduledNotifications]) {
        ret.push_back([[notification.userInfo objectForKey:@"_id"] intValue]);
    }
    return ret;
}
    
void Notifications::setLocalNotificationsDelegate(LocalNotificationsDelegate *delegate)
{
    NSUserNotificationCenter *center = [NSUserNotificationCenter defaultUserNotificationCenter];
    _localNotificationsDelegate = delegate;
    macDelegate = [[MacNotificationDelegate alloc] initWithDelegate:delegate];
    if(delegate)
    {
        [center setDelegate:[[MacNotificationDelegate alloc] initWithDelegate:delegate]];
    }
    else
    {
        [center setDelegate:nil];
    }
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
    
const Notification* Notifications::getLaunchedNotification()
{
    return nullptr;
}
    
}


@implementation MacNotificationDelegate

-(id)initWithDelegate:(avalon::LocalNotificationsDelegate*) delegate
{
    self = [super init];
    if(self)
    {
        _delegate = delegate;
    }
    return self;
}

-(void) dealloc
{
    
}

- (void)userNotificationCenter:(NSUserNotificationCenter *)center didDeliverNotification:(NSUserNotification *)notification
{
    
}

- (void)userNotificationCenter:(NSUserNotificationCenter *)center didActivateNotification:(NSUserNotification *)notification
{
    if(_delegate)
    {
        std::string message;
        std::string sound;
        std::unordered_map<std::string,std::string> userParams;
        
        if(notification.informativeText)
            message = [notification.informativeText cStringUsingEncoding:NSUTF8StringEncoding];
        if(notification.soundName)
            sound = [notification.soundName cStringUsingEncoding:NSUTF8StringEncoding];
        
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

        _delegate->onLocalNotification(true, [[notification.userInfo objectForKey:@"_id"] intValue], message, sound, 0, userParams);
    }
    
}

- (BOOL)userNotificationCenter:(NSUserNotificationCenter *)center shouldPresentNotification:(NSUserNotification *)notification
{
    return YES;
}

@end

