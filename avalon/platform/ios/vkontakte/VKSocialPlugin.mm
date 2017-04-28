//
//  VKSocialPlugin.mm
//  SkillGames
//
//  Created by Sergey on 18/04/2017.
//
//

#import <Foundation/Foundation.h>
#import <VKSdkFramework/VKSdkFramework.h>

#import "VKSocialPlugin.h"
#import "SocialPluginHelpers.h"

@interface IOSVKSdkDelegate : NSObject<VKSdkDelegate, VKSdkUIDelegate>
- (void)vkSdkAccessAuthorizationFinishedWithResult:(VKAuthorizationResult *)result;
- (void)vkSdkUserAuthorizationFailed;
@end

namespace avalon {
    
static std::vector<std::string> split(const char *str, char c)
{
    std::vector<std::string> result;
    
    if (str && *str != '\0')
    {
        do
        {
            const char *begin = str;
            
            while(*str != c && *str)
                str++;
            
            result.push_back(std::string(begin, str));
        } while (0 != *str++);
    }
    
    return result;
}

    
class VKSocialPluginIOS : public VKSocialPlugin
{
public:
    VKSocialPluginIOS()
    : _permissionsHelper({{SocialPermission::Type::PUBLIC_PROFILE, ""}, {SocialPermission::Type::EMAIL, "email"}, {SocialPermission::Type::FRIENDS, "friends"}})
    , _genderHelper(2, 1, 0)
    {
        NSString *VKAppID  = [[NSBundle mainBundle].infoDictionary objectForKey:@"VKAppID"];
        [VKSdk initializeWithAppId:VKAppID];
        _nativeDelegate = [[IOSVKSdkDelegate alloc] init];
        [[VKSdk instance] registerDelegate:_nativeDelegate];
        [VKSdk instance].uiDelegate = _nativeDelegate;
    }
    
    virtual void setDebug(bool value) override
    {
        _debug = value;
    }
    
    virtual void setDelegate(SocialPluginDelegate *delegate) override
    {
        _delegate = delegate;
    }
    
    virtual void login() override
    {
        requestReadPermissions({SocialPermission::Type::PUBLIC_PROFILE, SocialPermission::Type::FRIENDS, SocialPermission::Type::EMAIL});
    }
    
    virtual void login(const std::vector<SocialPermission>& permissions) override
    {
        requestReadPermissions(permissions);
    }
    
    virtual void requestReadPermissions(const std::vector<SocialPermission>& permissions) override
    {
        
        NSMutableArray *permissionArray = [NSMutableArray array];
        for (const auto &it : permissions)
        {
            const std::string& permission = _permissionsHelper.toString(it);
            if(!permission.empty())
                [permissionArray addObject:[NSString stringWithUTF8String:permission.c_str()]];
        }
        _readPermissions = permissions;
        
        if(_debug)
            NSLog(@"VKSocialPlugin::requestPublishPermissions permissions: %@", permissionArray);
        
        std::vector<SocialPermission> localPermissions = permissions;
        
        [VKSdk wakeUpSession:permissionArray completeBlock:^(VKAuthorizationState state, NSError *error) {
            if (state == VKAuthorizationAuthorized) {
                if(_delegate)
                    _delegate->onLogin({SocialPluginDelegate::Error::Type::SUCCESS, 0, ""}, [[VKSdk accessToken].accessToken UTF8String], localPermissions, {});
            } if (state == VKAuthorizationInitialized) {
                [VKSdk authorize:permissionArray];
            }
            else if (error) {
                _readPermissions.clear();
                if(_delegate)
                    _delegate->onLogin({SocialPluginDelegate::Error::Type::UNDEFINED, (int)state, [error.description UTF8String]}, "", {}, {});
            } 
        }];
    }
    
    virtual void requestPublishPermissions(const std::vector<SocialPermission>& permissions) override
    {
        /*
        NSMutableArray *permissionArray = [NSMutableArray array];
        for (const auto &it : permissions)
        {
            [permissionArray addObject:[NSString stringWithUTF8String:_facebookPermissionsHelper.toString(it).c_str()]];
        }

        UIViewController *viewController = [[[UIApplication sharedApplication] keyWindow] rootViewController];
        [_loginManager logInWithPublishPermissions:permissionArray
                             fromViewController:viewController
                                        handler:^(FBSDKLoginManagerLoginResult *result, NSError *error)
         {
             proccessLogin(result, error, true);
         }];
         */
    }

    virtual void logout() override
    {
        if(_debug)
            NSLog(@"VKSocialPlugin::logout logged state: %@", isLoggedIn()?@"true":@"false");
        
        if (isLoggedIn())
        {
            [VKSdk forceLogout];
            
            if (_delegate)
                _delegate->onLogout({SocialPluginDelegate::Error::Type::SUCCESS, 0, ""});
        }
    }
    
