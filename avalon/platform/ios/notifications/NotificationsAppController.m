//
//  NotificationsAppController.m
//  Avalon
//
//  Created by Sergey on 11.04.14.
//
//

#import "NotificationsAppController.h"

@implementation NotificationsAppController

- (void)application:(UIApplication *)application didReceiveRemoteNotification:(NSDictionary *)userInfo
{
    [[NSNotificationCenter defaultCenter] postNotificationName:@"UIApplicationDidReceiveRemoteNotification" object:nil userInfo:userInfo];
}
- (void)application:(UIApplication *)application didReceiveLocalNotification:(UILocalNotification *)notification
{
    [[NSNotificationCenter defaultCenter] postNotificationName:@"UIApplicationDidReceiveLocalNotification" object:nil userInfo:[NSDictionary dictionaryWithObject:notification forKey:@"notification"]];
}

- (void)application:(UIApplication *)application didRegisterForRemoteNotificationsWithDeviceToken:(NSData *)deviceToken
{
    [[NSNotificationCenter defaultCenter] postNotificationName:@"UIApplicationDidRegisterForRemoteNotificationsNotification" object:nil userInfo:[NSDictionary dictionaryWithObjectsAndKeys: deviceToken, @"data", nil]];
}
- (void)application:(UIApplication *)application didFailToRegisterForRemoteNotificationsWithError:(NSError *)error
{
    [[NSNotificationCenter defaultCenter] postNotificationName:@"UIApplicationDidRegisterForRemoteNotificationsNotification" object:nil userInfo:[NSDictionary dictionaryWithObjectsAndKeys: error, @"error", nil]];
}

@end
