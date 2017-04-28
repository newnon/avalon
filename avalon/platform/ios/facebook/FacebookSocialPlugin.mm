//
//  FacebookSocialPlugin.mm
//  SkillGames
//
//  Created by Vitaly on 14/04/2017.
//
//

#import <Foundation/Foundation.h>
#import <FBSDKCoreKit/FBSDKCoreKit.h>
#import <FBSDKLoginKit/FBSDKLoginManager.h>
#import <FBSDKLoginKit/FBSDKLoginManagerLoginResult.h>

#import "FacebookSocialPlugin.h"
#import "SocialPluginHelpers.h"

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
    
class FacebookSocialPluginIOS : public FacebookSocialPlugin
{
public:
    FacebookSocialPluginIOS()
    : _facebookPermissionsHelper({
        {SocialPermission::Type::PUBLIC_PROFILE, "public_profile"},
        {SocialPermission::Type::EMAIL, "email"},
        {SocialPermission::Type::FRIENDS, "user_friends"}})
    , _genderHelper("male", "female", "")
    {
        _loginManager = [[FBSDKLoginManager alloc] init];
    }
    
    virtual void setLoginBehavior(LoginBehavior type) override
    {
        if (_loginManager)
        {
            switch (type)
            {
                case LoginBehavior::Native:
                    [_loginManager setLoginBehavior:FBSDKLoginBehaviorNative];
                    break;
                case LoginBehavior::Browser:
                    [_loginManager setLoginBehavior:FBSDKLoginBehaviorBrowser];
                    break;
                case LoginBehavior::SystemAccount:
                    [_loginManager setLoginBehavior:FBSDKLoginBehaviorSystemAccount];
                    break;
                case LoginBehavior::Web:
                    [_loginManager setLoginBehavior:FBSDKLoginBehaviorWeb];
                    break;
            };
        }
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
            [permissionArray addObject:[NSString stringWithUTF8String:_facebookPermissionsHelper.toString(it).c_str()]];
        }
        
        if(_debug)
            NSLog(@"FacebookSocialPlugin::requestReadPermissions permissions: %@", permissionArray);
        