    virtual bool isLoggedIn() const override
    {
        if ([VKSdk isLoggedIn])
            return true;
        return false;
    }
    
    virtual std::string getUserID() const override
    {
        if (isLoggedIn())
        {
            NSString *userString = [VKSdk accessToken].userId;
            return [userString UTF8String];
        }
        return "";
    }
    
    virtual std::string getAccessToken() const override
    {
        if (isLoggedIn())
        {
            NSString *tokenString = [VKSdk accessToken].accessToken;
            return [tokenString UTF8String];
        }
        
        return "";
    }
    
    virtual std::string getAppId() const override
    {
        NSString *appId = [[NSBundle mainBundle].infoDictionary objectForKey:@"VKAppID"];
        return [appId UTF8String];
    }
    
    virtual std::vector<SocialPermission> getReadPermissionList() const override
    {
        return _readPermissions;
    }

    virtual std::vector<SocialPermission> getPublishPermissionList() const override
    {
        return _publishPermissions;
    }
    
    virtual void getMyProfile(int preferedPictureSize, void *userData, const std::vector<std::string> &additionalFields) override
    {
        if (isLoggedIn())
        {
            std::string fields = "id,first_name,last_name,nickname,sex,bdate";
            for (const auto &it : additionalFields)
            {
                fields += "," + it;
            }
            
            if(preferedPictureSize<75)
                fields += ",photo_50,photo_100";
            else if(preferedPictureSize<150)
                fields += ",photo_100";
            else
                fields += ",photo_200,photo_100";
            
            VKRequest * profileReq = [[VKApi users] get:@{VK_API_USER_ID : [VKSdk accessToken].userId, @"fields": [NSString stringWithUTF8String:fields.c_str()]}];
            
            [profileReq executeWithResultBlock:^(VKResponse * response) {
                if(_debug)
                    NSLog(@"VKSocialPlugin::getMyProfile result: %@", response.json);
                
                NSDictionary *result = (NSDictionary*)[response.json objectAtIndex:0];
                
                SocialProfile profile;
                
                for (id key in result)
                {
                    id object = [result objectForKey:key];
                    
                    if ([key isEqualToString:@"id"])
                    {
                        profile.uid = [[object stringValue] UTF8String];
                    }
                    else if ([key isEqualToString:@"first_name"])
                    {
                        profile.firstName = [object UTF8String];
                    }
                    else if ([key isEqualToString:@"last_name"])
                    {
                        profile.lastName = [object UTF8String];
                    }
                    else if ([key isEqualToString:@"nickname"])
                    {
                        profile.nickName = [object UTF8String];
                    }
                    else if ([key isEqualToString:@"email"])
                    {
                        profile.email = [object UTF8String];
                    }
                    else if ([key isEqualToString:@"sex"])
                    {
                        profile.gender = _genderHelper.toGender([object intValue]);
                    }
                    else if ([key isEqualToString:@"photo_50"])
                    {
                        if(preferedPictureSize<75)
                            profile.pictureUrl = [object UTF8String];
                    }
                    else if ([key isEqualToString:@"photo_100"])
                    {
                        profile.pictureId = [object UTF8String];
                        
                        if(preferedPictureSize > 75 && preferedPictureSize < 150)
                            profile.pictureUrl = [object UTF8String];
                    }
                    else if ([key isEqualToString:@"photo_200"])
                    {
                        if (preferedPictureSize > 150)
                            profile.pictureUrl = [object UTF8String];
                    }
                    else if ([key isEqualToString:@"bdate"])
                    {
                        std::vector<std::string> parts = split([object UTF8String], '.');
                        if(parts.size() == 2)
                            profile.birthDate = SocialProfile::BirthDate(std::stoi(parts[0]), std::stoi(parts[1]), 0);
                        else if(parts.size() == 3)
                            profile.birthDate = SocialProfile::BirthDate(std::stoi(parts[0]), std::stoi(parts[1]), std::stoi(parts[2]));
                    }
                    else
                    {
                        if ([object isKindOfClass:[NSArray class]] || [object isKindOfClass:[NSDictionary class]])
                        {
                            BOOL ret = [NSJSONSerialization isValidJSONObject:object];
                            if(ret)
                            {
                                NSError *error = nil;
                                NSData *data = [NSJSONSerialization dataWithJSONObject:object options:NSJSONWritingPrettyPrinted error:&error];
                                if(!error)
                                {
                                    std::string value(static_cast<const char*>(data.bytes), data.length);
                                    profile.otherValue.emplace_back([key UTF8String], value);
                                }
                            }
                        }
                        else if ([object isKindOfClass:[NSString class]])
                        {
                            profile.otherValue.emplace_back([key UTF8String], [object UTF8String]);
                        }
                        else if ([object isKindOfClass:[NSNumber class]])
                        {
                            profile.otherValue.emplace_back([key UTF8String], [[object stringValue] UTF8String]);
                        }
                        else
                        {
                            if(_debug)
                                NSLog(@"VKSocialPlugin::getMyProfile Wrong type for object %@ by key %@", object, key);
                        }
                    }
                }
                
                if (_delegate)
                    _delegate->onGetMyProfile({SocialPluginDelegate::Error::Type::SUCCESS, 0, ""}, userData, profile);
                
            } errorBlock:^(NSError * error) {
                if (_delegate)
                    _delegate->onGetMyProfile({SocialPluginDelegate::Error::Type::UNDEFINED, static_cast<int>(error.code), [error.description UTF8String]}, userData, _emptyProfile);
            }];
        }
        else
        {
            if (_delegate)
                _delegate->onGetMyProfile({SocialPluginDelegate::Error::Type::NO_LOGIN, 0, ""}, userData, _emptyProfile);
        }
    }
    
