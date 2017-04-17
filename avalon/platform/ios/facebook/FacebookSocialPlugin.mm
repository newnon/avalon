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

class FacebookSocialPluginIOS : public FacebookSocialPlugin
{
    enum class FacebookPermissionType
    {
        NONE = 0,
    };

public:
    FacebookSocialPluginIOS()
    : _facebookPermissionsHelper({{SocialPermission::Type::PUBLIC_PROFILE, "public_profile"}, {SocialPermission::Type::EMAIL, "email"}, {SocialPermission::Type::FRIENDS, "user_friends"}})
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
        if (!isLoggedIn())
        {
            [_loginManager logOut];
            _loginManager = nil;
            
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
    
    virtual std::vector<SocialPermission> getReadPermissionList() const override
    {
        return _readPermissions;
    }

    virtual std::vector<SocialPermission> getPublishPermissionList() const override
    {
        return _publishPermissions;
    }
    
    virtual void getMyProfile(int preferedPictureSize, const std::vector<std::string> &additionalFields) override
    {
        if (isLoggedIn())
        {
            std::string fields = "id,first_name,picture,last_name,middle_name,email,gender,birthday,age_range,installed";
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
                     profile.birthDay = std::numeric_limits<long long>::min();
                     
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
                             profile.gender = _genderHelper.fromString([object UTF8String]);
                         }
                         else if ([key isEqualToString:@"picture"])
                         {
                             profile.pictureUrl = preferedPictureSize == 0 ? [object UTF8String] : "";
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
                         _delegate->onGetMyProfile({SocialPluginDelegate::Error::Type::SUCCESS, 0, ""}, profile);
                 }
                 else
                 {
                     if (_delegate)
                         _delegate->onGetMyProfile({SocialPluginDelegate::Error::Type::UNDEFINED, error.code, [error.description UTF8String]}, _emptyProfile);
                 }
             }];
        }
        else
        {
            if (_delegate)
                _delegate->onGetMyProfile({SocialPluginDelegate::Error::Type::NO_LOGIN, 0, ""}, _emptyProfile);
        }
    }
    
    virtual void getProfiles(const std::vector<std::string> &userIds, int preferedPictureSize, const std::vector<std::string> &additionalFields) override
    {
    }
    
    virtual void getAppFriends(int preferedPictureSize, const std::vector<std::string> &additionalFields) override
    {
    }
    
private:
    void proccessLogin(FBSDKLoginManagerLoginResult *result, NSError *error, bool isPublish)
    {
        if (_delegate)
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
                {
                    _publishPermissions.emplace_back(_facebookPermissionsHelper.fromString([permision UTF8String]));
                }
                else
                {
                    _readPermissions.emplace_back(_facebookPermissionsHelper.fromString([permision UTF8String]));
                }
            }
            
            grantedPermissions.assign(_publishPermissions.begin(), _publishPermissions.end());
            grantedPermissions.insert(grantedPermissions.end(), _readPermissions.begin(), _readPermissions.end());

            for (id permision in result.declinedPermissions)
            {
                declinedPermissions.emplace_back(_facebookPermissionsHelper.fromString([permision UTF8String]));
            }

            const std::string &token = result.token ? [result.token.tokenString UTF8String] : "";
            
            if (error)
            {
                SocialPluginDelegate::Error socialError = {SocialPluginDelegate::Error::Type::UNDEFINED, error.code, [error.description UTF8String]};
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
    using FacebookPermission = PlatformSocialPermission<FacebookPermissionType>;
    
    FBSDKLoginManager *_loginManager = nil;
    SocialPluginDelegate *_delegate = nullptr;
    
    std::vector<SocialPermission> _readPermissions;
    std::vector<SocialPermission> _publishPermissions;
    
    SocialProfile _emptyProfile;
    GenderHelper _genderHelper;

    SocialPermissionsHelper<FacebookPermission> _facebookPermissionsHelper;
};


FacebookSocialPlugin &FacebookSocialPlugin::getInstance()
{
    static FacebookSocialPluginIOS facebook;
    return facebook;
}