        UIViewController *viewController = [[[UIApplication sharedApplication] keyWindow] rootViewController];
        [_loginManager logInWithReadPermissions:permissionArray
                             fromViewController:viewController
                                        handler:^(FBSDKLoginManagerLoginResult *result, NSError *error)
         {
             proccessLogin(result, error, false);
         }];
    }
    
    virtual void requestPublishPermissions(const std::vector<SocialPermission>& permissions) override
    {
        NSMutableArray *permissionArray = [NSMutableArray array];
        for (const auto &it : permissions)
        {
            [permissionArray addObject:[NSString stringWithUTF8String:_facebookPermissionsHelper.toString(it).c_str()]];
        }
        
        if(_debug)
            NSLog(@"FacebookSocialPlugin::requestPublishPermissions permissions: %@", permissionArray);

        UIViewController *viewController = [[[UIApplication sharedApplication] keyWindow] rootViewController];
        [_loginManager logInWithPublishPermissions:permissionArray
                             fromViewController:viewController
                                        handler:^(FBSDKLoginManagerLoginResult *result, NSError *error)
         {
             proccessLogin(result, error, true);
         }];
    }

    virtual void logout() override
    {
        if(_debug)
            NSLog(@"FacebookSocialPlugin::logout logged state: %@", isLoggedIn()?@"true":@"false");
            
        if (isLoggedIn())
        {
            [_loginManager logOut];
            
            if(_debug)
                NSLog(@"FacebookSocialPlugin::logout succes");
            
            if (_delegate)
                _delegate->onLogout({SocialPluginDelegate::Error::Type::SUCCESS, 0, ""});
        }
    }
    
    virtual bool isLoggedIn() const override
    {
        if ([FBSDKAccessToken currentAccessToken])
            return true;
        return false;
    }
    
    virtual std::string getUserID() const override
    {
        if (isLoggedIn())
        {
            NSString *userString = [FBSDKAccessToken currentAccessToken].userID;
            return [userString UTF8String];
        }
        
        return "";
    }
    
    virtual std::string getAccessToken() const override
    {
        if (isLoggedIn())
        {
            NSString *tokenString = [FBSDKAccessToken currentAccessToken].tokenString;
            return [tokenString UTF8String];
        }
        
        return "";
    }
    
    virtual std::string getAppId() const override
    {
        NSString *appId = [[NSBundle mainBundle].infoDictionary objectForKey:@"FacebookAppID"];
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
            std::string fields = "id,first_name,picture,last_name,middle_name,email,gender,birthday";
            for (const auto &it : additionalFields)
            {
                fields += "," + it;
            }
            
            [[[FBSDKGraphRequest alloc] initWithGraphPath:@"me" parameters:@{@"fields": [NSString stringWithUTF8String:fields.c_str()]}]
             startWithCompletionHandler:^(FBSDKGraphRequestConnection *connection, id result, NSError *error)
            {
                 if (!error)
                 {
                     SocialProfile profile;
                     
                     if(_debug)
                        NSLog(@"FacebookSocialPlugin::getMyProfile result: %@", result);
                     
                     for (id key in result)
                     {
                         id object = [result objectForKey:key];
                         
                         if ([key isEqualToString:@"id"])
                         {
                             profile.uid = [object UTF8String];
                         }
                         else if ([key isEqualToString:@"first_name"])
                         {
                             profile.firstName = [object UTF8String];
                         }
                         else if ([key isEqualToString:@"last_name"])
                         {
                             profile.lastName = [object UTF8String];
                         }
                         else if ([key isEqualToString:@"middle_name"])
                         {
                             profile.middleName = [object UTF8String];
                         }
                         else if ([key isEqualToString:@"email"])
                         {
                             profile.email = [object UTF8String];
                         }
                         else if ([key isEqualToString:@"gender"])
                         {
                             profile.gender = _genderHelper.toGender([object UTF8String]);
                         }
                         else if ([key isEqualToString:@"picture"])
                         {
                             id data = [object objectForKey:@"data"];
                             id url = [data objectForKey:@"url"];
                             profile.pictureUrl = preferedPictureSize == 0 ? [url UTF8String] : "";
                             profile.pictureId = [url UTF8String];
                         }
                         else if ([key isEqualToString:@"birthday"])
                         {
                             std::vector<std::string> parts = split([object UTF8String], '/');
                             if(parts.size() == 1)
                                 profile.birthDate = SocialProfile::BirthDate(0, 0, std::stoi(parts[0]));
                             if(parts.size() == 2)
                                 profile.birthDate = SocialProfile::BirthDate(std::stoi(parts[1]), std::stoi(parts[0]), 0);
                             else if(parts.size() == 3)
                                 profile.birthDate = SocialProfile::BirthDate(std::stoi(parts[1]), std::stoi(parts[0]), std::stoi(parts[2]));
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
                                 NSLog(@"Wrong type for object %@ by key %@", object, key);
                             }
                         }
                     }
                     
                     if (preferedPictureSize != 0)
                     {
                         const std::string &size = std::to_string(preferedPictureSize);
                         profile.pictureUrl = "https://graph.facebook.com/" + profile.uid + "/picture?width=" + size + "&height=" + size;
                     }
                     
                     if (_delegate)
                         _delegate->onGetMyProfile({SocialPluginDelegate::Error::Type::SUCCESS, 0, ""}, userData, profile);
                 }
                 else
                 {
                     if(_debug)
                         NSLog(@"FacebookSocialPlugin::getMyProfile error: %@", error);
                     
                     if (_delegate)
                         _delegate->onGetMyProfile({SocialPluginDelegate::Error::Type::UNDEFINED, static_cast<int>(error.code), [error.description UTF8String]}, userData, _emptyProfile);
                 }
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
    
private:
    void proccessLogin(FBSDKLoginManagerLoginResult *result, NSError *error, bool isPublish)
    {
        if (isPublish)
            _publishPermissions.clear();
        else
            _readPermissions.clear();
            
        std::vector<SocialPermission> declinedPermissions;
        std::vector<SocialPermission> grantedPermissions;
        
        for (id permision in result.grantedPermissions)
        {
            if (isPublish)
                _publishPermissions.emplace_back(_facebookPermissionsHelper.fromString([permision UTF8String]));
            else
                _readPermissions.emplace_back(_facebookPermissionsHelper.fromString([permision UTF8String]));
        }
        
        grantedPermissions.assign(_publishPermissions.begin(), _publishPermissions.end());
        grantedPermissions.insert(grantedPermissions.end(), _readPermissions.begin(), _readPermissions.end());

        for (id permision in result.declinedPermissions)
        {
            declinedPermissions.emplace_back(_facebookPermissionsHelper.fromString([permision UTF8String]));
        }

        const std::string &token = result.token ? [result.token.tokenString UTF8String] : "";
        
        if(_debug)
        {
            if(error)
            {
                NSLog(@"FacebookSocialPlugin::login error: %@", error);
            }
            else
            {
                if (result.isCancelled)
                {
                    NSLog(@"FacebookSocialPlugin::login canceled");
                }
                else
                {
                    NSLog(@"FacebookSocialPlugin::login succes granted permissions:%@ declined permissions:%@", result.grantedPermissions, result.declinedPermissions);
                }
            }
        }
        
        if(_delegate)
        {
            if (error)
            {
                SocialPluginDelegate::Error socialError = {SocialPluginDelegate::Error::Type::UNDEFINED, static_cast<int>(error.code), [error.description UTF8String]};
                _delegate->onLogin(socialError, token, grantedPermissions, declinedPermissions);
            }
            else
            {
                if (result.isCancelled)
                {
                    SocialPluginDelegate::Error socialError = {SocialPluginDelegate::Error::Type::USER_CANCEL, 0, ""};
                    _delegate->onLogin(socialError, token, grantedPermissions, declinedPermissions);
                }
                else
                {
                    SocialPluginDelegate::Error socialError = {SocialPluginDelegate::Error::Type::SUCCESS, 0, ""};
                    _delegate->onLogin(socialError, token, grantedPermissions, declinedPermissions);
                }
            }
        }
    }
    
private:
    
    FBSDKLoginManager *_loginManager = nil;
    SocialPluginDelegate *_delegate = nullptr;
    
    std::vector<SocialPermission> _readPermissions;
    std::vector<SocialPermission> _publishPermissions;
    
    SocialProfile _emptyProfile;
    GenderHelper<std::string> _genderHelper;
    
    bool _debug = false;

    SocialPermissionsHelper<FacebookPermission> _facebookPermissionsHelper;
};


FacebookSocialPlugin &FacebookSocialPlugin::getInstance()
{
    static FacebookSocialPluginIOS facebook;
    return facebook;
}
}
