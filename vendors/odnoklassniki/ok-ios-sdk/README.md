Join our chat on the app: Telegram Messenger, https://telegram.me/joinchat/An0xvgHDHvWlSWNQWuzOkQ 

###OK IOS SDK 2.0.14

[![Join the chat at https://gitter.im/apiok/ok-ios-sdk](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/apiok/ok-ios-sdk?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

If you are looking for the old version, please checkout tag 1.0
####How to use
First you should select External and IOS platforms and enable Client OAuth authorization using ok.ru app edit form. 
Also your should send request for LONG_ACCESS_TOKEN to [api-support](mailto:api-support@ok.ru) or you can simple not request for LONG_ACCESS_TOKEN permission during OAuth authorization.

Add *ok{appId}* schema to your app Info.plist file. For example *ok12345* if your app has appId *12345*.
Don't forget add ok{appId}://authorize to allowed redirect urls for your application in ok.ru app profile. Also you should add next block to your Info.plist file.
```xml
 <key>NSAppTransportSecurity</key>
    <dict>
        <key>NSAllowsArbitraryLoads</key>
        <true/>
    </dict>
```

Add OKSDK.h and OKSDK.m to your project. For example you can use git submodule.

Init your sdk in AppDelegate didFinishLaunchingWithOptions
```objective-c
- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    OKSDKInitSettings *settings = [OKSDKInitSettings new];
    settings.appKey = @"ABCDEFGABCDEGF";
    settings.appId = @"12345";
    settings.controllerHandler = ^{
        return self.window.rootViewController;
    };
    [OKSDK initWithSettings: settings];
    return YES;
}
```

Add openUrl to AppDelegate openURL
```objective-c
-(BOOL)application:(UIApplication *)application openURL:(NSURL *)url sourceApplication:(NSString *)sourceApplication annotation:(id)annotation {
    [OKSDK openUrl:url];
    return YES;
}
```

To understand how to interact with OKSDK please look at examples  [repository](https://github.com/apiok/ok-ios-sdk-examples)