    virtual void getProfiles(const std::vector<std::string> &userIds, void *userData, int preferedPictureSize, const std::vector<std::string> &additionalFields) override
    {
    }
    
    virtual void getAppFriends(int preferedPictureSize, void *userData, const std::vector<std::string> &additionalFields) override
    {
    }
    
    void proccessLogin(VKAuthorizationResult *result)
    {
        if(!result)
        {
            std::vector<SocialPermission> permissions = _readPermissions;
            _readPermissions.clear();
            if(_delegate)
            {
                SocialPluginDelegate::Error socialError = {SocialPluginDelegate::Error::Type::UNDEFINED, 0, ""};
                _delegate->onLogin(socialError, "", {}, permissions);
            }
            if(result.error)
                NSLog(@"VKSocialPlugin::login failed");
            return;
        }
        
        const std::string &token = result.token ? [result.token.accessToken UTF8String] : "";
        
        if(_debug)
        {
            if(result.error)
                NSLog(@"VKSocialPlugin::login error: %@", result.error);
            else
                NSLog(@"VKSocialPlugin::login succes granted permissions");
        }
        
        if (result.error)
        {
            std::vector<SocialPermission> permissions = _readPermissions;
            _readPermissions.clear();
            if(_delegate)
            {
                SocialPluginDelegate::Error socialError = {SocialPluginDelegate::Error::Type::UNDEFINED, static_cast<int>(result.error.code), [result.error.description UTF8String]};
                _delegate->onLogin(socialError, token, {}, permissions);
            }
        }
        else
        {
            if(_delegate)
            {
                SocialPluginDelegate::Error socialError = {SocialPluginDelegate::Error::Type::SUCCESS, 0, ""};
                _delegate->onLogin(socialError, token, _readPermissions, {});
            }
        }
    }

private:
    IOSVKSdkDelegate *_nativeDelegate = nil;
    SocialPluginDelegate *_delegate = nullptr;
    
    std::vector<SocialPermission> _readPermissions;
    std::vector<SocialPermission> _publishPermissions;
    
    SocialProfile _emptyProfile;
    GenderHelper<int> _genderHelper;
    
    bool _debug = false;

    SocialPermissionsHelper<VKPermission> _permissionsHelper;
};


VKSocialPlugin &VKSocialPlugin::getInstance()
{
    static VKSocialPluginIOS instance;
    return instance;
}
}

@implementation IOSVKSdkDelegate
- (void)vkSdkAccessAuthorizationFinishedWithResult:(VKAuthorizationResult *)result
{
    static_cast<avalon::VKSocialPluginIOS&>(avalon::VKSocialPlugin::getInstance()).proccessLogin(result);
}

- (void)vkSdkUserAuthorizationFailed
{
    static_cast<avalon::VKSocialPluginIOS&>(avalon::VKSocialPlugin::getInstance()).proccessLogin(nil);
}

- (void)vkSdkNeedCaptchaEnter:(VKError *)captchaError {
    VKCaptchaViewController *vc = [VKCaptchaViewController captchaControllerWithError:captchaError];
    UIViewController *viewController = [[[UIApplication sharedApplication] keyWindow] rootViewController];
    [vc presentIn:viewController];
}

- (void)vkSdkShouldPresentViewController:(UIViewController *)controller {
    UIViewController *viewController = [[[UIApplication sharedApplication] keyWindow] rootViewController];
    [viewController presentViewController:controller animated:YES completion:nil];
}


@end
