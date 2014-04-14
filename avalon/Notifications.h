#ifndef AVALON_NOTIFICATIONS_H
#define AVALON_NOTIFICATIONS_H

#include <string>
#include <vector>
#include <unordered_map>

namespace avalon {
    
    
class LocalNotificationsDelegate
{
public:
    virtual void onLocalNotification(bool active, const std::string &message, const std::string &sound, unsigned badgeNumber, const std::unordered_map<std::string,std::string> &userDict) = 0;
    virtual ~LocalNotificationsDelegate() {}
};
    
class RemoteNotificationsDelegate
{
public:
    virtual void onRemoteNotification(bool active, const std::string &message, const std::string &sound, unsigned badgeNumber, const std::unordered_map<std::string,std::string> &userDict) = 0;
    virtual void onRegisterForRemoteNotifications(const std::vector<unsigned char> &data, const std::string &error) = 0;
    virtual ~RemoteNotificationsDelegate() {}
};

class Notifications
{
public:
    static void schedule(const std::string &message, long long time, int id, const std::string &sound = "", unsigned badgeNumber = 0, const std::unordered_map<std::string,std::string> &userDict = {});
    static void cancel(int id);
    static void cancelAll();
    static bool isScheduled(int id);
    static std::vector<std::string> getScheduledIds();
    
    static void setBadgeNumber(unsigned value);
    static unsigned getBadgeNumber();
    
    static void setLocalNotificationsDelegate(LocalNotificationsDelegate *delegate);
    static void setRemoteNotificationsDelegate(RemoteNotificationsDelegate *delegate);
    
    static void registerForRemoteNotification();
    static void unregisterForRemoteNotifications();

    
    /*– scheduleLocalNotification:
    – presentLocalNotificationNow:
    – cancelLocalNotification:
    – cancelAllLocalNotifications
    scheduledLocalNotifications  property

    
    – registerForRemoteNotificationTypes:
    – unregisterForRemoteNotifications
    – enabledRemoteNotificationTypes*/
};

} // namespace avalon

#endif /* AVALON_NOTIFICATIONS_H */
